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
#include <stdint.h>
#include "decrypt.h"

#define  FIXED_KEY  "HubbleCvisionVN"
#define  CRYPTION_SIZE           512*1024 // 512KB

/* USAGE:
 * OUTPUT:
 * DESCRIPTION: this app will decrypt only first CRYPTION_SIZE kb
 * WARNING: this might fail with file has size smaller than CRYPTION_SIZE kb
 */
int main(int argc, char** argv)
{
    FILE *fpin;

    char buff[CRYPTION_SIZE] = {0};

    char cKeyModel[5] = {0};
    char cKeyVersion[9] = {0};
    char cDynamicKey[17] = {0};

    int n = 0, m, index;
    uint32_t iFileSize = 0;

    char *pInputFile = NULL;
    char cOutputFile[128] = {0};

    if (argc != 2 && argc != 3)
    {
        printf("%s [Firmware package - ext: .fw.pkg2] \n",argv[0]);  // ./aes_decrypt 1854-01.19.38.tar.gz
        printf("%s [Firmware package - ext: .txt] [Encrypt file - ext: .fw.pkg2] \n",argv[0]);  // ./aes_encrypt 1854-01.19.38.txt zImage.fw.pkg2
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
        pInputFile = argv[1];

        memcpy(cOutputFile, argv[1], index);
        memcpy(cOutputFile + index + 0, argv[1] + index, 13);
        strcpy(cOutputFile + index + 13, ".tar.gz");
        // Output file would be renamed to .tar.gz file
    }
    else if (argc == 3) // For case decrypt other files
    {
        pInputFile = argv[2];

        memcpy(cOutputFile, argv[2], strlen(argv[2]) - strlen(".fw.pkg2"));
        // Output file would be renamed to file removed the .fw.pkg2 extension
    }

    fpin = fopen(pInputFile, "rb+");
    if (fpin == NULL)
    {
        fprintf(stderr,"ERROR opening file %s. Abort!\n", pInputFile);
        return -2;
    }

    fseek(fpin, 0L, SEEK_END);
    iFileSize = ftell(fpin);

    memcpy(cKeyModel, argv[1] + index, 4);
    memcpy(cKeyVersion, argv[1] + index + 5, 8);
    snprintf(cDynamicKey, 17, "%4s%8sHUBL", cKeyModel, cKeyVersion);

    // Init key and IV
    alawDecryptKeyInit(cDynamicKey, FIXED_KEY);

    if (iFileSize < CRYPTION_SIZE)
    {
        n = fread(buff, sizeof(char), iFileSize, fpin);
    }
    else
    {
        n = fread(buff, sizeof(char), CRYPTION_SIZE, fpin);
    }

    fclose(fpin);

    m = alawDecryptWrapper(buff, n);
    if (m != n)
    {
        fprintf(stderr, "Error Decrypt");
        fclose(fpin);
        return -3;
    }

    fpin = fopen(pInputFile, "rb+");
    m = fwrite(buff, sizeof(char), n, fpin);
    fclose(fpin);

    rename(pInputFile, cOutputFile);



    printf("Successfully Decpypt\n");
    printf("Output File Decrypted: %s\n", cOutputFile);
    return 0;
}
