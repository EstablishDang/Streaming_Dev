#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mypublic.h"
#include "mbedtls/sha256.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#define SHA_STR_LENGTH 32

#define MAX_BUFFER 4096
#define KEY_SIZE   256

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
        printf( "failed mbedtls_ctr_drbg_seed returned %d", ret );
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
        printf("RSA Dec fail");
        ret = -1;
        }

    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
    mbedtls_rsa_free( &rsa );
    return ret;
    }

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
        printf("Can't open signature file\n");
        return -2;
        }
    i32ret = fread(enc_data, 1, KEY_SIZE, fp);
    fclose(fp);

    if (i32ret != KEY_SIZE)
        {
        printf("Signature file size isn't correct\n");
        return -3;
        }

    decrsa(enc_data, rsadec);

    if (memcmp(rsadec, cSHAorg, SHA_STR_LENGTH) == 0)
        {
        printf("Signature Matched\n");
        return 0;
        }
    else
        {
        printf("Signature Not Matched\n");
        return 1;
        }
    }

int main(int argc, char** argv)
{
    int ret;

    if (argc != 3)
    {
	printf("%s [source] [signature]\n",argv[0]);
	return -1;
    }
    ret = ota_is_fw_valid(argv[1],argv[2]);
    return ret;
}
