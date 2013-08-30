#pragma  once

#include <Windows.h>
#include <functional>
#include <ShellAPI.h>
struct DxMouseEvent 
{
	bool bMouseLeft;
	bool bMouseRight;
	bool bMouseMiddle;

	bool bKeyShift;
	bool bKeyCtrl;
	bool bKeyALt;

	int posX;
	int posY;
};


struct DxWheelEvent
{
	int wheelDelta;
	bool bKeyShift;
	bool bKeyCtrl;
	bool bKeyAlt;
};


struct DxSizeEvent
{
	DxSizeEvent(size_t x,size_t y)
	{
		w=x;
		h=y;
	}

	unsigned int w;
	unsigned int h;
};

typedef std::tr1::function<LRESULT (UINT uMsg,WPARAM wParam,LPARAM lParam)> DxMsgProc;

typedef std::tr1::function<int (DxSizeEvent& e)> WndSizeCallBack;
typedef std::tr1::function<int (unsigned int )> KeyCallback;
typedef std::tr1::function<int (DxMouseEvent& e)> MouseCallBack;
typedef std::tr1::function<int (DxWheelEvent& w)> WheelCallBack;

typedef std::tr1::function<void (HDROP hdrop)> DropFileCallback;

class Win32MsgBox
{
public:
	static DxMouseEvent GetMouseEventFromMsg(UINT uMsg,WPARAM wParam,LPARAM lParam);
	static DxWheelEvent GetWheelEvnetFromMsg(UINT uMsg,WPARAM wParam,LPARAM lParam);
	static DxSizeEvent GetSizeEventFromMsg(UINT uMsg,WPARAM wParam,LPARAM lParam);
};