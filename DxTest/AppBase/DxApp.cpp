#include "DxApp.h"
#include <Windows.h>

DxAppBase::DxAppBase(const char* name,unsigned int w,unsigned int h,unsigned int left,unsigned top)
{
	m_pWnd=std::tr1::make_shared<DxWindown>(name,w,h,left,top);
}

DxAppBase::~DxAppBase()
{

}


void DxAppBase::Init()
{
	InitApp();
}

void DxAppBase::ShudDown()
{
	ShutDownApp();
}

void DxAppBase::StartRun()
{
	MSG msg;
	msg.message=NULL;

	PeekMessage(&msg,NULL,0u,0u,PM_REMOVE);
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg,NULL,0u,0u,PM_REMOVE) != 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
 			static DWORD last=0;
 			static DWORD cur=0;
			last=cur;
			//cur=GetTickCount();
			cur=timeGetTime();
 			DWORD time=(cur-last);
 			
// 			static DWORD current_t=0;
// 			static DWORD last_t=0;
// 			last_t=current_t;
// 			current_t=timeGetTime();
// 			DWORD time=current_t-last_t;

			Update((float)time);
			Render((float)time);
		}
	}

}