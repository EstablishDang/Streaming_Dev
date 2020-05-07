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
/** @file temperature_callback.c
*   @author  Lap Dang
*   @date    2018/07/06
*   @brief This
*/

/*
 * Modify history:
 * 1.Date        : 2018/07/06
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
#include "nvconf.h"
#include "com_cvisionhk_temperature.h"
#include "camera_setting.h"

/**
 * @brief       To work with temperature module \par
 *              INPUT   :    command_id, input_value \par
 *              OUTPUT  :    output_val
 *
 * @param command_id is the integer
 * @param command_id is the integer
 * @param command_id is the integer
 * @return      0 if success \par
 *              otherwise if error \par
 *
 * @brief       Function to work with temperature module
 */
int temperature_callback(int command_id, int input_val, int *output_val)
{
	int ret = 0;
	int celsius_tempvalue = 0;
	char nvconf_val[32] ={0};
	char strVal[32] ={0};
	switch (command_id)
	{
		case TEMP_CMD_GET_RAW_VALUE :
			ret = com_cvisionhk_temp_get_val(output_val);
			syslog(LOG_INFO,"get temperature raw value : %d, ret=%d", output_val, ret);
			break;

		case TEMP_CMD_GET_VALUE :
			ret = com_cvisionhk_temp_get_compensated_val(&output_val);
			syslog(LOG_INFO,"get temperature value : %d, ret=%d", output_val, ret);
			break;

		case TEMP_CMD_GET_FAHREN_VALUE :
			ret = com_cvisionhk_temp_get_compensated_val(&celsius_tempvalue);
			if(ret == 0)
			{
				/* convert from Celsius to Fahrenheit */
				*output_val = (celsius_tempvalue * 0.1 * 1.8) + 32;
				syslog(LOG_INFO,"Read Temperature %s", output_val);
			}
			else
			{
				syslog(LOG_INFO,"Read Temperature failed");
			}
			break;

		case TEMP_CMD_SET_LOW_THRESHOLD :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_LO_THRESHOLD_VALUE, nvconf_val) < 0)
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				memset(strVal, 0, sizeof(strVal));
				sprintf(strVal, "lt=%s", nvconf_val);
				ret = com_cvisionhk_HubbleAlarm_m_ParameterSettingUpdate_sync(strVal);
			}
			syslog(LOG_INFO,"set low temperature threshold : %d", input_val);
			break;
		case TEMP_CMD_GET_LOW_THRESHOLD :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read( NVCONF_LO_THRESHOLD_VALUE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"temperature get low threshold command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case TEMP_CMD_SET_HIGH_THRESHOLD :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_HI_THRESHOLD_VALUE, nvconf_val) < 0)
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				memset(strVal, 0, sizeof(strVal));
				sprintf(strVal, "ht=%s", nvconf_val);
				ret = com_cvisionhk_HubbleAlarm_m_ParameterSettingUpdate_sync(strVal);
			}
			syslog(LOG_INFO,"set high temperature threshold : %d, ret =%d", input_val, ret);
			break;
		case TEMP_CMD_GET_HIGH_THRESHOLD :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read( NVCONF_HI_THRESHOLD_VALUE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"temperature get high threshold command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case TEMP_CMD_SET_LOW_NOTIFY :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_HI_THRESHOLD_VALUE, nvconf_val) < 0)
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				memset(strVal, 0, sizeof(strVal));
				sprintf(strVal, "ls=%s", nvconf_val);
				ret = com_cvisionhk_HubbleAlarm_m_ParameterSettingUpdate_sync(strVal);
			}
			syslog(LOG_INFO,"set low temperature notify : %d, ret =%d", input_val, ret);
			break;

		case TEMP_CMD_GET_LOW_NOTIFY :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read( NVCONF_LO_TEMP_DETECTION_ENABLE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"temperature get low notify command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case TEMP_CMD_SET_HIGH_NOTIFY :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_HI_THRESHOLD_VALUE, nvconf_val) < 0)
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				memset(strVal, 0, sizeof(strVal));
				sprintf(strVal, "hs=%s", nvconf_val);
				ret = com_cvisionhk_HubbleAlarm_m_ParameterSettingUpdate_sync(strVal);
			}
			syslog(LOG_INFO,"set high temperature notify : %d, ret =%d", input_val, ret);
			break;

		case TEMP_CMD_GET_HIGH_NOTIFY :
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read( NVCONF_HI_TEMP_DETECTION_ENABLE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"temperature get high notify command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		default :
			syslog(LOG_ERR,"Does not support command_id=%d", command_id);
			break;
	}
	return ret;
}