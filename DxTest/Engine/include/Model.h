#pragma  once
#include "STLDefine.h"
#include "dMathHeader.h"
#include "Mesh.h"
#include "Material.h"

class Model
{
public:
	Model(const char* name,Mesh* mesh)
	{
	//	mName=name;
		mMesh=mesh;
	}
	~Model();
	void Render()
	{
		mMesh->Render();
	}
	void SetYRoation(float angle)
	{
		mMesh->SetYRotation(angle);
	}
	bool LoadTexture(const char* fn)
	{
		return mMesh->LoadTexture(fn);
	}
	void SetPosition(D3DXVECTOR3 pos)
	{
		mMesh->SetMatWorld(pos);
	}
	void SetMatWorld(D3DXVECTOR3 pos,float angle,float scale)
	{
		mMesh->SetMatWorld(pos,angle,scale);
	}
private:
	std::string  mName;
	GdMat4  mWorldMat;
	Mesh*   mMesh;
	GdVec3 mPosition;
	GdVec3 mRotation;
	GdVec3 mScale;
	
	//renderop
};