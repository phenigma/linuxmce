/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mediastream.h"

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cond=1;

static void stop_handler(int signum)
{
	cond--;
	if (cond<0) exit(-1);
}

static bool_t parse_addr(const char *addr, char *ip, int len, int *port)
{
	char *semicolon;
	int iplen;
	int slen;
	
	*port=0;
	semicolon=strchr(addr,':');
	if (semicolon==NULL) return FALSE;
	iplen=semicolon-addr;
	slen=MIN(iplen,len-1);
	strncpy(ip,addr,slen);
	ip[slen]='\0';
	*port=atoi(semicolon+1);
	return TRUE;
}

const char *usage="mediastream --local <port> --remote <ip:port> --payload <payload type number>\n";
static void run_media_streams(int localport, const char *remote_ip, int remoteport, int payload);

int main(int argc, char * argv[])
{
	int i;
	int localport=0,remoteport=0,payload=0;
	char ip[50];
	char *tmp;
	
	/*create the rtp session */
	ortp_init();
	rtp_profile_set_payload(&av_profile,115,&payload_type_lpc1015);
	rtp_profile_set_payload(&av_profile,110,&payload_type_speex_nb);
	rtp_profile_set_payload(&av_profile,111,&payload_type_speex_wb);
	rtp_profile_set_payload(&av_profile,112,&payload_type_ilbc);
	rtp_profile_set_payload(&av_profile,98,&payload_type_h263_1998);
	if (argc<4) {
		printf(usage);
		return -1;
	}
	for (i=1;i<argc;i++){
		if (strcmp(argv[i],"--local")==0){
			i++;
			localport=atoi(argv[i]);
		}else if (strcmp(argv[i],"--remote")==0){
			i++;
			if (!parse_addr(argv[i],ip,sizeof(ip),&remoteport)) {
				printf(usage);
				return -1;
			}
			printf("Remote addr: ip=%s port=%i\n",ip,remoteport);
		}else if (strcmp(argv[i],"--payload")==0){
			i++;
			payload=atoi(argv[i]);
		}
	}
	tmp=getenv("defcard");
	if (tmp!=NULL) audio_stream_set_default_card(atoi(tmp));
	run_media_streams(localport,ip,remoteport,payload);
	return 0;
}

void run_media_streams(int localport,  const char *remote_ip, int remoteport, int payload)
{
	AudioStream *audio=NULL;
#ifdef VIDEO_ENABLED
	VideoStream *video=NULL;
#endif
	PayloadType *pt=rtp_profile_get_payload(&av_profile,payload);
	
	if (pt==NULL) {
		printf("Error: No payload type defined for %i !\n",payload);
		exit (-1);
	}
	ms_init();
	signal(SIGINT,stop_handler);
	
	if (pt->type!=PAYLOAD_VIDEO){
		printf("Starting audio stream.\n");
		audio=audio_stream_start(&av_profile,localport,remote_ip,remoteport,payload,50);
	}else{
#ifdef VIDEO_ENABLED
		printf("Starting video stream.\n");
		video=video_stream_start(&av_profile,
					localport,
					remote_ip,
					remoteport,
					payload,
					50,
					"/dev/video0");
#else
		printf("Error: video support not compiled.\n");
#endif
	}
	while(cond)
	{
		/* sleep until we receive SIGINT */
		sleep(1);
		ortp_global_stats_display();
	}
	
	printf("stoping all...\n");
	
	if (audio) audio_stream_stop(audio);
#ifdef VIDEO_ENABLED
	if (video) video_stream_stop(video);
#endif
}
