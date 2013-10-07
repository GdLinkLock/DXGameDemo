#pragma  once
#include "Mesh.h"
#include <d3d.h>
#include <d3dx9.h>
#include "../D3D/D3DUtil.h"
#include "../Log/ErrorInfo.h"
#include "GlobalConfig.h"
Mesh::Mesh(GeometryReource* p,const char* name)
{
	pVB=0;
	pTex=0;
	pIB=0;
	pParent=p;
	D3DXMatrixIdentity(&m_WorldMat);
	//D3DXMatrixScaling(&m_WorldMat,30,30,30);
 	 	
}
bool Mesh::LoadTexture(const char* name)
{
	if (FAILED(D3DXCreateTextureFromFileA(GetGlobalDevice(),name,&pTex)))
	{
		DXWriteConsol(CC_RED,"Error!");
		PopupError("Mesh::Mesh");
		return false;
	}
	return true;
}
Mesh::~Mesh()
{
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pTex);
}
D3DXVECTOR3 Mesh::GetPostion()
{
	return D3DXVECTOR3(m_WorldMat._41,m_WorldMat._42,m_WorldMat._43);
}
void Mesh::SetYRotation(float angle)
{
	SetMatWorld(GetPostion(),angle);
}
void Mesh::SetMatWorld(const D3DXVECTOR3& pos,const float angle/* =0.0f */,const float scale/* =1.0f */)
{
	D3DXMATRIXA16 world1,world2;
	D3DXMatrixIdentity(&world1);
	D3DXMatrixIdentity(&world2);

	//rotation
	D3DXMatrixRotationZ(&world1,angle);
	D3DXMatrixScaling(&world2,scale,scale,scale);
	D3DXMatrixMultiply(&world1,&world1,&world2);

	D3DXMatrixTranslation(&world2,pos.x,pos.y,pos.z);
	D3DXMatrixMultiply(&m_WorldMat,&world1,&world2);
}
void Mesh::Render()
{
	//顶点缓存，FVF，索引缓存，纹理

	IDirect3DDevice9* m_pDevice=GetGlobalDevice();
	//m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
	m_pDevice->SetTransform(D3DTS_WORLD,&m_WorldMat);
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
 	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );//将纹理颜色混合的第一个参数用于输出
 	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE  );//将纹理颜色混合的第一个参数的值去贴图颜色的值
	//光照，mat,draw
	m_pDevice->SetMaterial(&D3D::WHITE_MTRL);
	m_pDevice->SetTexture(0,pTex);


	m_pDevice->SetStreamSource(0,pVB,0,sizeof(tVertex));//绑定数据
	m_pDevice->SetIndices(pIB);
	m_pDevice->SetFVF(FVF_VERTEX);
	if (GlobalConfig::EnableWireFrame())
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	}
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_Desc.vertexNum,0,m_Desc.faceNum);
	grunTimeInfo.fTriangleNum +=m_Desc.faceNum;
	grunTimeInfo.aTriangleNum+=m_Desc.faceNum;
	grunTimeInfo.aDrawCoreNum++;
	grunTimeInfo.fDrawCoreNum++;
	if (GlobalConfig::EnableWireFrame())
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	}
	m_pDevice->SetRenderState(D3DRS_LIGHTING,true);
}
void Mesh::SetData(char* buf,int length,const char* name)
 {
}

void Mesh::SetIndexData(uint32* buf,int length)
{
	if (pIB)
	{
		SAFE_RELEASE(pIB);
		pIB=0;
	}
	HRESULT hr=GetGlobalDevice()->CreateIndexBuffer(length * sizeof(uint32),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pIB,0);
	if (FAILED(hr))
	{
		PopupError("fsdfdsf");
	}
	WORD* pIndex=0;
	pIB->Lock(0,0,(void**)&pIndex,0);
	for (int i=0;i<length;i++)
		pIndex[i]=buf[i];
	pIB->Unlock();

}
void Mesh::SetVertexData(tVertex* buf,int length)
{
	HRESULT hr=GetGlobalDevice()->CreateVertexBuffer(length*sizeof(tVertex),D3DUSAGE_WRITEONLY,FVF_VERTEX,D3DPOOL_MANAGED,&pVB,0);
	if (FAILED(hr))
	{
		PopupError("Mesh::SetVertexData");
	}
	tVertex* pVertexs;
	if (FAILED(pVB->Lock(0,0,(void**)&pVertexs,0)))
	{
		return ;
	}
	memcpy(pVertexs,buf,length*sizeof(tVertex));
	pVB->Unlock();
}

void Mesh::Cube()
{
}

bool Mesh::Draw()
{
 	return true;
}