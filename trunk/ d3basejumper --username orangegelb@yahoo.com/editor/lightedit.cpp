// lightedit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "lightedit.h"
#include ".\lightedit.h"
#include "../common/istring.h"

// LightEditor dialog

IMPLEMENT_DYNAMIC(LightEditor, CDialog)
LightEditor::LightEditor(CWnd* pParent /*=NULL*/)
	: CDialog(LightEditor::IDD, pParent)
{
    light = NULL;
    _isModified = false;
    _colorDialog = NULL;
}

LightEditor::~LightEditor()
{
}

void LightEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(LightEditor, CDialog)
//    ON_WM_SHOWWINDOW()
    ON_EN_CHANGE(IDC_ATTENUATION_0, OnEnChangeAttenuation0)
    ON_EN_CHANGE(IDC_ATTENUATION_1, OnEnChangeAttenuation1)
    ON_EN_CHANGE(IDC_ATTENUATION_2, OnEnChangeAttenuation2)
    ON_EN_CHANGE(IDC_DIFFUSE_B, OnEnChangeDiffuseB)
    ON_EN_CHANGE(IDC_DIFFUSE_G, OnEnChangeDiffuseG)
    ON_EN_CHANGE(IDC_DIFFUSE_R, OnEnChangeDiffuseR)
    ON_EN_CHANGE(IDC_PHI, OnEnChangePhi)
    ON_EN_CHANGE(IDC_RADIUS, OnEnChangeRadius)
    ON_EN_CHANGE(IDC_SPECULAR_B, OnEnChangeSpecularB)
    ON_EN_CHANGE(IDC_SPECULAR_G, OnEnChangeSpecularG)
    ON_EN_CHANGE(IDC_SPECULAR_R, OnEnChangeSpecularR)
    ON_EN_CHANGE(IDC_THETA, OnEnChangeTheta)
    ON_BN_CLICKED(IDC_SPECULAR_BTN, OnBnClickedSpecularBtn)
    ON_BN_CLICKED(IDC_DIFFUSE_BTN, OnBnClickedDiffuseBtn)
    ON_WM_SHOWWINDOW()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// LightEditor message handlers

void LightEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    assert( light );

    if( _colorDialog == NULL )
    {
        _colorDialog = new CColorDialog( 0, CC_ANYCOLOR | CC_FULLOPEN, this );
    }

    lightset()->SetWindowText( strformat( "%u", light->getLightset() ).c_str() );

    diffuseR()->SetWindowText( strformat( "%3.2f", light->getDiffuseColor()[0] ).c_str() );
    diffuseG()->SetWindowText( strformat( "%3.2f", light->getDiffuseColor()[1] ).c_str() );
    diffuseB()->SetWindowText( strformat( "%3.2f", light->getDiffuseColor()[2] ).c_str() );
    diffuseA()->SetWindowText( "" );

    specularR()->SetWindowText( strformat( "%3.2f", light->getSpecularColor()[0] ).c_str() );
    specularG()->SetWindowText( strformat( "%3.2f", light->getSpecularColor()[1] ).c_str() );
    specularB()->SetWindowText( strformat( "%3.2f", light->getSpecularColor()[2] ).c_str() );
    specularA()->SetWindowText( "" );

    diffuseA()->EnableWindow( false );
    specularA()->EnableWindow( false );
    
    switch( light->getType() )
    {
    case engine::ltAmbient:
    case engine::ltDirectional:
        radius()->EnableWindow( false );
        radius()->SetWindowText( "" );
        attenuation0()->EnableWindow( false );
        attenuation0()->SetWindowText( "" );
        attenuation1()->EnableWindow( false );
        attenuation1()->SetWindowText( "" );
        attenuation2()->EnableWindow( false );
        attenuation2()->SetWindowText( "" );
        theta()->EnableWindow( false );
        theta()->SetWindowText( "" );
        phi()->EnableWindow( false );
        phi()->SetWindowText( "" );
        break;
    case engine::ltPoint:
        radius()->EnableWindow( true );
        radius()->SetWindowText( strformat( "%3.2f", light->getRange() ).c_str() );
        attenuation0()->EnableWindow( true );
        attenuation0()->SetWindowText( strformat( "%9.8f", light->getAttenuation()[0] ).c_str() );
        attenuation1()->EnableWindow( true );
        attenuation1()->SetWindowText( strformat( "%9.8f", light->getAttenuation()[1] ).c_str() );
        attenuation2()->EnableWindow( true );
        attenuation2()->SetWindowText( strformat( "%9.8f", light->getAttenuation()[2] ).c_str() );
        theta()->EnableWindow( false );
        theta()->SetWindowText( "" );
        phi()->EnableWindow( false );
        theta()->SetWindowText( "" );
        break;
    case engine::ltSpot:
        radius()->EnableWindow( true );
        radius()->SetWindowText( strformat( "%3.2f", light->getRange() ).c_str() );
        attenuation0()->EnableWindow( true );
        attenuation0()->SetWindowText( strformat( "%9.8f", light->getAttenuation()[0] ).c_str() );
        attenuation1()->EnableWindow( true );
        attenuation1()->SetWindowText( strformat( "%9.8f", light->getAttenuation()[1] ).c_str() );
        attenuation2()->EnableWindow( true );
        attenuation2()->SetWindowText( strformat( "%9.8f", light->getAttenuation()[2] ).c_str() );
        theta()->EnableWindow( true );
        theta()->SetWindowText( strformat( "%6.5f", light->getTheta() ).c_str() );
        phi()->EnableWindow( true );
        theta()->SetWindowText( strformat( "%6.5f", light->getPhi() ).c_str() );
        break;
    }

    this->SetTimer( 1, 50, NULL );
}

void LightEditor::OnEnChangeAttenuation0()
{
    _isModified = true;
}

void LightEditor::OnEnChangeAttenuation1()
{
    _isModified = true;
}

void LightEditor::OnEnChangeAttenuation2()
{
    _isModified = true;
}

void LightEditor::OnEnChangeDiffuseB()
{
    _isModified = true;
}

void LightEditor::OnEnChangeDiffuseG()
{
    _isModified = true;
}

void LightEditor::OnEnChangeDiffuseR()
{
    _isModified = true;
}

void LightEditor::OnEnChangePhi()
{
    _isModified = true;
}

void LightEditor::OnEnChangeRadius()
{
    _isModified = true;
}

void LightEditor::OnEnChangeSpecularB()
{
    _isModified = true;
}

void LightEditor::OnEnChangeSpecularG()
{
    _isModified = true;
}

void LightEditor::OnEnChangeSpecularR()
{
    _isModified = true;
}

void LightEditor::OnEnChangeTheta()
{
    _isModified = true;
}

void LightEditor::OnBnClickedSpecularBtn()
{
    if( _colorDialog->DoModal() )
    {
        specularR()->SetWindowText( strformat( "%4.3f", float( GetRValue( _colorDialog->GetColor() ) ) / 255 ).c_str() );
        specularG()->SetWindowText( strformat( "%4.3f", float( GetGValue( _colorDialog->GetColor() ) ) / 255 ).c_str() );
        specularB()->SetWindowText( strformat( "%4.3f", float( GetBValue( _colorDialog->GetColor() ) ) / 255 ).c_str() );
        _isModified = true;
    }
}

void LightEditor::OnBnClickedDiffuseBtn()
{
    if( _colorDialog->DoModal() )
    {
        diffuseR()->SetWindowText( strformat( "%4.3f", float( GetRValue( _colorDialog->GetColor() ) ) / 255 ).c_str() );
        diffuseG()->SetWindowText( strformat( "%4.3f", float( GetGValue( _colorDialog->GetColor() ) ) / 255 ).c_str() );
        diffuseB()->SetWindowText( strformat( "%4.3f", float( GetBValue( _colorDialog->GetColor() ) ) / 255 ).c_str() );
        _isModified = true;
    }    
}

void LightEditor::OnTimer(UINT nIDEvent)
{
    if( nIDEvent == 1 && _isModified )
    {
        _isModified = true;

        CString string;
        Vector4f color;

        diffuseR()->GetWindowText( string ); color[0] = float( atof( string ) );
        diffuseG()->GetWindowText( string ); color[1] = float( atof( string ) );
        diffuseB()->GetWindowText( string ); color[2] = float( atof( string ) );
        color[3] = 1.0f;
        light->setDiffuseColor( color );

        if( light->getType() != engine::ltAmbient )
        {
            specularR()->GetWindowText( string ); color[0] = float( atof( string ) );
            specularG()->GetWindowText( string ); color[1] = float( atof( string ) );
            specularB()->GetWindowText( string ); color[2] = float( atof( string ) );
            color[3] = 1.0f;
            light->setSpecularColor( color );
        }

        if( light->getType() != engine::ltAmbient && 
            light->getType() != engine::ltDirectional )
        {
            radius()->GetWindowText( string );
            light->setRange( float( atof( string ) ) );
        }

        if( light->getType() != engine::ltAmbient && 
            light->getType() != engine::ltDirectional )
        {
            Vector3f attenuation;
            attenuation0()->GetWindowText( string ); attenuation[0] = float( atof( string ) );
            attenuation1()->GetWindowText( string ); attenuation[1] = float( atof( string ) );
            attenuation2()->GetWindowText( string ); attenuation[2] = float( atof( string ) );
            light->setAttenuation( attenuation );
        }

        if( light->getType() == engine::ltSpot )
        {
            theta()->GetWindowText( string );
            light->setTheta( float( atof( string ) ) );
            phi()->GetWindowText( string );
            light->setPhi( float( atof( string ) ) );
        }

        lightset()->GetWindowText( string );
        light->setLightset( atoi( string ) );
    }

    CDialog::OnTimer(nIDEvent);
}
