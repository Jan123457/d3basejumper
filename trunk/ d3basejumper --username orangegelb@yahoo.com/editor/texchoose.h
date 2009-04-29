#if !defined(AFX_TEXCHOOSE_H__B6E90AD7_2674_451B_8B4E_53BE45ACC4BC__INCLUDED_)
#define AFX_TEXCHOOSE_H__B6E90AD7_2674_451B_8B4E_53BE45ACC4BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// texchoose.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TextureChooser dialog

enum TextureChooserState
{
    tcsDontCare, // chooser is in "don't care" state
    tcsOk        // dialog was succesful, texture field contains selected texture
};

class TextureChooser : public CDialog
{
public:
    TextureChooserState state;
    int                 layerId; // -1 = normal map, -2 = environment map
    engine::ITexture*   texture;

private:
    bool isTextureChanged;
    bool isModified;

private:
    bool validateControls(void);

// Construction
public:
	TextureChooser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TextureChooser)
	enum { IDD = IDD_TEXTURE_CHOOSER };
	CComboBox	vAddressing;
	CComboBox	uAddressing;
	CEdit	maxAnisotropy;
	CComboBox	mipFilter;
	CComboBox	minFilter;
	CComboBox	magFilter;
	CComboBox	textureName;
    CEdit	mipmapLODBias;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TextureChooser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TextureChooser)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnEditupdateCombo1();
	afx_msg void OnButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMagfilter();
	afx_msg void OnSelchangeMinfilter();
	afx_msg void OnSelchangeMipfilter();
	afx_msg void OnChangeAnisotropy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeUaddressing();
	afx_msg void OnSelchangeVaddressing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnEnChangeMipmaplodbias();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXCHOOSE_H__B6E90AD7_2674_451B_8B4E_53BE45ACC4BC__INCLUDED_)
