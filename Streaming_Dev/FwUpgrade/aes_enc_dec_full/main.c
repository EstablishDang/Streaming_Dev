/*
* Copyright (C) 2019 CVision
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "encrypt.h"
#include "decrypt.h"

#define  FIXED_KEY  "HubbleCvisionVN"
#define  CRYPTION_SIZE           1024 // 512KB

#define BUILD_DECRYPT   1 // Build decrypt app by default, change when want to build encrypt app
#define BUILD_ENCRYPT   0

int main(int argc, char** argv)
{
    FILE *fpin;
    FILE *fpout;

    char buff[CRYPTION_SIZE] = {0};

    char cKeyModel[5] = {0};
    char cKeyVersion[9] = {0};
    char cDynamicKey[17] = {0};

    int n = 0, m, index;
    int iFileSize = 0;

    char *pExtension = NULL;
    char cInputFile[128] = {0};
    char cOutputFile[128] = {0};
    int type = 0;
    if (argc < 2 || argc > 4)
    {
        printf("%s [Firmware package - ext: .fw.pkg] \n", argv[0]);  // ./aes_decrypt 1854-01.19.38.tar.gz
        printf("%s [Firmware package - ext: .txt] [Encrypt file - ext: .fw.pkg] \n", argv[0]);  // ./aes_encrypt 1854-01.19.38.txt zImage.fw.pkg2
        return -1;
    }

    for(index = strlen(argv[1]) - 1; index >= 0; index--)
    {
        if (argv[1][index] == '/')
            break;
    }
    index = (index < 0)? 0 : index + 1;

    if (argc == 2) // For case decrypt firmware package file
    {
        strcpy(cInputFile, argv[1]);

#if (BUILD_DECRYPT)
        pExtension = strstr(argv[1], ".fw.pkg");
        if(pExtension)
        {
            *pExtension = 0x00;

            strcpy(cOutputFile, argv[1]);
            strcpy(cOutputFile + strlen(cOutputFile), ".tar.gz");
            printf("Output file name: %s\n", cOutputFile);
        }
#else
        pExtension = strstr(argv[1], ".tar.gz");
        pExtension = 0x00;

        strcpy(cOutputFile, argv[1]);
        strcpy(cOutputFile + strlen(cOutputFile), ".fw.pkg");
#endif
    }
    else if (argc == 3) // For case decrypt other files
    {
        strcpy(cInputFile, argv[2]);
#if (BUILD_DECRYPT)
        pExtension = strstr(argv[2], ".fw.pkg");
        if(pExtension)
        {
            *pExtension = 0x00;
            strcpy(cOutputFile, argv[2]);
            printf("Output file name: %s\n", cOutputFile);
        }
#else
        sprintf(cOutputFile, "%s.%s", argv[2], "fw.pkg");
#endif

        // Output file would be renamed to file removed the .fw.pkg2 extension
    }

    fpin = fopen(cInputFile, "rb+");
    if (fpin == NULL)
    {
        fprintf(stderr,"ERROR: Opening input file (%s) failed. Abort!\n", cInputFile);
        return -2;
    }

    memcpy(cKeyModel, argv[1] + index, 4);
    memcpy(cKeyVersion, argv[1] + index + 5, 8);

    snprintf(cDynamicKey, 17, "%4s%8sHUBL", cKeyModel, cKeyVersion);

#if (BUILD_DECRYPT)
    alawDecryptKeyInit(cDynamicKey, FIXED_KEY);
#else
    alawEncryptKeyInit(cDynamicKey, FIXED_KEY);
#endif

    fpout = fopen(cOutputFile, "wb+");
    if (fpout == NULL)
    {
        fprintf(stderr,"ERROR: Opening output file (%s) failed. Abort!\n", cOutputFile);
        return -2;
    }

    while((n = fread(buff, 1, CRYPTION_SIZE, fpin)) > 0)
    {
        if (n < CRYPTION_SIZE)
        {
            m = fwrite(buff, 1, n, fpout);
            if (m < n)
            {
                printf("Error Writing - in %d - out %d\n", n, m);
                fclose(fpin);
                fclose(fpout);
                return -1;
            }
            break;
        }

#if (BUILD_DECRYPT)
        m = alawDecryptWrapper(buff, n);
#else
        m = alawEncryptWrapper(buff, n);
#endif
        if (m != n)
        {
            printf("Error %s\n", type ? "Encrypt" : "Decrypt");
            fclose(fpin);
            fclose(fpout);
            return -1;
        }

        m = fwrite(buff, 1, n, fpout);
        if (m < n)
        {
            printf("Error Writing - in %d - out %d\n", n, m);
            fclose(fpin);
            fclose(fpout);
            return -1;
        }
    }

    fclose(fpin);
    fclose(fpout);

#if (BUILD_DECRYPT)
    unlink(cInputFile);
#endif

    return 0;
}
