#include "D3DXMesh.h"
#include "../Log/ErrorInfo.h"
#include <algorithm>

D3DXMesh::D3DXMesh():m_Mesh(0)
{
	D3DXMatrixIdentity(&m_MatWorld);
	m_fYangle=0;
}

D3DXMesh::~D3DXMesh()
{
	D3D::Release(m_Mesh);
	std::for_each(m_Textures.begin(),m_Textures.end(),[&](IDirect3DTexture9* p){D3D::Release(p);});
	m_Textures.clear();
}

bool D3DXMesh::CreateMesh(IDirect3DDevice9* pDevice,const char* meshName)
{
	HRESULT hr;
	ID3DXBuffer* addjBuffer;
	ID3DXBuffer* mtrlBuffer;
	DWORD  numMtrls;

	hr=D3DXLoadMeshFromX(
							meshName,
							D3DXMESH_MANAGED,
							pDevice,
							&addjBuffer,
							&mtrlBuffer,
							0,
							&numMtrls,
							&m_Mesh);
	if (FAILED(hr))
	{
		PopupError("Create Mesh Error!");
		return false;
	}

	//extract materials ,load textures
	if (mtrlBuffer!=0 && numMtrls!=0)
	{
		D3DXMATERIAL* mtrls=(D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		for (int i=0;i<numMtrls;i++)
		{
			// the MatD3D property doesn't have an ambient value set
			// when its loaded, so set it now:
			mtrls[i].MatD3D.Ambient=mtrls[i].MatD3D.Diffuse;
			//save ith material
			m_Materails.push_back(mtrls[i].MatD3D);

			//check if the ith material has associative texture
			if (mtrls[i].pTextureFilename != 0)
			{
				//yes load the texture for its subset
				IDirect3DTexture9* tex=0;
				D3DXCreateTextureFromFile(
											pDevice,
											mtrls[i].pTextureFilename,
											&tex);
				//save loaded texture
				m_Textures.push_back(tex);
			}
			else
			{
				//no texture for its subset
				m_Textures.push_back(0);
			}
		}
	}
	D3D::Release<ID3DXBuffer*>(mtrlBuffer);

	//optimize the mesh
	hr=m_Mesh->OptimizeInplace(
								D3DXMESHOPT_ATTRSORT|D3DXMESHOPT_COMPACT|D3DXMESHOPT_VERTEXCACHE,
								(DWORD*)addjBuffer->GetBufferPointer(),
								0,0,0);
	D3D::Release<ID3DXBuffer*>(addjBuffer);

	if (FAILED(hr))
	{
		PopupError("Mesh->optimize Error!");
		return false;
	}

	//set texture filters
	pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_POINT);

	//
	return true;
}

void D3DXMesh::DrawMesh(IDirect3DDevice9* pDevice)
{
	//this->SetMatWorld(this->GetPostion(),m_fYangle,1.0f);

	//开启照明
	pDevice->SetRenderState(D3DRS_LIGHTING,true);
	//设置环境光
	pDevice->SetRenderState(D3DRS_AMBIENT,0xfffffff);
	pDevice->SetTransform(D3DTS_WORLD,&m_MatWorld);

 	pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
 	pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
 	pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
 
 	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
 	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
 
 	//计划Alpha混合, 设置ALPHA混合系数
 	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  TRUE);
 	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
 	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	for (int i=0;i<m_Materails.size();i++)
	{
		pDevice->SetMaterial(&m_Materails[i]);
		pDevice->SetTexture(0,m_Textures[i]);
		m_Mesh->DrawSubset(i);
	}
	//禁用照明效果
	pDevice->SetRenderState( D3DRS_LIGHTING, FALSE ); 
}

void D3DXMesh::SetPosition(const D3DXVECTOR3& newPos)
{
// 	m_MatWorld._41=newPos.x;
// 	m_MatWorld._42=newPos.y;
// 	m_MatWorld._43=newPos.z;
	SetMatWorld(newPos);
}

D3DXVECTOR3 D3DXMesh::GetPostion()
{
	 return D3DXVECTOR3(m_MatWorld._41,m_MatWorld._42,m_MatWorld._43);
}

void D3DXMesh::SetYRoation(float angle)
{
	SetMatWorld(this->GetPostion(),angle,1.0f);
}

void D3DXMesh::SetScaleFactor(float factor)
{
	SetMatWorld(this->GetPostion(),0,factor);
}

void D3DXMesh::SetMatWorld(const D3DXVECTOR3& pos,const float angle/* =0.0f */,const float scale/* =1.0f */)
{
	D3DXMATRIXA16 world1,world2;
	D3DXMatrixIdentity(&world1);
	D3DXMatrixIdentity(&world2);

	//rotation
	D3DXMatrixRotationY(&world1,angle);
	D3DXMatrixScaling(&world2,scale,scale,scale);
	D3DXMatrixMultiply(&world1,&world1,&world2);

	D3DXMatrixTranslation(&world2,pos.x,pos.y,pos.z);
	D3DXMatrixMultiply(&m_MatWorld,&world1,&world2);
}