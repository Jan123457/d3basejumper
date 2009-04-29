#if !defined(AFX_EFFECTEDIT_H__A0112532_6F84_42C9_BBC5_14797987EF70__INCLUDED_)
#define AFX_EFFECTEDIT_H__A0112532_6F84_42C9_BBC5_14797987EF70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// effectedit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EffectEditor dialog

class EffectEditor : public CDialog
{
private:
    engine::IShader*   _dstShader;
    engine::IGeometry* _dstGeometry;
public:
    void setDestination(engine::IShader* shader) 
    { 
        _dstGeometry = NULL;
        _dstShader = shader; 
    }
    void setDestination(engine::IGeometry* geometry) 
    { 
        _dstGeometry = geometry; 
        _dstShader = NULL; 
    }
    engine::IEffect* getEffect(void)
    {
        assert( _dstShader && !_dstGeometry || !_dstShader && _dstGeometry );
        if( _dstShader ) return _dstShader->getEffect();
        else if( _dstGeometry ) return _dstGeometry->getEffect();
        else return NULL;
    }
    void setEffect(engine::IEffect* effect)
    {
        assert( _dstShader && !_dstGeometry || !_dstShader && _dstGeometry );
        if( _dstShader ) _dstShader->setEffect( effect );
        else if( _dstGeometry ) _dstGeometry->setEffect( effect );
    }

public:
    bool               isModified;
    

// Construction
public:
	EffectEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(EffectEditor)
	enum { IDD = IDD_EFFECT_EDITOR };
	CEdit	argR;
	CEdit	argG;
	CEdit	argB;
	CEdit	argA;
	CListCtrl	arguments;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EffectEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(EffectEditor)
	afx_msg void OnOkButton();
	afx_msg void OnCancelButton();
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeEffectname();
	afx_msg void OnClickArglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeR();
	afx_msg void OnChangeG();
	afx_msg void OnChangeB();
	afx_msg void OnChangeA();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTEDIT_H__A0112532_6F84_42C9_BBC5_14797987EF70__INCLUDED_)
