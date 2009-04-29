/**
 * This source code is a part of AqWARium game project.
 * (c) Digital Dimension Development, 2004-2005
 *
 * @description intelligence user data manipulation
 *
 * @author bad3p
 */

#if !defined(IUSERDATA_INCLUDED)
#define IUSERDATA_INCLUDED

#include "headers.h"

// find named user data array within given frame
static inline RpUserDataArray* RwFrameFindUserDataArray(
    RwFrame* frame, 
    const char* arrayName )
{
    RwInt32 udaCount = RwFrameGetUserDataArrayCount( frame );
    RpUserDataArray* uda;
    for( RwInt32 i=0; i<udaCount; i++ )
    {
        uda = RwFrameGetUserDataArray( frame, i );
        if( strcmp( RpUserDataArrayGetName( uda ), arrayName ) == 0 )
        {
            return uda;
        }
    }
    return NULL;
}

// find named user data array within given material
static inline RpUserDataArray* RpMaterialFindUserDataArray(
    RpMaterial* material, 
    const char* arrayName )
{
    RwInt32 udaCount = RpMaterialGetUserDataArrayCount( material );
    RpUserDataArray* uda;
    for( RwInt32 i=0; i<udaCount; i++ )
    {
        uda = RpMaterialGetUserDataArray( material, i );
        if( strcmp( RpUserDataArrayGetName( uda ), arrayName ) == 0 )
        {
            return uda;
        }
    }
    return NULL;
}

// find named user data array within given texture
static inline RpUserDataArray* RwTextureFindUserDataArray(
    RwTexture* texture, 
    const char* arrayName )
{
    RwInt32 udaCount = RwTextureGetUserDataArrayCount( texture );
    RpUserDataArray* uda;
    for( RwInt32 i=0; i<udaCount; i++ )
    {
        uda = RwTextureGetUserDataArray( texture, i );
        if( strcmp( RpUserDataArrayGetName( uda ), arrayName ) == 0 )
        {
            return uda;
        }
    }
    return NULL;
}

// create named user data array within given texture
static inline RpUserDataArray* RwTextureCreateUserDataArray(
    RwTexture* texture, 
    const char* arrayName,
    RpUserDataFormat format,
    RwInt32 numElements )
{
    RwInt32 arrayId = RwTextureAddUserDataArray( 
        texture, const_cast<RwChar*>( arrayName ), format, numElements 
    );
    assert( arrayId != -1 );
    return RwTextureGetUserDataArray( texture, arrayId );
}

// find named user data array within given geometry
static inline RpUserDataArray* RpGeometryFindUserDataArray(
    RpGeometry* geometry, 
    const char* arrayName )
{
    RwInt32 udaCount = RpGeometryGetUserDataArrayCount( geometry );
    RpUserDataArray* uda;
    for( RwInt32 i=0; i<udaCount; i++ )
    {
        uda = RpGeometryGetUserDataArray( geometry, i );
        if( strcmp( RpUserDataArrayGetName( uda ), arrayName ) == 0 )
        {
            return uda;
        }
    }
    return NULL;
}

#endif