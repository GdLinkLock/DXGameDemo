#pragma  once
#include "DXdefine.h"


struct RunTimeInfo
{
	//��֡����������
	unsigned int fTriangleNum;
	//�ܵ������� 
	unsigned int aTriangleNum;
	//��֡drawcore
	unsigned int fDrawCoreNum;
	//�ܵ�drawcore
	unsigned int aDrawCoreNum;

	unsigned int vbSize;
	unsigned int ibSize;

	void Reset()
	{
		fTriangleNum=0;
		fDrawCoreNum=0;
	}
};
