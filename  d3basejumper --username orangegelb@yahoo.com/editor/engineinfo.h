#if !defined(AFX_ENGINEINFO_H__20CFEA66_3622_43C9_9195_4F262A31A822__INCLUDED_)
#define AFX_ENGINEINFO_H__20CFEA66_3622_43C9_9195_4F262A31A822__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// engineinfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EngineInfo dialog

class EngineInfo : public CDialog
{
public:
    float fps;

// Construction
public:
	EngineInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EngineInfo)
	enum { IDD = IDD_ENGINE_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EngineInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(EngineInfo)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENGINEINFO_H__20CFEA66_3622_43C9_9195_4F262A31A822__INCLUDED_)
