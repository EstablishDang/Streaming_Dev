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
/** @file talkback_callback.c
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
#include "camera_setting.h"

int talkback_callback(int command_id)
{
    int ret = 0;
    
    switch(command_id)
    {
        case TALKBACK_CMD_LOCAL_ENABLE :
            break;
        case TALKBACK_CMD_LOCAL_DISABLE :
            break;
        case TALKBACK_CMD_REMOTE_ENABLE :
            break;
        case TALKBACK_CMD_REMOTE_DISABLE :
            break;
        case TALKBACK_CMD_CHECK_READY_PLAY :
            break;
        default:
            break;
	}
    return ret;
}

