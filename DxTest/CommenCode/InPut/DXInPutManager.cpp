#include "DXInPutManager.h"
#include "../DxGlobal/DxGlobal.h"

void DXInputManager::Init()
{
	DXWriteConsol(CC_WHITE,"DXInputManager::Init()");
	memset(m_Keys,0,256*(sizeof(bool)));
	memset(m_MouseButtons,0,5*(sizeof(bool)));

	m_bMouseMBtnDown=false;
	m_bMouseLBtnDown=false;
	m_bMouseRBtnDwon=false;

	m_KeyDownCount=0;

	m_LastLDrag_ScreenX=-1;
	m_LastLDrag_ScreenY=-1;
	m_LastRDrag_ScreenX=-1;
	m_LastRDrag_ScreenY=-1;

	m_MouseLBtnDragDeltaX=-1;
	m_MouseLBtnDragDeltaY=-1;
	m_MouseRBtnDragDeltaX=-1;
	m_MouseRBtnDragDeltaY=-1;

	m_MouseWheelDelta=0;

	m_bRBtnDraging=false;

	m_bLBtnDraging=false;
	m_LDragBeginPointX_Screen=0;
	m_LDragBeginPointY_Screen=0;
	m_LDragBeginPointX_Window=0;
	m_LDragBeginPointY_Window=0;
}

void DXInputManager::ResetPerFrame()
{
	//DXWriteConsol(CC_WHITE,"DXInputManager::ResetPerFrame()");
	m_MouseWheelDelta=0;
	m_MouseLBtnDragDeltaX=0;
	m_MouseLBtnDragDeltaY=0;
	m_MouseRBtnDragDeltaX=0;
	m_MouseRBtnDragDeltaY=0;
}

int DXInputManager::HandleMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//DXWriteConsol(CC_WHITE,"DXInputManager::HandleMsg");
	int r1=ProcessKeyBoardMsg(uMsg,wParam,lParam);
	if (r1==1)
		return 1;

	int r2=ProcessMouseMsg(uMsg,wParam,lParam);
	if (r2==1)
		return 1;

	return 0;
}

int DXInputManager::ProcessKeyBoardMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (uMsg==WM_KEYDOWN || uMsg==WM_SYSKEYDOWN || uMsg==WM_KEYUP || uMsg==WM_SYSKEYUP)
	{
		bool bKeyDown= (uMsg==WM_KEYDOWN||uMsg== WM_SYSKEYDOWN);
		m_Keys[(BYTE) (wParam&0xFF)]=bKeyDown;
	}
	return 0;
}

int DXInputManager::ProcessMouseMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//DXWriteConsol(CC_WHITE,"DXInputManager::ProcessMouseMsg");
	int res=0;
	switch (uMsg)
	{
	case  WM_LBUTTONDOWN:
		res=OnMouseLBtnDown(uMsg,wParam,lParam);
		break;
	case WM_LBUTTONUP:
		res=OnMouseLBtnUp(uMsg,wParam,lParam);
		break;
	case WM_RBUTTONDOWN:
		res=OnMOuseRBtnDown(uMsg,wParam,lParam);
		break;
	case WM_RBUTTONUP:
		res=OnMouseRBtnUp(uMsg,wParam,lParam);
		break;
	case WM_MBUTTONDOWN:
		res=OnMouseMBtnDwon(uMsg,wParam,lParam);
		break;
	case WM_MBUTTONUP:
		res=OnMouseMBtnUp(uMsg,wParam,lParam);
		break;
	case WM_MOUSEWHEEL:
		res=OnMouseWheel(uMsg,wParam,lParam);
		break;
	case WM_MOUSEMOVE:
		res=OnMouseMove(uMsg,wParam,lParam);
		break;
	default:
		res=0;
	}
	return 0;
}

bool DXInputManager::IsKeyDown(unsigned int key)
{
	return m_Keys[key];
}

void  DXInputManager::GetMouseLBtnDownPos_Screen(int* posX,int* posY)
{
	*posX=m_MouseLBtnDownPosX_Screen;
	*posY=m_MouseLBtnDownPosY_Screen;
}

void DXInputManager::GetMouseLBtnDownPos_Window(int* posX,int* posY)
{
	*posX=m_MouseLBtnDownPosX_Window;
	*posY=m_MouseLBtnDownPosY_Window;
}

void DXInputManager::GetMouseCurrentPos_Window(int* posX,int* posY)
{
	*posX=m_MousePosX_Window;
	*posY=m_MousePosY_Window;
}

int DXInputManager::GetWheelDistance()
{
	return m_MouseWheelDelta;
}

bool DXInputManager::IsMouseRBtnDraging()
{
	return m_bRBtnDraging;
}
bool DXInputManager::IsMouseLBtnDraging()
{
	return m_bLBtnDraging;
}

void DXInputManager::GetCurFramMouseLDragDelta(int* posX,int* posY)
{
	*posX=m_MouseLBtnDragDeltaX;
	*posY=m_MouseLBtnDragDeltaY;
}

void DXInputManager::GetCurFramMouseRDragDelta(int* posX,int* posY)
{
	*posX=m_MouseRBtnDragDeltaX;
	*posY=m_MouseRBtnDragDeltaY;
}

void DXInputManager::GetMouseLBtnDragingBeginPoint_Screen(int* posX,int* posY)
{
	*posX=m_LDragBeginPointX_Screen;
	*posY=m_LDragBeginPointY_Screen;
}

// void DXInputManager::GetMouseLBtnDownPos_Window(int* posX,int* posY)
// {
// 	*posX=m_LDragBeginPointX_Window;
// 	*posY=m_LDragBeginPointY_Window;
// }


int DXInputManager::OnMouseLBtnDown(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_bMouseLBtnDown=true;

	//记录鼠标左键按下的位置（窗口坐标系下）
	m_MouseLBtnDownPosX_Window=(int)LOWORD(lParam);
	m_MouseLBtnDownPosY_Window=(int)HIWORD(lParam);

	m_MousePosX_Window=m_MouseLBtnDownPosX_Window;
	m_MousePosY_Window=m_MouseLBtnDownPosY_Window;

	//记录鼠标左键按下的位置（屏幕坐标系下）
	POINT ptCurMousePos;
	::GetCursorPos(&ptCurMousePos);
	m_MouseLBtnDownPosX_Screen=ptCurMousePos.x;
	m_MouseLBtnDownPosY_Screen=ptCurMousePos.y;

	m_LastLDrag_ScreenX=m_MouseLBtnDownPosX_Screen;
	m_LastLDrag_ScreenY=m_MouseLBtnDownPosY_Screen;

	m_LDragBeginPointX_Screen=ptCurMousePos.x;
	m_LDragBeginPointY_Screen=ptCurMousePos.y;
	m_LDragBeginPointX_Window=m_MousePosX_Window;
	m_LDragBeginPointY_Window=m_MousePosY_Window;

	return 1;
}

int DXInputManager::OnMouseLBtnUp(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_bMouseLBtnDown=false;
	m_bLBtnDraging=false;

	m_MousePosX_Window=0;
	m_MousePosY_Window=0;

	m_LDragBeginPointX_Screen=0;
	m_LDragBeginPointY_Screen=0;
	m_LDragBeginPointX_Window=0;
	m_LDragBeginPointY_Window=0;

	return 1;
}

int DXInputManager::OnMOuseRBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_bMouseRBtnDwon=true;
	m_MousePosX_Window=( int )LOWORD( lParam );
	m_MousePosY_Window=( int )HIWORD( lParam );

	return 1;
}
int DXInputManager::OnMouseRBtnUp(UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_bMouseRBtnDwon=false;
	m_MousePosX_Window=( int )LOWORD( lParam );
	m_MousePosY_Window=( int )HIWORD( lParam );
	return 1;
}

int DXInputManager::OnMouseMBtnDwon(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_bMouseMBtnDown=true;
	m_MousePosX_Window=( int )LOWORD( lParam );
	m_MousePosY_Window=( int )HIWORD( lParam );
	return 1;
}

int DXInputManager::OnMouseMBtnUp(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_bMouseMBtnDown=false;
	m_MousePosX_Window=( int )LOWORD( lParam );
	m_MousePosY_Window=( int )HIWORD( lParam );
	return 1;
}

int DXInputManager::OnMouseWheel(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_MouseWheelDelta=0;
	if (uMsg==WM_MOUSEHWHEEL)
	{
		m_MouseWheelDelta=(short)HIWORD(lParam);
	}
	return 1;
}

int DXInputManager::OnMouseMove(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	m_bLBtnDraging=m_bMouseLBtnDown;
	m_bRBtnDraging=m_bMouseRBtnDwon;

	m_MousePosX_Window=(int)LOWORD(lParam);
	m_MousePosY_Window=(int)HIWORD(wParam);

	POINT ptCurPos;
	::GetCursorPos(&ptCurPos);

	if (m_bLBtnDraging)
	{
		m_MouseLBtnDragDeltaX=ptCurPos.x-m_LastLDrag_ScreenX;
		m_MouseLBtnDragDeltaY=ptCurPos.y-m_LastLDrag_ScreenY;
		m_LastLDrag_ScreenX=ptCurPos.x;
		m_LastLDrag_ScreenY=ptCurPos.y;
	}
	else if (m_bRBtnDraging)
	{
		m_MouseRBtnDragDeltaX=ptCurPos.x-m_LastRDrag_ScreenX;
		m_MouseRBtnDragDeltaY=ptCurPos.y-m_LastRDrag_ScreenY;
		m_LastRDrag_ScreenX=ptCurPos.x;
		m_LastRDrag_ScreenY=ptCurPos.y;
	}

	return 1;
}