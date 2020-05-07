/* FILE   : encrypt.c
 * DATE   : Jun 20, 2019
 * DESC   :
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/aes.h>

#define DEF_AES_KEY_SIZE                16
#define DEF_AES_IV_SIZE                 16

static uint8_t               gAlawDataEncryptKey[DEF_AES_KEY_SIZE] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
static uint8_t               gAlawDataEncryptIV[DEF_AES_IV_SIZE] = {21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};
static AES_KEY enc_key;

/*
 * PURPOSE : Init key and iv vector use for encypt
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawEncryptKeyInit(char *key, char *iv)
{
    /* Init key */
    bzero(gAlawDataEncryptKey, DEF_AES_KEY_SIZE);
    strcpy(gAlawDataEncryptKey, key);

    /* Init IV */
    bzero(gAlawDataEncryptIV, DEF_AES_IV_SIZE);
    strcpy(gAlawDataEncryptIV, iv);
    return 0;
}

/*
 * PURPOSE : Set encrypt key and iv vector use for encypt
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawSetEncryptKey()
{
    AES_set_encrypt_key(gAlawDataEncryptKey, 128, &enc_key);
    return 0;
}

/*
 * PURPOSE : Set encrypt key and iv vector use for encypt
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawEncrypt(const unsigned char *in, unsigned char *out, const unsigned long length)
{
    AES_cbc_encrypt(in, out, length, &enc_key, gAlawDataEncryptIV, AES_ENCRYPT);
    return 0;
}
