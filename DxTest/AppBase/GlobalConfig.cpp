#include "GlobalConfig.h"

bool GlobalConfig::m_cfg_bool[CFG_BOOL_END];

bool GlobalConfig::UpdataConfig(const char* cfgName)
{
	return false;
}

void GlobalConfig::InitAllConfig()
{
	InitCfgBool(CFG_BOOL_WIREFRAME,false);
	InitCfgBool(CFG_BOOL_LOSTDEVICE,false);
}

void GlobalConfig::InitCfgBool(eCONFIG_BOOL cfg,bool bvalue)
{
	m_cfg_bool[cfg]=bvalue;
}