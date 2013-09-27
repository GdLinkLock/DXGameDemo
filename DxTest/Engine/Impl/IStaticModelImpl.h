#include "STLDefine.h"
#include "engine_interface.h"
#include "Model.h"
class IStaticModelImpl:public GDI::IStaticModel
{
public:
	IStaticModelImpl(Model* model){};
	~IStaticModelImpl(){};
};