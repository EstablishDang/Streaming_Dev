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


#include <stdio.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <unistd.h>

#include "download.h"
#include "fwpath.h"
#include "zlog.h"
#include "nxcHubbleAPI.h"


static size_t
write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
    }

int
donwload_file(char * url, char * filepath)
    {
    int i32ret = 0, retries = 2, resume = 0, offset = 0;
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char userAgent[20] = {0};
    nxcHubbleGetUserAgent(userAgent);
    curl = curl_easy_init();
    if(curl == NULL)
        return -1;

    if(access(filepath, F_OK) == 0)
        {
        fp = fopen(filepath, "rb");
        if(fp)
            {
            fseek(fp, 0, SEEK_END);
            offset = ftell(fp);
            fclose(fp);

            dzlog_info("Resume download file(%s) at offset(%d)", filepath, offset);

            /* Flag for resuming previous download */
            resume = 1;
            }
        }

    fp = fopen(filepath, (resume ? "ab" : "wb"));
    if(fp == NULL)
        {
        dzlog_error("Error: open %s for write\n", filepath);
        curl_easy_cleanup(curl);
        return -1;
        }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent);
#if(DEF_SERVER_CERT_VERIFICATION)
    if(access(CERTIFICATE_HUBBLE_PATH, F_OK))
        {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        }
    else
        {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
        curl_easy_setopt(curl, CURLOPT_CAINFO, CERTIFICATE_HUBBLE_PATH);
        dzlog_info("Verify server certificate OTA");
        }
#else
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
#endif
    curl_easy_setopt(curl, CURLOPT_RESUME_FROM , offset);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 120);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    while(retries > 0)
        {
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            {
#if(DEF_SERVER_CERT_VERIFICATION)
            if(res == CURLE_PEER_FAILED_VERIFICATION && !access(CERTIFICATE_HUBBLE_PATH, F_OK))
                {
                dzlog_error("Verify peer failure, change using backup certificates");
                if(unlink(CERTIFICATE_HUBBLE_PATH) || symlink(CERTIFICATE_BUNDLE_PATH, CERTIFICATE_HUBBLE_PATH))
                    {
                    dzlog_error("Can't using backup CAs certificate to verify peer");
                    i32ret = -1;
                    break;
                    }
                }
#endif
            dzlog_info("Error %d : %s\n", res, curl_easy_strerror(res));
            i32ret = -1;
            if(res == CURLE_COULDNT_RESOLVE_HOST)
                {
                res_init();
                rewind(fp);
                retries--;
                }
            else
                retries = 0;
            }
        else
            {
            retries = 0;
            i32ret = 0;
            }
        }
    curl_easy_cleanup(curl);
    fflush(fp);
    fclose(fp);

#if 0
    if(i32ret != 0)
        remove(filepath);
#endif

    return i32ret;
    }

