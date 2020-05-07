/*
* Copyright (C) 2019 CVision
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "zlog.h"
#include "nxcHubbleAPI.h"
#include "nvconf.h"
#include "fwpath.h"

int
main(int agrc, char **argv)
    {
    int ret = 0;
    char server[128];
    char auth_token[64];

    int type = 0;

//    ret = dzlog_init(CVISION_LOG_CONFIG_PATH, "UPNO");
//    if(ret)
//        printf("zlog init failed\n");

    openlog("[OTA_NOTI]", LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Begin using syslog");

    if(agrc != 1)
        {
    	dzlog_info("Fail to run push notification app because of the number of arguments\n");
        return -1;
        }

    type = atoi(argv[1]);

    if(type == 99)
    {
		if(nvram_param_read(NVCONF_SERVER_API, server) != 0)
		{
			dzlog_info("No API server to inform\n");
			return -1;
		}
		if(nvram_param_read(NVCONF_MASTER_KEY, auth_token) == 0)
		{
			dzlog_info("No Master key\n");
			return -1;
		}
		ret = nxcHubblePushFWUpgrading(server, 443, auth_token);
		if(ret != 0)
		{
			dzlog_info("Upgrading notification response %d\n", ret);
		}
    }
    else
    {
    	// do nothing. Prevent someone try to use this app to push notification to server
    	dzlog_info("Upgrade notification had been called with arg %s\n", argv[1]);
    }

    closelog();
    return ret;
    }
