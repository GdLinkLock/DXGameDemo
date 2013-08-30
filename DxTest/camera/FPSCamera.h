#pragma  once

#include "CamaraBase.h"
#include "../AppBase/Win32MsgBox.h"


class FPSCamera
{
public:
	FPSCamera();
	~FPSCamera();

	void SetLookAt(const D3DXVECTOR3& at);
	void SetPos(const D3DXVECTOR3& pos);
	void SetLookAtDir(const D3DXVECTOR3& dir);
	void SetUp(const D3DXVECTOR3& up);

	const D3DXVECTOR3& GetPostion(){return m_Pos;}//返回当前摄像机位置向量
	const D3DXVECTOR3& GetTarget(){return m_TargitPos;}
	const D3DXVECTOR3& GetUp(){return m_UpDir;}
	const D3DXVECTOR3& GetViewAtDir(){return m_ViewAtDir;}//返回当前的观察向量
	void GetViewMatrix(D3DXMATRIX* V);//返回取景变换矩阵

	void UpdateFrame(float interval);

	//返回相机绕Y轴旋转
	float GetYAngle(){return m_fYangle;}
public:
	MouseCallBack GetMouseRightUpCallBack();
	MouseCallBack GetMouseRightDownCallBack();
	MouseCallBack GetMouseMoveCallBack();
	KeyCallback   GetKeyDownCallBack();
	WheelCallBack GetMouseWheelEventCallBack();

private:
	int OnkeyDown(unsigned int key);
	int OnMouseRightDown(const DxMouseEvent& e);
	int OnMouseRightUp(const DxMouseEvent& e);
	int OnMouseMove(const DxMouseEvent& e);
	int OnMouseWheel(const DxWheelEvent& e);

	void RotationYAxis(float degree);
	void RotationXAxis(float degree);
	
	//沿着三个方向移动
	void MoveAlongRight(float delta);
	void MoveALongUp(float delta);
	void MoveALongViewAt(float delta);

	//沿三个方向旋转
	void RotationUp(float degree);
	void RotationViewAt(float degree);
	void RotationRight(float degree);


private:
	D3DXVECTOR3 m_Pos;
	D3DXVECTOR3 m_RightDir;
	D3DXVECTOR3 m_UpDir;
	D3DXVECTOR3 m_ViewAtDir;

	D3DXVECTOR3 m_TargitPos;

	bool m_bRightDown;
	int  m_LastPosX;
	int  m_LastPosY;

	float m_fYangle;
};