#pragma  once

#include <d3dx9.h>
#include "../D3D/D3DUtil.h"
#include "../CommenCode/InPut/DXInPutManager.h"
#include "../D3D/RunTimeInfo.h"

enum CONSOL_COLOR
{
	CC_RED,
	CC_GREEN,
	CC_BLUE,
	CC_WHITE
};

extern RunTimeInfo grunTimeInfo;


void InitGlobal(HWND hwnd,unsigned int width,unsigned int hight);

IDirect3DDevice9* GetGlobalDevice();

DXInputManager* GetGlobalInputManager();


WORD MakeColor(CONSOL_COLOR color);
void DXWriteConsol(CONSOL_COLOR color,char* szstring,...);

void FrameBegin();
void FrameEnd();
DWORD GetFrameTime();