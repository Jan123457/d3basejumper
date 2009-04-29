#if !defined(AFX_SHADEREDIT_H__6904F097_D1E8_44FA_8D44_7E7A87A2ACBB__INCLUDED_)
#define AFX_SHADEREDIT_H__6904F097_D1E8_44FA_8D44_7E7A87A2ACBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// shaderedit.h : header file
//

#include "texchoose.h"
#include "effectedit.h"

/////////////////////////////////////////////////////////////////////////////
// ShaderEditor dialog

class ShaderEditor : public CDialog
{
public:
    engine::IShader* shader;

private:
    bool            isModified;
    CColorDialog*   colorDlg;
    TextureChooser* textureChooser;
    EffectEditor*   effectEditor;

private:
    void updateControls(void);
    bool validateControls(void);
    bool validateControl(int dlgId);
    bool validateDWORDControl(int dlgId);

private:
    void enableLayer(int layerId, bool enable);
    CButton* getLayerTextureBtn(int layerId);
    CComboBox* getLayerUVIDBox(int layerId);
    CComboBox* getLayerBlendingBox(int layerId);
    CString& getLayerUVID(int layerId);
    CString& getLayerBlending(int layerId);
    float& getLayerConstantR(int layerId);
    float& getLayerConstantG(int layerId);
    float& getLayerConstantB(int layerId);
    float& getLayerConstantA(int layerId);

// Construction
public:
	ShaderEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ShaderEditor)
	enum { IDD = IDD_SHADER_EDITOR };
	BOOL	flagAlphaBlending;
	BOOL	flagAlphaTesting;
	BOOL	flagCollision;
	BOOL	flagCulling;
	CString	alphaBlendingDst;
	CString	alphaBlendingOp;
	CString	alphaBlendingSrc;
	CString	alphaTestingFunct;
	float	diffuseA;
	float	diffuseB;
	float	diffuseG;
	float	diffuseR;
	CString	effectName;
	CString	layer0Blending;
	float	layer0ConstantA;
	float	layer0ConstantB;
	float	layer0ConstantG;
	float	layer0ConstantR;
	CString	layer0UVID;
	CString	layer1Blending;
	float	layer1ConstantA;
	float	layer1ConstantB;
	float	layer1ConstantG;
	float	layer1ConstantR;
	CString	layer1UVID;
	CString	layer2Blending;
	float	layer2ConstantA;
	float	layer2ConstantB;
	float	layer2ConstantG;
	float	layer2ConstantR;
	CString	layer2UVID;
	CString	layer3Blending;
	float	layer3ConstantA;
	float	layer3ConstantB;
	float	layer3ConstantG;
	float	layer3ConstantR;
	CString	layer3UVID;
	CString	normalMapUVID;
	float	pixelArgumentA;
	float	pixelArgumentB;
	float	pixelArgumentG;
	CString	pixelArgumentName;
	float	pixelArgumentR;
	float	specularA;
	float	specularB;
	float	specularG;
	float	specularPower;
	float	specularR;
	float	vertexArgumentA;
	float	vertexArgumentB;
	float	vertexArgumentG;
	CString	vertexArgumentName;
	float	vertexArgumentR;
	DWORD	alphaTestingRef;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ShaderEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ShaderEditor)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnFlagCulling();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFlagCollision();
	afx_msg void OnFlagAlphaBlending();
	afx_msg void OnFlagAlphaTesting();
	afx_msg void OnSelchangeAlphaBlendingSrc();
	afx_msg void OnSelchangeAlphaBlendingOp();
	afx_msg void OnSelchangeAlphaBlendingDst();
	afx_msg void OnSelchangeAlphaTestingFunct();
	afx_msg void OnChangeAlphaTestingRef();
	afx_msg void OnChangeDiffuseR();
	afx_msg void OnChangeDiffuseG();
	afx_msg void OnChangeDiffuseB();
	afx_msg void OnChangeDiffuseA();
	afx_msg void OnChangeSpecularR();
	afx_msg void OnChangeSpecularG();
	afx_msg void OnChangeSpecularB();
	afx_msg void OnChangeSpecularA();
	afx_msg void OnChangeSpecularPower();
	afx_msg void OnDiffuseBtn();
	afx_msg void OnSpecularBtn();
	afx_msg void OnLayer0TextureBtn();
	afx_msg void OnLayer1TextureBtn();
	afx_msg void OnSelchangeLayer0Uvid();
	afx_msg void OnSelchangeLayer1Uvid();
	afx_msg void OnSelchangeLayer2Uvid();
	afx_msg void OnSelchangeLayer3Uvid();
	afx_msg void OnSelchangeLayer0Blending();
	afx_msg void OnSelchangeLayer1Blending();
	afx_msg void OnSelchangeLayer2Blending();
	afx_msg void OnSelchangeLayer3Blending();
	afx_msg void OnChangeLayer0ConstantR();
	afx_msg void OnChangeLayer0ConstantG();
	afx_msg void OnChangeLayer0ConstantB();
	afx_msg void OnChangeLayer0ConstantA();
	afx_msg void OnChangeLayer1ConstantR();
	afx_msg void OnChangeLayer1ConstantG();
	afx_msg void OnChangeLayer1ConstantB();
	afx_msg void OnChangeLayer1ConstantA();
	afx_msg void OnChangeLayer2ConstantR();
	afx_msg void OnChangeLayer2ConstantG();
	afx_msg void OnChangeLayer2ConstantB();
	afx_msg void OnChangeLayer2ConstantA();
	afx_msg void OnChangeLayer3ConstantR();
	afx_msg void OnChangeLayer3ConstantG();
	afx_msg void OnChangeLayer3ConstantB();
	afx_msg void OnChangeLayer3ConstantA();
	afx_msg void OnNormalMapBtn();
	afx_msg void OnSelchangeNormalMapUvid();
	afx_msg void OnEffect();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedEnvMapBtn();
    // lightset identifier
    unsigned int lightset;
    afx_msg void OnEnChangeLightset();
    afx_msg void OnBnClickedLayer2TextureBtn();
    afx_msg void OnBnClickedLayer3TextureBtn();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADEREDIT_H__6904F097_D1E8_44FA_8D44_7E7A87A2ACBB__INCLUDED_)
