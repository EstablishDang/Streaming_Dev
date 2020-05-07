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
/** @file melody_callback.c
*   @author  Lap Dang
*   @date    2018/07/07
*   @brief This
*/

/*
 * Modify history:
 * 1.Date        : 2018/07/07
 *   Author      : Lap Dang
 *   Modification: Created file 
 *
 * 2.Date        : 
 *   Author      : 
 *   Modification:
*/

/**
 * @brief       To work with melody \par
 *              INPUT   :    command_id, input \par
 *              OUTPUT  :    output
 *
 * @param command_id is the integer
 * @param input is the void pointer
 * @param output is the void pointer
 * @return      0 if success \par
 *              otherwise if error \par
 *
 * @brief       Function to work with melody
 */
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "camera_setting.h"

int melody_callback(int command_id, void *input, void *output)
{
	int ret = 0;
	char *data = NULL;
	int *value = NULL;

	switch(command_id)
	{
		case MELODY_CMD_PLAY :
			break;
		case MELODY_CMD_STOP :
			break;
		case MELODY_CMD_GET_PLAY_ID :
			break;

		case MELODY_CMD_GET_MEDIA_LIST :   // for media feature
			break;
		case MELODY_CMD_MEDIA_NEXT :
			break;
		case MELODY_CMD_MEDIA_PRE :
			break;
		case MELODY_CMD_MDEIA_LIST_VERSON :
			break;
		case MELODY_CMD_MEDIA_GET_SONG :
			break;
		case MELODY_CMD_MEDIA_DOWNLOAD_SONG :
			break;
		case MELODY_CMD_MEDIA_DELETE_SONG :
			break;

		default :
			break;
	}
	return ret;
}