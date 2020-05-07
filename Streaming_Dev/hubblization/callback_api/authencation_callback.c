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
/** @file athencation_callback.c
*   @author  Lap Dang
*   @date    2018/07/09
*   @brief This
*/

/*
 * Modify history:
 * 1.Date        : 2018/07/09
 *   Author      : Lap Dang
 *   Modification: Created file 
 *
 * 2.Date        : 
 *   Author      : 
 *   Modification:
*/


#include <stdio.h>
#include <syslog.h>
#include "camera_setting.h"
#include "hubble_stat.h"

int authencation_callback(int command_id, void * input)
{
	int ret = 0;
	AUTH_INFO  *auth_pointer;
	switch(command_id)
	{
		case AUT_SET_API_KEY_TIMEZONE :
			auth_pointer = (AUTH_INFO*)input;
			syslog(LOG_INFO,"api_key=%s, timezone=%s", auth_pointer->api_key, auth_pointer->timezone);
			set_time_to_tz(auth_pointer->timezone);
			HubbleSignal(eHubbleHomeSignalConfUpdateUser, eHubbleHomeUserTimeZone);
			ret = com_cvisionhk_setup_s_inform_set_server_auth(auth_pointer->timezone, auth_pointer->api_key);
			break;
		case AUT_SET_MASTERKEY :
			break;
		default:
			break;
	}
	return ret;
}
