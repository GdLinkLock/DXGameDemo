#pragma  once

enum  ePrimitiveType
{
	PT_POINT=0,
	PT_LINELIST,
	PT_LINETRIP,
	PT_TRIANGLELIST,
	PT_TRIANGLESTRIP,
	PT_TRIANGLEFAN,
	PT_END
};

struct tSubLodInfo 
{
	unsigned int m_numElem;
	unsigned int m_numVisable;
	unsigned int m_offset;
	unsigned int m_subType;
	int m_bValid;

	tSubLodInfo()
	{
		m_numElem = m_numVisable = m_offset = 0;
		m_bValid = 0;
		m_subType = 0;
	}
};
struct tLodInfo 
{
	unsigned int m_numStrip;
	unsigned int m_numPoly;
	unsigned int m_stripIndex;

	tSubLodInfo m_subLodInfo[256];

	tLodInfo()
	{
		m_numStrip = m_numPoly = m_stripIndex = 0;
	}
};
struct   tGeometryDesc
{
	unsigned int faceNum;
	unsigned int vertexNum;
	tGeometryDesc()
	{
		faceNum=0;
		vertexNum=0;
	}

};

struct tMeshDesc 
{
	unsigned int m_vertCount;
	unsigned int m_triCount;
	unsigned int m_geoObjectId;
	unsigned int m_faceOffset;
	unsigned int m_vertOffset;

	int m_bUseSharedVertexData;
	int m_bUseSharedIndexData;

	char m_matName[256];
	char m_meshName[256];
	char m_geoObjName[256];

	//À©Õ¹Êý¾Ý
	float m_meshAABB[6];
	int m_meshIndex;
	int m_parMeshIndex;

	tMeshDesc()
	{
		memset(this,0,sizeof(tMeshDesc));
		m_parMeshIndex = -1;
	}
};