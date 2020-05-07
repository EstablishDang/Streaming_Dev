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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "fwpath.h"

#ifndef FWPATH_FILE
#define FWPATH_FILE "/mnt/cache/new_fwupgrade"
#endif

static char *fwpath_file = FWPATH_FILE;
static int  flag_exit = 0;

void
Fwupgrade()
    {
    char buf[128]={0};
    char cmd[128]={0};

	FILE * fp = fopen(fwpath_file, "r");
	if(fp == NULL)
		{
		printf("Request FWUpgrade but nothing to upgrade\n");
		return;
		}
	else
	{
		if(fgets(buf, 128, fp) != NULL)
		{

		}
		fclose(fp);
	
		if (strlen(buf) > 10) // filename of the real upgrade is much longer than that
		{
			sprintf(cmd, "fwupgrade.sh %s", buf);
			printf("%s\n", cmd);
			system(cmd);

			if(unlink(buf) != 0)
			{
					sprintf(cmd, "rm %s", buf);
					printf("%s\n", cmd);
					system(cmd);
			}
		}
			unlink(fwpath_file);
		}
    }

void
handlerCtrlC()
    {
    flag_exit = 1;
    }

int
main(int argc, char * argv[])
    {
    int ret = 0;
    if(argc > 1)
    	fwpath_file = argv[1];

    signal(SIGUSR1, Fwupgrade);
    signal(SIGINT, handlerCtrlC);

    while(flag_exit == 0)
        {
        pause();
        }

    printf("fwupgrade exit!");
    return 0;
    }

