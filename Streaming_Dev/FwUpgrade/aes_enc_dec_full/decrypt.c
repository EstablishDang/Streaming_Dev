/* FILE   : encrypt.c
 * DATE   : Jun 20, 2019
 * DESC   :
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "decrypt.h"
#include "aes.h"

#define DEF_AES_KEY_SIZE                16
#define DEF_AES_IV_SIZE                 16

static uint8_t               gAlawDataEncryptKey[DEF_AES_KEY_SIZE] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
static uint8_t               gAlawDataEncryptIV[DEF_AES_IV_SIZE] = {21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};



/*
 * PURPOSE : Fast encrypt an alaw frame
 * INPUT   : [key]         - AES Key
 *           [iv]          - AES Initialize vector
 *           [data]    - Frame lain data
 *           [length]  - Frame length
 * OUTPUT  : [data]    - Frame encrypted
 * RETURN  : The encrypted data length
 * DESCRIPT: None
 */
static size_t
alawDataDecrypt(uint8_t* key, uint8_t* iv, uint8_t* data, size_t length)
    {
    size_t  outLength = 0;
    uint8_t *outData = NULL;
    outLength = length;
    outData = aes_sw_decrypt(key, iv, data, length, &outLength);
    if(outLength <= length)
        {
        memcpy(data, outData, outLength);
        return outLength;
        }
    return -1;
    }

/*
 * PURPOSE : Wrapper function to encrypt a alaw frame
 * INPUT   : [frame]       - Pointer to frame data
 *           [frameLength] - Length of frame
 * OUTPUT  : None
 * RETURN  : Encrypted data length
 * DESCRIPT: This will encrypt directly from frame data
 */
size_t
alawDecryptWrapper(void* frame, size_t frameLength)
    {
    return alawDataDecrypt(gAlawDataEncryptKey, gAlawDataEncryptIV, frame, frameLength);
    }

/*
 * PURPOSE : Init key and iv vector use for encypt
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawDecryptKeyInit(char *key, char *iv)
    {
    /* Init key */
    bzero(gAlawDataEncryptKey, DEF_AES_KEY_SIZE);
    strcpy(gAlawDataEncryptKey, key);
    bzero(gAlawDataEncryptIV, DEF_AES_IV_SIZE);
    strcpy(gAlawDataEncryptIV, iv);
    return 0;
    }

