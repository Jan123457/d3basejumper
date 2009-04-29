// shaderedit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "shaderedit.h"
#include "../common/istring.h"
#include ".\shaderedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ShaderEditor dialog


ShaderEditor::ShaderEditor(CWnd* pParent /*=NULL*/)
	: CDialog(ShaderEditor::IDD, pParent)
    , lightset(0)
{
	//{{AFX_DATA_INIT(ShaderEditor)
	flagAlphaBlending = FALSE;
	flagAlphaTesting = FALSE;
	flagCollision = FALSE;
	flagCulling = FALSE;
	alphaBlendingDst = _T("");
	alphaBlendingOp = _T("");
	alphaBlendingSrc = _T("");
	alphaTestingFunct = _T("");
	diffuseA = 0.0f;
	diffuseB = 0.0f;
	diffuseG = 0.0f;
	diffuseR = 0.0f;
	effectName = _T("");
	layer0Blending = _T("");
	layer0ConstantA = 0.0f;
	layer0ConstantB = 0.0f;
	layer0ConstantG = 0.0f;
	layer0ConstantR = 0.0f;
	layer0UVID = _T("");
	layer1Blending = _T("");
	layer1ConstantA = 0.0f;
	layer1ConstantB = 0.0f;
	layer1ConstantG = 0.0f;
	layer1ConstantR = 0.0f;
	layer1UVID = _T("");
	layer2Blending = _T("");
	layer2ConstantA = 0.0f;
	layer2ConstantB = 0.0f;
	layer2ConstantG = 0.0f;
	layer2ConstantR = 0.0f;
	layer2UVID = _T("");
	layer3Blending = _T("");
	layer3ConstantA = 0.0f;
	layer3ConstantB = 0.0f;
	layer3ConstantG = 0.0f;
	layer3ConstantR = 0.0f;
	layer3UVID = _T("");
	normalMapUVID = _T("");
	pixelArgumentA = 0.0f;
	pixelArgumentB = 0.0f;
	pixelArgumentG = 0.0f;
	pixelArgumentName = _T("");
	pixelArgumentR = 0.0f;
	specularA = 0.0f;
	specularB = 0.0f;
	specularG = 0.0f;
	specularPower = 0.0f;
	specularR = 0.0f;
	vertexArgumentA = 0.0f;
	vertexArgumentB = 0.0f;
	vertexArgumentG = 0.0f;
	vertexArgumentName = _T("");
	vertexArgumentR = 0.0f;
	alphaTestingRef = 0;
    lightset = 0;
	//}}AFX_DATA_INIT

    shader = NULL;
    colorDlg = NULL;
    isModified = false;
}


void ShaderEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ShaderEditor)
	DDX_Check(pDX, IDC_FLAG_ALPHA_BLENDING, flagAlphaBlending);
	DDX_Check(pDX, IDC_FLAG_ALPHA_TESTING, flagAlphaTesting);
	DDX_Check(pDX, IDC_FLAG_COLLISION, flagCollision);
	DDX_Check(pDX, IDC_FLAG_CULLING, flagCulling);
	DDX_CBString(pDX, IDC_ALPHA_BLENDING_DST, alphaBlendingDst);
	DDX_CBString(pDX, IDC_ALPHA_BLENDING_OP, alphaBlendingOp);
	DDX_CBString(pDX, IDC_ALPHA_BLENDING_SRC, alphaBlendingSrc);
	DDX_CBString(pDX, IDC_ALPHA_TESTING_FUNCT, alphaTestingFunct);
	DDX_Text(pDX, IDC_DIFFUSE_A, diffuseA);
	DDX_Text(pDX, IDC_DIFFUSE_B, diffuseB);
	DDX_Text(pDX, IDC_DIFFUSE_G, diffuseG);
	DDX_Text(pDX, IDC_DIFFUSE_R, diffuseR);
	DDX_CBString(pDX, IDC_LAYER_0_BLENDING, layer0Blending);
	DDX_Text(pDX, IDC_LAYER_0_CONSTANT_A, layer0ConstantA);
	DDX_Text(pDX, IDC_LAYER_0_CONSTANT_B, layer0ConstantB);
	DDX_Text(pDX, IDC_LAYER_0_CONSTANT_G, layer0ConstantG);
	DDX_Text(pDX, IDC_LAYER_0_CONSTANT_R, layer0ConstantR);
	DDX_CBString(pDX, IDC_LAYER_0_UVID, layer0UVID);
	DDX_CBString(pDX, IDC_LAYER_1_BLENDING, layer1Blending);
	DDX_Text(pDX, IDC_LAYER_1_CONSTANT_A, layer1ConstantA);
	DDX_Text(pDX, IDC_LAYER_1_CONSTANT_B, layer1ConstantB);
	DDX_Text(pDX, IDC_LAYER_1_CONSTANT_G, layer1ConstantG);
	DDX_Text(pDX, IDC_LAYER_1_CONSTANT_R, layer1ConstantR);
	DDX_CBString(pDX, IDC_LAYER_1_UVID, layer1UVID);
	DDX_CBString(pDX, IDC_LAYER_2_BLENDING, layer2Blending);
	DDX_Text(pDX, IDC_LAYER_2_CONSTANT_A, layer2ConstantA);
	DDX_Text(pDX, IDC_LAYER_2_CONSTANT_B, layer2ConstantB);
	DDX_Text(pDX, IDC_LAYER_2_CONSTANT_G, layer2ConstantG);
	DDX_Text(pDX, IDC_LAYER_2_CONSTANT_R, layer2ConstantR);
	DDX_CBString(pDX, IDC_LAYER_2_UVID, layer2UVID);
	DDX_CBString(pDX, IDC_LAYER_3_BLENDING, layer3Blending);
	DDX_Text(pDX, IDC_LAYER_3_CONSTANT_A, layer3ConstantA);
	DDX_Text(pDX, IDC_LAYER_3_CONSTANT_B, layer3ConstantB);
	DDX_Text(pDX, IDC_LAYER_3_CONSTANT_G, layer3ConstantG);
	DDX_Text(pDX, IDC_LAYER_3_CONSTANT_R, layer3ConstantR);
	DDX_CBString(pDX, IDC_LAYER_3_UVID, layer3UVID);
	DDX_CBString(pDX, IDC_NORMAL_MAP_UVID, normalMapUVID);	
	DDX_Text(pDX, IDC_SPECULAR_A, specularA);
	DDX_Text(pDX, IDC_SPECULAR_B, specularB);
	DDX_Text(pDX, IDC_SPECULAR_G, specularG);
	DDX_Text(pDX, IDC_SPECULAR_POWER, specularPower);
	DDX_Text(pDX, IDC_SPECULAR_R, specularR);	
	DDX_Text(pDX, IDC_ALPHA_TESTING_REF, alphaTestingRef);
    DDX_Text(pDX, IDC_LIGHTSET, lightset);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ShaderEditor, CDialog)
	//{{AFX_MSG_MAP(ShaderEditor)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_FLAG_CULLING, OnFlagCulling)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FLAG_COLLISION, OnFlagCollision)
	ON_BN_CLICKED(IDC_FLAG_ALPHA_BLENDING, OnFlagAlphaBlending)
	ON_BN_CLICKED(IDC_FLAG_ALPHA_TESTING, OnFlagAlphaTesting)
	ON_CBN_SELCHANGE(IDC_ALPHA_BLENDING_SRC, OnSelchangeAlphaBlendingSrc)
	ON_CBN_SELCHANGE(IDC_ALPHA_BLENDING_OP, OnSelchangeAlphaBlendingOp)
	ON_CBN_SELCHANGE(IDC_ALPHA_BLENDING_DST, OnSelchangeAlphaBlendingDst)
	ON_CBN_SELCHANGE(IDC_ALPHA_TESTING_FUNCT, OnSelchangeAlphaTestingFunct)
	ON_EN_CHANGE(IDC_ALPHA_TESTING_REF, OnChangeAlphaTestingRef)
	ON_EN_CHANGE(IDC_DIFFUSE_R, OnChangeDiffuseR)
	ON_EN_CHANGE(IDC_DIFFUSE_G, OnChangeDiffuseG)
	ON_EN_CHANGE(IDC_DIFFUSE_B, OnChangeDiffuseB)
	ON_EN_CHANGE(IDC_DIFFUSE_A, OnChangeDiffuseA)
	ON_EN_CHANGE(IDC_SPECULAR_R, OnChangeSpecularR)
	ON_EN_CHANGE(IDC_SPECULAR_G, OnChangeSpecularG)
	ON_EN_CHANGE(IDC_SPECULAR_B, OnChangeSpecularB)
	ON_EN_CHANGE(IDC_SPECULAR_A, OnChangeSpecularA)
	ON_EN_CHANGE(IDC_SPECULAR_POWER, OnChangeSpecularPower)
	ON_BN_CLICKED(IDC_DIFFUSE_BTN, OnDiffuseBtn)
	ON_BN_CLICKED(IDC_SPECULAR_BTN, OnSpecularBtn)
	ON_BN_CLICKED(IDC_LAYER_0_TEXTURE_BTN, OnLayer0TextureBtn)
	ON_BN_CLICKED(IDC_LAYER_1_TEXTURE_BTN, OnLayer1TextureBtn)
	ON_CBN_SELCHANGE(IDC_LAYER_0_UVID, OnSelchangeLayer0Uvid)
	ON_CBN_SELCHANGE(IDC_LAYER_1_UVID, OnSelchangeLayer1Uvid)
	ON_CBN_SELCHANGE(IDC_LAYER_2_UVID, OnSelchangeLayer2Uvid)
	ON_CBN_SELCHANGE(IDC_LAYER_3_UVID, OnSelchangeLayer3Uvid)
	ON_CBN_SELCHANGE(IDC_LAYER_0_BLENDING, OnSelchangeLayer0Blending)
	ON_CBN_SELCHANGE(IDC_LAYER_1_BLENDING, OnSelchangeLayer1Blending)
	ON_CBN_SELCHANGE(IDC_LAYER_2_BLENDING, OnSelchangeLayer2Blending)
	ON_CBN_SELCHANGE(IDC_LAYER_3_BLENDING, OnSelchangeLayer3Blending)
	ON_EN_CHANGE(IDC_LAYER_0_CONSTANT_R, OnChangeLayer0ConstantR)
	ON_EN_CHANGE(IDC_LAYER_0_CONSTANT_G, OnChangeLayer0ConstantG)
	ON_EN_CHANGE(IDC_LAYER_0_CONSTANT_B, OnChangeLayer0ConstantB)
	ON_EN_CHANGE(IDC_LAYER_0_CONSTANT_A, OnChangeLayer0ConstantA)
	ON_EN_CHANGE(IDC_LAYER_1_CONSTANT_R, OnChangeLayer1ConstantR)
	ON_EN_CHANGE(IDC_LAYER_1_CONSTANT_G, OnChangeLayer1ConstantG)
	ON_EN_CHANGE(IDC_LAYER_1_CONSTANT_B, OnChangeLayer1ConstantB)
	ON_EN_CHANGE(IDC_LAYER_1_CONSTANT_A, OnChangeLayer1ConstantA)
	ON_EN_CHANGE(IDC_LAYER_2_CONSTANT_R, OnChangeLayer2ConstantR)
	ON_EN_CHANGE(IDC_LAYER_2_CONSTANT_G, OnChangeLayer2ConstantG)
	ON_EN_CHANGE(IDC_LAYER_2_CONSTANT_B, OnChangeLayer2ConstantB)
	ON_EN_CHANGE(IDC_LAYER_2_CONSTANT_A, OnChangeLayer2ConstantA)
	ON_EN_CHANGE(IDC_LAYER_3_CONSTANT_R, OnChangeLayer3ConstantR)
	ON_EN_CHANGE(IDC_LAYER_3_CONSTANT_G, OnChangeLayer3ConstantG)
	ON_EN_CHANGE(IDC_LAYER_3_CONSTANT_B, OnChangeLayer3ConstantB)
	ON_EN_CHANGE(IDC_LAYER_3_CONSTANT_A, OnChangeLayer3ConstantA)
	ON_BN_CLICKED(IDC_NORMAL_MAP_BTN, OnNormalMapBtn)
	ON_CBN_SELCHANGE(IDC_NORMAL_MAP_UVID, OnSelchangeNormalMapUvid)
	ON_BN_CLICKED(IDC_EFFECT, OnEffect)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_ENV_MAP_BTN, OnBnClickedEnvMapBtn)
    ON_EN_CHANGE(IDC_LIGHTSET, OnEnChangeLightset)
    ON_BN_CLICKED(IDC_LAYER_2_TEXTURE_BTN, OnBnClickedLayer2TextureBtn)
    ON_BN_CLICKED(IDC_LAYER_3_TEXTURE_BTN, OnBnClickedLayer3TextureBtn)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ShaderEditor behaviour

static const char* wrap(engine::BlendingMode value)
{
    switch( value )
    {
    case engine::bmZero: return "Zero";
    case engine::bmOne: return "One";
    case engine::bmSrcColor: return "SrcColor";
    case engine::bmInvSrcColor: return "InvSrcColor";
    case engine::bmSrcAlpha: return "SrcAlpha";
    case engine::bmInvSrcAlpha: return "InvSrcAlpha";
    case engine::bmDestAlpha: return "DestAlpha";
    case engine::bmInvDestAlpha: return "InvDestAlpha";
    case engine::bmDestColor: return "DestColor";
    case engine::bmInvDestColor: return "InvDestColor";
    case engine::bmSrcAlphaSat: return "SrcAlphaSat";
    default:
        assert( !"shouldn't be here!" );
        return "Zero";
    }
}

static engine::BlendingMode wrapMode(const char* value)
{
    if( strcmp( value, "Zero" ) == 0 ) return engine::bmZero;
    else if( strcmp( value, "One" ) == 0 ) return engine::bmOne;
    else if( strcmp( value, "SrcColor" ) == 0 ) return engine::bmSrcColor;
    else if( strcmp( value, "InvSrcColor" ) == 0 ) return engine::bmInvSrcColor;
    else if( strcmp( value, "SrcAlpha" ) == 0 ) return engine::bmSrcAlpha;
    else if( strcmp( value, "InvSrcAlpha" ) == 0 ) return engine::bmInvSrcAlpha;
    else if( strcmp( value, "DestAlpha" ) == 0 ) return engine::bmDestAlpha;
    else if( strcmp( value, "InvDestAlpha" ) == 0 ) return engine::bmInvDestAlpha;
    else if( strcmp( value, "DestColor" ) == 0 ) return engine::bmDestColor;
    else if( strcmp( value, "InvDestColor" ) == 0 ) return engine::bmInvDestColor;
    else if( strcmp( value, "SrcAlphaSat" ) == 0 ) return engine::bmSrcAlphaSat;
    assert( !"shouldn't be here!" );
    return engine::bmZero;
}

static const char* wrap(engine::BlendingOperation value)
{
    switch( value )
    {
    case engine::bpAdd: return "Add";
    case engine::bpSubtract: return "Subtract";
    case engine::bpRevSubtract: return "RevSubtract";
    case engine::bpMin: return "Min";
    case engine::bpMax: return "Max";
    default:
        assert( !"shouldn't be here!" );
        return "Zero";
    }
}

static engine::BlendingOperation wrapOp(const char* value)
{
    if( strcmp( value, "Add" ) == 0 ) return engine::bpAdd;
    else if( strcmp( value, "Subtract" ) == 0 ) return engine::bpSubtract;
    else if( strcmp( value, "RevSubtract" ) == 0 ) return engine::bpRevSubtract;
    else if( strcmp( value, "Min" ) == 0 ) return engine::bpMin;
    else if( strcmp( value, "Max" ) == 0 ) return engine::bpMax;
    assert( !"shouldn't be here!" );
    return engine::bpAdd;
}

static const char* wrap(engine::CompareFunction value)
{
    switch( value )
    {
    case engine::cfNever: return "Never";
    case engine::cfEqual: return "Equal";
    case engine::cfLess: return "Less";
    case engine::cfLessEqual: return "LessEqual";
    case engine::cfGreater: return "Greater";
    case engine::cfNotEqual: return "NotEqual";
    case engine::cfGreaterEqual: return "GreaterEqual";
    case engine::cfAlways: return "Always";
    default:
        assert( !"shouldn't be here!" );
        return "Never";
    }
}

static engine::CompareFunction wrapCmp(const char* value)
{
    if( strcmp( value, "Never" ) == 0 ) return engine::cfNever;
    else if( strcmp( value, "Equal" ) == 0 ) return engine::cfEqual;
    else if( strcmp( value, "Less" ) == 0 ) return engine::cfLess;
    else if( strcmp( value, "LessEqual" ) == 0 ) return engine::cfLessEqual;
    else if( strcmp( value, "Greater" ) == 0 ) return engine::cfGreater;
    else if( strcmp( value, "NotEqual" ) == 0 ) return engine::cfNotEqual;
    else if( strcmp( value, "GreaterEqual" ) == 0 ) return engine::cfGreaterEqual;
    else if( strcmp( value, "Always" ) == 0 ) return engine::cfAlways;
    assert( !"shouldn't be here!" );
    return engine::cfNever;
}

static const char* wrap(engine::BlendingType value)
{
    switch( value )
    {
    case engine::btOver: return "Over";
    case engine::btAdd: return "Add";
    case engine::btSub: return "Sub";
    case engine::btAddSmooth: return "AddSmooth";
    case engine::btAddSigned: return "AddSigned";
    case engine::btModulate: return "Modulate";
    case engine::btModulate2x: return "Modulate2x";
    case engine::btModulate4x: return "Modulate4x";
    case engine::btBlendTextureAlpha: return "BlendTextureAlpha";
    case engine::btLerp: return "Lerp";
    default:
        assert( !"shouldn't be here!" );
        return "Over";
    }
}

static engine::BlendingType wrapType(const char*  value)
{
    if( strcmp( value, "Over" ) == 0 ) return engine::btOver;
    else if( strcmp( value, "Add" ) == 0 ) return engine::btAdd;
    else if( strcmp( value, "Sub" ) == 0 ) return engine::btSub;
    else if( strcmp( value, "AddSigned" ) == 0 ) return engine::btAddSigned;
    else if( strcmp( value, "AddSmooth" ) == 0 ) return engine::btAddSmooth;
    else if( strcmp( value, "Modulate" ) == 0 ) return engine::btModulate;
    else if( strcmp( value, "Modulate2x" ) == 0 ) return engine::btModulate2x;
    else if( strcmp( value, "Modulate4x" ) == 0 ) return engine::btModulate4x;
    else if( strcmp( value, "BlendTextureAlpha" ) == 0 ) return engine::btBlendTextureAlpha;
    else if( strcmp( value, "Lerp" ) == 0 ) return engine::btLerp;
    assert( !"shouldn't be here!" );
    return engine::btOver;
}

void ShaderEditor::updateControls(void)
{
    assert( shader );

    int numLayers = shader->getNumLayers();

    flagCulling       = ( shader->getFlags() & engine::sfCulling );
    flagCulling       = flagCulling != 0;
    flagCollision     = ( shader->getFlags() & engine::sfCollision );
    flagCollision     = flagCollision != 0;
    flagAlphaBlending = ( shader->getFlags() & engine::sfAlphaBlending );
    flagAlphaBlending = flagAlphaBlending != 0;
    flagAlphaTesting  = ( shader->getFlags() & engine::sfAlphaTesting );
    flagAlphaTesting  = flagAlphaTesting != 0;

    alphaTestingRef = shader->getAlphaTestRef();

    lightset = shader->getLightset();

    diffuseR = shader->getDiffuseColor()[0];
    diffuseG = shader->getDiffuseColor()[1];
    diffuseB = shader->getDiffuseColor()[2];
    diffuseA = shader->getDiffuseColor()[3];

    specularR = shader->getSpecularColor()[0];
    specularG = shader->getSpecularColor()[1];
    specularB = shader->getSpecularColor()[2];
    specularA = shader->getSpecularColor()[3];

    specularPower = shader->getSpecularPower();

    enableLayer( 0, ( numLayers > 0 ) );
    enableLayer( 1, ( numLayers > 1 ) );
    enableLayer( 2, ( numLayers > 2 ) );
    enableLayer( 3, ( numLayers > 3 ) );

    for( int i=0; i<numLayers; i++ )
    {
        getLayerConstantR( i ) = shader->getLayerConstant( i )[0];
        getLayerConstantG( i ) = shader->getLayerConstant( i )[1];
        getLayerConstantB( i ) = shader->getLayerConstant( i )[2];
        getLayerConstantA( i ) = shader->getLayerConstant( i )[3];
    }

    UpdateData( FALSE );

    CComboBox* alphaBlendingSrcBox = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_SRC ) );
    CComboBox* alphaBlendingDstBox = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_DST ) );
    CComboBox* alphaBlendingOpBox = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_OP ) );
    if( flagAlphaBlending )
    {
        alphaBlendingSrcBox->EnableWindow( true );
        alphaBlendingDstBox->EnableWindow( true );
        alphaBlendingOpBox->EnableWindow( true );
    }
    else
    {
        alphaBlendingSrcBox->EnableWindow( false );
        alphaBlendingDstBox->EnableWindow( false );
        alphaBlendingOpBox->EnableWindow( false );
    }
    alphaBlendingSrcBox->SelectString( -1, wrap( shader->getSrcBlend() ) );
    alphaBlendingDstBox->SelectString( -1, wrap( shader->getDestBlend() ) );
    alphaBlendingOpBox->SelectString( -1, wrap( shader->getBlendOp() ) );
    
    /*
    alphaBlendingSrcBox->GetWindowText( alphaBlendingSrc );
    alphaBlendingDstBox->GetWindowText( alphaBlendingDst );
    alphaBlendingOpBox->GetWindowText( alphaBlendingOp );
    */

    CComboBox* alphaTestFunctionBox = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_TESTING_FUNCT ) );
    CEdit* alphaTestRefBox = static_cast<CEdit*>( GetDlgItem( IDC_ALPHA_TESTING_REF ) );
    if( flagAlphaTesting )
    {
        alphaTestFunctionBox->EnableWindow( true );
        alphaTestRefBox->EnableWindow( true );
    }
    else
    {
        alphaTestFunctionBox->EnableWindow( false );
        alphaTestRefBox->EnableWindow( false );
    }
    alphaTestFunctionBox->SelectString( -1, wrap( shader->getAlphaTestFunction() ) );
    
    /*
    alphaTestFunctionBox->GetWindowText( alphaTestingFunct );
    CString ref;
    alphaTestRefBox->GetWindowText( ref );
    alphaTestingRef = atoi( ref );
    */

    for( i=0; i<numLayers; i++ )
    {
        getLayerTextureBtn( i )->SetWindowText( shader->getLayerTexture( i )->getName() );
        getLayerUVIDBox( i )->SelectString( -1, strformat( "%d", shader->getLayerUV( i ) ).c_str() );
        getLayerBlendingBox( i )->SelectString( -1, wrap( shader->getLayerBlending( i ) ) );
    }
    for( i=numLayers; i<engine::maxTextureLayers; i++ )
    {
        getLayerTextureBtn( i )->SetWindowText( "" );
    }

    engine::ITexture* normalMap = shader->getNormalMap();
    if( normalMap )
    {
        CButton* normalMapBtn = static_cast<CButton*>( GetDlgItem( IDC_NORMAL_MAP_BTN ) );
        normalMapBtn->SetWindowText( normalMap->getName() );
        CComboBox* normalMapUVIDBox = static_cast<CComboBox*>( GetDlgItem( IDC_NORMAL_MAP_UVID ) );
        normalMapUVIDBox->SetCurSel( shader->getNormalMapUV() );
    }  
    
    UpdateData( TRUE );
}

bool ShaderEditor::validateControl(int dlgId)
{
    CString windowText;
    GetDlgItem( dlgId )->GetWindowText( windowText );
    if( windowText.IsEmpty() || !strIsNumber( windowText ) )
    {
        GetDlgItem( IDC_TROUBLEDESC )->SetWindowText( "invalid character" );
        return false;
    }
    else
    {
        return true;
    }
}

bool ShaderEditor::validateDWORDControl(int dlgId)
{
    CString windowText;
    GetDlgItem( dlgId )->GetWindowText( windowText );
    
    if( windowText.IsEmpty() || !strIsUnsignedInt( windowText ) )
    {
        GetDlgItem( IDC_TROUBLEDESC )->SetWindowText( "invalid character" );
        return false;
    }
    else
    {
        return true;
    }
}

bool ShaderEditor::validateControls(void)
{
    GetDlgItem( IDC_TROUBLEDESC )->SetWindowText( "none" );
    return validateDWORDControl( IDC_ALPHA_TESTING_REF ) &&
           validateControl( IDC_DIFFUSE_R ) &&
           validateControl( IDC_DIFFUSE_G ) &&
           validateControl( IDC_DIFFUSE_B ) &&
           validateControl( IDC_DIFFUSE_A ) &&
           validateControl( IDC_SPECULAR_R ) &&
           validateControl( IDC_SPECULAR_G ) &&
           validateControl( IDC_SPECULAR_B ) &&
           validateControl( IDC_SPECULAR_A ) &&
           validateControl( IDC_SPECULAR_POWER ) &&          
           validateControl( IDC_LAYER_0_CONSTANT_R ) &&
           validateControl( IDC_LAYER_0_CONSTANT_G ) &&
           validateControl( IDC_LAYER_0_CONSTANT_B ) &&
           validateControl( IDC_LAYER_0_CONSTANT_A ) &&             
           validateControl( IDC_LAYER_1_CONSTANT_R ) &&
           validateControl( IDC_LAYER_1_CONSTANT_G ) && 
           validateControl( IDC_LAYER_1_CONSTANT_B ) &&
           validateControl( IDC_LAYER_1_CONSTANT_A ) &&                                
           validateControl( IDC_LAYER_2_CONSTANT_R ) &&
           validateControl( IDC_LAYER_2_CONSTANT_G ) &&
           validateControl( IDC_LAYER_2_CONSTANT_B ) &&
           validateControl( IDC_LAYER_2_CONSTANT_A ) &&
           validateControl( IDC_LAYER_3_CONSTANT_R ) &&
           validateControl( IDC_LAYER_3_CONSTANT_G ) &&
           validateControl( IDC_LAYER_3_CONSTANT_B ) &&
           validateControl( IDC_LAYER_3_CONSTANT_A );

}

void ShaderEditor::enableLayer(int layerId, bool enable)
{
    switch(layerId)
    {
    case 0:
        GetDlgItem( IDC_LAYER_0_TEXTURE_BTN )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_UVID )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_BLENDING )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_CONSTANT_R )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_CONSTANT_G )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_CONSTANT_B )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_CONSTANT_A )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_0_CONSTANT_BTN )->EnableWindow( enable );
        break;
    case 1:
        GetDlgItem( IDC_LAYER_1_TEXTURE_BTN )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_UVID )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_BLENDING )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_CONSTANT_R )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_CONSTANT_G )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_CONSTANT_B )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_CONSTANT_A )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_1_CONSTANT_BTN )->EnableWindow( enable );
        break;
    case 2:
        GetDlgItem( IDC_LAYER_2_TEXTURE_BTN )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_UVID )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_BLENDING )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_CONSTANT_R )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_CONSTANT_G )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_CONSTANT_B )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_CONSTANT_A )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_2_CONSTANT_BTN )->EnableWindow( enable );
        break;
    case 3:
        GetDlgItem( IDC_LAYER_3_TEXTURE_BTN )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_UVID )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_BLENDING )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_CONSTANT_R )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_CONSTANT_G )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_CONSTANT_B )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_CONSTANT_A )->EnableWindow( enable );
        GetDlgItem( IDC_LAYER_3_CONSTANT_BTN )->EnableWindow( enable );
        break;
    default:
        assert( !"shouldn't be here!" );
    }
}

CButton* ShaderEditor::getLayerTextureBtn(int layerId)
{
    switch( layerId )
    {
    case 0:
        return static_cast<CButton*>( GetDlgItem( IDC_LAYER_0_TEXTURE_BTN ) );
    case 1:
        return static_cast<CButton*>( GetDlgItem( IDC_LAYER_1_TEXTURE_BTN ) );
    case 2:
        return static_cast<CButton*>( GetDlgItem( IDC_LAYER_2_TEXTURE_BTN ) );
    case 3:
        return static_cast<CButton*>( GetDlgItem( IDC_LAYER_3_TEXTURE_BTN ) );
    default:
        assert( !"shouldn't be here!" );
        return static_cast<CButton*>( GetDlgItem( IDC_LAYER_0_TEXTURE_BTN ) );
    }
}

CComboBox* ShaderEditor::getLayerUVIDBox(int layerId)
{
    switch( layerId )
    {
    case 0:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_0_UVID ) );
    case 1:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_1_UVID ) );
    case 2:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_2_UVID ) );
    case 3:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_3_UVID ) );
    default:
        assert( !"shouldn't be here!" );
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_0_UVID ) );
    }
}

CComboBox* ShaderEditor::getLayerBlendingBox(int layerId)
{
    switch( layerId )
    {
    case 0:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_0_BLENDING ) );
    case 1:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_1_BLENDING ) );
    case 2:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_2_BLENDING ) );
    case 3:
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_3_BLENDING ) );
    default:
        assert( !"shouldn't be here!" );
        return static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_0_BLENDING ) );
    }
}

CString& ShaderEditor::getLayerUVID(int layerId)
{
    switch( layerId )
    {
    case 0:
        return layer0UVID;
    case 1:
        return layer1UVID;
    case 2:
        return layer2UVID;
    case 3:
        return layer3UVID;
    default:
        assert( !"shouldn't be here!" );
        return layer0UVID;
    }
}

CString& ShaderEditor::getLayerBlending(int layerId)
{
    switch( layerId )
    {
    case 0:
        return layer0Blending;
    case 1:
        return layer1Blending;
    case 2:
        return layer2Blending;
    case 3:
        return layer3Blending;
    default:
        assert( !"shouldn't be here!" );
        return layer0Blending;
    }
}

float& ShaderEditor::getLayerConstantR(int layerId)
{
    switch( layerId )
    {
    case 0:
        return layer0ConstantR;
    case 1:
        return layer1ConstantR;
    case 2:
        return layer2ConstantR;
    case 3:
        return layer3ConstantR;
    default:
        assert( !"shouldn't be here!" );
        return layer0ConstantR;
    }
}

float& ShaderEditor::getLayerConstantG(int layerId)
{
    switch( layerId )
    {
    case 0:
        return layer0ConstantG;
    case 1:
        return layer1ConstantG;
    case 2:
        return layer2ConstantG;
    case 3:
        return layer3ConstantG;
    default:
        assert( !"shouldn't be here!" );
        return layer0ConstantG;
    }
}

float& ShaderEditor::getLayerConstantB(int layerId)
{
    switch( layerId )
    {
    case 0:
        return layer0ConstantB;
    case 1:
        return layer1ConstantB;
    case 2:
        return layer2ConstantB;
    case 3:
        return layer3ConstantB;
    default:
        assert( !"shouldn't be here!" );
        return layer0ConstantB;
    }
}

float& ShaderEditor::getLayerConstantA(int layerId)
{
    switch( layerId )
    {
    case 0:
        return layer0ConstantA;
    case 1:
        return layer1ConstantA;
    case 2:
        return layer2ConstantA;
    case 3:
        return layer3ConstantA;
    default:
        assert( !"shouldn't be here!" );
        return layer0ConstantA;
    }
}

/////////////////////////////////////////////////////////////////////////////
// ShaderEditor message handlers

void ShaderEditor::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if( shader )
    {
        if( textureChooser->IsWindowVisible() ) textureChooser->ShowWindow( SW_HIDE );
        if( effectEditor->IsWindowVisible() ) effectEditor->ShowWindow( SW_HIDE );
	    SetWindowText( shader->getName() );
        updateControls();
    }
}

BOOL ShaderEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();

    SetTimer( 1, 100,  NULL );

    colorDlg = new CColorDialog( 0, CC_ANYCOLOR | CC_FULLOPEN, this );

    textureChooser = new TextureChooser( this );
    textureChooser->Create( IDD_TEXTURE_CHOOSER, this );

    effectEditor = new EffectEditor( this );
    effectEditor->Create( IDD_EFFECT_EDITOR, this );

    CComboBox* box = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_SRC ) );
    box->ResetContent();
    box->AddString( "Zero" );
    box->AddString( "One" );
    box->AddString( "SrcColor" );
    box->AddString( "InvSrcColor" );
    box->AddString( "SrcAlpha" );
    box->AddString( "InvSrcAlpha" );
    box->AddString( "DestAlpha" );
    box->AddString( "InvDestAlpha" );
    box->AddString( "DestColor" );
    box->AddString( "InvDestColor" );
    box->AddString( "SrcAlphaSat" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_DST ) );
    box->ResetContent();
    box->AddString( "Zero" );
    box->AddString( "One" );
    box->AddString( "SrcColor" );
    box->AddString( "InvSrcColor" );
    box->AddString( "SrcAlpha" );
    box->AddString( "InvSrcAlpha" );
    box->AddString( "DestAlpha" );
    box->AddString( "InvDestAlpha" );
    box->AddString( "DestColor" );
    box->AddString( "InvDestColor" );
    box->AddString( "SrcAlphaSat" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_OP ) );
    box->ResetContent();
    box->AddString( "Add" );
    box->AddString( "Subtract" );
    box->AddString( "RevSubtract" );
    box->AddString( "Min" );
    box->AddString( "Max" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_TESTING_FUNCT ) );
    box->ResetContent();
    box->AddString( "Never" );
    box->AddString( "Equal" );
    box->AddString( "Less" );
    box->AddString( "LessEqual" );
    box->AddString( "Greater" );
    box->AddString( "NotEqual" );
    box->AddString( "GreaterEqual" );
    box->AddString( "Always" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_0_UVID ) );
    box->ResetContent();
    box->AddString( "0" );
    box->AddString( "1" );
    box->AddString( "2" );
    box->AddString( "3" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_1_UVID ) );
    box->ResetContent();
    box->AddString( "0" );
    box->AddString( "1" );
    box->AddString( "2" );
    box->AddString( "3" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_2_UVID ) );
    box->ResetContent();
    box->AddString( "0" );
    box->AddString( "1" );
    box->AddString( "2" );
    box->AddString( "3" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_3_UVID ) );
    box->ResetContent();
    box->AddString( "0" );
    box->AddString( "1" );
    box->AddString( "2" );
    box->AddString( "3" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_NORMAL_MAP_UVID ) );
    box->ResetContent();
    box->AddString( "0" );
    box->AddString( "1" );
    box->AddString( "2" );
    box->AddString( "3" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_0_BLENDING ) );
    box->ResetContent();
    box->AddString( "Over" );
    box->AddString( "Add" );
    box->AddString( "Sub" );
    box->AddString( "AddSigned" );
    box->AddString( "AddSmooth" );
    box->AddString( "Modulate" );
    box->AddString( "Modulate2x" );
    box->AddString( "Modulate4x" );
    box->AddString( "BlendTextureAlpha" );
    box->AddString( "Lerp" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_1_BLENDING ) );
    box->ResetContent();
    box->AddString( "Over" );
    box->AddString( "Add" );
    box->AddString( "Sub" );
    box->AddString( "AddSigned" );
    box->AddString( "AddSmooth" );
    box->AddString( "Modulate" );
    box->AddString( "Modulate2x" );
    box->AddString( "Modulate4x" );
    box->AddString( "BlendTextureAlpha" );
    box->AddString( "Lerp" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_2_BLENDING ) );
    box->ResetContent();
    box->AddString( "Over" );
    box->AddString( "Add" );
    box->AddString( "Sub" );
    box->AddString( "AddSigned" );
    box->AddString( "AddSmooth" );
    box->AddString( "Modulate" );
    box->AddString( "Modulate2x" );
    box->AddString( "Modulate4x" );
    box->AddString( "BlendTextureAlpha" );
    box->AddString( "Lerp" );

    box = static_cast<CComboBox*>( GetDlgItem( IDC_LAYER_3_BLENDING ) );
    box->ResetContent();
    box->AddString( "Over" );
    box->AddString( "Add" );
    box->AddString( "Sub" );
    box->AddString( "AddSigned" );
    box->AddString( "AddSmooth" );
    box->AddString( "Modulate" );
    box->AddString( "Modulate2x" );
    box->AddString( "Modulate4x" );
    box->AddString( "BlendTextureAlpha" );
    box->AddString( "Lerp" );

	return TRUE;
}

void ShaderEditor::OnTimer(UINT nIDEvent) 
{
    if( nIDEvent == 1 )
    {
        if( shader )
        {
            GetDlgItem( IDC_EFFECT )->SetWindowText(
                shader->getEffect() ? shader->getEffect()->getName() : "No effect"
            );
        }
        if( textureChooser->state == tcsOk )
        {
            textureChooser->state = tcsDontCare;
            if( textureChooser->layerId == -1 )
            {
                shader->setNormalMap( textureChooser->texture );
                CButton* normalMapBtn = static_cast<CButton*>( GetDlgItem( IDC_NORMAL_MAP_BTN ) );
                if( textureChooser->texture )
                {
                    normalMapBtn->SetWindowText( textureChooser->texture->getName() );
                }
                else
                {
                    normalMapBtn->SetWindowText( "" );
                }
            }
            else if( textureChooser->layerId == -2 )
            {
                shader->setEnvironmentMap( textureChooser->texture );
                CButton* envMapBtn = static_cast<CButton*>( GetDlgItem( IDC_ENV_MAP_BTN ) );
                if( textureChooser->texture )
                {
                    envMapBtn->SetWindowText( textureChooser->texture->getName() );
                }
                else
                {
                    envMapBtn->SetWindowText( "" );
                }
            }
            else
            {
                shader->setLayerTexture( 
                    textureChooser->layerId,
                    textureChooser->texture
                );
                if( textureChooser->texture )
                {
                    getLayerTextureBtn( textureChooser->layerId )->SetWindowText( 
                        textureChooser->texture->getName() 
                    );
                }
                else
                {
                    getLayerTextureBtn( textureChooser->layerId )->SetWindowText( "" );
                }
                textureChooser->layerId = 0;
                textureChooser->texture = NULL;
            }
        }
    }

	if( nIDEvent == 1 && isModified && validateControls() )
    {
        UpdateData();        

        unsigned int flags = 0;
        if( flagCulling ) flags |= engine::sfCulling;
        if( flagCollision ) flags |= engine::sfCollision;
        if( flagAlphaBlending ) 
        {
            flags |= engine::sfAlphaBlending;
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_SRC ) )->EnableWindow( true );
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_DST ) )->EnableWindow( true );
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_OP ) )->EnableWindow( true );    
        }
        else
        {
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_SRC ) )->EnableWindow( false );
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_DST ) )->EnableWindow( false );
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_BLENDING_OP ) )->EnableWindow( false );
        }
        if( flagAlphaTesting ) 
        {
            flags |= engine::sfAlphaTesting;
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_TESTING_FUNCT ) )->EnableWindow( true );
            static_cast<CEdit*>( GetDlgItem( IDC_ALPHA_TESTING_REF ) )->EnableWindow( true );
        }
        else
        {
            static_cast<CComboBox*>( GetDlgItem( IDC_ALPHA_TESTING_FUNCT ) )->EnableWindow( false );
            static_cast<CEdit*>( GetDlgItem( IDC_ALPHA_TESTING_REF ) )->EnableWindow( false );
        }
        shader->setFlags( flags );
        shader->setLightset( lightset );

        shader->setSrcBlend( wrapMode( alphaBlendingSrc ) );
        shader->setDestBlend( wrapMode( alphaBlendingDst ) );
        shader->setBlendOp( wrapOp( alphaBlendingOp ) );

        shader->setAlphaTestFunction( wrapCmp( alphaTestingFunct ) );
        shader->setAlphaTestRef( ((unsigned char)(alphaTestingRef)) );

        shader->setDiffuseColor( Vector4f( diffuseR, diffuseG, diffuseB, diffuseA ) );
        shader->setSpecularColor( Vector4f( specularR, specularG, specularB, specularA ) );
        shader->setSpecularPower( specularPower );
        
        for( int i=0; i<shader->getNumLayers(); i++ )
        {
            shader->setLayerUV( i, atoi( getLayerUVID( i ) ) );
            shader->setLayerBlending( i, wrapType( this->getLayerBlending( i ) ) );
            shader->setLayerConstant( i, Vector4f( 
                getLayerConstantR( i ),
                getLayerConstantG( i ),
                getLayerConstantB( i ),
                getLayerConstantA( i )
            ) );
        }

        shader->setNormalMapUV( int( atof( normalMapUVID ) ) );

        isModified = false;
    }
	
	CDialog::OnTimer(nIDEvent);
}

void ShaderEditor::OnFlagCulling() 
{
    isModified = true;	
}

void ShaderEditor::OnFlagCollision() 
{
    isModified = true;	
}

void ShaderEditor::OnFlagAlphaBlending() 
{
    isModified = true;
}

void ShaderEditor::OnFlagAlphaTesting() 
{
    isModified = true;
}

void ShaderEditor::OnSelchangeAlphaBlendingSrc() 
{
    isModified = true;	
}

void ShaderEditor::OnSelchangeAlphaBlendingOp() 
{
    isModified = true;	
}

void ShaderEditor::OnSelchangeAlphaBlendingDst() 
{
    isModified = true;	
}

void ShaderEditor::OnSelchangeAlphaTestingFunct() 
{
	isModified = true;
}

void ShaderEditor::OnChangeAlphaTestingRef() 
{
	isModified = true;
}

void ShaderEditor::OnChangeDiffuseR() 
{
	isModified = true;
}

void ShaderEditor::OnChangeDiffuseG() 
{
	isModified = true;
}

void ShaderEditor::OnChangeDiffuseB() 
{
	isModified = true;
}

void ShaderEditor::OnChangeDiffuseA() 
{
	isModified = true;
}

void ShaderEditor::OnChangeSpecularR() 
{
	isModified = true;
}

void ShaderEditor::OnChangeSpecularG() 
{
	isModified = true;
}

void ShaderEditor::OnChangeSpecularB() 
{
	isModified = true;
}

void ShaderEditor::OnChangeSpecularA() 
{
	isModified = true;
}

void ShaderEditor::OnChangeSpecularPower() 
{
	isModified = true;
}

void ShaderEditor::OnDiffuseBtn() 
{
    if( !validateControls() ) return;

    Vector4f diffuse = shader->getDiffuseColor();
    if( colorDlg->DoModal() )
    {
        diffuseR = diffuse[0] = float( GetRValue( colorDlg->GetColor() ) ) / 255;
        diffuseG = diffuse[1] = float( GetGValue( colorDlg->GetColor() ) ) / 255;
        diffuseB = diffuse[2] = float( GetBValue( colorDlg->GetColor() ) ) / 255;
        shader->setDiffuseColor( diffuse );
        UpdateData( FALSE );
    }
}

void ShaderEditor::OnSpecularBtn() 
{
    if( !validateControls() ) return;

    Vector4f specular = shader->getSpecularColor();
    if( colorDlg->DoModal() )
    {
        specularR = specular[0] = float( GetRValue( colorDlg->GetColor() ) ) / 255;
        specularG = specular[1] = float( GetGValue( colorDlg->GetColor() ) ) / 255;
        specularB = specular[2] = float( GetBValue( colorDlg->GetColor() ) ) / 255;
        shader->setSpecularColor( specular );
        UpdateData( FALSE );
    }	
}

void ShaderEditor::OnLayer0TextureBtn() 
{
    if( !validateControls() ) return;

    RECT buttonRect;
    getLayerTextureBtn( 0 )->GetWindowRect( &buttonRect );

    RECT dialogRect;
    textureChooser->GetWindowRect( &dialogRect );

    textureChooser->layerId = 0;
    textureChooser->texture = shader->getLayerTexture( 0 );
    textureChooser->SetWindowPos( NULL, buttonRect.left, buttonRect.top, dialogRect.right-dialogRect.left, dialogRect.bottom-dialogRect.top, 0 );
	textureChooser->ShowWindow( SW_SHOW );
}

void ShaderEditor::OnLayer1TextureBtn() 
{
    if( !validateControls() ) return;

    RECT buttonRect;
    getLayerTextureBtn( 1 )->GetWindowRect( &buttonRect );

    RECT dialogRect;
    textureChooser->GetWindowRect( &dialogRect );

    textureChooser->layerId = 1;
    textureChooser->texture = shader->getLayerTexture( 1 );
    textureChooser->SetWindowPos( NULL, buttonRect.left, buttonRect.top, dialogRect.right-dialogRect.left, dialogRect.bottom-dialogRect.top, 1 );
	textureChooser->ShowWindow( SW_SHOW );	
}

void ShaderEditor::OnSelchangeLayer0Uvid() 
{
	isModified = true;
}

void ShaderEditor::OnSelchangeLayer1Uvid() 
{
    isModified = true;	
}

void ShaderEditor::OnSelchangeLayer2Uvid() 
{
	isModified = true;
}

void ShaderEditor::OnSelchangeLayer3Uvid() 
{
	isModified = true;	
}

void ShaderEditor::OnSelchangeLayer0Blending() 
{
	isModified = true;
}

void ShaderEditor::OnSelchangeLayer1Blending() 
{
	isModified = true;
}

void ShaderEditor::OnSelchangeLayer2Blending() 
{
	isModified = true;
}

void ShaderEditor::OnSelchangeLayer3Blending() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer0ConstantR() 
{
    isModified = true;	
}

void ShaderEditor::OnChangeLayer0ConstantG() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer0ConstantB() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer0ConstantA() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer1ConstantR() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer1ConstantG() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer1ConstantB() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer1ConstantA() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer2ConstantR() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer2ConstantG() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer2ConstantB() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer2ConstantA() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer3ConstantR() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer3ConstantG() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer3ConstantB() 
{
	isModified = true;
}

void ShaderEditor::OnChangeLayer3ConstantA() 
{
	isModified = true;
}

void ShaderEditor::OnNormalMapBtn() 
{
    if( !validateControls() ) return;

    CButton* normalMapBtn = static_cast<CButton*>( GetDlgItem( IDC_NORMAL_MAP_BTN ) );
    RECT buttonRect;
    normalMapBtn->GetWindowRect( &buttonRect );

    RECT dialogRect;
    textureChooser->GetWindowRect( &dialogRect );

    textureChooser->layerId = -1;
    textureChooser->texture = shader->getNormalMap();
    textureChooser->SetWindowPos( NULL, buttonRect.left, buttonRect.top, dialogRect.right-dialogRect.left, dialogRect.bottom-dialogRect.top, 1 );
	textureChooser->ShowWindow( SW_SHOW );
}

void ShaderEditor::OnSelchangeNormalMapUvid() 
{
	isModified = true;	
}

void ShaderEditor::OnEffect() 
{
    if( !validateControls() ) return;

    RECT buttonRect;
    GetDlgItem( IDC_EFFECT )->GetWindowRect( &buttonRect );

    RECT dialogRect;
    effectEditor->GetWindowRect( &dialogRect );
    
    effectEditor->SetWindowPos( 
        NULL, 
        buttonRect.left,
        buttonRect.bottom-(dialogRect.bottom-dialogRect.top),
        dialogRect.right-dialogRect.left, 
        dialogRect.bottom-dialogRect.top, 
        0 
    );

    effectEditor->setDestination( shader );
	effectEditor->ShowWindow( SW_SHOW );
}

void ShaderEditor::OnClose() 
{
	shader = NULL;
	CDialog::OnClose();
}

void ShaderEditor::OnBnClickedEnvMapBtn()
{
    if( !validateControls() ) return;

    CButton* envMapBtn = static_cast<CButton*>( GetDlgItem( IDC_ENV_MAP_BTN ) );
    RECT buttonRect;
    envMapBtn->GetWindowRect( &buttonRect );

    RECT dialogRect;
    textureChooser->GetWindowRect( &dialogRect );

    textureChooser->layerId = -2;
    textureChooser->texture = shader->getEnvironmentMap();
    textureChooser->SetWindowPos( NULL, buttonRect.left, buttonRect.top, dialogRect.right-dialogRect.left, dialogRect.bottom-dialogRect.top, 1 );
	textureChooser->ShowWindow( SW_SHOW );
    
}

void ShaderEditor::OnEnChangeLightset()
{
    isModified = true;
}

void ShaderEditor::OnBnClickedLayer2TextureBtn()
{
    if( !validateControls() ) return;

    RECT buttonRect;
    getLayerTextureBtn( 2 )->GetWindowRect( &buttonRect );

    RECT dialogRect;
    textureChooser->GetWindowRect( &dialogRect );

    textureChooser->layerId = 2;
    textureChooser->texture = shader->getLayerTexture( 2 );
    textureChooser->SetWindowPos( NULL, buttonRect.left, buttonRect.top, dialogRect.right-dialogRect.left, dialogRect.bottom-dialogRect.top, 1 );
	textureChooser->ShowWindow( SW_SHOW );	
}

void ShaderEditor::OnBnClickedLayer3TextureBtn()
{
    if( !validateControls() ) return;

    RECT buttonRect;
    getLayerTextureBtn( 3 )->GetWindowRect( &buttonRect );

    RECT dialogRect;
    textureChooser->GetWindowRect( &dialogRect );

    textureChooser->layerId = 3;
    textureChooser->texture = shader->getLayerTexture( 3 );
    textureChooser->SetWindowPos( NULL, buttonRect.left, buttonRect.top, dialogRect.right-dialogRect.left, dialogRect.bottom-dialogRect.top, 1 );
	textureChooser->ShowWindow( SW_SHOW );	
}
