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
#include "../D3D/Terrain.h"
#include "../D3D/DXFront.h"
#include "GlobalConfig.h"
#include "../d3d/Particle.h"
#include "../D3D/RenderInfo.h"
#include "Model.h"
#include "SceneManager.h"




class TestApp:public DxAppBase
{
public:
	std::tr1::shared_ptr<D3DXMesh> m_DXMesh;
	std::tr1::shared_ptr<D3DXMesh> m_MeshTank;
	std::tr1::shared_ptr<D3DXMesh> m_MeshTree;
	std::tr1::shared_ptr<SkyBox>  m_SkyBox;
	std::tr1::shared_ptr<Terrian> m_Terrain;
	D3DLIGHT9 light;
	std::tr1::shared_ptr<DXText> m_Text;
	std::tr1::shared_ptr<ParticleBase> m_Snow;
	Model*  m_Model;
	//----------------------------------------------------
public:
	std::tr1::shared_ptr<FPSCamera> m_FPSCamera;

	void InitCamera();
	void UpdateCamera(float interval);
public:
	TestApp(const char* name,unsigned int width,unsigned int hight,unsigned int left,unsigned int top):DxAppBase(name,width,hight,left,top)
	{
		m_RenderInfo.width=width;
		m_RenderInfo.height=hight;
		m_RenderInfo.bwindowed=false;
		m_RenderInfo.hwnd=GetWnd()->GetHwnd();
	}
	~TestApp()
	{

	}


 	int OnkeyDown(unsigned int key);
public:
	void InitApp();

	int OnWndSize(DxSizeEvent& e);

	void ShutDownApp()
	{

	}

	void Render(float timeDelta);
	void Update(float interval);

private:
	void OnReset(HRESULT& hr);
	void RenderTextHelp(float timedelta);

	RenderInfo m_RenderInfo;
public:
	void SetRenderInfo(RenderInfo info)
	{
		m_RenderInfo.backFMT=info.backFMT;
		m_RenderInfo.bwindowed=info.bwindowed;
		m_RenderInfo.height=info.height;
		m_RenderInfo.hwnd=info.hwnd;
		m_RenderInfo.width=info.width;
		m_RenderInfo.zFMT=info.zFMT;
	};
	RenderInfo GetRenderInfo(){return m_RenderInfo;}

};