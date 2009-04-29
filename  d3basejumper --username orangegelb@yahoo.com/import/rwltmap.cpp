
#include "headers.h"
#include "import.h"
#include "iuserdata.h"
#include "../common/istring.h"

/**
 * renderware asset incapsulation
 */

class LightmapAsset
{
public:
    // container for RpClump objects
    typedef std::list<RpClump*> RpClumps;
    typedef RpClumps::iterator RpClumpI;
    // container for named group of RpClump objects
    typedef std::pair<std::string,RpClumps> RpClumpGroup;
    typedef std::list<RpClumpGroup> RpClumpGroups;
    typedef RpClumpGroups::iterator RpClumpGroupI;
private:
    std::string      _resourceName;
    RwTexDictionary* _textures;
    std::string      _worldGroupName;
    RpWorld*         _world;    
    RpClumpGroups    _clumpGroups;
public:
    LightmapAsset(const char* resourceName)
    {
        RwChunkHeaderInfo chunkHeaderInfo;
        RwChunkGroup* chunkGroup = NULL;
        RpClumpGroup* currentClumpGroup = NULL;
        RpClump* clump = NULL;

        // initialize asset
        _resourceName = resourceName;
        _textures     = NULL;
        _world        = NULL;

        // open stream
        RwStream* stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, resourceName );  
        if( !stream ) throw Exception( "Input::calculateLightMaps() : can't open resource \"%s\"", resourceName );

        // process stream chunkz        
        while( RwStreamReadChunkHeaderInfo( stream, &chunkHeaderInfo ) )
        {
            switch( chunkHeaderInfo.type )
            {
            case rwID_PITEXDICTIONARY:
                _textures = RtPITexDictionaryStreamRead( stream );
                RwTexDictionarySetCurrent( _textures );
                break;
            case rwID_CHUNKGROUPSTART:
		        if( chunkGroup == NULL ) 
                {
                    chunkGroup = RwChunkGroupBeginStreamRead( stream );
                    currentClumpGroup = new RpClumpGroup;
                    currentClumpGroup->first = chunkGroup->name;
                }
                break;
            case rwID_CHUNKGROUPEND:
                if( chunkGroup != NULL )
		        {
                    if( currentClumpGroup->second.size() )
                    {
                        _clumpGroups.push_back( *currentClumpGroup );
                    }
                    delete currentClumpGroup;
                    currentClumpGroup = NULL;
			        RwChunkGroupEndStreamRead( chunkGroup, stream );
			        RwChunkGroupDestroy( chunkGroup );
			        chunkGroup = NULL;                    
		        }
                break;
            case rwID_CLUMP:
                assert( _textures );
                assert( chunkGroup );
                assert( currentClumpGroup );
                clump = RpClumpStreamRead( stream );
                currentClumpGroup->second.push_back( clump );
                break;
            case rwID_WORLD:
                assert( chunkGroup );
                _worldGroupName = chunkGroup->name;
                _world = RpWorldStreamRead( stream );
                break;
            default:
                RwStreamSkip( stream, chunkHeaderInfo.length );
                break;
            }            
        }
        RwStreamClose( stream, NULL );

        // add clumps to world
        if( _world ) 
        {
            for( RpClumpGroupI clumpGroupI = _clumpGroups.begin();
                               clumpGroupI != _clumpGroups.end();
                               clumpGroupI++ )
            {
                for( RpClumpI clumpI = clumpGroupI->second.begin();
                              clumpI != clumpGroupI->second.end();
                              clumpI++ )
                {
                    RpWorldAddClump( _world, *(clumpI) );
                }
            }
        }
    }
    virtual ~LightmapAsset()
    {
        // remove clumps from world and destroy clumps        
        for( RpClumpGroupI clumpGroupI = _clumpGroups.begin();
                           clumpGroupI != _clumpGroups.end();
                           clumpGroupI++ )
        {
            for( RpClumpI clumpI = clumpGroupI->second.begin();
                          clumpI != clumpGroupI->second.end();
                          clumpI++ )
            {
                if( _world ) RpWorldRemoveClump( _world, *(clumpI) );
                RpClumpDestroy( *(clumpI) );
            }
        }

        // destroy world
        if( _world ) RpWorldDestroy( _world );

        // destroy textures
        RwTexDictionaryDestroy( _textures );
    }
public:
    void save(const char* resourceName)
    {
        // open stream
        RwStream* stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, resourceName );  
        if( !stream ) throw Exception( "Input::calculateLightMaps() : can't open resource \"%s\"", resourceName );

        // write texture dictionary
        RtPITexDictionaryStreamWrite( _textures, stream );

        // write world
        if( _world )
        {
            RwChunkGroup* chunkGroup = RwChunkGroupCreate();
            RwChunkGroupSetName( chunkGroup, _worldGroupName.c_str() );
            RwChunkGroupBeginStreamWrite( chunkGroup, stream );
            RpWorldStreamWrite( _world, stream );
            RwChunkGroupEndStreamWrite( chunkGroup, stream );
            RwChunkGroupDestroy( chunkGroup );
        }

        // write clumps
        for( RpClumpGroupI clumpGroupI = _clumpGroups.begin();
                           clumpGroupI != _clumpGroups.end();
                           clumpGroupI++ )
        {
            RwChunkGroup* chunkGroup = RwChunkGroupCreate();
            RwChunkGroupSetName( chunkGroup, clumpGroupI->first.c_str() );
            RwChunkGroupBeginStreamWrite( chunkGroup, stream );

            for( RpClumpI clumpI = clumpGroupI->second.begin();
                          clumpI != clumpGroupI->second.end();
                          clumpI++ )
            {
                RpClumpStreamWrite( *(clumpI), stream );
            }

            RwChunkGroupEndStreamWrite( chunkGroup, stream );
            RwChunkGroupDestroy( chunkGroup );
        }

        RwStreamClose( stream, NULL );
    }
private:
    static RpWorldSector* sectorCanBeLightmapped(RpWorldSector *worldSector, void *data)
    {
        bool* flag = reinterpret_cast<bool*>( data );
        if( worldSector->texCoords[1] != NULL ) *flag = true;
        return NULL;
    }
public:
    bool canBeLightmapped(void)
    {
        if( !_world ) return false;

        bool flag = false;
        RpWorldForAllWorldSectors( _world, sectorCanBeLightmapped, &flag );
        return flag;
    }
private:
    static RpMaterial* materialSetupLightmaps(RpMaterial *material, void *data)
    {
        if( !( RtLtMapMaterialGetFlags( material ) & rtLTMAPMATERIALAREALIGHT ) )
        {
            RtLtMapMaterialSetFlags( 
                material, 
                RtLtMapMaterialGetFlags( material ) | rtLTMAPMATERIALLIGHTMAP
            );
        }
        return material;
    }
    static RpWorldSector* sectorSetupLightmaps(RpWorldSector *worldSector, void *data)
    {
        // destroy lightmap
        RwTexture* lightmap = RpLtMapWorldSectorGetLightMap( worldSector );
        if( lightmap ) RwTexDictionaryRemoveTexture( lightmap );
        RtLtMapWorldSectorLightMapDestroy( worldSector );

        // setup flags
        RtLtMapWorldSectorSetFlags( worldSector, rtLTMAPOBJECTLIGHTMAP );

        return worldSector;
    }
    static RpAtomic* atomicSetupLightmaps(RpAtomic* atomic, void *data)
    {
        // destroy lightmap
        RwTexture* lightmap = RpLtMapAtomicGetLightMap( atomic );
        if( lightmap ) RwTexDictionaryRemoveTexture( lightmap );
        RtLtMapAtomicLightMapDestroy( atomic );

        // setup flags        
        RtLtMapAtomicSetFlags( 
            atomic, 
            RtLtMapAtomicGetFlags( atomic ) | rtLTMAPOBJECTLIGHTMAP 
        );

        // retrieve geomtery
        RpGeometry* geometry = RpAtomicGetGeometry( atomic );
        RpGeometryForAllMaterials( geometry, materialSetupLightmaps, data );

        return atomic;
    }
    static RpClump* clumpSetupLightmaps(RpClump* clump, void *data)
    {
        RpClumpForAllAtomics( clump, atomicSetupLightmaps, data );
        return clump;
    }
public:
    void setupLightmaps(void)
    {
        if( !_world ) return;
        RpWorldForAllWorldSectors( _world, sectorSetupLightmaps, NULL );
        RpWorldForAllClumps( _world, clumpSetupLightmaps, NULL );
        RpWorldForAllMaterials( _world, materialSetupLightmaps, NULL );
    }
public:
    inline const char* getResourceName(void) { return _resourceName.c_str(); }
    inline RpWorld* getWorld(void) { return _world; }
};

/**
 * progress callback support
 */

void*                            Import::cipUserData = NULL;
import::LightMapProgressCallback Import::cipCallback = NULL;

RwBool Import::callbackIlluminateProgress(RwInt32 message, RwReal value)
{
    if( cipCallback ) cipCallback( value * 0.01f, cipUserData );
    return true;
}

/**
 * various callbacks
 */

RpMaterial* Import::callbackSetLightmapDensityModifier(RpMaterial *material, void *data)
{
    float worldDensity = *( reinterpret_cast<float*>( data ) );
    float materialDensity = RtLtMapMaterialGetLightMapDensityModifier( material );

    RtLtMapMaterialSetLightMapDensityModifier( 
        material, materialDensity * worldDensity
    );

    return material;
}

/**
 * lightmap options
 */

static RwInt32  lightmapSize         = 128;
static RwReal   lightmapDensity      = 2.0f;
static RwUInt32 lightmapSuperSample  = 1;

static RwReal arealtDensity    = 1.0f;
static RwReal arealtDensityMod = 0.01f;
static RwReal arealtRadiusMod  = 1.0f;
static RwReal arealtCutoff     = 1.0f;

void Import::calculateLightMaps(const char* resourceName, import::LightMapProgressCallback callback, void* userData )
{
    cipUserData = userData;
    cipCallback = callback;

    // start progress
    if( cipCallback ) cipCallback( 0.0f, cipUserData );

    // create asset object
    LightmapAsset asset( resourceName );

    // check if world have a second UV-set
    if( asset.getWorld() && asset.canBeLightmapped() )
    {
        // calculate appropriate lightmap density for specified scene
        float worldDensity = RtLtMapWorldCalculateDensity( asset.getWorld() );

        // setup lightmapping options for asset
        asset.setupLightmaps();

        // setup density modifier for all materials
        RpWorldForAllMaterials( asset.getWorld(), callbackSetLightmapDensityModifier, &worldDensity );

        // setup lightmap size
        RtLtMapLightMapSetDefaultSize( lightmapSize );

        // prepare lightmapping session
        RtLtMapLightingSession session;
        RtLtMapLightingSessionInitialize( &session, asset.getWorld() );
        session.progressCallBack = callbackIlluminateProgress;

        // create lightmaps, calculate uvs
        RtLtMapLightMapsCreate( &session, lightmapDensity, NULL );

        // setup area lights
        RtLtMapSetAreaLightDensityModifier( arealtDensityMod );
        RtLtMapSetAreaLightRadiusModifier( arealtRadiusMod );
        RtLtMapSetAreaLightErrorCutoff( arealtCutoff );
        RtLtMapAreaLightGroup* areaLights = RtLtMapAreaLightGroupCreate( &session, arealtDensity );

        // illuminate lightmaps
        RtLtMapIlluminate( &session, areaLights, lightmapSuperSample );

        // close lightmapping session
        RtLtMapAreaLightGroupDestroy( areaLights );
        RtLtMapLightingSessionDeInitialize( &session );

        // save asset
        std::string newResourceName = expath( resourceName ) + 
                                      exname( resourceName ) +
                                      "_ltmp.rws";
        asset.save( newResourceName.c_str() );
    }

    // end of progress
    if( cipCallback ) cipCallback( 1.0f, cipUserData );

    cipUserData = NULL;
    cipCallback = NULL;
}