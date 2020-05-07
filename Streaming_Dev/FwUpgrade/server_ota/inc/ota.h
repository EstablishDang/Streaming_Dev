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

#if(DEF_SERVER_CERT_VERIFICATION)
#define OTA_SERVER_LOCATION_BASE    "https://ota.hubble.in/ota1/"HUBBLE_CAMERA_MODEL_STRING"_patch"
#else
#define OTA_SERVER_LOCATION_BASE    "http://ota.hubble.in/ota1/"HUBBLE_CAMERA_MODEL_STRING"_patch"
#endif /* __DEF_SERVER_CERT_VERIFICATION__ */

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

int ota_get_fw_filename(char* fw_url, char *result);
int ota_get_sig_filename(char* sig_url, char *result);
int ota_is_fw_valid(char * fwpath, char *md5path);

#endif /* __CVISION_FW_OTA_H__ */
