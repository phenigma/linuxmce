
/*
linphone
Copyright (C) 2000  Simon MORLAT (simon.morlat@free.fr)

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

#include "misc.h"
#include <mediastream.h>
#include <mscodec.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef INET6
#include <sys/socket.h>
#include <netdb.h>
#endif

static char lock_name[80];
static char lock_set=0;

/* put a lock file in /tmp. this is called when linphone runs as a daemon*/
int set_lock_file()
{
	FILE *lockfile;
	
	snprintf(lock_name,80,"/tmp/linphone.%i",getuid());
	lockfile=fopen(lock_name,"w");
	if (lockfile==NULL)
	{
		printf("Failed to create lock file.\n");
		return(-1);
	}
	fprintf(lockfile,"%i",getpid());
	fclose(lockfile);
	lock_set=1;
	return(0);
}

/* looks if there is a lock file. If presents return its content (the pid of the already running linphone), if not found, returns -1*/
int get_lock_file()
{
	int pid;
	FILE *lockfile;
	
	snprintf(lock_name,80,"/tmp/linphone.%i",getuid());
	lockfile=fopen(lock_name,"r");
	if (lockfile==NULL)
		return(-1);
	if (fscanf(lockfile,"%i",&pid)!=1){
		g_warning("Could not read pid in lock file.");
		fclose(lockfile);
		return -1;
	}
	fclose(lockfile);
	return pid;
}

/* remove the lock file if it was set*/
int remove_lock_file()
{
	int err=0;
	if (lock_set)
	{
		err=unlink(lock_name);
		lock_set=0;
	}
	return(err);
}

static int esd_killed=0;
static int artsd_killed=0;

int kill_sound_daemons()
{
	char *command;
	int err;
	FILE *out;
	pid_t pid;
	
	/* try to kill all artsd*/
	command=g_strdup_printf("ps -u %s |grep artsd",getenv("LOGNAME"));
	out=popen(command,"r");
	if (out!=NULL)
	{
		do{
			err=fscanf(out,"%i",&pid);
			if (err==1) {
				if (linphone_arts_suspend()<0) kill(pid,SIGINT);
				artsd_killed=1;
			}
		}while(err==1);
		pclose(out);
	}
	g_free(command);
	/* kill aplay, because for some reason on kde3 it is spawned when artsd dies*/
	command=g_strdup_printf("ps -u %s |grep aplay",getenv("LOGNAME"));
	out=popen(command,"r");
	if (out!=NULL)
	{
		do{
			err=fscanf(out,"%i",&pid);
			if (err==1) {
				kill(pid,SIGINT);
			}
		}while(err==1);
		pclose(out);
	}
	g_free(command);
	
	/* do the same with esd*/
	command=g_strdup_printf("ps -u %s |grep esd",getenv("LOGNAME"));
	out=popen(command,"r");
	if (out!=NULL)
	{
		do{
			err=fscanf(out,"%i",&pid);
			if (err==1) {
				kill(pid,SIGINT);
				esd_killed=1;
			}
		}while(err==1);
		pclose(out);
	}
	g_free(command);
	return(0);
}

void restore_sound_daemons()
{
	if (esd_killed){
		esd_killed=0;
		g_message("Restarting esd...");
		g_spawn_command_line_async("esd",NULL);
	}
	if (artsd_killed){
		artsd_killed=0;
		g_message("Restarting artsd...");
		if (linphone_arts_restore()<0) g_spawn_command_line_async("artsd",NULL);
	}
}


/*try to open dsp. If failed, kill sound daemons if allowed by user, and try agin. If fail again, display the error message. Close it in success*/
int __try_open_dsp(LinphoneCore *lc, int devid)
{
	int err1,err2,retval=-1;
	
	err1=test_audio_dev(devid);
	
	if (err1>=0){
		retval=err1;
		goto end;
	}
	
	switch (err1){
		case -ENODEV: /* no such device */
		case -ENOENT: /* no such file */
			return err1;
	}
	
	/* else it is probably EWOULDBLOCK, EAGAIN */	
	if (lc->sound_conf.autokill)
	{
		g_message("killing sound daemons.\n");
		kill_sound_daemons();
		sleep(1);   /* wait for sound daemons to be killed...*/
	}
	else
	{
		
		return(-1);
	}
	err2=test_audio_dev(devid);
	if (err2<0)
	{
		char *msg;
		SndCard *sc=snd_card_manager_get_card(snd_card_manager,devid);
		if (sc!=NULL){
			msg=g_strdup_printf(_("Linphone could not open audio device %s. Check if your sound card is fully configured and working."),
												snd_card_get_identifier(sc));
			lc->vtable.display_warning(lc,msg);
			g_free(msg);
		}else{
			g_warning("Fail to open undefined audio device !");
		}
		return(-1);
	}
	retval=err2;
	
	end:
	if (retval>0) lc->sound_conf.latency=retval/8;
	return retval;
}

int try_open_dsp(LinphoneCore *lc){
	int play_dev=lc->sound_conf.play_sndcard;
	int capt_dev=lc->sound_conf.capt_sndcard;
	int err;
	if (play_dev!=capt_dev) {
		err=__try_open_dsp(lc,play_dev);
		err=__try_open_dsp(lc,capt_dev);
	}else err=__try_open_dsp(lc,play_dev);
	return err;
}

char *int2str(int number)
{
	gchar *numstr=g_malloc(10);
	snprintf(numstr,10,"%i",number);
	return numstr;
}



void check_sound_device(LinphoneCore *lc)
{
	int fd,len;
	gint a;
	char *file=NULL;
	char *i810_audio=NULL;
	char *snd_pcm_oss=NULL;
	char *snd_mixer_oss=NULL;
	char *snd_pcm=NULL;
	
	fd=open("/proc/modules",O_RDONLY);
	if (fd>0){
		/* read the entire /proc/modules file and check if sound conf seems correct */
		/*a=fstat(fd,&statbuf);
		if (a<0) g_warning("Can't stat /proc/modules:%s.",strerror(errno));
		len=statbuf.st_size;
		if (len==0) g_warning("/proc/modules has zero size!");
		*/
		/***** fstat does not work on /proc/modules for unknown reason *****/
		len=6000;
		file=g_malloc(len+1);
		a=read(fd,file,len);
		if (a<len) file=g_realloc(file,a+1);
		file[a]='\0';
		i810_audio=strstr(file,"i810_audio");
		if (i810_audio!=NULL){
			/* I'm sorry i put this warning in comments because
			 * i don't use yet the right driver !! */
/*			lc->vtable.display_warning(lc,_("You are currently using the i810_audio driver.\nThis driver is buggy and so does not work with Linphone.\nWe suggest that you replace it by its equivalent ALSA driver,\neither with packages from your distribution, or by downloading\nALSA drivers at http://www.alsa-project.org."));*/
			goto end;
		}
		snd_pcm=strstr(file,"snd-pcm");
		if (snd_pcm!=NULL){
			snd_pcm_oss=strstr(file,"snd-pcm-oss");
			snd_mixer_oss=strstr(file,"snd-mixer-oss");
			if (snd_pcm_oss==NULL){
				lc->vtable.display_warning(lc,_("Your computer appears to be using ALSA sound drivers.\nThis is the best choice. However the pcm oss emulation module\nis missing and linphone needs it. Please execute\n'modprobe snd-pcm-oss' as root to load it."));
			}
			if (snd_mixer_oss==NULL){
				lc->vtable.display_warning(lc,_("Your computer appears to be using ALSA sound drivers.\nThis is the best choice. However the mixer oss emulation module\nis missing and linphone needs it. Please execute\n 'modprobe snd-mixer-oss' as root to load it."));
			}
		}
	}else {
#ifdef __LINUX__
		g_warning("Could not open /proc/modules.");
#endif
	}
	/* now check general volume. Some user forget to rise it and then complain that linphone is
	not working */
	/* but some other users complain that linphone should not change levels... 
	if (lc->sound_conf.sndcard!=NULL){
		a=snd_card_get_level(lc->sound_conf.sndcard,SND_CARD_LEVEL_GENERAL);
		if (a<50){
			g_warning("General level is quite low (%i). Linphone rises it up for you.",a);
			snd_card_set_level(lc->sound_conf.sndcard,SND_CARD_LEVEL_GENERAL,80);
		}
	}
	*/
	end:
	if (file!=NULL) g_free(file);
	if (fd>0) close(fd);
}

#define UDP_HDR_SZ 8
#define RTP_HDR_SZ 12
#define IP4_HDR_SZ 20   /*20 is the minimum, but there may be some options*/

gchar *payload_type_get_description(PayloadType *pt){
	if (pt->user_data!=NULL){
		MSFilterInfo *c=(MSFilterInfo*)pt->user_data;
		return c->description;
	}
	return NULL;
}	

void payload_type_set_enable(PayloadType *pt,gint value) 
{
	if ((value)!=0) payload_type_set_flag(pt,PAYLOAD_TYPE_ENABLED); \
	else payload_type_unset_flag(pt,PAYLOAD_TYPE_ENABLED); 
}
void payload_type_set_usable(PayloadType *pt,gint value) 
{
	if ((value)!=0) payload_type_set_flag(pt,PAYLOAD_TYPE_USABLE); \
	else payload_type_unset_flag(pt,PAYLOAD_TYPE_USABLE); 
}



gboolean payload_type_enabled(PayloadType *pt) {
	return (((pt)->flags & PAYLOAD_TYPE_ENABLED)!=0);
}
gboolean payload_type_usable(PayloadType *pt) {
	return ((pt)->flags & PAYLOAD_TYPE_USABLE);
}

gint payload_type_get_bitrate(PayloadType *pt)
{
	return pt->normal_bitrate;
}
const gchar *payload_type_get_mime(PayloadType *pt){
	return pt->mime_type;
}
gint payload_type_get_rate(PayloadType *pt){
	return pt->clock_rate;
}




/* return TRUE if codec can be used with bandwidth, FALSE else*/
gboolean payload_type_check_usable(PayloadType *pt,double bandwidth)
{
	gboolean ret=0;
	double codec_band;
	double npacket;
	double packet_size;
	
	switch (pt->type){
		case PAYLOAD_AUDIO_CONTINUOUS:
		case PAYLOAD_AUDIO_PACKETIZED:
			/* very approximative estimation... revisit*/
			npacket=50;
			packet_size=(double)(pt->normal_bitrate/(50*8))+UDP_HDR_SZ+RTP_HDR_SZ+IP4_HDR_SZ;
			codec_band=packet_size*8.0*npacket;
			ret=(codec_band<bandwidth);
			//g_message("Payload %s: %g",pt->mime_type,codec_band);
			break;
		case PAYLOAD_VIDEO:
			if (bandwidth>=128000) ret=TRUE;
			else ret=FALSE;
			break;
	}
	/*if (!ret) g_warning("Payload %s is not usable with your internet connection.",pt->mime_type);*/
	
	return ret;
}

void linphone_core_check_codecs_for_bandwidth(LinphoneCore *lc)
{
	GList *elem;
	PayloadType *pt;
	
	for(elem=lc->codecs_conf.audio_codecs;elem!=NULL;elem=g_list_next(elem))
	{
		pt=(PayloadType*)elem->data;
		if (strcmp("speex",pt->mime_type)==0 && pt->clock_rate==8000){
			if (lc->net_conf.bandwidth<bandwidths[CONNECTION_TYPE_NUMERIS]){
				pt->normal_bitrate=8000;
			}else pt->normal_bitrate=20000;
		}	
		payload_type_set_usable(pt,payload_type_check_usable(pt,lc->net_conf.bandwidth));	
	}
	for(elem=lc->codecs_conf.video_codecs;elem!=NULL;elem=g_list_next(elem))
	{
		pt=(PayloadType*)elem->data;
		payload_type_set_usable(pt,payload_type_check_usable(pt,lc->net_conf.bandwidth));	
	}
}

GList *fix_codec_list(RtpProfile *prof, GList *conflist)
{
	GList *elem;
	GList *newlist=NULL;
	MSCodecInfo *codec;
	PayloadType *payload,*confpayload;
	for (elem=conflist;elem!=NULL;elem=g_list_next(elem))
	{
		confpayload=(PayloadType*)elem->data;
			
		payload=rtp_profile_find_payload(prof,
								confpayload->mime_type,
								confpayload->clock_rate);
		if (payload!=NULL){
			/*check if mediastreamer supports this payload*/
			switch (payload->type){
				case PAYLOAD_AUDIO_CONTINUOUS:
				case PAYLOAD_AUDIO_PACKETIZED:
					codec=ms_audio_codec_info_get(payload->mime_type);
					break;
				case PAYLOAD_VIDEO:
					codec=ms_video_codec_info_get(payload->mime_type);
				break;
			default:
				g_error("Unsupported rtp media type.");
				codec=NULL;
			}
			if (codec==NULL){
				g_warning("There is no plugin to handle %s",confpayload->mime_type);
			}
			else 
			{
				payload_type_set_user_data(payload,(gpointer)codec);
				payload_type_set_enable(payload,payload_type_enabled(confpayload));
				newlist=g_list_append(newlist,(gpointer)payload);
			}
		}
		else{
			g_warning("Cannot support %s/%i: does not exist.",confpayload->mime_type,
								confpayload->clock_rate);
		}
	}
	return newlist;
}


void linphone_core_setup_local_rtp_profile(LinphoneCore *lc)
{
	int i;
	GList *audiopt,*videopt;
	MSCodecInfo *codec;
	PayloadType *payload;
	
	lc->local_profile=&av_profile;
	
	/* first look at the list given by configuration file to see if 
	it is correct */
	audiopt=fix_codec_list(lc->local_profile,lc->codecs_conf.audio_codecs);
	videopt=fix_codec_list(lc->local_profile,lc->codecs_conf.video_codecs);
	
	/* now find and add payloads that are not listed in the configuration
	codec list */
	for (i=0;i<127;i++)
	{
		payload=rtp_profile_get_payload(lc->local_profile,i);
		if (payload!=NULL){
			if (payload_type_get_user_data(payload)!=NULL) continue;
			/* find a mediastreamer codec for this payload type */
			switch (payload->type){
				case PAYLOAD_AUDIO_CONTINUOUS:
				case PAYLOAD_AUDIO_PACKETIZED:
					codec=ms_audio_codec_info_get(payload->mime_type);
					if (codec!=NULL){
						g_message("Adding new codec %s/%i",payload->mime_type,payload->clock_rate);
						payload_type_set_enable(payload,1);
						payload_type_set_user_data(payload,(gpointer)codec);
						audiopt=g_list_append(audiopt,(gpointer)payload);
					}
					break;
				case PAYLOAD_VIDEO:
					codec=ms_video_codec_info_get(payload->mime_type);
					if (codec!=NULL){
						payload_type_set_enable(payload,1);
						payload_type_set_user_data(payload,(gpointer)codec);
						videopt=g_list_append(videopt,(gpointer)payload);
					}
					break;
				default:
					g_error("Unsupported rtp media type.");
			}
		}
	}
	g_list_foreach(lc->codecs_conf.audio_codecs,(GFunc)payload_type_destroy,NULL);
	g_list_foreach(lc->codecs_conf.video_codecs,(GFunc)payload_type_destroy,NULL);
	g_list_free(lc->codecs_conf.audio_codecs);
	g_list_free(lc->codecs_conf.video_codecs);
	/* set the fixed lists instead:*/
	lc->codecs_conf.audio_codecs=audiopt;
	lc->codecs_conf.video_codecs=videopt;
	linphone_core_check_codecs_for_bandwidth(lc);
}

int from_2char_without_params(osip_from_t *from,char **str)
{
	osip_from_t *tmpfrom=NULL;
	osip_from_clone(from,&tmpfrom);
	if (tmpfrom!=NULL){
		while(!osip_list_eol(tmpfrom->gen_params,0)){
			osip_generic_param_t *param=(osip_generic_param_t*)osip_list_get(tmpfrom->gen_params,0);
			osip_generic_param_free(param);
			osip_list_remove(tmpfrom->gen_params,0);
		}
	}else return -1;
	osip_from_to_str(tmpfrom,str);
	osip_from_free(tmpfrom);
	return 0;
}


#if defined(HAVE_GETIFADDRS) && defined(INET6)
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
gboolean host_has_ipv6_network()
{
	struct ifaddrs *ifp;
	struct ifaddrs *ifpstart;
	gboolean ipv6_present=FALSE;
	
	if (getifaddrs (&ifpstart) < 0)
	{
		return FALSE;
	}
	
	for (ifp=ifpstart; ifp != NULL; ifp = ifp->ifa_next)
	{
		if (!ifp->ifa_addr)
		  continue;

		switch (ifp->ifa_addr->sa_family) {
		case AF_INET:
		        
			break;
		case AF_INET6:
		    ipv6_present=TRUE;
			break;
		default:
		        continue;
  		}
	}

	freeifaddrs (ifpstart);

	return ipv6_present;
}
#else

gboolean host_has_ipv6_network()
{
	return FALSE;
}


#endif
