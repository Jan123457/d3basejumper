
#include "headers.h"
#include "jumper.h"
#include "imath.h"

/**
 * jump animation
 */

static engine::AnimSequence runningJumpSequence = 
{ 
    FRAMETIME(1358), 
    FRAMETIME(1380),  
    engine::ltNone, 
    0.0f 
};

/**
 * class implementation
 */

Jumper::RunningJump::RunningJump(Jumper* jumper, NxActor* actor, MatrixConversion* mc, float vel) :
    JumperAction( jumper )
{
    // set action properties
    _actionTime = 0.0f;
    _blendTime = 0.1f;
    _endOfAction = false;
    _phActor = actor;
    _matrixConversion = mc;
    _vel = vel;

    engine::IAnimationController* animCtrl = _clump->getAnimationController();

    // capture blend source
    animCtrl->captureBlendSrc();

    // reset animation mixer
    for( unsigned int i=0; i<engine::maxAnimationTracks; i++ )
    {
        if( animCtrl->getTrackAnimation( i ) ) animCtrl->setTrackActivity( i, false );
    }

    // setup animation cycle
    animCtrl->setTrackAnimation( 0, &runningJumpSequence );
    animCtrl->setTrackActivity( 0, true );
    animCtrl->setTrackSpeed( 0, 0.75f );
    animCtrl->setTrackWeight( 0, 1.0f );
    animCtrl->resetTrackTime( 0 );
    animCtrl->advance( 0.0f );

    // capture blend destination
    animCtrl->captureBlendDst();
    animCtrl->blend( 0.0f );
}

/**
 * Action
 */

void Jumper::RunningJump::update(float dt)
{
    updateAnimation( dt );
    _clump->getFrame()->getLTM();

    if( _actionTime < _blendTime ) 
    {
        Vector3f dir = _clump->getFrame()->getAt();
        dir.normalize();
        _clump->getFrame()->setPos( _clump->getFrame()->getPos() + dir * dt * _vel );            
        return;
    }

    if( _phActor->isSleeping() )
    {
        // setup physics
        Matrix4f sampleLTM = Jumper::getCollisionFF( _clump )->getFrame()->getLTM();
        _phActor->setGlobalPose( wrap( sampleLTM ) );
        _phActor->wakeUp();
        NxVec3 velH = wrap( _clump->getFrame()->getAt() );
        velH.normalize();
        velH *= _vel * 0.01f;
        NxVec3 velV = wrap( _clump->getFrame()->getUp() );
        velV.normalize();
        velV *= 1.5f;
        _phActor->setLinearVelocity( velH + velV );
        _jumper->initOverburdenCalculator( velH + velV );
    }
    else
    {
        _clump->getFrame()->setMatrix( _matrixConversion->convert( wrap( _phActor->getGlobalPose() ) ) );
    }

    if( _clump->getAnimationController()->isEndOfAnimation( 0 ) )
    {
        _endOfAction = true;
    }
}