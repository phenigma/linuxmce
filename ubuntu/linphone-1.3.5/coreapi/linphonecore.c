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

#include "linphonecore.h"
#include "lpconfig.h"
#include "private.h"
#include <mediastream.h>
#include <eXosip.h>
#include "sdphandler.h"

#include "misc.h"

#include <ortp/telephonyevents.h>
#include "exevents.h"

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#ifdef INET6  
#include <netdb.h>  
#endif

#include "enum.h"

extern char *strdup_printf(const char *,...);

#define MAX_IF 20
const double bandwidths[5]=
{
	28800.0,	/*28k modem*/
	28800.0, /*56k modem*/
	64000.0, /*64k modem (numeris)*/
	128000.0,	/*adsl or cable*/
	10e6, /* ethernet */
};/* carefull: these are the upstream bandwidth !!!*/

/* relative path where is stored local ring*/
#define LOCAL_RING "rings/oldphone.wav"
/* same for remote ring (ringback)*/
#define REMOTE_RING_FR "ringback.wav"
#define REMOTE_RING_US "ringback.wav"


/* these are useful messages for the status */
gchar *ready;
gchar *end;
gchar *contacting;
gchar *connected;
gchar *cancel;
gchar *contacted;


sdp_handler_t linphone_sdphandler={
	linphone_accept_audio_offer,   /*from remote sdp */
	linphone_accept_video_offer,   /*from remote sdp */
	linphone_set_audio_offer,	/*to local sdp */
	linphone_set_video_offer,	/*to local sdp */
	linphone_read_audio_answer,	/*from incoming answer  */
	linphone_read_video_answer	/*from incoming answer  */
};

void lc_callback_obj_init(LCCallbackObj *obj,LinphoneCoreCbFunc func,gpointer ud)
{
  obj->_func=func;
  obj->_user_data=ud;
}

gint lc_callback_obj_invoke(LCCallbackObj *obj, LinphoneCore *lc){
  if (obj->_func!=NULL) obj->_func(lc,obj->_user_data);
  return 0;
}

static void  linphone_call_init_common(LinphoneCall *call, char *from, char *to){
	sdp_context_set_user_pointer(call->sdpctx,(void*)call);
	call->state=LCStateInit;
	call->start_time=time(NULL);
	call->log=linphone_call_log_new(call, from, to);
}

LinphoneCall * linphone_call_new_outgoing(struct _LinphoneCore *lc, const osip_from_t *from, const osip_to_t *to)
{
	LinphoneCall *call=g_new0(LinphoneCall,1);
	char *localip=NULL;
	const char *nat_address=NULL;
	char *fromstr=NULL,*tostr=NULL;
	call->dir=LinphoneCallOutgoing;
	call->cid=-1;
	call->did=-1;
	call->core=lc;
	nat_address=linphone_core_get_nat_address_if_used(lc);
	if (nat_address==NULL)
		eXosip_get_localip_for(to->url->host,&localip);
	else{
		localip=strdup(nat_address);
	}
	call->sdpctx=sdp_handler_create_context(&linphone_sdphandler,localip,from->url->username);
	osip_free(localip);
	call->profile=lc->local_profile; /*use the readonly local profile*/
	osip_from_to_str(from,&fromstr);
	osip_to_to_str(to,&tostr);
	linphone_call_init_common(call,fromstr,tostr);
	return call;
}


LinphoneCall * linphone_call_new_incoming(LinphoneCore *lc, const char *from, const char *to, int cid, int did)
{
	LinphoneCall *call=g_new0(LinphoneCall,1);
	osip_from_t *me= linphone_core_get_primary_contact_parsed(lc);
	call->dir=LinphoneCallIncoming;
	call->cid=cid;
	call->did=did;
	call->core=lc;
	call->sdpctx=sdp_handler_create_context(&linphone_sdphandler,linphone_core_get_nat_address_if_used(lc),me->url->username);
	linphone_call_init_common(call, osip_strdup(from), osip_strdup(to));
	return call;
}

void linphone_call_destroy(LinphoneCall *obj)
{
	linphone_call_log_completed(obj->log,obj);
	if (obj->profile!=NULL && obj->profile!=&av_profile) rtp_profile_destroy(obj->profile);
	if (obj->sdpctx!=NULL) sdp_context_free(obj->sdpctx);
	g_free(obj);
}

LinphoneCallLog * linphone_call_log_new(LinphoneCall *call, gchar *from, gchar *to){
	LinphoneCallLog *cl=g_new0(LinphoneCallLog,1);
	cl->lc=call->core;
	cl->dir=call->dir;
	cl->start_date=g_strdup(ctime(&call->start_time));
	cl->from=from;
	cl->to=to;
	return cl;
}
void linphone_call_log_completed(LinphoneCallLog *calllog, LinphoneCall *call){
	LinphoneCore *lc=call->core;
	calllog->duration=time(NULL)-call->start_time;
	switch(call->state){
		case LCStateInit:
			calllog->status=LinphoneCallAborted;
			break;
		case LCStateRinging:
			if (calllog->dir==LinphoneCallIncoming){
				gchar *info;
				calllog->status=LinphoneCallMissed;
				lc->missed_calls++;
				info=g_strdup_printf(_("You have missed %i call(s)."),lc->missed_calls);
				lc->vtable.display_status(lc,info);
				g_free(info);
			}
			else calllog->status=LinphoneCallAborted;
			break;
		case LCStateAVRunning:
			calllog->status=LinphoneCallSuccess;
			break;
	}
	lc->call_logs=g_list_append(lc->call_logs,(gpointer)calllog);
	if (g_list_length(lc->call_logs)>lc->max_call_logs){
		GList *elem;
		elem=g_list_first(lc->call_logs);
		linphone_call_log_destroy((LinphoneCallLog*)elem->data);
		lc->call_logs=g_list_remove_link(lc->call_logs,elem);
	}
	if (lc->vtable.call_log_updated!=NULL){
		lc->vtable.call_log_updated(lc,calllog);
	}
}

gchar * linphone_call_log_to_str(LinphoneCallLog *cl){
	gchar *status;
	switch(cl->status){
		case LinphoneCallAborted:
			status=_("aborted");
			break;
		case LinphoneCallSuccess:
			status=_("completed");
			break;
		case LinphoneCallMissed:
			status=_("missed");
			break;
		default:
			status="unknown";
	}
	return g_strdup_printf(_("%s at %sFrom: %s\nTo: %s\nStatus: %s\nDuration: %i mn %i sec\n"),
			(cl->dir==LinphoneCallIncoming) ? _("Incoming call") : _("Outgoing call"),
			cl->start_date,
			cl->from,
			cl->to,
			status,
			cl->duration/60,
			cl->duration%60);
}

void linphone_call_log_destroy(LinphoneCallLog *cl){
	if (cl->start_date!=NULL) g_free(cl->start_date);
	if (cl->from!=NULL) osip_free(cl->from);
	if (cl->to!=NULL) osip_free(cl->to);
	g_free(cl);
}


static FILE * lc_logfile=NULL;

void
__null_log_handler (const gchar * log_domain, GLogLevelFlags log_level,
		   const gchar * message, gpointer user_data)
{
	return;
}

void
__file_log_handler (const gchar * log_domain, GLogLevelFlags log_level,
		       const gchar * message, gpointer user_data)
{
	fprintf (lc_logfile, "%s:(GLogLevel=%i)** %s\n", log_domain, log_level,
		 message);
}

void linphone_core_enable_logs(FILE *file){
	if (file==NULL) file=stdout;
	if (file!=stdout){
		lc_logfile=file;
		g_log_set_handler ("LinphoneCore", G_LOG_LEVEL_MASK,
				   __file_log_handler, NULL);
		g_log_set_handler ("MediaStreamer", G_LOG_LEVEL_MASK,
				   __file_log_handler, NULL);
	}
	ortp_set_log_file(file);
	ortp_set_log_level_mask(ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR|ORTP_FATAL);
	osip_trace_initialize (OSIP_INFO4, file);
}
void linphone_core_disable_logs(){
	int tl;
	for (tl=0;tl<=OSIP_INFO4;tl++) osip_trace_disable_level(tl);
	g_log_set_handler ("LinphoneCore",
				   G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_WARNING,
				   __null_log_handler, NULL);
	g_log_set_handler ("MediaStreamer",
			   G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_WARNING,
			   __null_log_handler, NULL);
	ortp_set_log_level_mask(ORTP_ERROR|ORTP_FATAL);
}


#ifdef LINPHONE_DEPRECATED
/* this is the retrieve_if function for FreeBSD. It uses getifaddrs system call instead of ioctl in order to get interface names and address.*/
/* Thanks 	to Wolfgang for this .*/
#ifdef HAVE_GETIFADDRS
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
GList *
retrieve_if ()
{
	int i;
	struct ifaddrs *ifp;
	struct ifaddrs *ifpstart;
	GList *interfaces=NULL;

	if (getifaddrs (&ifpstart) < 0)
	{
		return NULL;
	}
	ifp=ifpstart;
	for (i = 0; ifp != NULL; ifp = ifp->ifa_next)
	{
		interface_t *netif;
#ifdef INET6
		char namebuf[BUFSIZ];

		if (!ifp->ifa_addr)
		  continue;

		switch (ifp->ifa_addr->sa_family) {
		case AF_INET:
		        getnameinfo(ifp->ifa_addr, sizeof (struct sockaddr_in) ,
				    namebuf, sizeof(namebuf), NULL, 0, NI_NUMERICHOST);
		        netif=g_new0(interface_t,1);
			netif->index=i;
			netif->family = AF_INET;
			strncpy (netif->name, ifp->ifa_name,20);
			strncpy (netif->ipaddr, namebuf,20);
			interfaces=g_list_append(interfaces,netif);
			i++;
			break;
		case AF_INET6:
		        getnameinfo(ifp->ifa_addr, sizeof (struct sockaddr_in6) ,
				    namebuf, sizeof(namebuf), NULL, 0, NI_NUMERICHOST);
			if (strchr(namebuf, '%') == NULL) {
			  netif=g_new0(interface_t,1);
			  netif->index=i;
			  netif->family = AF_INET6;
			  strncpy (netif->name, ifp->ifa_name,20);
			  strncpy (netif->ipaddr, namebuf, 40);
			  interfaces=g_list_append(interfaces,netif);
			}
			i++;
			break;
		default:
		        continue;
  		}
#else
		if (ifp->ifa_addr->sa_family != AF_INET)
			continue;

		/*printf ("ifc %s %s\n",
			ifp->ifa_name,
			inet_ntoa (((struct sockaddr_in *) ifp->ifa_addr)->
				   sin_addr));*/
			
		netif=g_new0(interface_t,1);
		netif->index=i;
		strncpy (netif->name, ifp->ifa_name,20);
		strncpy (netif->ip4addr, inet_ntoa (((struct sockaddr_in *) ifp->ifa_addr)->sin_addr),20);
		interfaces=g_list_append(interfaces,netif);
		i++;
#endif
	}

	freeifaddrs (ifpstart);

	return interfaces;
}
#else

GList * retrieve_if()
{
	int sock,err,if_count,i;
	struct ifconf netconf;
	char buffer[sizeof(struct ifreq)*MAX_IF];
	GList *interfaces=NULL;
	
	netconf.ifc_len=sizeof(struct ifreq)*MAX_IF;
	netconf.ifc_buf=buffer;
	sock=socket(PF_INET, SOCK_DGRAM,0);
	err=ioctl(sock,SIOCGIFCONF,&netconf);
	if (err<0) printf("Error in ioctl: %i.\n",errno);
	close(sock);
	
	if_count=netconf.ifc_len/sizeof(struct ifreq);
	g_message("Found %i interfaces.\n",if_count);
	for (i=0;i<if_count;i++)
	{
#ifdef INET6
		interface_t *netif;
		char namebuf[BUFSIZ];

		
		

		switch (netconf.ifc_req[i].ifr_addr.sa_family) {
		case AF_INET:
			getnameinfo(&netconf.ifc_req[i].ifr_addr, sizeof (struct sockaddr_in) ,
				    namebuf, sizeof(namebuf), NULL, 0, NI_NUMERICHOST);
			netif=g_new0(interface_t,1);
			netif->index=i;
			netif->family = AF_INET;
			strncpy (netif->name, netconf.ifc_req[i].ifr_name,20);
			strncpy (netif->ipaddr, namebuf,20);
			interfaces=g_list_append(interfaces,netif);
			break;
		case AF_INET6:
			getnameinfo(&netconf.ifc_req[i].ifr_addr, sizeof (struct sockaddr_in6) ,
				    namebuf, sizeof(namebuf), NULL, 0, NI_NUMERICHOST);
			if (strchr(namebuf, '%') == NULL) {
				netif=g_new0(interface_t,1);
			  	netif->index=i;
			  	netif->family = AF_INET6;
			  	strncpy (netif->name, netconf.ifc_req[i].ifr_name,20);
			  	strncpy (netif->ipaddr, namebuf, 40);
			  	interfaces=g_list_append(interfaces,netif);
			}
			break;
		default:
		        continue;
  		}
		
#else
		interface_t *netif;
		if (netconf.ifc_req[i].ifr_addr.sa_family != AF_INET)
			continue;
		netif=g_new0(interface_t,1);
		strncpy(netif->name,netconf.ifc_req[i].ifr_name,20);
		strncpy(netif->ip4addr,inet_ntoa(((struct sockaddr_in*)(&netconf.ifc_req[i].ifr_addr))->sin_addr),20);
		netif->index=i;
		interfaces=g_list_append(interfaces,netif);
#endif
	}
	return interfaces;
}
#endif



gboolean find_interface(net_config_t *config, char *ifname)
{
	GList *interfaces=config->interfaces;
	interface_t *tmpif;
#ifdef INET6
	interface_t *tmp4if = NULL;
	int checkflag = 0;
#endif
	
	tmpif=(interface_t*) interfaces->data;
	while (interfaces!=NULL){
		tmpif=(interface_t*) interfaces->data;
		if (ifname!=NULL){
			if (strcmp(ifname,tmpif->name)==0){
#ifdef INET6
			  if (tmpif->family == AF_INET6) {
				config->sel_if=tmpif;
				return 1;
			  } else {
			        checkflag = 1;
				tmp4if = tmpif;
			  }
#else
				config->sel_if=tmpif;
				return 1;
#endif
			}
		}
		interfaces=g_list_next(interfaces);
	}
#ifdef INET6
	if (checkflag == 1) {
	        config->sel_if=tmp4if;
		return 1;
	}
#endif
	return 0;
}

void find_best_interface(net_config_t *config)
{
	interface_t *tmpif;
	tmpif=(interface_t*) config->interfaces->data;
	if (!find_interface(config,"ppp0")){
		/* if ppp0 is not found, then default to eth0 */
		if (!find_interface(config,"eth0")){
			/* if eth0 is not found, try ed0 (used on FreeBsd) */
			if (!find_interface(config,"ed0")){
				/* if eth0 is not found, then default to lo */
				if (!find_interface(config,"lo")){
					/* if lo is not found, default to first interface in the list */
					config->sel_if=tmpif;
				}
			}
		}
	}
}

#endif /*LINPHONE_DEPRECATED*/

void
net_config_read (LinphoneCore *lc)
{
	gint tmp;
	LpConfig *config=lc->config;

	tmp=lp_config_get_int(config,"net","con_type",CONNECTION_TYPE_ADSL);
	linphone_core_set_connection_type(lc,tmp);
	
}

static int get_best_device(){
	/* choose an alsa card if available, because alsa native support works better for low latency*/
	int i;
	int devid=0;
	const char *id;
	SndCard *sndcard;
	for(i=0;i<MAX_SND_CARDS;i++){
		sndcard=snd_card_manager_get_card(snd_card_manager,i);
		if (sndcard==NULL){
			devid=0;
			break;
		}
		id=snd_card_get_identifier(sndcard);
		if (strstr(id,"Advanced Linux Sound Architecture")!=0){
			g_message("Choosing alsa device %s by default.",id);
			devid=i;
			break;
		}
	}
	return devid;
}

#ifdef HAVE_ALSA_ASOUNDLIB_H
#include <alsacard.h>
#endif

void sound_config_read(LinphoneCore *lc)
{
	int devid,tmp;
	const char *tmpbuf;
	
	devid=lp_config_get_int(lc->config,"sound","playback_dev_id",-1);
	if (devid==-1) {
		devid=get_best_device();
	}
	linphone_core_set_playback_device(lc,devid);
	
	devid=lp_config_get_int(lc->config,"sound","ringer_dev_id",-1);
	if (devid==-1) {
		devid=linphone_core_get_playback_device(lc);
	}
	linphone_core_set_ringer_device(lc,devid);
	
	devid=lp_config_get_int(lc->config,"sound","capture_dev_id",-1);
	if (devid==-1) {
		devid=linphone_core_get_playback_device(lc);
	}
	linphone_core_set_capture_device(lc,devid);
	
	tmp=lp_config_get_int(lc->config,"sound","play_lev",80);
	linphone_core_set_play_level(lc,tmp);
	tmp=lp_config_get_int(lc->config,"sound","ring_lev",80);
	linphone_core_set_ring_level(lc,tmp);
	tmp=lp_config_get_int(lc->config,"sound","rec_lev",80);
	linphone_core_set_rec_level(lc,tmp);
	tmpbuf=lp_config_get_string(lc->config,"sound","source","m");
	linphone_core_set_sound_source(lc,tmpbuf[0]);
	
	tmpbuf=PACKAGE_SOUND_DIR "/" LOCAL_RING;
	tmpbuf=lp_config_get_string(lc->config,"sound","local_ring",tmpbuf);
	if (!g_file_test(tmpbuf,G_FILE_TEST_EXISTS)){
		tmpbuf=PACKAGE_SOUND_DIR "/" LOCAL_RING;
	}
	if (strstr(tmpbuf,".wav")==NULL){
		/* it currently uses old sound files, so replace them */
		tmpbuf=PACKAGE_SOUND_DIR "/" LOCAL_RING;
	}
	
	linphone_core_set_ring(lc,tmpbuf);
	
	tmpbuf=PACKAGE_SOUND_DIR "/" REMOTE_RING_FR;
	tmpbuf=lp_config_get_string(lc->config,"sound","remote_ring",tmpbuf);
	if (!g_file_test(tmpbuf,G_FILE_TEST_EXISTS)){
		tmpbuf=PACKAGE_SOUND_DIR "/" REMOTE_RING_FR;
	}
	if (strstr(tmpbuf,".wav")==NULL){
		/* it currently uses old sound files, so replace them */
		tmpbuf=PACKAGE_SOUND_DIR "/" REMOTE_RING_FR;
	}
	linphone_core_set_ringback(lc,0);
	lc->sound_conf.autokill=1;
#ifdef HAVE_ALSA_ASOUNDLIB_H
	tmpbuf=lp_config_get_string(lc->config,"sound","alsadev",NULL);
	if (tmpbuf!=NULL){
		alsa_card_manager_set_default_pcm_device(tmpbuf);
	}
#endif
	check_sound_device(lc);
}

void sip_config_read(LinphoneCore *lc)
{
	gchar *contact;
	const char *tmpstr;
	gint port;
	gint i,tmp;
	gint ipv6;
	port=lp_config_get_int(lc->config,"sip","use_info",0);
	linphone_core_set_use_info_for_dtmf(lc,port);

	ipv6=lp_config_get_int(lc->config,"sip","use_ipv6",-1);
	if (ipv6==-1){
		ipv6=0;
		if (host_has_ipv6_network()){
			lc->vtable.display_message(lc,_("Your machine appears to be connected to an IPv6 network. By default linphone always uses IPv4. Please update your configuration if you want to use IPv6"));
		}
	}
	linphone_core_enable_ipv6(lc,ipv6);
	port=lp_config_get_int(lc->config,"sip","sip_port",5060);
	linphone_core_set_sip_port(lc,port);
	
	tmpstr=lp_config_get_string(lc->config,"sip","contact",NULL);
	if (tmpstr==NULL) {
		gchar *hostname=getenv("HOST");
		gchar *username=getenv("USER");
		if (hostname==NULL) hostname=getenv("HOSTNAME");
		if (hostname==NULL)
			hostname="unknown-host";
		if (username==NULL){
			username="toto";
		}
		contact=g_strdup_printf("sip:%s@%s",username,hostname);
	}else contact=g_strdup(tmpstr);
	linphone_core_set_primary_contact(lc,contact);
	g_free(contact);
	

	tmp=lp_config_get_int(lc->config,"sip","guess_hostname",1);
	linphone_core_set_guess_hostname(lc,tmp);
	
	tmpstr=lp_config_get_string(lc->config,"net","nat_address",NULL);
	if (tmpstr!=NULL && (strlen(tmpstr)<1)) tmpstr=NULL;
	tmp=lp_config_get_int(lc->config,"net","use_nat",0);
	linphone_core_set_nat_address(lc,tmpstr,tmp);		
	
	tmp=lp_config_get_int(lc->config,"sip","inc_timeout",15);
	linphone_core_set_inc_timeout(lc,tmp);

	/* get proxies config */
	for(i=0;; i++){
		LinphoneProxyConfig *cfg=linphone_proxy_config_new_from_config_file(lc->config,i);
		if (cfg!=NULL){
			linphone_core_add_proxy_config(lc,cfg);
		}else{
			break;
		}
	}
	/* get the default proxy */
	tmp=lp_config_get_int(lc->config,"sip","default_proxy",-1);
	linphone_core_set_default_proxy_index(lc,tmp);
	
	/* read authentication information */
	for(i=0;; i++){
		LinphoneAuthInfo *ai=linphone_auth_info_new_from_config_file(lc->config,i);
		if (ai!=NULL){
			linphone_core_add_auth_info(lc,ai);
		}else{
			break;
		}
	}
	
}

void rtp_config_read(LinphoneCore *lc)
{
	gint port;
	gint jitt_comp;
	port=lp_config_get_int(lc->config,"rtp","audio_rtp_port",7078);
	linphone_core_set_audio_port(lc,port);
	
	port=lp_config_get_int(lc->config,"rtp","video_rtp_port",9078);
	if (port==0) port=9078;
	linphone_core_set_video_port(lc,port);
	
	jitt_comp=lp_config_get_int(lc->config,"rtp","audio_jitt_comp",60);
	linphone_core_set_audio_jittcomp(lc,jitt_comp);		
	jitt_comp=lp_config_get_int(lc->config,"rtp","video_jitt_comp",60);
}


PayloadType * get_codec(LpConfig *config, char* type,int index){
	gchar codeckey[50];
	const gchar *mime;
	gint rate,enabled;
	PayloadType *pt;
	
	snprintf(codeckey,50,"%s_%i",type,index);
	mime=lp_config_get_string(config,codeckey,"mime",NULL);
	if (mime==NULL || strlen(mime)==0 ) return NULL;
	
	pt=payload_type_new();
	pt->mime_type=g_strdup(mime);
	
	rate=lp_config_get_int(config,codeckey,"rate",8000);
	pt->clock_rate=rate;
	
	enabled=lp_config_get_int(config,codeckey,"enabled",1);
	if (enabled ) pt->flags|=PAYLOAD_TYPE_ENABLED;
	//g_message("Found codec %s/%i",pt->mime_type,pt->clock_rate);
	return pt;
}

void codecs_config_read(LinphoneCore *lc)
{
	int i;
	PayloadType *pt;
	GList *audio_codecs=NULL;
	GList *video_codecs=NULL;
	for (i=0;;i++){
		pt=get_codec(lc->config,"audio_codec",i);
		if (pt==NULL) break;
		audio_codecs=g_list_append(audio_codecs,(gpointer)pt);
	}
	for (i=0;;i++){
		pt=get_codec(lc->config,"video_codec",i);
		if (pt==NULL) break;
		video_codecs=g_list_append(video_codecs,(gpointer)pt);
	}
	linphone_core_set_audio_codecs(lc,audio_codecs);
	linphone_core_set_video_codecs(lc,video_codecs);
	linphone_core_setup_local_rtp_profile(lc);
}

void video_config_read(LinphoneCore *lc)
{
	/*gint tmp;*/
	const gchar *str;
	
	str=lp_config_get_string(lc->config,"video","device","/dev/video0");
	linphone_core_set_video_device(lc,NULL,str);
	
#ifdef VIDEO_ENABLED
	linphone_core_enable_video(lc,TRUE);
	linphone_core_enable_video_preview(lc,TRUE);
#endif
/*
#ifdef VIDEO_ENABLED
	tmp=1;
#else
	tmp=0;
#endif
	tmp=lp_config_get_int(lc->config,"video","enabled",tmp);
	linphone_core_enable_video(lc,tmp);
	tmp=lp_config_get_int(lc->config,"video","show_local",1);
	linphone_core_enable_video_preview(lc,tmp);
*/
}

void ui_config_read(LinphoneCore *lc)
{
	LinphoneFriend *lf;
	int i;
	for (i=0;(lf=linphone_friend_new_from_config_file(lc,i))!=NULL;i++){
		linphone_core_add_friend(lc,lf);
	}
	
}

void autoreplier_config_init(LinphoneCore *lc)
{
	autoreplier_config_t *config=&lc->autoreplier_conf;
	config->enabled=lp_config_get_int(lc->config,"autoreplier","enabled",0);
	config->after_seconds=lp_config_get_int(lc->config,"autoreplier","after_seconds",6);
	config->max_users=lp_config_get_int(lc->config,"autoreplier","max_users",1);
	config->max_rec_time=lp_config_get_int(lc->config,"autoreplier","max_rec_time",60);
	config->max_rec_msg=lp_config_get_int(lc->config,"autoreplier","max_rec_msg",10);
	config->message=lp_config_get_string(lc->config,"autoreplier","message",NULL);
}

void
linphone_core_init (LinphoneCore * lc,LinphoneCoreVTable *vtable, const gchar *config_path, gpointer userdata)
{
	
	ortp_init();
	rtp_profile_set_payload(&av_profile,115,&payload_type_lpc1015);
	rtp_profile_set_payload(&av_profile,110,&payload_type_speex_nb);
	rtp_profile_set_payload(&av_profile,111,&payload_type_speex_wb);
	rtp_profile_set_payload(&av_profile,112,&payload_type_ilbc);
	rtp_profile_set_payload(&av_profile,116,&payload_type_truespeech);
	rtp_profile_set_payload(&av_profile,101,&payload_type_telephone_event);
	
	rtp_profile_set_payload(&av_profile,98,&payload_type_h263_1998);
	
	ms_init();
	ms_speex_codec_init();
	
	memset (lc, 0, sizeof (LinphoneCore));
	lc->data=userdata;
	ready=_("Ready.");
  	end=_("Communication ended.");
	contacting= _("Contacting ");
	contacted=_("is calling you.");
	connected=_("Connected.");
	cancel=_("Call cancelled.");
	
	memcpy(&lc->vtable,vtable,sizeof(LinphoneCoreVTable));

	lc->config=lp_config_new(config_path);

  
#ifdef VINCENT_MAURY_RSVP
	/* default qos parameters : rsvp on, rpc off */
	lc->rsvp_enable = 1;
	lc->rpc_enable = 0;
#endif

	sound_config_read(lc);
	sip_config_read(lc); /* this will start eXosip*/
	
	net_config_read(lc);
	rtp_config_read(lc);
	codecs_config_read(lc);
	video_config_read(lc);
	//autoreplier_config_init(&lc->autoreplier_conf);
	
	lc->presence_mode=LINPHONE_STATUS_ONLINE;
	lc->max_call_logs=15;
	ui_config_read(lc);
	lc->in_main_thread=1;
	lc->lock=g_mutex_new();
	lc->vtable.display_status(lc,ready);
}

LinphoneCore *linphone_core_new(LinphoneCoreVTable *vtable,
						const gchar *config_path, gpointer userdata)
{
	LinphoneCore *core=g_new(LinphoneCore,1);
	linphone_core_init(core,vtable,config_path,userdata);
	return core;
}

GList *linphone_core_get_audio_codecs(LinphoneCore *lc)
{
	return g_list_copy(lc->codecs_conf.audio_codecs);
}

GList *linphone_core_get_video_codecs(LinphoneCore *lc)
{
	return g_list_copy(lc->codecs_conf.video_codecs);
}

int linphone_core_set_primary_contact(LinphoneCore *lc,const gchar *contact)
{
	if (lc->sip_conf.contact!=NULL) g_free(lc->sip_conf.contact);
	lc->sip_conf.contact=g_strdup(contact);
	if (lc->sip_conf.guessed_contact!=NULL){
		g_free(lc->sip_conf.guessed_contact);
		lc->sip_conf.guessed_contact=NULL;
	}
	return 0;
}

const gchar *linphone_core_get_primary_contact(LinphoneCore *lc)
{
	gchar *identity;
	if (lc->sip_conf.guess_hostname){
		if (lc->sip_conf.guessed_contact==NULL || lc->sip_conf.loopback_only){
			gchar *tmp=NULL,*guessed=NULL;
			osip_from_t *url;
			if (lc->sip_conf.guessed_contact!=NULL){
				g_free(lc->sip_conf.guessed_contact);
				lc->sip_conf.guessed_contact=NULL;
			}
			
			osip_from_init(&url);
			if (osip_from_parse(url,lc->sip_conf.contact)==0){
				
			}else g_error("Could not parse identity contact !");
			if (!lc->sip_conf.ipv6_enabled){
				eXosip_get_localip_for("15.128.128.93", &tmp);
			}else{
				eXosip_get_localip_for("3ffe:4015:bbbb:70d0:201:2ff:fe09:81b1", &tmp);
			}
			if (strcmp(tmp,"127.0.0.1")==0 || strcmp(tmp,"::1")==0 ){
				g_warning("Local loopback network only !");
				lc->sip_conf.loopback_only=TRUE;
			}else lc->sip_conf.loopback_only=FALSE;
			osip_free(url->url->host);
			url->url->host=tmp;
			if (url->url->port!=NULL){
				osip_free(url->url->port);
				url->url->port=NULL;
			}
			if (lc->sip_conf.sip_port!=5060){
				url->url->port=strdup_printf("%i",lc->sip_conf.sip_port);
			}
			osip_from_to_str(url,&guessed);
			lc->sip_conf.guessed_contact=guessed;
			
			osip_from_free(url);
			
		}
		identity=lc->sip_conf.guessed_contact;
	}else{
		identity=lc->sip_conf.contact;
	}
	return identity;
}

void linphone_core_set_guess_hostname(LinphoneCore *lc, gboolean val){
	lc->sip_conf.guess_hostname=val;
}	
gboolean linphone_core_get_guess_hostname(LinphoneCore *lc){
	return lc->sip_conf.guess_hostname;
}

osip_from_t *linphone_core_get_primary_contact_parsed(LinphoneCore *lc)
{
	int err;
	osip_from_t *contact;
	osip_from_init(&contact);
	err=osip_from_parse(contact,linphone_core_get_primary_contact(lc));
	if (err<0) {
		osip_from_free(contact);
		return NULL;
	}
	return contact;
}

int linphone_core_set_audio_codecs(LinphoneCore *lc, GList *codecs)
{
	if (lc->codecs_conf.audio_codecs!=NULL) g_list_free(lc->codecs_conf.audio_codecs);
	lc->codecs_conf.audio_codecs=codecs;
	return 0;
}

int linphone_core_set_video_codecs(LinphoneCore *lc, GList *codecs)
{
	if (lc->codecs_conf.video_codecs!=NULL) g_list_free(lc->codecs_conf.video_codecs);
	lc->codecs_conf.video_codecs=codecs;
	return 0;
}

GList * linphone_core_get_friend_list(LinphoneCore *lc)
{
	return lc->friends;
}


GList * linphone_core_get_net_interfaces(LinphoneCore *lc)
{
	return g_list_copy(lc->net_conf.interfaces);
}

interface_t * linphone_core_get_selected_interface(LinphoneCore *lc)
{
	return lc->net_conf.sel_if;
}

void linphone_core_set_connection_type(LinphoneCore *lc, int type)
{
	lc->net_conf.con_type=type;
	lc->net_conf.bandwidth=bandwidths[type];
	linphone_core_check_codecs_for_bandwidth(lc);
}

int linphone_core_get_connection_type(LinphoneCore *lc)
{
	return lc->net_conf.con_type;
}

int linphone_core_get_audio_jittcomp(LinphoneCore *lc)
{
	return lc->rtp_conf.audio_jitt_comp;
}

int linphone_core_get_audio_port(LinphoneCore *lc)
{
	return lc->rtp_conf.audio_rtp_port;
}

int linphone_core_get_video_port(LinphoneCore *lc){
	return lc->rtp_conf.video_rtp_port;
}

void linphone_core_set_audio_jittcomp(LinphoneCore *lc, int value)
{
	lc->rtp_conf.audio_jitt_comp=value;
}

void linphone_core_set_audio_port(LinphoneCore *lc, int port)
{
	lc->rtp_conf.audio_rtp_port=port;
}

void linphone_core_set_video_port(LinphoneCore *lc, int port){
	lc->rtp_conf.video_rtp_port=port;
}

gboolean linphone_core_get_use_info_for_dtmf(LinphoneCore *lc)
{
	return lc->sip_conf.use_info;
}

void linphone_core_set_use_info_for_dtmf(LinphoneCore *lc,gboolean use_info)
{
	lc->sip_conf.use_info=use_info;
}

int linphone_core_get_sip_port(LinphoneCore *lc)
{
	return lc->sip_conf.sip_port;
}

static gboolean exosip_running=FALSE;

void linphone_core_set_sip_port(LinphoneCore *lc,int port)
{
	int err=0;
	if (port==lc->sip_conf.sip_port) return;
	lc->sip_conf.sip_port=port;
	if (exosip_running) eXosip_quit();
	err=eXosip_init(NULL,stdout,port);
	if (err<0){
		char *msg=g_strdup_printf("UDP port %i seems already in use ! Cannot initialize.",port);
		g_warning(msg);
		lc->vtable.display_warning(lc,msg);
		g_free(msg);
		return;
	}
#ifdef VINCENT_MAURY_RSVP
	/* tell exosip the qos settings according to default linphone parameters */
	eXosip_set_rsvp_mode (lc->rsvp_enable);
	eXosip_set_rpc_mode (lc->rpc_enable);
#endif
	eXosip_set_user_agent("Linphone-" LINPHONE_VERSION "/eXosip");
	exosip_running=TRUE;
}

gboolean linphone_core_ipv6_enabled(LinphoneCore *lc){
	return lc->sip_conf.ipv6_enabled;
}
void linphone_core_enable_ipv6(LinphoneCore *lc, gboolean val){
	if (lc->sip_conf.ipv6_enabled!=val){
		lc->sip_conf.ipv6_enabled=val;
		eXosip_enable_ipv6(val);
		if (exosip_running){
			/* we need to restart eXosip */
			linphone_core_set_sip_port(lc, lc->sip_conf.sip_port);
		}
	}
}

void linphone_core_iterate(LinphoneCore *lc)
{
	eXosip_event_t *ev;
#ifdef AMD
	printf("entering linphone_core_iterate()\n");
#endif
	if (lc->preview_finished){
		lc->preview_finished=0;
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
		lc_callback_obj_invoke(&lc->preview_finished_cb,lc);
		restore_sound_daemons();
	}
#ifdef AMD
	printf("getting eXosip event...\n");
#endif
	
	if (exosip_running){
		while((ev=eXosip_event_wait(0,0))!=NULL){
			linphone_core_process_event(lc,ev);
		}
		linphone_core_update_proxy_register(lc);
		linphone_core_refresh_subscribes(lc);
	}
	if (lc->call!=NULL){
		LinphoneCall *call=lc->call;
		if (call->dir==LinphoneCallIncoming && call->state==LCStateRinging){
			int elapsed=time(NULL)-call->start_time;
			g_message("incoming call ringing for %i seconds",elapsed);
			if (elapsed>lc->sip_conf.inc_timeout){
				linphone_core_terminate_call(lc,NULL);
			}
		}
	}

#ifdef AMD
	printf("iterate done...\n");
#endif
}


gboolean linphone_core_is_in_main_thread(LinphoneCore *lc){
	return TRUE;
}

static osip_to_t *osip_to_create(const char *to){
	osip_to_t *ret;
	osip_to_init(&ret);
	if (osip_to_parse(ret,to)<0){
		osip_to_free(ret);
		return NULL;
	}
	return ret;
}

gboolean linphone_core_interpret_url(LinphoneCore *lc, const char *url, char **real_url, osip_to_t **real_parsed_url){
	enum_lookup_res_t *enumres=NULL;
	osip_to_t *parsed_url=NULL;
	char *enum_domain=NULL;
	LinphoneProxyConfig *proxy;
	char *tmpurl;
	
	if (real_url!=NULL) *real_url=NULL;
	if (real_parsed_url!=NULL) *real_parsed_url=NULL;
	
	if (is_enum(url,&enum_domain)){
		lc->vtable.display_status(lc,_("Looking for telephone number destination..."));
		if (enum_lookup(enum_domain,&enumres)<0){
			lc->vtable.display_status(lc,_("Could not resolve this number."));
			g_free(enum_domain);
			return FALSE;
		}
		g_free(enum_domain);
		tmpurl=enumres->sip_address[0];
		if (real_url!=NULL) *real_url=g_strdup(tmpurl);
		if (real_parsed_url!=NULL) *real_parsed_url=osip_to_create(tmpurl);
		enum_lookup_res_free(enumres);
		return TRUE;
	}
	/* check if we have a "sip:" */
	if (strstr(url,"sip:")==NULL){
		/* this doesn't look like a true sip uri */
		proxy=lc->default_proxy;
		if (proxy!=NULL){
			/* append the proxy suffix */
			osip_uri_t *proxy_uri;
			char *sipaddr;
			const char *proxy_addr=linphone_proxy_config_get_addr(proxy);
			osip_uri_init(&proxy_uri);
			if (osip_uri_parse(proxy_uri,proxy_addr)<0){
				osip_uri_free(proxy_uri);
				return FALSE;
			}
			if (proxy_uri->port!=NULL)
				sipaddr=g_strdup_printf("sip:%s@%s:%s",url,proxy_uri->host,proxy_uri->port);
			else
				sipaddr=g_strdup_printf("sip:%s@%s",url,proxy_uri->host);
			if (real_parsed_url!=NULL) *real_parsed_url=osip_to_create(sipaddr);
			if (real_url!=NULL) *real_url=sipaddr;
			else g_free(sipaddr);
			return TRUE;
		}
	}
	parsed_url=osip_to_create(url);
	if (parsed_url!=NULL){
		if (real_url!=NULL) *real_url=g_strdup(url);
		if (real_parsed_url!=NULL) *real_parsed_url=parsed_url;
		else osip_to_free(parsed_url);
		return TRUE;
	}
	/* else we could not do anything with url given by user, so display an error */
	if (lc->vtable.display_warning!=NULL){
		lc->vtable.display_warning(lc,_("Could not parse given sip address. A sip url usually looks like sip:user@domain"));
	}
	return FALSE;
}

const char * linphone_core_get_identity(LinphoneCore *lc){
	LinphoneProxyConfig *proxy=NULL;
	const char *from;
	linphone_core_get_default_proxy(lc,&proxy);
	if (proxy!=NULL) {
		from=linphone_proxy_config_get_identity(proxy);
	}else from=linphone_core_get_primary_contact(lc);
	return from;
}

const char * linphone_core_get_route(LinphoneCore *lc){
	LinphoneProxyConfig *proxy=NULL;
	const char *route=NULL;
	linphone_core_get_default_proxy(lc,&proxy);
	if (proxy!=NULL) {
		route=linphone_proxy_config_get_route(proxy);
	}
	return route;
}

int linphone_core_invite(LinphoneCore *lc, const char *url)
{
	gchar *barmsg;
	gint err=0;
	gchar *sdpmesg=NULL;
	gchar *route=NULL;
	const gchar *from=NULL;
	osip_message_t *invite=NULL;
	sdp_context_t *ctx=NULL;
	LinphoneProxyConfig *proxy=NULL;
	osip_from_t *parsed_url2=NULL;
	osip_to_t *real_parsed_url=NULL;
	char *real_url=NULL;
	
	linphone_core_get_default_proxy(lc,&proxy);
	if (!linphone_core_interpret_url(lc,url,&real_url,&real_parsed_url)){
		/* bad url */
		return -1;
	}
	barmsg=g_strdup_printf("%s %s", contacting, real_url);
	lc->vtable.display_status(lc,barmsg);
	g_free(barmsg);
	if (proxy!=NULL) {
		route=linphone_proxy_config_get_route(proxy);
		from=linphone_proxy_config_get_identity(proxy);
	}
	/* if no proxy or no identity defined for this proxy, default to primary contact*/
	if (from==NULL) from=linphone_core_get_primary_contact(lc);

	err=eXosip_build_initial_invite(&invite,(gchar*)real_url,(gchar*)from,
								(gchar*)route,"Phone call");

	if (err<0){
		g_warning("Could not build initial invite");
		goto end;
	}
	/* make sdp message */
	
	osip_from_init(&parsed_url2);
	osip_from_parse(parsed_url2,from);
	
	lc->call=linphone_call_new_outgoing(lc,parsed_url2,real_parsed_url);
	ctx=lc->call->sdpctx;
	sdpmesg=sdp_context_get_offer(ctx);
	eXosip_lock();
	err=eXosip_initiate_call_with_body(invite,"application/sdp",sdpmesg,(void*)lc->call);
	lc->call->cid=err;
	eXosip_unlock();
	if (err<0){
		g_warning("Could not initiate call.");
		lc->vtable.display_status(lc,ready);
		linphone_call_destroy(lc->call);
		lc->call=NULL;
	}
	
	goto end;
	end:
		if (real_url!=NULL) g_free(real_url);
		if (real_parsed_url!=NULL) osip_to_free(real_parsed_url);
		if (parsed_url2!=NULL) osip_from_free(parsed_url2);
	return (err<0) ? -1 : 0;
}

int linphone_core_refer(LinphoneCore *lc, const char *url)
{
    char *real_url=NULL;
    osip_to_t *real_parsed_url=NULL;
    LinphoneCall *call;
    if (!linphone_core_interpret_url(lc,url,&real_url,&real_parsed_url)){
        /* bad url */
        return -1;
    }
    call=lc->call;
    if (call==NULL){
        g_warning("No established call to refer.");
        return -1;
    }
    lc->call=NULL;
    
    eXosip_lock();
    eXosip_transfer_call(call->did, real_url);
    eXosip_unlock();
    return 0;
}    

gboolean linphone_core_inc_invite_pending(LinphoneCore*lc){
	if (lc->call==NULL){
		return FALSE;
	}
	return TRUE;
}

#ifdef VINCENT_MAURY_RSVP
/* on=1 for RPC_ENABLE=1...*/
int linphone_core_set_rpc_mode(LinphoneCore *lc, int on)
{
	if (on==1)
		printf("RPC_ENABLE set on\n");
	else 
		printf("RPC_ENABLE set off\n");
	lc->rpc_enable = (on==1);
	/* need to tell eXosip the new setting */
	if (eXosip_set_rpc_mode (lc->rpc_enable)!=0)
		return -1;
	return 0;
}

/* on=1 for RSVP_ENABLE=1...*/
int linphone_core_set_rsvp_mode(LinphoneCore *lc, int on)
{
	if (on==1)
		printf("RSVP_ENABLE set on\n");
	else 
		printf("RSVP_ENABLE set off\n");
	lc->rsvp_enable = (on==1);
	/* need to tell eXosip the new setting */
	if (eXosip_set_rsvp_mode (lc->rsvp_enable)!=0)
		return -1;
	return 0;
}

/* answer : 1 for yes, 0 for no */
int linphone_core_change_qos(LinphoneCore *lc, int answer)
{
	char *sdpmesg;
	if (lc->call==NULL){
		return -1;
	}
	
	if (lc->rsvp_enable && answer==1)
	{
		/* answer is yes, local setting is with qos, so 
		 * the user chose to continue with no qos ! */
		/* so switch in normal mode : ring and 180 */
		lc->rsvp_enable = 0; /* no more rsvp */
		eXosip_set_rsvp_mode (lc->rsvp_enable);
		/* send 180 */
		eXosip_lock();
		eXosip_answer_call(lc->call->did,180,NULL);
		eXosip_unlock();
		/* play the ring */
		if (try_open_dsp(lc)>0){
			g_message("Starting local ring...");
			lc->ringstream=ring_start(lc->sound_conf.local_ring,
					2,snd_card_manager_get_card(snd_card_manager,lc->sound_conf.ring_sndcard));
		}
	}
	else if (!lc->rsvp_enable && answer==1)
	{
		/* switch to QoS mode on : answer 183 session progress */
		lc->rsvp_enable = 1;
		eXosip_set_rsvp_mode (lc->rsvp_enable);
		/* take the sdp already computed, see osipuacb.c */
		sdpmesg=lc->call->sdpctx->answerstr;
		eXosip_lock();
		eXosip_answer_call_with_body(lc->call->did,183,"application/sdp",sdpmesg);
		eXosip_unlock();
	}
	else
	{
		/* decline offer (603) */
		linphone_core_terminate_call(lc, NULL);
	}
	return 0;
}
#endif

void linphone_core_start_media_streams(LinphoneCore *lc, LinphoneCall *call){
	osip_from_t *me=linphone_core_get_primary_contact_parsed(lc);
	char *cname=g_strdup_printf("%s@%s",me->url->username,me->url->host);
	{
		int jitt_comp;
		StreamParams *audio_params=&call->audio_params;
		SndCard *playcard=snd_card_manager_get_card(snd_card_manager,lc->sound_conf.play_sndcard);
		SndCard *captcard=snd_card_manager_get_card(snd_card_manager,lc->sound_conf.capt_sndcard);
		if (playcard==NULL) {
			g_warning("No card defined for playback !");
			goto end;
		}
		if (captcard==NULL) {
			g_warning("No card defined for capture !");
			goto end;
		}
		
		/* adjust rtp jitter compensation. It must be at least the latency of the sound card */
		jitt_comp=MAX(lc->sound_conf.latency,lc->rtp_conf.audio_jitt_comp);
		lc->audiostream=audio_stream_start_with_sndcards(call->profile,audio_params->localport,
							audio_params->remoteaddr,audio_params->remoteport,
							audio_params->pt,jitt_comp,playcard,captcard);
		if (lc->audiostream==NULL)
		  {
		    /* should terminate call */
		  }
		else
		  {
		    audio_stream_set_rtcp_information(lc->audiostream, cname);
		  }
	}
#ifdef VIDEO_ENABLED
	{
		/* shutdown preview */
		if (lc->previewstream!=NULL) {
			video_preview_stop(lc->previewstream);
			lc->previewstream=NULL;
		}
		if (lc->video_conf.enabled){
			int jitt_comp;
			StreamParams *video_params=&call->video_params;
			
			if (video_params->remoteport>0){
				/* adjust rtp jitter compensation. It must be at least the latency of the sound card */
				jitt_comp=MAX(lc->sound_conf.latency,lc->rtp_conf.audio_jitt_comp);
				lc->videostream=video_stream_start(call->profile,video_params->localport,
									video_params->remoteaddr,video_params->remoteport,
									video_params->pt,jitt_comp, lc->video_conf.show_local, "Video4Linux",
									lc->video_conf.device);
				video_stream_set_rtcp_information(lc->videostream, cname);
			}
		}
	}
#endif
	goto end;
	end:
	g_free(cname);
	osip_from_free(me);
	lc->call->state=LCStateAVRunning;
}

void linphone_core_stop_media_streams(LinphoneCore *lc){
	if (lc->audiostream!=NULL) {
		audio_stream_stop(lc->audiostream);
		lc->audiostream=NULL;
		restore_sound_daemons();
	}
#ifdef VIDEO_ENABLED
	if (lc->videostream!=NULL){
		video_stream_stop(lc->videostream);
		lc->videostream=NULL;
	}
	if (linphone_core_video_preview_enabled(lc)){
		if (lc->previewstream==NULL){
			lc->previewstream=video_preview_start("Video4Linux",lc->video_conf.device);
		}
	}
#endif
}

int linphone_core_accept_call(LinphoneCore *lc, const char *url)
{
	char *sdpmesg;
	
	if (lc->call==NULL){
		return -1;
	}
	
	
	/*stop ringing */
	if (lc->ringstream!=NULL) {
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
	}
	/* sends a 200 OK */
	sdpmesg=lc->call->sdpctx->answerstr;	/* takes the sdp already computed*/
	eXosip_lock();
	eXosip_answer_call_with_body(lc->call->did,200,"application/sdp",sdpmesg);
	eXosip_unlock();
	lc->vtable.display_status(lc,connected);
	
	linphone_core_start_media_streams(lc, lc->call);
	return 0;
}

int linphone_core_terminate_call(LinphoneCore *lc, const char *url)
{
	LinphoneCall *call=lc->call;
	if (call==NULL){
		return -1;
	}
	lc->call=NULL;
	
	eXosip_lock();
	eXosip_terminate_call(call->cid,call->did);
	eXosip_unlock();
	
	/*stop ringing*/
	if (lc->ringstream!=NULL) {
		ring_stop(lc->ringstream);
		lc->ringstream=NULL;
	}
	linphone_core_stop_media_streams(lc);
	lc->vtable.display_status(lc,end);
	linphone_call_destroy(call);
	return 0;
}

int linphone_core_send_publish(LinphoneCore *lc,
			       LinphoneOnlineStatus presence_mode)
{
	GList *elem;
	for (elem=linphone_core_get_proxy_config_list(lc);elem!=NULL;elem=g_list_next(elem)){
		LinphoneProxyConfig *cfg=(LinphoneProxyConfig*)elem->data;
		if (cfg->publish) linphone_proxy_config_send_publish(cfg,presence_mode);
	}
	return 0;
}

void linphone_core_set_inc_timeout(LinphoneCore *lc, int seconds){
	lc->sip_conf.inc_timeout=seconds;
}

int linphone_core_get_inc_timeout(LinphoneCore *lc){
	return lc->sip_conf.inc_timeout;
}

void linphone_core_set_presence_info(LinphoneCore *lc,gint minutes_away,
													const gchar *contact,
													LinphoneOnlineStatus presence_mode)
{
	int contactok=-1;
	if (minutes_away>0) lc->minutes_away=minutes_away;
	if (contact!=NULL) {
		osip_from_t *url;
		osip_from_init(&url);
		contactok=osip_from_parse(url,contact);
		if (contactok>=0) {
			g_message("contact url is correct.");
		}
		osip_from_free(url);
		
	}
	if (contactok>=0){
		if (lc->alt_contact!=NULL) g_free(lc->alt_contact);
		lc->alt_contact=g_strdup(contact);
	}
	if (lc->presence_mode!=presence_mode){
		linphone_core_notify_all_friends(lc,presence_mode);
		/* 
		   Improve the use of all LINPHONE_STATUS available.
		   !TODO Do not mix "presence status" with "answer status code"..
		   Use correct parameter to follow sip_if_match/sip_etag.
		 */
		linphone_core_send_publish(lc,presence_mode);
	}
	lc->presence_mode=presence_mode;
	
}

/* sound functions */
gint linphone_core_get_play_level(LinphoneCore *lc)
{
	return lc->sound_conf.play_lev;
}
gint linphone_core_get_ring_level(LinphoneCore *lc)
{
	return lc->sound_conf.ring_lev;
}
gint linphone_core_get_rec_level(LinphoneCore *lc){
	return lc->sound_conf.rec_lev;
}
void linphone_core_set_ring_level(LinphoneCore *lc, gint level){
	SndCard *sndcard;
	lc->sound_conf.ring_lev=level;
	sndcard=snd_card_manager_get_card(snd_card_manager,lc->sound_conf.ring_sndcard);
	g_return_if_fail(sndcard!=NULL);
	snd_card_set_level(sndcard,SND_CARD_LEVEL_OUTPUT,level);
}

void linphone_core_set_play_level(LinphoneCore *lc, gint level){
	SndCard *sndcard;
	lc->sound_conf.play_lev=level;
	sndcard=snd_card_manager_get_card(snd_card_manager,lc->sound_conf.play_sndcard);
	g_return_if_fail(sndcard!=NULL);
	snd_card_set_level(sndcard,SND_CARD_LEVEL_OUTPUT,level);
}

void linphone_core_set_rec_level(LinphoneCore *lc, gint level)
{
	SndCard *sndcard;
	lc->sound_conf.rec_lev=level;
	sndcard=snd_card_manager_get_card(snd_card_manager,lc->sound_conf.capt_sndcard);
	g_return_if_fail(sndcard!=NULL);
	snd_card_set_level(sndcard,SND_CARD_LEVEL_INPUT,level);
}
gint linphone_core_set_ringer_device(LinphoneCore *lc, gint devid)
{
	SndCard *sndcard;
	int tmp;
	
	sndcard=snd_card_manager_get_card(snd_card_manager,devid);
	if (sndcard==NULL){
		g_warning("Sound card with index %i does not exist.",devid);
		devid=0;
	}
	lc->sound_conf.ring_sndcard=devid;
	tmp=test_audio_dev(devid);
	if (tmp>0) lc->sound_conf.latency=tmp;
	return 0;
}

gint linphone_core_set_playback_device(LinphoneCore *lc, gint devid)
{
	SndCard *sndcard;
	int tmp;
	
	sndcard=snd_card_manager_get_card(snd_card_manager,devid);
	if (sndcard==NULL){
		g_warning("Sound card with index %i does not exist.",devid);
		devid=0;
	}
	lc->sound_conf.play_sndcard=devid;
	tmp=test_audio_dev(devid);
	if (tmp>0) lc->sound_conf.latency=tmp;
	return 0;
}

gint linphone_core_set_capture_device(LinphoneCore *lc, gint devid)
{
	SndCard *sndcard;
	
	sndcard=snd_card_manager_get_card(snd_card_manager,devid);
	if (sndcard==NULL){
		g_warning("Sound card with index %i does not exist.",devid);
		devid=0;
	}
	lc->sound_conf.capt_sndcard=devid;
	return 0;
}

gint linphone_core_set_ringer_device_from_name(LinphoneCore *lc,const gchar *name)
{
	SndCard *card;
	gint index;
	card=snd_card_manager_get_card_with_string(snd_card_manager,name,&index);
	if (card!=NULL){
		g_message("Using card %s for ringer, id=%i",name,index);
		linphone_core_set_ringer_device(lc,index);
		return 0;
	}
	return -1;
}

gint linphone_core_set_playback_device_from_name(LinphoneCore *lc,const gchar *name)
{
	SndCard *card;
	gint index;
	card=snd_card_manager_get_card_with_string(snd_card_manager,name,&index);
	if (card!=NULL){
		g_message("Using card %s for playback, id=%i",name,index);
		linphone_core_set_playback_device(lc,index);
		return 0;
	}
	return -1;
}

gint linphone_core_set_capture_device_from_name(LinphoneCore *lc,const gchar *name)
{
	SndCard *card;
	gint index;
	card=snd_card_manager_get_card_with_string(snd_card_manager,name,&index);
	if (card!=NULL){
		g_message("Using card %s for capture, id=%i",name,index);
		linphone_core_set_capture_device(lc,index);
		return 0;
	}
	return -1;
}

gint linphone_core_get_ringer_device(LinphoneCore *lc)
{
	return lc->sound_conf.ring_sndcard;
}

gint linphone_core_get_playback_device(LinphoneCore *lc)
{
	return lc->sound_conf.play_sndcard;
}

gint linphone_core_get_capture_device(LinphoneCore *lc)
{
	return lc->sound_conf.capt_sndcard;
}

SndCardManager * linphone_core_get_card_manager(LinphoneCore *lc){
	return snd_card_manager;
}

gchar linphone_core_get_sound_source(LinphoneCore *lc)
{
	return lc->sound_conf.source;
}
void linphone_core_set_sound_source(LinphoneCore *lc, gchar source)
{
	SndCard *sndcard=snd_card_manager_get_card(snd_card_manager,lc->sound_conf.capt_sndcard);
	lc->sound_conf.source=source;
	g_return_if_fail(sndcard!=NULL);
	snd_card_set_rec_source(sndcard,source);
}

void linphone_core_set_ring(LinphoneCore *lc,const gchar *path){
	if (lc->sound_conf.local_ring!=0){
		g_free(lc->sound_conf.local_ring);
	}
	lc->sound_conf.local_ring=g_strdup(path);
}
gchar *linphone_core_get_ring(LinphoneCore *lc){
	return lc->sound_conf.local_ring;
}

static void notify_end_of_ring(MSFilter *f,gint event, gpointer value,gpointer user_data){
  LinphoneCore *lc=(LinphoneCore*)user_data;
  lc->preview_finished=1;
}

gint linphone_core_preview_ring(LinphoneCore *lc, const gchar *ring,LinphoneCoreCbFunc func,gpointer userdata)
{
  if (lc->ringstream!=0){
    g_warning("Cannot start ring now,there's already a ring being played");
    return -1;
  }
  lc_callback_obj_init(&lc->preview_finished_cb,func,userdata);
  lc->preview_finished=0;
  if (try_open_dsp(lc)<0) return -1;
  lc->ringstream=ring_start_with_cb((char*)ring,2,snd_card_manager_get_card(snd_card_manager,lc->sound_conf.ring_sndcard),(MSFilterNotifyFunc)notify_end_of_ring,(gpointer)lc);
  return 0;
}


void linphone_core_set_ringback(LinphoneCore *lc,RingBackType type){
	switch(type){
		case RINGBACK_TYPE_FR:
			lc->sound_conf.remote_ring=PACKAGE_SOUND_DIR "/" REMOTE_RING_FR;
		break;
		case RINGBACK_TYPE_US:
			lc->sound_conf.remote_ring=PACKAGE_SOUND_DIR "/" REMOTE_RING_US;
		break;
	}
}
RingBackType linphone_core_get_ringback(LinphoneCore *lc);

void linphone_core_send_dtmf(LinphoneCore *lc,gchar dtmf)
{
  if (linphone_core_get_use_info_for_dtmf(lc)==0){
    /* In Band DTMF */
    if (lc->audiostream!=NULL){
      audio_stream_send_dtmf(lc->audiostream,dtmf);
    }
  }
  else{
	char dtmf_body[1000];
    /* Out of Band DTMF (use INFO method) */
    LinphoneCall *call=lc->call;
    if (call==NULL){
      return;
    }
    
    
    snprintf(dtmf_body, 999, "Signal=%c\r\nDuration=250\r\n", dtmf);
    eXosip_lock();
    eXosip_info_call(call->did, "application/dtmf-relay", dtmf_body);
    eXosip_unlock();
  }
}

void linphone_core_force_ip_address(LinphoneCore *lc, const gchar *ipaddr)
{
	linphone_core_set_nat_address(lc,ipaddr,1);
}


void linphone_core_set_nat_address(LinphoneCore *lc, const gchar *addr, gboolean use)
{
	gchar *wmsg;
	gchar *tmp=NULL;
	gint err;
	struct addrinfo hints,*res;
	
	if (addr!=NULL){
		if (use){
			/*check the ip address given */
			memset(&hints,0,sizeof(struct addrinfo));
			if (lc->sip_conf.ipv6_enabled)
				hints.ai_family=AF_INET6;
			else 
				hints.ai_family=AF_INET;
			hints.ai_socktype = SOCK_DGRAM;
			err=getaddrinfo(addr,NULL,&hints,&res);
			if (err!=0){
				wmsg=g_strdup_printf(_("Invalid nat address '%s' : %s"),
					addr, gai_strerror(err));
				g_warning(wmsg); // what is this for ?
				lc->vtable.display_warning(lc, wmsg);
				g_free(wmsg);
				return;
			}
			/*now get it as an numeric ip address */
			tmp=g_malloc0(50);
			err=getnameinfo(res->ai_addr,res->ai_addrlen,tmp,50,NULL,0,NI_NUMERICHOST);
			if (err!=0){
				wmsg=g_strdup_printf(_("Invalid nat address '%s' : %s"),
					addr, gai_strerror(err));
				g_warning(wmsg); // what is this for ?
				lc->vtable.display_warning(lc, wmsg);
				g_free(wmsg);
				g_free(tmp);
				freeaddrinfo(res);
				return;
			}
			freeaddrinfo(res);
		}
		if (lc->net_conf.nat_address!=NULL){
			g_free(lc->net_conf.nat_address);
		}
		lc->net_conf.nat_address=tmp;
	}

	if (use && lc->net_conf.nat_address!=NULL) {
		lc->net_conf.use_nat=TRUE;
		eXosip_set_firewallip(lc->net_conf.nat_address);
	}
	else {
		lc->net_conf.use_nat=FALSE;
		eXosip_set_firewallip("");		
	}
}

const gchar *linphone_core_get_nat_address(LinphoneCore *lc, gboolean *use)
{
	if (use!=NULL) *use=lc->net_conf.use_nat;
	if (lc->net_conf.nat_address!=NULL) return lc->net_conf.nat_address;
	else return NULL;
}

const gchar *linphone_core_get_nat_address_if_used(LinphoneCore *lc){
	if (lc->net_conf.use_nat && lc->net_conf.nat_address!=NULL) return lc->net_conf.nat_address;
	else return NULL;		
}

GList * linphone_core_get_call_logs(LinphoneCore *lc){
	lc->missed_calls=0;
	return lc->call_logs;
}

void linphone_core_enable_video(LinphoneCore *lc, gboolean val){
	lc->video_conf.enabled=val;
}
gboolean linphone_core_video_enabled(LinphoneCore *lc){
	return lc->video_conf.enabled;
}

void linphone_core_enable_video_preview(LinphoneCore *lc, gboolean val){
	lc->video_conf.show_local=val;
#ifdef VIDEO_ENABLED
	if (lc->videostream==NULL){
		if (val){
			if (lc->previewstream==NULL){
				lc->previewstream=video_preview_start("Video4Linux",lc->video_conf.device);
			}
		}else{
			if (lc->previewstream!=NULL){
				video_preview_stop(lc->previewstream);
				lc->previewstream=NULL;
			}
		}
	}
#endif
}
gboolean linphone_core_video_preview_enabled(LinphoneCore *lc){
	return lc->video_conf.show_local;
}

int linphone_core_set_video_device(LinphoneCore *lc, const gchar *method, const gchar *device){
	if (lc->video_conf.device!=NULL){
		g_free(lc->video_conf.device);
		lc->video_conf.device=NULL;
	}
	if (device!=NULL)
		lc->video_conf.device=g_strdup(device);
	return 0;
}

void *linphone_core_get_user_data(LinphoneCore *lc){
	return lc->data;
}

void net_config_uninit(LinphoneCore *lc)
{
	net_config_t *config=&lc->net_conf;
#ifdef LINPHONE_DEPRECATED
	gnome_config_set_string("net/if_name",config->sel_if->name);
	g_list_foreach(config->interfaces,(GFunc)g_free,NULL);
	g_list_free(config->interfaces);
	config->interfaces=NULL;
#endif
	lp_config_set_int(lc->config,"net","con_type",config->con_type);
	
	lp_config_set_int(lc->config,"net","use_nat",config->use_nat);
	if (config->nat_address!=NULL)
		lp_config_set_string(lc->config,"net","nat_address",config->nat_address);
	
}


void sip_config_uninit(LinphoneCore *lc)
{
	GList *elem;
	gint i;
	sip_config_t *config=&lc->sip_conf;
	lp_config_set_int(lc->config,"sip","sip_port",config->sip_port);
	lp_config_set_int(lc->config,"sip","guess_hostname",config->guess_hostname);
	lp_config_set_string(lc->config,"sip","contact",config->contact);
	lp_config_set_int(lc->config,"sip","inc_timeout",config->inc_timeout);
	lp_config_set_int(lc->config,"sip","use_info",config->use_info);
	lp_config_set_int(lc->config,"sip","use_ipv6",config->ipv6_enabled);
	for(elem=config->proxies,i=0;elem!=NULL;elem=g_list_next(elem),i++){
		LinphoneProxyConfig *cfg=(LinphoneProxyConfig*)(elem->data);
		linphone_proxy_config_write_to_config_file(lc->config,cfg,i);
		linphone_proxy_config_edit(cfg);	/* to unregister */
	}

	if (exosip_running)
	  {
	    int i;
	    for (i=0;i<20;i++)
	      {
		eXosip_event_t *ev;
		while((ev=eXosip_event_wait(0,0))!=NULL){
		  linphone_core_process_event(lc,ev);
		}
		usleep(100000);
	      }
	  }
	
	linphone_proxy_config_write_to_config_file(lc->config,NULL,i);	/*mark the end */
	
	for(elem=lc->auth_info,i=0;elem!=NULL;elem=g_list_next(elem),i++){
		LinphoneAuthInfo *ai=(LinphoneAuthInfo*)(elem->data);
		linphone_auth_info_write_config(lc->config,ai,i);
	}
	linphone_auth_info_write_config(lc->config,NULL,i); /* mark the end */
}

void rtp_config_uninit(LinphoneCore *lc)
{
	rtp_config_t *config=&lc->rtp_conf;
	lp_config_set_int(lc->config,"rtp","audio_rtp_port",config->audio_rtp_port);
	lp_config_set_int(lc->config,"rtp","video_rtp_port",config->video_rtp_port);
	lp_config_set_int(lc->config,"rtp","audio_jitt_comp",config->audio_jitt_comp);
	lp_config_set_int(lc->config,"rtp","video_jitt_comp",config->audio_jitt_comp);
}

void sound_config_uninit(LinphoneCore *lc)
{
	char tmpbuf[2];
	sound_config_t *config=&lc->sound_conf;
	lp_config_set_int(lc->config,"sound","playback_dev_id",config->play_sndcard);
	lp_config_set_int(lc->config,"sound","ringer_dev_id",config->ring_sndcard);
	lp_config_set_int(lc->config,"sound","capture_dev_id",config->capt_sndcard);
	lp_config_set_int(lc->config,"sound","rec_lev",config->rec_lev);
	lp_config_set_int(lc->config,"sound","play_lev",config->play_lev);
	lp_config_set_int(lc->config,"sound","ring_lev",config->ring_lev);
	tmpbuf[0]=config->source;
	tmpbuf[1]='\0';
	lp_config_set_string(lc->config,"sound","source",tmpbuf);
	lp_config_set_string(lc->config,"sound","local_ring",config->local_ring);
	lp_config_set_string(lc->config,"sound","remote_ring",config->remote_ring);
}

void video_config_uninit(LinphoneCore *lc)
{
	video_config_t *config=&lc->video_conf;
	lp_config_set_int(lc->config,"video","enabled",config->enabled);
	lp_config_set_int(lc->config,"video","show_local",config->show_local);
}

void codecs_config_uninit(LinphoneCore *lc)
{
	PayloadType *pt;
	codecs_config_t *config=&lc->codecs_conf;
	GList *node;
	gchar key[50];
	gint index;
	index=0;
	for(node=config->audio_codecs;node!=NULL;node=g_list_next(node)){
		pt=(PayloadType*)(node->data);
		sprintf(key,"audio_codec_%i",index);
		lp_config_set_string(lc->config,key,"mime",pt->mime_type);
		lp_config_set_int(lc->config,key,"rate",pt->clock_rate);
		lp_config_set_int(lc->config,key,"enabled",payload_type_enabled(pt));
		index++;
	}
	index=0;
	for(node=config->video_codecs;node!=NULL;node=g_list_next(node)){
		pt=(PayloadType*)(node->data);
		sprintf(key,"video_codec_%i",index);
		lp_config_set_string(lc->config,key,"mime",pt->mime_type);
		lp_config_set_int(lc->config,key,"rate",pt->clock_rate);
		lp_config_set_int(lc->config,key,"enabled",payload_type_enabled(pt));
		index++;
	}
}

void ui_config_uninit(LinphoneCore* lc)
{
	GList *elem;
	int i;
	for (elem=lc->friends,i=0; elem!=NULL; elem=g_list_next(elem),i++){
		linphone_friend_write_to_config_file(lc->config,(LinphoneFriend*)elem->data,i);
		linphone_friend_destroy(elem->data);
	}
	linphone_friend_write_to_config_file(lc->config,NULL,i);	/* set the end */
	g_list_free(lc->friends);
	lc->friends=NULL;
}

LpConfig *linphone_core_get_config(LinphoneCore *lc){
	return lc->config;
}

void linphone_core_uninit(LinphoneCore *lc)
{
#ifdef VIDEO_ENABLED
	if (lc->previewstream!=NULL){
		video_preview_stop(lc->previewstream);
		lc->previewstream=NULL;
	}
#endif
	/* save all config */
	net_config_uninit(lc);
	sip_config_uninit(lc);
	lp_config_set_int(lc->config,"sip","default_proxy",linphone_core_get_default_proxy(lc,NULL));
	rtp_config_uninit(lc);
	sound_config_uninit(lc);
	video_config_uninit(lc);
	codecs_config_uninit(lc);
	ui_config_uninit(lc);
	lp_config_sync(lc->config);
	lp_config_destroy(lc->config);
	
	ortp_exit();
	eXosip_quit();
}

void linphone_core_destroy(LinphoneCore *lc){
	linphone_core_uninit(lc);
	g_free(lc);
}

