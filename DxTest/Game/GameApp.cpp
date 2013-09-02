#include "GameApp.h"

void TestApp::InitCamera()
{
	m_FPSCamera=std::make_shared<FPSCamera>();
	m_FPSCamera->SetPos(D3DXVECTOR3(0.0f, 300.0f, -700.0f));  //设置摄像机所在的位置
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
				D3DXVECTOR3 pos=m_DXMesh->GetPostion();
				m_FPSCamera->SetPos(D3DXVECTOR3(pos.x,pos.y+300,pos.z-700));
				m_FPSCamera->SetLookAt(pos);
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

		//==============================================================================

		m_DXMesh=std::make_shared<D3DXMesh>();
		m_DXMesh->CreateMesh(Device,"castle.x");
		m_DXMesh->SetPosition(D3DXVECTOR3(-900,0,0));


		m_MeshTank=std::make_shared<D3DXMesh>();
		m_MeshTank->CreateMesh(Device,"tank_gudao.x");
		m_DXMesh->SetPosition(D3DXVECTOR3(500,0,500));


		m_MeshTree=std::make_shared<D3DXMesh>();
		m_MeshTree->CreateMesh(Device,"shu.x");
		D3DXVECTOR3 treePos=m_MeshTree->GetPostion();
		treePos.x =-100;
		treePos.z=-50;
		treePos.y=0;
		m_MeshTree->SetPosition(treePos);

		m_SkyBox=std::make_shared<SkyBox>(Device);
		m_SkyBox->LoadSkyTextureFromFile("frontsnow1.jpg",
			"backsnow1.jpg",
			"leftsnow1.jpg",
			"rightsnow1.jpg",
			"topsnow1.jpg");
		m_SkyBox->InitSkyBox(40000);


		// 创建地面顶点缓存
		Device->CreateVertexBuffer(4 * sizeof(D3D::Vertex), 0, 
			D3D::Vertex::FVF, D3DPOOL_MANAGED, &mFloorVB, NULL);

		D3D::Vertex *pVertices = NULL;
		mFloorVB->Lock(0, 0, (void**)&pVertices, 0);
		pVertices[0] = D3D::Vertex(-5000.0f, 0.0f, -5000.0f,  0.0f, 30.0f);
		pVertices[1] = D3D::Vertex(-5000.0f, 0.0f,  5000.0f,  0.0f,  0.0f);
		pVertices[2] = D3D::Vertex( 5000.0f, 0.0f, -5000.0f, 30.0f, 30.0f); 
		pVertices[3] = D3D::Vertex( 5000.0f, 0.0f,  5000.0f, 30.0f,  0.0f);
		mFloorVB->Unlock();


		//创建地面纹理
		D3DXCreateTextureFromFile(Device, "terrain_grass.jpg", &mFloorTex);
		Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		// Set Lights.
		//

		D3DXVECTOR3 dir(1.0f, -1.0f, 1.0f);
		D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
		D3DLIGHT9 light = D3D::InitDirectionalLight(&dir, &col);

		Device->SetLight(0, &light);
		Device->LightEnable(0, true);
		Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		Device->SetRenderState(D3DRS_SPECULARENABLE, true);


		D3DXMATRIX V;
		m_FPSCamera->GetViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		//
		// Set projection matrix.
		//

		D3DXMATRIX proj;
		D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)WINDOW_WIDTH / (float)WINDOW_HIGHT,
			1.0f,
			150000.0f);
		Device->SetTransform(D3DTS_PROJECTION, &proj);
	}


	void TestApp::Render(float timeDelta)
	{
		IDirect3DDevice9* Device=GetGlobalDevice();
		UpdateCamera(timeDelta);
		FrameBegin();

		//
		// Render
		//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0xffffffff, 1.0f, 0);
		Device->BeginScene();

		m_DXMesh->DrawMesh(Device);
		m_MeshTank->DrawMesh(Device);
		m_MeshTree->DrawMesh(Device);


		D3DXMATRIX matFloor;
		D3DXMatrixTranslation(&matFloor, 0.0f, 0.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &matFloor);
		Device->SetStreamSource(0, mFloorVB, 0, sizeof(D3D::Vertex));
		Device->SetFVF(D3D::Vertex::FVF);
		Device->SetTexture(0, mFloorTex);
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//绘制天空
		D3DXMATRIX matSky,matTransSky,matRotSky;
		D3DXMatrixTranslation(&matTransSky,0.0f,-3500.0f,0.0f);
		D3DXMatrixRotationY(&matRotSky, -0.000005f*timeGetTime());   //旋转天空网格, 简单模拟云彩运动效果
		matSky=matTransSky*matRotSky;
		m_SkyBox->RenderSkyBox(matSky);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);

		//-----------------------------------------------------
		GetGlobalInputManager()->ResetPerFrame();
		FrameEnd();
	}

	void TestApp::Update(float interval)
	{
		DXInputManager* inputMng=GetGlobalInputManager();
		static float posX=0,posY=0,posZ=0;
		float delta=interval* 0.1f;


		if (inputMng->IsKeyDown('A'))
		{
			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
			pos.x -= delta;
			m_MeshTank->SetPosition(pos);
			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
			CamPos.x -=delta;
			//m_MeshTank->SetYAngle(m_FPSCamera->GetYAngle() + D3DX_PI/2);
			m_MeshTank->SetYRoation(/*m_FPSCamera->GetYAngle() + */D3DX_PI/2);
			m_FPSCamera->SetPos(CamPos);
			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
		}
		if (inputMng->IsKeyDown('D'))
		{
			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
			pos.x += delta;
			m_MeshTank->SetPosition(pos);
			//m_MeshTank->SetYAngle(m_FPSCamera->GetYAngle() - D3DX_PI/2);
			m_MeshTank->SetYRoation(/*m_FPSCamera->GetYAngle()*/ -D3DX_PI/2);
			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
			CamPos.x +=delta;
			m_FPSCamera->SetPos(CamPos);
			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
		}
		if (inputMng->IsKeyDown('W'))
		{
			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
			pos.z += delta;
			m_MeshTank->SetPosition(pos);
			//m_MeshTank->SetYAngle(m_FPSCamera->GetYAngle()  + D3DX_PI);
			m_MeshTank->SetYRoation(/*m_FPSCamera->GetYAngle() + */D3DX_PI);
			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
			CamPos.z +=delta;
			m_FPSCamera->SetPos(CamPos);
			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
		}
		if (inputMng->IsKeyDown('S'))
		{
			D3DXVECTOR3 pos=m_MeshTank->GetPostion();
			pos.z -= delta;
			m_MeshTank->SetPosition(pos);
			//m_MeshTank->SetYAngle(m_FPSCamera->GetYAngle());
			m_MeshTank->SetYRoation(/*m_FPSCamera->GetYAngle()*/0);
			D3DXVECTOR3 CamPos=m_FPSCamera->GetPostion();
			CamPos.z -=delta;
			m_FPSCamera->SetPos(CamPos);
			m_FPSCamera->SetLookAt(m_MeshTank->GetPostion());
			DXWriteConsol(CC_GREEN,"TankPos=(%f,%f,%f)",pos.x,pos.y,pos.z);
			DXWriteConsol(CC_GREEN,"CamPos=(%f,%f,%f)",CamPos.x,CamPos.y,CamPos.z);
		}

	}