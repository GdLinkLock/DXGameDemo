#pragma  once
#include <Windows.h>

class DXInputManager
{
public:
	DXInputManager(){};
	~DXInputManager(){};

	void Init();
	void ResetPerFrame();
	int HandleMsg(UINT uMsg,WPARAM wParam,LPARAM lParam);
	bool IsKeyDown(unsigned int key);

	bool IsMouseLButtonDown(){return m_bMouseLBtnDown;}
	bool IsMouseRButtonDown(){return m_bMouseRBtnDwon;}
	bool IsMouseMButtonDown(){return m_bMouseMBtnDown;}

	void GetMouseLBtnDownPos_Window(int* posX,int* posY);
	void GetMouseLBtnDownPos_Screen(int* posX,int* posY);

	void GetMouseCurrentPos_Window(int* posX,int* posY);

	int GetWheelDistance();
	bool IsMouseLBtnDraging();
	bool IsMouseRBtnDraging();

	//得到本帧 左键，右键拖拽移动距离（屏幕空间）
	void GetCurFramMouseLDragDelta(int* posX,int* posY);
	void GetCurFramMouseRDragDelta(int* posX,int* posY);

	//得到左键拖拽开始的位置
	void GetMouseLBtnDragingBeginPoint_Window(int* posX,int* poxY);
	void GetMouseLBtnDragingBeginPoint_Screen(int* posX,int* posY);

private:
	//!处理键盘的消息。返回0表示没有处理希望用默认消息处理函数处理；返回1表示已经处理，不希望再经过默认的处理
	int ProcessKeyBoardMsg(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int ProcessMouseMsg(UINT uMsg,WPARAM wParam,LPARAM lParam);

	int OnMouseLBtnDown(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int OnMouseLBtnUp(UINT uMsg,WPARAM wParam,LPARAM lParam);

	int OnMOuseRBtnDown(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int OnMouseRBtnUp(UINT uMsg,WPARAM wParam,LPARAM lParam);

	int OnMouseMBtnDwon(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int OnMouseMBtnUp(UINT uMsg,WPARAM wParam,LPARAM lParam);

	int OnMouseWheel(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int OnMouseMove(UINT uMsg,WPARAM wParam,LPARAM lParam);

private:

	//!记录一帧中的按键输入,1表示按下状态
	bool m_Keys[256];
	bool m_MouseButtons[5];
	
	//记录多少键 按下
	int m_KeyDownCount;
	//左键是否按下
	bool m_bMouseLBtnDown;

	//右键是否按下
	bool m_bMouseRBtnDwon;

	//中键是否按下
	bool m_bMouseMBtnDown;

	//记录左键按下时在窗口下的坐标
	int m_MouseLBtnDownPosX_Screen;
	int m_MouseLBtnDownPosY_Screen;

	//记录左键按下时在屏幕下的坐标
	int m_MouseLBtnDownPosX_Window;
	int m_MouseLBtnDownPosY_Window;

	//记录鼠标指针当前在窗口坐标下的位置
	int m_MousePosX_Window;
	int m_MousePosY_Window;

	//记录左键拖拽时上一帧鼠标的位置（整个屏幕的位置，非窗口位置）
	int m_LastLDrag_ScreenX;
	int m_LastLDrag_ScreenY;

	//左键拖拽时地 起始位置（窗口坐标）
	int m_LDragBeginPointX_Window;
	int m_LDragBeginPointY_Window;

	//左键拖拽时 的起始位置（屏幕坐标）
	int m_LDragBeginPointX_Screen;
	int m_LDragBeginPointY_Screen;
	//记录右键拖拽时 上一帧鼠标的位置（整个屏幕位置）
	int m_LastRDrag_ScreenX;
	int m_LastRDrag_ScreenY;
	//记录鼠标拖拽移动的距离
	int m_MouseLBtnDragDeltaX;
	int m_MouseLBtnDragDeltaY;
	int m_MouseRBtnDragDeltaX;
	int m_MouseRBtnDragDeltaY;

	//记录鼠标滚轮移动距离
	int m_MouseWheelDelta;
	//是否被拖拽
	bool m_bLBtnDraging;
	bool m_bRBtnDraging;
};