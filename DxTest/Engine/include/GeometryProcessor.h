#pragma  once
#include "STLDefine.h"
#include "NV_ASE.H"
#include "GeometryResource.h"
using namespace  ase;
typedef unsigned int uint32;
struct tFaceByMat
{
	std::string m_shaderName;
	//triangleList用
	std::vector<uint32> m_faces;
	//strip直接记录一个所用的t_strip_mat
	t_strip_mat* m_pStripMat;
	//自动生成的名字,用作meshName
	std::string m_meshName;
	//属于哪个Geo
	uint32 m_geomId;
	//属于的geo
	geomobj* m_pGeoObj;

	//是否要toLocal
	bool m_toLocal;
	//是否要反转UV
	bool m_bFlipV;

	tFaceByMat()
	{
		m_geomId = 0;
		m_pGeoObj = 0;
		m_pStripMat = 0;
		m_toLocal = false;
		m_bFlipV = true;
	}
};


class GeometryProcessor 
{
public:
	GeometryProcessor(){};
	~GeometryProcessor(){};
public:
	int ProcessResource(const std::string&  fn,GeometryReource* pRes);

	int Parse();
	void ParseByTriangleList(geomobj* obj,unsigned int geoIndex);
	void ParseByStrip(geomobj* obj,unsigned int geoIndex);

	int Fill();
	void FillByTriangleList(GeometryReource* pGeoComposor,tFaceByMat& curFaces,tMeshDesc& meshDesc);
	void FillByStrip(GeometryReource* pGeoComposor,tFaceByMat& curFace,tMeshDesc& meshDesc);

	void GetPosBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace);
	void GetNorBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace);
	void GetTanBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace);
	void GetUVBuffer(float* pBuffer,tFaceByMat& curFace, const float* t, const unsigned int* tf);
	void GetColorBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace);

	void ProcessLod(GeometryReource* pGeoComposor,t_strip_mat* pStrip,tMeshDesc& meshDesc);
	//如果当前SIZE大于MAXSIZE，则更新MAXSIZE
	uint32* GetuintBuffer(uint32 needSize);
	float* GetfBuffer(uint32 needSize);
protected:
	//ase model
	model* m_pModel;
	//按照材质分组的三角型集合
	std::vector<tFaceByMat> m_faces;
	//GeometryReource
	GeometryReource* m_pGeometry;
	float m_geoMatrix[16];

	float* pbuffer;
};

class GeometryProcessBuffer
{
public:
	GeometryProcessBuffer();
	virtual ~GeometryProcessBuffer();
	//初始化
	virtual void Init(uint32 bSize);
	//清理
	virtual void CleanUp();
	//获取读取geometry用到的buffer
	unsigned char* GetGeometryBuffer(unsigned int bSize);
private:
	GeometryProcessBuffer(const GeometryProcessBuffer& );
	GeometryProcessBuffer& operator =(const GeometryProcessBuffer& );
protected:
	uint32 m_bufferSize;
	unsigned char* m_buffer;
};