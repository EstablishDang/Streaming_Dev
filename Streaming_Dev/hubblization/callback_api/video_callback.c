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
/** @file video_callback.c
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
#include "com_cvisionhk_MotionDetection.h"
#include "camera_setting.h"

int video_callback(int command_id, int input_val, int *output_val)
{
	int ret = 0;
	char nvconf_val[32] ={0};
	switch(command_id)
	{
		case VIDEO_CMD_SET_CONTRAST:	//for video contrast , range 0 -99
			syslog(LOG_INFO,"video set contrast value=%d", input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_CONTRAST, nvconf_val) < 0 )
            {
            	ret = ERROR_CODE_FAILED;
            }
       		else
            {
            	avcomm_set_contrast(input_val);
            }
			break;
		case VIDEO_CMD_GET_CONTRAST:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_CONTRAST , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get contrast command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_BRIGHTNESS:   //for video brightness , range 1 -7
			syslog(LOG_INFO,"video set brightness command_id=%d, value=%d", command_id, input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_BRIGHTNESS, nvconf_val) < 0 )
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				avcomm_set_brightness(input_val);
			}
			break;
		case VIDEO_CMD_GET_BRIGHTNESS:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_BRIGHTNESS , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get brightness command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_RESOLUTION:    //for video resolution , range 360, 480, 640, 720, 1080
			syslog(LOG_INFO,"video set resolution command_id=%d, value=%d", command_id, input_val);
			break;
		case VIDEO_CMD_GET_RESOLUTION:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_RESOLUTION , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get resolution command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_FRAMERATE:	//for video framerate , range 5->30
			syslog(LOG_INFO,"video set framerate command_id=%d, value=%d", command_id, input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_VIDEO_FRAMERATE, nvconf_val) < 0 )
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				ret = avcomm_set_framerate(input_val);
			}
			break;
		case VIDEO_CMD_GET_FRAMERATE:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_VIDEO_FRAMERATE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get framerate command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_BITRATE:  //for video bitrate , range 300->1500 Kbit
			syslog(LOG_INFO,"video set bitrate value=%d", input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_VIDEO_BITRATE, nvconf_val) < 0 )
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				ret = avcomm_set_bitrate(input_val);
			}
			break;
		case VIDEO_CMD_GET_BITRATE:   
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_VIDEO_BITRATE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get bitrate command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_GOP:  //for video bitrate , range 5->30 
			syslog(LOG_INFO,"video set gop value=%d", input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_VIDEO_GOP, nvconf_val) < 0 )
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				ret = avcomm_set_videogop(input_val);
			}
			break;
		case VIDEO_CMD_GET_GOP:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_VIDEO_GOP , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get group of pic command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_FLIPUP:   //for video flip-up , 0 : normal ; 1 : flip-up 
			syslog(LOG_INFO,"video set flipup value=%d", input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_FLIPUP, nvconf_val) < 0 )
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				com_cvisionhk_motion_detection_m_pause_sync(MOTIONDTC_PAUSE_START, &ret);
				syslog(LOG_INFO,"com_cvisionhk_motion_detection_m_pause_sync : pause  ret=%d", ret);
				ret = avcomm_set_flipup(input_val);
				com_cvisionhk_motion_detection_m_pause_sync(MOTIONDTC_PAUSE_STOP, &ret);
				syslog(LOG_INFO,"com_cvisionhk_motion_detection_m_pause_sync : resume  ret=%d", ret);
			}
			break;
		case VIDEO_CMD_GET_FLIPUP:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_FLIPUP , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get flip-up command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_FLICKER:   //for video flip-up , 50 or 60 depend on AC power
			syslog(LOG_INFO,"video set flicker value=%d", input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if(nvram_param_write(NVCONF_FLICKER, nvconf_val) < 0 )
			{
				ret = ERROR_CODE_FAILED;
			}
			else
			{
				ret = avcomm_set_flickering(input_val);
			}
			break;
		case VIDEO_CMD_GET_FLICKER:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_FLICKER , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get flicker command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_QUALITY: // //for video quality , rang 0->2
			syslog(LOG_INFO,"video set quality value=%d", input_val);
			ret = avcomm_set_video_quality(input_val);
			break;
		case VIDEO_CMD_GET_QUALITY:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_VIDEO_QUALITY , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get quality command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_SET_NIGHT_VISION:
			syslog(LOG_INFO,"video set night vision value=%d", input_val);
			memset(nvconf_val, 0, sizeof(nvconf_val));
			sprintf(nvconf_val, "%d", input_val);
			if (nvram_param_write(NVCONF_CDS_MODE, nvconf_val) != 0)
			{
				ret =ERROR_CODE_FAILED;
			}
			break;
		case VIDEO_CMD_GET_NIGHT_VISION:
			memset(nvconf_val, 0, sizeof(nvconf_val));
			ret = nvram_param_read(NVCONF_CDS_MODE , nvconf_val);
			*output_val =atoi(nvconf_val);
			syslog(LOG_INFO,"video get night vision command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;

		case VIDEO_CMD_GET_NIGHT_SENSOR_VALUE:
		    ret = com_cvisionhk_CDS_get_val(output_val);
			syslog(LOG_INFO,"video get night sensor value command_id=%d, ret=%d, value=%d", command_id, ret, *output_val);
			break;
		case VIDEO_CMD_SET_SATURATION:
			syslog(LOG_INFO,"video set saturation value=%d", input_val);
			break;
		case VIDEO_CMD_SET_SHARPNESS:
			syslog(LOG_INFO,"video set sharpness value=%d", input_val);
			break;
		case VIDEO_CMD_SET_IR_PWM:
			break;

		case VIDEO_CMD_START_RTSP:  //input port, not support http command from Hubble Server
			break;
		case VIDEO_CMD_STOP_RTSP:  //
			break;

		default:
			syslog(LOG_ERR,"Does not support command_id=%d", command_id);
			break;
	}
	return ret;
}
