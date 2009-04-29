// bspedit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "bspedit.h"
#include "../common/istring.h"
#include ".\bspedit.h"


// BSPEditor dialog

IMPLEMENT_DYNAMIC(BSPEditor, CDialog)
BSPEditor::BSPEditor(CWnd* pParent /*=NULL*/)
	: CDialog(BSPEditor::IDD, pParent)
{
    bsp = NULL;
    isModified = false;
}

BSPEditor::~BSPEditor()
{
}

void BSPEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BSPEditor, CDialog)
    ON_WM_SHOWWINDOW()
    ON_CBN_SELCHANGE(IDC_FOG_TYPE, OnCbnSelchangeFogType)
    ON_WM_TIMER()
    ON_EN_CHANGE(IDC_FOG_R, OnEnChangeFogR)
    ON_EN_CHANGE(IDC_FOG_G, OnEnChangeFogG)
    ON_EN_CHANGE(IDC_FOG_B, OnEnChangeFogB)
    ON_EN_CHANGE(IDC_FOG_A, OnEnChangeFogA)
    ON_EN_CHANGE(IDC_FOG_START, OnEnChangeFogStart)
    ON_EN_CHANGE(IDC_FOG_END, OnEnChangeFogEnd)
    ON_EN_CHANGE(IDC_FOG_DENSITY, OnEnChangeFogDensity)
    ON_BN_CLICKED(IDC_DIFFUSE_BTN, OnBnClickedDiffuseBtn)
END_MESSAGE_MAP()

// BSPEditor wrapping code

static const char* wrap(engine::FogType value)
{
    switch( value )
    {
    case engine::fogNone: return "None";
    case engine::fogExp: return "Exp";
    case engine::fogExp2: return "Exp2";
    case engine::fogLinear: return "Linear";    
    default:
        assert( !"shouldn't be here!" );
        return "None";
    }
}

static engine::FogType wrap(const char* value)
{
    if( strcmp( value, "None" ) == 0 ) return engine::fogNone;
    else if( strcmp( value, "Exp" ) == 0 ) return engine::fogExp;
    else if( strcmp( value, "Exp2" ) == 0 ) return engine::fogExp2;
    else if( strcmp( value, "Linear" ) == 0 ) return engine::fogLinear;
    else return engine::fogNone;
}

// BSPEditor message handlers

void BSPEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    SetTimer( 1, 100,  NULL );

    CComboBox* box = static_cast<CComboBox*>( GetDlgItem( IDC_FOG_TYPE ) );
    box->ResetContent();
    box->AddString( "None" );
    box->AddString( "Exp" );
    box->AddString( "Exp2" );
    box->AddString( "Linear" );
    box->SelectString( -1, wrap( bsp->getFogType() ) );

    Vector4f fogColor = bsp->getFogColor();

    CEdit* edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_R ) );
    edit->SetWindowText( strformat( "%3.2f", fogColor[0] ).c_str() );

    edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_G ) );
    edit->SetWindowText( strformat( "%3.2f", fogColor[1] ).c_str() );

    edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_B ) );
    edit->SetWindowText( strformat( "%3.2f", fogColor[2] ).c_str() );

    edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_A ) );
    edit->SetWindowText( strformat( "%3.2f", fogColor[3] ).c_str() );

    edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_START ) );
    edit->SetWindowText( strformat( "%1.0f", bsp->getFogStart() ).c_str() );

    edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_END ) );
    edit->SetWindowText( strformat( "%1.0f", bsp->getFogEnd() ).c_str() );

    edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_DENSITY ) );
    edit->SetWindowText( strformat( "%7.6f", bsp->getFogDensity() ).c_str() );
}

void BSPEditor::OnCbnSelchangeFogType()
{
    isModified = true;    
}

void BSPEditor::OnTimer(UINT nIDEvent)
{
    if( nIDEvent == 1 && isModified)
    {
        isModified = false;

        CString string;
        CComboBox* box = static_cast<CComboBox*>( GetDlgItem( IDC_FOG_TYPE ) );
        box->GetWindowText( string );
        bsp->setFogType( wrap( string ) );

        Vector4f fogColor;

        CEdit* edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_R ) );
        edit->GetWindowText( string );
        fogColor[0] = float( atof( string ) );

        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_G ) );
        edit->GetWindowText( string );
        fogColor[1] = float( atof( string ) );

        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_B ) );
        edit->GetWindowText( string );
        fogColor[2] = float( atof( string ) );

        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_A ) );
        edit->GetWindowText( string );
        fogColor[3] = float( atof( string ) );

        bsp->setFogColor( fogColor );

        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_START ) );
        edit->GetWindowText( string );
        bsp->setFogStart( float( atof( string ) ) );
        
        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_END ) );
        edit->GetWindowText( string );
        bsp->setFogEnd( float( atof( string ) ) );
    
        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_DENSITY ) );
        edit->GetWindowText( string );
        bsp->setFogDensity( float( atof( string ) ) );
    }

    CDialog::OnTimer(nIDEvent);
}

void BSPEditor::OnEnChangeFogR()
{
    isModified = true;
}

void BSPEditor::OnEnChangeFogG()
{
    isModified = true;
}

void BSPEditor::OnEnChangeFogB()
{
    isModified = true;
}

void BSPEditor::OnEnChangeFogA()
{
    isModified = true;
}

void BSPEditor::OnEnChangeFogStart()
{
    isModified = true;
}

void BSPEditor::OnEnChangeFogEnd()
{
    isModified = true;
}

void BSPEditor::OnEnChangeFogDensity()
{
    isModified = true;
}

void BSPEditor::OnBnClickedDiffuseBtn()
{
    CColorDialog* colorDlg = new CColorDialog( 0, CC_ANYCOLOR | CC_FULLOPEN, this );
    if( colorDlg->DoModal() )
    {
        Vector4f fogColor = bsp->getFogColor();
        fogColor[0] = float( GetRValue( colorDlg->GetColor() ) ) / 255;
        fogColor[1] = float( GetGValue( colorDlg->GetColor() ) ) / 255;
        fogColor[2] = float( GetBValue( colorDlg->GetColor() ) ) / 255;
        
        CEdit* edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_R ) );
        edit->SetWindowText( strformat( "%3.2f", fogColor[0] ).c_str() );
        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_G ) );
        edit->SetWindowText( strformat( "%3.2f", fogColor[1] ).c_str() );
        edit = static_cast<CEdit*>( GetDlgItem( IDC_FOG_B ) );
        edit->SetWindowText( strformat( "%3.2f", fogColor[2] ).c_str() );

        bsp->setFogColor( fogColor );
    }
    delete colorDlg;
}