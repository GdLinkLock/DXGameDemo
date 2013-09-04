#include "Terrain.h"
#include "../Log/ErrorInfo.h"

Terrian::Terrian(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_pTexture = NULL;
	m_pIB = NULL;
	m_pVB = NULL;
	numCellPerRow = 0;
	numCellPerCol = 0;
	numVertexPerCol = 0;
	numVertexPerRow = 0;
	numVertexes = 0;
	m_Width = 0.0f;
	m_Depth = 0.0f;
	cellSpace = 0.0f;
	heightScale = 0.0f;
	D3DXMatrixIdentity(&m_WorldMat);

	bWireFrame=false;
}
Terrian::~Terrian()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pTexture);
}


bool Terrian::LoadTerrainFromFile(const char* rawFileName,const char* textureName)
{
	//���ļ�
	std::ifstream inFile;
	inFile.open(rawFileName,std::ios::binary);
	//�õ��ļ���С
	inFile.seekg(0,std::ios::end);
	std::vector<BYTE> fileData(inFile.tellg());
	inFile.seekg(0,std::ios::beg);
	inFile.read((char*)&fileData[0],fileData.size());
	inFile.close();

	//����߶���Ϣ
	m_HeightInfo.resize(fileData.size());
	for (size_t i=0;i<fileData.size();i++)
		m_HeightInfo[i]=fileData[i];

	//��������
	HRESULT hr=D3DXCreateTextureFromFile(m_pDevice,textureName,&m_pTexture);
	if (FAILED(hr))
	{
		PopupError("Create texture in Terrain!");
		return false;
	}
	return true;
}

bool Terrian::InitTerrain(int row,int col,float space,float scale)
{
	numCellPerRow = row;
	numCellPerCol = col;
	cellSpace=space;
	heightScale=scale;
	numVertexPerCol=col+1;
	numVertexPerRow=row+1;
	numVertexes=numVertexPerCol* numVertexPerRow;
	m_Width=numCellPerRow*cellSpace;
	m_Depth=numCellPerCol*cellSpace;
	//��������
	for (size_t i=0;i<m_HeightInfo.size();i++)
	{
		m_HeightInfo[i] *= scale;
	}

	//�������㻺�棬���������ʣ�unlock
	HRESULT hr;
	hr=m_pDevice->CreateVertexBuffer(numVertexes*sizeof(tTerrainVertex),D3DUSAGE_WRITEONLY,
										tTerrainVertex::FVF,D3DPOOL_MANAGED,&m_pVB,0);
	if (FAILED(hr))
	{
		PopupError("Create VB in Terrain failed!");
		return false;
	}
	tTerrainVertex* pVertex;
	m_pVB->Lock(0,0,(void**)&pVertex,0);//ǰ��������Ϊ0 ��ʾlock ��������
	float fStartX=-m_Width/2;
	float fStartZ=m_Depth/2;
	float fEndX=m_Width/2;
	float fEndZ=-m_Depth/2;
	float coordU=10.0/(FLOAT)numCellPerRow;
	float coordV=10.0/(FLOAT)numCellPerCol;

	int index=0, ii=0,j=0;
	for (float z=fStartZ;z>=fEndZ; z-=cellSpace,ii++)
	{
		j=0;
		for (float x=fStartX;x<=fEndX;x+=cellSpace,j++)
		{
			index = ii*numCellPerRow + j;
			pVertex[index] = tTerrainVertex(x,m_HeightInfo[index],z,j*coordU,ii*coordV);
			index++;//����ƺ�û��Ҫ
		}
	}
	m_pVB->Unlock();

	//�����������棬���������ʣ�unlock
	//�����С=������*6=�����ε�����*3������ÿ���������㣬ÿ���������㣬��ôһ�����ĸ�����Ҳ�������������Σ�����Ҫ18����������
	hr=m_pDevice->CreateIndexBuffer(numVertexes * 6 * sizeof(WORD),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_pIB,0);
	if (FAILED(hr))
	{
		PopupError("Create index in Terrain failed!");
		return false;
	}
	WORD* pIndex=0;
	m_pIB->Lock(0,0,(void**)&pIndex,0);
	index=0;
	//
	//  A---B
	//  |   |
	//  C---D
	//
	for (int nRow=0;nRow<numCellPerRow-1;nRow++)
	{
		for (int nCol=0;nCol<numCellPerCol-1;nCol++)
		{
			pIndex[index]=nRow* numCellPerRow + nCol;//A
			pIndex[index+1]=nRow*numCellPerRow + nCol +1;//B
			pIndex[index+2]=(nRow+1)*numCellPerRow + nCol;//C

			pIndex[index+3]=(nRow+1)*numCellPerRow + nCol;//C
			pIndex[index+4]=nRow*numCellPerRow + nCol +1;//B
			pIndex[index+5]=(nRow+1)*numCellPerRow + nCol +1;//D
			index+=6;
		}
	}
	m_pIB->Unlock();
	return true;
}

void Terrian::RenderTerrain()
{
	D3DXMatrixTranslation(&m_WorldMat, 0.0f, 0.0f, 0);
	//���㻺�棬FVF���������棬����
	m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(tTerrainVertex));//������
	m_pDevice->SetFVF(tTerrainVertex::FVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->SetTexture(0,m_pTexture);

	//���գ�mat,draw
	m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
	m_pDevice->SetTransform(D3DTS_WORLD,&m_WorldMat);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,numVertexes,0,numVertexes*2);
	m_pDevice->SetRenderState(D3DRS_LIGHTING,true);
	m_pDevice->SetTexture(0,0);

	if (bWireFrame)
	{
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,numVertexes,0,numVertexes*2);
		m_pDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	}
}

float Terrian::GetHeight(float x,float z)
{
	x = ((float)m_Width / 2.0f) + x;
	z = ((float)m_Depth / 2.0f) - z;

	x /= (float)cellSpace;
	z /= (float)cellSpace;


	float col = ::floorf(x);
	float row = ::floorf(z);

	// 
	//  A   B
	//  *---*
	//  | / |
	//  *---*  
	//  C   D

	float A = GetHeightByRowCol(row,   col);
	float B = GetHeightByRowCol(row,   col+1);
	float C = GetHeightByRowCol(row+1, col);
	float D = GetHeightByRowCol(row+1, col+1);

	float dx = x - col;
	float dz = z - row;

	float height = 0.0f;
	if(dz < 1.0f - dx) 
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		height = A + D3D::Lerp(0.0f, uy, dx) + D3D::Lerp(0.0f, vy, dz);
	}
	else
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		height = D + D3D::Lerp(0.0f, uy, 1.0f - dx) + D3D::Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

float Terrian::GetHeightByRowCol(int row,int col)
{
	int idx=row*numCellPerRow+col;
	return m_HeightInfo[idx];
}

void Terrian::SetRenderFrame(bool frame)
{
	bWireFrame=frame;
}