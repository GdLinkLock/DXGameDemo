#include "NvAseUtil.h"


	const char*	GetMeshShaderName(int matid, int submat, mat_array& matlib)
	{
		if (matid<0)//no material on it
			return (const char*)NULL;

		matobj* mat = 0;
		matobj* matnode = matlib[matid];


		//get the root material node
		mat = GetMaterialLeaf(matnode,submat);

		if (!mat)
			return (const char*)NULL;

		return mat->name;
	}

	matobj* GetMaterialLeaf(matobj* matnode, uint32 submat)
	{
		if (!matnode) return 0;

		if (matnode->classname)
		{
			if (!strcmp(matnode->classname,"\"Multi/Sub-Object\""))
				return GetMaterialLeaf(matnode->submat[submat],submat);
		}

		if (!matnode->submat.size() || submat >= matnode->submat.size() )//no sub material , it's a standard material
			return matnode;

		return GetMaterialLeaf(matnode->submat[submat],submat);
	}

	/// 只允许有一层多维材质，即多维材质的子材质不能再是多维材质
	int bHasMultiSubMat(int matid, int submat, mat_array& matlib)
	{
		if (matid<0)//no material on it
			return 0;

		matobj* mat = 0;
		matobj* matnode = matlib[matid];

		if (matnode->classname)
		{
			if (strcmp(matnode->classname,"\"Multi/Sub-Object\"") == 0)
			{
				/// 返回子材质id
				return submat;
			}
			else
			{
				return 0;
			}
		}

		if (!matnode->submat.empty())
		{
			/// 返回子材质id
			return submat;
		}

		return 0;
	}

	bool CombineStrips(strip_array& strips, std::vector<uint32>& strip, std::vector<uint32>& tri, uint32& nPolyNum, uint32& orgPolyCount)
	{
		//if (GlobalConfig::UseTriList())
		if (true)
		{
			strip.clear();
			for (size_t i=0; i<strips.size(); ++i)
			{
				const t_strip* aStrip = strips[i];
				for (size_t j=0; j<aStrip->elems.size()-2; ++j)
				{
					if (aStrip->elems[j]==aStrip->elems[j+1] ||
						aStrip->elems[j]==aStrip->elems[j+2] ||
						aStrip->elems[j+2]==aStrip->elems[j+1] )
						continue;
					++nPolyNum;
					if (j%2==0)
					{
						tri.push_back(aStrip->elems[j]);
						tri.push_back(aStrip->elems[j+1]);
						tri.push_back(aStrip->elems[j+2]);
					}
					else
					{
						tri.push_back(aStrip->elems[j+1]);
						tri.push_back(aStrip->elems[j]);
						tri.push_back(aStrip->elems[j+2]);
					}
				}
			}

			return true;
		}
		else
		{
			bool bStrip=false, bTri=false;
			for (size_t j=0;j<strips.size();++j)
			{
				orgPolyCount += strips[j]->elems.size()-2;
				if (strips[j]->elems.size()==3)
				{
					bTri = true;
				}
				else
				{
					bStrip = true;
				}

				if (bStrip&& bTri)
					break;
			}

			int nNumMesh = 0;
			if (bStrip&bTri)
				nNumMesh = 2;
			else if (bStrip||bTri)
				nNumMesh = 1;
			else
				nNumMesh=0;

			if (!nNumMesh)
			{
				return false;
			}

			for (size_t j=0;j<strips.size();++j)
			{
				if (strips[j]->elems.size()==3)
				{
					tri.push_back(strips[j]->elems[0]);
					tri.push_back(strips[j]->elems[1]);
					tri.push_back(strips[j]->elems[2]);
					++nPolyNum;
				}
				else
				{

					if (strip.size())
					{//connect and copy

						if ( strip.size()%2 )
						{
							strip.push_back(strip[strip.size()-1]);
							if (strips[j]->elems[0] !=strips[j]->elems[1])
							{//copy 2 same verts, to flip strip,
								strip.push_back(strips[j]->elems[0]);
								strip.push_back(strips[j]->elems[0]);

								for (size_t e =0;e<strips[j]->elems.size();++e)
									strip.push_back(strips[j]->elems[e]);
							}
							else
							{
								//shoudl discard strips[j]->elems[0]!
								//strip.push_back(strips[j]->elems[0]);
								for (size_t e =0;e<strips[j]->elems.size();++e)
									strip.push_back(strips[j]->elems[e]);
							}
						}
						else
						{//even, just connect and copy
							strip.push_back(strip[strip.size()-1]);
							strip.push_back(strips[j]->elems[0]);
							for (size_t e =0;e<strips[j]->elems.size();++e)
								strip.push_back(strips[j]->elems[e]);
						}

					}
					else
					{//copy
						strip.assign(strips[j]->elems.begin(),strips[j]->elems.end());
					}
				}
			}

			if (tri.size() && strip.size()&&false)
			{
				for (size_t j=0;j<tri.size()/3;j+=3)
				{
					if ( strip.size()%2 )
					{
						strip.push_back(strip[strip.size()-1]);
					}

					strip.push_back(strip[strip.size()-1]);

					strip.push_back(tri[j]);

					strip.push_back(tri[j]);
					strip.push_back(tri[j+1]);
					strip.push_back(tri[j+2]);

				}
				tri.clear();
			}

			if (strip.size())
			{
				for (size_t i=0;i<strip.size()-2;++i)
				{
					if (strip[i]==strip[i+1]||
						strip[i]==strip[i+2]||
						strip[i+1]==strip[i+2] )
						continue;

					++nPolyNum;

				}
			}

			if (tri.size())
			{
				for (size_t i=0; i<tri.size()/3; ++i)
				{
					if (tri[3*i]==tri[3*i+1]||
						tri[3*i+1]==tri[3*i+2]||
						tri[3*i]==tri[3*i+2])
					{
						continue;
					}

					++nPolyNum;
				}
			}

			return true;
		}
	}

	std::string StripObjName(const std::string& name, uint32 subMatid)
	{
		size_t head = name.find_first_not_of('\"');
		size_t tail = name.find_last_not_of('\"');

		std::string tmpName = name.substr(head, tail - head + 1);

		if (subMatid != 0)
		{
			tmpName = tmpName+"%"+xToStr(subMatid+1);
		}
		return tmpName;
	}