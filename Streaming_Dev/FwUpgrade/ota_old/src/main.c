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
#include "fwpath.h"
#include "ota.h"
#include "download.h"
#include "zlog.h"
#include "nvconf.h"
#include "hubble_stat.h"

#include "fwupgrade_extra_function.h"

#ifndef OTA_FW_PACKAGES_FLAG
#define OTA_FW_PACKAGES_FLAG    CVISION_FW_UPGRADE_PKG"/hubble_ota_pkg"
#endif
#ifndef OTA_FW_PACKAGES_SIGNATURE
#define OTA_FW_PACKAGES_SIGNATURE   CVISION_FW_UPGRADE_PKG"/hubble_ota_sig"
#endif
#ifndef OTA_FW_PACK_DEC_FLAG
#define OTA_FW_PACK_DEC_FLAG   CVISION_FW_UPGRADE_PKG"/firmware_dec"
#endif

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
    int fw_for_dev = 0;
    char filepath[128] = {0};
    char fwfilepath[128] = {0};
    char sServerURL[64] = {0};
    char *pc_tmp = NULL;
    char url[128];
    char buf[128];
    char udid[32];
    int is_new_fw = 0, fw_downloaded = 0;
    int ota_major = 0, ota_minor = 0, ota_revision = 0;
    int sys_major = 0, sys_minor = 0, sys_revision = 0;
    FILE *fp = NULL;

    // check whether an available package already avail to upgrade or not
    if(access(OTA_FW_PACKAGES_FLAG, R_OK) == 0 && access(OTA_FW_PACKAGES_SIGNATURE, R_OK) == 0)
        {
            //TODO check
        dzlog_info("Upgrade Package avail\n");
        return eOTAErrDownLoadSuccess;
        }

    // get the server URL dynamically
    if(nvram_param_read(NVCONF_SERVER_OTA, sServerURL) < 0)
        sprintf(sServerURL, "%s%s", OTA_SERVER_URL_DEF_HOST, OTA_SERVER_URL_TAIL);
    else
        strcpy(sServerURL + strlen(sServerURL), OTA_SERVER_URL_TAIL);

    dzlog_info("OTA base URL: %s", sServerURL);

    // random sleep up to
    //! Download udid.txt
    sprintf(url, "%s/%s", sServerURL, "udid.txt");
    sprintf(filepath, "/tmp/%s", "udid.txt");
    i32ret = donwload_file(url, filepath);
    if(i32ret != 0)
        {
        dzlog_error("Error: %s Download file fail", filepath);
        fw_for_dev = 0;
        }
    else
        {
        //! Get UDID
        if(nvram_param_read(NVCONF_UDID, udid) != 0)
            {
            fp = popen("ftest deviceid", "r");
            if(fp == NULL)
                {
                dzlog_error("Couldn't get UDID");
                return eOTAErrSystemErr;
                }
            if(fgets(udid, 32, fp) == NULL)
                return -3;
            pclose(fp);
            }

        fp = fopen(filepath, "r");
        if(fp == NULL)
            {
            dzlog_error("Couldn't read udid.txt");
            return eOTAErrSystemErr;
            }
        fw_for_dev = 0;
        while(fgets(buf, 128, fp) != NULL)
            {
            if(strncmp(buf, udid, 26) == 0)
                fw_for_dev = 1;
            }
        fclose(fp);
        remove(filepath);
        }

    //! Download version.txt
    if(fw_for_dev == 0)
        {
        dzlog_info("Its normal upgrade");
        sprintf(url, "%s/%s", sServerURL, "version.txt");
        sprintf(filepath, "%s/%s", CVISION_FW_UPGRADE_PKG, "version.txt");
        }
    else
        {
        dzlog_info("Its dev upgrade");
        fp = fopen("/tmp/dev_device.txt", "wb+");
        fclose(fp);
        sprintf(url, "%s/%s", sServerURL, "version_dev.txt");
        sprintf(filepath, "%s/%s", CVISION_FW_UPGRADE_PKG, "version_dev.txt");
        }

    i32ret = donwload_file(url, filepath);
    if(i32ret != 0)
        {
        dzlog_error("Error: %s Download file fail", filepath);
        return eOTAErrDownLoadFail;
        }

    //! Check version
    i32ret = ota_get_version(filepath, &ota_major, &ota_minor, &ota_revision);
    if(i32ret != 0)
        {
        dzlog_error("Error: get OTA version");
        return eOTAErrSystemErr;
        }

    dzlog_info("Last version: %02d.%02d.%02d", ota_major, ota_minor, ota_revision);
    i32ret = get_system_version(&sys_major, &sys_minor, &sys_revision);
    if(i32ret != 0)
        {
        dzlog_error("Error: get OTA version");
        return eOTAErrSystemErr;
        }
    dzlog_info("System version: %02d.%02d.%02d", sys_major, sys_minor, sys_revision);

    is_new_fw = 0;
    if(ota_major > sys_major)
        {
        is_new_fw = 1;
        }
    else if(ota_major == sys_major)
        {
        if(ota_minor > sys_minor)
            is_new_fw = 1;
        else if(ota_minor == sys_minor)
            {
            if(ota_revision > sys_revision)
                is_new_fw = 1;
            }
        }

    // Remove version text file
    remove(filepath);

    if(is_new_fw == 0)
        return eOTAErrFirmwareUpToDate;
    else
        {
        pc_tmp = ota_get_fw_filename(ota_major, ota_minor, ota_revision);
        sprintf(fwfilepath, CVISION_FW_UPGRADE_PKG"/%s", pc_tmp);
        pc_tmp = ota_get_sig_filename(ota_major, ota_minor, ota_revision);
        sprintf(filepath, CVISION_FW_UPGRADE_PKG"/%s", pc_tmp);

        //! Check fw ready downloaded
        if(access(fwfilepath, F_OK) != -1)
            {
            // file exists
            dzlog_info("File %s exist", fwfilepath);
            if(access(filepath, F_OK) != -1)
                {
                dzlog_info("File %s exist", filepath);
                //! Checksum
                i32ret = ota_is_fw_valid(fwfilepath, filepath);
                if(i32ret == 0)
                    {
                    dzlog_info("Upgrade package is ready");
                    fw_downloaded = 1;
                    }
                }
            }

        if(fw_downloaded == 0)
            {
            int retries = 3;
            while(retries--)
                {
                sprintf(url, "%s/%s", sServerURL, basename(fwfilepath));
                i32ret = donwload_file(url, fwfilepath);
                if(i32ret != 0)
                    {
                    dzlog_info("Error: %s Download file fail. Error code %d", url, i32ret);
                    goto remove_old_files;
                    }
                sprintf(url, "%s/%s", sServerURL, basename(filepath));
                i32ret = donwload_file(url, filepath);
                if(i32ret != 0)
                    {
                    dzlog_info("Error: %s Download file fail. Error code %d", url, i32ret);
                    goto remove_old_files;
                    }

                //! Check signature
                i32ret = ota_is_fw_valid(fwfilepath, filepath);
                if(i32ret != 0)
                    {
                    dzlog_error("Error: checksum not match");
                    goto remove_old_files;
                    }
                fw_downloaded = 1;
                break;
remove_old_files:
                remove(fwfilepath);
                remove(filepath);
                remove(OTA_FW_PACKAGES_FLAG);
                remove(OTA_FW_PACKAGES_SIGNATURE);
                remove(OTA_FW_PACK_DEC_FLAG);
                }
            }

        //! Trigger fwupgrade
        if(fw_downloaded == 1) // doublecheck whether upgrade already be called or not. If already no need to do anything
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
            dzlog_info("Mark the path digital signature %s\n", filepath);
            fprintf(fp, "%s\n", filepath);  // Write package name to OTA_FW_PACKAGES_FLAG file
            fclose(fp);

            return fw_upgrade_extra_function(ota_major, ota_minor, ota_revision);
            }
        else  // NO NEED TO DO ANYTHING
            {
            dzlog_error("Download retry timeout");
            return eOTAErrDownLoadFail;
            }
        }
    return eOTAErrDownLoadSuccess;
    }

int
main(int argc, char *argv[])
    {
    eOTAErrorCode eErrCode;
    int           iRetCode;
    uint32_t      prevTime = 0;
    uint32_t      curTime = 0;
    struct stat st = {0};

//    iRetCode = dzlog_init(CVISION_LOG_CONFIG_PATH, "OTA");
//    if(iRetCode)
//        printf("zlog init failed\n");

    openlog("[OTA_OLD]", LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Begin using syslog");

    dzlog_info("OTA for MODEL %s. Using secure HTTP", HUBBLE_CAMERA_MODEL_STRING);

    if (stat(CVISION_FW_UPGRADE_PKG, &st) == -1)
        {
        mkdir(CVISION_FW_UPGRADE_PKG, 0777);
        }

    //Work around solution
    //Delay OTA process for 10 minutes after camera have NTP
    sleep(600);

    /* Requirement : Only upgrade at night reboot mode and
     * the first 30 minutes. After that, exit ota_upgrade
     */
#if (HC_FAMILY_CAM_MODEL)
    eErrCode = ota_check_and_download();
    if(eErrCode == eOTAErrDownLoadSuccess)
        HubbleSignal(eHubbleHomeSignalFwUpgrade, eHubbleHomeGeneralStatusDownloaded);
#else
    prevTime = cameraSecondsTimeGet();
    while(1)
        {
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

    closelog ();
    return 0;
    }

