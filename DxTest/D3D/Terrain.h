#pragma  once
#include <d3dx9.h>
#include <string>
#include <vector>
#include <fstream>
#include "D3DUtil.h"


class Terrian
{

public:
	Terrian(IDirect3DDevice9* Device);
	~Terrian();

	//从指定文件加载高度信息和地形纹理
	bool LoadTerrainFromFile(const char* rawFileName,const char* textureName);
	//初始化地形
	bool InitTerrain(int row,int col,float space,float scale);
	void RenderTerrain();

	float GetHeight(float x,float z);
	float GetHeightByRowCol(int row,int col);

	//设置填充模式
	void SetRenderFrame(bool frame);
private:

	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTexture;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9* m_pIB;

	int numCellPerRow;//行单元格数=每行顶点数-1
	int numCellPerCol;//每列单元格数=每列顶点数-1
	int numVertexPerRow;//每行顶点
	int numVertexPerCol;//每列顶点
	int numVertexes; //单元格总数
	float cellSpace; //单元格间距
	float m_Width;
	float m_Depth;//

	float heightScale;//高度缩放比例

	std::vector<float> m_HeightInfo;

	D3DXMATRIX m_WorldMat;

	bool bWireFrame;
public:


	struct tTerrainVertex
	{
		tTerrainVertex(float x,float y,float z,float u,float v)
		{
			_x=x;
			_y=y;
			_z=z;
			_u=u;
			_v=v;
		}

		float _x,_y,_z;
		float _u,_v;
		static const DWORD FVF=D3DFVF_TEX1|D3DFVF_XYZ;
	};
};

