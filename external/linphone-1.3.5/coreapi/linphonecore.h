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
#ifndef LINPHONECORE_H
#define LINPHONECORE_H

#include <sndcard.h>

#undef PACKAGE
#undef VERSION

#include <config.h>

#ifdef HAVE_GETTEXT
#include <libintl.h>
#ifndef _
#define _(String) gettext(String)
#endif
#else
#define _(something)	(something)
#endif

#ifdef HAVE_GLIB
#include <glib.h>
#else
#include <uglib.h>
#endif

#include <osipparser2/osip_message.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef PayloadType_defined
#define PayloadType_defined
typedef struct _PayloadType PayloadType;
#endif

gboolean payload_type_enabled(struct _PayloadType *pt);
gboolean payload_type_usable(struct _PayloadType *pt);
void payload_type_set_enable(struct _PayloadType *pt,gint value);
void payload_type_set_usable(struct _PayloadType *pt,gint value);
gchar *payload_type_get_description(struct _PayloadType *pt);
gint payload_type_get_bitrate(PayloadType *pt);
const gchar *payload_type_get_mime(PayloadType *pt);
gint payload_type_get_rate(PayloadType *pt);



#define CONNECTION_TYPE_28KMODEM 0
#define CONNECTION_TYPE_56KMODEM 1
#define CONNECTION_TYPE_NUMERIS 2
#define CONNECTION_TYPE_ADSL 3
#define CONNECTION_TYPE_ETHERNET 4

extern const double bandwidths[5];

struct _LpConfig;

typedef struct sip_config
{
	gchar *contact;
	gchar *guessed_contact;
	gint sip_port;
	GList *proxies;
	GList *deleted_proxies;
	int inc_timeout;	/*timeout after an un-answered incoming call is rejected*/
	gboolean use_info;
	gboolean guess_hostname;
	gboolean loopback_only;
	gboolean ipv6_enabled;
} sip_config_t;

typedef struct rtp_config
{
	guint16 audio_rtp_port;
	guint16 video_rtp_port;
	gint audio_jitt_comp;  /*jitter compensation*/
	gint video_jitt_comp;  /*jitter compensation*/
}rtp_config_t;

typedef struct _interface_t
{
	char name[20];
#ifdef INET6
	int  family;
	char ipaddr[40];
#else
	char ip4addr[20];
#endif
	int index;
} interface_t;

typedef struct net_config
{
	GList *interfaces;	/* list of interface_t */
	interface_t *sel_if;
	double bandwidth; 		/* the bandwidth in bit/s of the selected interface*/ /*unused*/
	gchar *nat_address;
	gboolean use_nat;
	gint con_type;			/* type of connection 28k 56k 128k adsl ...*/
}net_config_t;


typedef struct sound_config
{
	int ring_sndcard;	/* the playback sndcard currently used */
	int play_sndcard;	/* the playback sndcard currently used */
	int capt_sndcard; /* the capture sndcard currently used */
	gint latency;	/* latency in samples of the current used sound device */
	char rec_lev;
	char play_lev;
	char ring_lev;
	char source;
	char pad;
	char *local_ring;
	char *remote_ring;
	gchar *alsadev;	/* for people who wants to override the default plughw:0,0 choice*/
	int autokill:1;  /* 1= automatically free access to sound device by killing artsd and esd*/
} sound_config_t;

typedef struct codecs_config
{
	GList *audio_codecs;  /* list of audio codecs in order of preference*/
	GList *video_codecs;	/* for later use*/
}codecs_config_t;

typedef struct video_config{
	gchar *device;
	gboolean enabled;
	gboolean show_local;
}video_config_t;

typedef struct ui_config
{
	int is_daemon;
	int is_applet;
	guint timer_id;  /* the timer id for registration */
}ui_config_t;



typedef struct autoreplier_config
{
	gint enabled;
	gint after_seconds;		/* accept the call after x seconds*/
	gint max_users;			/* maximum number of user that can call simultaneously */
	gint max_rec_time;  	/* the max time of incoming voice recorded */
	gint max_rec_msg;		/* maximum number of recorded messages */
	const gchar *message;		/* the path of the file to be played */
}autoreplier_config_t;


struct _LinphoneCore;
struct _sdp_context;
	
typedef struct _StreamParams
{
	int initialized;
	int line;
	int localport;
	int remoteport;
	char *remoteaddr;
	int pt;
} StreamParams;

typedef enum _LCState{
	LCStateInit,
	LCStateRinging,
	LCStateAVRunning
}LCState;

typedef enum _LinphoneCallDir {LinphoneCallOutgoing, LinphoneCallIncoming} LinphoneCallDir;


typedef struct _LinphoneCall
{
	struct _LinphoneCore *core;
	StreamParams audio_params;
	StreamParams video_params;
	LinphoneCallDir dir;
	struct _RtpProfile *profile;	/*points to the local_profile or to the remote "guessed" profile*/
	struct _LinphoneCallLog *log;
	int cid; /*call id */
	int did; /*dialog id */
	struct _sdp_context *sdpctx;
	time_t start_time;
	LCState	state;
	gboolean auth_pending;	
} LinphoneCall;

LinphoneCall * linphone_call_new_outgoing(struct _LinphoneCore *lc, const osip_from_t *from, const osip_to_t *to);
LinphoneCall * linphone_call_new_incoming(struct _LinphoneCore *lc, const char *from, const char *to,
											int cid, int did);
#define linphone_call_set_state(lcall,st)	(lcall)->state=(st)
void linphone_call_destroy(struct _LinphoneCall *obj);


typedef enum _LinphoneCallStatus { 
	LinphoneCallSuccess,
	LinphoneCallAborted,
	LinphoneCallMissed
} LinphoneCallStatus;

typedef struct _LinphoneCallLog{
	struct _LinphoneCore *lc;
	LinphoneCallDir dir;
	LinphoneCallStatus status;
	char *from;
	char *to;
	char *start_date;
	int duration;
	
} LinphoneCallLog;

/* private: */
LinphoneCallLog * linphone_call_log_new(LinphoneCall *call, char *local, char * remote);
void linphone_call_log_completed(LinphoneCallLog *calllog, LinphoneCall *call);
void linphone_call_log_destroy(LinphoneCallLog *cl);

/*public: */
gchar * linphone_call_log_to_str(LinphoneCallLog *cl);

typedef enum{
	LinphoneSPWait,
	LinphoneSPDeny,
	LinphoneSPAccept
}LinphoneSubscribePolicy;

typedef struct _LinphoneFriend{
	osip_from_t *url;
	char *contact;	/* not stored, use for immediate outgoing subscribe after receiving incoming subscribe*/
	int sid;
	int nid;
	int last_outsubsc;
	LinphoneSubscribePolicy pol;
	struct _LinphoneProxyConfig *proxy;
	struct _LinphoneCore *lc;
	gboolean subscribe;
	gboolean inc_subscribe_pending;
}LinphoneFriend;	

LinphoneFriend * linphone_friend_new();
LinphoneFriend *linphone_friend_new_with_addr(const gchar *addr);
gint linphone_friend_set_sip_addr(LinphoneFriend *fr, const gchar *name);
gint linphone_friend_send_subscribe(LinphoneFriend *fr, gboolean val);
gint linphone_friend_set_inc_subscribe_policy(LinphoneFriend *fr, LinphoneSubscribePolicy pol);
gint linphone_friend_set_proxy(LinphoneFriend *fr, struct _LinphoneProxyConfig *cfg);
void linphone_friend_edit(LinphoneFriend *fr);
void linphone_friend_done(LinphoneFriend *fr);
void linphone_friend_destroy(LinphoneFriend *lf);
/* memory returned by those 3 functions must be freed */
gchar *linphone_friend_get_name(LinphoneFriend *lf);
gchar *linphone_friend_get_addr(LinphoneFriend *lf);
gchar *linphone_friend_get_url(LinphoneFriend *lf);	/* name <sip address> */
#define linphone_friend_url(lf) ((lf)->url)

void linphone_friend_write_to_config_file(struct _LpConfig *config, LinphoneFriend *lf, int index);
LinphoneFriend * linphone_friend_new_from_config_file(struct _LinphoneCore *lc, int index);

typedef struct _LinphoneProxyConfig
{
	struct _LinphoneCore *lc;
	gchar *reg_proxy;
	gchar *reg_identity;
	gchar *reg_route;
	gchar *realm;
	int expires;
	int reg_time;
	int rid;
	gboolean frozen;
	gboolean reg_sendregister;
	gboolean auth_pending;
	gboolean registered;
	gboolean publish;
} LinphoneProxyConfig;

LinphoneProxyConfig *linphone_proxy_config_new(const gchar *server_addr);
gint linphone_proxy_config_set_server_addr(LinphoneProxyConfig *obj, const gchar *server_addr);
void linphone_proxy_config_set_identity(LinphoneProxyConfig *obj, const gchar *identity);
void linphone_proxy_config_set_route(LinphoneProxyConfig *obj, const gchar *route);
void linphone_proxy_config_expires(LinphoneProxyConfig *obj, const gint expires);
void linphone_proxy_config_enableregister(LinphoneProxyConfig *obj, gboolean val);
void linphone_proxy_config_edit(LinphoneProxyConfig *obj);
void linphone_proxy_config_done(LinphoneProxyConfig *obj);
void linphone_proxy_config_enable_publish(LinphoneProxyConfig *obj, gboolean val);
#define linphone_proxy_config_get_route(obj)  ((obj)->reg_route)
#define linphone_proxy_config_get_identity(obj)	((obj)->reg_identity)
#define linphone_proxy_config_publish_enabled(obj) ((obj)->publish)
#define linphone_proxy_config_get_addr(obj) ((obj)->reg_proxy)

/* destruction is called automatically when removing the proxy config */
void linphone_proxy_config_destroy(LinphoneProxyConfig *cfg);
LinphoneProxyConfig *linphone_proxy_config_new_from_config_file(struct _LpConfig *config, int index);
void linphone_proxy_config_write_to_config_file(struct _LpConfig* config,LinphoneProxyConfig *obj, int index);

typedef struct _LinphoneAuthInfo
{
	gchar *username;
	gchar *realm;
	gchar *userid;
	gchar *passwd;
	gchar *ha1;
	gboolean works;
}LinphoneAuthInfo;

LinphoneAuthInfo *linphone_auth_info_new(const gchar *username, const gchar *userid,
				   										const gchar *passwd, const gchar *ha1,const gchar *realm);
void linphone_auth_info_set_passwd(LinphoneAuthInfo *info, const gchar *passwd);
/* you don't need those function*/
void linphone_auth_info_destroy(LinphoneAuthInfo *info);
LinphoneAuthInfo * linphone_auth_info_new_from_config_file(struct _LpConfig *config, int pos);

struct _LinphoneChatRoom{
	struct _LinphoneCore *lc;
	char  *peer;
	osip_from_t *peer_url;
	gpointer user_data;
};
typedef struct _LinphoneChatRoom LinphoneChatRoom;

LinphoneChatRoom * linphone_core_create_chat_room(struct _LinphoneCore *lc, const char *to);
void linphone_chat_room_send_message(LinphoneChatRoom *cr, const char *msg);
void linphone_chat_room_destroy(LinphoneChatRoom *cr);
void linphone_chat_room_set_user_data(LinphoneChatRoom *cr, gpointer ud);
gpointer linphone_chat_room_get_user_data(LinphoneChatRoom *cr);

typedef void (*ShowInterfaceCb)(struct _LinphoneCore *lc);
typedef void (*InviteReceivedCb)(struct _LinphoneCore *lc, const char *from);
typedef void (*ByeReceivedCb)(struct _LinphoneCore *lc, const char *from);
typedef void (*DisplayStatusCb)(struct _LinphoneCore *lc, char *message);
typedef void (*DisplayMessageCb)(struct _LinphoneCore *lc, char *message);
typedef void (*DisplayUrlCb)(struct _LinphoneCore *lc, char *message,char *url);
typedef void (*DisplayQuestionCb)(struct _LinphoneCore *lc, char *message);
typedef void (*LinphoneCoreCbFunc)(struct _LinphoneCore *lc,gpointer user_data);
typedef void (*NotifyReceivedCb)(struct _LinphoneCore *lc, LinphoneFriend * fid, const char *url, const char *status, const char *img);
typedef void (*NewUnknownSubscriberCb)(struct _LinphoneCore *lc, LinphoneFriend *lf, const char *url);
typedef void (*AuthInfoRequested)(struct _LinphoneCore *lc, const char *realm, const char *username);
typedef void (*CallLogUpdated)(struct _LinphoneCore *lc, struct _LinphoneCallLog *newcl);
typedef void (*TextMessageReceived)(struct _LinphoneCore *lc, LinphoneChatRoom *room, const gchar *from, const gchar *message);
typedef struct _LinphoneVTable
{
	ShowInterfaceCb show;
	InviteReceivedCb inv_recv;
	ByeReceivedCb bye_recv;
	NotifyReceivedCb notify_recv;
	NewUnknownSubscriberCb new_unknown_subscriber;
	AuthInfoRequested auth_info_requested;
	DisplayStatusCb display_status;
	DisplayMessageCb display_message;
#ifdef VINCENT_MAURY_RSVP
	/* the yes/no dialog box */
	DisplayMessageCb display_yes_no;
#endif
	DisplayMessageCb display_warning;
	DisplayUrlCb display_url;
	DisplayQuestionCb display_question;
	CallLogUpdated call_log_updated;
	TextMessageReceived text_received;
} LinphoneCoreVTable;

typedef struct _LCCallbackObj
{
  LinphoneCoreCbFunc _func;
  gpointer _user_data;
}LCCallbackObj;

typedef enum _LinphoneOnlineStatus{
	LINPHONE_STATUS_UNKNOWN,
	LINPHONE_STATUS_ONLINE,
	LINPHONE_STATUS_BUSY,
	LINPHONE_STATUS_BERIGHTBACK,
	LINPHONE_STATUS_AWAY,
	LINPHONE_STATUS_ONTHEPHONE,
	LINPHONE_STATUS_OUTTOLUNCH,
	LINPHONE_STATUS_NOT_DISTURB,
	LINPHONE_STATUS_MOVED,
	LINPHONE_STATUS_ALT_SERVICE,
	LINPHONE_STATUS_OFFLINE,
	LINPHONE_STATUS_END
}LinphoneOnlineStatus;

typedef struct _LinphoneCore
{
	LinphoneCoreVTable vtable;
	struct _LpConfig *config;
	net_config_t net_conf;
	sip_config_t sip_conf;
	rtp_config_t rtp_conf;
	sound_config_t sound_conf;
	video_config_t video_conf;
	codecs_config_t codecs_conf;
	ui_config_t ui_conf;
	autoreplier_config_t autoreplier_conf;
	LinphoneProxyConfig *default_proxy;
	GList *friends;
	GList *auth_info;
	struct _RingStream *ringstream;
	LCCallbackObj preview_finished_cb;
	gboolean preview_finished;
	LinphoneCall *call;   /* the current call, in the future it will be a list of calls (conferencing)*/
	int rid; /*registration id*/
	GList *queued_calls;	/* used by the autoreplier */
	GList *call_logs;
	GList *chatrooms;
	int max_call_logs;
	int missed_calls;
	struct _AudioStream *audiostream;  /**/
	struct _VideoStream *videostream;
	struct _VideoStream *previewstream;
	struct _RtpProfile *local_profile;
	GList *subscribers;	/* unknown subscribers */
	int minutes_away;
	LinphoneOnlineStatus	presence_mode;
	char *alt_contact;
	int in_main_thread;
	void *data;
	GMutex *lock;
#ifdef VINCENT_MAURY_RSVP
	/* QoS parameters*/
	int rsvp_enable;
	int rpc_enable;
#endif
} LinphoneCore;


/* The legacy API */

void linphone_core_enable_logs(FILE *file);
void linphone_core_disable_logs();

LinphoneCore *linphone_core_new(LinphoneCoreVTable *vtable,
						const gchar *config_path, gpointer userdata);

void linphone_core_init(LinphoneCore *lc,LinphoneCoreVTable *vtable,
						const gchar *config_path, gpointer userdata);

int linphone_core_set_primary_contact(LinphoneCore *lc, const gchar *contact);

const gchar *linphone_core_get_primary_contact(LinphoneCore *lc);

void linphone_core_set_guess_hostname(LinphoneCore *lc, gboolean val);
gboolean linphone_core_get_guess_hostname(LinphoneCore *lc);

gboolean linphone_core_ipv6_enabled(LinphoneCore *lc);
void linphone_core_enable_ipv6(LinphoneCore *lc, gboolean val);

osip_from_t *linphone_core_get_primary_contact_parsed(LinphoneCore *lc);


#ifdef LINPHONE_DEPRECATED
GList * linphone_core_get_net_interfaces(LinphoneCore *lc);

interface_t * linphone_core_get_selected_interface(LinphoneCore *lc);

#ifdef INET6
void linphone_core_select_net_interface(LinphoneCore *lc, gint index);
#else
void linphone_core_select_net_interface(LinphoneCore *lc, gchar *ifname);
#endif

#endif

void linphone_core_set_connection_type(LinphoneCore *lc, int type);

int linphone_core_get_connection_type(LinphoneCore *lc);

int linphone_core_invite(LinphoneCore *lc, const gchar *url);

int linphone_core_refer(LinphoneCore *lc, const gchar *url);

#ifdef VINCENT_MAURY_RSVP
/* QoS functions */
int linphone_core_set_rpc_mode(LinphoneCore *lc, int on); /* on = 1 (RPC_ENABLE = 1) */
int linphone_core_set_rsvp_mode(LinphoneCore *lc, int on); /* on = 1 (RSVP_ENABLE = 1) */
int linphone_core_change_qos(LinphoneCore *lc, int answer); /* answer = 1 for yes, 0 for no */
#endif
gboolean linphone_core_inc_invite_pending(LinphoneCore*lc);

int linphone_core_accept_call(LinphoneCore *lc, const char *url);

int linphone_core_terminate_call(LinphoneCore *lc, const char *url);

void linphone_core_send_dtmf(LinphoneCore *lc,gchar dtmf);

/* returns a GList of MSCodecInfo */
GList *linphone_core_get_audio_codecs(LinphoneCore *lc);

int linphone_core_set_audio_codecs(LinphoneCore *lc, GList *codecs);

/* returns a GList of MSCodecInfo */
GList *linphone_core_get_video_codecs(LinphoneCore *lc);

int linphone_core_set_video_codecs(LinphoneCore *lc, GList *codecs);

void linphone_core_add_proxy_config(LinphoneCore *lc, LinphoneProxyConfig *config);

void linphone_core_remove_proxy_config(LinphoneCore *lc, LinphoneProxyConfig *config);

GList *linphone_core_get_proxy_config_list(LinphoneCore *lc);

void linphone_core_set_default_proxy(LinphoneCore *lc, LinphoneProxyConfig *config);

void linphone_core_set_default_proxy_index(LinphoneCore *lc, int index);

int linphone_core_get_default_proxy(LinphoneCore *lc, LinphoneProxyConfig **config);

void linphone_core_add_auth_info(LinphoneCore *lc, LinphoneAuthInfo *info);

void linphone_core_remove_auth_info(LinphoneCore *lc, LinphoneAuthInfo *info);

void linphone_core_clear_all_auth_info(LinphoneCore *lc);

int linphone_core_get_audio_jittcomp(LinphoneCore *lc);

int linphone_core_get_audio_port(LinphoneCore *lc);

void linphone_core_set_audio_jittcomp(LinphoneCore *lc, int value);

void linphone_core_set_audio_port(LinphoneCore *lc, int port);

void linphone_core_set_video_port(LinphoneCore *lc, int port);

void linphone_core_set_use_info_for_dtmf(LinphoneCore *lc, gboolean use_info);

gboolean linphone_core_get_use_info_for_dtmf(LinphoneCore *lc);

int linphone_core_get_sip_port(LinphoneCore *lc);

void linphone_core_set_sip_port(LinphoneCore *lc,int port);

void linphone_core_set_inc_timeout(LinphoneCore *lc, int seconds);

int linphone_core_get_inc_timeout(LinphoneCore *lc);

/* give a NULL addr to only set nat use */
void linphone_core_set_nat_address(LinphoneCore *lc, const gchar *addr, gboolean use);

const gchar *linphone_core_get_nat_address(LinphoneCore *lc, gboolean *use);

const gchar *linphone_core_get_nat_address_if_used(LinphoneCore *lc);

void linphone_core_force_ip_address(LinphoneCore *lc, const gchar *ipaddr);

/* function to be periodically called in a main loop */
void linphone_core_iterate(LinphoneCore *lc);

/* sound functions */
SndCardManager * linphone_core_get_card_manager(LinphoneCore *lc);
gint linphone_core_get_ring_level(LinphoneCore *lc);
gint linphone_core_get_play_level(LinphoneCore *lc);
gint linphone_core_get_rec_level(LinphoneCore *lc);
void linphone_core_set_ring_level(LinphoneCore *lc, gint level);
void linphone_core_set_play_level(LinphoneCore *lc, gint level);
void linphone_core_set_rec_level(LinphoneCore *lc, gint level);
gint linphone_core_get_ringer_device(LinphoneCore *lc);
gint linphone_core_get_playback_device(LinphoneCore *lc);
gint linphone_core_get_capture_device(LinphoneCore *lc);
gint linphone_core_set_ringer_device(LinphoneCore *lc, gint devid);
gint linphone_core_set_playback_device(LinphoneCore *lc, gint devid);
gint linphone_core_set_capture_device(LinphoneCore *lc, gint devid);
gint linphone_core_set_ringer_device_from_name(LinphoneCore *lc, const gchar *name);
gint linphone_core_set_playback_device_from_name(LinphoneCore *lc, const gchar *name);
gint linphone_core_set_capture_device_from_name(LinphoneCore *lc, const gchar *name);
gchar linphone_core_get_sound_source(LinphoneCore *lc);
void linphone_core_set_sound_source(LinphoneCore *lc, gchar source);
void linphone_core_set_ring(LinphoneCore *lc, const gchar *path);
gchar *linphone_core_get_ring(LinphoneCore *lc);
gint linphone_core_preview_ring(LinphoneCore *lc, const gchar *ring,LinphoneCoreCbFunc func,gpointer userdata);
typedef enum {RINGBACK_TYPE_FR,RINGBACK_TYPE_US} RingBackType;
void linphone_core_set_ringback(LinphoneCore *lc,RingBackType type);
RingBackType linphone_core_get_ringback(LinphoneCore *lc);

void linphone_core_set_presence_info(LinphoneCore *lc,gint minutes_away,
													const gchar *contact,
													LinphoneOnlineStatus os);

void linphone_core_add_friend(LinphoneCore *lc, LinphoneFriend *fr);
void linphone_core_remove_friend(LinphoneCore *lc, LinphoneFriend *fr);
void linphone_core_reject_subscriber(LinphoneCore *lc, LinphoneFriend *lf);
/* a list of LinphoneFriend */
GList * linphone_core_get_friend_list(LinphoneCore *lc);
/* notify all friends that have subscribed */
void linphone_core_notify_all_friends(LinphoneCore *lc, LinphoneOnlineStatus os);

/* returns a list of LinphoneCallLog */
GList * linphone_core_get_call_logs(LinphoneCore *lc);

/* video support */
void linphone_core_enable_video(LinphoneCore *lc, gboolean val);
gboolean linphone_core_video_enabled(LinphoneCore *lc);

void linphone_core_enable_video_preview(LinphoneCore *lc, gboolean val);
gboolean linphone_core_video_preview_enabled(LinphoneCore *lc);

int linphone_core_set_video_device(LinphoneCore *lc, const gchar *method, const gchar *device);

gboolean linphone_core_is_in_main_thread(LinphoneCore *lc);

void *linphone_core_get_user_data(LinphoneCore *lc);

/* returns LpConfig object to read/write to the config file: usefull if you wish to extend
the config file with your own sections */
struct _LpConfig *linphone_core_get_config(LinphoneCore *lc);

void linphone_core_uninit(LinphoneCore *lc);
void linphone_core_destroy(LinphoneCore *lc);

/* end of lecacy api */

/*internal use only */
#define linphone_core_thread_enter(lc) (lc)->in_main_thread=0
#define linphone_core_thread_leave(lc) (lc)->in_main_thread=1
#define linphone_core_lock(lc)	g_mutex_lock((lc)->lock)
#define linphone_core_unlock(lc)	g_mutex_unlock((lc)->lock)
void linphone_core_start_media_streams(LinphoneCore *lc, LinphoneCall *call);
void linphone_core_stop_media_streams(LinphoneCore *lc);
const char * linphone_core_get_identity(LinphoneCore *lc);
const char * linphone_core_get_route(LinphoneCore *lc);
gboolean linphone_core_interpret_url(LinphoneCore *lc, const char *url, char **real_url, osip_to_t **real_parsed_url);

#ifdef __cplusplus
}
#endif

#endif
