#pragma  once
#include "GeometryProcessor.h"
#include "NvAseUtil.h"
#include "Head.h"
GeometryProcessBuffer pbuff;
GeometryProcessBuffer* GetProcessBuf()
{
	return &pbuff;
}

GeometryProcessBuffer::GeometryProcessBuffer()
{
	m_bufferSize = 0;
	m_buffer = 0;
}
GeometryProcessBuffer::~GeometryProcessBuffer()
{
	CleanUp();
}
void GeometryProcessBuffer::Init(uint32 bSize)
{
	assert(!m_buffer);

	m_bufferSize = bSize;
	m_buffer = new unsigned char[m_bufferSize];
}
void GeometryProcessBuffer::CleanUp()
{
	if(m_buffer)
	{
		delete[] m_buffer;
	}
	m_buffer = 0;
}
unsigned char* GeometryProcessBuffer::GetGeometryBuffer(unsigned int bSize)
{
	if(bSize > m_bufferSize)
	{
		CleanUp();
		Init(bSize);
	}

	return m_buffer;
}

//========================================================
int GeometryProcessor::ProcessResource(const std::string&  fn,GeometryReource* pRes)
{
	if(!pRes)
	{
		return 0;
	}

	// 先尝试读取lightmap网格
// 	if (LoadLightmapMesh(pRes, fileName))
// 	{
// 		return 1;
// 	}


	m_pModel = ase::load(fn.c_str(),1.f,1);

	if(!m_pModel)
	{
		return 0;
	}

	m_pGeometry = static_cast<GeometryReource*>(pRes);

	Parse();
	Fill();

	delete m_pModel;
	m_pModel = 0;

	return 0;
}
int GeometryProcessor::Parse()
{
	//组织ASE MODEL的数据。按照材质分好组。
	m_faces.clear();

	if(m_pModel)
	{
		for(size_t geoIndex = 0;geoIndex<m_pModel->geom.size();++geoIndex)
		{
			geomobj* obj = m_pModel->geom[geoIndex];
			if(!obj->strip)
			{
				ParseByTriangleList(obj,geoIndex);
			}
			else
			{
				ParseByStrip(obj,geoIndex);
			}
		}

		return 1;
	}

	return 0;
}
void GeometryProcessor::ParseByTriangleList(geomobj* obj,unsigned int geoIndex)
{
	uint32 submat = 0;
	std::vector<const char*>	shd_names;
	std::vector<uint32>	submats;
	std::vector<uint32>   shdidx;
	//find submat id, and the number of material in this ase object!
	for (uint32 i=0;i<obj->numf;++i)
	{
		submat = obj->fsubmat[i];

		size_t j;
		for (j=0;j<submats.size();++j)
			if (submats[j]==submat)
				break;

		if (j==submats.size())//a new submat id
		{
			submats.push_back(submat);
			const char* shadername = GetMeshShaderName(obj->matidx,submat,m_pModel->mat);
			if(!shadername)
			{
				continue;
			}
			size_t k;
			for (k = 0;k<shd_names.size();++k)
				if (shd_names[k]==shadername) break;

			if (k==shd_names.size())//a new shader name
				shd_names.push_back(shadername);

			shdidx.push_back(k);
		}
	}

	for (uint32 i=0;i<shd_names.size();++i)
	{
		//对shd_names做遍历
		tFaceByMat singleFace;

		singleFace.m_geomId = geoIndex;
		singleFace.m_shaderName = shd_names[i];

		for (uint32  j=0;j<obj->numf;++j)
		{
			size_t k;
			for (k=0;k<submats.size();++k)
				if (submats[k]==obj->fsubmat[j])
					break;
			if (k>=submats.size())
			{
				obj->fsubmat[j]=0;
				k=0;
			}

			if (singleFace.m_shaderName == shd_names[shdidx[k]])
			{
				submat = submats[k];
				singleFace.m_faces.push_back(j);
			}
		}

		singleFace.m_meshName = StripObjName(std::string(obj->name),bHasMultiSubMat(obj->matidx, submat, m_pModel->mat));
		singleFace.m_pGeoObj = obj;

		if (obj->anim || obj->parent)
		{
			singleFace.m_toLocal = true;
		}

		m_faces.push_back(singleFace);
	}
}
void GeometryProcessor::ParseByStrip(geomobj* obj,unsigned int geoIndex)
{
	//根据stripArray
	if(obj)
	{
		stripmat_array& stripArray = obj->strip->strip_mat;
		for(size_t j=0;j<stripArray.size();++j)
		{
			t_strip_mat* pAseMesh = stripArray[j];
			if (!pAseMesh->v || !pAseMesh->numVert)
			{
				continue;
			}
			tFaceByMat curFace; 
			curFace.m_geomId = geoIndex;
			curFace.m_meshName = StripObjName(std::string(obj->name),bHasMultiSubMat(obj->matidx,  pAseMesh->submat, m_pModel->mat));	
			const char* pShaderName = GetMeshShaderName(obj->matidx, pAseMesh->submat, m_pModel->mat);
			if(pShaderName)
			{
				curFace.m_shaderName = pShaderName;
			}
			curFace.m_pGeoObj = obj;
			curFace.m_pStripMat = pAseMesh;

			m_faces.push_back(curFace);
		}
	}
}
void GeometryProcessor::ProcessLod(GeometryReource* pGeoComposor,t_strip_mat* pStrip,tMeshDesc& meshDesc)
{
	if(!pStrip)
	{
		//非strip的LOD
		tLodInfo lodInfo;
		lodInfo.m_numStrip = 1;
		lodInfo.m_stripIndex = 0;
		lodInfo.m_numPoly = meshDesc.m_vertCount/3;
		lodInfo.m_subLodInfo[0].m_bValid = 1;
		lodInfo.m_subLodInfo[0].m_numElem = meshDesc.m_vertCount;
		lodInfo.m_subLodInfo[0].m_numVisable = meshDesc.m_vertCount;
		lodInfo.m_subLodInfo[0].m_offset = 0;
		lodInfo.m_subLodInfo[0].m_subType = (unsigned int)PT_TRIANGLELIST;

		//pGeoComposor->SetLodInfo(lodInfo,meshDesc);
		return;
	}

	uint32 m_num_totalelem=0;
	if (pStrip->lods.size())
	{
		std::vector<uint32> stri;
		std::vector<uint32> tri;
		std::vector<uint32*> elem_arr;
		std::vector<uint32> elem_size_arr;
		std::vector<uint32*> elem_del;

		for (size_t nStripArr=0;nStripArr<pStrip->lods.size()+1;++nStripArr)
		{
			stri.clear();
			tri.clear();

			strip_array* strips = &pStrip->strips;

			if (nStripArr)
				strips = pStrip->lods[nStripArr-1];

			uint32 nPolyNum=0;
			uint32 nOrgPolyNum = 0;
			if (!CombineStrips(*strips, stri, tri, nPolyNum, nOrgPolyNum))
				continue;

			if (!nStripArr)
			{
				meshDesc.m_triCount = nPolyNum;
			//	pGeoComposor->SetMeshDesc(meshDesc);
			}

			tLodInfo lodInfo;
			lodInfo.m_numStrip = (stri.size()>0? 1:0) +(tri.size()>0? 1:0);

			int n=0;
			if (stri.size())
			{
				lodInfo.m_subLodInfo[n].m_numElem = stri.size();
				lodInfo.m_subLodInfo[n].m_numVisable = stri.size();
				lodInfo.m_subLodInfo[n].m_offset = m_num_totalelem*sizeof(uint32);
				lodInfo.m_subLodInfo[n].m_bValid = 1;
				lodInfo.m_subLodInfo[n].m_subType = (unsigned int)PT_TRIANGLESTRIP;

				++n;

				m_num_totalelem += stri.size();
			}

			if (tri.size())
			{
				lodInfo.m_subLodInfo[n].m_numElem = tri.size();
				lodInfo.m_subLodInfo[n].m_numVisable = tri.size();
				lodInfo.m_subLodInfo[n].m_offset = m_num_totalelem*sizeof(uint32);
				lodInfo.m_subLodInfo[n].m_bValid = 1;
				lodInfo.m_subLodInfo[n].m_subType = (unsigned int)PT_TRIANGLELIST;

				m_num_totalelem += tri.size();
			}

			lodInfo.m_numPoly = m_num_totalelem / 3;
			lodInfo.m_stripIndex = nStripArr;

			//pGeoComposor->SetLodInfo(lodInfo,meshDesc);

			uint32* elem = new uint32 [stri.size()+tri.size()];
			elem_del.push_back(elem);
			uint32* oelem = elem;
			for (size_t j=0;j<stri.size();++j,++elem)
			{
				*elem = stri[j];
			}

			if (stri.size())
			{
				elem_arr.push_back(oelem);
				elem_size_arr.push_back(stri.size());
			}

			oelem = elem;
			for (size_t j=0;j<tri.size();++j,++elem)
			{
				*elem = tri[j];
			}
			if (tri.size())
			{
				elem_arr.push_back(oelem);
				elem_size_arr.push_back(tri.size());
			}

		}//all strips include lod

		//生成索引BUFFER

		uint32* pIndexBuffer =(uint32*)new char[m_num_totalelem*sizeof(uint32)] /*GetuintBuffer(m_num_totalelem)*/;
		//pIndexData->m_nIndexCount = m_num_totalelem;
		//pIndexData->m_nInitIndexCount = m_num_totalelem;
		for (size_t nElemSize=0; nElemSize<elem_size_arr.size();++nElemSize)
		{
			memcpy(pIndexBuffer, elem_arr[nElemSize], elem_size_arr[nElemSize]*sizeof(uint32));
			pIndexBuffer += elem_size_arr[nElemSize];//pDst32 += elem_size_arr[nElemSize]*sizeof(uint32); modify by liren 2010-01-06
		//   DXWriteConsol(CC_WHITE,"index=%d",*(pIndexBuffer));
		}

		//pGeoComposor->SetIndexComponentData(pIndexBuffer,m_num_totalelem,m_num_totalelem,0,0,0,meshDesc);
		pGeoComposor->GetMeshByName(meshDesc.m_meshName)->SetIndexData(pIndexBuffer,m_num_totalelem);
		for (uint32 i=0; i<elem_del.size();++i)
		{
			delete []elem_del[i];
		}
	}
	else
	{
		std::vector<ase::t_strip*> ase_triangles;
		//mesh->m_num_totalelem=0;
		//unsigned int totalElement = 0;
		std::vector<uint32> stri;
		std::vector<uint32> tri;
		unsigned int nNumLodElem = 0;
		for (size_t j=0;j<pStrip->lodlist.size();++j)
		{
			lodobj* lobj = pStrip->lodlist[j];
			nNumLodElem+=lobj->striplength + lobj->indicescount;
		}

		//combine all triangles to ONE strip
		uint32 nPolyNum=0;
		uint32 nOrgPolyNum = 0;
		CombineStrips(pStrip->strips, stri, tri, nPolyNum, nOrgPolyNum);

		meshDesc.m_triCount = nPolyNum;
		pGeoComposor->SetMeshDesc(meshDesc);

		int nstrip=0;
		if (stri.size())
			nstrip++;
		if (tri.size())
			nstrip++;

		m_num_totalelem = stri.size() + tri.size();
		m_num_totalelem +=nNumLodElem;

		unsigned int stripIndex = 0;
		if (nstrip)
		{
			tLodInfo lodInfo;
			lodInfo.m_numPoly = nPolyNum;
			lodInfo.m_numStrip = nstrip;
			lodInfo.m_stripIndex = stripIndex;

			uint32 elemoffset=0;
			uint32 n = 0;
			if (stri.size())
			{
				lodInfo.m_subLodInfo[0].m_numElem = (uint32)(stri.size());
				lodInfo.m_subLodInfo[0].m_numVisable = lodInfo.m_subLodInfo[0].m_numElem;
				//lodInfo.m_subLodInfo[0].smooth=0;
				lodInfo.m_subLodInfo[0].m_subType=PT_TRIANGLESTRIP;
				lodInfo.m_subLodInfo[0].m_bValid = 1;
				lodInfo.m_subLodInfo[0].m_offset = elemoffset;

				++n;

				elemoffset+=static_cast<uint32>(stri.size())*sizeof(uint32);
			}
			if (tri.size())
			{
				lodInfo.m_subLodInfo[n].m_numElem = (uint32)tri.size();
				lodInfo.m_subLodInfo[n].m_numVisable = lodInfo.m_subLodInfo[n].m_numElem;
				//lodInfo.m_subLodInfo[n].smooth=0;
				lodInfo.m_subLodInfo[n].m_subType=PT_TRIANGLELIST;
				lodInfo.m_subLodInfo[n].m_bValid = 1;
				lodInfo.m_subLodInfo[n].m_offset = elemoffset;

				elemoffset+=static_cast<uint32>(tri.size())*sizeof(uint32);
			}

			//pGeoComposor->SetLodInfo(lodInfo,meshDesc);

			//生成索引数据
			uint32* pIndexBuffer =  (uint32*)new char[m_num_totalelem*sizeof(uint32)];/*GetuintBuffer(m_num_totalelem)*/;
			for (size_t j=0; j<stri.size(); ++j)
			{
				pIndexBuffer[j] = stri[j]; 
			}

			for (size_t j=0; j<tri.size(); ++j)
			{
				pIndexBuffer[j] = tri[j];
			}

			uint32* pElem =  pIndexBuffer + elemoffset/sizeof(uint32);
			for (size_t j=0; j<pStrip->lodlist.size(); ++j)
			{
				lodobj* lobj = pStrip->lodlist[j];
				uint32 nNumStrip = uint32(lobj->striplength!=0) + uint32(lobj->indicescount!=0);

				++stripIndex;

				tLodInfo lodInfo;
				lodInfo.m_numStrip = nNumStrip;
				lodInfo.m_stripIndex = stripIndex;

				int n=0;
				if (lobj->striplength)
				{
					lodInfo.m_subLodInfo[n].m_offset = elemoffset;
					//lodInfo.m_subLodInfo[n].smooth=0;
					lodInfo.m_subLodInfo[n].m_subType=PT_TRIANGLESTRIP;
					lodInfo.m_subLodInfo[n].m_numElem  = lobj->striplength;
					lodInfo.m_subLodInfo[n].m_numVisable = lobj->striplength;
					lodInfo.m_subLodInfo[n].m_bValid = 1;
					++n;

					for (uint16 nn = 0;nn<(uint16)lobj->striplength;++nn,++pElem)
						*pElem = (uint32)lobj->pStrip[nn];

					elemoffset+=lobj->striplength*sizeof(uint32);
					lodInfo.m_numPoly = lobj->striplength-2;
				}
				if (lobj->indicescount)
				{
					lodInfo.m_subLodInfo[n].m_offset = elemoffset;
					//lodInfo.m_subLodInfo[n].smooth=0;
					lodInfo.m_subLodInfo[n].m_subType=PT_TRIANGLELIST;
					lodInfo.m_subLodInfo[n].m_numElem  = lobj->indicescount;
					lodInfo.m_subLodInfo[n].m_numVisable = lobj->indicescount;
					lodInfo.m_subLodInfo[n].m_bValid = 1;

					for (uint16 nn=0;nn<(uint16)lobj->indicescount;++nn,++pElem)
						*pElem = (uint32)lobj->pIndices[nn];

					elemoffset+=lobj->indicescount*sizeof(uint32);

					lodInfo.m_numPoly += lobj->indicescount/3;
				}

				//pGeoComposor->SetLodInfo(lodInfo,meshDesc);
			}
			for (int i=0;i<m_num_totalelem;i++)
			{
				//DXWriteConsol(CC_WHITE,"index=%d",*(pIndexBuffer+i));
			}
			pGeoComposor->GetMeshByName(meshDesc.m_meshName)->SetIndexData(pIndexBuffer,m_num_totalelem);
			delete [] pIndexBuffer;
		//	pGeoComposor->SetIndexComponentData(pIndexBuffer,m_num_totalelem,m_num_totalelem,0,0,0,meshDesc);
		}
		else
		{
			//ERROR
			return;
		}
	}
}
int GeometryProcessor::Fill()
{
	if(m_pModel&&m_pGeometry&&!m_faces.empty())
	{
		//从MODEL中取出数据,设置给GEOMETRY
		//faces中全部都是按照材质分好组的三角形.

		//设置MeshDesc
		for(size_t i =0;i<m_faces.size();++i)
		{
			tFaceByMat& curFaces = m_faces.at(i);
			//每个mesh必须有材质名和mesh名
			if(!curFaces.m_meshName.empty()&&!curFaces.m_shaderName.empty())
			{
				tMeshDesc meshDesc;
				meshDesc.m_geoObjectId = curFaces.m_geomId;
				std::string objName = StripObjName(std::string(curFaces.m_pGeoObj->name),0);
				strcpy(meshDesc.m_geoObjName,objName.c_str());
				strcpy(meshDesc.m_meshName,curFaces.m_meshName.c_str());
				strcpy(meshDesc.m_matName,curFaces.m_shaderName.c_str());

				if(!curFaces.m_pStripMat)
				{
					meshDesc.m_vertCount = curFaces.m_faces.size()*3;
					meshDesc.m_triCount = curFaces.m_faces.size();

					m_pGeometry->CreateMesh(meshDesc);

					FillByTriangleList(m_pGeometry,curFaces,meshDesc);

				}
				else
				{
					meshDesc.m_vertCount = curFaces.m_pStripMat->numVert;
					//LOD的第0层
					meshDesc.m_triCount = 0;

					m_pGeometry->CreateMesh(meshDesc);
					FillByStrip(m_pGeometry,curFaces,meshDesc);

					ProcessLod(m_pGeometry,curFaces.m_pStripMat,meshDesc);
				}
			}
			else
			{
				//存在mesh没有自己的名字或者没有指定材质

				continue;
			}
		}

		//处理级联关系
		GdMat4 geomTM;

		for(size_t geoIndex = 0;geoIndex<m_pModel->geom.size();++geoIndex)
		{
			geomobj* obj = m_pModel->geom[geoIndex];

			if(obj->parent||obj->anim)
			{
				if (obj->parent)
				{
					GdMat4 parentInv;
					parentInv.FromFloatsColumnMajor(obj->parent->tm);
					parentInv.InverseSelf();
					geomTM.FromFloatsColumnMajor(obj->tm);
					geomTM = parentInv*geomTM;
				}
				else
				{
					geomTM.FromFloatsColumnMajor(obj->tm);
				}
			}
			else
			{
				geomTM.Identity();
			}

			GdVec3 scale,pos;
			GdQuat quat;
			idMath::Init();
			geomTM.Decompose(scale,quat,pos);

			m_pGeometry->SetChildPosInfo(pos.ToFloatPtr());
			m_pGeometry->SetChildRotInfo(quat.ToFloatPtr());
			m_pGeometry->SetChildScaleInfo(scale.ToFloatPtr());

		}

		//setup parent id
		for(size_t geoIndex = 0;geoIndex<m_pModel->geom.size();++geoIndex)
		{
			geomobj* obj = m_pModel->geom[geoIndex];

			if (!obj->parent) 
				continue;
			//if (vGeomProperty[i]!=0)//only strip geom can have parent??
			//	continue;
			size_t j;
			for (j=0;j<m_pModel->geom.size();++j)
			{
				if (obj->parent == m_pModel->geom[j])
					break;
			}

			if (j==m_pModel->geom.size())
				continue;

			//m_pGeometry->ChangeGeometryParentId(geoIndex,j);
		}

		return 1;
	}

	return 0;
}
void GeometryProcessor::FillByTriangleList(GeometryReource* pGeoComposor,tFaceByMat& curFaces,tMeshDesc& meshDesc)
{
	if(!pGeoComposor)
	{
		return;
	}

	//index
	uint32 bufferSize = meshDesc.m_vertCount;

	uint32* pBuffer = GetuintBuffer(bufferSize);
	for(uint32 i=0;i<bufferSize;++i)
	{
		pBuffer[i] = i;
	}
	//设置ib

	//设置VB,IB,NORMAL,TAN,UV,COLOR等
	//开始组织数据.
	//pos
	float* posBuffer = /*GetfBuffer(bufferSize)*/(float*)(new unsigned char[bufferSize*sizeof(float)]);
	if(meshDesc.m_vertCount)
	{
		unsigned int bufferSize = meshDesc.m_vertCount*3;
		GetPosBuffer(&posBuffer,bufferSize,curFaces);
	//	pGeoComposor->SetVertexComponentData((unsigned char*)posBuffer,bufferSize*sizeof(float),VCS_Position,VCT_FLOAT3,meshDesc);
	}
// 	nor
// 		float* norBuffer = /*GetfBuffer(bufferSize)*/(float*)new unsigned char[bufferSize*sizeof(float)];
// 		if(curFaces.m_pGeoObj->numn)
// 		{
// 			unsigned int bufferSize = meshDesc.m_vertCount*3;
// 			GetNorBuffer(&norBuffer,bufferSize,curFaces);
// 			//pGeoComposor->SetVertexComponentData((unsigned char*)norBuffer,bufferSize*sizeof(float),VCS_Normal,VCT_FLOAT3,meshDesc);
// 			for (int i=0;i<meshDesc.m_vertCount;i++)
// 			{
// 				DXWriteConsol(CC_RED,"line=%d,%f,%f,%f",i,*(norBuffer+i*3),*(norBuffer+i*3+1),*(norBuffer+i*3+2));
// 			}
// 		}
// 		
// 		
// 		//UV
// 		float* uvBuffer =/* GetfBuffer(bufferSize)*/(float*)new unsigned char[bufferSize*sizeof(float)];
// 		if (curFaces.m_pGeoObj->numtf && curFaces.m_pGeoObj->numtf==curFaces.m_pGeoObj->numf)
// 		{
// 			unsigned int bufferSize = meshDesc.m_vertCount*2;
// 			if (curFaces.m_pGeoObj->t)
// 			{
// 				GetUVBuffer(uvBuffer,curFaces, curFaces.m_pGeoObj->t, curFaces.m_pGeoObj->tf);
// 			//	pGeoComposor->SetVertexComponentData((unsigned char*)uvBuffer,bufferSize*sizeof(float),VCS_Texcoordinate0,VCT_FLOAT2,meshDesc);
// 				for (int i=0;i<meshDesc.m_vertCount;i++)
// 				{
// 					DXWriteConsol(CC_BLUE,"line=%d,%f,%f",i,*(uvBuffer+i*2),*(uvBuffer+i*2+1));
// 				}
// 			}
// 		}
	tVertex buffer[72];
	for (int i=0;i<meshDesc.m_vertCount;i++)
	{
		buffer[i]=tVertex(*(posBuffer+i*3),*(posBuffer+i*3+1),*(posBuffer+i*3+2)/*,*(uvBuffer+i*2),*(uvBuffer+i*2+1),*(norBuffer+i*3),*(norBuffer+i*3+1),*(norBuffer+i*3+2)*/);
		//DXWriteConsol(CC_GREEN,"line=%d,%f,%f,%f,%f,%f,%f,%f,%f",i,*(vv+i*3),*(vv+i*3+1),*(vv+i*3+2),*(tt+i*3),*(tt+i*3+1),*(nn+i*3),*(nn+i*3+1),*(nn+i*3+2));
	}
	char* buf=new char[meshDesc.m_vertCount*sizeof(tVertex)];
	m_pGeometry->GetMeshList().at(0)->SetData(buf,meshDesc.m_vertCount,"");
	delete  [] posBuffer;

}
void GeometryProcessor::FillByStrip(GeometryReource* pGeoComposor,tFaceByMat& curFace,tMeshDesc& meshDesc)
{
	if(!curFace.m_pStripMat)
	{
		return;
	}

	meshDesc.m_vertCount = curFace.m_pStripMat->numVert;

	GdMat4 tmpInv;
	tmpInv.FromFloatsColumnMajor(curFace.m_pGeoObj->tm);
	tmpInv.InverseSelf();

	GdMat4 tmpTransMat;
	tmpTransMat.FromFloatsColumnMajor(curFace.m_pGeoObj->tm);
	tmpTransMat.TransposeSelf();

	tVertex* buffer=new tVertex[meshDesc.m_vertCount];
	

	if(meshDesc.m_vertCount&&curFace.m_pStripMat->v)
	{
		unsigned int bufferSize = meshDesc.m_vertCount*3;
		float* pBuffer =(float*)new char[bufferSize*sizeof(float)]/* GetfBuffer(bufferSize)*/;
		memcpy(pBuffer,curFace.m_pStripMat->v,bufferSize*sizeof(float));

		if(curFace.m_toLocal)
		{
			GdVec3 curPos;
			for(size_t i = 0;i<meshDesc.m_vertCount;++i)
			{
				curPos.x = pBuffer[i*3];
				curPos.y = pBuffer[i*3+1];
				curPos.z = pBuffer[i*3+2];

				curPos = tmpInv*curPos;

				pBuffer[i*3] = curPos.x;
				pBuffer[i*3+1] = curPos.y;
				pBuffer[i*3+2] = curPos.z;
			}
		}
		for (int i=0;i<meshDesc.m_vertCount;i++)
		{
			buffer[i].x=pBuffer[i*3];
			buffer[i].y=pBuffer[i*3+1];
			buffer[i].z=pBuffer[i*3+2];
			//DXWriteConsol(CC_GREEN,"line=%d,%f,%f,%f",i,buffer[i].x,buffer[i].y,buffer[i].z);
		}
		delete [] pBuffer;
		//pGeoComposor->SetVertexComponentData((unsigned char*)pBuffer,bufferSize*sizeof(float),VCS_Position,VCT_FLOAT3,meshDesc);
	}

	if(curFace.m_pStripMat->n)
	{
		unsigned int bufferSize = meshDesc.m_vertCount*3;
	    float* pBuffer =(float*)new char[bufferSize*sizeof(float)];
		memcpy(pBuffer,curFace.m_pStripMat->n,bufferSize*sizeof(float));

		if(curFace.m_toLocal)
		{
			GdVec3 curNormal;
			for(size_t i = 0;i<meshDesc.m_vertCount;++i)
			{
				curNormal.x = pBuffer[i*3];
				curNormal.y = pBuffer[i*3+1];
				curNormal.z = pBuffer[i*3+2];

				curNormal = tmpTransMat.Rotate(curNormal);

				pBuffer[i*3] = curNormal.x;
				pBuffer[i*3+1] = curNormal.y;
				pBuffer[i*3+2] = curNormal.z;
			}
		}
		for (int i=0;i<meshDesc.m_vertCount;i++)
		{
			buffer[i].n1=pBuffer[i*3];
			buffer[i].n2=pBuffer[i*3+1];
			buffer[i].n3=pBuffer[i*3+2];
			//DXWriteConsol(CC_RED,"normal=%d,%f,%f,%f",i,buffer[i].n1,buffer[i].n2,buffer[i].n3);
		}
		delete [] pBuffer;
	//	pGeoComposor->SetVertexComponentData((unsigned char*)pBuffer,bufferSize*sizeof(float),VCS_Normal,VCT_FLOAT3,meshDesc);
	}

// 	if(curFace.m_pStripMat->Tn)
// 	{
// 		unsigned int bufferSize = meshDesc.m_vertCount*4;
// 		float* pBuffer = GetfBuffer(bufferSize);
// 		memcpy(pBuffer,curFace.m_pStripMat->Tn,bufferSize*sizeof(float));
// 
// 		if(curFace.m_toLocal)
// 		{
// 			GdVec3 curTan;
// 			for(size_t i = 0;i<meshDesc.m_vertCount;++i)
// 			{
// 				curTan.x = pBuffer[i*3];
// 				curTan.y = pBuffer[i*3+1];
// 				curTan.z = pBuffer[i*3+2];
// 
// 				curTan = tmpTransMat.Rotate(curTan);
// 
// 				pBuffer[i*3] = curTan.x;
// 				pBuffer[i*3+1] = curTan.y;
// 				pBuffer[i*3+2] = curTan.z;
// 				//第4个分量不变，手相性
// 			}
// 		}
// 
// 		//pGeoComposor->SetVertexComponentData((unsigned char*)pBuffer,bufferSize*sizeof(float),VCS_Tangent,VCT_FLOAT4,meshDesc);
// 	}
// 
// 	if(curFace.m_pStripMat->c)
// 	{
// 		unsigned int bufferSize = meshDesc.m_vertCount*4;
// 	//	pGeoComposor->SetVertexComponentData((unsigned char*)curFace.m_pStripMat->c,bufferSize*sizeof(float),VCS_DiffuseColorDX9,VCT_FLOAT4,meshDesc);
// 	}

	//多层UV
	if(curFace.m_pStripMat->t)
	{
		unsigned int uvSize = meshDesc.m_vertCount*2;
		float* pBuffer =(float*)new char[uvSize*sizeof(float)];
		memcpy(pBuffer,curFace.m_pStripMat->t,sizeof(float)*uvSize);

		if(curFace.m_bFlipV)
		{
			float v;
			for(size_t i = 0;i<meshDesc.m_vertCount;++i)
			{
				v = pBuffer[i*2+1];
				v = 1.f-v;
				pBuffer[i*2+1] = v;
			}
		}
		for (int i=0;i<meshDesc.m_vertCount;i++)
		{
			buffer[i].u=pBuffer[i*2];
			buffer[i].v=pBuffer[i*2+1];
		//	DXWriteConsol(CC_RED,"normal=%d,%f,%f",i,buffer[i].u,buffer[i].v);
		}
		delete [] pBuffer;
		//pGeoComposor->SetVertexComponentData((unsigned char*)pBuffer,uvSize*sizeof(float),VCS_Texcoordinate0,VCT_FLOAT2,meshDesc);
	}
	if(curFace.m_pStripMat->t2)
	{
		unsigned int uvSize = meshDesc.m_vertCount*2;
		float* pBuffer = GetfBuffer(uvSize);
		memcpy(pBuffer,curFace.m_pStripMat->t2,sizeof(float)*uvSize);

		if(curFace.m_bFlipV)
		{
			float v;
			for(size_t i = 0;i<meshDesc.m_vertCount;++i)
			{
				v = pBuffer[i*2+1];
				v = 1.f-v;
				pBuffer[i*2+1] = v;
			}
		}

	//	pGeoComposor->SetVertexComponentData((unsigned char*)pBuffer,uvSize*sizeof(float),VCS_Texcoordinate1,VCT_FLOAT2,meshDesc);
	}
	for (int i=0;i<meshDesc.m_vertCount;i++)
	{
	//	DXWriteConsol(CC_GREEN,"line=%d,%f,%f,%f,%f,%f,%f,%f,%f",i,buffer[i].x,buffer[i].y,buffer[i].z,buffer[i].n1,buffer[i].n2,buffer[i].n3,buffer[i].u,buffer[i].v);
	}
	pGeoComposor->GetMeshByName(meshDesc.m_meshName)->SetVertexData(buffer,meshDesc.m_vertCount);
	delete [] buffer;
}

void GeometryProcessor::GetPosBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace)
{
	if(ppBuffer&&bufferSize&&curFace.m_pGeoObj)
	{
		//填充ppBuffer
		GdMat4 objMat;
		objMat.FromFloatsColumnMajor(curFace.m_pGeoObj->tm);
		objMat.InverseSelf();

		float* pBuffer = (*ppBuffer);

		GdVec3 curPos;
		for(size_t i = 0;i<curFace.m_faces.size();++i)
		{
			unsigned int posIndex = curFace.m_faces.at(i)*3;

			for(int k=0;k<3;++k)
			{
				curPos.x = curFace.m_pGeoObj->v[curFace.m_pGeoObj->f[posIndex+k]*3];
				curPos.y = curFace.m_pGeoObj->v[curFace.m_pGeoObj->f[posIndex+k]*3+1];
				curPos.z = curFace.m_pGeoObj->v[curFace.m_pGeoObj->f[posIndex+k]*3+2];

				if(curFace.m_toLocal)
				{
					curPos = objMat*curPos;
				}
				DXWriteConsol(CC_GREEN,"line=%d,%f,%f,%f",i,curPos.x,curPos.y,curPos.z);
				(*pBuffer++) = curPos.x;
				(*pBuffer++) = curPos.y;
				(*pBuffer++) = curPos.z;
			}
		}
	}
}

void GeometryProcessor::GetNorBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace)
{
	if(ppBuffer&&bufferSize&&curFace.m_pGeoObj)
	{
		GdMat4 objTransMat;
		objTransMat.FromFloatsColumnMajor(curFace.m_pGeoObj->tm);
		objTransMat.TransposeSelf();

		//填充ppBuffer
		float* pBuffer = (*ppBuffer);

		GdVec3 curNormal;
		for(size_t i = 0;i<curFace.m_faces.size();++i)
		{
			unsigned int posIndex = curFace.m_faces.at(i)*3;

			for(int k=0;k<3;++k)
			{
				curNormal.x = curFace.m_pGeoObj->n[curFace.m_pGeoObj->fvn[posIndex+k]*3];
				curNormal.y = curFace.m_pGeoObj->n[curFace.m_pGeoObj->fvn[posIndex+k]*3+1];
				curNormal.z = curFace.m_pGeoObj->n[curFace.m_pGeoObj->fvn[posIndex+k]*3+2];

				if(curFace.m_toLocal)
				{
					curNormal = objTransMat.Rotate(curNormal);
				}

				(*pBuffer++) = curNormal.x;
				(*pBuffer++) = curNormal.y;
				(*pBuffer++) = curNormal.z;
			}
		}
	}
}

void GeometryProcessor::GetTanBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace)
{
	if(ppBuffer&&bufferSize&&curFace.m_pGeoObj)
	{
		idMat4 objTransMat;
		objTransMat.FromFloatsColumnMajor(curFace.m_pGeoObj->tm);
		objTransMat.TransposeSelf();

		//填充ppBuffer
		float* pBuffer = (*ppBuffer);

		GdVec3 curTan;
		for(size_t i = 0;i<curFace.m_faces.size();++i)
		{
			unsigned int posIndex = curFace.m_faces.at(i)*3;

			for(int k=0;k<3;++k)
			{
				curTan.x = curFace.m_pGeoObj->Tn[curFace.m_pGeoObj->fvTn[posIndex+k]*4];
				curTan.y = curFace.m_pGeoObj->Tn[curFace.m_pGeoObj->fvTn[posIndex+k]*4+1];
				curTan.z = curFace.m_pGeoObj->Tn[curFace.m_pGeoObj->fvTn[posIndex+k]*4+2];

				if(curFace.m_toLocal)
				{
					curTan = objTransMat.Rotate(curTan);
				}

				(*pBuffer++) = curTan.x;
				(*pBuffer++) = curTan.y;
				(*pBuffer++) = curTan.z;
				(*pBuffer++) = curFace.m_pGeoObj->Tn[curFace.m_pGeoObj->fvTn[posIndex+k]*4+3];
			}
		}
	}
}

void GeometryProcessor::GetUVBuffer(float* pBuffer,tFaceByMat& curFace, const float* t, const unsigned int* tf)
{
	if(pBuffer&&curFace.m_pGeoObj)
	{
		for(size_t i = 0;i<curFace.m_faces.size();++i)
		{
			unsigned int posIndex = curFace.m_faces.at(i)*3;

			for(int k=0;k<3;++k)
			{
				(*pBuffer++) = t[tf[posIndex+k]*3];

				if(curFace.m_bFlipV)
				{
					(*pBuffer++) = 1.f - t[tf[posIndex+k]*3+1];
				}
				else
				{
					(*pBuffer++) = t[tf[posIndex+k]*3+1];
				}

			}
		}
	}
}

void GeometryProcessor::GetColorBuffer(float** ppBuffer,unsigned int bufferSize,tFaceByMat& curFace)
{
	if(ppBuffer&&bufferSize&&curFace.m_pGeoObj)
	{
		//填充ppBuffer
		float* pBuffer = (*ppBuffer);
		for(size_t i = 0;i<curFace.m_faces.size();++i)
		{
			unsigned int posIndex = curFace.m_faces.at(i)*3;

			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex]*3];
			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex]*3+1];
			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex]*3+2];
			(*pBuffer++) = 1.0f;

			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex+1]*3];
			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex+1]*3+1];
			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex+1]*3+2];
			(*pBuffer++) = 1.0f;

			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex+2]*3];
			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex+2]*3+1];
			(*pBuffer++) = curFace.m_pGeoObj->c[curFace.m_pGeoObj->cf[posIndex+2]*3+2];
			(*pBuffer++) = 1.0f;
		}
	}
}

uint32* GeometryProcessor::GetuintBuffer(uint32 needSize)
{
	
	//return m_uIntBuffer;
	unsigned char* pBuffer =GetProcessBuf()->GetGeometryBuffer(needSize*sizeof(uint32));
	if(!pBuffer)
	{
		return 0;
	}
	return (uint32*)pBuffer;
}

float* GeometryProcessor::GetfBuffer(uint32 needSize)
{
	
	unsigned char* pBuffer =GetProcessBuf()->GetGeometryBuffer(needSize*sizeof(float));
	if(!pBuffer)
	{
		
		return 0;
	}
	return (float*)pBuffer;
}
