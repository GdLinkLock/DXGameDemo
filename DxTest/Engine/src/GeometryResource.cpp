#include "GeometryResource.h"

GeometryReource::GeometryReource()
{

}
GeometryReource::~GeometryReource()
{
	
};
int GeometryReource::CreateMesh(const tMeshDesc& meshDesc)
{
	if(strlen(meshDesc.m_meshName)!=0)
	{
		//´´½¨MESH
		Mesh* pMesh = CreateMesh(meshDesc.m_meshName);
		if(pMesh)
		{
			//bool res = BuildMesh(pMesh,meshDesc);
// 			if(!res)
// 			{
// 				return 0;
// 			}
			SetMeshDesc(meshDesc);

			return 1;
		}
	}
	return 0;
}
void GeometryReource::SetMeshDesc(const tMeshDesc& meshDesc)
{
	Mesh* pMesh=GetMeshByName(meshDesc.m_meshName);
	if (pMesh)
	{
		pMesh->m_Desc.vertexNum=meshDesc.m_vertCount;
		pMesh->m_Desc.faceNum=meshDesc.m_triCount;
	}
}

Mesh* GeometryReource::CreateMesh(const std::string& name)
{
	assert(GetMeshByName(name)==0);
	Mesh* pSub = CreateMeshImpl(name);
	m_Meshes.push_back(pSub);
	return pSub;
}
Mesh* GeometryReource::CreateMeshImpl(const  std::string& meshName)
{
	Mesh* pMesh = new Mesh(this, meshName.c_str());
	return pMesh;
}
Mesh* GeometryReource::GetMeshByName(const std::string& name)
{
	for (size_t i=0; i<m_Meshes.size(); ++i)
	{
		Mesh* pSub = m_Meshes[i];

		//if (pSub&&pSub->GetName()==name)
		{
			return pSub;
		}
	}

	return 0;
}
void GeometryReource::SetChildScaleInfo(float* scale)
{
	m_vScales.push_back(GdVec3(scale[0],scale[1],scale[2]));
}
void GeometryReource::SetChildPosInfo(float* pos)
{
	m_vPositions.push_back(GdVec3(pos[0],pos[1],pos[2]));
}
void GeometryReource::SetChildRotInfo(float* rot)
{
	m_vRotations.push_back(GdQuat(rot[0],rot[1],rot[2],rot[3]));
}

