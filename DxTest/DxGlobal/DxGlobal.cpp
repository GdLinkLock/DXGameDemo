#include "DxGlobal.h"
#include <Windows.h>
#include "../D3D/RunTimeInfo.h"

IDirect3DDevice9* gpDevice=0;
HANDLE gHandle=0;
DXInputManager* gpInputManger;

DWORD gLastTime=0;
DWORD gFrameTime=0;


RunTimeInfo grunTimeInfo;

void InitGlobal(HWND hwnd,unsigned int width,unsigned int hight)
{
	static bool bInited=false;
	grunTimeInfo.Reset();
	if (!bInited)
	{
		bInited=true;
		D3D::InitD3D(hwnd,width,hight,true,D3DDEVTYPE_HAL,&gpDevice);

		//::AllocConsole();
		gHandle=::GetStdHandle(STD_OUTPUT_HANDLE);

		if (!gpInputManger)
		{
			gpInputManger=new DXInputManager;
			gpInputManger->Init();
		}
	}
}

IDirect3DDevice9* GetGlobalDevice()
{
	if (gpDevice)
	{
		return gpDevice;
	}
	return 0;
}

DXInputManager* GetGlobalInputManager()
{
	if (gpInputManger)
		return gpInputManger;
	return 0;
}


WORD MakeColor(CONSOL_COLOR color)
{
	WORD res=0;
	if (color==CC_RED)
	{
		res=FOREGROUND_RED | FOREGROUND_INTENSITY;
	}
	else if (color == CC_GREEN)
	{
		res= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	}
	else if(color == CC_BLUE)
	{
		 res=FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	}
	else if (color == CC_WHITE)
	{
		res=FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	}

	return res;
}


void DXWriteConsol(CONSOL_COLOR color,char* szstring,...)
{
	static char buffer[1024];
	va_list va;
	va_start(va,szstring);
	vsprintf_s(buffer,szstring,va);
	va_end(va);

	SetConsoleTextAttribute(gHandle,MakeColor(color));
	DWORD numWriten;
	WriteConsoleA(gHandle,buffer,(DWORD)strlen(buffer),&numWriten,0);
	WriteConsoleA(gHandle,"\n",1,&numWriten,0);
}

void FrameBegin()
{
	gLastTime=GetTickCount();
	//DXWriteConsol(CC_WHITE,"last=%f",gLastTime);
}
void FrameEnd()
{
	DWORD cutTime=GetTickCount();
	gFrameTime=cutTime-gLastTime;
}
DWORD GetFrameTime()
{
	return gFrameTime;
}