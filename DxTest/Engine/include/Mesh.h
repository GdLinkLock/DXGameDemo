#pragma  once
#include "STLDefine.h"
#include "../NV/include/nv_util/NV_ASE.H"
#include <d3dx9.h>
#include "DxGlobal.h"
#include "Head.h"
//#include "GeometryResource.h"

class GeometryReource;
struct tVertex
{
	
	tVertex()	{}
	tVertex(float _x,float _y,float _z)
	{
		x=_x;	y=_y;	z=_z;
		u=0;	v=0;
    	n1=0;n2=0;n3=0;
	}
	tVertex(float _x,float _y,float _z,float _n1,float _n2,float _n3,float _u,float _v)
	{
		x=_x;	y=_y;	z=_z;
		n1=_n1;	n2=_n2;	n3=_n3;
		u=_u;	v=_v;
	}
	float x,y,z;
	float n1,n2,n3;
	float u,v;
};
#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
class Mesh
{
public:
	friend class GeometryReource;

	Mesh(GeometryReource* p,const char* name);
	Mesh();
	virtual ~Mesh();
	void Render();
	bool LoadTexture(const char* name);
	void SetIndexData(uint32* buf,int length);
	void SetVertexData(tVertex* buf,int length);
	void SetData(char* buf,int length,const char* name);
	void Release();
	void Mesh::SetMatWorld(const D3DXVECTOR3& pos,const float angle =0.0f ,const float scale=1.0f );
	void SetYRotation(float angle);
	D3DXVECTOR3 GetPostion();
	////
	bool Draw();
	void Cube();
private:
	std::string m_meshName;
	GeometryReource* pParent;
	//vb
	IDirect3DVertexBuffer9*  pVB;
	IDirect3DTexture9* pTex;
	IDirect3DIndexBuffer9* pIB;
	std::vector<D3DMATERIAL9> m_Materails;

	tGeometryDesc m_Desc;
	D3DXMATRIX  m_WorldMat;
	//std::vector<IDirect3DTexture9*> m_Textures;
	//ib
	//aabb
	//material
};