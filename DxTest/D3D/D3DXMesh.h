#pragma  once
#include "D3DUtil.h"
#include <vector>

class D3DXMesh
{
public:
	D3DXMesh();
	~D3DXMesh();

	bool CreateMesh(IDirect3DDevice9* pDevice,const char* meshName);
	void DrawMesh(IDirect3DDevice9* pDevice);

	void SetMatWorld(const D3DXVECTOR3& pos,const float angle=0.0f,const float scale=1.0f);
	D3DXMATRIX& GetMatWorld() {return m_MatWorld;}

	D3DXVECTOR3 GetPostion();
	void SetPosition(const D3DXVECTOR3& newPos);
	void SetRoation(float angle);
	void SetScaleFactor(float factor);
	float GetScaleFactor();

	void SetYAngle(float angle){m_fYangle =angle;}
protected:
	std::vector<D3DMATERIAL9> m_Materails;
	std::vector<IDirect3DTexture9*> m_Textures;
	ID3DXMesh*  m_Mesh;

	D3DXMATRIX m_MatWorld;

	//ÈÆYÖáµÄÐý×ª
	float m_fYangle;
};