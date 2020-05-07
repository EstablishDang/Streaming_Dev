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

/* FILE   : ota.h
 * AUTHOR : Lap Dang ☻
 * DATE   : Jan 7, 2019
 * DESC   : Firmware OTA
 */

#ifndef __CVISION_FW_OTA_H__
#define __CVISION_FW_OTA_H__
#include "fwpath.h"

#define OTA_SERVER_URL_DEF_HOST     "https://ota.hubble.in"

#ifndef OTA_SERVER_URL_TAIL
#define OTA_SERVER_URL_TAIL         "/ota1/"HUBBLE_CAMERA_MODEL_STRING"_patch"
#endif

typedef enum eOTAErrorCode
{
    eOTAErrDownLoadSuccess,
    eOTAErrDownLoadFail,
    eOTAErrNetworkFail,
    eOTAErrFirmwareUpToDate,
    eOTAErrFirmwareUpgrading,
    eOTAErrSystemErr,
    eOTAErrInvalid,
} eOTAErrorCode;

int
ota_get_version(char *filepath, int *major, int *minor, int *revision);

int
get_system_version(int *major, int *minor, int *revision);

char*
ota_get_fw_filename(int major, int minor, int revision);

char*
ota_get_md5_filename(int major, int minor, int revision);

int
ota_is_fw_valid(char *fwpath, char *md5path);

#endif /* __CVISION_FW_OTA_H__ */
