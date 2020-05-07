/* FILE   : aes.c
 * AUTHOR : leon
 * DATE   : Dec 13, 2013
 * DESC   : Contain all hanlde about AES encrypt
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <stdint.h>
#include <fcntl.h>

#include "aes_cryptodev.h"
#include "aes_sw.h"
#include "aes.h"



/******************************************************************
                          TYPE DEFINE
 ******************************************************************/
typedef struct tAESInternalBuf
    {
    void*   data;
    size_t  size;
    }tAESInternalBuf;


/******************************************************************
                          STATIC VARIABLE
 ******************************************************************/
static char             encryptMode[16] = {'c', 'b', 'c', 0};
static tAESInternalBuf  hwEncBuffer = {0, 0};
static tAESInternalBuf swDecBuf   = {0, 0};

/*
 * PURPOSE : Encrypt data with specify key using hardware engine
 * INPUT   : [key]     - aes key
 *           [iv]      - initialize vector
 *           [dataIn]  - data need to encrypt
 *           [iSize]   - size of data input
 * OUTPUT  : [oSize]   - size of data encrypted
 * RETURN  : Pointer to encrypted data
 * DESCRIPT: Wrapper function. User must free return pointer after use
 */
uint8_t*
aes_hw_encrypt(uint8_t* key, uint8_t* iv, uint8_t* dataIn, size_t iSize, size_t* oSize)
    {
    aes_cryptodev_ctx hw_ctx;

    //Check input params
    if(key == NULL || dataIn == NULL)
        {
//        mylog_error("[AES_HW] - Input params is NULL");
        *oSize = 0;
        return NULL;
        }

    //Initialize hardware AES engine
    if(!aes_cryptodev_init(&hw_ctx, key, (_AES_MAX_KEY_SIZE_ / 8), encryptMode))
        {
        *oSize = 0;
        return NULL;
        }

    //Prepare buffer for data encrypted
    if((iSize % _AES_BLK_SIZE_) != 0)
        (*oSize) = ((iSize / 16) + 1) * 16;
    else
        (*oSize) = iSize;

    /* Prepare buffer */
    if(hwEncBuffer.size < (*oSize))
        {
        hwEncBuffer.data = (void*)realloc(hwEncBuffer.data, sizeof(uint8_t) * (*oSize));
        if(hwEncBuffer.data != NULL)
            hwEncBuffer.size = (*oSize);
        }

    if(hwEncBuffer.data != NULL)
        {
        memset(hwEncBuffer.data, 0, (*oSize));
        memcpy(hwEncBuffer.data, dataIn, (*oSize));
        //Encrypt using hardware
        if(!aes_cryptodev_encrypt(&hw_ctx, iv, hwEncBuffer.data, hwEncBuffer.data, (*oSize)))
            {
            *oSize = 0;
            return NULL;
            }
        }
    aes_cryptodev_fini(&hw_ctx);
    return hwEncBuffer.data;
    }

/*
 * PURPOSE : Decrypt data with specify key using software engine
 * INPUT   : [key]     - aes key
 *           [iv]      - initialize vector
 *           [dataIn]  - data need to decrypt
 *           [iSize]   - size of data input
 * OUTPUT  : [oSize]   - size of data decrypted
 * RETURN  : Pointer to decrypted data
 * DESCRIPT: Wrapper function. User must free return pointer after use
 */
uint8_t*
aes_sw_decrypt(uint8_t* key, uint8_t* iv, uint8_t* dataIn, size_t iSize, size_t* oSize)
    {
    aes_context sw_ctx;

    //Check input params
    if(key == NULL || dataIn == NULL)
        {
        //mylog_error("[AES_SW] - Input params is NULL");
        *oSize = 0;
        return NULL;
        }

    //Initialize software AES engine
    if(aes_set_key(&sw_ctx, key, _AES_MAX_KEY_SIZE_, encryptMode))
        {
        //mylog_error("[AES_SW] - aes_set_key failed");
        *oSize = 0;
        return NULL;
        }

    //Prepare buffer
    if(swDecBuf.size < (*oSize))
        {
        swDecBuf.data = (void*)realloc(swDecBuf.data, sizeof(uint8_t) * (*oSize));
        if(swDecBuf.data != NULL)
            swDecBuf.size = (*oSize);
        }

    if(swDecBuf.data != NULL)
        {
        memset(swDecBuf.data, 0, (*oSize));
        memcpy(swDecBuf.data, dataIn, iSize);
        //Decrypt using software
        aes_decrypt(&sw_ctx, iv, swDecBuf.data, iSize);
	*oSize = iSize;
        }
    return swDecBuf.data;
    }

/******************************************************************
  PUBLIC FUNCTION
 ******************************************************************/

/*
 * PURPOSE : Encrypt data with specify key using software engine
 * INPUT   : [key]     - aes key
 *           [iv]      - initialize vector
 *           [dataIn]  - data need to encrypt
 *           [iSize]   - size of data input
 * OUTPUT  : [oSize]   - size of data encrypted
 * RETURN  : Pointer to encrypted data
 * DESCRIPT: Wrapper function. User must free return pointer after use
 */
uint8_t*
aes_sw_encrypt(uint8_t* key, uint8_t* iv, uint8_t* dataIn, size_t iSize, size_t* oSize)
    {
    uint8_t     *encBuf = NULL;
    aes_context sw_ctx;

    //Check input params
    if(key == NULL || dataIn == NULL)
        {
        *oSize = 0;
        return NULL;
        }

    //Initialize software AES engine
    if(aes_set_key(&sw_ctx, key, _AES_MAX_KEY_SIZE_, encryptMode))
        {
        *oSize = 0;
        return NULL;
        }

    //Prepare buffer for data encrypted
    if((iSize % _AES_BLK_SIZE_) != 0)
        (*oSize) =  ((iSize / 16) + 1) * 16;
    else
        (*oSize) = iSize;

    encBuf = (uint8_t*) malloc(sizeof(uint8_t) * (*oSize));
    if(encBuf != NULL)
        {
        memset(encBuf, 0, (*oSize));
        memcpy(encBuf, dataIn, iSize);
        //Encrypt using software
        aes_encrypt(&sw_ctx, iv, encBuf, *(oSize));
        }
    return encBuf;
    }
