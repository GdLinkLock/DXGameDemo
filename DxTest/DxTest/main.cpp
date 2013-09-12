// DxTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../Game/GameApp.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{

	std::tr1::shared_ptr<TestApp> pApp=std::make_shared<TestApp>("DxApp",WINDOW_WIDTH,WINDOW_HIGHT,0,0);
	InitGlobal(pApp->GetWnd()->GetHwnd(),WINDOW_WIDTH,WINDOW_HIGHT);
	RenderInfo info;
	info.width=WINDOW_WIDTH;
	info.height=WINDOW_HIGHT;
	info.bwindowed=true;
	info.backFMT=D3DFMT_A8R8G8B8;
	info.zFMT=D3DFMT_D24S8;
	info.hwnd=pApp->GetWnd()->GetHwnd();
	pApp->SetRenderInfo(info);
	pApp->Init();
	pApp->StartRun();
	pApp->ShudDown();

	return 1;
}

