#pragma once

#include "STLDefine.h"
#include <sstream>
#include "NV_ASE.H"

using namespace ase;

	template <class Type>
	inline std::string xToStr(Type i)
	{
		std::stringstream s;
		s << i;
		return s.str();
	}

	const char* GetMeshShaderName(int matid, int submat, mat_array& matlib);

	matobj* GetMaterialLeaf(matobj* matnode, uint32 submat);

	//!检查是否含有子材质，因为有些模型指定了子材质ID，但是却没有赋予多维材质类型的球，影响实例编辑，add by lh, 返回子材质ID，false返回0
	int	bHasMultiSubMat(int matid, int submat, mat_array& matlib);

	//!合并strip
	bool CombineStrips(strip_array& strips,std::vector<uint32>& strip,std::vector<uint32>& tri,uint32& nPolyNum, uint32& orgPolyCount);

	std::string StripObjName(const std::string& name, uint32 subMatid);
