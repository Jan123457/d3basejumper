
#include "headers.h"
#include "pilotchute.h"
#include "jumper.h"
#include "gameplay.h"
#include "../common/istring.h"
#include "version.h"

/**
 * related animations
 */

static engine::AnimSequence openingSequence = 
{
    FRAMETIME(1), FRAMETIME(56), engine::ltNone, 0.0f 
};

/**
 * render callback
 */

engine::IAtomic* PilotchuteRenderCallback::onRenderPilotchute(engine::IAtomic* atomic, void* data)
{
    assert( reinterpret_cast<PilotchuteRenderCallback*>(data)->canopyTexture );
    assert( atomic->getGeometry()->getNumShaders() > 0 );
    assert( atomic->getGeometry()->getShader(0)->getNumLayers() > 0 );

    // replace first texture on first shader
    atomic->getGeometry()->getShader(0)->setLayerTexture( 
        0, reinterpret_cast<CanopyRenderCallback*>(data)->canopyTexture
    );
    return atomic;
}

void PilotchuteRenderCallback::restoreAtomic(engine::IAtomic* atomic)
{
    assert( atomic->getGeometry()->getNumShaders() > 0 );
    assert( atomic->getGeometry()->getShader(0)->getNumLayers() > 0 );

    engine::ITexture* texture = Gameplay::iEngine->getTexture( "white" );
    assert( texture );
    atomic->getGeometry()->getShader(0)->setLayerTexture( 0, texture );
}

PilotchuteRenderCallback::~PilotchuteRenderCallback()
{
    if( canopyTexture ) canopyTexture->release();
}

void PilotchuteRenderCallback::setTexture(const char* textureName, const char* resourceName)
{
    if( canopyTexture )
    {
        canopyTexture->release();
        canopyTexture = NULL;
    }
    canopyTexture = Gameplay::iEngine->getTexture( textureName );
    if( canopyTexture == NULL )
    {
        canopyTexture = Gameplay::iEngine->createTexture( resourceName );
        assert( canopyTexture );
        canopyTexture->setMinFilter( engine::ftAnisotropic );
        canopyTexture->setMagFilter( engine::ftLinear );
        canopyTexture->setMipFilter( engine::ftLinear );
    }
    // +1 to mark textures as used by this object
    canopyTexture->addReference();
}

void PilotchuteRenderCallback::setTexture(database::Canopy* gearRecord)
{
    database::GearTexture* textureInfo = database::GearTexture::getRecord( gearRecord->textureId );
    setTexture( textureInfo->textureName, textureInfo->resourceName );
}

void PilotchuteRenderCallback::setTexture(Gear* gear)
{
    setTexture( database::Canopy::getRecord( gear->id ) );
}

void PilotchuteRenderCallback::apply(engine::IClump* clump)
{
    callback::AtomicL atomics;
    clump->forAllAtomics( callback::enumerateAtomics, &atomics );
    for( callback::AtomicI atomicI = atomics.begin(); atomicI != atomics.end(); atomicI++ )
    {
        if( PilotchuteSimulator::getCollision( clump ) != (*atomicI) )
        {
            (*atomicI)->setRenderCallback( onRenderPilotchute, this );
        }
    }
}

void PilotchuteRenderCallback::restore(engine::IClump* clump)
{
    callback::AtomicL atomics;
    clump->forAllAtomics( callback::enumerateAtomics, &atomics );
    for( callback::AtomicI atomicI = atomics.begin(); atomicI != atomics.end(); atomicI++ )
    {
        if( PilotchuteSimulator::getCollision( clump ) != (*atomicI) )
        {
            restoreAtomic( (*atomicI) );
        }
    }
}

/**
 * model management
 */

engine::IAtomic* PilotchuteSimulator::getCollision(engine::IClump* clump)
{
    engine::IFrame* collisionFrame = Gameplay::iEngine->findFrame( clump->getFrame(), "Collision_child0" );
    assert( collisionFrame );
    return Gameplay::iEngine->getAtomic( clump, collisionFrame );
}

/**
 * class implementation
 */

static engine::IAtomic* setPilotchuteUpdateTresholdCB(engine::IAtomic* atomic, void* data)
{
    atomic->setUpdateTreshold( 0.0f, 0.0f );
    return atomic;
}

PilotchuteSimulator::PilotchuteSimulator(Actor* jumper, database::Canopy* canopyInfo, database::Pilotchute* gearRecord) : 
    Actor( jumper )
{   
    _name = "PilotchuteSimulator";
    _canopyInfo = canopyInfo;
    _gearRecord = gearRecord;
    _cordLength = 3.0f;
    _state = pcsStowed;
    _phConnected = NULL;
    _phConnectedFrame = NULL;
    _pullFrame = NULL;
    _phPilotchute = NULL;
    _phJoint = NULL;

    _poseModeFrame = Gameplay::iEngine->createFrame( "PilotchutePoseFrame" ); assert( _poseModeFrame );

    // create 3d-models
    engine::IClump* pilotTemplate = Gameplay::iGameplay->findClump( "Pilot02" ); assert( pilotTemplate );
    engine::IClump* cordTemplate  = Gameplay::iGameplay->findClump( "Cord" ); assert( cordTemplate );
    _pilotClump = pilotTemplate->clone( "Pilotchute" );
    _cordClump  = cordTemplate->clone( "Cord" );
    getCollision( _pilotClump )->setFlags( 0 );

    _pilotClump->forAllAtomics( setPilotchuteUpdateTresholdCB, NULL );
    _cordClump->forAllAtomics( setPilotchuteUpdateTresholdCB, NULL );

    // scale 3d model by gear scale ratio    
    Matrix4f m = _pilotClump->getFrame()->getMatrix();
    Vector3f scale(
        _gearRecord->scale * 0.65f,
        _gearRecord->scale * 0.65f,
        _gearRecord->scale * 0.65f
    );
    scaleMatrix( m, scale );
    _pilotClump->getFrame()->setMatrix( m );

    // calculate pilot local anchor
    Matrix4f childLTM  = m; orthoNormalize( childLTM );
    Matrix4f parentLTM = PilotchuteSimulator::getCollision( _pilotClump )->getFrame()->getLTM(); orthoNormalize( parentLTM );
    MatrixConversion mc;
    mc.setup( parentLTM, childLTM );
    Matrix4f childM = mc.getTransformation();
    _phPilotAnchor = wrap( Vector3f( childM[3][0], childM[3][1], childM[3][2] ) );

    // setup rendering
    _renderCallback = new PilotchuteRenderCallback;
    _renderCallback->setTexture( _canopyInfo );
    #ifdef GAMEPLAY_EDITION_ATARI
        Jumper* player = dynamic_cast<Jumper*>( jumper );
        if( player->isPlayer() && player->getScene()->getCareer()->getLicensedFlag() )
        {
            // force extra white texture
            _renderCallback->setTexture( "P_01_27_ExtraW", "./res/x/textures/P_01_27_ExtraW.dds" );
        }
    #endif
    _renderCallback->apply( _pilotClump );

    // setup animation
    _pilotClump->getAnimationController()->setTrackAnimation( 0, &openingSequence );
    _pilotClump->getAnimationController()->setTrackSpeed( 0, 1.0f );
    _pilotClump->getAnimationController()->setTrackWeight( 0, 1.0f );
    _pilotClump->getAnimationController()->setTrackActivity( 0, true );
    _pilotClump->getAnimationController()->advance( 0.0f );
}

PilotchuteSimulator::~PilotchuteSimulator()
{
    if( _poseModeFrame ) _poseModeFrame->release();
    if( _phJoint ) _scene->getPhScene()->releaseJoint( *_phJoint );
    if( _phPilotchute ) _scene->getPhScene()->releaseActor( *_phPilotchute );
    // delete 3d models
    assert( _scene );
    if( _state != pcsStowed )
    {
        _scene->getStage()->remove( _pilotClump );
        _scene->getStage()->remove( _cordClump );
    }
    _renderCallback->restore( _pilotClump );
    delete _renderCallback;
    _pilotClump->release();
    _cordClump->release();
}

void PilotchuteSimulator::onUpdateActivity(float dt)
{
    if( isPulled() && !isDropped() )
    {
        Matrix4f m = _pullFrame->getLTM();
        orthoNormalize( m );

        Matrix4f maxCorrector = Gameplay::iEngine->rotateMatrix(
            Matrix4f( 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 ),
            Vector3f( 1,0,0 ),
            -90.0f
        );
        m = Gameplay::iEngine->transformMatrix( maxCorrector, m );

        Matrix4f pilotM = _pilotClump->getFrame()->getMatrix();
        Vector3f pilotS = calcScale( pilotM );
        scaleMatrix( m, pilotS );
        _pilotClump->getFrame()->setMatrix( m );
        Jumper::placeCord( _cordClump, _phConnectedFrame->getPos(), _pilotClump->getFrame()->getPos() );
    }
    else if( isDropped() )
    {        
        _pilotClump->getFrame()->setMatrix( _mcPilotchute.convert( wrap( _phPilotchute->getGlobalPose() ) ) );
        _pilotClump->getFrame()->getLTM();
        Jumper::placeCord( _cordClump, _phConnectedFrame->getPos(), _pilotClump->getFrame()->getPos() );
    }
}

void PilotchuteSimulator::onUpdatePhysics(void)
{
    if( !isDropped() ) return;

    // velocity of pilotchute
    NxVec3 velocity = _phPilotchute->getLinearVelocity();

    // wind velocity
    float windFactor = 0.5f;
    NxVec3 wind = _scene->getWindAtPoint( _phPilotchute->getGlobalPose().t );
    velocity += wind * windFactor;

    // velocity interpolation coefficient
    float Iv = velocity.magnitude() / _gearRecord->Vrec;

    // local coordinate system of pilotchute
    NxMat34 pose = _phPilotchute->getGlobalPose();
    NxVec3 x = pose.M.getColumn(0);
    NxVec3 y = pose.M.getColumn(1);
    NxVec3 z = pose.M.getColumn(2);

    // recommended speed and woking effectivity
    float effectivity = velocity.magnitude() / _gearRecord->Vrec;

    // additional regulation multiplier
    float rmul = 1.65f;
   
    // animate using effectivity coefficient
    _pilotClump->getAnimationController()->advance( ::simulationStepTime * effectivity * rmul );

    // air resistance force
    NxVec3 Fair = velocity * Iv * -_gearRecord->Fair;

    // finalize motion equation
    _phPilotchute->addForce( Fair );
}

void PilotchuteSimulator::connect(NxActor* actor, engine::IFrame* frame, const NxVec3& localAnchor)
{
    assert( actor ); assert( frame );
    _phConnected = actor;
    _phConnectedFrame = frame;
    _phLocalAnchor = localAnchor;

    // re-initialize joint
    if( _phJoint )
    {        
        _scene->getPhScene()->releaseJoint( *_phJoint );
        NxDistanceJointDesc jointDesc;
        jointDesc.actor[0] = _phConnected;
        jointDesc.actor[1] = _phPilotchute;
        jointDesc.maxDistance = _cordLength;
        jointDesc.minDistance = 0.0f;
        jointDesc.flags = NX_DJF_MAX_DISTANCE_ENABLED;        
        jointDesc.localAnchor[0] = _phLocalAnchor;
        jointDesc.localAnchor[1] = _phPilotAnchor;
        jointDesc.jointFlags = NX_JF_VISUALIZATION;
        _phJoint = _scene->getPhScene()->createJoint( jointDesc ); 
        assert( _phJoint );
    }
}

void PilotchuteSimulator::pull(engine::IFrame* frame)
{
    assert( frame ); assert( _scene ); assert( isConnected() );
    _pullFrame = frame;
    _scene->getStage()->add( _cordClump );
    _scene->getStage()->add( _pilotClump );
}

void PilotchuteSimulator::pull(Matrix4f pose)
{
    assert( _scene ); assert( isConnected() );
    _poseModeFrame->setMatrix( pose );
    _pullFrame = _poseModeFrame;
    _scene->getStage()->add( _cordClump );
    _scene->getStage()->add( _pilotClump );
}

void PilotchuteSimulator::drop(const NxVec3& velocity)
{
    assert( isConnected() );
    assert( isPulled() );

    // initialize pilotchute physics simulator
    NxBodyDesc nxBodyDesc;
    nxBodyDesc.massSpaceInertia.set( 0,0,0 ); // tensor will be computed automatically
    nxBodyDesc.mass = _gearRecord->mass;
    nxBodyDesc.linearDamping = 0.0f;
    nxBodyDesc.angularDamping = 10.0f;
    nxBodyDesc.flags = NX_BF_VISUALIZATION;
    nxBodyDesc.solverIterationCount = 16;
    Vector3f aabbScale(
        getCollision( _pilotClump )->getFrame()->getRight().length(),
        getCollision( _pilotClump )->getFrame()->getUp().length(),
        getCollision( _pilotClump )->getFrame()->getAt().length()
    );
    Vector3f aabbInf = getCollision( _pilotClump )->getGeometry()->getAABBInf();
    Vector3f aabbSup = getCollision( _pilotClump )->getGeometry()->getAABBSup();
    Vector3f aabbDim = ( aabbSup - aabbInf );
    aabbDim[0] *= aabbScale[0] * 0.5f,
    aabbDim[1] *= aabbScale[1] * 0.5f,
    aabbDim[2] *= aabbScale[2] * 0.5f;
    NxBoxShapeDesc nxBoxDesc;
    nxBoxDesc.dimensions = wrap( aabbDim );
    nxBoxDesc.materialIndex = _scene->getPhClothMaterial()->getMaterialIndex();
    NxActorDesc nxActorDesc;
    nxActorDesc.userData = this;
    nxActorDesc.shapes.pushBack( &nxBoxDesc );
    nxActorDesc.body = &nxBodyDesc;
    Matrix4f collisionLTM = getCollision( _pilotClump )->getFrame()->getLTM();
    orthoNormalize( collisionLTM );
    nxActorDesc.globalPose = wrap( collisionLTM );
    _phPilotchute = _scene->getPhScene()->createActor( nxActorDesc );
    assert( _phPilotchute );

    // initialize velocity
    _phPilotchute->addForce( _phConnected->getLinearVelocity(), NX_VELOCITY_CHANGE );
    _phPilotchute->addForce( velocity, NX_VELOCITY_CHANGE );

    // initialize PTV transformation
    Matrix4f pilotchuteLTM = getCollision( _pilotClump )->getFrame()->getLTM();
    Matrix4f viewLTM = _pilotClump->getFrame()->getLTM();
    _mcPilotchute.setup( pilotchuteLTM, viewLTM );

    // initialize joint
    NxDistanceJointDesc jointDesc;
    jointDesc.actor[0] = _phConnected;
    jointDesc.actor[1] = _phPilotchute;    
    jointDesc.maxDistance = _cordLength;
    jointDesc.minDistance = 0.0f;
    jointDesc.flags = NX_DJF_MAX_DISTANCE_ENABLED;    
    jointDesc.localAnchor[0] = _phLocalAnchor;
    jointDesc.localAnchor[1] = _phPilotAnchor;
    jointDesc.jointFlags = NX_JF_VISUALIZATION;
    _phJoint = _scene->getPhScene()->createJoint( jointDesc ); 
    assert( _phJoint );
}

void PilotchuteSimulator::setInflation(float value)
{
    value = value < 0 ? 0 : ( value > 1 ? 1 : value );

    _pilotClump->getAnimationController()->resetTrackTime( 0 );
    _pilotClump->getAnimationController()->advance( value * ( openingSequence.endTime - openingSequence.startTime ) );
}