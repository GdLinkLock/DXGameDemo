#include "SkyBox.h"
#include "../Log/ErrorInfo.h"
#include "D3DUtil.h"

SkyBox::SkyBox(IDirect3DDevice9* device):m_pDevice(device),m_pVB(0)
{
	for (int i = 0; i < 5; i++)
	{
		m_TexArray[i]=0;
	}
	m_Length=0;
	D3DXMatrixIdentity(&m_matWorld);
}
SkyBox::~SkyBox()
{
	SAFE_RELEASE(m_pVB);
	for (int i=0;i<5;i++)
	{
		SAFE_RELEASE(m_TexArray[i]);
	}

}

bool SkyBox::InitSkyBox(float length)
{
	m_Length=length;
	//创建顶点缓存
	m_pDevice->CreateVertexBuffer(20*sizeof(tSkyBoxVertex),0,tSkyBoxVertex::FVF,D3DPOOL_MANAGED,&m_pVB,0);

	tSkyBoxVertex vertex[]=
	{
		//前面的四个顶点
		{ -m_Length/2, 0.0f,    m_Length/2, 0.0f, 1.0f, },
		{ -m_Length/2, m_Length/2,   m_Length/2, 0.0f, 0.0f, },
		{  m_Length/2, 0.0f,    m_Length/2, 1.0f, 1.0f, },
		{  m_Length/2, m_Length/2,   m_Length/2, 1.0f, 0.0f, },

		//背面的四个顶点
		{  m_Length/2, 0.0f,   -m_Length/2, 0.0f, 1.0f, },
		{  m_Length/2, m_Length/2,  -m_Length/2, 0.0f, 0.0f, },
		{ -m_Length/2, 0.0f,   -m_Length/2, 1.0f, 1.0f, },
		{ -m_Length/2, m_Length/2,  -m_Length/2, 1.0f, 0.0f, },

		//左面的四个顶点
		{ -m_Length/2, 0.0f,   -m_Length/2, 0.0f, 1.0f, },
		{ -m_Length/2, m_Length/2,  -m_Length/2, 0.0f, 0.0f, },
		{ -m_Length/2, 0.0f,    m_Length/2, 1.0f, 1.0f, },
		{ -m_Length/2, m_Length/2,   m_Length/2, 1.0f, 0.0f, },

		//右面的四个顶点
		{ m_Length/2, 0.0f,   m_Length/2, 0.0f, 1.0f, },
		{ m_Length/2, m_Length/2,  m_Length/2, 0.0f, 0.0f, },
		{ m_Length/2, 0.0f,  -m_Length/2, 1.0f, 1.0f, },
		{ m_Length/2, m_Length/2, -m_Length/2, 1.0f, 0.0f, },

		//上面的四个顶点
		{  m_Length/2, m_Length/2, -m_Length/2, 1.0f, 0.0f, },
		{  m_Length/2, m_Length/2,  m_Length/2, 1.0f, 1.0f, },
		{ -m_Length/2, m_Length/2, -m_Length/2, 0.0f, 0.0f, },
		{ -m_Length/2, m_Length/2,  m_Length/2, 0.0f, 1.0f, },
	};
	void* pVertexs;
	if (FAILED(m_pVB->Lock(0,0,(void**)&pVertexs,0)))
	{
		PopupError("SkyBox::Lock,Error!");
		return false;
	}
	memcpy(pVertexs,vertex,sizeof(vertex));
	m_pVB->Unlock();

	return true;
}

bool SkyBox::LoadSkyTextureFromFile(const char* front,const char* back,const char* left,const char* right,const char* top)
{
	D3DXCreateTextureFromFile(m_pDevice,front,&m_TexArray[0]);
	D3DXCreateTextureFromFile(m_pDevice,back,&m_TexArray[1]);
	D3DXCreateTextureFromFile(m_pDevice,left,&m_TexArray[2]);
	D3DXCreateTextureFromFile(m_pDevice,right,&m_TexArray[3]);
	D3DXCreateTextureFromFile(m_pDevice,top,&m_TexArray[4]);
	return true;
}

void SkyBox::SetMatWorld(D3DXMATRIX mat)
{
	m_matWorld=mat;
}

void SkyBox::RenderSkyBox()
{
	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );//将纹理颜色混合的第一个参数用于输出
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE  );//将纹理颜色混合的第一个参数的值去贴图颜色的值
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);//镜面
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	//
	m_pDevice->SetTransform(D3DTS_WORLD,&m_matWorld);
	m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(tSkyBoxVertex));//将包含几何信息的顶点和渲染流水线绑定
	m_pDevice->SetFVF(tSkyBoxVertex::FVF);

	//绘制5个面
	for (int i=0;i<5;i++)
	{
		m_pDevice->SetTexture(0,m_TexArray[i]);
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,i*4,2);
	}
}