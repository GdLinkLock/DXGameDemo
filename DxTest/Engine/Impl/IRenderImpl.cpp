#include "IRenderImpl.h"
#include "Model.h"
#include "IStaticModelImpl.h"

GDI::IStaticModel* IRenderImpl::CreateStaticModel(const char* fn)
{
	Model* model=SceneManager::Instance()->LoadModelFromFile(fn);
	GDI::IStaticModel* imodel=new IStaticModelImpl(model);//new attention memory
	return imodel;
}
