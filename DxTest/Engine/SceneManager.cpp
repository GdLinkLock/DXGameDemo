#pragma  once
#include "SceneManager.h"
#include "Mesh.h"
#include "NV_ASE.H"
#include "GeometryResource.h"
#include "GeometryProcessor.h"
#include "../DxGlobal/DxGlobal.h"
using namespace ase;
SceneManager* SceneManager::scenemng=0;

	bool SceneManager::Init()
	{
		return false;
	}
	Model* SceneManager::LoadModelFromFile(const char* fn)
	{
 			GeometryProcessor* processor=new GeometryProcessor;
 			GeometryReource* pRes=new GeometryReource;
 			processor->ProcessResource(fn,pRes);
 			Model* p=new Model(fn, pRes->GetMeshList().at(0));
 			 m_ModelList.push_back(p);
 			 return p;
		char buf[128];
		::GetCurrentDirectory(128,buf);
		DXWriteConsol(CC_GREEN,"dir=%s",buf);
		ase::model*  pmodel= ase::load(fn,1.f,1);
		if (!pmodel)
		{
			return 0;
		}
		Mesh* mesh=new Mesh(0,pmodel->name);
	
		//parse
		for(size_t geoIndex = 0;geoIndex<pmodel->geom.size();++geoIndex)
		{
			geomobj* obj = pmodel->geom[geoIndex];
			if (!obj->strip)
			{
				//triangle
					DXWriteConsol(CC_RED,"pmodel->geom.size()=%d",pmodel->geom.size());
				//	processor->ParseByTriangleList(obj,geoIndex);
			}
			else 
			{//strip
				if(obj)
				{
					stripmat_array& stripArray = obj->strip->strip_mat;
					DXWriteConsol(CC_RED,"stripArray.size()=%d",stripArray.size());
					for(size_t j=0;j<stripArray.size();++j)
					{
						t_strip_mat* pAseMesh = stripArray[j];
						DXWriteConsol(CC_RED,"verCount=%d",pAseMesh->numVert);
						float* vv=pAseMesh->v;
						float* tt=pAseMesh->t;
						float* nn=pAseMesh->n;
						tVertex* buffer=new tVertex[pAseMesh->numVert];
						for (int i=0;i<pAseMesh->numVert;i++)
						{
							buffer[i]=tVertex(*(vv+i*3),*(vv+i*3+1),*(vv+i*3+2),*(tt+i*2),*(tt+i*2+1),*(nn+i*3),*(nn+i*3+1),*(nn+i*3+2));
							DXWriteConsol(CC_GREEN,"line=%d,%f,%f,%f,%f,%f,%f,%f,%f",i,*(vv+i*3),*(vv+i*3+1),*(vv+i*3+2),*(tt+i*3),*(tt+i*3+1),*(nn+i*3),*(nn+i*3+1),*(nn+i*3+2));
						}
						
						if (!pAseMesh->v || !pAseMesh->numVert)
						{
							continue;
						}
						char* pbu=new char[sizeof(tVertex)*pAseMesh->numVert];
						memcpy(pbu,buffer,sizeof(tVertex)*pAseMesh->numVert);
						mesh->SetData(pbu,pAseMesh->numVert,"G:\\code\\DXGameDemo\\bin\\topsnow1.jpg");
						Model* p=new Model(pmodel->name,mesh);
					   m_ModelList.push_back(p);
					   delete [] pbu;
						return p;
					}
				}
			}
		}
// 		//fill
	
	}

	SceneManager* SceneManager::Instance()
	{
		if (scenemng==0)
		{
			scenemng=new SceneManager;
		}
		return scenemng;
	}
	void SceneManager::ReleaseMng()
	{
		if (scenemng)
		{
			delete scenemng;
			scenemng=0;
		}
	}
	SceneManager::~SceneManager()
	{
		ReleaseMng();
	}