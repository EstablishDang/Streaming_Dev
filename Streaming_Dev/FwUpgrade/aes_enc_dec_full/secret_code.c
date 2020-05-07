/* FILE   : secret_code.c
 * DATE   : Dec 7, 2015
 * DESC   : generate secret code based on MAC address
 */

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stddef.h>

#ifndef PC_MODE
#include "nxcNetwork.h"
#include "nxcGenUDID.h"
#endif

#ifdef PC_MODE
int getMacAddr(char *macAddress)
{
    struct ifreq ifr;
    struct ifconf ifc;

    char buf[1024];
    unsigned char mac_address[6];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { /* handle error*/ };

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */ }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it) {
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    success = 1;
                    break;
                }
            }
        }
        else { /* handle error */ }
    }

    if (success)
    {
        memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
        sprintf(macAddress, "%02X%02X%02X%02X%02X%02X",
                mac_address[0],
                mac_address[1],
                mac_address[2],
                mac_address[3],
                mac_address[4],
                mac_address[5]);
        return 0;
    }
    return 1;
}

int genUDID(unsigned short chrTypeNum, char* usModelNum, char* strMAC, char* strUDID)
{
    int retVal  = 0;

    static unsigned int mnttable[] = {7,3,7,3,7,2,5,6};

    char result[9] = {0};
    char tmp[3] = {0};
    unsigned short a1,a2,a3,a4,a5,a6,m1,m2,a,b,c,d,e,f,g,h;

    if(strMAC == NULL || strUDID == NULL || usModelNum == NULL){
        return -1;
    }

    tmp[2]=0;
    tmp[0]= usModelNum[0];
    tmp[1]= usModelNum[1];
    m1 = strtoul(tmp,NULL,16);

    tmp[0]= usModelNum[2];
    tmp[1]= usModelNum[3];
    m2 = strtoul(tmp,NULL,16);

    tmp[0]=strMAC[0];
    tmp[1]=strMAC[1];
    a6 = strtoul(tmp,NULL,16);

    tmp[0]=strMAC[2];
    tmp[1]=strMAC[3];
    a5 = strtoul(tmp,NULL,16);

    tmp[0]=strMAC[4];
    tmp[1]=strMAC[5];
    a4 = strtoul(tmp,NULL,16);

    tmp[0]=strMAC[6];
    tmp[1]=strMAC[7];
    a3 = strtoul(tmp,NULL,16);

    tmp[0]=strMAC[8];
    tmp[1]=strMAC[9];
    a2 = strtoul(tmp,NULL,16);

    tmp[0]=strMAC[10];
    tmp[1]=strMAC[11];
    a1 = strtoul(tmp,NULL,16);

    a = (unsigned short)(((a1+a5) * mnttable[0]) % 26) % 25 + 'A';
    b = (unsigned short)(((a2+a5) * mnttable[1]) % 26) % 25 + 'A';
    c = (unsigned short)(((a3+a5) * mnttable[2]) % 26) % 25 + 'A';
    d = (unsigned short)(((a4+a5) * mnttable[3]) % 26) % 25 + 'A';
    e = (unsigned short)(((a5+a6) * mnttable[4]) % 26) % 25 + 'A';
    f = (unsigned short)(((a3+a4) * mnttable[5]) % 26) % 25 + 'A';
    g = (unsigned short)(((a3+m2) * mnttable[6]) % 26) % 25 + 'A';
    h = (unsigned short)(((chrTypeNum + m1 + m2 + a1 + a2) * mnttable[7]) % 26)  % 25  + 'A';

    sprintf(result,"%c%c%c%c%c%c%c%c",a,b,c,d,e,f,g,h);
    sprintf(strUDID,"%02d",chrTypeNum);
    strcat(strUDID,usModelNum);
    strcat(strUDID,strMAC);
    strcat(strUDID,result);

    return retVal;
}
#endif

void macGenSecretCode(unsigned char * input, int inlength, unsigned char *output)
{
    unsigned char temp[16] = { 0 };
    unsigned char index;

    for (index = 0; index < 16; index++)
        temp[index] = 0;

    index = 0;
    while ((index < 16) && (index < inlength))
    {
        temp[index] = input[index];
        index++;
    }

    temp[0] = temp[0] ^ 0xFA;
    temp[1] = temp[1] ^ 0xAB ^ temp[0];
    temp[2] = temp[2] ^ 0x12 ^ temp[1];
    temp[3] = temp[3] ^ 0x33 ^ temp[2];
    temp[4] = temp[4] ^ 0x45 ^ temp[3];
    temp[5] = temp[5] ^ 0xFF ^ temp[4];
    temp[6] = temp[6] ^ 0x4B ^ temp[5];
    temp[7] = temp[7] ^ 0x57 ^ temp[6];
    temp[8] = temp[8] ^ 0x22 ^ temp[7];
    temp[9] = temp[9] ^ 0x91 ^ temp[8];
    temp[10] = temp[10] ^ 0x9A ^ temp[9];
    temp[11] = temp[11] ^ 0xC7 ^ temp[10];
    temp[12] = temp[12] ^ 0x80 ^ temp[11];
    temp[13] = temp[13] ^ 0x1F ^ temp[12];
    temp[14] = temp[14] ^ 0xD2 ^ temp[13];
    temp[15] = temp[15] ^ 0x6E ^ temp[14];
    for (index = 0; index < 16; index++)
        output[index] = temp[index] + 7;
    output[16] = 0;
}

int generateSecretCode(char *output)
{
    unsigned char macin[13]; // length <=16+1

#ifdef PC_MODE
    if (getMacAddr(macin) != NULL)
#else
    if (nxcNetworkGetMacAddr(macin) != NULL)
#endif
        macGenSecretCode(macin, 12, output);
    else
        return 1;
    return 0;
}
