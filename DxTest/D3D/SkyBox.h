#pragma  once
#include <d3dx9.h>

class  SkyBox
{
public:
	SkyBox(IDirect3DDevice9* device);
	~SkyBox();

	bool InitSkyBox(float length);
	bool LoadSkyTextureFromFile(const char* front,const char* back,const char* left,const char* right,const char* top);
	void RenderSkyBox();

	void SetMatWorld(D3DXMATRIX mat);
public:
	struct tSkyBoxVertex
	{
		float x,y,z;
		float u,v;
		static const DWORD FVF=D3DFVF_XYZ|D3DFVF_TEX1;
	};

private:

	IDirect3DDevice9* m_pDevice;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DTexture9* m_TexArray[5];

	float m_Length;
	D3DXMATRIX m_matWorld;

};

