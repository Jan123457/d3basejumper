#if !defined(AFX_HELPDLG_H__8D3EA7D5_8534_41AA_AF94_E65CAF1FB491__INCLUDED_)
#define AFX_HELPDLG_H__8D3EA7D5_8534_41AA_AF94_E65CAF1FB491__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// helpdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HelpDialog dialog

class HelpDialog : public CDialog
{
// Construction
public:
	HelpDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(HelpDialog)
	enum { IDD = IDD_HELPDIALOG_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HelpDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(HelpDialog)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPDLG_H__8D3EA7D5_8534_41AA_AF94_E65CAF1FB491__INCLUDED_)
