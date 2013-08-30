#pragma  once
#include <Windows.h>
#include "../AppBase/DxApp.h"
#include "../DxGlobal/DxGlobal.h"
#include "../camera/CamaraBase.h"
#include "../camera/FPSCamera.h"
#include "../Log/ErrorInfo.h"
#include "../CommenCode/Util/FileOperation.h"
#include "../D3D/D3DXMesh.h"
#include <io.h>
#include "../camera/camera.h"
#include "../D3D/SkyBox.h"

#define  WINDOW_WIDTH 1024
#define  WINDOW_HIGHT 768


class TestApp:public DxAppBase
{
public:
	std::tr1::shared_ptr<D3DXMesh> m_DXMesh;
	std::tr1::shared_ptr<D3DXMesh> m_MeshTank;
	std::tr1::shared_ptr<D3DXMesh> m_MeshTree;
	std::tr1::shared_ptr<SkyBox>  m_SkyBox;
	
	//floor
	IDirect3DTexture9* mFloorTex;
	IDirect3DVertexBuffer9* mFloorVB;

	//----------------------------------------------------
public:
	std::tr1::shared_ptr<FPSCamera> m_FPSCamera;

	void InitCamera();
	void UpdateCamera(float interval);
public:
	TestApp(const char* name,unsigned int width,unsigned int hight,unsigned int left,unsigned int top):DxAppBase(name,width,hight,left,top)
	{

	}
	~TestApp()
	{

	}


 	int OnkeyDown(unsigned int key);
public:
	void InitApp();


	void ShutDownApp()
	{

	}

	void Render(float timeDelta);
	void Update(float interval);
};