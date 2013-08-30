#pragma  once
#include <d3dx9.h>
#include "../CommenCode/InPut/DXInPutManager.h"

enum CameraKeys
{
	CAM_STRAFE_LEFT = 0,
	CAM_STRAFE_RIGHT,
	CAM_MOVE_FORWARD,
	CAM_MOVE_BACKWARD,
	CAM_MOVE_UP,
	CAM_MOVE_DOWN,
	CAM_RESET,
	CAM_CONTROLDOWN,
	CAM_SHIFTDOWN,
	CAM_ALTDOWN,

	CAM_MAX_KEYS,
	CAM_UNKNOWN     = 0xFF
};


	
	 CameraKeys GetModelViewCameraMapedKey( unsigned int nKey );
	 CameraKeys GetFPSCameraMapedKey( unsigned int nKey );
	 int GetFPSCameraKeyCodeFromLogicKey(CameraKeys nKey );


class CameraBase
{
public:
	virtual ~CameraBase(){}

	virtual void SetPos(const D3DXVECTOR3& pos)=0;
	virtual void SetLookAt(const D3DXVECTOR3& at)=0;
	virtual void SetLookAtDir(const D3DXVECTOR3& dir)=0;

	virtual const D3DXVECTOR3& GetPos()=0;
	virtual const D3DXVECTOR3& GetViewAtDir()=0;
	virtual const D3DXVECTOR3& GetUp()=0;
};