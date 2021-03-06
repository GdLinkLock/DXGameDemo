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
	// Set default settings 
	UINT AdapterToUse=D3DADAPTER_DEFAULT; 
	D3DDEVTYPE DeviceType=D3DDEVTYPE_HAL;


	// Look for 'NVIDIA PerfHUD' adapter 
	// If it is present, override default settings 
	for (UINT Adapter=0;Adapter<d3d9->GetAdapterCount();Adapter++) 
	{ 
		D3DADAPTER_IDENTIFIER9        Identifier; 
		HRESULT                       Res; 
		Res = d3d9->GetAdapterIdentifier(Adapter,0,&Identifier); 
		if (strstr(Identifier.Description,"PerfHUD") != 0) 
		{ 
			AdapterToUse=Adapter; 
			DeviceType=D3DDEVTYPE_REF; 
			//DXWriteConsol(CC_BLUE,"perfhud");
			break; 
		} 
	} 

	//4.create the device
	hr=d3d9->CreateDevice(AdapterToUse,
		DeviceType,
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

 float D3D::GetFps(float delta)
 {
	 static float fps=0;
	 static float escape=0;
	 static int frame=0;

	 frame++;
	 escape +=delta*0.001;
	 if (escape >= 1.0)
	 {
		 fps=(float)frame/escape;
		 frame=0;
		 escape=0;
	 }
	 return fps;
 }

 DWORD D3D::F2Dw(float fval)
 {
	 return *(DWORD*)(&fval);
 }

 float D3D::GetRandomFloat(float min,float max)
 {
	 if (min > max)
		return min;
	 return ((float)rand()/(float)RAND_MAX)*(max -min)+ min;
 }

 int D3D::GetRandomInt(int min,int max)
 {
	 if (min > max)
		return min;
	 return rand()%(max-min+1) + min;
 }

 D3DXVECTOR3 D3D::GetRandomVec3(D3DXVECTOR3 min,D3DXVECTOR3 max)
 {
	 D3DXVECTOR3 vec3;
	 vec3.x=D3D::GetRandomFloat(min.x,max.x);
	 vec3.y=D3D::GetRandomFloat(min.y,max.y);
	 vec3.z=D3D::GetRandomFloat(min.z,max.z);
	 return vec3;
 }