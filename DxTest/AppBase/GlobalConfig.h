#pragma  once

enum eCONFIG_BOOL
{
	CFG_BOOL_WIREFRAME,//�߿�ģʽ
	CFG_BOOL_LOSTDEVICE,//�豸��ʧ 

	CFG_BOOL_END
};

 class GlobalConfig
 {
 public:

	 static bool UpdataConfig(const char* cfgName);
	 static void InitAllConfig();
	 static void InitCfgBool(eCONFIG_BOOL cfg,bool bvalue);


	 //�߿�ģʽ
	 static bool EnableWireFrame(){return m_cfg_bool[CFG_BOOL_WIREFRAME];}
	 static void SetEnableWireFrame(bool b){m_cfg_bool[CFG_BOOL_WIREFRAME]=b;}
	 //�豸��ʧ
	 static bool IsLostDevice(){return m_cfg_bool[CFG_BOOL_LOSTDEVICE];}
	 static void SetLostDevice(bool b){m_cfg_bool[CFG_BOOL_LOSTDEVICE]=b;}
 private:
	 static bool  m_cfg_bool[CFG_BOOL_END];

 };