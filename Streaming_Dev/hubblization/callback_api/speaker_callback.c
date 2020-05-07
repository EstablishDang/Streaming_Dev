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
/** @file command_speaker.c
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
int speaker_callback(int command_id, void *input, void *output)
{
	int ret = 0;

	switch(command_id)
	{
		case SPK_CMD_SET_VOL :
			break;
		case SPK_CMD_GET_VOL :
			break;
		case SPK_CMD_SET_PROMPT_LANGUAGE :
			break;
		case SPK_CMD_GET_PROMPT_LANGUAGE :
			break;
	}
	return ret;
}
