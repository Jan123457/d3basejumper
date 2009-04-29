
#include "headers.h"
#include "import.h"
#include "iuserdata.h"

import::IImportStream* Import::importRws(const char* resourceName)
{
    return new RenderwareImport( resourceName );
}

/**
 * RenderwareImport : type wrappers
 */

static Vector4f wrap(const RwRGBA* value)
{
    return Vector4f(
        float( value->red ) / 255.0f,
        float( value->green ) / 255.0f,
        float( value->blue ) / 255.0f,
        float( value->alpha ) / 255.0f
    );
}

static Vector4f wrap(const RwRGBAReal* value)
{
    return Vector4f( value->red, value->green, value->blue, value->alpha );
}

static Vector3f wrap(const RwV3d* v)
{
    return Vector3f( v->x, v->y, v->z );
}

static Vector3f wrapScaled(const RwV3d* v)
{
    return Vector3f( v->x, v->y, v->z );
}

static float wrapScaled(RwReal v)
{
    return v;
}

static Vector2f wrap(const RwTexCoords* uv)
{
    return Vector2f( uv->u, uv->v );
}

static Matrix4f wrap(RwMatrix* matrix)
{
    return Matrix4f(
        matrix->right.x, matrix->right.y, matrix->right.z, 1.0f,
        matrix->up.x, matrix->up.y, matrix->up.z, 1.0f,
        matrix->at.x, matrix->at.y, matrix->at.z, 1.0f,
        matrix->pos.x, matrix->pos.y, matrix->pos.z, 1.0f
    );
}

/**
 * handness matrix
 */

Matrix4f Import::generateHandnessMatrix(const Vector3f& translate, const Vector3f& axis, float angle)
{
	RwMatrix matrix;
	
	RwV3d t = { translate[0], translate[1], translate[2] };
	RwMatrixTranslate( &matrix, &t, rwCOMBINEREPLACE );

	RwV3d a = { axis[0], axis[1], axis[2] };
	RwMatrixRotate( &matrix, &a, angle, rwCOMBINEPOSTCONCAT );
	
	return wrap( &matrix );
}

/**
 * RenderwareImport : callbacks
 */

RwTexture* RenderwareImport::importTextureCB(RwTexture* texture, void* pData)
{
    ((RenderwareImport*)(pData))->_importObjects.push_back( ImportObject( texture ) );
    return texture;
}

RpClump* RenderwareImport::importClumpCB(RpClump* clump, const char* clumpName, void* pData)
{
    // import clump content
    importFrameCB( RpClumpGetFrame( clump ), pData );
    RpClumpForAllAtomics( clump, importAtomicCB, pData );
    RpClumpForAllLights( clump, importLightCB, pData );
    // import clump
    ((RenderwareImport*)(pData))->_importObjects.push_back( ImportObject( clump, clumpName ) );
    return clump;
}

RwFrame* RenderwareImport::importFrameCB(RwFrame* frame, void* pData)
{
    ((RenderwareImport*)(pData))->_importObjects.push_back( ImportObject( frame ) );
    RwFrameForAllChildren( frame, importFrameCB, pData );
    return frame;
}

RpMaterial* RenderwareImport::importMaterialCB(RpMaterial* material, void* pData)
{
    ((RenderwareImport*)(pData))->_importObjects.push_back( ImportObject( material ) );
    return material;
}

RpGeometry* RenderwareImport::importGeometryCB(RpGeometry* geometry, void* pData)
{
    for( int i=0; i<RpGeometryGetNumMaterials( geometry ); i++ )
    {
        importMaterialCB( RpGeometryGetMaterial( geometry, i ) , pData );
    }

    ((RenderwareImport*)(pData))->_importObjects.push_back( ImportObject( geometry ) );
    return geometry;
}

RpAtomic* RenderwareImport::importAtomicCB(RpAtomic* atomic, void* pData)
{
    importGeometryCB( RpAtomicGetGeometry( atomic ), pData );

    RwTexture* lightmap = RpLtMapAtomicGetLightMap( atomic );
    if( lightmap ) importTextureCB( lightmap, pData );

    ((RenderwareImport*)(pData))->_importObjects.push_back( ImportObject( atomic ) );
    return atomic;
}

void RenderwareImport::importSectorCB(RpSector* sector, void* data)
{
    if( sector->type == -1 )
    {
        RpWorldSector* worldSector = (RpWorldSector*)( sector );
        RwTexture* lightmap = RpLtMapWorldSectorGetLightMap( worldSector );
        if( lightmap ) importTextureCB( lightmap, data );
    }

    ((RenderwareImport*)(data))->_importObjects.push_back( ImportObject( sector ) );
    
    if( sector->type != -1 )
    {
        RpPlaneSector* planeSector = (RpPlaneSector*)( sector );
        importSectorCB( planeSector->leftSubTree, data );
        importSectorCB( planeSector->rightSubTree, data );
    }
}

RpWorld* RenderwareImport::importWorldCB(RpWorld* world, const char* name, void* data)
{
    int numMaterials = RpWorldGetNumMaterials( world );
    for( int i=0; i<numMaterials; i++ ) importMaterialCB( RpWorldGetMaterial( world, i ), data );
    ImportObject importObject( world );
    importObject.name = name;
    ((RenderwareImport*)(data))->_importObjects.push_back( importObject );
    importSectorCB( world->rootSector, data );
    return world;
}

RpLight* RenderwareImport::importLightCB(RpLight* light, void* data)
{
    ((RenderwareImport*)(data))->_importObjects.push_back( ImportObject( light ) );
    return light;
}

/**
 * RenderwareImport : class implementation
 */

RenderwareImport::RenderwareImport(const char* resourceName)
{
    std::string resourcePath = "./res/import/";
    resourcePath += resourceName;

    // first, import textures
    RwStream* stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, resourcePath.c_str() );  
    if( !stream ) throw Exception( "Input::importFromRws() : can't open resource \"%s\"", resourceName );
    
    // process stream chunkz
    RwChunkHeaderInfo chunkHeaderInfo;
    _textures = NULL;
    while( RwStreamReadChunkHeaderInfo( stream, &chunkHeaderInfo ) )
    switch( chunkHeaderInfo.type )
    {
    case rwID_PITEXDICTIONARY:
        _textures = RtPITexDictionaryStreamRead( stream );
        break;
    default:
        RwStreamSkip( stream, chunkHeaderInfo.length );
        break;
    }
    if( _textures )
    {
        RwTexDictionaryForAllTextures( _textures, importTextureCB, this );
        RwTexDictionarySetCurrent( _textures );
    }
    RwStreamClose( stream, NULL );

    // second, import scenes & clumps
    stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, resourcePath.c_str() ); 
    assert( stream );

    // process stream chunkz
    RwChunkGroup* chunkGroup = NULL;
    RpClump* clump = NULL;
    RpWorld* world = NULL;
    while( RwStreamReadChunkHeaderInfo( stream, &chunkHeaderInfo ) ) 
    switch( chunkHeaderInfo.type )    
    {    
    case rwID_CHUNKGROUPSTART:
		if( chunkGroup == NULL ) chunkGroup = RwChunkGroupBeginStreamRead( stream );
        break;
    case rwID_CHUNKGROUPEND:
        if( chunkGroup != NULL )
		{
			RwChunkGroupEndStreamRead( chunkGroup, stream );
			RwChunkGroupDestroy( chunkGroup );
			chunkGroup = NULL;
		}
        break;
    case rwID_CLUMP:
        assert( chunkGroup );
        clump = RpClumpStreamRead( stream );
        importClumpCB( clump, chunkGroup->name, this );
        break;	
    case rwID_WORLD:
        assert( chunkGroup );
        world = RpWorldStreamRead( stream );
        importWorldCB( world, chunkGroup->name, this );
        break;
    default:
        RwStreamSkip( stream, chunkHeaderInfo.length );
        break;
    }

    _importObjects.push_back( ImportObject() );
    _currentObject = _importObjects.begin();
}

RenderwareImport::~RenderwareImport()
{
    for( ImportObjectI importObjectI = _importObjects.begin();
                       importObjectI != _importObjects.end();
                       importObjectI++ )
    {
        if( importObjectI->type == import::itClump )
        {
            RpClumpDestroy( importObjectI->object.clump );
        }
    }
    if( _textures ) RwTexDictionaryDestroy( _textures );
}

/**
 * RenderwareImport : IImportStream implementation
 */

import::ImportType RenderwareImport::getType(void)
{
    return _currentObject->type;
}

static import::ImportAddressMode wrap(RwTextureAddressMode addressMode)
{
    switch( addressMode )
    {
    case rwTEXTUREADDRESSWRAP: return import::iamWrap;
    case rwTEXTUREADDRESSMIRROR: return import::iamMirror;
    case rwTEXTUREADDRESSCLAMP: return import::iamClamp;
    case rwTEXTUREADDRESSBORDER: return import::iamBorder;
    default: return import::iamWrap;
    }
}

static import::ImportFilterMode wrap(RwTextureFilterMode filterMode)
{
    switch( filterMode )
    {
    case rwFILTERNEAREST: return import::ifmNearest;
    case rwFILTERLINEAR: return import::ifmLinear;
    case rwFILTERMIPNEAREST: return import::ifmMipNearest;
    case rwFILTERMIPLINEAR: return import::ifmMipLinear;
    case rwFILTERLINEARMIPNEAREST: return import::ifmLinearMipNearest;
    case rwFILTERLINEARMIPLINEAR: return import::ifmLinearMipLinear; 
    default: return import::ifmNearest;
    }
};

import::ImportTexture* RenderwareImport::importTexture(void)
{
    assert( _currentObject->type == import::itTexture );
    
    RwTexture* texture = _currentObject->object.texture;
    import::ImportTexture* importData = new import::ImportTexture;
    importData->id = (import::iuid)( texture );
    if( RwTextureGetName( texture ) )
    {
        importData->name = new char[ strlen( RwTextureGetName( texture ) ) + 1 ];
        strcpy( importData->name, RwTextureGetName( texture ) );
    }
    importData->width = RwRasterGetWidth( RwTextureGetRaster( texture ) );
    importData->height = RwRasterGetHeight( RwTextureGetRaster( texture ) );
    importData->depth = 32;
    importData->addressMode = wrap( RwTextureGetAddressing( texture ) );
    importData->filterMode = wrap( RwTextureGetFilterMode( texture ) );

    // create image from texture
    RwImage* image = RwImageCreate( importData->width, importData->height, importData->depth );   
    RwImageAllocatePixels( image );
    RwImageSetFromRaster( image, RwTextureGetRaster( texture ) );
    importData->stride = RwImageGetStride( image );
    importData->pixels = new unsigned char[ importData->stride * importData->height ];
    memcpy( importData->pixels, RwImageGetPixels( image ), importData->stride * importData->height );
    RwImageDestroy( image );

    _currentObject++;

    return importData;
}

import::ImportMaterial* RenderwareImport::importMaterial(void)
{
    assert( _currentObject->type == import::itMaterial );
    RpMaterial* material = _currentObject->object.material;

    import::ImportMaterial* importData = new import::ImportMaterial;
    importData->id = (import::iuid)( material );
    importData->color = wrap( RpMaterialGetColor( material ) );
    
    RpUserDataArray* uda = RpMaterialFindUserDataArray( material, "name" );
    if( uda )
    {
        importData->name = new char[ strlen( RpUserDataArrayGetString( uda, 0 ) ) + 1 ];
        strcpy( importData->name, RpUserDataArrayGetString( uda, 0 ) );
    }

    if( RpMaterialGetTexture( material ) )
    {
        importData->textureId = (import::iuid)( RpMaterialGetTexture( material ) );
    }

    if( RpMatFXMaterialGetEffects( material ) == rpMATFXEFFECTDUAL )
    {
        importData->dualPassTextureId = (import::iuid)( RpMatFXMaterialGetDualTexture( material ) );

        RwBlendFunction srcBlendMode, dstBlendMode;
        RpMatFXMaterialGetDualBlendModes( material, &srcBlendMode, &dstBlendMode );
        if( srcBlendMode == rwBLENDONE && dstBlendMode == rwBLENDONE )
        {
            importData->dualPassBlendType = import::ImportMaterial::btAdd;
        }
        else if( ( srcBlendMode == rwBLENDZERO && dstBlendMode == rwBLENDONE ) ||
                 ( srcBlendMode == rwBLENDONE && dstBlendMode == rwBLENDZERO ) )
        {
            importData->dualPassBlendType = import::ImportMaterial::btModulate;
        }
        else if( srcBlendMode == rwBLENDSRCALPHA && dstBlendMode == rwBLENDINVSRCALPHA )
        {
            importData->dualPassBlendType = import::ImportMaterial::btBlendTextureAlpha;
        }
        else
        {
            importData->dualPassBlendType = import::ImportMaterial::btOver;
        }
    }
    else
    {
        importData->dualPassTextureId = 0;
        importData->dualPassBlendType = import::ImportMaterial::btOver;
    }

    _currentObject++;

    return importData;
}

import::ImportFrame* RenderwareImport::importFrame(void)
{
    assert( _currentObject->type == import::itFrame );
    RwFrame* frame = _currentObject->object.frame;

    import::ImportFrame* importData = new import::ImportFrame;
    importData->id = (import::iuid)( frame );

    if( RwFrameGetParent( frame ) ) 
    {
        importData->parentId = (import::iuid)( RwFrameGetParent( frame ) );
    }

    RpUserDataArray* uda = RwFrameFindUserDataArray( frame, "name" );
    if( uda )
    {
        importData->name = new char[ strlen( RpUserDataArrayGetString( uda, 0 ) ) + 1 ];
        strcpy( importData->name, RpUserDataArrayGetString( uda, 0 ) );
    }

    importData->modeling = wrap( RwFrameGetMatrix( frame ) );

    _currentObject++;

    return importData;
}

import::ImportGeometry* RenderwareImport::importGeometry(void)
{
    assert( _currentObject->type == import::itGeometry );
    RpGeometry* geometry = _currentObject->object.geometry;

    import::ImportGeometry* importData = new import::ImportGeometry;
    importData->id = (import::iuid)( geometry );

    RpUserDataArray* uda = RpGeometryFindUserDataArray( geometry, "name" );
    if( uda )
    {
        importData->name = new char[ strlen( RpUserDataArrayGetString( uda, 0 ) ) + 1 ];
        strcpy( importData->name, RpUserDataArrayGetString( uda, 0 ) );
    }

    importData->numVertices = RpGeometryGetNumVertices( geometry );
    importData->numTriangles = RpGeometryGetNumTriangles( geometry );
    importData->numUVs = 1;

    RwTexCoords* uvs2 = RpGeometryGetVertexTexCoords( geometry, rwTEXTURECOORDINATEINDEX1 );
    if( uvs2 != NULL ) importData->numUVs++;

    importData->numMaterials = RpGeometryGetNumMaterials( geometry );

    importData->vertices = new Vector3f[ importData->numVertices ];
    const RwV3d* vertices = RpMorphTargetGetVertices( RpGeometryGetMorphTarget( geometry, 0 ) );
    for( int i=0; i<importData->numVertices; i++ )
    {
        importData->vertices[i] = wrapScaled( vertices + i );
    }

    importData->normals = new Vector3f[ importData->numVertices ];
    const RwV3d* normals = RpMorphTargetGetVertexNormals( RpGeometryGetMorphTarget( geometry, 0 ) );
    for( i=0; i<importData->numVertices; i++ )
    {
        importData->normals[i] = wrap( normals + i );
    }

    importData->uvs[0] = new Vector2f[ importData->numVertices ];
    const RwTexCoords* uvs = RpGeometryGetVertexTexCoords( geometry, rwTEXTURECOORDINATEINDEX0 );
    if( uvs ) for( i=0; i<importData->numVertices; i++ )
    {
        importData->uvs[0][i] = wrap( uvs + i );
    }
    else for( i=0; i<importData->numVertices; i++ )
    {
        importData->uvs[0][i].set( 0,0 );
    }

    if( importData->numUVs == 2 )
    {
        importData->uvs[1] = new Vector2f[ importData->numVertices ];    
        for( i=0; i<importData->numVertices; i++ )
        {
            importData->uvs[1][i] = wrap( uvs2 + i );
        }    
    }

    const RwRGBA* prelights = RpGeometryGetPreLightColors( geometry );
    if( prelights )
    {
        importData->prelights = new Vector4f[ importData->numVertices ];
        for( i=0; i<importData->numVertices; i++ )
        {
            importData->prelights[i] = wrap( prelights + i );
        }
    }

    importData->triangles = new import::Triangle[ importData->numTriangles ];
    const RpTriangle* triangles = RpGeometryGetTriangles( geometry );
    for( i=0; i<importData->numTriangles; i++ )
    {
        importData->triangles[i].vertexId[0] = triangles[i].vertIndex[0];
        importData->triangles[i].vertexId[1] = triangles[i].vertIndex[1];
        importData->triangles[i].vertexId[2] = triangles[i].vertIndex[2];
        importData->triangles[i].materialId = triangles[i].matIndex;
    }

    importData->materials = new import::iuid[ importData->numMaterials ];
    for( i=0; i<importData->numMaterials; i++ )
    {
        importData->materials[i] = (import::iuid)( RpGeometryGetMaterial( geometry, i ) );
    }

    _currentObject++;

    return importData;
}

import::ImportAtomic* RenderwareImport::importAtomic(void)
{
    assert( _currentObject->type == import::itAtomic );
    RpAtomic* atomic = _currentObject->object.atomic;

    import::ImportAtomic* importData = new import::ImportAtomic;
    importData->id = (import::iuid)( atomic );
    importData->frameId = (import::iuid)( RpAtomicGetFrame( atomic ) );
    importData->geometryId = (import::iuid)( RpAtomicGetGeometry( atomic ) );
    importData->lightmapId = (import::iuid)( RpLtMapAtomicGetLightMap( atomic ) );
    _currentObject++;

    return importData;
}

static RpAtomic* atomicEnumerationCB(RpAtomic* atomic, void* pData)
{
    std::list<RpAtomic*>* atomicList = (std::list<RpAtomic*>*)(pData);
    atomicList->push_back( atomic );
    return atomic;
}

static RpLight* lightEnumerationCB(RpLight* light, void* pData)
{
    std::list<RpLight*>* lightList = (std::list<RpLight*>*)(pData);
    lightList->push_back( light );
    return light;
}

import::ImportClump* RenderwareImport::importClump(void)
{
    assert( _currentObject->type == import::itClump );
    RpClump* clump = _currentObject->object.clump;

    import::ImportClump* importData = new import::ImportClump;
    importData->id = (import::iuid)( clump );
    importData->name = new char[ _currentObject->name.length() + 1];
    strcpy( importData->name, _currentObject->name.c_str() );

    std::list<RpAtomic*> atomicList;
    RpClumpForAllAtomics( clump, atomicEnumerationCB, &atomicList );
    std::list<RpLight*> lightList;
    RpClumpForAllLights( clump, lightEnumerationCB, &lightList );

    importData->frameId = (import::iuid)( RpClumpGetFrame( clump ) );
    importData->numAtomics = atomicList.size();
    importData->atomics = new import::iuid[ importData->numAtomics ];
    int i = 0;
    for( std::list<RpAtomic*>::iterator atomicI = atomicList.begin();
                                        atomicI != atomicList.end();
                                        atomicI++, i++ )
    {
        importData->atomics[i] = (import::iuid)(*atomicI);
    }

    importData->numLights = lightList.size();
    if( importData->numLights )
    {
        importData->lights = new import::iuid[ importData->numLights ];
        i=0;
        for( std::list<RpLight*>::iterator lightI = lightList.begin();
                                           lightI != lightList.end();
                                           lightI++, i++ )
        {
            importData->lights[i] = (import::iuid)(*lightI);
        }
    }

    _currentObject++;

    return importData;
}

struct ParentSearchData
{
    RpWorld*       world;
    RpWorldSector* worldSector;
    RpWorldSector* candidate;
};

static RwBool RwBBoxIsInside(const RwBBox* box1, const RwBBox* box2)
{
    return box1->sup.x >= box2->sup.x && 
           box1->sup.y >= box2->sup.y && 
           box1->sup.z >= box2->sup.z && 
           box1->inf.x <= box2->inf.x && 
           box1->inf.y <= box2->inf.y && 
           box1->inf.z <= box2->inf.z;
}

static RwBool RwBBoxIsLess(const RwBBox* box1, const RwBBox* box2)
{
    RwV3d diag1, diag2;
    RwV3dSub( &diag1, &box1->sup, &box1->inf );
    RwV3dSub( &diag2, &box2->sup, &box2->inf );
    return RwV3dLength( &diag1 ) > RwV3dLength( &diag2 );
}

static RpSector* RpWorldSectorGetParentCB(RpSector* sector, RpSector* target)
{
    if( sector->type == -1 ) return NULL;
    RpPlaneSector* planeSector = (RpPlaneSector*)( sector );
    if( planeSector->leftSubTree == target ) return sector;
    if( planeSector->rightSubTree == target ) return sector;
    RpSector* result = RpWorldSectorGetParentCB( planeSector->leftSubTree, target );
    if( result ) return result;
    return RpWorldSectorGetParentCB( planeSector->rightSubTree, target );
}

static RpSector* RpWorldSectorGetParent(RpSector* sector, RpWorld* world)
{    
    return RpWorldSectorGetParentCB( world->rootSector, sector );
}

static void RpSectorBuildBBox(RpSector* sector, RwBBox* box)
{
    if( sector->type != -1 )
    {
        RpPlaneSector* planeSector = (RpPlaneSector*)( sector );
        RwBBox leftBox; 
        RpSectorBuildBBox( planeSector->leftSubTree, &leftBox ); 
        RwBBox rightBox;
        RpSectorBuildBBox( planeSector->rightSubTree, &rightBox ); 
        RwBBoxInitialize( box, &leftBox.inf );
        RwBBoxAddPoint( box, &leftBox.sup );
        RwBBoxAddPoint( box, &rightBox.inf );
        RwBBoxAddPoint( box, &rightBox.sup );
    }
    else
    {
        *box = ((RpWorldSector*)(sector))->boundingBox;
    }
}

import::ImportWorldSector* RenderwareImport::importWorldSector(void)
{
    RwInt32 i,j;

    assert( _currentObject->type == import::itWorldSector );
    RpSector* sector = _currentObject->object.sector;
    RpWorld* world = _currentWorld;

    if( sector->type != -1 )
    {
        RpPlaneSector* planeSector = (RpPlaneSector*)( sector );
        import::ImportWorldSector* importData = new import::ImportWorldSector;
        importData->id = (import::iuid)( sector );
        RwBBox box;
        RpSectorBuildBBox( sector, &box );
        importData->aabbInf = wrapScaled( &box.inf );
        importData->aabbSup = wrapScaled( &box.sup );
        importData->normals = NULL;
        importData->numTriangles = 0;
        importData->numUVs = 0;
        importData->numVertices = 0;
        importData->parentId = (import::iuid)( RpWorldSectorGetParent( sector, world ) );
        importData->prelights = NULL;
        importData->triangles = NULL;
        importData->uvs[0] = NULL;
        importData->uvs[1] = NULL;
        importData->uvs[2] = NULL;
        importData->uvs[3] = NULL;
        importData->vertices = NULL;
        importData->worldId = (import::iuid)( world );
        _currentObject++;
        return importData;
    }

    RpWorldSector* worldSector = (RpWorldSector*)( sector );

    RwInt32 numVertices  = worldSector->numVertices;
    RwInt32 numTriangles = worldSector->numTriangles;    
    
    RpTriangle*     triangles = worldSector->triangles;
    RwV3d*          vertices  = worldSector->vertices;
    RpVertexNormal* normals   = worldSector->normals;
    RwTexCoords**   texCoords = worldSector->texCoords;
    RwRGBA*         preLitLum = worldSector->preLitLum;

    RwInt32 numUVs = 0;
    if( texCoords[0] ) numUVs++;
    if( texCoords[1] ) numUVs++;

    import::ImportWorldSector* importData = new import::ImportWorldSector;
    importData->id           = (import::iuid)( sector );
    importData->worldId      = (import::iuid)( world );
    importData->aabbInf      = wrapScaled( &RpWorldSectorGetBBox( worldSector )->inf );
    importData->aabbSup      = wrapScaled( &RpWorldSectorGetBBox( worldSector )->sup );
    importData->numVertices  = numVertices;
    importData->numTriangles = numTriangles;
    importData->numUVs       = numUVs;
    importData->parentId     = (import::iuid)( RpWorldSectorGetParent( sector, world ) );
    importData->lightmapId   = (import::iuid)( RpLtMapWorldSectorGetLightMap ( worldSector ) ); 

    importData->vertices = new Vector3f[numVertices];
    importData->normals  = new Vector3f[numVertices];
    if( preLitLum ) importData->prelights = new Vector4f[numVertices];
    for( i=0; i<numUVs; i++ ) importData->uvs[i] = new Vector2f[numVertices];
    importData->triangles = new import::Triangle[numTriangles];

    RwV3d normal;
    for( i=0; i<numVertices; i++ )
    {
        importData->vertices[i] = wrapScaled( vertices+i );
        RPV3DFROMVERTEXNORMAL( normal, normals[i] );
        importData->normals[i] = wrap( &normal );
        for( j=0; j<numUVs; j++ ) importData->uvs[j][i] = wrap( texCoords[j]+i );
        if( preLitLum ) importData->prelights[i] = wrap( preLitLum + i );
    }
    for( i=0; i<numTriangles; i++ )
    {
        importData->triangles[i].vertexId[0] = triangles[i].vertIndex[0];
        importData->triangles[i].vertexId[1] = triangles[i].vertIndex[1];
        importData->triangles[i].vertexId[2] = triangles[i].vertIndex[2];
        importData->triangles[i].materialId = triangles[i].matIndex;
    }
    _currentObject++;

    return importData;
}

import::ImportWorld* RenderwareImport::importWorld(void)
{
    assert( _currentObject->type == import::itWorld );
    RpWorld* world = _currentObject->object.world;

    import::ImportWorld* importData = new import::ImportWorld;
    
    importData->id = (import::iuid)( world );
    importData->name = new char[_currentObject->name.length()+1];
    strcpy( importData->name, _currentObject->name.c_str() );

    importData->aabbInf = wrap( &RpWorldGetBBox( world )->inf );
    importData->aabbSup = wrap( &RpWorldGetBBox( world )->sup );

    importData->numMaterials = RpWorldGetNumMaterials( world );
    importData->materials = new import::iuid[ importData->numMaterials ];
    for( int i=0; i<importData->numMaterials; i++ )
    {
        importData->materials[i] = (import::iuid)( RpWorldGetMaterial( world, i ) );
    }
    _currentObject++;
    _currentWorld = world;

    return importData;
}

import::ImportLight* RenderwareImport::importLight(void)
{
    assert( _currentObject->type == import::itLight );
    RpLight* light = _currentObject->object.light;

    import::ImportLight* importData = new import::ImportLight;
    switch( RpLightGetType( light ) )
    {
    case rpLIGHTDIRECTIONAL:
        importData->type = import::ImportLight::ltDirectional;
        break;
    case rpLIGHTAMBIENT:
        importData->type = import::ImportLight::ltAmbient;
        break;
    case rpLIGHTPOINT:
        importData->type = import::ImportLight::ltPoint;
        break;
    case rpLIGHTSPOT:
    case rpLIGHTSPOTSOFT:
        importData->type = import::ImportLight::ltSpot;
        break;
    default:
        assert( !"shouldn't be here!" );
    };

    importData->radius    = wrapScaled( RpLightGetRadius( light ) );
    importData->color     = wrap( RpLightGetColor( light ) );
    importData->coneAngle = RpLightGetConeAngle( light );

    importData->id = (import::iuid)( light );
    importData->frameId = (import::iuid)( RpLightGetFrame( light ) );

    _currentObject++;

    return importData;
}