#pragma  once
#include "DXdefine.h"


struct RunTimeInfo
{
	//本帧三角形数量
	unsigned int fTriangleNum;
	//总的三角形 
	unsigned int aTriangleNum;
	//本帧drawcore
	unsigned int fDrawCoreNum;
	//总的drawcore
	unsigned int aDrawCoreNum;

	unsigned int vbSize;
	unsigned int ibSize;

	void Reset()
	{
		fTriangleNum=0;
		fDrawCoreNum=0;
	}
};
