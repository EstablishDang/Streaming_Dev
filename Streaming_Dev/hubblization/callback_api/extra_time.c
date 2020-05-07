/*
 * Copyright (C) 2015 CVision.
 *
 * This unpublished material is proprietary to CVision.
 * All rights reserved. The methods and
 * techniques described herein are considered trade secrets
 * and/or confidential. Reproduction or distribution, in whole
 * or in part, is forbidden except by express written permission
 * of CVision.
 */

/* FILE   : extra_time.c
 * AUTHOR : Lap Dang
 * DATE   : Jan 7, 2019
 * DESC   : This file contain handler for set time zone
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include "fwpath.h"

#ifndef TZ_FILE
#define TZ_FILE "/tmp/TZ"
#endif

/*
 * After this, use TZ=0
 */
int set_time_to_tz(char *timezone)
    {
    int rc, mins = 0, hours = 0, ret = 0;
    char tmp[64];
    FILE * fp = NULL;

    assert(timezone);
#if DEF_DATE_TIME_ISO8601_FORMAT
    ret = sscanf(timezone, "%d:%d", &hours, &mins);
#else
    ret = sscanf(timezone, "%d.%d", &hours, &mins);
#endif
    syslog(LOG_INFO, "ret=%d - %s\n", ret, timezone);
    if(ret < 2)
        return -1;
    else if(hours > 14 || hours < -12)
        {
        return -2;
        }
    else if(mins < 0 || mins > 59)
        return -3;

    if(hours > 0)
        sprintf(tmp, "UTC%d:%d\n", -hours, mins);
    else
        sprintf(tmp, "UTC+%d:%d\n", -hours, mins);

    fp = fopen(TZ_FILE, "w");
    if(fp == NULL)
        {
        return -4;
        }
    fwrite(tmp, 1, strlen((char *) tmp), fp);
    fflush(fp);
    fclose(fp);

    return 0;
    }
int get_tz_from_time(char * timezone)
    {
    struct timeval now;
    struct timezone tz;
    int rc, mins = 0, hours = 0;

    rc = gettimeofday(&now, &tz);
    if(rc != 0)
        {
        if(errno == EFAULT)
            syslog(LOG_ERR, "HT : EFAULT\n");
        else if(errno == EINVAL)
            syslog(LOG_ERR, "HT : EINVAL\n");
        else if(errno == EPERM)
            syslog(  LOG_ERR, "HT : EPERM\n");
        return 1;
        }

    hours = tz.tz_minuteswest / 60;
    mins = tz.tz_minuteswest % 60;
    if(mins < 0)
        mins = -mins;
    if(hours > 0)
        sprintf(timezone, "UTC+%d:%d", hours, mins);
    else
        sprintf(timezone, "UTC%d:%d", hours, mins);
    return 0;
    }
