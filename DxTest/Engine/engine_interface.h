#pragma  once
#include "STLDefine.h"

namespace GDI
{
	class IResource
	{
	public:
		std::string GetRourceName();
	};

	class IGeometry:public IResource
	{

	};


	class IObject
	{

	};
	class IMoveObject:public IObject
	{

	};
	class IStaticModel:public IMoveObject
	{
	public:

	};

	class IRenderCore
	{
		IStaticModel* CreateStaticModel(const char* fn);
	};
}