#pragma  once
#include "DXdefine.h"

class RenderInfo
{
public:
	RenderInfo()
	{
		hwnd=0;
		width=1024;
		height=768;
		bwindowed =true;
		backFMT=D3DFMT_A8R8G8B8;
		zFMT=D3DFMT_D24FS8;
	}

	HWND hwnd;
	unsigned int width;
	unsigned int height;
	bool bwindowed;
	_D3DFORMAT  backFMT; //back buffer
	_D3DFORMAT  zFMT; //z buffer
};