#include "D3DUtil.h"
#include "../Log/ErrorInfo.h"



const DWORD D3D::Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;



bool D3D::InitD3D(  HWND hwnd, int width,int hight,bool windowed, D3DDEVTYPE d3dType, IDirect3DDevice9** device )
{
	HRESULT hr;
	//1.create IDirect3D9 object
	IDirect3D9* d3d9=0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9)
	{
		PopupError("Create D3d9 Error!");
		return false;
	}

	//2.check for hardware cap
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,d3dType,&caps);
	int vp;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp=D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp=D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//3.fill out D3DPRESENT_PARAMETERS structure
 	D3DPRESENT_PARAMETERS d3dpp;
 	d3dpp.BackBufferWidth=width;
 	d3dpp.BackBufferHeight=hight;
 	d3dpp.BackBufferCount=1;
 	d3dpp.BackBufferFormat=D3DFMT_A8R8G8B8;
 
 	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
 	d3dpp.MultiSampleQuality=0; //多重采样质量水平
 
 	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD; //交换链中的缓存的页面置换方式
 	d3dpp.hDeviceWindow=hwnd;
 	d3dpp.Windowed=windowed;  //窗口模式
 	d3dpp.EnableAutoDepthStencil=true;  //d3d自动创建并维护深度缓存或模板缓存
 	d3dpp.AutoDepthStencilFormat=D3DFMT_D24S8;//深度缓存或模板缓存的像素格式
 	d3dpp.Flags=0;
 	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;//刷新频率
 	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
	
	//4.create the device
	hr=d3d9->CreateDevice(D3DADAPTER_DEFAULT,
							d3dType,
							hwnd,
							vp,
							&d3dpp,
							device);
	if (FAILED(hr))
	{
		d3dpp.AutoDepthStencilFormat=D3DFMT_D16;
		hr=d3d9->CreateDevice(D3DADAPTER_DEFAULT,
								d3dType,
								hwnd,
								vp,
								&d3dpp,
								device);
		if (FAILED(hr))
		{
			d3d9->Release();
			PopupError("Create Device Failed!");
			return false;
		}
	}

	d3d9->Release();
	return true;
}

D3DLIGHT9 D3D::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 D3D::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 D3D::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}
// 
 D3DMATERIAL9 D3D::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
 {
 	D3DMATERIAL9 mtrl;
 	mtrl.Ambient  = a;
 	mtrl.Diffuse  = d;
 	mtrl.Specular = s;
 	mtrl.Emissive = e;
 	mtrl.Power    = p;
 	return mtrl;
 }
 
 D3D::BoundingBox::BoundingBox()
 {
 	// infinite small 
 	_min.x = D3D::INFINITY;
 	_min.y = D3D::INFINITY;
 	_min.z = D3D::INFINITY;
 
 	_max.x = -D3D::INFINITY;
 	_max.y = -D3D::INFINITY;
 	_max.z = -D3D::INFINITY;
 }
 
 bool D3D::BoundingBox::isPointInside(D3DXVECTOR3& p)
 {
 	if( p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
 		p.x <= _max.x && p.y <= _max.y && p.z <= _max.z )
 	{
 		return true;
 	}
 	else
 	{
 		return false;
 	}
 }
 
 D3D::BoundingSphere::BoundingSphere()
 {
 	_radius = 0.0f;
 }
 
 

 void DrawCoord(IDirect3DDevice9* device,D3DXVECTOR3 begin,D3DXVECTOR3 end,D3DXCOLOR color)
 {
	
 }

 float D3D::Lerp(float a, float b, float t)
 {
	 return a - (a*t) + (b*t);
 }