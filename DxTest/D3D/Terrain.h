#pragma  once
#include <d3dx9.h>
#include <string>
#include <vector>
#include <fstream>

class Terrian
{

public:
	Terrian();
	~Terrian();

	bool LoadTerrainFromFile(const char* rawFileName,const char* textureName);
	bool InitTerrain(int row,int col,float space,float scale);
	bool RenderTerrain();

private:

	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTexture;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9* m_pIB;

	int numRow;
	int numCol;
	int numVertexPerRow;
	int numVertexPerCol;
	float cellSpace;
	float heightScale;




	struct TerrainVertex
	{
		TerrainVertex(float x,float y,float z,float u,float v)
		{
			_x=x;
			_y=y;
			_z=z;
			_u=u;
			_v=v;
		}

		float _x,_y,_z;
		float _u,_v;
		static const DWORD FVF;
	};

};

