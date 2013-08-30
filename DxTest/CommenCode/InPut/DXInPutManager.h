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

	//�õ���֡ ������Ҽ���ק�ƶ����루��Ļ�ռ䣩
	void GetCurFramMouseLDragDelta(int* posX,int* posY);
	void GetCurFramMouseRDragDelta(int* posX,int* posY);

	//�õ������ק��ʼ��λ��
	void GetMouseLBtnDragingBeginPoint_Window(int* posX,int* poxY);
	void GetMouseLBtnDragingBeginPoint_Screen(int* posX,int* posY);

private:
	//!������̵���Ϣ������0��ʾû�д���ϣ����Ĭ����Ϣ��������������1��ʾ�Ѿ�������ϣ���پ���Ĭ�ϵĴ���
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

	//!��¼һ֡�еİ�������,1��ʾ����״̬
	bool m_Keys[256];
	bool m_MouseButtons[5];
	
	//��¼���ټ� ����
	int m_KeyDownCount;
	//����Ƿ���
	bool m_bMouseLBtnDown;

	//�Ҽ��Ƿ���
	bool m_bMouseRBtnDwon;

	//�м��Ƿ���
	bool m_bMouseMBtnDown;

	//��¼�������ʱ�ڴ����µ�����
	int m_MouseLBtnDownPosX_Screen;
	int m_MouseLBtnDownPosY_Screen;

	//��¼�������ʱ����Ļ�µ�����
	int m_MouseLBtnDownPosX_Window;
	int m_MouseLBtnDownPosY_Window;

	//��¼���ָ�뵱ǰ�ڴ��������µ�λ��
	int m_MousePosX_Window;
	int m_MousePosY_Window;

	//��¼�����קʱ��һ֡����λ�ã�������Ļ��λ�ã��Ǵ���λ�ã�
	int m_LastLDrag_ScreenX;
	int m_LastLDrag_ScreenY;

	//�����קʱ�� ��ʼλ�ã��������꣩
	int m_LDragBeginPointX_Window;
	int m_LDragBeginPointY_Window;

	//�����קʱ ����ʼλ�ã���Ļ���꣩
	int m_LDragBeginPointX_Screen;
	int m_LDragBeginPointY_Screen;
	//��¼�Ҽ���קʱ ��һ֡����λ�ã�������Ļλ�ã�
	int m_LastRDrag_ScreenX;
	int m_LastRDrag_ScreenY;
	//��¼�����ק�ƶ��ľ���
	int m_MouseLBtnDragDeltaX;
	int m_MouseLBtnDragDeltaY;
	int m_MouseRBtnDragDeltaX;
	int m_MouseRBtnDragDeltaY;

	//��¼�������ƶ�����
	int m_MouseWheelDelta;
	//�Ƿ���ק
	bool m_bLBtnDraging;
	bool m_bRBtnDraging;
};