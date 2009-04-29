// geoedit.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "geoedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GeometryEditor dialog


GeometryEditor::GeometryEditor(CWnd* pParent /*=NULL*/)
	: CDialog(GeometryEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(GeometryEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    effectEditor = NULL;
    geometry = NULL;
}


void GeometryEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GeometryEditor)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GeometryEditor, CDialog)
	//{{AFX_MSG_MAP(GeometryEditor)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_EFFECT, OnEffect)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeometryEditor message handlers

void GeometryEditor::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	assert( geometry );

    if( geometry->getName() ) SetWindowText( geometry->getName() );

    const char* effectName = "No effect";
    if( geometry->getEffect() ) effectName = geometry->getEffect()->getName();
    GetDlgItem( IDC_EFFECT )->SetWindowText( effectName );    
}

BOOL GeometryEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	effectEditor = new EffectEditor( this );
    effectEditor->Create( IDD_EFFECT_EDITOR, this );

    SetTimer( 1, 100, NULL );
	
	return TRUE;
}

void GeometryEditor::OnEffect() 
{
    RECT buttonRect;
    GetDlgItem( IDC_EFFECT )->GetWindowRect( &buttonRect );

    RECT dialogRect;
    effectEditor->GetWindowRect( &dialogRect );
    
    effectEditor->SetWindowPos( 
        NULL, 
        buttonRect.left,
        buttonRect.top,
        dialogRect.right-dialogRect.left, 
        dialogRect.bottom-dialogRect.top, 
        0 
    );

	effectEditor->setDestination( geometry );
    effectEditor->ShowWindow( SW_SHOW );
}

void GeometryEditor::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == 1 )
    {
        if( effectEditor->IsWindowVisible() )
        {
            RECT buttonRect;
            GetDlgItem( IDC_EFFECT )->GetWindowRect( &buttonRect );

            RECT dialogRect;
            effectEditor->GetWindowRect( &dialogRect );
    
            effectEditor->SetWindowPos( 
                NULL, 
                buttonRect.left,
                buttonRect.top,
                dialogRect.right-dialogRect.left, 
                dialogRect.bottom-dialogRect.top, 
                0 
            );
        }

        if( geometry )
        {
            const char* effectName = "No effect";
            if( geometry->getEffect() ) effectName = geometry->getEffect()->getName();
            GetDlgItem( IDC_EFFECT )->SetWindowText( effectName );
        }
    }
	
	CDialog::OnTimer(nIDEvent);
}

void GeometryEditor::OnClose() 
{
	geometry = NULL;
	CDialog::OnClose();
}
