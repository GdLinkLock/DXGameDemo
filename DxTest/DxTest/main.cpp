// DxTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../Game/GameApp.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd )
{

	std::tr1::shared_ptr<TestApp> pApp=std::make_shared<TestApp>("DxApp",WINDOW_WIDTH,WINDOW_HIGHT,0,0);
	InitGlobal(pApp->GetWnd()->GetHwnd(),WINDOW_WIDTH,WINDOW_HIGHT);
	
	pApp->Init();
	pApp->StartRun();
	pApp->ShudDown();

	return 1;
}

