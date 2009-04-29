#pragma once


// LightEditor dialog

class LightEditor : public CDialog
{
public:
	DECLARE_DYNAMIC(LightEditor)
public:
    engine::ILight* light;
public:
	LightEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~LightEditor();
private:
    CColorDialog* _colorDialog;
    bool          _isModified;
private:
    inline CEdit* diffuseR(void) { return static_cast<CEdit*>( GetDlgItem( IDC_DIFFUSE_R ) ); }
    inline CEdit* diffuseG(void) { return static_cast<CEdit*>( GetDlgItem( IDC_DIFFUSE_G ) ); }
    inline CEdit* diffuseB(void) { return static_cast<CEdit*>( GetDlgItem( IDC_DIFFUSE_B ) ); }
    inline CEdit* diffuseA(void) { return static_cast<CEdit*>( GetDlgItem( IDC_DIFFUSE_A ) ); }
    inline CEdit* specularR(void) { return static_cast<CEdit*>( GetDlgItem( IDC_SPECULAR_R ) ); }
    inline CEdit* specularG(void) { return static_cast<CEdit*>( GetDlgItem( IDC_SPECULAR_G ) ); }
    inline CEdit* specularB(void) { return static_cast<CEdit*>( GetDlgItem( IDC_SPECULAR_B ) ); }
    inline CEdit* specularA(void) { return static_cast<CEdit*>( GetDlgItem( IDC_SPECULAR_A ) ); }
    inline CEdit* radius(void) { return static_cast<CEdit*>( GetDlgItem( IDC_RADIUS ) ); }
    inline CEdit* attenuation0(void) { return static_cast<CEdit*>( GetDlgItem( IDC_ATTENUATION_0 ) ); }
    inline CEdit* attenuation1(void) { return static_cast<CEdit*>( GetDlgItem( IDC_ATTENUATION_1 ) ); }
    inline CEdit* attenuation2(void) { return static_cast<CEdit*>( GetDlgItem( IDC_ATTENUATION_2 ) ); }
    inline CEdit* theta(void) { return static_cast<CEdit*>( GetDlgItem( IDC_THETA ) ); }
    inline CEdit* phi(void) { return static_cast<CEdit*>( GetDlgItem( IDC_PHI ) ); }
    inline CEdit* lightset(void) { return static_cast<CEdit*>( GetDlgItem( IDC_LIGHTSET ) ); }


// Dialog Data
	enum { IDD = IDD_LIGHT_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnEnChangeAttenuation0();
    afx_msg void OnEnChangeAttenuation1();
    afx_msg void OnEnChangeAttenuation2();
    afx_msg void OnEnChangeDiffuseB();
    afx_msg void OnEnChangeDiffuseG();
    afx_msg void OnEnChangeDiffuseR();
    afx_msg void OnEnChangePhi();
    afx_msg void OnEnChangeRadius();
    afx_msg void OnEnChangeSpecularB();
    afx_msg void OnEnChangeSpecularG();
    afx_msg void OnEnChangeSpecularR();
    afx_msg void OnEnChangeTheta();
    afx_msg void OnBnClickedSpecularBtn();
    afx_msg void OnBnClickedDiffuseBtn();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnTimer(UINT nIDEvent);
};
