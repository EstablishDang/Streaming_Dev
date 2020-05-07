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
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int *shmProgress;
//int initSharedMem = 0;
//int progressCounter =0;
int getSharedMem()
{
    int shmid;
    key_t key;
    int *shm;
    int rtn;
    key = 5678;
   if ((shmid = shmget(key, 8, 0666)) < 0) {
         return -1;
   }
   if ((shmProgress = shmat(shmid, NULL, 0)) == (int *) -1) {
         return -1;
   }
   rtn = *shmProgress;
   return rtn;
                                           
}

void sharedProgress(int state)
{
    int shmid;
    key_t key;
    //int *shm; 
    int retry = 0;
//    if (initSharedMem==0)
    {
      key = 5678;
      if ((shmid = shmget(key, 8, IPC_CREAT | 0666)) < 0) {
          perror("shmget");
         // return;
      }
      if ((shmProgress = shmat(shmid, NULL, 0)) == (int *) -1) {
          perror("shmat");
          return;
      }
    //  initSharedMem=1;
    }
    *shmProgress = state;
    *(shmProgress+1) = 0;
}

int main(int argc,char** argv)
{
    if (argc == 1)
    {
	//printf("%s [percent]\n",argv[0]);
	printf("%d",getSharedMem());
    }else
        sharedProgress(atoi(argv[1]));
    return 0;
}
