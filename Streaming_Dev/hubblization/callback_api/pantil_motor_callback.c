/*
*   This unpublished material is proprietary to Hubble.
*   Copyright (C) 2019 Hubble.
*   All rights reserved. The methods and
*   techniques described herein are considered trade secrets
*   and/or confidential. Reproduction or distribution, in whole
*   or in part, is forbidden except by express written permission
*   of Hubble.
*    
*   
*/
/** @file command_motor.c
*   @author  Lap Dang
*   @date    2018/05/21
*   @brief This
*/

/*
 * Modify history:
 * 1.Date        : 2018/05/21
 *   Author      : Lap Dang
 *   Modification: Created file 
 *
 * 2.Date        : 
 *   Author      : 
 *   Modification:
*/
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "camera_setting.h"

int pantil_motor_callback(int command_id, int input, void *output)
{
	int ret = 0;

	switch(command_id)
	{
		case PTZ_CMD_MOVE_LEFT :
			break;
		case PTZ_CMD_MOVE_RIGHT :
			break;
		case PTZ_CMD_MOVE_LEFT_STEP :
			break;
		case PTZ_CMD_MOVE_RIGHT_STEP :
			break;

		case PTZ_CMD_MOVE_UP :
			break;
		case PTZ_CMD_MOVE_DOWN :
			break;

		case PTZ_CMD_MOVE_FORWARD_AND_BACK :
			break;
		case PTZ_CMD_MOVE_LEFT_AND_RIGHT :
			break;

		case PTZ_CMD_SET_H_CRUISE :
			break;
		case PTZ_CMD_GET_H_CRUISE :
			break;
		case PTZ_CMD_GET_SOC_VERSION :
			break;

		default:
			break;
	}
	return ret;
}
