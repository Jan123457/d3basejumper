// helpdlg.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "helpdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HelpDialog dialog


HelpDialog::HelpDialog(CWnd* pParent /*=NULL*/)
	: CDialog(HelpDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(HelpDialog)
	//}}AFX_DATA_INIT
}


void HelpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(HelpDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(HelpDialog, CDialog)
	//{{AFX_MSG_MAP(HelpDialog)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HelpDialog message handlers

void HelpDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}
