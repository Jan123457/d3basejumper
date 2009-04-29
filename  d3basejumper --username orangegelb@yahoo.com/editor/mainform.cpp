// mainform.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "mainform.h"
#include "helpdlg.h"
#include "shaderedit.h"
#include "../common/istring.h"
#include ".\mainform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static engine::IEngine* _iEngine = NULL;

engine::IEngine* iEngine(void)
{
    if( !_iEngine ) queryInterfaceT( "Engine", &_iEngine );
    return _iEngine;
}

/////////////////////////////////////////////////////////////////////////////
// MainForm dialog

MainForm::MainForm(CWnd* pParent /*=NULL*/)
	: CDialog(MainForm::IDD, pParent)
{
	//{{AFX_DATA_INIT(MainForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    engineInfo       = NULL;
    renderView       = NULL;
    currentAsset     = NULL;
    defaultBSP       = NULL;
    geometryEditor   = NULL;
    bspEditor        = NULL;
	currentBSP       = NULL;    

    CBitmap* bitmap;
    graphImages = new CImageList;
    graphImages->Create( 16, 16, ILC_COLOR24, 1, 1 );

    // fill image list
    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_WORLD );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_CLUMP );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_FRAME );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_ATOMIC );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_GEOMETRY );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_SHADER );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_ATOMIC ); /* sectors are uses same icon that the atomics */
    graphImages->Add( bitmap, (CBitmap*)(NULL) );

    bitmap = new CBitmap;
    bitmap->LoadBitmap( IDB_LIGHT );
    graphImages->Add( bitmap, (CBitmap*)(NULL) );
}

MainForm::~MainForm()
{    
}

void MainForm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MainForm)
	DDX_Control(pDX, IDC_SHELF, shelf);
	DDX_Control(pDX, IDC_CONSOLE, consoleBox);
	DDX_Control(pDX, IDC_COMMAND, commandBox);
	DDX_Control(pDX, IDC_ASSETGRAPH, assetGraph);
	//}}AFX_DATA_MAP
}

LRESULT MainForm::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    // process messages
    switch( message )
    {
    case WM_CLOSE:
        destroyAsset();
        if( defaultBSP ) 
        {
            if( currentBSP == defaultBSP ) currentBSP = NULL;
            defaultBSP->release();
        }        
        PostQuitMessage( 0 );
        break;
    }

    return CDialog::WindowProc( message, wParam, lParam );
}

BEGIN_MESSAGE_MAP(MainForm, CDialog)
	//{{AFX_MSG_MAP(MainForm)
	ON_WM_SIZE()
	ON_COMMAND(ID_ASSET_IMPORT, OnAssetImport)
	ON_COMMAND(ID_HELP_EDITORHELP, OnHelpEditorhelp)
	ON_COMMAND(ID_ASSET_SAVE, OnAssetSave)
	ON_COMMAND(ID_ASSET_OPEN, OnAssetOpen)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_ASSETGRAPH, OnDblclkAssetgraph)
	ON_COMMAND(ID_WINDOW_ENGINEINFO, OnWindowEngineinfo)
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_CLUMP_HIDEALLATOMICS, OnClumpHideallatomics)
    ON_COMMAND(ID_ANIMATION_RESETANIMATION, OnAnimationResetanimation)
    ON_COMMAND(ID_SPEED_1X, OnSpeed1x)
    ON_COMMAND(ID_SPEED_0, OnSpeed0)
    ON_COMMAND(ID_SPEED_1, OnSpeed025x)
    ON_COMMAND(ID_GENERATOR_GEOMETRYDETAILS, OnGeneratorGeometrydetails)
    ON_COMMAND(ID_CLUMP_HIDESELECTEDATOMIC, OnClumpHideselectedatomic)
    ON_COMMAND(ID_CLUMP_FORCEPRELIGHT, OnClumpForceprelight)
    ON_COMMAND(ID_ENABLE_ALPHA_TREE, OnEnableAlphaTree)
    ON_COMMAND(ID_ENABLE_ALPHA_BANISTER, OnEnableAlphaBanister)
    ON_COMMAND(ID_ENABLE_NO_CULL, OnEnableNoCull)
    ON_COMMAND(ID_TOOLS_CREATEBUMPMAP, OnToolsCreatebumpmap)
    ON_COMMAND(ID_TOOLS_NORMALMAPFROMTEXTURE, OnToolsNormalmapfromtexture)
    ON_COMMAND(ID_SETNUMBEROFTEXTURELAYERS_0, OnSetNumTextureLayers0)
    ON_COMMAND(ID_SETNUMBEROFTEXTURELAYERS_1, OnSetNumTextureLayers1)
    ON_COMMAND(ID_SETNUMBEROFTEXTURELAYERS_2, OnSetNumTextureLayers2)
    ON_COMMAND(ID_SETNUMBEROFTEXTURELAYERS_3, OnSetNumTextureLayers3)
    ON_COMMAND(ID_SETNUMBEROFTEXTURELAYERS_4, OnSetNumTextureLayers4)
    ON_COMMAND(ID_TOOLS_CALCULATELIGHTMAPS32825, OnToolsCalculateLightMaps)
    ON_COMMAND(ID_CLUMP_DELETE, OnClumpDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MainForm extended behaviour

void MainForm::alignControls(void)
{
    RECT clientRect;
    GetClientRect( &clientRect );

    if( shelf.m_hWnd )
    {
        RECT rect;
        shelf.GetClientRect( &rect );
        shelf.SetWindowPos( 
            NULL, 
            0, 
            0, 
            clientRect.right - clientRect.left, 
            rect.bottom - rect.top, 
            0 
        );
    }

    if( commandBox.m_hWnd )
    {
        RECT rect;
        commandBox.GetWindowRect ( &rect );
        commandBox.SetWindowPos( 
            NULL, 
            0, 
            clientRect.bottom - ( rect.bottom - rect.top ), 
            clientRect.right - clientRect.left, 
            rect.bottom - rect.top, 
            0 
        );
    }

    if( consoleBox.m_hWnd )
    {
        RECT commandBoxRect, consoleBoxRect;
        commandBox.GetWindowRect( &commandBoxRect );
        consoleBox.GetWindowRect( &consoleBoxRect );
        consoleBox.SetWindowPos( 
            NULL, 
            0, 
            clientRect.bottom - ( consoleBoxRect.bottom - consoleBoxRect.top ) - ( commandBoxRect.bottom - commandBoxRect.top ), 
            clientRect.right - clientRect.left, 
            consoleBoxRect.bottom - consoleBoxRect.top, 
            0 
        );
    }

    if( assetGraph.m_hWnd )
    {
        RECT assetGraphRect, shelfRect, commandBoxRect, consoleBoxRect;
        assetGraph.GetWindowRect( &assetGraphRect );
        shelf.GetClientRect( &shelfRect );
        commandBox.GetWindowRect( &commandBoxRect );
        consoleBox.GetWindowRect( &consoleBoxRect );
        assetGraph.SetWindowPos( 
            NULL, 
            0, 
            shelfRect.bottom - shelfRect.top, 
            assetGraphRect.right - assetGraphRect.left, 
            clientRect.bottom - clientRect.top - ( shelfRect.bottom - shelfRect.top ) - ( consoleBoxRect.bottom - consoleBoxRect.top ) - ( commandBoxRect.bottom - commandBoxRect.top ) - 1, 
            0 
        );
    }

    if( renderView )
    {
        RECT renderViewRect, sceneGraphRect, shelfRect, commandBoxRect, consoleBoxRect;
        renderView->GetWindowRect( &renderViewRect );
        assetGraph.GetWindowRect( &sceneGraphRect );        
        shelf.GetClientRect( &shelfRect );
        commandBox.GetWindowRect( &commandBoxRect );
        consoleBox.GetWindowRect( &consoleBoxRect );
        renderView->SetWindowPos(
            NULL,
            sceneGraphRect.right - sceneGraphRect.left,
            shelfRect.bottom - shelfRect.top,
            clientRect.right - clientRect.left - ( sceneGraphRect.right - sceneGraphRect.left ),
            clientRect.bottom - clientRect.top - ( shelfRect.bottom - shelfRect.top ) - ( consoleBoxRect.bottom - consoleBoxRect.top ) - ( commandBoxRect.bottom - commandBoxRect.top ) - 1, 
            0
        );
    }
}

void MainForm::destroyAsset(void)
{
    if( currentAsset )
    {
        bspList.clear();
        clumpList.clear();
        for( BSPI bspI = bspList.begin(); bspI != bspList.end(); bspI++ )
        {
            if( currentBSP == *bspI )
            {
                currentBSP = NULL;
            }
        }
        currentAsset->release();
        currentAsset = NULL;
    }
}

void MainForm::createDefaultStage(void)
{
    defaultBSP = iEngine()->createBSP( 
        "Default scene",
        Vector3f( -100000, -100000, -100000 ),
        Vector3f( 100000, 100000, 100000 )
    );

    engine::ILight* ambientLight = iEngine()->createLight( engine::ltAmbient );
    engine::IFrame* ambientLightFrame = iEngine()->createFrame( "ambientLightFrame" );
    ambientLight->setFrame( ambientLightFrame );
    ambientLight->setDiffuseColor( Vector4f( 0.25f, 0.25f, 0.25f, 1.0f ) );
    defaultBSP->add( ambientLight );

    engine::ILight* pointLight = iEngine()->createLight( engine::ltPoint );
    engine::IFrame* pointLightFrame = iEngine()->createFrame( "pointLightFrame" );
    pointLight->setFrame( pointLightFrame );
    pointLight->setDiffuseColor( Vector4f( 1, 1, 1, 1 ) );
    pointLight->setSpecularColor( Vector4f( 1, 1, 1, 1 ) );    
	pointLight->setRange( 10000.0f );
    pointLight->setAttenuation( Vector3f( 0, 0.001f, 0 ) );
    pointLightFrame->translate( Vector3f( 1000,1000,1000 ) );
    defaultBSP->add( pointLight );
}

static engine::IBSP* assetEnumerateBSPCB(engine::IBSP* bsp, void* data)
{
    BSPL* bspL = reinterpret_cast<BSPL*>( data );
    bspL->push_back( bsp );
    return bsp;
}

static engine::IClump* assetEnumerateClumpCB(engine::IClump* clump, void* data)
{
    ClumpL* clumpL = reinterpret_cast<ClumpL*>( data );
    clumpL->push_back( clump );
    return clump;
}

const float numFramesPerSecond = 100.0f/3.0f;

#define FRAMETIME(F) ( float(F)/numFramesPerSecond )

void MainForm::preparateAsset(void)
{
    bspList.clear();
    clumpList.clear();

    // enumerate asset internals        
    currentAsset->forAllBSPs( assetEnumerateBSPCB, &bspList );
    currentAsset->forAllClumps( assetEnumerateClumpCB, &clumpList );

    // create default stage world if no worlds present in imported asset
    if( bspList.size() == 0 ) 
    {
        if( !defaultBSP ) createDefaultStage();
        currentBSP = defaultBSP;
    }
    else
    {
        currentBSP = *bspList.begin();
    }

    // add clumps from asset to current stage
    for( ClumpI clumpI = clumpList.begin(); clumpI != clumpList.end(); clumpI++ )
    {
        currentBSP->add( *clumpI );
    }

    if( renderView ) renderView->resetCameraMatrix();

    // build tree of scene objects
    buildAssetTree();
}

void MainForm::buildAssetTree(void)
{
    assetGraph.DeleteAllItems();

    if( assetGraph.GetImageList( TVSIL_NORMAL ) == NULL )
    {
        assetGraph.SetImageList( graphImages, TVSIL_NORMAL );
    }

    insertWorld( currentBSP, TVI_ROOT );
}

static engine::IBSPSector* collectSectorCB(engine::IBSPSector* sector, void* data)
{
    reinterpret_cast<BSPSectorL*>( data )->push_back( sector );
    return sector;
}

static engine::ILight* collectLightCB(engine::ILight* light, void* data)
{
    reinterpret_cast<LightL*>( data )->push_back( light );
    return light;
}

void MainForm::insertWorld(engine::IBSP* bsp, HTREEITEM parentItem)
{
    HTREEITEM bspItem = assetGraph.InsertItem(
        bsp->getName(),
        0, 0,
        parentItem,
        TVI_LAST
    );
    assert( bspItem != NULL );
    assetGraph.SetItemData( bspItem, (DWORD)( bsp ) );

    HTREEITEM sectorFolder = assetGraph.InsertItem(
        "BSP",
        -1, -1,
        bspItem,
        TVI_LAST
    );
    assert( sectorFolder != NULL );

    // collect sectors
    BSPSectorL sectors;
    bsp->forAllSectors( collectSectorCB, &sectors );
    for( BSPSectorI sectorI = sectors.begin(); sectorI != sectors.end(); sectorI++ )
    {
        if( (*sectorI)->isLeaf() )
        {
            insertSector( *sectorI, sectorFolder );
        }
    }

    // collect lights
    LightL lights;
    bsp->forAllLights( collectLightCB, &lights );
    if( lights.size() )
    {
        HTREEITEM lightFolder = assetGraph.InsertItem(
            "Lights",
            -1, -1,
            bspItem,
            TVI_LAST
        );
        assert( lightFolder != NULL );        
        for( LightI lightI = lights.begin(); lightI != lights.end(); lightI++ )
        {
            insertLight( *lightI, lightFolder );
        }
    }

    // add clumps from asset
    for( ClumpI clumpI = clumpList.begin(); clumpI != clumpList.end(); clumpI++ )
    {
        insertClump( *clumpI, bspItem );
    }
}

static engine::IAtomic* collectAtomicCB(engine::IAtomic* atomic, void* data)
{
    reinterpret_cast<AtomicL*>( data )->push_back( atomic );
    return atomic;
}

void MainForm::insertClump(engine::IClump* clump, HTREEITEM parentItem)
{
    HTREEITEM clumpItem = assetGraph.InsertItem(
        clump->getName(),
        1, 1,
        parentItem,
        TVI_LAST
    );
    assert( clumpItem != NULL );
    assetGraph.SetItemData( clumpItem, (DWORD)( clump ) );

    // collect atomics
    AtomicL atomics;
    clump->forAllAtomics( collectAtomicCB, &atomics );
    
    // collect lights
    LightL lights;
    clump->forAllLights( collectLightCB, &lights );

    // insert frames
    insertFrame( clump->getFrame(), clumpItem, atomics, lights );
}

engine::IFrame* collectChildrenFramesCB(engine::IFrame* frame, void* data)
{
    reinterpret_cast<FrameL*>( data )->push_back( frame );
    return frame;
}

void MainForm::insertFrame(engine::IFrame* frame, HTREEITEM parentItem, AtomicL& atomics, LightL& lights)
{
    const char* frameName = frame->getName();
    if( frameName == NULL || strcmp( frameName, "" ) == 0 )
    {
        frameName = "Root";
    }

    HTREEITEM frameItem = assetGraph.InsertItem(
        frameName,
        2, 2,
        parentItem,
        TVI_LAST
    );
    assert( frameItem != NULL );
    assetGraph.SetItemData( frameItem, (DWORD)( frame ) );

    // attach children frames
    FrameL childrenFrames;
    frame->forAllChildren( collectChildrenFramesCB, &childrenFrames );
    for( FrameI frameI = childrenFrames.begin(); frameI != childrenFrames.end(); frameI++ )
    {
        insertFrame( *frameI, frameItem, atomics, lights );
    }

    // find atomics, attached to this frame
    for( AtomicI atomicI = atomics.begin(); atomicI != atomics.end(); atomicI++ )
    {
        if( (*atomicI)->getFrame() == frame )
        {
            insertAtomic( *atomicI, frameItem );
        }
    }

    // find lights, attached to this frame
    for( LightI lightI = lights.begin(); lightI != lights.end(); lightI++ )
    {
        if( (*lightI)->getFrame() == frame )
        {
            insertLight( *lightI, frameItem );
        }
    }
}

void MainForm::insertAtomic(engine::IAtomic* atomic, HTREEITEM parentItem)
{
    HTREEITEM atomicItem = assetGraph.InsertItem(
        strformat( "Atomic 0x%X", (unsigned int)(atomic) ).c_str(),
        3, 3,
        parentItem,
        TVI_LAST
    );
    assert( atomicItem != NULL );
    assetGraph.SetItemData( atomicItem, (DWORD)( atomic ) );

    if( atomic->getGeometry() )
    {
        insertGeometry( atomic->getGeometry(), atomicItem );
    }
}

void MainForm::insertGeometry(engine::IGeometry* geometry, HTREEITEM parentItem)
{
    HTREEITEM geometryItem = assetGraph.InsertItem(
        geometry->getName(),
        4, 4,
        parentItem,
        TVI_LAST
    );
    assert( geometryItem != NULL );
    assetGraph.SetItemData( geometryItem, (DWORD)( geometry ) );

    for( int i=0; i<geometry->getNumShaders(); i++ )
    {
        insertShader( geometry->getShader( i ), geometryItem );
    }
}

void MainForm::insertShader(engine::IShader* shader, HTREEITEM parentItem)
{
    HTREEITEM shaderItem = assetGraph.InsertItem(
        shader->getName(),
        5, 5,
        parentItem,
        TVI_LAST
    );
    assert( shaderItem != NULL );
    assetGraph.SetItemData( shaderItem, (DWORD)( shader ) );
}

void MainForm::insertSector(engine::IBSPSector* sector, HTREEITEM parentItem)
{
    HTREEITEM sectorItem = assetGraph.InsertItem(
        strformat( "Sector %X", (unsigned int)(sector) ).c_str(),
        6, 6,
        parentItem,
        TVI_LAST
    );
    assert( sectorItem != NULL );
    assetGraph.SetItemData( sectorItem, (DWORD)( sector ) );

    // insert geometry
    if( sector->getGeometry() )
    {
        insertGeometry( sector->getGeometry(), sectorItem );
    }
}

void MainForm::insertLight(engine::ILight* light, HTREEITEM parentItem)
{
    std::string lightName;
    switch( light->getType() )
    {
    case engine::ltAmbient: 
        lightName = strformat( "Ambient %X", (unsigned int)(light) );
        break;
    case engine::ltPoint: 
        lightName = strformat( "Point %X", (unsigned int)(light) );
        break;
    case engine::ltSpot: 
        lightName = strformat( "Spot %X", (unsigned int)(light) );
        break;
    case engine::ltDirectional: 
        lightName = strformat( "Directional %X", (unsigned int)(light) );
        break;
    default:
        lightName = strformat( "Unidentified %X", (unsigned int)(light) );
        break;
    }

    HTREEITEM lightItem = assetGraph.InsertItem(
        lightName.c_str(),
        7, 7,
        parentItem,
        TVI_LAST
    );
    assert( lightItem != NULL );
    assetGraph.SetItemData( lightItem, (DWORD)( light ) );
}

/////////////////////////////////////////////////////////////////////////////
// MainForm message handlers

void MainForm::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	alignControls();
}

BOOL MainForm::OnInitDialog() 
{
	CDialog::OnInitDialog();

    // create editor dialogs
    shaderEditor = new ShaderEditor( this );
    shaderEditor->Create( IDD_SHADER_EDITOR, this );

    engineInfo   = new EngineInfo( this );
    engineInfo->Create( IDD_ENGINE_INFO, this );

    geometryEditor = new GeometryEditor( this );
    geometryEditor->Create( IDD_GEOMETRY_EDITOR, this );

    bspEditor = new BSPEditor( this );
    bspEditor->Create( IDD_BSP_EDITOR, this );

    lightEditor = new LightEditor( this );
    lightEditor->Create( IDD_LIGHT_EDITOR, this );

    // initialize timer
    SetTimer( 1, 10,  NULL );
	
	// make shelves
    /*
    shelf.InsertItem( 0, "Game" );
    shelf.InsertItem( 1, "Scene" );

    CButton* button = new CButton;
    RECT r;
    r.left = 4, r.right = 24, r.top = 24, r.bottom = 44;
    button->Create( "Mode", BS_PUSHBUTTON, r, &shelf, 1000 );
    button->ShowWindow( SW_SHOW );
    */
	
	return TRUE;  
}

void makeEngineCompatiblePath(std::string& string)
{
    for( unsigned int i=0; i<string.length(); i++ ) if( string[i] == '\\' ) string[i] = '/';
}

void MainForm::OnAssetImport() 
{
    CFileDialog fileDialog(
        true,
        "*.*",
        NULL,
        0,
        "RenderWare stream (*.rws)|*.rws|All Files (*.*)|*.*||",
        this
    );

    // setup browsing from import directory
    char currentDir[1024];
    GetCurrentDirectory( 1024, currentDir );
    std::string initialDir = currentDir;
    initialDir += "\\res";
    fileDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

    if( fileDialog.DoModal() == IDOK )
    {
        // back to application current directory
        SetCurrentDirectory( currentDir );

        // destroy previous asset
        destroyAsset();

        // import new asset
        std::string fileName = fileDialog.GetPathName();
        makeEngineCompatiblePath( fileName );
        currentAsset = iEngine()->createAsset( engine::atImport, fileName.c_str() );

        // preparate
        preparateAsset();
    }
}

void MainForm::OnHelpEditorhelp() 
{
    if( helpDialog ) helpDialog->ShowWindow( SW_SHOW );    
}

void MainForm::OnAssetSave() 
{
    if( !currentAsset ) return;

	CFileDialog fileDialog(
        false,
        "*.ba",
        NULL,
        0,
        "D3 binary asset (*.ba)|*.ba||",
        this
    );

    // setup browsing from import directory
    char currentDir[1024];
    GetCurrentDirectory( 1024, currentDir );
    std::string initialDir = currentDir;
    initialDir += "\\res";
    fileDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

    if( fileDialog.DoModal() == IDOK )
    {
        // back to application current directory
        SetCurrentDirectory( currentDir );

        // create binary asset
        std::string fileName = fileDialog.GetPathName();
        makeEngineCompatiblePath( fileName );
        engine::IAsset* binaryAsset = iEngine()->createAsset( engine::atBinary, fileName.c_str() );

        // move current asset to binary asset
        binaryAsset->clear();
        binaryAsset->seize( currentAsset );
        currentAsset->release();
        currentAsset = binaryAsset;

        // serialize asset
        currentAsset->serialize();
    }
}

void MainForm::OnAssetOpen() 
{
	CFileDialog fileDialog(
        false,
        "*.*",
        NULL,
        0,
        "D3 binary asset (*.ba)|*.ba|DirectX file (*.x)|*.x||",
        this
    );

    // setup browsing from import directory
    char currentDir[1024];
    GetCurrentDirectory( 1024, currentDir );
    std::string initialDir = currentDir;
    initialDir += "\\res";
    fileDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

    if( fileDialog.DoModal() == IDOK )
    {
        // back to application current directory
        SetCurrentDirectory( currentDir );

        // destroy previous asset
        destroyAsset();

        // choose asset type
        engine::AssetType assetType;
        if( stricmp( fileDialog.GetFileExt(), "ba" ) == 0 )
        {
            assetType = engine::atBinary;
        }
        else if( stricmp( fileDialog.GetFileExt(), "x" ) == 0 )
        {
            assetType = engine::atXFile;
        }
        else
        {
            AfxMessageBox( "Unknown asset type!" );
            return;
        }

        // open new asset
        std::string fileName = fileDialog.GetPathName();
        makeEngineCompatiblePath( fileName );
        currentAsset = iEngine()->createAsset( assetType, fileName.c_str() );

        // preparate
        preparateAsset();
    }
}

static engine::IClump* collectClumpCB(engine::IClump* clump, void* data)
{
    reinterpret_cast<ClumpL*>( data )->push_back( clump );
    return clump;
}

static engine::ILight* rotateLightCB(engine::ILight* light, void* data)
{
    light->getFrame()->rotate( Vector3f( 0,1,0 ), 1 );
    return light;
}

void MainForm::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnTimer(nIDEvent);

    for( ClumpI clumpI = clumpList.begin(); clumpI != clumpList.end(); clumpI++ )
    {
        if( (*clumpI)->getAnimationController() )
        {
            (*clumpI)->getAnimationController()->advance( 10.0f/1000.0f );
        }
    }

    //if( currentBSP ) currentBSP->forAllLights( rotateLightCB, NULL );

    engineInfo->fps = renderView->getFPS();
}

void MainForm::OnDblclkAssetgraph(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // bsp
        case 0:
            bspEditor->bsp = (engine::IBSP*)( assetGraph.GetItemData( selectedItem ) );
            bspEditor->ShowWindow( SW_HIDE );
            bspEditor->ShowWindow( SW_SHOW );
            break;
        // geometry
        case 4:
            geometryEditor->geometry = (engine::IGeometry*)( assetGraph.GetItemData( selectedItem ) );
            geometryEditor->ShowWindow( SW_HIDE );
            geometryEditor->ShowWindow( SW_SHOW );
            break;
        // shader
        case 5:
            shaderEditor->shader = (engine::IShader*)( assetGraph.GetItemData( selectedItem ) );
            shaderEditor->ShowWindow( SW_HIDE );
            shaderEditor->ShowWindow( SW_SHOW );
            break;
        // light
        case 7:
            lightEditor->light = (engine::ILight*)( assetGraph.GetItemData( selectedItem ) );
            lightEditor->ShowWindow( SW_HIDE );
            lightEditor->ShowWindow( SW_SHOW );
            break;
        default:
            break;
        }
    }

    *pResult = 0;
}

void MainForm::OnWindowEngineinfo() 
{
	if( engineInfo )
    {
        if( !engineInfo->IsWindowVisible() )
        {
            engineInfo->ShowWindow( SW_SHOW );
        }
    }
}

static engine::IAtomic* setAtomicFlagsCB(engine::IAtomic* atomic, void* data)
{
    atomic->setFlags( *( (unsigned int*)( data ) ) );
    return atomic;
}

void MainForm::OnClumpHideallatomics()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                unsigned int flags = 0;
                reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->forAllAtomics( setAtomicFlagsCB, &flags );
                break;
            }
        default:
            break;
        }
    }
}

void MainForm::OnAnimationResetanimation()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                unsigned int flags = 0;
                reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->getAnimationController()->resetTrackTime( 0 );
                break;
            }
        default:
            break;
        }
    }    
}

void MainForm::OnSpeed1x()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                unsigned int flags = 0;
                reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->getAnimationController()->setTrackSpeed( 0, 1.0f );
                break;
            }
        default:
            break;
        }
    }    
}

void MainForm::OnSpeed0()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                unsigned int flags = 0;
                reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->getAnimationController()->setTrackSpeed( 0, 0.5f );
                break;
            }
        default:
            break;
        }
    }  
}

void MainForm::OnSpeed025x()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                unsigned int flags = 0;
                reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->getAnimationController()->setTrackSpeed( 0, 0.25f );
                break;
            }
        default:
            break;
        }
    } 
}

void MainForm::OnGeneratorGeometrydetails()
{
}

void MainForm::OnClumpHideselectedatomic()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // atomic
        case 3:
            {
                reinterpret_cast<engine::IAtomic*>( assetGraph.GetItemData( selectedItem ) )->setFlags( 0 );
                break;
            }
        default:
            break;
        }
    }
}

typedef std::list<engine::IGeometry*> GeometryL;
typedef GeometryL::iterator GeometryI;

engine::IAtomic* collectAtomicGeometryCB(engine::IAtomic* atomic, void* data)
{
    GeometryL* geometries = (GeometryL*)( data );
    for( GeometryI geometryI = geometries->begin();
                   geometryI != geometries->end();
                   geometryI++ )
    {
        if( *geometryI == atomic->getGeometry() ) return atomic;
    }
    geometries->push_back( atomic->getGeometry() );
    return atomic;
}

void MainForm::OnClumpForceprelight()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                // collect geometries
                GeometryL geometries;
                reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->forAllAtomics( collectAtomicGeometryCB, &geometries );
                // ask color
                CColorDialog* colorDlg = new CColorDialog( 0, CC_ANYCOLOR | CC_FULLOPEN, this );
                if( colorDlg->DoModal() )
                {
                    Vector4f fogColor;
                    fogColor[0] = float( GetRValue( colorDlg->GetColor() ) ) / 255;
                    fogColor[1] = float( GetGValue( colorDlg->GetColor() ) ) / 255;
                    fogColor[2] = float( GetBValue( colorDlg->GetColor() ) ) / 255;
                    fogColor[3] = 1.0f;
                    for( GeometryI geometryI = geometries.begin();
                                   geometryI != geometries.end();
                                   geometryI++ )
                    {
                        (*geometryI)->forcePrelight( fogColor );
                    }
                }
                delete colorDlg;
                break;
            }
        default:
            break;
        }
    }
}

static bool         _alphaTest = false;
static unsigned int _alphaTestRef = 32;

static engine::IAtomic* atomicPanalphaCB(engine::IAtomic* atomic, void* data)
{
    engine::IGeometry* geometry = atomic->getGeometry();
    unsigned int numShaders = geometry->getNumShaders();
    for( unsigned int i=0; i<numShaders; i++ )
    {
        unsigned int flags = geometry->getShader( i )->getFlags();
        flags |= engine::sfAlphaBlending;
        flags &= ~engine::sfCulling;
        if( _alphaTest ) flags |= engine::sfAlphaTesting;        
        geometry->getShader( i )->setFlags( flags );
        if( _alphaTest ) 
        {
            geometry->getShader( i )->setAlphaTestFunction( engine::cfGreater );
            geometry->getShader( i )->setAlphaTestRef( _alphaTestRef );
        }
    }
    return atomic;
}

void MainForm::OnEnableAlphaTree()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            _alphaTest    = true;
            _alphaTestRef = 16;
            reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->forAllAtomics( atomicPanalphaCB, NULL );
            break;        
        default:
            break;
        }
    }    
}

void MainForm::OnEnableAlphaBanister()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            _alphaTest    = true;
            _alphaTestRef = 2;
            reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->forAllAtomics( atomicPanalphaCB, NULL );
            break;        
        default:
            break;
        }
    }
}

static engine::IAtomic* atomicPancullCB(engine::IAtomic* atomic, void* data)
{
    engine::IGeometry* geometry = atomic->getGeometry();
    unsigned int numShaders = geometry->getNumShaders();
    for( unsigned int i=0; i<numShaders; i++ )
    {
        unsigned int flags = geometry->getShader( i )->getFlags();
        flags &= ~engine::sfCulling;
        geometry->getShader( i )->setFlags( flags );
    }
    return atomic;
}

void MainForm::OnEnableNoCull()
{
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) )->forAllAtomics( atomicPancullCB, NULL );
            break;        
        default:
            break;
        }
    }
}

void MainForm::OnToolsCreatebumpmap()
{
    CFileDialog fileDialog(
        true,
        "*.dds",
        NULL,
        0,
        "DirectX texture (*.dds)|*.dds|All Files (*.*)|*.*||",
        this
    );    

    // setup browsing from resource directory
    char currentDir[1024];
    GetCurrentDirectory( 1024, currentDir );
    std::string initialDir = currentDir;
    initialDir += "\\res";
    fileDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

    if( fileDialog.DoModal() == IDOK )
    {
        // back to application current directory
        SetCurrentDirectory( currentDir );

        std::string fileName = fileDialog.GetPathName();
        makeEngineCompatiblePath( fileName );

        // load texture
        engine::IEngine* iEngine;
        queryInterfaceT( "Engine", &iEngine );
        engine::ITexture* texture = iEngine->createTexture( fileName.c_str() );
        assert( texture );

        // create DUDV
        engine::ITexture* dudv = iEngine->createDUDVFromNormalMap( texture, "DUDVTEMP" );

        CFileDialog saveDialog(
            false,
            "*.dds",
            NULL,
            0,
            "DirectX texture (*.dds)|*.dds|All Files (*.*)|*.*||",
            this
        );    

        // setup browsing from resource directory
        GetCurrentDirectory( 1024, currentDir );
        std::string initialDir = currentDir;
        initialDir += "\\res";
        saveDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

        if( saveDialog.DoModal() == IDOK )
        {
            // back to application current directory
            SetCurrentDirectory( currentDir );

            fileName = saveDialog.GetPathName();
            makeEngineCompatiblePath( fileName );
            dudv->save( fileName.c_str() );
        }

        dudv->release();
        texture->release();
    }
}

void MainForm::OnToolsNormalmapfromtexture()
{
    CFileDialog fileDialog(
        true,
        "*.*",
        NULL,
        0,
        "All Files (*.*)|*.*||",
        this
    );    

    // setup browsing from resource directory
    char currentDir[1024];
    GetCurrentDirectory( 1024, currentDir );
    std::string initialDir = currentDir;
    initialDir += "\\res";
    fileDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

    if( fileDialog.DoModal() == IDOK )
    {
        // back to application current directory
        SetCurrentDirectory( currentDir );

        std::string resourceName1 = fileDialog.GetPathName();
        makeEngineCompatiblePath( resourceName1 );

        CFileDialog saveDialog(
            false,
            "*.bmp",
            NULL,
            0,
            "Windows bitmap (*.bmp)|*.bmp||",
            this
        );    

        // setup browsing from resource directory
        GetCurrentDirectory( 1024, currentDir );
        std::string initialDir = currentDir;
        initialDir += "\\res";
        saveDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

        if( saveDialog.DoModal() == IDOK )
        {
            // back to application current directory
            SetCurrentDirectory( currentDir );

            std::string resourceName2 = saveDialog.GetPathName();
            makeEngineCompatiblePath( resourceName2 );

            import::IImport* iImport;
            queryInterfaceT( "Import", &iImport );
            iImport->createNormalMap( resourceName1.c_str(), resourceName2.c_str(), 1.0f );
        }
    }
}

void MainForm::OnSetNumTextureLayers0()
{
    setNumberOfTextureLayers( 0 );
}

void MainForm::OnSetNumTextureLayers1()
{
    setNumberOfTextureLayers( 1 );
}

void MainForm::OnSetNumTextureLayers2()
{
    setNumberOfTextureLayers( 2 );
}

void MainForm::OnSetNumTextureLayers3()
{
    setNumberOfTextureLayers( 3 );
}

void MainForm::OnSetNumTextureLayers4()
{
    setNumberOfTextureLayers( 4 );
}

void MainForm::setNumberOfTextureLayers(unsigned int numLayers)
{
    // close shader editor
    shaderEditor->ShowWindow( SW_HIDE );

    // retrieve selected item
    engine::IShader* shader = NULL;
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // shader
        case 5:
            {
                shader = reinterpret_cast<engine::IShader*>( assetGraph.GetItemData( selectedItem ) );
                break;
            }
        default:
            break;
        }
    }

    // no shader? - break function
    if( shader == NULL ) return;

    engine::IEngine* iEngine;
    queryInterfaceT( "Engine", &iEngine );

    // seacrh for white texture
    engine::ITexture* white = iEngine->getTexture( "white" );
    assert( white );

    // set number of textures
    shader->setNumLayers( numLayers, white );
}

void MainForm::OnToolsCalculateLightMaps()
{
    CFileDialog fileDialog(
        true,
        "*.*",
        NULL,
        0,
        "RenderWare stream (*.rws)|*.rws|All Files (*.*)|*.*||",
        this
    );

    // setup browsing from import directory
    char currentDir[1024];
    GetCurrentDirectory( 1024, currentDir );
    std::string initialDir = currentDir;
    initialDir += "\\res";
    fileDialog.m_ofn.lpstrInitialDir = initialDir.c_str();

    if( fileDialog.DoModal() == IDOK )
    {
        // back to application current directory
        SetCurrentDirectory( currentDir );

        // resource path
        std::string fileName = fileDialog.GetPathName();
        makeEngineCompatiblePath( fileName );

        // calculate lightmaps
        import::IImport* iImport;
        queryInterfaceT( "Import", &iImport );
        iImport->calculateLightMaps( fileName.c_str(), lightMapCallback, this );
    }
}

void MainForm::lightMapCallback(float progress, void* userData)
{
    MainForm* __this = reinterpret_cast<MainForm*>( userData );

    std::string text = strformat( "Calculate lightmaps...%3.2f%%", progress * 100 );
    __this->consoleBox.SetWindowText( text.c_str() );

    MSG msg;
    if( PeekMessage( &msg, __this->m_hWnd, 0, 0, PM_REMOVE ) )
    {
        DispatchMessage( &msg );
    }
}
void MainForm::OnClumpDelete()
{
    // retrieve selected item
    engine::IClump* clump = NULL;
    HTREEITEM selectedItem = assetGraph.GetSelectedItem();
    if( selectedItem )
    {
        int itemType;
        assetGraph.GetItemImage( selectedItem, itemType, itemType );
        switch( itemType )
        {
        // clump
        case 1:
            {
                clump = reinterpret_cast<engine::IClump*>( assetGraph.GetItemData( selectedItem ) );
                break;
            }
        default:
            break;
        }
    }    
    if( clump )
    {
        if( clump->getBSP() ) clump->getBSP()->remove( clump );
        currentAsset->remove( clump );
        clump->release();

        clumpList.clear();
        currentAsset->forAllClumps( assetEnumerateClumpCB, &clumpList );
    }
}
