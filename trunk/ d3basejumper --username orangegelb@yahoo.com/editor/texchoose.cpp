// texchoose.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "texchoose.h"
#include "../common/istring.h"
#include ".\texchoose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// type wrappers

static const char* wrap(engine::FilterType value)
{
    switch( value )
    {
    case engine::ftNone: return "None";
    case engine::ftPoint: return "Point";
    case engine::ftLinear: return "Linear";
    case engine::ftAnisotropic: return "Anisotropic";
    case engine::ftPyramidal: return "Pyramidal";
    case engine::ftGaussian: return "Gaussian";
    default:
        assert( !"shouldn't be here!" );
        return "Point";
    }
}

static const char* wrap(engine::AddressType value)
{
    switch( value )
    {
    case engine::atWrap: return "Wrap";
    case engine::atMirror: return "Mirror";
    case engine::atClamp: return "Clamp";
    case engine::atBorder: return "Border";
    default:
        assert( !"shouldn't be here!" );
        return "Wrap";
    }
}

static engine::FilterType wrapFilter(const char* value)
{
    if( strcmp( value, "None" ) == 0 ) return engine::ftNone;
    else if( strcmp( value, "Point" ) == 0 ) return engine::ftPoint;
    else if( strcmp( value, "Linear" ) == 0 ) return engine::ftLinear;
    else if( strcmp( value, "Anisotropic" ) == 0 ) return engine::ftAnisotropic;
    else if( strcmp( value, "Pyramidal" ) == 0 ) return engine::ftPyramidal;
    else if( strcmp( value, "Gaussian" ) == 0 ) return engine::ftGaussian;
    assert( !"shouldn't be here!" );
    return engine::ftPoint;
}

static engine::AddressType wrapAddress(const char* value)
{
    if( strcmp( value, "Wrap" ) == 0 ) return engine::atWrap;
    else if( strcmp( value, "Mirror" ) == 0 ) return engine::atMirror;
    else if( strcmp( value, "Clamp" ) == 0 ) return engine::atClamp;
    else if( strcmp( value, "Border" ) == 0 ) return engine::atBorder;
    return engine::atWrap;
}

/////////////////////////////////////////////////////////////////////////////
// TextureChooser dialog


TextureChooser::TextureChooser(CWnd* pParent /*=NULL*/)
	: CDialog(TextureChooser::IDD, pParent)
{
    state = tcsDontCare;
    isModified = false;
    isTextureChanged = false;

	//{{AFX_DATA_INIT(TextureChooser)
	//}}AFX_DATA_INIT
}

bool TextureChooser::validateControls(void)
{
    CString windowText;
    maxAnisotropy.GetWindowText( windowText );
    if( windowText.IsEmpty() ) return false;
    if( !strIsUnsignedInt( windowText ) ) return false;
    mipmapLODBias.GetWindowText( windowText );
    if( windowText.IsEmpty() ) return false;    
    return true;
}

void TextureChooser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TextureChooser)
	DDX_Control(pDX, IDC_VADDRESSING, vAddressing);
	DDX_Control(pDX, IDC_UADDRESSING, uAddressing);
	DDX_Control(pDX, IDC_ANISOTROPY, maxAnisotropy);
	DDX_Control(pDX, IDC_MIPFILTER, mipFilter);
	DDX_Control(pDX, IDC_MINFILTER, minFilter);
	DDX_Control(pDX, IDC_MAGFILTER, magFilter);
	DDX_Control(pDX, IDC_COMBO1, textureName);
    DDX_Control(pDX, IDC_MIPMAPLODBIAS, mipmapLODBias);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TextureChooser, CDialog)
	//{{AFX_MSG_MAP(TextureChooser)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_CBN_EDITUPDATE(IDC_COMBO1, OnEditupdateCombo1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_CBN_SELCHANGE(IDC_MAGFILTER, OnSelchangeMagfilter)
	ON_CBN_SELCHANGE(IDC_MINFILTER, OnSelchangeMinfilter)
	ON_CBN_SELCHANGE(IDC_MIPFILTER, OnSelchangeMipfilter)
	ON_EN_CHANGE(IDC_ANISOTROPY, OnChangeAnisotropy)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_UADDRESSING, OnSelchangeUaddressing)
	ON_CBN_SELCHANGE(IDC_VADDRESSING, OnSelchangeVaddressing)
    ON_EN_CHANGE(IDC_MIPMAPLODBIAS, OnEnChangeMipmaplodbias)
	//}}AFX_MSG_MAP    
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TextureChooser message handlers

void TextureChooser::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    state = tcsDontCare;

    textureName.ResetContent();

    int numTextures = iEngine()->getNumTextures();
    for( int i=0; i<numTextures; i++ )
    {
        if( ( strcmp( iEngine()->getTexture( i )->getName(), "defaultColorBuffer" ) != 0 ) &&
            ( strcmp( iEngine()->getTexture( i )->getName(), "defaultDepthBuffer" ) != 0 ) )
        {
            textureName.AddString( iEngine()->getTexture( i )->getName() );
        }
    }

    if( texture )
    {
        magFilter.EnableWindow( true );
        minFilter.EnableWindow( true );
        mipFilter.EnableWindow( true );
        maxAnisotropy.EnableWindow( true );
        mipmapLODBias.EnableWindow( true );

        textureName.SelectString( -1, texture->getName() );

        magFilter.SelectString( -1, wrap( texture->getMagFilter() ) );
        minFilter.SelectString( -1, wrap( texture->getMinFilter() ) );
        mipFilter.SelectString( -1, wrap( texture->getMipFilter() ) );
        maxAnisotropy.SetWindowText( strformat( "%d", texture->getMaxAnisotropy() ).c_str() );
        mipmapLODBias.SetWindowText( strformat( "%6.3f", texture->getMipmapLODBias() ).c_str() );

        uAddressing.EnableWindow( true );
        vAddressing.EnableWindow( true );

        uAddressing.SelectString( -1, wrap( texture->getAddressTypeU() ) );
        vAddressing.SelectString( -1, wrap( texture->getAddressTypeV() ) );
    }
    else
    {
        magFilter.EnableWindow( false );
        minFilter.EnableWindow( false );
        mipFilter.EnableWindow( false );
        maxAnisotropy.EnableWindow( false );
        mipmapLODBias.EnableWindow( false );
        uAddressing.EnableWindow( false );
        vAddressing.EnableWindow( false );
    }

	CDialog::OnShowWindow(bShow, nStatus);
}

void TextureChooser::OnOK() 
{
    CString textureNameText;
    textureName.GetWindowText( textureNameText );
	texture = iEngine()->getTexture( textureNameText );

    state = tcsOk;

	CDialog::OnOK();
}

void TextureChooser::OnSelchangeCombo1() 
{
    isTextureChanged = true;
}

void TextureChooser::OnEditupdateCombo1() 
{
}

void TextureChooser::OnButton1() 
{
	CFileDialog fileDialog(
        false,
        "*.dds",
        NULL,
        0,
        "DirectX texture (*.dds)|*.dds||",
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

        std::string fileName = fileDialog.GetPathName();
        texture = iEngine()->createTexture( fileName.c_str() );

        textureName.ResetContent();

        int numTextures = iEngine()->getNumTextures();
        for( int i=0; i<numTextures; i++ )
        {
            if( ( strcmp( iEngine()->getTexture( i )->getName(), "defaultColorBuffer" ) != 0 ) &&
                ( strcmp( iEngine()->getTexture( i )->getName(), "defaultDepthBuffer" ) != 0 ) )
            {
                textureName.AddString( iEngine()->getTexture( i )->getName() );
            }
        }
        textureName.SelectString( -1, texture->getName() );
    }
}

BOOL TextureChooser::OnInitDialog() 
{
	CDialog::OnInitDialog();

    SetTimer( 1, 100,  NULL );
	
	magFilter.ResetContent();
    magFilter.AddString( "Point" );
    magFilter.AddString( "Linear" );
    magFilter.AddString( "Pyramidal" );
    magFilter.AddString( "Gaussian" ); 

    minFilter.ResetContent();
    minFilter.AddString( "Point" );
    minFilter.AddString( "Linear" );
    minFilter.AddString( "Anisotropic" );
    minFilter.AddString( "Pyramidal" );
    minFilter.AddString( "Gaussian" ); 

    mipFilter.ResetContent();
    mipFilter.AddString( "None" );
    mipFilter.AddString( "Point" );
    mipFilter.AddString( "Linear" );

    uAddressing.ResetContent();
    uAddressing.AddString( "Wrap" );
    uAddressing.AddString( "Mirror" );
    uAddressing.AddString( "Clamp" );
    uAddressing.AddString( "Border" );

    vAddressing.ResetContent();
    vAddressing.AddString( "Wrap" );
    vAddressing.AddString( "Mirror" );
    vAddressing.AddString( "Clamp" );
    vAddressing.AddString( "Border" );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TextureChooser::OnSelchangeMagfilter() 
{
	isModified = true;
}

void TextureChooser::OnSelchangeMinfilter() 
{
	isModified = true;
}

void TextureChooser::OnSelchangeMipfilter() 
{
	isModified = true;
}

void TextureChooser::OnChangeAnisotropy() 
{
	isModified = true;
}

void TextureChooser::OnTimer(UINT nIDEvent) 
{
    if( nIDEvent == 1 && isTextureChanged )
    {
        isTextureChanged = false;
        CString textureNameText;
        textureName.GetWindowText( textureNameText );
        texture = iEngine()->getTexture( textureNameText );
        
        if( texture )
        {
            magFilter.EnableWindow( true );
            minFilter.EnableWindow( true );
            mipFilter.EnableWindow( true );
            maxAnisotropy.EnableWindow( true );
            mipmapLODBias.EnableWindow( true );
            magFilter.SelectString( -1, wrap( texture->getMagFilter() ) );
            minFilter.SelectString( -1, wrap( texture->getMinFilter() ) );
            mipFilter.SelectString( -1, wrap( texture->getMipFilter() ) );
            maxAnisotropy.SetWindowText( strformat( "%d", texture->getMaxAnisotropy() ).c_str() );
            mipmapLODBias.SetWindowText( strformat( "%6.3f", texture->getMipmapLODBias() ).c_str() );
            uAddressing.EnableWindow( true );
            uAddressing.SelectString( -1, wrap( texture->getAddressTypeU() ) );
            vAddressing.EnableWindow( true );
            vAddressing.SelectString( -1, wrap( texture->getAddressTypeV() ) );
        }
        else
        {
            magFilter.EnableWindow( false );
            minFilter.EnableWindow( false );
            mipFilter.EnableWindow( false );
            maxAnisotropy.EnableWindow( false );
            mipmapLODBias.EnableWindow( false );

            uAddressing.EnableWindow( false );
            vAddressing.EnableWindow( false );
        }
    }

	if( nIDEvent == 1 && isModified && validateControls() )
    {
        isModified = false;

        if( texture )
        {
            CString windowText;
            magFilter.GetWindowText( windowText );
            texture->setMagFilter( wrapFilter( windowText ) );
            
            minFilter.GetWindowText( windowText );
            texture->setMinFilter( wrapFilter( windowText ) );
            
            mipFilter.GetWindowText( windowText );
            texture->setMipFilter( wrapFilter( windowText ) );

            maxAnisotropy.GetWindowText( windowText );
            texture->setMaxAnisotropy( atoi( windowText ) );

            mipmapLODBias.GetWindowText( windowText );
            texture->setMipmapLODBias( float( atof( windowText ) ) );

            uAddressing.GetWindowText( windowText );
            texture->setAddressTypeU( wrapAddress( windowText ) );

            vAddressing.GetWindowText( windowText );
            texture->setAddressTypeV( wrapAddress( windowText ) );
        }
    }
	
	CDialog::OnTimer(nIDEvent);
}

void TextureChooser::OnSelchangeUaddressing() 
{
	isModified = true;
}

void TextureChooser::OnSelchangeVaddressing() 
{
	isModified = true;
}

void TextureChooser::OnEnChangeMipmaplodbias()
{
    isModified = true;
}
