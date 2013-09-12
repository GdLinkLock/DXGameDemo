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

	//��ָ���ļ����ظ߶���Ϣ�͵�������
	bool LoadTerrainFromFile(const char* rawFileName,const char* textureName);
	//��ʼ������
	bool InitTerrain(int row,int col,float space,float scale);
	void RenderTerrain();

	float GetHeight(float x,float z);
	float GetHeightByRowCol(int row,int col);

	//�������ģʽ
	void SetRenderFrame(bool frame);
private:

	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTexture;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9* m_pIB;

	int numCellPerRow;//�е�Ԫ����=ÿ�ж�����-1
	int numCellPerCol;//ÿ�е�Ԫ����=ÿ�ж�����-1
	int numVertexPerRow;//ÿ�ж���
	int numVertexPerCol;//ÿ�ж���
	int numVertexes; //��Ԫ������
	float cellSpace; //��Ԫ����
	float m_Width;
	float m_Depth;//

	float heightScale;//�߶����ű���

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

