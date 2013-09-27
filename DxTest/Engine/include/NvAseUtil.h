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

	//!����Ƿ����Ӳ��ʣ���Ϊ��Щģ��ָ�����Ӳ���ID������ȴû�и����ά�������͵���Ӱ��ʵ���༭��add by lh, �����Ӳ���ID��false����0
	int	bHasMultiSubMat(int matid, int submat, mat_array& matlib);

	//!�ϲ�strip
	bool CombineStrips(strip_array& strips,std::vector<uint32>& strip,std::vector<uint32>& tri,uint32& nPolyNum, uint32& orgPolyCount);

	std::string StripObjName(const std::string& name, uint32 subMatid);
