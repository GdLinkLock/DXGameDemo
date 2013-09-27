#pragma  once
#include "STLDefine.h"
#include "Mesh.h"
#include "dMathHeader.h"
#include "Head.h"
class GeometryReource
{
public:
	GeometryReource();
	~GeometryReource();
	   Mesh* CreateMesh(const std::string& name);
	   Mesh* CreateMeshImpl(const  std::string& meshName);
	   Mesh* GetMeshByName(const std::string& name);
		virtual int CreateMesh(const tMeshDesc& meshDesc);
		virtual void SetMeshDesc(const tMeshDesc& meshDesc); 
		virtual void SetChildPosInfo(float* pos);
		virtual void SetChildRotInfo(float* rot);
		virtual void SetChildScaleInfo(float* scale);
		std::vector<Mesh*>& GetMeshList(){return m_Meshes;};
private:
	std::vector<Mesh*> m_Meshes;
	//aabb
	//
	tGeometryDesc m_GeoDesc;
	std::vector<int> m_vParentId;
	std::vector<GdVec3> m_vPositions;
	std::vector<GdVec3> m_vScales;
	std::vector<GdQuat> m_vRotations;
};