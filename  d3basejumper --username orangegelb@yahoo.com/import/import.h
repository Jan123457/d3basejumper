/**
 * This source code is a part of early D3 game project.
 * (c) Digital Dimension Development, 2004-2005
 *
 * @description import entity
 *
 * @author bad3p
 */

#ifndef IMPORT_IMPLEMENTATION_INCLUDED
#define IMPORT_IMPLEMENTATION_INCLUDED

#include "headers.h"
#include "../shared/ccor.h"
#include "../shared/import.h"
#include "../shared/mainwnd.h"

using namespace ccor;

/**
 * renderware import stream
 */

class RenderwareImport : public import::IImportStream
{
private:
    class ImportObject
    {
    public:
        import::ImportType type;
        union ObjectUnion
        {
            RwTexture*       texture;
            RpMaterial*      material;
            RwFrame*         frame;
            RpGeometry*      geometry;
            RpAtomic*        atomic;
            RpClump*         clump;
            RpSector*        sector;
            RpWorld*         world;
            RpLight*         light;
			RtAnimAnimation* animation;
        }
        object;
        std::string name;
		RpClump* animatedHierarchy;
    public:
        ImportObject() : type(import::itNULL) { object.texture = NULL; }
        ImportObject(RwTexture* texture) : type(import::itTexture) { object.texture = texture; }
        ImportObject(RpMaterial* material) : type(import::itMaterial) { object.material = material; }
        ImportObject(RwFrame* frame) : type(import::itFrame) { object.frame = frame; }
        ImportObject(RpGeometry* geometry) : type(import::itGeometry) { object.geometry = geometry; }
        ImportObject(RpAtomic* atomic) : type(import::itAtomic) { object.atomic = atomic; }
        ImportObject(RpClump* clump, const char* clumpName) : type(import::itClump), name(clumpName) { object.clump = clump; }
        ImportObject(RpSector* sector) : type(import::itWorldSector), name("") { object.sector = sector; }
        ImportObject(RpWorld* world) : type(import::itWorld), name("") { object.world = world; }
        ImportObject(RpLight* light) : type(import::itLight), name("") { object.light = light; }
    };
private:
    typedef std::list<ImportObject> ImportObjectL;
    typedef ImportObjectL::iterator ImportObjectI;
private:
    ImportObjectL _importObjects;
    ImportObjectI _currentObject;
private:
    RwTexDictionary* _textures;
    RpWorld*         _currentWorld;
private:
    static RwTexture* importTextureCB(RwTexture* texture, void* pData);
    static RpClump* importClumpCB(RpClump* clump, const char* clumpName, void* pData);
    static RwFrame* importFrameCB(RwFrame* frame, void* pData);
    static RpMaterial* importMaterialCB(RpMaterial* material, void* pData);
    static RpGeometry* importGeometryCB(RpGeometry* geometry, void* pData);
    static RpAtomic* importAtomicCB(RpAtomic* atomic, void* pData);
    static void importSectorCB(RpSector* worldSector, void* data);
    static RpWorld* importWorldCB(RpWorld* world, const char* name, void* data);
    static RpLight* importLightCB(RpLight* light, void* data);
public:
    // class implementation
    RenderwareImport(const char* resourceName);
    virtual ~RenderwareImport();
    // IImportStream
    virtual import::ImportType __stdcall getType(void);
    virtual import::ImportTexture* __stdcall importTexture(void);
    virtual import::ImportMaterial* __stdcall importMaterial(void);
    virtual import::ImportFrame* __stdcall importFrame(void);
    virtual import::ImportGeometry* __stdcall importGeometry(void);
    virtual import::ImportAtomic* __stdcall importAtomic(void);
    virtual import::ImportClump* __stdcall importClump(void);
    virtual import::ImportWorldSector* __stdcall importWorldSector(void);
    virtual import::ImportWorld* __stdcall importWorld(void);
    virtual import::ImportLight* __stdcall importLight(void);
};

/**
 * IImport implementation
 */

class Import : public EntityBase,
               virtual public import::IImport
{
private:
    static import::LightMapProgressCallback cipCallback;
    static void*                            cipUserData;
private:
    static RwBool callbackIlluminateProgress(RwInt32 message, RwReal value);
    static RpMaterial* callbackSetLightmapDensityModifier(RpMaterial *material, void *data);
public:
    // class implementation
    Import();
    virtual ~Import();
    // component support
    static EntityBase* creator();
    virtual void __stdcall entityDestroy();
    // EntityBase 
    virtual void __stdcall entityInit(Object * p);
    virtual void __stdcall entityAct(float dt);
    virtual void __stdcall entityHandleEvent(evtid_t id, trigid_t trigId, Object* param);
    virtual IBase* __stdcall entityAskInterface(iid_t id);
    // IImport
	virtual Matrix4f __stdcall generateHandnessMatrix(const Vector3f& translate, const Vector3f& axis, float angle);
    virtual import::IImportStream* __stdcall importRws(const char* resourceName);
    virtual void __stdcall release(import::ImportTexture* importData);
    virtual void __stdcall release(import::ImportMaterial* importData);
    virtual void __stdcall release(import::ImportFrame* importData);
    virtual void __stdcall release(import::ImportGeometry* importData);
    virtual void __stdcall release(import::ImportAtomic* importData);
    virtual void __stdcall release(import::ImportClump* importData);
    virtual void __stdcall release(import::IImportStream* importStream);
    virtual void __stdcall release(import::ImportWorldSector* importData);
    virtual void __stdcall release(import::ImportWorld* importData);
    virtual void __stdcall release(import::ImportLight* importData);
    virtual void __stdcall createNormalMap(
        int               Width,
        int               Height,
        int               Depth,
        unsigned char*    Pixels,
        int               stride,
        bool              clamp,
        float             bumpiness
    );
    virtual void __stdcall createNormalMap(
        const char* resourceName1,
        const char* resourceName2,
        float bumpiness
    );
    virtual void __stdcall calculateLightMaps(
        const char* resourceName, 
        import::LightMapProgressCallback callback,
        void* userData
    );
public:
    // import instance for module internal usage
    static Import* instance;
};

#endif
