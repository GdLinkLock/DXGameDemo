#include "CamaraBase.h"
#include <Windows.h>

CameraKeys GetModelViewCameraMapedKey( unsigned int nKey )
{
	return CAM_UNKNOWN;
}

CameraKeys GetFPSCameraMapedKey( unsigned int nKey )
{
	switch( nKey )
	{
	case 'J':
		return CAM_STRAFE_LEFT;
	case 'L':
		return CAM_STRAFE_RIGHT;
	case 'I':
		return CAM_MOVE_FORWARD;
	case 'K':
		return CAM_MOVE_BACKWARD;
	case 'O':
		return CAM_MOVE_DOWN;
	case 'U':
		return CAM_MOVE_UP;
	default:
		return CAM_UNKNOWN;
	}

	return CAM_UNKNOWN;
}

int GetFPSCameraKeyCodeFromLogicKey(CameraKeys nKey )
{
	switch( nKey )
	{
	case CAM_STRAFE_LEFT:
		return 'J';
	case CAM_STRAFE_RIGHT:
		return 'L';
	case CAM_MOVE_FORWARD:
		return 'I';
	case CAM_MOVE_BACKWARD:
		return 'K';
	case CAM_MOVE_DOWN:
		return 'O';
	case CAM_MOVE_UP:
		return 'U';
	case CAM_SHIFTDOWN:
		return VK_SHIFT;
	default:
		return -1;
	}
	return -1;
}