
#if !defined(AFX_EDITOR_H__AFF49861_824D_4582_8343_5BBC1604CA40__INCLUDED_)
#define AFX_EDITOR_H__AFF49861_824D_4582_8343_5BBC1604CA40__INCLUDED_

#include "resource.h"		
#include "../shared/ccor.h" 
#include "../shared/mainwnd.h" 
#include "../shared/engine.h" 
#include "../shared/import.h" 
#include "mainform.h"
#include "renderview.h"
#include "helpdlg.h"

using namespace ccor;

class MainWnd : public EntityBase,
                virtual public mainwnd::IMainWnd
{
private:
    CFont*      _font;
    MainForm*   _mainForm;
    RenderView* _renderView;
    HelpDialog* _helpDialog;
public:
    MainWnd();
    virtual ~MainWnd();
    // component support
    static EntityBase* creator() ;
    virtual void __stdcall entityDestroy();
    // EntityBase
    virtual void __stdcall entityInit(Object * p);
    virtual void __stdcall entityAct(float dt);
    virtual void __stdcall entityHandleEvent(evtid_t id, trigid_t trigId, Object * param);
    virtual IBase * __stdcall entityAskInterface(iid_t id);
    // IMainWnd
    virtual void __stdcall setWindowText(const char* text) {}
    virtual int __stdcall getHandle(void);
public:
    static MainWnd* instance;
};

#endif
