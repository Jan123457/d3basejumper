// effectedit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "effectedit.h"
#include "../common/istring.h"
#include ".\effectedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EffectEditor dialog


EffectEditor::EffectEditor(CWnd* pParent /*=NULL*/)
	: CDialog(EffectEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(EffectEditor)
	//}}AFX_DATA_INIT
    _dstShader   = NULL;
    _dstGeometry = NULL;
    isModified = false;
}


void EffectEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EffectEditor)
	DDX_Control(pDX, IDC_R, argR);
	DDX_Control(pDX, IDC_G, argG);
	DDX_Control(pDX, IDC_B, argB);
	DDX_Control(pDX, IDC_A, argA);
	DDX_Control(pDX, IDC_ARGLIST, arguments);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EffectEditor, CDialog)
	//{{AFX_MSG_MAP(EffectEditor)
	ON_BN_CLICKED(IDC_OK_BUTTON, OnOkButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, OnCancelButton)
//	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_EFFECTNAME, OnSelchangeEffectname)
	ON_NOTIFY(NM_CLICK, IDC_ARGLIST, OnClickArglist)
	ON_EN_CHANGE(IDC_R, OnChangeR)
	ON_EN_CHANGE(IDC_G, OnChangeG)
	ON_EN_CHANGE(IDC_B, OnChangeB)
	ON_EN_CHANGE(IDC_A, OnChangeA)
	//}}AFX_MSG_MAP
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EffectEditor behaviour

const char* wrap(VariantType type)
{
    switch( type )
    {
    case vtBool: return "Bool";
    case vtInt: return "Int";
    case vtFloat: return "Float";
    case vtVector2f: return "Vector2f";
    case vtVector3f: return "Vector3f";
    case vtVector4f: return "Vector4f";
    case vtMatrix2f: return "Matrix2f";
    case vtMatrix3f: return "Matrix3f";
    case vtMatrix4f: return "Matrix4f";
    case vtString: return "String";
    default:
        assert( !"shouldn't be here!" );
        return "Bool";
    };
}

/////////////////////////////////////////////////////////////////////////////
// EffectEditor message handlers

void EffectEditor::OnOkButton() 
{   
	ShowWindow( SW_HIDE );
}

void EffectEditor::OnCancelButton() 
{
    ShowWindow( SW_HIDE );
}

void EffectEditor::OnTimer(UINT nIDEvent) 
{
    if( nIDEvent == 1 && isModified )
    {
        isModified = false;

        CString effectName;
        GetDlgItem( IDC_EFFECTNAME )->GetWindowText( effectName );

        if( ( getEffect() == NULL && strcmp( effectName, "No effect" ) != 0 ) ||
            ( getEffect() != NULL && strcmp( effectName, getEffect()->getName() ) != 0 ) )
        {
            engine::IEffect* effect = getEffect();
            if( effect ) 
            {
                setEffect( NULL );
                effect->release();
            }
            if( strcmp( effectName, "No effect" ) != 0 )
            {
                effect = iEngine()->createEffect( effectName );
                if( effect ) setEffect( effect );
            }

            arguments.DeleteAllItems();    
            if( getEffect() )
            {
                for( int i=0; i<getEffect()->getNumArguments(); i++ )
                {
                    Variant argument = getEffect()->getArgument( i );
                    arguments.InsertItem( i, getEffect()->getArgumentName( i ) );
                    arguments.SetItemText( i, 1, wrap( argument.type ) );
                }
            }

            argR.EnableWindow( false );
            argG.EnableWindow( false );
            argB.EnableWindow( false );
            argA.EnableWindow( false );
        }
        else
        {
            POSITION pos = arguments.GetFirstSelectedItemPosition();
            if( pos != NULL )
            {
                CString  windowText;
                Vector2f vector2d;
                Vector3f vector3d;
                Vector4f vector4d;
                int argId = arguments.GetNextSelectedItem( pos );
                Variant value = getEffect()->getArgument( argId );
                switch( value.type )
                {
                case vtBool:                    
                    argR.GetWindowText( windowText );
                    value = Variant( 0 != atoi( windowText ) );
                    break;
                case vtInt:
                    argR.GetWindowText( windowText );
                    value = Variant( atoi( windowText ) );
                    break;
                case vtFloat:
                    argR.GetWindowText( windowText );
                    value = Variant( (float)(atof( windowText )) );
                    break;
                case vtVector2f:                    
                    argR.GetWindowText( windowText ); vector2d[0] = float( atof( windowText ) );
                    argG.GetWindowText( windowText ); vector2d[1] = float( atof( windowText ) );
                    value = Variant( vector2d );
                    break;
                case vtVector3f:
                    argR.GetWindowText( windowText ); vector3d[0] = float( atof( windowText ) );
                    argG.GetWindowText( windowText ); vector3d[1] = float( atof( windowText ) );
                    argB.GetWindowText( windowText ); vector3d[2] = float( atof( windowText ) );
                    value = Variant( vector3d );
                    break;
                case vtVector4f:
                    argR.GetWindowText( windowText ); vector4d[0] = float( atof( windowText ) );
                    argG.GetWindowText( windowText ); vector4d[1] = float( atof( windowText ) );
                    argB.GetWindowText( windowText ); vector4d[2] = float( atof( windowText ) );
                    argA.GetWindowText( windowText ); vector4d[3] = float( atof( windowText ) );
                    value = Variant( vector4d );
                    break;
                default:
                    assert( !"shouldn't be here!" );
                }
                getEffect()->setArgument( argId, value );
            }
        }
    }

	CDialog::OnTimer(nIDEvent);
}

BOOL EffectEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    SetTimer( 1, 100, NULL );

    RECT clientRect;
    arguments.GetClientRect( &clientRect );
    arguments.InsertColumn( 0, "Name", LVCFMT_LEFT, clientRect.right-clientRect.left-64, -1 );
    arguments.InsertColumn( 1, "Type", LVCFMT_LEFT, 64, -1 );    
	
	return TRUE;
}

void EffectEditor::OnSelchangeEffectname() 
{
    isModified = true;
}

void EffectEditor::OnClickArglist(NMHDR* pNMHDR, LRESULT* pResult) 
{
    POSITION pos = arguments.GetFirstSelectedItemPosition();
    if( pos != NULL )
    {
        int argId = arguments.GetNextSelectedItem( pos );
        Variant value = getEffect()->getArgument( argId );
        // setup edit boxes
        argR.EnableWindow( false );
        argG.EnableWindow( false );
        argB.EnableWindow( false );
        argA.EnableWindow( false );
        switch( value.type )
        {
        case vtBool:
            argR.EnableWindow( true );
            argR.SetWindowText( strformat( "%d", (bool)(value) ).c_str() );
            break;
        case vtInt:
            argR.EnableWindow( true );
            argR.SetWindowText( strformat( "%d", (int)(value) ).c_str() );
            break;
        case vtFloat:
            argR.EnableWindow( true );
            argR.SetWindowText( strformat( "%4.2f", (float)(value) ).c_str() );
            break;
        case vtVector2f:
            argR.EnableWindow( true );
            argG.EnableWindow( true );
            argR.SetWindowText( strformat( "%4.2f", value.operator Vector2f()[0] ).c_str() );
            argG.SetWindowText( strformat( "%4.2f", value.operator Vector2f()[1] ).c_str() );
            break;
        case vtVector3f:
            argR.EnableWindow( true );
            argG.EnableWindow( true );
            argB.EnableWindow( true );
            argR.SetWindowText( strformat( "%4.2f", value.operator Vector3f()[0] ).c_str() );
            argG.SetWindowText( strformat( "%4.2f", value.operator Vector3f()[1] ).c_str() );
            argB.SetWindowText( strformat( "%4.2f", value.operator Vector3f()[2] ).c_str() );
            break;
        case vtVector4f:
            argR.EnableWindow( true );
            argG.EnableWindow( true );
            argB.EnableWindow( true );
            argA.EnableWindow( true );
            argR.SetWindowText( strformat( "%4.2f", value.operator Vector4f()[0] ).c_str() );
            argG.SetWindowText( strformat( "%4.2f", value.operator Vector4f()[1] ).c_str() );
            argB.SetWindowText( strformat( "%4.2f", value.operator Vector4f()[2] ).c_str() );
            argA.SetWindowText( strformat( "%4.2f", value.operator Vector4f()[3] ).c_str() );
            break;
        }
    }
    *pResult = 0;
}

void EffectEditor::OnChangeR() 
{
	isModified = true;
}

void EffectEditor::OnChangeG() 
{
    isModified = true;	
}

void EffectEditor::OnChangeB() 
{
	isModified = true;
}

void EffectEditor::OnChangeA() 
{
	isModified = true;
}

void EffectEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

    CComboBox* effectName = static_cast<CComboBox*>( GetDlgItem( IDC_EFFECTNAME ) );
    effectName->ResetContent();
    effectName->AddString( "No effect" );
    for( int i=0; i<iEngine()->getNumEffects(); i++ )
    {
        engine::IEffect* testIssueEffect = iEngine()->createEffect( iEngine()->getEffectName( i ) );
        assert( testIssueEffect );
        bool isCompatible = false;
        if( _dstShader ) isCompatible = testIssueEffect->isCompatible( _dstShader );
        if( _dstGeometry ) isCompatible = testIssueEffect->isCompatible( _dstGeometry );
        testIssueEffect->release();

        if( isCompatible ) effectName->AddString( iEngine()->getEffectName( i ) );
    }

    const char* effect = "No effect";
    if( getEffect() ) effect = getEffect()->getName();
    effectName->SelectString( -1, effect );
    
    arguments.DeleteAllItems();    
    if( getEffect() )
    {
        for( i=0; i<getEffect()->getNumArguments(); i++ )
        {
            Variant argument = getEffect()->getArgument( i );
            arguments.InsertItem( i, getEffect()->getArgumentName( i ) );
            arguments.SetItemText( i, 1, wrap( argument.type ) );
        }
    }

    argR.EnableWindow( false );
    argR.SetWindowText( "" );
    argG.EnableWindow( false );
    argG.SetWindowText( "" );
    argB.EnableWindow( false );
    argB.SetWindowText( "" );
    argA.EnableWindow( false );
    argA.SetWindowText( "" );

    assert( _dstShader || _dstGeometry );
}
