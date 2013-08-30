#include "Win32MsgBox.h"


DxMouseEvent Win32MsgBox::GetMouseEventFromMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	DxMouseEvent dxe;
	dxe.bMouseLeft = (wParam & MK_LBUTTON)==1;
	dxe.bMouseRight = (wParam & MK_RBUTTON)==1;
	dxe.bMouseMiddle = (wParam & MK_MBUTTON)==1;
	dxe.bKeyALt = (wParam & MK_ALT)==1;
	dxe.bKeyShift = (wParam & MK_SHIFT)==1;
	dxe.bKeyCtrl = (wParam & MK_CONTROL)==1;
	dxe.posX=LOWORD(lParam);
	dxe.posY=HIWORD(lParam);
	return dxe;
}

DxSizeEvent Win32MsgBox::GetSizeEventFromMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	
	unsigned int w=LOWORD(lParam);
	unsigned int h=HIWORD(lParam);
	return DxSizeEvent(w,h);
}

DxWheelEvent Win32MsgBox::GetWheelEvnetFromMsg(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	DxWheelEvent dxe;
	dxe.bKeyAlt=(wParam & MK_ALT)==1;
	dxe.bKeyShift=(wParam & MK_SHIFT)==1;
	dxe.bKeyCtrl=(wParam & MK_CONTROL)==1;
	dxe.wheelDelta=(short)HIWORD(lParam);
	return dxe;
}