#pragma  once
#include <d3dx9.h>
#include <d3d9.h>
#include <string>
#include <vector>
#include "../DxGlobal/DxGlobal.h"
struct tPointVertex
{
	D3DXVECTOR3 position;
	D3DXCOLOR color;
	static const DWORD FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
};

struct tAtrribute
{
	D3DXVECTOR3 vPosition;
	D3DXVECTOR3 vVelocity;
	D3DXVECTOR3 vAcceleration;
	float       fLifeTime;
	float       fAge;

	D3DXCOLOR   color;
	bool        bIsAlive;

	tAtrribute()
	{
		fLifeTime=0.f;
		fAge=0.f;
		bIsAlive=true;
	}
};

class ParticleBase
{
public:
	ParticleBase(IDirect3DDevice9* pdevice);
	virtual ~ParticleBase();

	bool InitParticle(const char* texture);

	virtual void UpdataParticle(float delta)=0;
	virtual void ResetAttribute(tAtrribute& attr)=0;
	void RenderParticle();


	void AddAttribute();
	void RemoveDeadParticle();
	

	bool IsEmpty();
	bool IsDead();

	//设备丢失
	void OnLostDevice();
	void OnResetDevice();
	void OnDestroyDevice();
private:
	void BeforRender();
	void AfterRender();
protected:
	IDirect3DDevice9* m_pDevice;
	IDirect3DTexture9* m_pTexture;
	IDirect3DVertexBuffer9* m_pVB;

	std::vector<tAtrribute> m_Attribute;

	float m_PointSize;

	//为了提高渲染效率，定义下面三个变量
	DWORD m_vbsize;
	DWORD m_offsize;
	DWORD m_batchsize;
};


class SnowParticle:public ParticleBase
{
public:
	virtual void UpdataParticle(float delta);
	virtual void ResetAttribute(tAtrribute& attr);

	SnowParticle(IDirect3DDevice9* pdevice,long int num);
	~SnowParticle();

};