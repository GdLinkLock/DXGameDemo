#include "GameApp.h"


void TestApp::InitCamera()
{
	m_FPSCamera=std::make_shared<FPSCamera>();
	m_FPSCamera->SetPos(D3DXVECTOR3(0.0f, 2.0, -18.0f));  //设置摄像机所在的位置
	m_FPSCamera->SetLookAt(D3DXVECTOR3(0.0f, 0.0f, 0.0f));  //设置目标观察点所在的位置

	m_pWnd->RigisterMouseRightDown(m_FPSCamera->GetMouseRightDownCallBack());
	m_pWnd->RigisterMouseRightUp(m_FPSCamera->GetMouseRightUpCallBack());
	m_pWnd->RigisterMouseMoveEvent(m_FPSCamera->GetMouseMoveCallBack());
	DXWriteConsol(CC_WHITE,"Init FPSCamera");
}
void TestApp::UpdateCamera(float interval)
{
	m_FPSCamera->UpdateFrame(interval);

	D3DXMATRIX V;
	m_FPSCamera->GetViewMatrix(&V);
	GetGlobalDevice()->SetTransform(D3DTS_VIEW,&V);
}



	int TestApp::OnkeyDown(unsigned int key)
	{
		switch (key)
		{
		case VK_F1: //快速找到主角
			{
// 				D3DXVECTOR3 pos=m_DXMesh->GetPostion();
// 				m_FPSCamera->SetPos(D3DXVECTOR3(pos.x,pos.y+300,pos.z-700));
// 				m_FPSCamera->SetLookAt(pos);
				break;
			}
		case VK_NUMPAD0:
			{
			
				static bool bframe=false;
				bframe = !bframe;
				GlobalConfig::SetEnableWireFrame(bframe);
				break;
			}
		default:
			return 0;
		}

		return 1;
	}

	void TestApp::InitApp()
	{
		InitCamera();
		IDirect3DDevice9* Device=GetGlobalDevice();
		m_pWnd->GetMsgProc()=std::tr1::bind(&DXInputManager::HandleMsg,GetGlobalInputManager(),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);
		KeyCallback kcb=std::tr1::bind(&TestApp::OnkeyDown,this,std::placeholders::_1);
		m_pWnd->RigisterKeyDown(kcb);
		WndSizeCallBack wcb=std::tr1::bind(&TestApp::OnWndSize,this,std::placeholders::_1);
		m_pWnd->RigesterWndSizeEvent(wcb);
		//==============================================================================
	   //m_Model=SceneManager::Instance()->LoadModelFromFile("./default/default.ase");
		//m_Model=SceneManager::Instance()->LoadModelFromFile("4560003.ase");
		//m_Model->LoadTexture("4560003001_D.dds");
		m_Model=SceneManager::Instance()->LoadModelFromFile("./wall/palaceroom_wall001.ase");
		m_Model->LoadTexture("./wall/palaceroom_wall001_d.dds");
	//	m_Model->SetPosition(D3DXVECTOR3(0,0,0));
	//	m_Snow=std::make_shared<SnowParticle>(Device,8000);
	//	m_Snow->InitParticle("snow1.jpg");
		m_Text=std::make_shared<DXText>(Device);

	//	m_Terrain=std::make_shared<Terrian>(Device);
//		m_Terrain->LoadTerrainFromFile("heighmap.raw","terrain_grass.jpg");
//		m_Terrain->InitTerrain(300,300, 20.0f, 0.3f);

	//	m_DXMesh=std::make_shared<D3DXMesh>();
	//	m_DXMesh->CreateMesh(Device,"castle400.X");
//		float hCastle=m_Terrain->GetHeight(300,500);
//		m_DXMesh->SetPosition(D3DXVECTOR3(-100,450+hCastle,600));


	//	m_MeshTank=std::make_shared<D3DXMesh>();
	//	m_MeshTank->CreateMesh(Device,"tank_gudao120.x");
	//	float hTank=m_Terrain->GetHeight(300,500);
	//	m_MeshTank->SetPosition(D3DXVECTOR3(0,hTank,0));


// 		m_MeshTree=std::make_shared<D3DXMesh>();
// 		m_MeshTree->CreateMesh(Device,"meizhouyu80.x");
// 		float hTree=m_Terrain->GetHeight(300,500);
// 		m_MeshTree->SetPosition(D3DXVECTOR3(-100,hTree,0));

// 		m_SkyBox=std::make_shared<SkyBox>(Device);
// 		m_SkyBox->LoadSkyTextureFromFile("frontsnow1.jpg",
// 			"backsnow1.jpg",
// 			"leftsnow1.jpg",
// 			"rightsnow1.jpg",
// 			"topsnow1.jpg");
// 		m_SkyBox->InitSkyBox(50000);


 		D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
 		D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
 		light = D3D::InitDirectionalLight(&dir, &col);
// 	   D3DLIGHT9 light;
// 	   ::ZeroMemory(&light, sizeof(light));
// 	   light.Type      = D3DLIGHT_DIRECTIONAL;
// 	   light.Ambient   = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
// 	   light.Diffuse   = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
// 	   light.Specular  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
// 	   light.Direction = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	   Device->SetLight(0, &light);
	   Device->LightEnable(0, true);

		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_SPECULARENABLE, true);

		m_FPSCamera->SetViewMatrix(Device);
		m_FPSCamera->SetProjMatrix(Device);

		D3DVIEWPORT9 vp={0,0,WINDOW_WIDTH,WINDOW_HIGHT,0,1};
		Device->SetViewport(&vp);

	}

	void TestApp::RenderTextHelp(float timeDelta)
	{
		float clor[3]={0,255,0};
		float fps=D3D::GetFps(timeDelta);
		m_Text->PushText(0,0,clor,"FPS:%.2f",fps);
		m_Text->PushText(0,18,clor,"本帧三角形数量：%d",grunTimeInfo.fTriangleNum);
		m_Text->PushText(0,36,clor,"总的三角形数量:%d",grunTimeInfo.aTriangleNum);
		m_Text->PushText(0,54,clor,"本帧drawcall：%d",grunTimeInfo.fDrawCoreNum);
		m_Text->PushText(0,72,clor,"总的drawcall:%d",grunTimeInfo.aDrawCoreNum);
		m_Text->RenderText();
	}

	void TestApp::Render(float timeDelta)
	{
		grunTimeInfo.Reset();
		IDirect3DDevice9* Device=GetGlobalDevice();
		HRESULT hr;
		if (GlobalConfig::IsLostDevice())
		{
			//让cpu完成现在的事情
			Sleep(100);

			if (FAILED(hr=Device->TestCooperativeLevel()))
			{   
				if (hr == D3DERR_DEVICELOST)
				{
					//设备丢失，但是这个时候还不能恢复
					return ;
				}
				if (hr ==D3DERR_DEVICENOTRESET)//这个时候可以恢复了
				{
					OnReset(hr);
				}
				return ;
			}
			GlobalConfig::SetLostDevice(false);
		}

		UpdateCamera(timeDelta);
		FrameBegin();

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0x0000000, 1.0f, 0);
		Device->BeginScene();

// 		m_DXMesh->DrawMesh(Device);
// 		m_MeshTank->DrawMesh(Device);
// 		m_MeshTree->DrawMesh(Device);
	//	m_Terrain->RenderTerrain();


		//snow
// 		m_Snow->UpdataParticle(timeDelta);
// 		m_Snow->RenderParticle();


		//绘制天空
// 		D3DXMATRIX matSky,matTransSky,matRotSky;
// 		D3DXMatrixTranslation(&matTransSky,0.0f,-3500.0f,0.0f);
// 		D3DXMatrixRotationY(&matRotSky, -0.000005f*timeGetTime());   //旋转天空网格, 简单模拟云彩运动效果
// 		matSky=matTransSky*matRotSky;
		//m_SkyBox->SetMatWorld(matSky);
		//m_SkyBox->RenderSkyBox();

		m_Model->Render();
		RenderTextHelp(timeDelta);
		Device->EndScene();

		hr = Device->Present(NULL, NULL, NULL, NULL);
		if(hr == D3DERR_DEVICELOST)
		{
			GlobalConfig::SetLostDevice(true);
		}
		
		//-----------------------------------------------------
		GetGlobalInputManager()->ResetPerFrame();
		FrameEnd();
	}

	void TestApp::Update(float interval)
	{
		DXInputManager* inputMng=GetGlobalInputManager();
		static float posX=0,posY=0,posZ=0;
		float delta=interval* 0.1f;
		static float angle=0;
		angle +=0.001;
		if (inputMng->IsKeyDown(VK_LEFT))
		{
			m_Model->SetYRoation(angle);
		}

// 		if (inputMng->IsKeyDown('A'))
// 		{
// 			
// 			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
// 			pos.x -= delta;
// 			float height=m_Terrain->GetHeight(pos.x,pos.z);
// 			pos.y = height;
// 			m_MeshTank->SetPosition(pos);
// 			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
// 			CamPos.x -=delta;
// 			CamPos.y=pos.y+300;
// 			m_MeshTank->SetYRoation(D3DX_PI/2);
// 			m_FPSCamera->SetPos(CamPos);
// 			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
// 			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
// 			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
// 		}
// 		if (inputMng->IsKeyDown('D'))
// 		{
// 			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
// 			pos.x += delta;
// 			float height=m_Terrain->GetHeight(pos.x,pos.z);
// 			pos.y = height;
// 			m_MeshTank->SetPosition(pos);
// 			m_MeshTank->SetYRoation( -D3DX_PI/2);
// 			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
// 			CamPos.x +=delta;
// 			CamPos.y=pos.y+300;
// 			m_FPSCamera->SetPos(CamPos);
// 			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
// 			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
// 			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
// 		}
// 		if (inputMng->IsKeyDown('W'))
// 		{
// 			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
// 			pos.z += delta;
// 			float height=m_Terrain->GetHeight(pos.x,pos.z);
// 			pos.y = height;
// 			m_MeshTank->SetPosition(pos);
// 			m_MeshTank->SetYRoation(D3DX_PI);
// 			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
// 			CamPos.z +=delta;
// 			CamPos.y=pos.y+300;
// 			m_FPSCamera->SetPos(CamPos);
// 			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
// 			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
// 			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
// 		}
// 		if (inputMng->IsKeyDown('S'))
// 		{
// 			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
// 			pos.z -= delta;
// 			float height=m_Terrain->GetHeight(pos.x,pos.z);
// 			pos.y = height;
// 			m_MeshTank->SetPosition(pos);
// 			m_MeshTank->SetYRoation(0);
// 			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
// 			CamPos.z -=delta;
// 			CamPos.y=pos.y+300;
// 			m_FPSCamera->SetPos(CamPos);
// 			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
// 			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
// 			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
// 		}
		
		
	}

	int TestApp::OnWndSize(DxSizeEvent& e)
	{
		m_RenderInfo.width=e.w;
		m_RenderInfo.height=e.h;
		//PopupError("width=%d,height=%d",e.w,e.h);
		//设置backbuffer和viewport


		HRESULT hr;
		OnReset(hr);
		D3DVIEWPORT9 vp={0,0,m_RenderInfo.width,m_RenderInfo.height,0,1};
		GetGlobalDevice()->SetViewport(&vp);
		return 1;
	}

	void TestApp::OnReset(HRESULT& hr)
	{
		IDirect3DDevice9* Device=GetGlobalDevice();
		//on lost
	//	m_Snow->OnLostDevice();
		m_Text->OnLostDevice();

		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.BackBufferWidth=m_RenderInfo.width;
		d3dpp.BackBufferHeight=m_RenderInfo.height;
		d3dpp.BackBufferCount=1;
		d3dpp.BackBufferFormat=m_RenderInfo.backFMT;

		d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality=0; //多重采样质量水平

		d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD; //交换链中的缓存的页面置换方式
		d3dpp.hDeviceWindow=m_RenderInfo.hwnd;
		d3dpp.Windowed=m_RenderInfo.bwindowed;  //窗口模式
		d3dpp.EnableAutoDepthStencil=true;  //d3d自动创建并维护深度缓存或模板缓存
		d3dpp.AutoDepthStencilFormat=m_RenderInfo.zFMT;//深度缓存或模板缓存的像素格式
		d3dpp.Flags=0;
		d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;//刷新频率
		d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;

		hr=Device->Reset(&d3dpp);
		if (hr==D3DERR_DEVICELOST)
		{
			DXWriteConsol(CC_GREEN,"Error=D3DERR_DEVICELOST");
		}
		else if(hr ==D3DERR_DEVICEREMOVED)
		{
			DXWriteConsol(CC_GREEN,"Error=D3DERR_DEVICEREMOVED");
		}
		else if(D3DERR_DRIVERINTERNALERROR == hr)
		{
			DXWriteConsol(CC_GREEN,"Error=D3DERR_DRIVERINTERNALERROR");
		}
		else if(D3DERR_OUTOFVIDEOMEMORY == hr)
		{
			DXWriteConsol(CC_GREEN,"Error=D3DERR_OUTOFVIDEOMEMORY");
		}
		else if(D3DERR_INVALIDCALL == hr)
		{
			DXWriteConsol(CC_GREEN,"Error=D3DERR_INVALIDCALL");
		}

		if (FAILED(hr))
		{
			DXWriteConsol(CC_GREEN,"Error=%x",hr);
			return ;
		}
		Device->SetLight(0, &light);
		Device->LightEnable(0, true);
		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_SPECULARENABLE, true);


		Device->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		Device->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		Device->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);

		m_FPSCamera->SetViewMatrix(Device);
		m_FPSCamera->SetProjMatrix(Device);

		//on reset
		//m_Snow->OnResetDevice();
		m_Text->OnResetDecive();
	}