// engineinfo.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "engineinfo.h"
#include "../common/istring.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EngineInfo dialog


EngineInfo::EngineInfo(CWnd* pParent /*=NULL*/)
	: CDialog(EngineInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(EngineInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void EngineInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EngineInfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EngineInfo, CDialog)
	//{{AFX_MSG_MAP(EngineInfo)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EngineInfo message handlers

void EngineInfo::OnTimer(UINT nIDEvent) 
{
	if( nIDEvent == 1 )
    {
        engine::IEngine* iEngine;
        if( askInterfaceT( "Engine", &iEngine ) )
        {
            engine::RenderStatictics* statistics = iEngine->getStatictics();

            GetDlgItem( IDC_FPS )->SetWindowText( strformat( "%2.1f", fps ).c_str() );
            GetDlgItem( IDC_BSP_TOTAL )->SetWindowText( strformat( "%d", statistics->bspTotal ).c_str() );
            GetDlgItem( IDC_BSP_RENDERED )->SetWindowText( strformat( "%d", statistics->bspRendered ).c_str() );
            GetDlgItem( IDC_ATOMICS_RENDERED )->SetWindowText( strformat( "%d", statistics->atomicsRendered ).c_str() );
            GetDlgItem( IDC_ALPHA_RENDERED )->SetWindowText( strformat( "%d", statistics->alphaObjectsRendered ).c_str() );
            GetDlgItem( IDC_SHADER_CACHE_HITS )->SetWindowText( strformat( "%d", statistics->shaderCacheHits ).c_str() );
        }
    }
	
	CDialog::OnTimer(nIDEvent);
}

BOOL EngineInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	
    SetTimer( 1, 50,  NULL );
	
    return TRUE;
}
