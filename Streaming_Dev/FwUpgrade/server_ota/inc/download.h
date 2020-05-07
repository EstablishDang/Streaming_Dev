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

/* FILE   : download.h
 * AUTHOR : Lap Dang ☻
 * DATE   : Jan 7, 2019
 * DESC   : Firmware download from server using curl
 */

#ifndef __CVISION_FW_OTA_DOWNLOAD_H__
#define __CVISION_FW_OTA_DOWNLOAD_H__

int donwload_file(char * url, char * filepath);

#endif //__CVISION_FW_OTA_DOWNLOAD_H__
