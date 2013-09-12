#include "Particle.h"
#include "../Log/ErrorInfo.h"
#include "D3DUtil.h"

const int  ParticleCounts=8000;
const char* texName=0;

void ParticleBase::OnLostDevice()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pTexture);
}

void ParticleBase::OnResetDevice()
{
	InitParticle(texName);
}

ParticleBase::ParticleBase(IDirect3DDevice9* pdevice)
{
	m_pDevice=pdevice;
	m_pVB=0;
	
	m_pTexture=0;
}

ParticleBase::~ParticleBase()
{
	SAFE_RELEASE(m_pVB);

	SAFE_RELEASE(m_pTexture);

}

bool ParticleBase::InitParticle(const char* texture)
{
	srand(GetTickCount());
	texName=texture;
	//创建顶点缓存，加载贴图
	HRESULT hr;
	hr=m_pDevice->CreateVertexBuffer(m_vbsize*sizeof(tPointVertex),
									D3DUSAGE_DYNAMIC|D3DUSAGE_POINTS| D3DUSAGE_WRITEONLY,
									 tPointVertex::FVF,
										D3DPOOL_DEFAULT,
										&m_pVB,0);
	grunTimeInfo.vbSize +=m_vbsize*sizeof(tPointVertex);
	if (FAILED(hr))
	{
		PopupError("Create Vb in Particle Error!");
		return false;
	}
	
	hr=D3DXCreateTextureFromFileA(m_pDevice,texture,&m_pTexture);
	if (FAILED(hr))
	{
		PopupError("Create Texture in Particle Error!");
		return false;
	}

	return true;
}
void ParticleBase::AddAttribute()
{
	tAtrribute attr;
	ResetAttribute(attr);
	m_Attribute.push_back(attr);
}

void ParticleBase::RenderParticle()
{
	if (!m_Attribute.empty())
	{
		BeforRender();//设置必要的渲染状态

		m_pDevice->SetTexture(0,m_pTexture);
		m_pDevice->SetFVF(tPointVertex::FVF);
		m_pDevice->SetStreamSource(0,m_pVB,0,sizeof(tPointVertex));

		if (m_offsize >= m_vbsize)
			m_offsize=0;

		tPointVertex* pVertex;
		m_pVB->Lock(m_offsize*sizeof(tPointVertex),m_batchsize*sizeof(tPointVertex),(void**)&pVertex,
						m_offsize?D3DLOCK_NOOVERWRITE:D3DLOCK_DISCARD);

		DWORD numIniBatch=0;

		std::vector<tAtrribute>::iterator itr;
		for (itr=m_Attribute.begin();itr!=m_Attribute.end();++itr)
		{
			if (itr->bIsAlive)
			{
				pVertex->position=itr->vPosition;
				pVertex->color=(D3DXCOLOR)itr->color;
				pVertex++;//移动到下一个顶点

				numIniBatch++;

				if (numIniBatch == m_batchsize)
				{
					m_pVB->Unlock();
					m_pDevice->DrawPrimitive(D3DPT_POINTLIST,m_offsize,m_batchsize);
					grunTimeInfo.fTriangleNum += m_batchsize;
					grunTimeInfo.aTriangleNum+=m_batchsize;
					grunTimeInfo.aDrawCoreNum++;
					grunTimeInfo.fDrawCoreNum++;
					//当显卡在绘制填充好的batch的时候，cpu应该继续填充下一个batch
					m_offsize += m_batchsize;
					if (m_offsize >= m_vbsize)
						m_offsize =0;

					m_pVB->Lock(m_offsize*sizeof(tPointVertex),m_batchsize*sizeof(tPointVertex),
									(void**)&pVertex,
									  m_offsize?D3DLOCK_NOOVERWRITE:D3DLOCK_DISCARD);
					numIniBatch=0;
				}

			}
		}
		m_pVB->Unlock();

		//上面在绘制的时候条件是 batch被填满了，可是最后一个batch可能不会 刚好满足条件，所以在花一次
		if (numIniBatch)
		{
			m_pDevice->DrawPrimitive(D3DPT_POINTLIST,m_offsize,numIniBatch);
			grunTimeInfo.fTriangleNum += m_batchsize;
			grunTimeInfo.aTriangleNum+=m_batchsize;
			grunTimeInfo.aDrawCoreNum++;
			grunTimeInfo.fDrawCoreNum++;
			 
		}
		m_offsize += m_batchsize;
		

	
		AfterRender();
	}
}

bool ParticleBase::IsDead()
{
	//只有有一个处理live状态，就认为活着
	for (std::vector<tAtrribute>::iterator itr =m_Attribute.begin();itr != m_Attribute.end();itr++)
	{
		if (itr->bIsAlive)
		{
			return false;
		}
	}
	return true;
}

bool ParticleBase::IsEmpty()
{
	return m_Attribute.empty();
}

void ParticleBase::AfterRender()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING,true);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,false);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,false);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
}

void ParticleBase::BeforRender()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING,false);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,true);//如果顶点没有指定 点精灵大小，就必须设置成true，默认false
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,true);//如果true使用世界左边，近大远小；false则使用屏幕坐标
	m_pDevice->SetRenderState(D3DRS_POINTSIZE,D3D::F2Dw(m_PointSize));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN,D3D::F2Dw(0.0f));


	//这三个参数可以影响粒子显示的大小，有一个公式
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A,D3D::F2Dw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B,D3D::F2Dw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C,D3D::F2Dw((1.0f)));
// 
// 	设置纹理状态
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);   //将纹理颜色混合的第一个参数的颜色值用于输出
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );   //纹理颜色混合的第一个参数的值就取纹理颜色值
	m_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );  //缩小过滤状态采用线性纹理过滤
	m_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR ); //放大过滤状态采用线性纹理过滤


	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

// 	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );


}


//---------------------------------------------------------------------
SnowParticle::SnowParticle(IDirect3DDevice9* pdevice,long num):ParticleBase(pdevice)
{
	m_offsize=0;
	m_vbsize=2048;
	m_batchsize=512;

	m_PointSize=5.0f;

	for (int i=0;i<num;i++)
	{
		AddAttribute();
	}
}

void SnowParticle::ResetAttribute(tAtrribute& attr)
{
	attr.bIsAlive=true;
	
	attr.vPosition=D3D::GetRandomVec3(D3DXVECTOR3(-3000.0f,0.f,-1000.f),D3DXVECTOR3(3000.f,800.f,1000.f));

	attr.vVelocity.x=D3D::GetRandomFloat(0.0f,1.0f)* -6.0f;
	attr.vVelocity.y=D3D::GetRandomFloat(0.0f,1.0f)* -15.0f;
	attr.vVelocity.z=0;

	attr.color=D3D::WHITE;

}

void SnowParticle::UpdataParticle(float delta)
{
	std::vector<tAtrribute>::iterator itr=m_Attribute.begin();
	while (itr!=m_Attribute.end())
	{
		itr->vPosition += itr->vVelocity * delta * 0.01f;
		if (itr->vPosition.y < 0)
		{
			ResetAttribute(*itr);
		}
		++itr;
	}
}

SnowParticle::~SnowParticle()
{

}