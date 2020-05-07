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
/** @file system_callback.c
*   @author  Lap Dang
*   @date    2018/07/06
*   @brief This file implements set-get camera system info
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
#include "camera_setting.h"
#include "nvconf.h"
#include "hubble_stat.h"


#ifndef DAYLIGHT_SAVING_TIME_PATH
#define DAYLIGHT_SAVING_TIME_PATH 				"/tmp/DST"
#endif /* DAYLIGHT_SAVING_TIME_PATH */
#ifndef EXT_HARD_RESET_SCRIPT
#define EXT_HARD_RESET_SCRIPT       			CVISION_FW_APP_PATH"/hard_reset.sh"
#endif
#define NXCCOMMON_HTTPSERVER_MODULE             "HT"
#define RESET_FACTORY_REBOOT_REASON             "Reset factory"

static int reset_factory(void)
{
    char cmd[512] = {0};
    //add skill ftpuploader #MBP945-816
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, (sizeof(cmd)/sizeof(cmd[0])) - 1, "killall -9 %s", CVISION_FTP_UPLOADER_APP_NAME);
    RunSystemCmd(cmd);

    /* Process the message */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 511, "rm -rf %s", USER1_CONF_PATH);
    RunSystemCmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 511, "cd %s && ls *.conf | grep -E -v 'reboot_reason.log|threshold.conf|system.conf|zlog.conf' | xargs rm -rf", IN_FW_CONF_PATH);
    RunSystemCmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 511, "cd %s && ls *.conf *.json bootup_info | grep -E -v 'reboot_reason.log|threshold.conf|system.conf|zlog.conf' | xargs rm -rf", CONFIG_RW_PATH);
    RunSystemCmd(cmd);

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, 511, "rm -rf %s/* %s/*", CVISION_FW_UPGRADE_PKG, TMP_STORAGE);
    RunSystemCmd(cmd);

#if (DEF_DO_EXTRA_CMD_DURING_FACTORY_RESET)
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd)-1, "%s",EXT_HARD_RESET_SCRIPT);
    RunSystemCmd(cmd);
#endif

    nxcCommon_logging_reboot_reason(NXCCOMMON_HTTPSERVER_MODULE,
                                    __FILE__, __LINE__,
                                    RESET_FACTORY_REBOOT_REASON);
    //Flush data
    sync();
    nxcCommon_watchdog_reboot();
	return 0;
}
static int reboot_camera(char *input)
{
	char cmd[512] = {0};
	int reboot_delay = 0;
	reboot_delay = atoi(input);
	if ((int*)input != NULL)
	{
		if(reboot_delay < 0)
		{
			return -2;
		}
		//dzlog_info( "cmd_reboot, camera will reboot after %d",reboot_delay);
		sync();
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd) - 1, "reboot -d %d -f &", reboot_delay);
		RunSystemCmd(cmd);
	}
	else
	{
		return -3;
	}
	return 0;
}
#if 0
static int Get_model(char *out)
{
    char strModel[8] = {0};
    char strUDID[32] = {0};
    int  ret = -1;
    ret = nvram_param_read(NVCONF_MODEL, out);
    Get_udid(strUDID);
    memcpy(strModel, strUDID + 2, 4);
    //dzlog_info( "Model %s", strModel);

    if( ((ret == 0) && (strcmp(out, strModel) != 0)) ||
            ((ret < 0) && (strModel[0] != 0))
      )
        {
        if(nvram_param_write1(NVCONF_MODEL, strModel) < 0)
            ret = -2;
        else
            ret = 0;
        }
    strcpy(out, strModel);
    //dzlog_info( "Model = %s, ret = %d", out, ret);
    return 0;
}
#endif
static int set_city_timezone(char* Input)
{
    int ret = 0;
    FILE *fp = NULL;
    //dzlog_info( "cmd_set_city_timezone value=%s", Input);
    if(nvram_param_write(NVCONF_CITY_TIME_ZONE, Input) < 0)
        ret = -1;
    fp = fopen(DAYLIGHT_SAVING_TIME_PATH, "w");
    if(fp == NULL)
    {
        ret = -2;
    }
    if(ret == 0)
    {
        fwrite(Input, 1, strlen((char *) Input), fp);
        fclose(fp);
        HubbleSignal(eHubbleHomeSignalConfUpdateUser, eHubbleHomeUserTimeZone);
    }
    //dzlog_info( "ret = %d", ret);
    return ret;
}
static int set_time_zone(char *input)
{
    int     ret = 0;
    char    *pctmp = NULL;

    if(strchr(input, '-') == NULL)
    {
        pctmp = strchr(input, ' ');
        if(pctmp)
            *pctmp = '+';
    }
    ret = set_time_to_tz(input);
    //dzlog_info( "set_time_to_tz ret = %d", ret);
    if(ret == 0)
    {
        if(nvram_param_write(NVCONF_TIME_ZONE, input) < 0)
            ret = -2;
    }
    HubbleSignal(eHubbleHomeSignalConfUpdateUser, eHubbleHomeUserTimeZone);
    //dzlog_info( "ret = %d", ret);
    return ret;
}
static int set_date_time(char *input)
{
    int  ret = 0;
    char cCmdBuf[512] = {0};
    int year, month, day, hour, min, second;
    ret = sscanf( input, "%04d%02d%02d%02d%02d.%02d", &year, &month, &day, &hour, &min, &second );
    if(ret == 6)
    {
        snprintf(cCmdBuf, sizeof(cCmdBuf), "date -s \"%s\"", input);
        ret = RunSystemCmd(cCmdBuf);
    }
    else
    {
        //dzlog_error( "Can not parse value of date");
        ret = -1;
    }
    //dzlog_info( "ret = %d", ret);
    return ret;
}

int system_callback(int command_id, void *input, void *output)
{
	int ret = 0;
	char *data = NULL;
	switch(command_id)
	{
		case SYS_CMD_RESTART_SYSTEM:
			ret = com_cvisionhk_setup_s_start_register();
			syslog(LOG_INFO,"restart system");
			break;

		case SYS_CMD_RESET_FACTORY:
			ret = reset_factory();
			syslog(LOG_INFO,"reset factory");
			break;

		case SYS_CMD_REBOOT_CAMERA:
			ret = reboot_camera((char*)input);
			syslog(LOG_INFO,"reboot camera");
			break;

		case SYS_CMD_GET_VERSION:
			data = (char*)output;
			ret = nvram_param_read(NVCONF_FW_VERSION, data);
			syslog(LOG_INFO,"get version");
			break;

		case SYS_CMD_GET_UDID:
			//ret = Get_udid((char*)output);
			ret = nvram_param_read(NVCONF_UDID, (char*)output);
			syslog(LOG_INFO,"get UDID");
			break;

		case SYS_CMD_GET_MODEL:
			ret =  ret = nvram_param_read(NVCONF_MODEL, (char*)output);
			syslog(LOG_INFO,"get model");
			break;

		case SYS_CMD_SET_LOCAL_TIMEZONE:
			if(NULL == input)
			{
				syslog(LOG_INFO,"set local timezone : Invalid input");
				ret = -3;
			}
			else
			{
				data = (char*) input;
				ret = set_city_timezone(data);
				syslog(LOG_INFO,"set local timezone : %s", data);				
			}
			break;

		case SYS_CMD_GET_LOCAL_TIMEZONE:
			data = (char*)output;
			ret = nvram_param_read(NVCONF_CITY_TIME_ZONE, data);
			syslog(LOG_INFO,"get local timezone");
			break;

		case SYS_CMD_SET_TIMEZONE:
			syslog(LOG_INFO,"set timezone");
			if(NULL == input)
			{
				syslog(LOG_INFO,"set timezone : Invalid input");
				ret = -3;
			}
			else
			{
				data = (char*) input;
				ret = set_time_zone(data);
				syslog(LOG_INFO,"set timezone : %s", data);				
			}
			break;

		case SYS_CMD_GET_TIMEZONE:
			data = (char*)output;
			nvram_param_read(NVCONF_TIME_ZONE, data);
			syslog(LOG_INFO,"get timezone");
			break;

		case SYS_CMD_SET_DATE:
			if(NULL == input)
			{
				syslog(LOG_INFO,"set date : Invalid input");
				ret = -3;
			}
			else
			{
				data = (char*) input;
				ret = set_date_time(data);
				syslog(LOG_INFO,"set date : %s", data);				
			}
			break;

		case SYS_CMD_GET_DATE:
			if(NULL == input)
			{
				syslog(LOG_INFO,"get date: Invalid input");
				ret = -3;
			}
			else
			{
				data = (char*) input;
				ret = nvram_param_read(NVCONF_TIME_ZONE, data);
				syslog(LOG_INFO,"get date : %s", data);
			}
			break;

		case SYS_CMD_GET_CAMERA_SETTING:
			break;

		case SYS_CMD_ENABLE_SYSTEM_MONITOR:
			break;

		case SYS_CMD_REQUEST_LOG:
			break;

		case SYS_CMD_SET_SUBSCRIPTION:
			if(NULL == input)
			{
				syslog(LOG_INFO,"set subscription : Invalid input");
				ret = -3;
			}
			else
			{
				data = (char*) input;
				syslog(LOG_INFO,"set subscription : %s", data);				
			}
			break;

		case SYS_CMD_GET_SUBSCRIPTION:
			break;

		case SYS_CMD_REQUEST_UPDATE_MVR_SCHEDULE:
			break;

		case SYS_CMD_REQUEST_UPDATE_SERVER_URL:
			break;

		case SYS_CMD_UPDATE_TIME:
			break;

		case SYS_CMD_RESTART_NEO:
			if(NULL == input)
			{
				syslog(LOG_INFO,"restart NEO : Invalid input");
				ret = -3;
			}
			else
			{
				data = (char*) input;
				syslog(LOG_INFO,"restart NEO : %s", data);				
			}
			break;

		default:
			break;
	}
	return ret;
}
