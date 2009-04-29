#if !defined(AFX_MAINFORM_H__9BDEBF8D_E496_4E1A_9CBB_DE25FF13AB11__INCLUDED_)
#define AFX_MAINFORM_H__9BDEBF8D_E496_4E1A_9CBB_DE25FF13AB11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mainform.h : header file
//

#include "renderview.h"
#include "helpdlg.h"
#include "shaderedit.h"
#include "engineinfo.h"
#include "geoedit.h"
#include "bspedit.h"
#include "lightedit.h"

/////////////////////////////////////////////////////////////////////////////
// MainForm data manipulation

typedef std::list<engine::IBSP*> BSPL;
typedef BSPL::iterator BSPI;

typedef std::list<engine::IBSPSector*> BSPSectorL;
typedef BSPSectorL::iterator BSPSectorI;

typedef std::list<engine::IClump*> ClumpL;
typedef ClumpL::iterator ClumpI;

typedef std::list<engine::IAtomic*> AtomicL;
typedef AtomicL::iterator AtomicI;

typedef std::list<engine::IFrame*> FrameL;
typedef FrameL::iterator FrameI;

typedef std::list<engine::ILight*> LightL;
typedef LightL::iterator LightI;

engine::IEngine* iEngine(void);

/////////////////////////////////////////////////////////////////////////////
// MainForm dialog

class MainForm : public CDialog
{
public:
    CImageList*       graphImages;
    RenderView*       renderView;
    HelpDialog*       helpDialog;
    ShaderEditor*     shaderEditor;
    EngineInfo*       engineInfo;
    GeometryEditor*   geometryEditor;
    BSPEditor*        bspEditor;
    LightEditor*      lightEditor;

private:
    // workspace
    engine::IAsset*      currentAsset;
    BSPL                 bspList;
    ClumpL               clumpList;
    engine::IBSP*        defaultBSP;
	engine::IBSP*        currentBSP;

private:
    void alignControls(void);
    void destroyAsset(void);
    void createDefaultStage(void);
    void preparateAsset(void);
    void buildAssetTree(void);
    void insertWorld(engine::IBSP* bsp, HTREEITEM parentItem);
    void insertClump(engine::IClump* clump, HTREEITEM parentItem);
    void insertFrame(engine::IFrame* frame, HTREEITEM parentItem, AtomicL& atomics, LightL& lights);
    void insertAtomic(engine::IAtomic* atomic, HTREEITEM parentItem);
    void insertGeometry(engine::IGeometry* geometry, HTREEITEM parentItem);
    void insertShader(engine::IShader* shader, HTREEITEM parentItem);
    void insertSector(engine::IBSPSector* sector, HTREEITEM parentItem);
    void insertLight(engine::ILight* light, HTREEITEM parentItem);
    void setNumberOfTextureLayers(unsigned int numLayers);

private:
    static void lightMapCallback(float progress, void* userData);

public:
	engine::IBSP* getCurrentBSP(void) { return currentBSP; }

// Construction
public:
	MainForm(CWnd* pParent = NULL);   // standard constructor
    virtual ~MainForm();

// Dialog Data
	//{{AFX_DATA(MainForm)
	enum { IDD = IDD_MAINFORM_DIALOG };
	CTabCtrl	shelf;
	CEdit	consoleBox;
	CEdit	commandBox;
	CTreeCtrl	assetGraph;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MainForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MainForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnAssetImport();
	afx_msg void OnHelpEditorhelp();
	afx_msg void OnAssetSave();
	afx_msg void OnAssetOpen();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblclkAssetgraph(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnWindowEngineinfo();    
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnClumpHideallatomics();
    afx_msg void OnAnimationResetanimation();
    afx_msg void OnSpeed1x();
    afx_msg void OnSpeed0();
    afx_msg void OnSpeed025x();
    afx_msg void OnGeneratorGeometrydetails();
    afx_msg void OnClumpHideselectedatomic();
    afx_msg void OnClumpForceprelight();
    afx_msg void OnEnableAlphaTree();
    afx_msg void OnEnableAlphaBanister();
    afx_msg void OnEnableNoCull();
    afx_msg void OnToolsCreatebumpmap();
    afx_msg void OnToolsNormalmapfromtexture();
    afx_msg void OnSetNumTextureLayers0();
    afx_msg void OnSetNumTextureLayers1();
    afx_msg void OnSetNumTextureLayers2();
    afx_msg void OnSetNumTextureLayers3();
    afx_msg void OnSetNumTextureLayers4();
    afx_msg void OnToolsCalculateLightMaps();
    afx_msg void OnClumpDelete();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFORM_H__9BDEBF8D_E496_4E1A_9CBB_DE25FF13AB11__INCLUDED_)
