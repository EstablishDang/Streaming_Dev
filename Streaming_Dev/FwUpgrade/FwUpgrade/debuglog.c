/*
* Copyright (C) 2019 CVision
*
* This unpublished material is proprietary to CVision.
* All rights reserved. The methods and
* techniques described herein are considered trade secrets
* and/or confidential. Reproduction or distribution, in whole
* or in part, is forbidden except by express written permission
* of CVision.
*/

#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "debuglog.h"
void startlog(char* name)
{
    openlog(name,LOG_PID | LOG_NDELAY|LOG_NDELAY,LOG_USER);
}
void printlog(const char* format, ...)
{
    va_list arg;
    va_start(arg,format);
    vsyslog(LOG_INFO,format,arg);
    va_end(arg);
}
