#pragma  once
#include <Windows.h>
#include <functional>
#include "Win32MsgBox.h"
#include <vector>


class DxWindown
{
public:
	DxWindown(const char* wndName,unsigned int width,unsigned int hight,unsigned int posLeft,unsigned int posTop);
	~DxWindown();

	HWND GetHwnd(){return m_hwnd;}
	unsigned int GetWidth(){return m_width;}
	unsigned int GetHight(){return m_hight;}
	DxMsgProc& GetMsgProc(){return dx_msg_proc;}

	void RigesterWndSizeEvent(WndSizeCallBack cb);
	void RigsterMouseLeftDown(MouseCallBack cb);
	void RigisterMouseLeftUp(MouseCallBack cb);
	void RigisterMouseRightDown(MouseCallBack cb);
	void RigisterMouseRightUp(MouseCallBack cb);
	void RigsterMouseMiddleDown(MouseCallBack cb);
	void RigeterMouseMiddleUp(MouseCallBack cb);
	void RigsterMouseWheelEvent(WheelCallBack cb);
	void RigisterMouseMoveEvent(MouseCallBack cb);
	void RigisterKeyDown(KeyCallback cb);
	void RigisterKeyUp(KeyCallback cb);
	void RigisterDropEvent(DropFileCallback cb);


private:
	void CreateWnd();
	static LRESULT CALLBACK StaticWndPoc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT DxWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

private:
	DxMsgProc dx_msg_proc;
	std::vector<WndSizeCallBack> wnd_size_event;
	std::vector<MouseCallBack> mouse_left_down;
	std::vector<MouseCallBack> mouse_right_up;
	std::vector<MouseCallBack> mouse_left_up;
	std::vector<MouseCallBack> mouse_right_down;
	std::vector<MouseCallBack> mouse_mid_down;
	std::vector<MouseCallBack> mouse_mid_up;
	std::vector<MouseCallBack> mouse_move_event;
	std::vector<WheelCallBack> mouse_whell_event;
	std::vector<KeyCallback> key_down_event;
	std::vector<KeyCallback> key_up_envent;
	std::vector<DropFileCallback> drop_file_event;

private:

	HWND m_hwnd;
	unsigned int m_hight;
	unsigned int m_width;
	unsigned int m_pos_left;
    unsigned int m_pos_top;
	std::string m_wnd_name;
};
