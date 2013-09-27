#include "STLDefine.h"
#include "engine_interface.h"
#include "SceneManager.h"

class IRenderImpl:public GDI::IRenderCore
{
public:
	GDI::IStaticModel* CreateStaticModel(const char* fn);
private:

};