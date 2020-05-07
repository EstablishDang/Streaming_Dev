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

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_

#include <syslog.h>

void startlog(char* name);
void printlog(const char* format, ...);
#undef printf
#define printf printlog

#endif /* MLSCRC_H_ */
