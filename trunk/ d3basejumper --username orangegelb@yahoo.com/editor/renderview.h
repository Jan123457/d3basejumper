#if !defined(AFX_RENDERVIEW_H__6CD6CD58_39E9_478A_85D8_88886B8BB4F0__INCLUDED_)
#define AFX_RENDERVIEW_H__6CD6CD58_39E9_478A_85D8_88886B8BB4F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// renderview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RenderView dialog

class RenderView : public CDialog
{
private:
    unsigned int _framesRendered;
    unsigned int _tickStart;
    float        _fps;

private:
    // last mouse position
    CPoint   _lastPoint;

private:
    // camera manipulation
    float    _spectateTilt;
    float    _spectateTurn;
    float    _spectateOffset;
    Vector3f _lookAtPos;
    Matrix4f _cameraMatrix;
    bool     _manipulateCamera;    
    bool     _lButton;
    bool     _rButton;
    bool     _mButton;

private:
    void switchGameMode(void);

public:
    float getFPS(void) { return _fps; }
    void resetCameraMatrix(void);
    void buildCameraMatrix(void);
	void renderScene(engine::IBSP* bsp);

private:
    bool   _gameMode;
    CPoint _windowCenter;

// Construction
public:
	RenderView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(RenderView)
	enum { IDD = IDD_RENDERVIEW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RenderView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RenderView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERVIEW_H__6CD6CD58_39E9_478A_85D8_88886B8BB4F0__INCLUDED_)
