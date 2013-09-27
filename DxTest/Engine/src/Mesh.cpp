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
	D3DXMatrixScaling(&m_WorldMat,30,30,30);
 	 	 if (FAILED(D3DXCreateTextureFromFileA(GetGlobalDevice(),"123456.jpg",&pTex)))
 	 	 {
 			 DXWriteConsol(CC_RED,"Error!");
			 PopupError("Mesh::Mesh");
 	 	 }
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
	SetMatWorld(GetPostion(),angle,30);
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
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
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
	m_pDevice->SetFVF(tVertex::FVF);
	if (GlobalConfig::EnableWireFrame())
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	}
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,m_Desc.vertexNum,0,m_Desc.faceNum);
	if (GlobalConfig::EnableWireFrame())
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	}
	m_pDevice->SetRenderState(D3DRS_LIGHTING,true);
}
void Mesh::SetData(char* buf,int length,const char* name)
 {
	 IDirect3DDevice9* m_pDevice=GetGlobalDevice();
	 HRESULT  hr=D3DXCreateTextureFromFileA(m_pDevice,name,&pTex);
	if (hr==D3DERR_OUTOFVIDEOMEMORY)
	{
		return;
	}
	else if (hr==D3DERR_NOTAVAILABLE)
	{
		return;
	}
	else if (hr==D3DERR_INVALIDCALL)
	{
		return;
	}
	else if (hr==D3DXERR_INVALIDDATA)
	{
		return;
	}
	else if (hr==E_OUTOFMEMORY)
	{
		return;
	}
		
	hr=GetGlobalDevice()->CreateVertexBuffer(length*sizeof(tVertex),D3DUSAGE_WRITEONLY,tVertex::FVF,D3DPOOL_MANAGED,&pVB,0);
	if (FAILED(hr))
	{
	}
	void* pVertexs;
	if (FAILED(pVB->Lock(0,0,(void**)&pVertexs,0)))
	{
		return ;
	}
	memcpy(pVertexs,buf,length*sizeof(tVertex));
	pVB->Unlock();
}

void Mesh::SetIndexData(uint32* buf,int length)
{
	HRESULT hr=GetGlobalDevice()->CreateIndexBuffer(length * sizeof(uint32),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&pIB,0);
	if (FAILED(hr))
	{
		PopupError("fsdfdsf");
	}
	WORD* pIndex=0;
	pIB->Lock(0,0,(void**)&pIndex,0);
// 	fill in the front face index data
// 		i[0] = 0; i[1] = 1; i[2] = 2;
// 		i[3] = 0; i[4] = 2; i[5] = 3;
// 	
// 		// fill in the back face index data
// 		i[6] = 4; i[7]  = 5; i[8]  = 6;
// 		i[9] = 4; i[10] = 6; i[11] = 7;
// 	
// 		// fill in the top face index data
// 		i[12] = 8; i[13] =  9; i[14] = 10;
// 		i[15] = 8; i[16] = 10; i[17] = 11;
// 	
// 		// fill in the bottom face index data
// 		i[18] = 12; i[19] = 13; i[20] = 14;
// 		i[21] = 12; i[22] = 14; i[23] = 15;
// 	
// 		// fill in the left face index data
// 		i[24] = 16; i[25] = 17; i[26] = 18;
// 		i[27] = 16; i[28] = 18; i[29] = 19;
// 	
// 		// fill in the right face index data
// 		i[30] = 20; i[31] = 21; i[32] = 22;
// 		i[33] = 20; i[34] = 22; i[35] = 23;
// 	pIndex[0]=0;
// 	pIndex[1]=1;
// 	pIndex[2]=2;
// 	pIndex[3]=0;
// 	pIndex[4]=3;
// 	pIndex[5]=1;
// 	pIndex[6]=4;
// 	pIndex[7]=5;
// 	pIndex[8]=6;
// 	pIndex[9]=4;
// 	pIndex[10]=7;
// 	pIndex[11]=5;
// 	pIndex[12]=8;
// 	pIndex[13]=9;
// 	pIndex[14]=10;
// 	pIndex[15]=8;
// 	pIndex[16]=11;
// 	pIndex[17]=9;
// 	pIndex[18]=12;
// 	pIndex[19]=13;
// 	pIndex[20]=14;
// 	pIndex[21]=12;
// 	pIndex[22]=15;
// 	pIndex[23]=13;
// 	pIndex[24]=16;
// 	pIndex[25]=17;
// 	pIndex[26]=18;
// 	pIndex[27]=16;
// 	pIndex[28]=19;
// 	pIndex[29]=17;
// 	pIndex[30]=20;
// 	pIndex[31]=21;
// 	pIndex[32]=22;
// 	pIndex[33]=20;
// 	pIndex[34]=21;
// 	pIndex[35]=23;
	for (int i=0;i<length;i++)
	{
		pIndex[i]=buf[i];
	//	DXWriteConsol(CC_WHITE,"index=%d",pIndex[i]);
	}
	pIB->Unlock();

}
void Mesh::SetVertexData(tVertex* buf,int length)
{
	HRESULT hr=GetGlobalDevice()->CreateVertexBuffer(length*sizeof(tVertex),D3DUSAGE_WRITEONLY,tVertex::FVF,D3DPOOL_MANAGED,&pVB,0);
	if (FAILED(hr))
	{
		PopupError("Mesh::SetVertexData");
	}
	tVertex* pVertexs;
	if (FAILED(pVB->Lock(0,0,(void**)&pVertexs,0)))
	{
		return ;
	}
// 	fill in the front face vertex data
// 		v[0] = tVertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 		v[1] = tVertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 		v[2] = tVertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
// 		v[3] = tVertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	
// 		// fill in the back face vertex data
// 		v[4] = tVertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
// 		v[5] = tVertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
// 		v[6] = tVertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
// 		v[7] = tVertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
// 	
// 		// fill in the top face vertex data
// 		v[8]  = tVertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
// 		v[9]  = tVertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
// 		v[10] = tVertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
// 		v[11] = tVertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		// fill in the bottom face vertex data
// 		v[12] = tVertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
// 		v[13] = tVertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
// 		v[14] = tVertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
// 		v[15] = tVertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		// fill in the left face vertex data
// 		v[16] = tVertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
// 		v[17] = tVertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
// 		v[18] = tVertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
// 		v[19] = tVertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		// fill in the right face vertex data
// 		v[20] = tVertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
// 		v[21] = tVertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
// 		v[22] = tVertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
// 		v[23] = tVertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	/*memcpy(pVertexs,buf,length*sizeof(tVertex));*/
	for (int i = 0; i < length; i++)
	{
		pVertexs[i]=buf[i];
	}
	pVB->Unlock();
}

void Mesh::Cube()
{
// 	save a ptr to the device
// 		IDirect3DDevice9* _device = GetGlobalDevice();
// 	
// 		_device->CreateVertexBuffer(
// 			24 * sizeof(Vertex), 
// 			D3DUSAGE_WRITEONLY,
// 			FVF_VERTEX,
// 			D3DPOOL_MANAGED,
// 			&pVB,
// 			0);
// 	
// 		Vertex* v;
// 		pVB->Lock(0, 0, (void**)&v, 0);
// 	
// 		// build box
// 	
// 		// fill in the front face vertex data
// 		v[0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
// 		v[1] = Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
// 		v[2] = Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
// 		v[3] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
// 	
// 		// fill in the back face vertex data
// 		v[4] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
// 		v[5] = Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
// 		v[6] = Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
// 		v[7] = Vertex(-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
// 	
// 		// fill in the top face vertex data
// 		v[8]  = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
// 		v[9]  = Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
// 		v[10] = Vertex( 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
// 		v[11] = Vertex( 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		// fill in the bottom face vertex data
// 		v[12] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
// 		v[13] = Vertex( 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
// 		v[14] = Vertex( 1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
// 		v[15] = Vertex(-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		// fill in the left face vertex data
// 		v[16] = Vertex(-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
// 		v[17] = Vertex(-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
// 		v[18] = Vertex(-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
// 		v[19] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		// fill in the right face vertex data
// 		v[20] = Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
// 		v[21] = Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
// 		v[22] = Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
// 		v[23] = Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
// 	
// 		pVB->Unlock();
// 	
// 		_device->CreateIndexBuffer(
// 			36 * sizeof(WORD),
// 			D3DUSAGE_WRITEONLY,
// 			D3DFMT_INDEX16,
// 			D3DPOOL_MANAGED,
// 			&pIB,
// 			0);
// 	
// 		WORD* i = 0;
// 		pIB->Lock(0, 0, (void**)&i, 0);
// 	
// 		// fill in the front face index data
// 		i[0] = 0; i[1] = 1; i[2] = 2;
// 		i[3] = 0; i[4] = 2; i[5] = 3;
// 	
// 		// fill in the back face index data
// 		i[6] = 4; i[7]  = 5; i[8]  = 6;
// 		i[9] = 4; i[10] = 6; i[11] = 7;
// 	
// 		// fill in the top face index data
// 		i[12] = 8; i[13] =  9; i[14] = 10;
// 		i[15] = 8; i[16] = 10; i[17] = 11;
// 	
// 		// fill in the bottom face index data
// 		i[18] = 12; i[19] = 13; i[20] = 14;
// 		i[21] = 12; i[22] = 14; i[23] = 15;
// 	
// 		// fill in the left face index data
// 		i[24] = 16; i[25] = 17; i[26] = 18;
// 		i[27] = 16; i[28] = 18; i[29] = 19;
// 	
// 		// fill in the right face index data
// 		i[30] = 20; i[31] = 21; i[32] = 22;
// 		i[33] = 20; i[34] = 22; i[35] = 23;
// 	
// 		pIB->Unlock();
}

bool Mesh::Draw()
{
// 	IDirect3DDevice9* _device = GetGlobalDevice();
// 
// 	_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
// 	_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
// 	_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
// 	_device->SetMaterial(&D3D::WHITE_MTRL);
// 	_device->SetTexture(0, pTex);
// 		
// 	_device->SetStreamSource(0, pVB, 0, sizeof(Vertex));
// 	_device->SetIndices(pIB);
// 	_device->SetFVF(FVF_VERTEX);
// 	_device->DrawIndexedPrimitive(
// 		D3DPT_TRIANGLELIST, 
// 		0,                  
// 		0,                  
// 		24,
// 		0,
// 		12);  
// 
 	return true;
}