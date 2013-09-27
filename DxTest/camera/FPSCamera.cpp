#include "FPSCamera.h"
#include "../DxGlobal/DxGlobal.h"
#include <Windows.h>

FPSCamera::FPSCamera()
{
	m_Pos   = D3DXVECTOR3(0.0f, 0.0f, -200.0f);
	m_RightDir = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_UpDir    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_ViewAtDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_TargitPos=D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_bRightDown=false;
	m_LastPosX=0;
	m_LastPosY=0;
	m_fYangle=0;
}
FPSCamera::~FPSCamera()
{

}

MouseCallBack FPSCamera::GetMouseRightUpCallBack()
{
	MouseCallBack cb;
	cb=std::tr1::bind(&FPSCamera::OnMouseRightUp,this,std::placeholders::_1);
	return cb;
}

MouseCallBack FPSCamera::GetMouseRightDownCallBack()
{
	MouseCallBack cb=std::tr1::bind(&FPSCamera::OnMouseRightDown,this,std::placeholders::_1);
	return cb;
}

MouseCallBack FPSCamera::GetMouseMoveCallBack()
{
	MouseCallBack cb=std::tr1::bind(&FPSCamera::OnMouseMove,this,std::placeholders::_1);
	return cb;
}
//  
  WheelCallBack FPSCamera::GetMouseWheelEventCallBack()
  {
  	WheelCallBack cb=std::tr1::bind(&FPSCamera::OnMouseWheel,this,std::placeholders::_1);
  	return cb;
  }
//  
  KeyCallback FPSCamera::GetKeyDownCallBack()
  {
  	KeyCallback cb=std::tr1::bind(&FPSCamera::OnkeyDown,this,std::placeholders::_1);
  	return cb;
  }


void FPSCamera::UpdateFrame(float interval)
{
	DXInputManager* inputMng=GetGlobalInputManager();
	interval *=0.1f;
	if (inputMng->IsKeyDown(GetFPSCameraKeyCodeFromLogicKey(CAM_STRAFE_RIGHT)))
		MoveAlongRight(interval);
	if(inputMng->IsKeyDown(GetFPSCameraKeyCodeFromLogicKey(CAM_STRAFE_LEFT)))
		MoveAlongRight(-interval);

	if(inputMng->IsKeyDown(GetFPSCameraKeyCodeFromLogicKey(CAM_MOVE_UP)))
		MoveALongUp(interval);

	if(inputMng->IsKeyDown(GetFPSCameraKeyCodeFromLogicKey(CAM_MOVE_DOWN)))
		MoveALongUp(-interval);

	if(inputMng->IsKeyDown(GetFPSCameraKeyCodeFromLogicKey(CAM_MOVE_FORWARD)))
		MoveALongViewAt(interval);

	if(inputMng->IsKeyDown(GetFPSCameraKeyCodeFromLogicKey(CAM_MOVE_BACKWARD)))
		MoveALongViewAt(-interval);
}

int FPSCamera::OnMouseWheel(const DxWheelEvent& e)
{
	return 1;
}

int FPSCamera::OnMouseMove(const DxMouseEvent& e)
{
	if(!m_bRightDown)
		return 1;

	POINT ptCurPos;
	GetCursorPos(&ptCurPos);
	int mouseDeltaX=ptCurPos.x-m_LastPosX;
	int mouseDeltaY=ptCurPos.y-m_LastPosY;

	float degreeX=mouseDeltaX * 0.01f;
	float degreeY=mouseDeltaY * 0.01f;
	m_fYangle += degreeX;
	
	RotationUp(degreeX);
	RotationRight(degreeY);

	m_LastPosY=ptCurPos.y;
	m_LastPosX=ptCurPos.x;

	return 1;
}

void FPSCamera::RotationYAxis(float degree)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_UpDir, degree);//创建出绕m_vUpVector旋转fAngle个角度的R矩阵
	D3DXVec3TransformCoord(&m_RightDir, &m_RightDir, &R);//让m_vRightVector向量绕m_vUpVector旋转fAngle个角度
	D3DXVec3TransformCoord(&m_ViewAtDir, &m_ViewAtDir, &R);//让m_vLookVector向量绕m_vUpVector旋转fAngle个角度
}

void FPSCamera::RotationXAxis(float degree)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_RightDir, degree);
	D3DXVec3TransformCoord(&m_UpDir, &m_UpDir, &R);// 
	D3DXVec3TransformCoord(&m_ViewAtDir, &m_ViewAtDir, &R); 
}

int FPSCamera::OnMouseRightDown(const DxMouseEvent& e)
{
	m_bRightDown=true;
	POINT ptCurPos;
	GetCursorPos(&ptCurPos);
	m_LastPosX=ptCurPos.x;
	m_LastPosY=ptCurPos.y;

	return 1;
}

int FPSCamera::OnMouseRightUp(const DxMouseEvent& e)
{
	m_bRightDown=false;

	return 1;
}

int FPSCamera::OnkeyDown(unsigned int key)
{
	DXWriteConsol(CC_GREEN,"FPSCamera::OnkeyDown");
	return 1;
}


void FPSCamera::GetViewMatrix(D3DXMATRIX* V)
{
	
	D3DXVec3Normalize(&m_ViewAtDir, &m_ViewAtDir);

	D3DXVec3Cross(&m_UpDir, &m_ViewAtDir, &m_RightDir);
	D3DXVec3Normalize(&m_UpDir, &m_UpDir);

	D3DXVec3Cross(&m_RightDir, &m_UpDir, &m_ViewAtDir);
	D3DXVec3Normalize(&m_RightDir, &m_RightDir);

	
	float x = -D3DXVec3Dot(&m_RightDir, &m_Pos);
	float y = -D3DXVec3Dot(&m_UpDir, &m_Pos);
	float z = -D3DXVec3Dot(&m_ViewAtDir, &m_Pos);

	//rx   ux   dx   0
	//ry   uy   dy   0
	//rz   uz   dz   0
	//-p.r -p.u -p.d 1

	(*V)(0,0) =m_RightDir.x; (*V)(0, 1) =m_UpDir.x; (*V)(0, 2) = m_ViewAtDir.x; (*V)(0, 3) = 0.0f;
	(*V)(1,0) =m_RightDir.y; (*V)(1, 1) =m_UpDir.y; (*V)(1, 2) = m_ViewAtDir.y; (*V)(1, 3) = 0.0f;
	(*V)(2,0) =m_RightDir.z; (*V)(2, 1) =m_UpDir.z; (*V)(2, 2) = m_ViewAtDir.z; (*V)(2, 3) = 0.0f;
	(*V)(3,0) = x;        (*V)(3, 1) = y;     (*V)(3, 2) = z;       (*V)(3, 3) = 1.0f;
}

void FPSCamera::SetPos(const D3DXVECTOR3& pos)
{
	m_Pos=pos;
	 
}
void FPSCamera::SetLookAt(const D3DXVECTOR3& at)
{
	m_ViewAtDir=at-m_Pos;
	D3DXVec3Normalize(&m_ViewAtDir, &m_ViewAtDir);//规范化 

	//正交并规范化 
	D3DXVec3Cross(&m_RightDir, &D3DXVECTOR3(0,1,0), &m_ViewAtDir);//第二个参数必须是0.1.0 这样才能保证摄像机不会滚动
	D3DXVec3Normalize(&m_RightDir, &m_RightDir);

	D3DXVec3Cross(&m_UpDir, &m_ViewAtDir, &m_RightDir);
	D3DXVec3Normalize(&m_UpDir, &m_UpDir);
	
}
void FPSCamera::SetLookAtDir(const D3DXVECTOR3& dir)
{
	m_ViewAtDir=dir;
}
void FPSCamera::SetUp(const D3DXVECTOR3& up)
{
	m_UpDir=up;
}


void FPSCamera::MoveAlongRight(float delta)
{
	m_Pos += m_RightDir*delta;
	m_TargitPos +=m_RightDir*delta;
}

void FPSCamera::MoveALongUp(float delta)
{
	m_Pos += m_UpDir*delta;
	m_TargitPos +=m_UpDir*delta;
}

void FPSCamera::MoveALongViewAt(float delta)
{
	m_Pos += m_ViewAtDir*delta;
	m_TargitPos +=m_ViewAtDir*delta;
}

void FPSCamera::RotationRight(float degree)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T,&m_RightDir,degree);
	D3DXVec3TransformCoord(&m_UpDir,&m_UpDir,&T);
	D3DXVec3TransformCoord(&m_ViewAtDir,&m_ViewAtDir,&T);
	m_TargitPos=m_ViewAtDir*D3DXVec3Length(&m_Pos);
}
void FPSCamera::RotationUp(float degree)
{
	D3DXMATRIX T;
	//D3DXMatrixRotationAxis(&T,&m_UpDir,degree);
	D3DXMatrixRotationY(&T,degree); //修改旋转轴，这里是Y轴，不能是updir，因为经过一些运动后updir可能不再是0.1.0
	D3DXVec3TransformCoord(&m_RightDir,&m_RightDir,&T);
	D3DXVec3TransformCoord(&m_ViewAtDir,&m_ViewAtDir,&T);
	m_TargitPos=m_ViewAtDir*D3DXVec3Length(&m_Pos);
}

void FPSCamera::SetViewMatrix(IDirect3DDevice9* device)
{
	D3DXMATRIX V ;
	this->GetViewMatrix(&V);
	device->SetTransform(D3DTS_VIEW, &V);
}

void FPSCamera::SetProjMatrix(IDirect3DDevice9* device)
{
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)1024 / (float)768,
		1.0f,
		1000.0f);
	device->SetTransform(D3DTS_PROJECTION, &proj);
}