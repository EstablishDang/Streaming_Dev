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

/* FILE   : otc.h
 * AUTHOR : Lap Dang ☻
 * DATE   : Jan 7, 2019
 * DESC   : Firmware OTA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mbedtls/sha256.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "fwpath.h"
#include "mypublic.h"
#include "ota.h"
#include "nvconf.h"
#include "zlog.h"

#define VERSION_SEPERATE "."
#define MAX_BUFFER 4096
#define KEY_SIZE   256

#if (FULL_ENCRYPTION_FW_PKG)
#if (DEF_SHA512_FOR_DIGITAL_SIGNATURE)
#define SIGNATURE_EXTENSION                 ".sha512"
#else
#define SIGNATURE_EXTENSION                 ".sig2"
#endif
#else
#define SIGNATURE_EXTENSION                 ".sig"
#endif

#if (DEF_SHA512_FOR_DIGITAL_SIGNATURE)
#define SHA_STR_LENGTH 64
#else
#define SHA_STR_LENGTH 32
#endif

int ota_get_version(char * filepath, int *major, int *minor, int *revision)
{
    char buffer[64];
    char *version = NULL;
    char fw_ver[64];
    FILE *fp = fopen(filepath, "r");
    if(fp == NULL)
        return -1;

    if(fgets(buffer, sizeof(buffer) - 1, fp) == NULL)
    {
        fclose(fp);
        return -2;
    }

    version = strstr(buffer, "=");
    if(version)
    {
        version += 1;
        printf("%s : %s", __FUNCTION__, buffer);
        sscanf(version, "%02d"VERSION_SEPERATE"%02d"VERSION_SEPERATE"%02d",
               major, minor, revision);
        snprintf(fw_ver, sizeof(fw_ver), "%02d"VERSION_SEPERATE"%02d"VERSION_SEPERATE"%02d",
                *major, *minor, *revision);
        nvram_param_write(NVCONF_FW_UPGRADE_VERSION, fw_ver);  // Used to keep the lastest version on server
        printf("%s : %s", __FUNCTION__, fw_ver);
        fclose(fp);
        return 0;
    }
    else
    {
        fclose(fp);
        return -3;
    }
}

int get_system_version(int *major, int *minor, int *revision)
    {
    int iret = 0;
    char buf[128];

    iret = nvram_param_read(NVCONF_FW_VERSION, buf);
    if(iret == 0)
        {
        iret = sscanf(buf, "%d"VERSION_SEPERATE"%d"VERSION_SEPERATE"%d",
                      major, minor, revision);
        if(iret == EOF)
            iret = -1;
        else
            iret = 0;
        }
    return iret;
    }

char* ota_get_fw_filename(int major, int minor, int revision)
    {
    static char buffer[FILENAME_MAX];

#if (FULL_ENCRYPTION_FW_PKG)
    sprintf(buffer, HUBBLE_CAMERA_MODEL_STRING"-%02d"VERSION_SEPERATE"%02d"VERSION_SEPERATE"%02d.fw.pkg", major, minor, revision);
#else
    sprintf(buffer, HUBBLE_CAMERA_MODEL_STRING"-%02d"VERSION_SEPERATE"%02d"VERSION_SEPERATE"%02d.fw.pkg2", major, minor, revision);
#endif
    return buffer;
    }

char* ota_get_sig_filename(int major, int minor, int revision)
    {
    static char buffer[FILENAME_MAX];

    sprintf(buffer, HUBBLE_CAMERA_MODEL_STRING"-%02d"VERSION_SEPERATE"%02d"VERSION_SEPERATE"%02d%s", major, minor, revision, SIGNATURE_EXTENSION);

    return buffer;
    }

static int decrsa(unsigned char* encdata, unsigned char* decdata)
    {
    int ret, i;
    int size_enc;
    mbedtls_rsa_context rsa;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    char RSA_N[260] = {0};
    char RSA_E[8] = {0};
    const unsigned char *pers = "rsa_decrypt";

    size_enc = strlen((const char*)encdata);
    mbedtls_rsa_init( &rsa, MBEDTLS_RSA_PKCS_V15, 0 );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );

    ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                                        &entropy, pers,
                                        strlen( pers ) );
    if(ret != 0)
        {
        dzlog_error( "failed mbedtls_ctr_drbg_seed returned %d", ret );
        mbedtls_ctr_drbg_free( &ctr_drbg );
        mbedtls_entropy_free( &entropy );
        mbedtls_rsa_free( &rsa );
        return -1;
        }

    for(i=0; i < mymodul_len; i++)
        {
        sprintf(&RSA_N[i*2], "%02X", mymodul_buf[i]);
        }

    for(i=0; i < mypublic_len; i++)
        {
        sprintf(&RSA_E[i*2], "%02X", mypublic_buf[i]);
        }

    mbedtls_mpi_read_string( &rsa.N , 16, RSA_N  );
    mbedtls_mpi_read_string( &rsa.E , 16, RSA_E  );
    rsa.len = mymodul_len;
    ret = mbedtls_rsa_pkcs1_decrypt( &rsa, mbedtls_ctr_drbg_random,
                                            &ctr_drbg, MBEDTLS_RSA_PUBLIC, &size_enc,
                                            encdata, decdata, 2048 );
    if(ret != 0)
        {
        dzlog_error("RSA Dec fail");
        ret = -1;
        }

    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
    mbedtls_rsa_free( &rsa );
    return ret;
    }

#if (DEF_SHA512_FOR_DIGITAL_SIGNATURE)
int ota_is_fw_valid(char * fwpath, char *cSigPath)
{
    int i32ret = 0;
    unsigned char cSHAorg[SHA_STR_LENGTH] = {0};
    unsigned char enc_data[KEY_SIZE] = {0};
    unsigned char rsadec[KEY_SIZE] = {0};
    unsigned char buffer[MAX_BUFFER];
    int size;
    FILE *fp = NULL;
    mbedtls_sha512_context ctx;

    memset(buffer, 0, MAX_BUFFER);

    fp = fopen(fwpath,"rb");
    if(fp == NULL)
        {
        return -1;
        }

    mbedtls_sha512_init( &ctx );
    mbedtls_sha512_starts(&ctx, 0);
    while((size = fread(buffer, 1, MAX_BUFFER, fp)) > 0)
        {
        mbedtls_sha512_update( &ctx, buffer, size );
        }

    mbedtls_sha512_finish( &ctx, cSHAorg );
    mbedtls_sha512_free( &ctx );
    fclose(fp);

    fp = fopen(cSigPath, "rb");
    if(fp == NULL)
        {
        dzlog_error("Cant open Signature File\n");
        return -2;
        }
    i32ret = fread(enc_data, 1, KEY_SIZE, fp);
    fclose(fp);

    if (i32ret != KEY_SIZE)
        {
        dzlog_info("Signature File Size isnt correct\n");
        return -3;
        }

    decrsa(enc_data, rsadec);

    if (memcmp(rsadec, cSHAorg, SHA_STR_LENGTH) == 0)
        {
        dzlog_info("Signature Matched");
        return 0;
        }
    else
        {
        dzlog_error("Signature Not Matched");
        return 1;
        }
    return 1;
    }
#else
int ota_is_fw_valid(char * fwpath, char *cSigPath)
{
    int i32ret = 0;
    unsigned char cSHAorg[SHA_STR_LENGTH] = {0};
    unsigned char enc_data[KEY_SIZE] = {0};
    unsigned char rsadec[KEY_SIZE] = {0};
    unsigned char buffer[MAX_BUFFER];
    int size;
    FILE *fp = NULL;
    mbedtls_sha256_context ctx;

    memset(buffer, 0, MAX_BUFFER);

    fp = fopen(fwpath,"rb");
    if(fp == NULL)
        {
        return -1;
        }

    mbedtls_sha256_init( &ctx );
    mbedtls_sha256_starts(&ctx, 0);
    while((size = fread(buffer, 1, MAX_BUFFER, fp)) > 0)
        {
        mbedtls_sha256_update( &ctx, buffer, size );
        }

    mbedtls_sha256_finish( &ctx, cSHAorg );
    mbedtls_sha256_free( &ctx );
    fclose(fp);

    fp = fopen(cSigPath, "rb");
    if(fp == NULL)
        {
        dzlog_error("Cant open Signature File\n");
        return -2;
        }
    i32ret = fread(enc_data, 1, KEY_SIZE, fp);
    fclose(fp);

    if (i32ret != KEY_SIZE)
        {
        dzlog_info("Signature File Size isnt correct\n");
        return -3;
        }

    decrsa(enc_data, rsadec);

    if (memcmp(rsadec, cSHAorg, SHA_STR_LENGTH) == 0)
        {
        dzlog_info("Signature Matched");
        return 0;
        }
    else
        {
        dzlog_error("Signature Not Matched");
        return 1;
        }
    return 1;
    }
#endif
