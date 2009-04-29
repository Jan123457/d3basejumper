#if !defined(AFX_GEOEDIT_H__35C98D0C_8B50_4A67_84A7_B66DDAE81C0A__INCLUDED_)
#define AFX_GEOEDIT_H__35C98D0C_8B50_4A67_84A7_B66DDAE81C0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// geoedit.h : header file
//

#include "effectedit.h"

/////////////////////////////////////////////////////////////////////////////
// GeometryEditor dialog

class GeometryEditor : public CDialog
{
public:
    EffectEditor* effectEditor;
    engine::IGeometry* geometry;

// Construction
public:
	GeometryEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GeometryEditor)
	enum { IDD = IDD_GEOMETRY_EDITOR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GeometryEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GeometryEditor)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnEffect();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOEDIT_H__35C98D0C_8B50_4A67_84A7_B66DDAE81C0A__INCLUDED_)
