// renderview.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "renderview.h"
#include "mainform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// RenderView dialog


RenderView::RenderView(CWnd* pParent /*=NULL*/)
	: CDialog(RenderView::IDD, pParent)
{
	//{{AFX_DATA_INIT(RenderView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    _gameMode = false;
    _manipulateCamera = false;
    _lButton = _rButton = _mButton = false;

    _lookAtPos.set( 0,0,0 );    
    _spectateTilt = _spectateTurn = 0;    
    _spectateOffset = -10;

    _framesRendered = 0;
    _tickStart = GetTickCount();
    _fps = 0;
}


void RenderView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RenderView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RenderView, CDialog)
	//{{AFX_MSG_MAP(RenderView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RenderView message handlers

LRESULT RenderView::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    // build MSG structure
    MSG msg;
    msg.hwnd = m_hWnd, msg.lParam = lParam, msg.wParam = wParam, msg.message = message;

    // translate keyboard messages, ignore repeat events
    if( ( msg.message == WM_KEYDOWN && !(msg.lParam & 0x40000000) ) || 
        msg.message == WM_KEYUP )
    {
        if( _gameMode ) 
        {
            getCore()->activate(
                mainwnd::TriggerMainwnd::tid,
                &Holder<MSG>(msg)
            );
        }
    }

    // translate character code to users
    else if( msg.message == WM_CHAR )
    {
        if( _gameMode ) getCore()->activate(
            mainwnd::TriggerMainwnd::tid,
            &Holder<MSG>(msg)
        );
    }

    // translate mousemove messages
    else if( msg.message == WM_MOUSEMOVE )
    {
        if( GetFocus() != this ) SetFocus();

        CPoint point( GET_X_LPARAM( msg.lParam ), GET_Y_LPARAM( msg.lParam ) );
        ClientToScreen( &point );
        if( _gameMode && ( point.x != _windowCenter.x || point.y != _windowCenter.y ) )
        {
            int x = _windowCenter.x - point.x;
            int y = _windowCenter.y - point.y;
            // make offset value
            msg.lParam = MAKELPARAM( x, y );
            SetCursorPos( _windowCenter.x, _windowCenter.y );
            // process messaging
            getCore()->activate(
                mainwnd::TriggerMainwnd::tid,
                &Holder<MSG>(msg)
            );
        }
    }

    // translate other mouse messages
    else if( msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONDOWN ||
             msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONDOWN ||
             msg.message == WM_RBUTTONUP || msg.message == WM_MBUTTONDOWN ||
             msg.message == WM_MBUTTONUP || msg.message == WM_MOUSEWHEEL 
           )
    {        
        if( _gameMode ) getCore()->activate(
            mainwnd::TriggerMainwnd::tid,
            &Holder<MSG>(msg)
        );
    }

    // translate window size-move messages
    else if( msg.message == WM_SIZE || msg.message == WM_MOVE ||
             msg.message == WM_SYSCOMMAND 
           )
    {
        // reset last mouse position
        RECT rect; GetClientRect( &rect ); ClientToScreen( &rect );
        _windowCenter.x = rect.left + ( rect.right - rect.left ) / 2;
        _windowCenter.y = rect.top + ( rect.bottom - rect.top ) / 2;

        getCore()->activate(
            mainwnd::TriggerMainwnd::tid,
            &Holder<MSG>(msg)
        );                
    }

    // translate killfocus & setfocus
    else if( msg.message == WM_KILLFOCUS || msg.message == WM_SETFOCUS )
    {
        getCore()->activate(
            mainwnd::TriggerMainwnd::tid,
            &Holder<MSG>(msg)
        );
    }

    // middle mouse button
    if( msg.message == WM_MBUTTONDOWN ) _mButton = true;
    if( msg.message == WM_MBUTTONUP ) _mButton = false;
    // switch game mode
    if( msg.message == WM_KEYDOWN && msg.wParam == 192 ) switchGameMode();

    return CDialog::WindowProc( message, wParam, lParam );
}

void RenderView::switchGameMode(void)
{
    _gameMode = !_gameMode;

    if( _gameMode )
    {
        SetCursorPos( _windowCenter.x, _windowCenter.y );
        ShowCursor( false );
    }
    else
    {
        ShowCursor( true );
    }
}

void RenderView::resetCameraMatrix(void)
{
    _lookAtPos.set( 0,0,0 );    
    _spectateTilt = _spectateTurn = 0;    
    _spectateOffset = 1000;
    buildCameraMatrix();
}

void RenderView::buildCameraMatrix(void)
{
    _cameraMatrix.set( 1,0,0,0,
                       0,1,0,0,
                       0,0,1,0,
                       0,0,0,1 );
    _cameraMatrix = iEngine()->rotateMatrix( _cameraMatrix, Vector3f( 1,0,0 ), -_spectateTilt );
    _cameraMatrix = iEngine()->rotateMatrix( _cameraMatrix, Vector3f( 0,1,0 ), _spectateTurn );
    _cameraMatrix = iEngine()->translateMatrix( _cameraMatrix, _lookAtPos );
    _cameraMatrix = iEngine()->translateMatrix( _cameraMatrix, Vector3f( _cameraMatrix[2][0], _cameraMatrix[2][1], _cameraMatrix[2][2] ) * _spectateOffset );
    iEngine()->getDefaultCamera()->getFrame()->setMatrix( _cameraMatrix );
}

void RenderView::renderScene(engine::IBSP* bsp)
{
    memset( iEngine()->getStatictics(), 0, sizeof(engine::RenderStatictics) );

    iEngine()->getDefaultCamera()->setFarClipPlane( 3000000 );
	iEngine()->getDefaultCamera()->beginScene( 
        engine::cmClearColor | engine::cmClearDepth | engine::cmClearStencil,
		Vector4f( 0.5f, 0.5f, 0.5f, 1 )
	);
	bsp->render();
	iEngine()->getDefaultCamera()->endScene();
	iEngine()->present();

    _framesRendered++;
    if( GetTickCount() - _tickStart > 250 )
    {
        _fps = float( _framesRendered ) * 1000.0f / float( GetTickCount() - _tickStart );
        _tickStart = GetTickCount();
        _framesRendered = 0;
    }
}

void RenderView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    _lButton = true;
	CDialog::OnLButtonDown(nFlags, point);
}

void RenderView::OnLButtonUp(UINT nFlags, CPoint point) 
{	
    _lButton = false;
	CDialog::OnLButtonUp(nFlags, point);
}

void RenderView::OnMouseMove(UINT nFlags, CPoint point) 
{
    if( !_gameMode )
    {
        // rotation mode
	    if( _manipulateCamera && _lButton )
        {
            _spectateTilt += point.y - _lastPoint.y;
            _spectateTurn -= point.x - _lastPoint.x;
            if( _spectateTurn > 360 ) _spectateTurn -= 360;
            if( _spectateTurn < -360 ) _spectateTurn += 360;
            if( _spectateTilt > 89 ) _spectateTilt = 89;
            if( _spectateTilt < -89 ) _spectateTilt = -89;
            buildCameraMatrix(); 
        }
        // translation mode
        if( _manipulateCamera && _rButton )
        {
            _spectateOffset += 50.0f * ( point.y - _lastPoint.y + point.x - _lastPoint.x );
            if( _spectateOffset < 100.0f ) _spectateOffset = 100.0f;
            buildCameraMatrix();
        }
        // move mode
        if( _manipulateCamera && _mButton ) 
        {
            _lookAtPos += 
                iEngine()->getDefaultCamera()->getFrame()->getRight() * float( point.x - _lastPoint.x ) * -_spectateOffset * 0.01f +
                iEngine()->getDefaultCamera()->getFrame()->getUp() * float( point.y - _lastPoint.y ) * _spectateOffset * 0.01f;
            buildCameraMatrix();
        }
    }
	
	CDialog::OnMouseMove(nFlags, point);

    _lastPoint = point;
}

void RenderView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if( !_gameMode )
    {
        if( nChar == 16 ) _manipulateCamera = true;
    }
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void RenderView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( !_gameMode )
    {
        if( nChar == 16 ) _manipulateCamera = false;
    }
	
	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

void RenderView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	_rButton = false;
	CDialog::OnRButtonUp(nFlags, point);
}

void RenderView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	_rButton = true;
	CDialog::OnRButtonDown(nFlags, point);
}

BOOL RenderView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
