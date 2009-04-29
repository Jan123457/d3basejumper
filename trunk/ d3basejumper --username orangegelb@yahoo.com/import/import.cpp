
#include "headers.h"
#include "import.h"
#include "rtfsyst.h"

/**
 * class implementation
 */
    
Import::Import()
{
}

Import::~Import()
{
    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();
}

EntityBase* Import::creator()
{
    return new Import;
}

void Import::entityDestroy()
{
    delete this;
}

/**
 * renderware utilites system
 */

static void* _generic_malloc(size_t size, RwUInt32 hint) 
{
    return (malloc(size));
}

static void _generic_free(void *orgMem) 
{
    free(orgMem);
}

static void* _generic_realloc(void *orgMem, size_t newSize, RwUInt32 hint) 
{
    return (realloc(orgMem, newSize));
}

static void* _generic_calloc(size_t numObj, size_t sizeObj, RwUInt32 hint) 
{
    return (calloc(numObj, sizeObj));
}

static RwMemoryFunctions genericFunctions = 
{
    _generic_malloc, 
    _generic_free,
    _generic_realloc,
    _generic_calloc
};

static void debugHandler(RwDebugType type, const RwChar * string) 
{
    OutputDebugString( string );
    OutputDebugString( "\n" );
}

static RwBool RpD3FSystInstallFileSystem(RwInt32 maxNBF)
{
    RwChar      curDirBuffer[_MAX_PATH];
    RwUInt32    retValue;
    RtFileSystem *wfs, *unc;

    RwUInt32 drivesMask;
    RwInt32 drive;
    RwChar  fsName[2];

    RtFSManagerOpen(RTFSMAN_UNLIMITED_NUM_FS);
    
    retValue = GetCurrentDirectory(_MAX_PATH, curDirBuffer);
    if (0 == retValue || retValue > _MAX_PATH)
    {
        return FALSE;
    }

    rwstrcat( curDirBuffer, "\\" );

    if ((unc = RtWinFSystemInit(maxNBF, curDirBuffer, "unc")) != NULL)
    {
        if (RtFSManagerRegister(unc) == FALSE)
        {
            return (FALSE);
        }
    }
    else
    {
        return (FALSE);
    }
       
    CharUpper(curDirBuffer);
    
    drivesMask = GetLogicalDrives();

    for( drive = 2, drivesMask >>= 1; drivesMask != 0; drive++, drivesMask >>= 1)
    {
        if (drivesMask & 0x01)
        {
            RwInt32 ret;
            RwChar  deviceName[4];

            deviceName[0] = drive + 'A' - 1;
            deviceName[1] = ':';
            deviceName[2] = '\\';   
            deviceName[3] = '\0';

            ret = GetDriveType(deviceName);

            if ( ret != DRIVE_UNKNOWN && ret != DRIVE_NO_ROOT_DIR )
            {
                deviceName[2] = '\0';

                fsName[0] = deviceName[0];
                fsName[1] = '\0';

                if ((wfs = RtWinFSystemInit(maxNBF, deviceName, fsName)) != NULL)
                {
                    if (RtFSManagerRegister(wfs) == FALSE)
                    {
                        return (FALSE);
                    }
                    else
                    {
                        if (curDirBuffer[1] != ':')
                        {
                            RtFSManagerSetDefaultFileSystem(unc);
                        }
                        else if (deviceName[0] == curDirBuffer[0])
                        {
                            RtFSManagerSetDefaultFileSystem(wfs);
                        }
                    }
                }
                else
                {
                    return (FALSE);
                }
            }
        }
    }

    return (TRUE);
}

void Import::entityInit(Object * p)
{
    // initialize renderware engine:
    RwInt32 initFlag = 0;
    RwInt32 resourceAreaSize = 8388608;
    if( !RwEngineInit( &genericFunctions, initFlag, resourceAreaSize ) )
    {
        throw Exception( "RwEngineInit failed" );
    }
    // setup debug handler
    RwDebugSetHandler( debugHandler );
    // install file system
    if( !RpD3FSystInstallFileSystem( 5 ) )
    {
        throw Exception( "RpIOInstallFileSystem failed" );
    }
    // attach pluginz
    if( !RpWorldPluginAttach() ||
        !RpAnisotPluginAttach() ||
        !RpMatFXPluginAttach() || 
        !RpHAnimPluginAttach() ||
        !RtAnimInitialize() || 
        !RpSkinPluginAttach() ||
        !RpCollisionPluginAttach() ||
        !RpPTankPluginAttach() ||
        !RpPrtStdPluginAttach() ||
        !RpPrtAdvPluginAttach() ||
        !RpUserDataPluginAttach() ||
        !RpUVAnimPluginAttach() ||
        !RpRandomPluginAttach() || 
        !RpPVSPluginAttach() ||
        !RpSplinePluginAttach() ||
        !RpNormMapPluginAttach() ||
        !RtCharsetOpen() ||
        !RpRandomPluginAttach() ||
        !RpLtMapPluginAttach() ||
        !RpMipmapKLPluginAttach() )
    {
        throw Exception( "Plugin attachment failed" );
    }
    // retrieve main window handle
    RwInt32 wnd = -1;
    mainwnd::IMainWnd* mainWnd;
    queryInterface( "MainWnd", &mainWnd );
    wnd = mainWnd->getHandle();
    if( wnd == -1 ) throw Exception( "engine: error: no main window found" );
    // open engine    
    RwEngineOpenParams openParams;
    openParams.displayID = (HWND)(wnd);
    if( !RwEngineOpen( &openParams ) )
    {
        throw Exception( "RwEngineOpen failed" );
    }
    // start engine
    if( !RwEngineStart() )
    {
        throw Exception( "RwEngineStart failed" );
    }
    // register image format
    if( !RwImageRegisterImageFormat( "bmp", RtBMPImageRead, RtBMPImageWrite ) ||
        !RwImageRegisterImageFormat( "png", RtPNGImageRead, RtPNGImageWrite ) ||
        !RwImageRegisterImageFormat( "tif", RtTIFFImageRead, NULL ) )
    {
        throw Exception( "Image format registering failed" );
    }
}

void Import::entityAct(float dt)
{
}

void Import::entityHandleEvent(evtid_t id, trigid_t trigId, Object* param)
{
}

IBase* Import::entityAskInterface(iid_t id)
{
    if( id == import::IImport::iid ) return this;
    return NULL;
}

/**
 * IImport
 */

void Import::release(import::ImportTexture* importData)
{
    if( importData->name ) delete[] importData->name;
    if( importData->pixels ) delete[] importData->pixels;
    delete importData;
}

void Import::release(import::ImportMaterial* importData)
{
    if( importData->name ) delete[] importData->name;
    delete importData;
}

void Import::release(import::ImportFrame* importData)
{
    if( importData->name ) delete[] importData->name;
    delete importData;
}

void Import::release(import::ImportGeometry* importData)
{
    if( importData->name ) delete[] importData->name;
    if( importData->vertices ) delete[] importData->vertices;
    if( importData->normals ) delete[] importData->normals;
    if( importData->uvs[0] ) delete[] importData->uvs[0];
    if( importData->uvs[1] ) delete[] importData->uvs[1];
    if( importData->uvs[2] ) delete[] importData->uvs[2];
    if( importData->uvs[3] ) delete[] importData->uvs[3];
    if( importData->uvs[4] ) delete[] importData->uvs[4];
    if( importData->uvs[5] ) delete[] importData->uvs[5];
    if( importData->uvs[6] ) delete[] importData->uvs[6];
    if( importData->uvs[7] ) delete[] importData->uvs[7];
    if( importData->prelights ) delete[] importData->prelights;
    if( importData->triangles ) delete[] importData->triangles;
    if( importData->materials ) delete[] importData->materials;	
}

void Import::release(import::ImportAtomic* importData)
{
    delete importData;
}

void Import::release(import::ImportClump* importData)
{
    if( importData->name ) delete[] importData->name;
    if( importData->atomics ) delete[] importData->atomics;
    if( importData->lights ) delete[] importData->lights;
    delete importData;
}

void Import::release(import::ImportWorldSector* importData)
{    
    if( importData->vertices ) delete[] importData->vertices;
    if( importData->normals ) delete[] importData->normals;
    if( importData->uvs[0] ) delete[] importData->uvs[0];
    if( importData->uvs[1] ) delete[] importData->uvs[1];
    if( importData->uvs[2] ) delete[] importData->uvs[2];
    if( importData->uvs[3] ) delete[] importData->uvs[3];
    if( importData->uvs[4] ) delete[] importData->uvs[4];
    if( importData->uvs[5] ) delete[] importData->uvs[5];
    if( importData->uvs[6] ) delete[] importData->uvs[6];
    if( importData->uvs[7] ) delete[] importData->uvs[7];
    if( importData->prelights ) delete[] importData->prelights;
    if( importData->triangles ) delete[] importData->triangles;
    delete importData;
}

void Import::release(import::ImportWorld* importData)
{
    if( importData->name ) delete[] importData->name;
    delete importData;
}

void Import::release(import::ImportLight* importData)
{
    delete importData;
}

void Import::release(import::IImportStream* importStream)
{
    if( dynamic_cast<RenderwareImport*>( importStream ) )
    {
        delete dynamic_cast<RenderwareImport*>( importStream );
    }
}

/**
 * normal map creation
 */

void Import::createNormalMap(
    const char* resourceName1,
    const char* resourceName2,
    float bumpiness
)
{
    RwImage* image = RwImageRead( resourceName1 ); assert( image );
    if( image )
    {
        RwImage* normalMap = RtNormMapCreateFromImage( image, false, bumpiness ); assert( normalMap );
        if( normalMap )
        {
            RwImageWrite( normalMap, resourceName2 );
            RwImageDestroy( normalMap );
        }
    }    
    RwImageDestroy( image );
}

void Import::createNormalMap(
    int               Width,
    int               Height,
    int               Depth,
    unsigned char*    Pixels,
    int               stride,
    bool              clamp,
    float             bumpiness
)
{
    RwImage* image = RwImageCreate( Width, Height, Depth ); assert( image );
    RwImageAllocatePixels( image );
    assert( RwImageGetStride( image ) == stride );
    memcpy( RwImageGetPixels( image ), Pixels, stride * Height );

    RwImage* normalMap = RtNormMapCreateFromImage( image, clamp, bumpiness ); assert( normalMap );
    memcpy( Pixels, RwImageGetPixels( normalMap ), stride * Height );

    RwImageDestroy( normalMap );
    RwImageDestroy( image );
}