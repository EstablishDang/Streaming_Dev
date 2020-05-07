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
/** @file network_callback.c
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
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include "nxcNetwork.h"
#include "cvappname.h"
#include "camera_setting.h"

int network_callback(int command_id, void *input, void *output)
{
	int ret =0;
	ROUTER_INFO *router_info;
	char *input_str = NULL;
	switch(command_id)
	{
		case NWK_CMD_SAVE_WIFI_INFO :
			router_info = (ROUTER_INFO*) input;
			ret = com_cvisionhk_setup_s_inform_set_wireless_save(router_info->ssid, router_info->wifi_mode, router_info->encrypt_type, router_info->pwd, router_info->protocol, router_info->network_type);
			syslog(LOG_INFO, "%s   %s   %s   %s    %s    %s",router_info->ssid, router_info->wifi_mode, router_info->encrypt_type, router_info->pwd, router_info->protocol, router_info->network_type);
			break;
		case NWK_CMD_GET_ROUTER_LIST :
			input_str = (char *)input;
			syslog(LOG_INFO,"router list file : %s", input_str);
			ret = nxcNetworkGetWifiListFile(input_str, 1);
			if(ret == 0)
			{
				HubbleKillProcByName(CVISION_SETUP_APP_NAME, SIGUSR1);
			}
			break;
		case NWK_CMD_GET_MAC_ADDRESS :
			break;
		case NWK_CMD_GET_WIFI_STRENGTH :
			break;
		case NWK_CMD_GET_CONNECTION_STATUS :
			break;
		case NWK_CMD_DISABLE_UAP_TIMEOUT :
			break;

		case NWK_CMD_SET_WIFI_MODE :
			break;
		case NWK_CMD_GET_WIFI_MODE :
			break;
		case NWK_CMD_SET_ROUTER_INFO :
			break;
		case NWK_CMD_GET_ROUTER_INFO :
			break;
		case NWK_CMD_GET_LOCAL_IP :
			break;

		case NWK_CMD_TEST_IPERF :
			break;
		case NWK_CMD_TEST_IPERF2 :
			break;
		default :
			break;
	}
	return ret;
}