/*

   upload.exe  --  Upload a file to a server using forms.


   DESCRIPTION

       This is a CGI program to upload one or more files to a WWW
       server, using standard HTML forms instead of FTP. It works with
       Netscape 3.0 and 4.0, and Internet Explorer 4.0.

       See the manpage for more information.

   AUTHOR

       Jeroen C. Kessels
       Internet Engineer
       mailto:jeroen@kessels.com       http://www.kessels.com/
       Tel: +31(0)654 744 702


   COPYRIGHT

       Jeroen C. Kessels
       9 december 2000


   VERSION 2.6

*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifdef unix
#include <sys/stat.h>
#else
#include <io.h>
#ifdef _MSC_VER
#include <direct.h>
#else
#include <dir.h>
#endif
#endif
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>



#ifdef unix
#define stricmp         strcasecmp
#define strnicmp        strncasecmp
#endif



#define COPYRIGHT  "<center><font size=6><b>Upload v2.6</b></font><br>&copy; 2000 <a href='http://www.kessels.com/'>Jeroen C. Kessels</a></center>\n<hr>"
#define NO   0
#define YES  1
#define MUST 2



/* Define DIRSEP, the character that will be used to separate directories. */
#ifdef unix
#define DIRSEP "/\\"
#else
#define DIRSEP "\\/"
#endif
// define for full upgrade
#define STATE_NOT_READY 0
#define STATE_DOWNLOADING 1
#define STATE_EXTRACTING 2
#define STATE_BURNING 3
#define STATE_COMPLETE 4




//Justin
int *shmProgress;
int getSharedMem()
{
    int shmid;
    key_t key;
    //int *shm;
    int rtn;
    key = 5678;
    if ((shmid = shmget(key, 8, 0666)) < 0) {
      return -1;
    }
    if ((shmProgress = shmat(shmid, NULL, 0)) == (int *) -1) {
      return -1;
    }
    rtn = *shmProgress;
    return rtn;
}
//end Justin

int main(int argc, char *argv[], char *environment[]) {

	//ShowOkPage();
	//fprintf(stdout,"burning_process :%i",getSharedMem());
	//system("fwupgrade_percent 50");
	//printlog("HHHHHHHH\n");
	//fprintf(stderr,"WTF");
  	fprintf(stdout,"Content-type: text/html\n\n");
	fprintf(stdout,"burning_process :%d",getSharedMem());
  	exit(0);
}


/*
Ideeen:

- Multiple filemasks.
- Maximum directory size.
- Maximum and Minimum file size.
- Result-macros as parameters to OkUrl.

*/
