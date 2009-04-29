#pragma once


// BSPEditor dialog

class BSPEditor : public CDialog
{
	DECLARE_DYNAMIC(BSPEditor)

private:
    bool isModified;

public:
    engine::IBSP* bsp;

public:
	BSPEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~BSPEditor();

// Dialog Data
	enum { IDD = IDD_BSP_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnCbnSelchangeFogType();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnEnChangeFogR();
    afx_msg void OnEnChangeFogG();
    afx_msg void OnEnChangeFogB();
    afx_msg void OnEnChangeFogA();
    afx_msg void OnEnChangeFogStart();
    afx_msg void OnEnChangeFogEnd();
    afx_msg void OnEnChangeFogDensity();
    afx_msg void OnBnClickedDiffuseBtn();
};
