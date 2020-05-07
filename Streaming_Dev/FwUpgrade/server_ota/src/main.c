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

/* FILE   : main.c
 * AUTHOR : Lap Dang ☻
 * DATE   : Jan 7, 2019
 * DESC   : Firmware OTA main application
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <dbus/dbus.h>
#include <sys/stat.h>
#include <signal.h>

#include "download.h"
#include "ota.h"
#include "nvconf.h"
#include "hubble_stat.h"
#include "hubble_system.h"
#include "fwpath.h"
#include "zlog.h"

#ifndef OTA_FW_PACKAGES_FLAG
#define OTA_FW_PACKAGES_FLAG    CVISION_FW_UPGRADE_PKG"/hubble_ota_pkg"
#endif
#ifndef OTA_FW_PACKAGES_SIGNATURE
#define OTA_FW_PACKAGES_SIGNATURE   CVISION_FW_UPGRADE_PKG"/hubble_ota_sig"
#endif
#ifndef OTA_FW_PACK_DEC_FLAG
#define OTA_FW_PACK_DEC_FLAG   CVISION_FW_UPGRADE_PKG"/firmware_dec"
#endif

#define OTA_SERVER_LOCATION     OTA_SERVER_LOCATION_BASE
#define DEV_TEST                1

static uint32_t      prevTime = 0;
static uint32_t      curTime = 0;

/*
 * PURPOSE : Set status of update camera ip task
 * INPUT   : [stat] - Status of task
 * OUTPUT  : None
 * RETURN  : None
 * DESCRIPT: None
 */
uint32_t
cameraSecondsTimeGet()
    {
    uint32_t ret = 0;
    struct timespec t = {0};

    if(clock_gettime(0, &t) == 0)
        ret = t.tv_sec;
    return ret;
    }

eOTAErrorCode
ota_check_and_download(void)
    {
    int i32ret = 0;
    char sigfilepath[128] = {0,};
    char fwfilepath[128] = {0,};
    char cSigUrl[2048] = {0,};
    char cFwUrl[2048] = {0,};
    char cSigFileName[32] = {0,};
    char cFwPkgName[32] = {0,};
    int is_new_fw = 1;
    int fw_downloaded = 0;
    int retries = 3;
    FILE *fp = NULL;

    // check whether an available package already avail to upgrade or not
    if(access(OTA_FW_PACKAGES_FLAG, R_OK) == 0 && access(OTA_FW_PACKAGES_SIGNATURE, R_OK) == 0)
        {
            //TODO check
        dzlog_info("Upgrade Package avail\n");
        return eOTAErrDownLoadSuccess;
        }

    // Read the signature firmware packgae download url
    fp = fopen(SIGNATURE_URL_PATH, "r");
    if(fp == NULL)
        {
        dzlog_info("Not found signature url! Firmware is up to date\n");
        return eOTAErrFirmwareUpToDate;
        }

    while(fgets(cSigUrl, sizeof(cSigUrl), fp) != NULL)
        {
        if(strcmp(cSigUrl, "") == 0)
            is_new_fw = 0;
        }
    fclose(fp);

    fp = fopen(FWPACKAGE_URL_PATH, "r");
    if(fp == NULL)
        {
        dzlog_info("Not found firmware package url! Firmware is up to date\n");
        return eOTAErrFirmwareUpToDate;
        }
    while(fgets(cFwUrl, sizeof(cFwUrl), fp) != NULL)
        {
        if(strcmp(cFwUrl, "") == 0)
            is_new_fw = 0;
        }
    fclose(fp);

    if(is_new_fw == 0)
        return eOTAErrFirmwareUpToDate;

    //! Get firmware
    ota_get_fw_filename(cFwUrl, cFwPkgName);
    sprintf(fwfilepath, CVISION_FW_UPGRADE_PKG"/%s", cFwPkgName);

    ota_get_sig_filename(cSigUrl, cSigFileName);
    sprintf(sigfilepath, CVISION_FW_UPGRADE_PKG"/%s", cSigFileName);

    //! Verify the firmware that already downloaded
    if(access(fwfilepath, F_OK) == 0)
        {
        // file exists
        dzlog_info("Package file (%s) exist", fwfilepath);
        if(access(sigfilepath, F_OK) == 0)
            {
            dzlog_info("Signature file (%s) exist", sigfilepath);
            //! Checksum
            i32ret = ota_is_fw_valid(fwfilepath, sigfilepath);
            if(i32ret == 0)
                {
                dzlog_info("Downloaded firmware is valid! Ready for upgrade!");
                fw_downloaded = 1;
                }
            }
        }
    else
        {
        //! Clear the download directory
        HubbleRemoveDirectory(CVISION_FW_UPGRADE_PKG, 1);
        }

    if(fw_downloaded == 0)
        {
        while(retries--)
            {
            i32ret = donwload_file(cFwUrl, fwfilepath);
            if(i32ret != 0)
                {
                dzlog_info("Error: %s Download file fail\n", cFwUrl);
                goto remove_old_files;
                }
            i32ret = donwload_file(cSigUrl, sigfilepath);
            if(i32ret != 0)
                {
                dzlog_error("Error: %s Download file fail\n", cSigUrl);
                goto remove_old_files;
                }
            //! Checksum
            i32ret = ota_is_fw_valid(fwfilepath, sigfilepath);
            if(i32ret != 0)
                {
                dzlog_error("Error: checksum not match");
                goto remove_old_files;
                }
            fw_downloaded = 1;
            break;

remove_old_files:
#if 0
            remove(fwfilepath);
#endif
            remove(sigfilepath);
            remove(OTA_FW_PACKAGES_FLAG);
            remove(OTA_FW_PACKAGES_SIGNATURE);
            remove(OTA_FW_PACK_DEC_FLAG);
            }
        }

    //! Trigger firmware upgrade
    if(fw_downloaded == 1) // double check whether upgrade already be called or not. If already no need to do anything
        {
        fp = fopen(OTA_FW_PACKAGES_FLAG, "w");
        if(fp == NULL)
            {
            return eOTAErrSystemErr;
            }

        dzlog_info("Mark the path firmware package %s\n", fwfilepath);
        fprintf(fp, "%s\n", fwfilepath);  // Write package name to OTA_FW_PACKAGES_FLAG file
        fclose(fp);

        fp = fopen(OTA_FW_PACKAGES_SIGNATURE, "w");
        if(fp == NULL)
            {
            return eOTAErrSystemErr;
            }
        dzlog_info("Mark the path digital signature %s\n", sigfilepath);
        fprintf(fp, "%s\n", sigfilepath);  // Write package name to OTA_FW_PACKAGES_FLAG file
        fclose(fp);
        }
    else  // NO NEED TO DO ANYTHING
        {
        dzlog_error("Download retry timeout");
        return eOTAErrDownLoadFail;
        }

    return eOTAErrDownLoadSuccess;
    }

void
handlerUSR1()
{
    prevTime = cameraSecondsTimeGet();
}

int
main(int argc, char *argv[])
    {
    eOTAErrorCode eErrCode;
    int           iRetCode;
    struct stat st = {0};

//    iRetCode = dzlog_init(CVISION_LOG_CONFIG_PATH, "OTA");
//    if(iRetCode)
//        printf("ZLOG initial: failed\n");

    openlog("[OTA_SERVER]", LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Begin using syslog");

    dzlog_info("Server OTA feature for MODEL %s. Using HTTPS", HUBBLE_CAMERA_MODEL_STRING);

    if (stat(CVISION_FW_UPGRADE_PKG, &st) == -1)
        {
        mkdir(CVISION_FW_UPGRADE_PKG, 0777);
        }

    /* Requirement : Only check and do OTA after get firmware url from
       bootup query response from server.
     */

    signal(SIGUSR1, handlerUSR1);
    pause();

    // Wait, then start to check & download
    sleep(30);

#if (HC_FAMILY_CAM_MODEL)
    dzlog_info("Got signal after camera boot up done. Start to check OTA!");
    eErrCode = ota_check_and_download();
    if(eErrCode == eOTAErrDownLoadSuccess)
        HubbleSignal(eHubbleHomeSignalFwUpgrade, eHubbleHomeGeneralStatusDownloaded);

    if(access(FWUPGRADE_FORCE_FLAG, F_OK) == 0)
    {
        FILE *pFile = NULL;
        int force_upgrade = 0;
        char force_flag[8] = {0};

        pFile = fopen(FWUPGRADE_FORCE_FLAG, "r");
        if(pFile)
        {
            fscanf(pFile, "%s", force_flag);
            dzlog_info("Server OTA force flag (%s)", force_flag);

            if(strcmp(force_flag, "true") == 0)
                force_upgrade = 1;
            fclose(pFile);
        }

        if(force_upgrade)
        {
            system("touch /mnt/cache/nightreboot.flag");
            system("reboot -d 10 -f");
        }
    }
#else
    while(1)
        {
        dzlog_info("Got signal after camera boot up done. Start to check OTA!");
        curTime = cameraSecondsTimeGet();
        eErrCode = ota_check_and_download();
        if(eErrCode == eOTAErrDownLoadSuccess)
            {
            if(HubbleSignal(eHubbleHomeSignalFwUpgrade, eHubbleHomeGeneralStatusDownloaded) == 0)
                break;
            }
        if(curTime - prevTime > 1000000)
            prevTime = cameraSecondsTimeGet();
        else
            {
            //Excess 30 minutes
            if(curTime - prevTime > 1800)
                {
                sleep(1800);
                }
            else
                sleep(300);
            }
        }
#endif

    dzlog_info("OTA UPGRADE EXIT");

    closelog();
    return 0;
    }

