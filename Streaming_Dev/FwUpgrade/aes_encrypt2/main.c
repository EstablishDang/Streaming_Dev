#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "encrypt.h"

#define FIXED_KEY  "HubbleCvisionVN"
#define  CRYPTION_SIZE           512*1024 // 512KB

/* USAGE:
 * OUTPUT:
 * DESCRIPTION: this app will encrypt only first CRYPTION_SIZE kb
 * WARNING: this might fail with file has size smaller than CRYPTION_SIZE kb
 */
int main(int argc, char** argv)
{
    FILE *fpin;

    char buff[CRYPTION_SIZE] = {0};

    char cKeyModel[5] = {0}; // ex: 0082
    char cKeyVersion[9] = {0}; // ex:01.99.38
    char cDynamicKey[17] = {0};

    int n = 0, m, index;
    uint32_t iFileSize = 0;

    char *pInputFile = NULL;
    char cOutputFile[128] = {0};

    if (argc != 2 && argc != 3)
    {
        printf("%s [Firmware package - ext: .tar.gz] \n",argv[0]);  // ./aes_encrypt 1854-01.19.38.tar.gz
        printf("%s [Firmware package - ext: .txt] [Encrypt file] \n",argv[0]); // ./aes_encrypt 1854-01.19.38.txt zImage
        return -1;
    }

    for(index = strlen(argv[1]) - 1; index >= 0; index--)
    {
        if (argv[1][index] == '/')
            break;
    }
    index = (index < 0)? 0 : index + 1;

    if (argc == 2) // For case encrypt firmware package file
    {
        pInputFile = argv[1];

        memcpy(cOutputFile, argv[1], index);
        memcpy(cOutputFile + index + 0, argv[1] + index, 13);
        strcpy(cOutputFile + index + 13, ".fw.pkg2");
        // Output file would be renamed to .fw.pkg2 file
    }
    else if (argc == 3) // For case encrypt a single file
    {
        pInputFile = argv[2];

        sprintf(cOutputFile, "%s.%s", argv[2], "fw.pkg2");
        // Output file would be renamed, add .fw.pkg2 extension
    }

    fpin = fopen(pInputFile, "rb+");
    if (fpin == NULL)
    {
        fprintf(stderr,"ERROR opening file %s. Abort!\n", pInputFile);
        return -2;
    }

    fseek(fpin, 0L, SEEK_END);
    iFileSize = ftell(fpin);

    // Init key and IV
    memcpy(cKeyModel, argv[1] + index, 4);
    memcpy(cKeyVersion, argv[1] + index + 5, 8);
    snprintf(cDynamicKey, 17, "%4s%8sHUBL", cKeyModel, cKeyVersion);

    alawEncryptKeyInit(cDynamicKey, FIXED_KEY);
    alawSetEncryptKey();

    printf("Successfully Encrypt, %d\n", iFileSize);

    if (iFileSize < CRYPTION_SIZE)
    {
        n = fread(buff, sizeof(char), iFileSize, fpin);

    }
    else
    {
        n = fread(buff, sizeof(char), CRYPTION_SIZE, fpin);
    }

    fclose(fpin);

    alawEncrypt(buff, buff, n);

    fpin = fopen(pInputFile, "rb+");
    m = fwrite(buff, sizeof(char), n, fpin);
    fclose(fpin);

    rename(pInputFile, cOutputFile);

    printf("Output File Encrypted: %s\n", cOutputFile);
    return 0;
}
