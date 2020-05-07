#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include "hubble_sdk.h"
#include "nvconf.h"
#include "camera_setting.h"
#include "hubble_common_api.h"

/*
 * PURPOSE : Main application. This just setup HTTP server
 *           and register callback function
 * INPUT   : None
 * OUTPUT  : None
 * RETURN  : None
 * DESCRIPT: None
 */
int main(int argc, const char *argv[])
{
    int ret = 0;
    char filename[64] ={0};
    char value_nvconf[128] ={0};
    CAM_SDK camera;

    bzero(&camera, sizeof(camera)); // this is important
#if 0
    strcpy(camera.camInfo.masterkey, "");
    strcpy(camera.camInfo.mac,"000AE2107E64");
    strcpy(camera.camInfo.model, "0854");
    strcpy(camera.camInfo.version, "03.10.10");
    //strcpy(camera.camInfo.udid, "011854000AE2107E64QSAGSQGI"); >>not correct with novouton platform when genUDID
    strcpy(camera.camInfo.serverURL, "cs.hubble.in");
    camera.camInfo.serverPort = 443;
#endif
#if 0   //camera 1
    //strcpy(camera.camInfo.masterkey, "vZPoBd8go0oDgyos");
    strcpy(camera.camInfo.masterkey, "");
    strcpy(camera.camInfo.mac ,"000C4385110A");
    strcpy(camera.camInfo.model, "0091");
    strcpy(camera.camInfo.version, "03.10.02");
    //strcpy(camera.camInfo.udid, "010091000C4385110AYJBDGKMY");
    strcpy(camera.camInfo.serverURL, "cs.hubble.in");
    camera.camInfo.serverPort = 443;
#endif

#if 0  // camera 2
    strcpy(camera.camInfo.masterkey, "");
    strcpy(camera.camInfo.mac ,"000C43750883");
    strcpy(camera.camInfo.model, "0073");
    strcpy(camera.camInfo.version, "03.10.10");
    //strcpy(camera.camInfo.udid, "010073000C43750883NITDGEQW");
    strcpy(camera.camInfo.serverURL, "cs.hubble.in");
    camera.camInfo.serverPort = 443;
#endif

#if 0 //camera 3
    strcpy(camera.camInfo.masterkey, "");
    strcpy(camera.camInfo.mac ,"000C43750887");
    strcpy(camera.camInfo.model, "0073");
    strcpy(camera.camInfo.version, "03.10.10");
    //strcpy(camera.camInfo.udid, "010073000C43750887PITDGEQU");
    strcpy(camera.camInfo.serverURL, "cs.hubble.in");
    camera.camInfo.serverPort = 443;
    strcpy(camera.camInfo.mqtts_url, "");
#endif
    strcpy(camera.camInfo.masterkey, "");
    camera.camInfo.serverPort = 443;
    strcpy(camera.camInfo.mqtts_url, "");

    if(nvram_param_read(NVCONF_MAC_ADDRESS, camera.camInfo.mac) < 0 )
	{
		syslog(LOG_ERR, "cannot read mac address");
	}

    if(nvram_param_read(NVCONF_MODEL, camera.camInfo.model) < 0 )
	{
		syslog(LOG_ERR, "cannot read camera model");
	}

    if(nvram_param_read(NVCONF_FW_VERSION, camera.camInfo.version) < 0 )
	{
		syslog(LOG_ERR, "cannot read version");
	}

    if(nvram_param_read(NVCONF_SERVER_API, camera.camInfo.serverURL) < 0 )
	{
		syslog(LOG_ERR, "can not read server url");
	}
    sprintf(camera.camInfo.user_agent, "%s/%s", camera.camInfo.model, camera.camInfo.version);

        //to setup camera
    camera.camHdl.authencation_callback   = authencation_callback;

    camera.camHdl.pantil_motor_callback   = pantil_motor_callback;

    camera.camHdl.melody_callback         = melody_callback;

    camera.camHdl.network_callback        = network_callback;

    camera.camHdl.speaker_callback        = speaker_callback;

    camera.camHdl.system_callback         = system_callback;

    camera.camHdl.temperature_callback    = temperature_callback;

    camera.camHdl.talkback_callback       = talkback_callback;

    camera.camHdl.video_callback          = video_callback;
    /* Get params to use thread or not */
        /* For logging */

    //to genera udid
    ret =  hubble_gen_udid(&camera.camInfo);
    printf("Camera udid : %s\n", camera.camInfo.udid);

    ret = nvram_param_write(NVCONF_UDID,  camera.camInfo.udid);
    //to test genera upload file
    ret =  hubble_gen_snapshot_name( camera.camInfo.mac, "04", filename);
    printf("hubble_gen_snapshot_name : %s\n", filename);
    init_hubbble(&camera);
    
    while((NULL == camera.camInfo.masterkey) || (strcmp(camera.camInfo.masterkey,"") == 0))
    {
        syslog(LOG_INFO,"Camera haven't registered with hubble Server\n");
        if(nvram_param_read(NVCONF_MASTER_KEY, value_nvconf) < 0)
        {
            syslog(LOG_INFO,"Cannot read nvconf \n");
        }
        else
        {
            syslog(LOG_INFO,"Masterkey=%d \n", value_nvconf);
            if((strcmp(value_nvconf,"") != 0) && (strcmp(value_nvconf,"-1") != 0))
            {
                syslog(LOG_INFO,"Camera master key=%s\n", value_nvconf);
                strcpy(camera.camInfo.masterkey, value_nvconf);
            }
        }
        sleep(5);
    }
    run_hubble(&camera);
	// peter: forever loop, not the right way,should use pthread_join 
	// to wait for all other sub-threads to end
    while(1);//justt for test
    return 0;
}

