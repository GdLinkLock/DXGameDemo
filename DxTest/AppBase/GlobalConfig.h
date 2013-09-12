#pragma  once

enum eCONFIG_BOOL
{
	CFG_BOOL_WIREFRAME,//线框模式
	CFG_BOOL_LOSTDEVICE,//设备丢失 

	CFG_BOOL_END
};

 class GlobalConfig
 {
 public:

	 static bool UpdataConfig(const char* cfgName);
	 static void InitAllConfig();
	 static void InitCfgBool(eCONFIG_BOOL cfg,bool bvalue);


	 //线框模式
	 static bool EnableWireFrame(){return m_cfg_bool[CFG_BOOL_WIREFRAME];}
	 static void SetEnableWireFrame(bool b){m_cfg_bool[CFG_BOOL_WIREFRAME]=b;}
	 //设备丢失
	 static bool IsLostDevice(){return m_cfg_bool[CFG_BOOL_LOSTDEVICE];}
	 static void SetLostDevice(bool b){m_cfg_bool[CFG_BOOL_LOSTDEVICE]=b;}
 private:
	 static bool  m_cfg_bool[CFG_BOOL_END];

 };