/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2006, Digium, Inc.
 *
 * Mark Spencer <markster@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*! \file
 *
 * \brief Routines implementing call features as call pickup, parking and transfer
 *
 * \author Mark Spencer <markster@digium.com> 
 */

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision: 77778 $")

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <netinet/in.h>

#include "asterisk/lock.h"
#include "asterisk/file.h"
#include "asterisk/logger.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/options.h"
#include "asterisk/causes.h"
#include "asterisk/module.h"
#include "asterisk/translate.h"
#include "asterisk/app.h"
#include "asterisk/say.h"
#include "asterisk/features.h"
#include "asterisk/musiconhold.h"
#include "asterisk/config.h"
#include "asterisk/cli.h"
#include "asterisk/manager.h"
#include "asterisk/utils.h"
#include "asterisk/adsi.h"
#include "asterisk/devicestate.h"
#include "asterisk/monitor.h"

#define DEFAULT_PARK_TIME 45000
#define DEFAULT_TRANSFER_DIGIT_TIMEOUT 3000
#define DEFAULT_FEATURE_DIGIT_TIMEOUT 500
#define DEFAULT_NOANSWER_TIMEOUT_ATTENDED_TRANSFER 15000

#define AST_MAX_WATCHERS 256

enum {
	AST_FEATURE_FLAG_NEEDSDTMF = (1 << 0),
	AST_FEATURE_FLAG_ONPEER =    (1 << 1),
	AST_FEATURE_FLAG_ONSELF =    (1 << 2),
	AST_FEATURE_FLAG_BYCALLEE =  (1 << 3),
	AST_FEATURE_FLAG_BYCALLER =  (1 << 4),
	AST_FEATURE_FLAG_BYBOTH	 =   (3 << 3),
};

static char *parkedcall = "ParkedCall";

static int parkaddhints = 0;                               /*!< Add parking hints automatically */
static int parkingtime = DEFAULT_PARK_TIME;                /*!< No more than 45 seconds parked before you do something with them */
static char parking_con[AST_MAX_EXTENSION];                /*!< Context for which parking is made accessible */
static char parking_con_dial[AST_MAX_EXTENSION];           /*!< Context for dialback for parking (KLUDGE) */
static char parking_ext[AST_MAX_EXTENSION];                /*!< Extension you type to park the call */
static char pickup_ext[AST_MAX_EXTENSION];                 /*!< Call pickup extension */
static char parkmohclass[MAX_MUSICCLASS];                  /*!< Music class used for parking */
static int parking_start;                                  /*!< First available extension for parking */
static int parking_stop;                                   /*!< Last available extension for parking */

static char courtesytone[256];                             /*!< Courtesy tone */
static int parkedplay = 0;                                 /*!< Who to play the courtesy tone to */
static char xfersound[256];                                /*!< Call transfer sound */
static char xferfailsound[256];                            /*!< Call transfer failure sound */

static int parking_offset;
static int parkfindnext;

static int adsipark;

static int transferdigittimeout;
static int featuredigittimeout;

static int atxfernoanswertimeout;

static char *registrar = "res_features";		   /*!< Registrar for operations */

/* module and CLI command definitions */
static char *synopsis = "Answer a parked call";

static char *descrip = "ParkedCall(exten):"
"Used to connect to a parked call.  This application is always\n"
"registered internally and does not need to be explicitly added\n"
"into the dialplan, although you should include the 'parkedcalls'\n"
"context.\n";

static char *parkcall = "Park";

static char *synopsis2 = "Park yourself";

static char *descrip2 = "Park():"
"Used to park yourself (typically in combination with a supervised\n"
"transfer to know the parking space). This application is always\n"
"registered internally and does not need to be explicitly added\n"
"into the dialplan, although you should include the 'parkedcalls'\n"
"context (or the context specified in features.conf).\n\n"
"If you set the PARKINGEXTEN variable to an extension in your\n"
"parking context, park() will park the call on that extension, unless\n"
"it already exists. In that case, execution will continue at next\n"
"priority.\n" ;

static struct ast_app *monitor_app = NULL;
static int monitor_ok = 1;

struct parkeduser {
	struct ast_channel *chan;                   /*!< Parking channel */
	struct timeval start;                       /*!< Time the parking started */
	int parkingnum;                             /*!< Parking lot */
	char parkingexten[AST_MAX_EXTENSION];       /*!< If set beforehand, parking extension used for this call */
	char context[AST_MAX_CONTEXT];              /*!< Where to go if our parking time expires */
	char exten[AST_MAX_EXTENSION];
	int priority;
	int parkingtime;                            /*!< Maximum length in parking lot before return */
	int notquiteyet;
	char peername[1024];
	unsigned char moh_trys;
	struct parkeduser *next;
};

static struct parkeduser *parkinglot;

AST_MUTEX_DEFINE_STATIC(parking_lock);	/*!< protects all static variables above */

static pthread_t parking_thread;

char *ast_parking_ext(void)
{
	return parking_ext;
}

char *ast_pickup_ext(void)
{
	return pickup_ext;
}

struct ast_bridge_thread_obj 
{
	struct ast_bridge_config bconfig;
	struct ast_channel *chan;
	struct ast_channel *peer;
};



/*! \brief store context, priority and extension */
static void set_c_e_p(struct ast_channel *chan, const char *context, const char *ext, int pri)
{
	ast_copy_string(chan->context, context, sizeof(chan->context));
	ast_copy_string(chan->exten, ext, sizeof(chan->exten));
	chan->priority = pri;
}

static void check_goto_on_transfer(struct ast_channel *chan) 
{
	struct ast_channel *xferchan;
	const char *val = pbx_builtin_getvar_helper(chan, "GOTO_ON_BLINDXFR");
	char *x, *goto_on_transfer;
	struct ast_frame *f;

	if (ast_strlen_zero(val))
		return;

	goto_on_transfer = ast_strdupa(val);

	if (!(xferchan = ast_channel_alloc(0, AST_STATE_DOWN, 0, 0, "", "", "", 0, chan->name)))
		return;

	for (x = goto_on_transfer; x && *x; x++) {
		if (*x == '^')
			*x = '|';
	}
	/* Make formats okay */
	xferchan->readformat = chan->readformat;
	xferchan->writeformat = chan->writeformat;
	ast_channel_masquerade(xferchan, chan);
	ast_parseable_goto(xferchan, goto_on_transfer);
	xferchan->_state = AST_STATE_UP;
	ast_clear_flag(xferchan, AST_FLAGS_ALL);	
	xferchan->_softhangup = 0;
	if ((f = ast_read(xferchan))) {
		ast_frfree(f);
		f = NULL;
		ast_pbx_start(xferchan);
	} else {
		ast_hangup(xferchan);
	}
}

static struct ast_channel *ast_feature_request_and_dial(struct ast_channel *caller, const char *type, int format, void *data, int timeout, int *outstate, const char *cid_num, const char *cid_name);


static void *ast_bridge_call_thread(void *data) 
{
	struct ast_bridge_thread_obj *tobj = data;

	tobj->chan->appl = "Transferred Call";
	tobj->chan->data = tobj->peer->name;
	tobj->peer->appl = "Transferred Call";
	tobj->peer->data = tobj->chan->name;
	if (tobj->chan->cdr) {
		ast_cdr_reset(tobj->chan->cdr, NULL);
		ast_cdr_setdestchan(tobj->chan->cdr, tobj->peer->name);
	}
	if (tobj->peer->cdr) {
		ast_cdr_reset(tobj->peer->cdr, NULL);
		ast_cdr_setdestchan(tobj->peer->cdr, tobj->chan->name);
	}

	ast_bridge_call(tobj->peer, tobj->chan, &tobj->bconfig);
	ast_hangup(tobj->chan);
	ast_hangup(tobj->peer);
	bzero(tobj, sizeof(*tobj)); /*! \todo XXX for safety */
	free(tobj);
	return NULL;
}

static void ast_bridge_call_thread_launch(void *data) 
{
	pthread_t thread;
	pthread_attr_t attr;
	struct sched_param sched;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ast_pthread_create(&thread, &attr,ast_bridge_call_thread, data);
	pthread_attr_destroy(&attr);
	memset(&sched, 0, sizeof(sched));
	pthread_setschedparam(thread, SCHED_RR, &sched);
}

static int adsi_announce_park(struct ast_channel *chan, char *parkingexten)
{
	int res;
	int justify[5] = {ADSI_JUST_CENT, ADSI_JUST_CENT, ADSI_JUST_CENT, ADSI_JUST_CENT};
	char tmp[256];
	char *message[5] = {NULL, NULL, NULL, NULL, NULL};

	snprintf(tmp, sizeof(tmp), "Parked on %s", parkingexten);
	message[0] = tmp;
	res = ast_adsi_load_session(chan, NULL, 0, 1);
	if (res == -1)
		return res;
	return ast_adsi_print(chan, message, justify, 1);
}

/*! \brief Notify metermaids that we've changed an extension */
static void notify_metermaids(char *exten, char *context)
{
	if (option_debug > 3)
		ast_log(LOG_DEBUG, "Notification of state change to metermaids %s@%s\n", exten, context);

	/* Send notification to devicestate subsystem */
	ast_device_state_changed("park:%s@%s", exten, context);
	return;
}

/*! \brief metermaids callback from devicestate.c */
static int metermaidstate(const char *data)
{
	int res = AST_DEVICE_INVALID;
	char *context = ast_strdupa(data);
	char *exten;

	exten = strsep(&context, "@");
	if (!context)
		return res;
	
	if (option_debug > 3)
		ast_log(LOG_DEBUG, "Checking state of exten %s in context %s\n", exten, context);

	res = ast_exists_extension(NULL, context, exten, 1, NULL);

	if (!res)
		return AST_DEVICE_NOT_INUSE;
	else
		return AST_DEVICE_INUSE;
}

/*! \brief Park a call 
 	\note We put the user in the parking list, then wake up the parking thread to be sure it looks
	after these channels too */
int ast_park_call(struct ast_channel *chan, struct ast_channel *peer, int timeout, int *extout)
{
	struct parkeduser *pu, *cur;
	int i, x = -1, parking_range;
	struct ast_context *con;
	const char *parkingexten;
	
	/* Allocate memory for parking data */
	if (!(pu = ast_calloc(1, sizeof(*pu)))) 
		return -1;

	/* Lock parking lot */
	ast_mutex_lock(&parking_lock);
	/* Check for channel variable PARKINGEXTEN */
	parkingexten = pbx_builtin_getvar_helper(chan, "PARKINGEXTEN");
	if (!ast_strlen_zero(parkingexten)) {
		if (ast_exists_extension(NULL, parking_con, parkingexten, 1, NULL)) {
			ast_mutex_unlock(&parking_lock);
			free(pu);
			ast_log(LOG_WARNING, "Requested parking extension already exists: %s@%s\n", parkingexten, parking_con);
			return -1; /* We failed to park this call, plain and simple so we need to error out */
		}
		ast_copy_string(pu->parkingexten, parkingexten, sizeof(pu->parkingexten));
		x = atoi(parkingexten);
	} else {
		/* Select parking space within range */
		parking_range = parking_stop - parking_start+1;
		for (i = 0; i < parking_range; i++) {
			x = (i + parking_offset) % parking_range + parking_start;
			cur = parkinglot;
			while(cur) {
				if (cur->parkingnum == x) 
					break;
				cur = cur->next;
			}
			if (!cur)
				break;
		}

		if (!(i < parking_range)) {
			ast_log(LOG_WARNING, "No more parking spaces\n");
			free(pu);
			ast_mutex_unlock(&parking_lock);
			return -1;
		}
		/* Set pointer for next parking */
		if (parkfindnext) 
			parking_offset = x - parking_start + 1;
	}
	
	chan->appl = "Parked Call";
	chan->data = NULL; 

	pu->chan = chan;
	
	/* Put the parked channel on hold if we have two different channels */
	if (chan != peer) {
		ast_indicate_data(pu->chan, AST_CONTROL_HOLD, 
			S_OR(parkmohclass, NULL),
			!ast_strlen_zero(parkmohclass) ? strlen(parkmohclass) + 1 : 0);
	}
	
	pu->start = ast_tvnow();
	pu->parkingnum = x;
	pu->parkingtime = (timeout > 0) ? timeout : parkingtime;
	if (extout)
		*extout = x;

	if (peer) 
		ast_copy_string(pu->peername, peer->name, sizeof(pu->peername));

	/* Remember what had been dialed, so that if the parking
	   expires, we try to come back to the same place */
	ast_copy_string(pu->context, S_OR(chan->macrocontext, chan->context), sizeof(pu->context));
	ast_copy_string(pu->exten, S_OR(chan->macroexten, chan->exten), sizeof(pu->exten));
	pu->priority = chan->macropriority ? chan->macropriority : chan->priority;
	pu->next = parkinglot;
	parkinglot = pu;

	/* If parking a channel directly, don't quiet yet get parking running on it */
	if (peer == chan) 
		pu->notquiteyet = 1;
	ast_mutex_unlock(&parking_lock);
	/* Wake up the (presumably select()ing) thread */
	pthread_kill(parking_thread, SIGURG);
	if (option_verbose > 1) 
		ast_verbose(VERBOSE_PREFIX_2 "Parked %s on %d@%s. Will timeout back to extension [%s] %s, %d in %d seconds\n", pu->chan->name, pu->parkingnum, parking_con, pu->context, pu->exten, pu->priority, (pu->parkingtime/1000));

	if (pu->parkingnum != -1)
		snprintf(pu->parkingexten, sizeof(pu->parkingexten), "%d", x);
	manager_event(EVENT_FLAG_CALL, "ParkedCall",
		"Exten: %s\r\n"
		"Channel: %s\r\n"
		"From: %s\r\n"
		"Timeout: %ld\r\n"
		"CallerID: %s\r\n"
		"CallerIDName: %s\r\n",
		pu->parkingexten, pu->chan->name, peer ? peer->name : "",
		(long)pu->start.tv_sec + (long)(pu->parkingtime/1000) - (long)time(NULL),
		S_OR(pu->chan->cid.cid_num, "<unknown>"),
		S_OR(pu->chan->cid.cid_name, "<unknown>")
		);

	if (peer && adsipark && ast_adsi_available(peer)) {
		adsi_announce_park(peer, pu->parkingexten);	/* Only supports parking numbers */
		ast_adsi_unload_session(peer);
	}

	con = ast_context_find(parking_con);
	if (!con) 
		con = ast_context_create(NULL, parking_con, registrar);
	if (!con)	/* Still no context? Bad */
		ast_log(LOG_ERROR, "Parking context '%s' does not exist and unable to create\n", parking_con);
	/* Tell the peer channel the number of the parking space */
	if (peer && pu->parkingnum != -1) /* Only say number if it's a number */
		ast_say_digits(peer, pu->parkingnum, "", peer->language);
	if (con) {
		if (!ast_add_extension2(con, 1, pu->parkingexten, 1, NULL, NULL, parkedcall, strdup(pu->parkingexten), ast_free, registrar))
			notify_metermaids(pu->parkingexten, parking_con);
	}
	if (pu->notquiteyet) {
		/* Wake up parking thread if we're really done */
		ast_indicate_data(pu->chan, AST_CONTROL_HOLD, 
			S_OR(parkmohclass, NULL),
			!ast_strlen_zero(parkmohclass) ? strlen(parkmohclass) + 1 : 0);
		pu->notquiteyet = 0;
		pthread_kill(parking_thread, SIGURG);
	}
	return 0;
}

int ast_masq_park_call(struct ast_channel *rchan, struct ast_channel *peer, int timeout, int *extout)
{
	struct ast_channel *chan;
	struct ast_frame *f;

	/* Make a new, fake channel that we'll use to masquerade in the real one */
	if (!(chan = ast_channel_alloc(0, AST_STATE_DOWN, 0, 0, rchan->accountcode, rchan->exten, rchan->context, rchan->amaflags, "Parked/%s",rchan->name))) {
		ast_log(LOG_WARNING, "Unable to create parked channel\n");
		return -1;
	}

	/* Make formats okay */
	chan->readformat = rchan->readformat;
	chan->writeformat = rchan->writeformat;
	ast_channel_masquerade(chan, rchan);

	/* Setup the extensions and such */
	set_c_e_p(chan, rchan->context, rchan->exten, rchan->priority);

	/* Make the masq execute */
	f = ast_read(chan);
	if (f)
		ast_frfree(f);

	ast_park_call(chan, peer, timeout, extout);
	return 0;
}


#define FEATURE_RETURN_HANGUP		-1
#define FEATURE_RETURN_SUCCESSBREAK	 0
#define FEATURE_RETURN_PBX_KEEPALIVE	AST_PBX_KEEPALIVE
#define FEATURE_RETURN_NO_HANGUP_PEER	AST_PBX_NO_HANGUP_PEER
#define FEATURE_RETURN_PASSDIGITS	 21
#define FEATURE_RETURN_STOREDIGITS	 22
#define FEATURE_RETURN_SUCCESS	 	 23

#define FEATURE_SENSE_CHAN	(1 << 0)
#define FEATURE_SENSE_PEER	(1 << 1)

/*! \brief
 * set caller and callee according to the direction
 */
static void set_peers(struct ast_channel **caller, struct ast_channel **callee,
	struct ast_channel *peer, struct ast_channel *chan, int sense)
{
	if (sense == FEATURE_SENSE_PEER) {
		*caller = peer;
		*callee = chan;
	} else {
		*callee = peer;
		*caller = chan;
	}
}

/*! \brief support routing for one touch call parking */
static int builtin_parkcall(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	struct ast_channel *parker;
        struct ast_channel *parkee;
	int res = 0;
	struct ast_module_user *u;

	u = ast_module_user_add(chan);

	set_peers(&parker, &parkee, peer, chan, sense);
	/* Setup the exten/priority to be s/1 since we don't know
	   where this call should return */
	strcpy(chan->exten, "s");
	chan->priority = 1;
	if (chan->_state != AST_STATE_UP)
		res = ast_answer(chan);
	if (!res)
		res = ast_safe_sleep(chan, 1000);
	if (!res)
		res = ast_park_call(parkee, parker, 0, NULL);

	ast_module_user_remove(u);

	if (!res) {
		if (sense == FEATURE_SENSE_CHAN)
			res = AST_PBX_NO_HANGUP_PEER;
		else
			res = AST_PBX_KEEPALIVE;
	}
	return res;

}

static int builtin_automonitor(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	char *caller_chan_id = NULL, *callee_chan_id = NULL, *args = NULL, *touch_filename = NULL;
	int x = 0;
	size_t len;
	struct ast_channel *caller_chan, *callee_chan;

	if (!monitor_ok) {
		ast_log(LOG_ERROR,"Cannot record the call. The monitor application is disabled.\n");
		return -1;
	}

	if (!monitor_app && !(monitor_app = pbx_findapp("Monitor"))) {
		monitor_ok = 0;
		ast_log(LOG_ERROR,"Cannot record the call. The monitor application is disabled.\n");
		return -1;
	}

	set_peers(&caller_chan, &callee_chan, peer, chan, sense);

	if (!ast_strlen_zero(courtesytone)) {
		if (ast_autoservice_start(callee_chan))
			return -1;
		if (ast_stream_and_wait(caller_chan, courtesytone, caller_chan->language, "")) {
			ast_log(LOG_WARNING, "Failed to play courtesy tone!\n");
			ast_autoservice_stop(callee_chan);
			return -1;
		}
		if (ast_autoservice_stop(callee_chan))
			return -1;
	}
	
	if (callee_chan->monitor) {
		if (option_verbose > 3)
			ast_verbose(VERBOSE_PREFIX_3 "User hit '%s' to stop recording call.\n", code);
		ast_monitor_stop(callee_chan, 1);
		return FEATURE_RETURN_SUCCESS;
	}

	if (caller_chan && callee_chan) {
		const char *touch_format = pbx_builtin_getvar_helper(caller_chan, "TOUCH_MONITOR_FORMAT");
		const char *touch_monitor = pbx_builtin_getvar_helper(caller_chan, "TOUCH_MONITOR");

		if (!touch_format)
			touch_format = pbx_builtin_getvar_helper(callee_chan, "TOUCH_MONITOR_FORMAT");

		if (!touch_monitor)
			touch_monitor = pbx_builtin_getvar_helper(callee_chan, "TOUCH_MONITOR");
	
		if (touch_monitor) {
			len = strlen(touch_monitor) + 50;
			args = alloca(len);
			touch_filename = alloca(len);
			snprintf(touch_filename, len, "auto-%ld-%s", (long)time(NULL), touch_monitor);
			snprintf(args, len, "%s|%s|m", (touch_format) ? touch_format : "wav", touch_filename);
		} else {
			caller_chan_id = ast_strdupa(S_OR(caller_chan->cid.cid_num, caller_chan->name));
			callee_chan_id = ast_strdupa(S_OR(callee_chan->cid.cid_num, callee_chan->name));
			len = strlen(caller_chan_id) + strlen(callee_chan_id) + 50;
			args = alloca(len);
			touch_filename = alloca(len);
			snprintf(touch_filename, len, "auto-%ld-%s-%s", (long)time(NULL), caller_chan_id, callee_chan_id);
			snprintf(args, len, "%s|%s|m", S_OR(touch_format, "wav"), touch_filename);
		}

		for( x = 0; x < strlen(args); x++) {
			if (args[x] == '/')
				args[x] = '-';
		}
		
		if (option_verbose > 3)
			ast_verbose(VERBOSE_PREFIX_3 "User hit '%s' to record call. filename: %s\n", code, args);

		pbx_exec(callee_chan, monitor_app, args);
		pbx_builtin_setvar_helper(callee_chan, "TOUCH_MONITOR_OUTPUT", touch_filename);
		pbx_builtin_setvar_helper(caller_chan, "TOUCH_MONITOR_OUTPUT", touch_filename);
	
		return FEATURE_RETURN_SUCCESS;
	}
	
	ast_log(LOG_NOTICE,"Cannot record the call. One or both channels have gone away.\n");	
	return -1;
}

static int builtin_disconnect(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	if (option_verbose > 3)
		ast_verbose(VERBOSE_PREFIX_3 "User hit '%s' to disconnect call.\n", code);
	return FEATURE_RETURN_HANGUP;
}

static int finishup(struct ast_channel *chan)
{
        ast_indicate(chan, AST_CONTROL_UNHOLD);
  
        return ast_autoservice_stop(chan);
}

/*! \brief Find the context for the transfer */
static const char *real_ctx(struct ast_channel *transferer, struct ast_channel *transferee)
{
        const char *s = pbx_builtin_getvar_helper(transferer, "TRANSFER_CONTEXT");
        if (ast_strlen_zero(s))
                s = pbx_builtin_getvar_helper(transferee, "TRANSFER_CONTEXT");
        if (ast_strlen_zero(s)) /* Use the non-macro context to transfer the call XXX ? */
                s = transferer->macrocontext;
        if (ast_strlen_zero(s))
                s = transferer->context;
        return s;  
}

static int builtin_blindtransfer(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	struct ast_channel *transferer;
	struct ast_channel *transferee;
	const char *transferer_real_context;
	char xferto[256];
	int res;

	set_peers(&transferer, &transferee, peer, chan, sense);
	transferer_real_context = real_ctx(transferer, transferee);
	/* Start autoservice on chan while we talk to the originator */
	ast_autoservice_start(transferee);
	ast_indicate(transferee, AST_CONTROL_HOLD);

	memset(xferto, 0, sizeof(xferto));
	
	/* Transfer */
	res = ast_stream_and_wait(transferer, "pbx-transfer", transferer->language, AST_DIGIT_ANY);
	if (res < 0) {
		finishup(transferee);
		return -1; /* error ? */
	}
	if (res > 0)	/* If they've typed a digit already, handle it */
		xferto[0] = (char) res;

	ast_stopstream(transferer);
	res = ast_app_dtget(transferer, transferer_real_context, xferto, sizeof(xferto), 100, transferdigittimeout);
	if (res < 0) {  /* hangup, would be 0 for invalid and 1 for valid */
		finishup(transferee);
		return res;
	}
	if (!strcmp(xferto, ast_parking_ext())) {
		res = finishup(transferee);
		if (res)
			res = -1;
		else if (!ast_park_call(transferee, transferer, 0, NULL)) {	/* success */
			/* We return non-zero, but tell the PBX not to hang the channel when
			   the thread dies -- We have to be careful now though.  We are responsible for 
			   hanging up the channel, else it will never be hung up! */

			return (transferer == peer) ? AST_PBX_KEEPALIVE : AST_PBX_NO_HANGUP_PEER;
		} else {
			ast_log(LOG_WARNING, "Unable to park call %s\n", transferee->name);
		}
		/*! \todo XXX Maybe we should have another message here instead of invalid extension XXX */
	} else if (ast_exists_extension(transferee, transferer_real_context, xferto, 1, transferer->cid.cid_num)) {
		pbx_builtin_setvar_helper(peer, "BLINDTRANSFER", transferee->name);
		pbx_builtin_setvar_helper(chan, "BLINDTRANSFER", peer->name);
		res=finishup(transferee);
		if (!transferer->cdr) {
			transferer->cdr=ast_cdr_alloc();
			if (transferer) {
				ast_cdr_init(transferer->cdr, transferer); /* initilize our channel's cdr */
				ast_cdr_start(transferer->cdr);
			}
		}
		if (transferer->cdr) {
			ast_cdr_setdestchan(transferer->cdr, transferee->name);
			ast_cdr_setapp(transferer->cdr, "BLINDTRANSFER","");
		}
		if (!transferee->pbx)
			res = -1;
		
		if (option_verbose > 2) 
			ast_verbose(VERBOSE_PREFIX_3 "Transferring %s to '%s' (context %s) priority 1\n"
				    ,transferee->name, xferto, transferer_real_context);
		if (ast_async_goto(transferee, transferer_real_context, xferto, 1))
			ast_log(LOG_WARNING, "Async goto failed :-(\n");
		check_goto_on_transfer(transferer);
		return res;
	} else {
		if (option_verbose > 2)	
			ast_verbose(VERBOSE_PREFIX_3 "Unable to find extension '%s' in context '%s'\n", xferto, transferer_real_context);
	}
	if (ast_stream_and_wait(transferer, xferfailsound, transferer->language, AST_DIGIT_ANY) < 0 ) {
		finishup(transferee);
		return -1;
	}
	ast_stopstream(transferer);
	res = finishup(transferee);
	if (res) {
		if (option_verbose > 1)
			ast_verbose(VERBOSE_PREFIX_2 "Hungup during autoservice stop on '%s'\n", transferee->name);
		return res;
	}
	return FEATURE_RETURN_SUCCESS;
}

static int check_compat(struct ast_channel *c, struct ast_channel *newchan)
{
	if (ast_channel_make_compatible(c, newchan) < 0) {
		ast_log(LOG_WARNING, "Had to drop call because I couldn't make %s compatible with %s\n",
			c->name, newchan->name);
		ast_hangup(newchan);
		return -1;
	}
	return 0;
}

static int builtin_atxfer(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	struct ast_channel *transferer;
	struct ast_channel *transferee;
	const char *transferer_real_context;
	char xferto[256] = "";
	int res;
	int outstate=0;
	struct ast_channel *newchan;
	struct ast_channel *xferchan;
	struct ast_bridge_thread_obj *tobj;
	struct ast_bridge_config bconfig;
	struct ast_frame *f;
	int l;

	if (option_debug)
		ast_log(LOG_DEBUG, "Executing Attended Transfer %s, %s (sense=%d) \n", chan->name, peer->name, sense);
	set_peers(&transferer, &transferee, peer, chan, sense);
        transferer_real_context = real_ctx(transferer, transferee);
	/* Start autoservice on chan while we talk to the originator */
	ast_autoservice_start(transferee);
	ast_indicate(transferee, AST_CONTROL_HOLD);
	
	/* Transfer */
	res = ast_stream_and_wait(transferer, "pbx-transfer", transferer->language, AST_DIGIT_ANY);
	if (res < 0) {
		finishup(transferee);
		return res;
	}
	if (res > 0) /* If they've typed a digit already, handle it */
		xferto[0] = (char) res;

	/* this is specific of atxfer */
	res = ast_app_dtget(transferer, transferer_real_context, xferto, sizeof(xferto), 100, transferdigittimeout);
        if (res < 0) {  /* hangup, would be 0 for invalid and 1 for valid */
                finishup(transferee);
                return res;
        }
	if (res == 0) {
		ast_log(LOG_WARNING, "Did not read data.\n");
		finishup(transferee);
		if (ast_stream_and_wait(transferer, "beeperr", transferer->language, ""))
			return -1;
		return FEATURE_RETURN_SUCCESS;
	}

	/* valid extension, res == 1 */
	if (!ast_exists_extension(transferer, transferer_real_context, xferto, 1, transferer->cid.cid_num)) {
		ast_log(LOG_WARNING, "Extension %s does not exist in context %s\n",xferto,transferer_real_context);
		finishup(transferee);
		if (ast_stream_and_wait(transferer, "beeperr", transferer->language, ""))
			return -1;
		return FEATURE_RETURN_SUCCESS;
	}

	l = strlen(xferto);
	snprintf(xferto + l, sizeof(xferto) - l, "@%s/n", transferer_real_context);	/* append context */
	newchan = ast_feature_request_and_dial(transferer, "Local", ast_best_codec(transferer->nativeformats),
		xferto, atxfernoanswertimeout, &outstate, transferer->cid.cid_num, transferer->cid.cid_name);
	ast_indicate(transferer, -1);
	if (!newchan) {
		finishup(transferee);
		/* any reason besides user requested cancel and busy triggers the failed sound */
		if (outstate != AST_CONTROL_UNHOLD && outstate != AST_CONTROL_BUSY &&
				ast_stream_and_wait(transferer, xferfailsound, transferer->language, ""))
			return -1;
		return FEATURE_RETURN_SUCCESS;
	}

	if (check_compat(transferer, newchan))
		return -1;
	memset(&bconfig,0,sizeof(struct ast_bridge_config));
	ast_set_flag(&(bconfig.features_caller), AST_FEATURE_DISCONNECT);
	ast_set_flag(&(bconfig.features_callee), AST_FEATURE_DISCONNECT);
	res = ast_bridge_call(transferer, newchan, &bconfig);
	if (newchan->_softhangup || !transferer->_softhangup) {
		ast_hangup(newchan);
		if (ast_stream_and_wait(transferer, xfersound, transferer->language, ""))
			ast_log(LOG_WARNING, "Failed to play transfer sound!\n");
		finishup(transferee);
		transferer->_softhangup = 0;
		return FEATURE_RETURN_SUCCESS;
	}
	
	if (check_compat(transferee, newchan))
		return -1;

	ast_indicate(transferee, AST_CONTROL_UNHOLD);
	
	if ((ast_autoservice_stop(transferee) < 0)
	   || (ast_waitfordigit(transferee, 100) < 0)
	   || (ast_waitfordigit(newchan, 100) < 0) 
	   || ast_check_hangup(transferee) 
	   || ast_check_hangup(newchan)) {
		ast_hangup(newchan);
		return -1;
	}

	xferchan = ast_channel_alloc(0, AST_STATE_DOWN, 0, 0, "", "", "", 0, "Transfered/%s", transferee->name);
	if (!xferchan) {
		ast_hangup(newchan);
		return -1;
	}
	/* Make formats okay */
	xferchan->readformat = transferee->readformat;
	xferchan->writeformat = transferee->writeformat;
	ast_channel_masquerade(xferchan, transferee);
	ast_explicit_goto(xferchan, transferee->context, transferee->exten, transferee->priority);
	xferchan->_state = AST_STATE_UP;
	ast_clear_flag(xferchan, AST_FLAGS_ALL);	
	xferchan->_softhangup = 0;

	if ((f = ast_read(xferchan)))
		ast_frfree(f);

	newchan->_state = AST_STATE_UP;
	ast_clear_flag(newchan, AST_FLAGS_ALL);	
	newchan->_softhangup = 0;

	tobj = ast_calloc(1, sizeof(struct ast_bridge_thread_obj));
	if (!tobj) {
		ast_hangup(xferchan);
		ast_hangup(newchan);
		return -1;
	}
	tobj->chan = xferchan;
	tobj->peer = newchan;
	tobj->bconfig = *config;

	if (ast_stream_and_wait(newchan, xfersound, newchan->language, ""))
		ast_log(LOG_WARNING, "Failed to play transfer sound!\n");
	ast_bridge_call_thread_launch(tobj);
	return -1;	/* XXX meaning the channel is bridged ? */
}


/* add atxfer and automon as undefined so you can only use em if you configure them */
#define FEATURES_COUNT (sizeof(builtin_features) / sizeof(builtin_features[0]))

AST_RWLOCK_DEFINE_STATIC(features_lock);

static struct ast_call_feature builtin_features[] = 
 {
	{ AST_FEATURE_REDIRECT, "Blind Transfer", "blindxfer", "#", "#", builtin_blindtransfer, AST_FEATURE_FLAG_NEEDSDTMF, "" },
	{ AST_FEATURE_REDIRECT, "Attended Transfer", "atxfer", "", "", builtin_atxfer, AST_FEATURE_FLAG_NEEDSDTMF, "" },
	{ AST_FEATURE_AUTOMON, "One Touch Monitor", "automon", "", "", builtin_automonitor, AST_FEATURE_FLAG_NEEDSDTMF, "" },
	{ AST_FEATURE_DISCONNECT, "Disconnect Call", "disconnect", "*", "*", builtin_disconnect, AST_FEATURE_FLAG_NEEDSDTMF, "" },
	{ AST_FEATURE_PARKCALL, "Park Call", "parkcall", "", "", builtin_parkcall, AST_FEATURE_FLAG_NEEDSDTMF, "" },
};


static AST_LIST_HEAD_STATIC(feature_list,ast_call_feature);

/*! \brief register new feature into feature_list*/
void ast_register_feature(struct ast_call_feature *feature)
{
	if (!feature) {
		ast_log(LOG_NOTICE,"You didn't pass a feature!\n");
    		return;
	}
  
	AST_LIST_LOCK(&feature_list);
	AST_LIST_INSERT_HEAD(&feature_list,feature,feature_entry);
	AST_LIST_UNLOCK(&feature_list);

	if (option_verbose >= 2) 
		ast_verbose(VERBOSE_PREFIX_2 "Registered Feature '%s'\n",feature->sname);
}

/*! \brief unregister feature from feature_list */
void ast_unregister_feature(struct ast_call_feature *feature)
{
	if (!feature)
		return;

	AST_LIST_LOCK(&feature_list);
	AST_LIST_REMOVE(&feature_list,feature,feature_entry);
	AST_LIST_UNLOCK(&feature_list);
	free(feature);
}

/*! \brief Remove all features in the list */
static void ast_unregister_features(void)
{
	struct ast_call_feature *feature;

	AST_LIST_LOCK(&feature_list);
	while ((feature = AST_LIST_REMOVE_HEAD(&feature_list,feature_entry)))
		free(feature);
	AST_LIST_UNLOCK(&feature_list);
}

/*! \brief find a feature by name */
static struct ast_call_feature *find_dynamic_feature(const char *name)
{
	struct ast_call_feature *tmp;

	AST_LIST_TRAVERSE(&feature_list, tmp, feature_entry) {
		if (!strcasecmp(tmp->sname, name))
			break;
	}

	return tmp;
}

/*! \brief exec an app by feature */
static int feature_exec_app(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	struct ast_app *app;
	struct ast_call_feature *feature;
	struct ast_channel *work, *idle;
	int res;

	AST_LIST_LOCK(&feature_list);
	AST_LIST_TRAVERSE(&feature_list, feature, feature_entry) {
		if (!strcasecmp(feature->exten, code))
			break;
	}
	AST_LIST_UNLOCK(&feature_list);

	if (!feature) { /* shouldn't ever happen! */
		ast_log(LOG_NOTICE, "Found feature before, but at execing we've lost it??\n");
		return -1; 
	}

	if (sense == FEATURE_SENSE_CHAN) {
		if (!ast_test_flag(feature, AST_FEATURE_FLAG_BYCALLER))
			return FEATURE_RETURN_PASSDIGITS;
		if (ast_test_flag(feature, AST_FEATURE_FLAG_ONSELF)) {
			work = chan;
			idle = peer;
		} else {
			work = peer;
			idle = chan;
		}
	} else {
		if (!ast_test_flag(feature, AST_FEATURE_FLAG_BYCALLEE))
			return FEATURE_RETURN_PASSDIGITS;
		if (ast_test_flag(feature, AST_FEATURE_FLAG_ONSELF)) {
			work = peer;
			idle = chan;
		} else {
			work = chan;
			idle = peer;
		}
	}

	if (!(app = pbx_findapp(feature->app))) {
		ast_log(LOG_WARNING, "Could not find application (%s)\n", feature->app);
		return -2;
	}

	ast_autoservice_start(idle);
	
	if (!ast_strlen_zero(feature->moh_class))
		ast_moh_start(idle, feature->moh_class, NULL);

	res = pbx_exec(work, app, feature->app_args);

	if (!ast_strlen_zero(feature->moh_class))
		ast_moh_stop(idle);

	ast_autoservice_stop(idle);

	if (res == AST_PBX_KEEPALIVE)
		return FEATURE_RETURN_PBX_KEEPALIVE;
	else if (res == AST_PBX_NO_HANGUP_PEER)
		return FEATURE_RETURN_NO_HANGUP_PEER;
	else if (res)
		return FEATURE_RETURN_SUCCESSBREAK;
	
	return FEATURE_RETURN_SUCCESS;	/*! \todo XXX should probably return res */
}

static void unmap_features(void)
{
	int x;

	ast_rwlock_wrlock(&features_lock);
	for (x = 0; x < FEATURES_COUNT; x++)
		strcpy(builtin_features[x].exten, builtin_features[x].default_exten);
	ast_rwlock_unlock(&features_lock);
}

static int remap_feature(const char *name, const char *value)
{
	int x, res = -1;

	ast_rwlock_wrlock(&features_lock);
	for (x = 0; x < FEATURES_COUNT; x++) {
		if (strcasecmp(builtin_features[x].sname, name))
			continue;

		ast_copy_string(builtin_features[x].exten, value, sizeof(builtin_features[x].exten));
		res = 0;
		break;
	}
	ast_rwlock_unlock(&features_lock);

	return res;
}

static int ast_feature_interpret(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config, char *code, int sense)
{
	int x;
	struct ast_flags features;
	int res = FEATURE_RETURN_PASSDIGITS;
	struct ast_call_feature *feature;
	const char *dynamic_features=pbx_builtin_getvar_helper(chan,"DYNAMIC_FEATURES");
	char *tmp, *tok;

	if (sense == FEATURE_SENSE_CHAN)
		ast_copy_flags(&features, &(config->features_caller), AST_FLAGS_ALL);	
	else
		ast_copy_flags(&features, &(config->features_callee), AST_FLAGS_ALL);	
	if (option_debug > 2)
		ast_log(LOG_DEBUG, "Feature interpret: chan=%s, peer=%s, sense=%d, features=%d\n", chan->name, peer->name, sense, features.flags);

	ast_rwlock_rdlock(&features_lock);
	for (x = 0; x < FEATURES_COUNT; x++) {
		if ((ast_test_flag(&features, builtin_features[x].feature_mask)) &&
		    !ast_strlen_zero(builtin_features[x].exten)) {
			/* Feature is up for consideration */
			if (!strcmp(builtin_features[x].exten, code)) {
				res = builtin_features[x].operation(chan, peer, config, code, sense);
				break;
			} else if (!strncmp(builtin_features[x].exten, code, strlen(code))) {
				if (res == FEATURE_RETURN_PASSDIGITS)
					res = FEATURE_RETURN_STOREDIGITS;
			}
		}
	}
	ast_rwlock_unlock(&features_lock);

	if (ast_strlen_zero(dynamic_features))
		return res;

	tmp = ast_strdupa(dynamic_features);

	while ((tok = strsep(&tmp, "#"))) {
		AST_LIST_LOCK(&feature_list);	
		if (!(feature = find_dynamic_feature(tok))) {
			AST_LIST_UNLOCK(&feature_list);
			continue;
		}
			
		/* Feature is up for consideration */
		if (!strcmp(feature->exten, code)) {
			if (option_verbose > 2)
				ast_verbose(VERBOSE_PREFIX_3 " Feature Found: %s exten: %s\n",feature->sname, tok);
			res = feature->operation(chan, peer, config, code, sense);
			AST_LIST_UNLOCK(&feature_list);
			break;
		} else if (!strncmp(feature->exten, code, strlen(code)))
			res = FEATURE_RETURN_STOREDIGITS;

		AST_LIST_UNLOCK(&feature_list);
	}
	
	return res;
}

static void set_config_flags(struct ast_channel *chan, struct ast_channel *peer, struct ast_bridge_config *config)
{
	int x;
	
	ast_clear_flag(config, AST_FLAGS_ALL);

	ast_rwlock_rdlock(&features_lock);
	for (x = 0; x < FEATURES_COUNT; x++) {
		if (!ast_test_flag(builtin_features + x, AST_FEATURE_FLAG_NEEDSDTMF))
			continue;

		if (ast_test_flag(&(config->features_caller), builtin_features[x].feature_mask))
			ast_set_flag(config, AST_BRIDGE_DTMF_CHANNEL_0);

		if (ast_test_flag(&(config->features_callee), builtin_features[x].feature_mask))
			ast_set_flag(config, AST_BRIDGE_DTMF_CHANNEL_1);
	}
	ast_rwlock_unlock(&features_lock);
	
	if (chan && peer && !(ast_test_flag(config, AST_BRIDGE_DTMF_CHANNEL_0) && ast_test_flag(config, AST_BRIDGE_DTMF_CHANNEL_1))) {
		const char *dynamic_features = pbx_builtin_getvar_helper(chan, "DYNAMIC_FEATURES");

		if (dynamic_features) {
			char *tmp = ast_strdupa(dynamic_features);
			char *tok;
			struct ast_call_feature *feature;

			/* while we have a feature */
			while ((tok = strsep(&tmp, "#"))) {
				AST_LIST_LOCK(&feature_list);
				if ((feature = find_dynamic_feature(tok)) && ast_test_flag(feature, AST_FEATURE_FLAG_NEEDSDTMF)) {
					if (ast_test_flag(feature, AST_FEATURE_FLAG_BYCALLER))
						ast_set_flag(config, AST_BRIDGE_DTMF_CHANNEL_0);
					if (ast_test_flag(feature, AST_FEATURE_FLAG_BYCALLEE))
						ast_set_flag(config, AST_BRIDGE_DTMF_CHANNEL_1);
				}
				AST_LIST_UNLOCK(&feature_list);
			}
		}
	}
}

/*! \todo XXX Check - this is very similar to the code in channel.c */
static struct ast_channel *ast_feature_request_and_dial(struct ast_channel *caller, const char *type, int format, void *data, int timeout, int *outstate, const char *cid_num, const char *cid_name)
{
	int state = 0;
	int cause = 0;
	int to;
	struct ast_channel *chan;
	struct ast_channel *monitor_chans[2];
	struct ast_channel *active_channel;
	int res = 0, ready = 0;
	
	if ((chan = ast_request(type, format, data, &cause))) {
		ast_set_callerid(chan, cid_num, cid_name, cid_num);
		ast_channel_inherit_variables(caller, chan);	
		pbx_builtin_setvar_helper(chan, "TRANSFERERNAME", caller->name);
		if (!chan->cdr) {
			chan->cdr=ast_cdr_alloc();
			if (chan->cdr) {
				ast_cdr_init(chan->cdr, chan); /* initilize our channel's cdr */
				ast_cdr_start(chan->cdr);
			}
		}
			
		if (!ast_call(chan, data, timeout)) {
			struct timeval started;
			int x, len = 0;
			char *disconnect_code = NULL, *dialed_code = NULL;

			ast_indicate(caller, AST_CONTROL_RINGING);
			/* support dialing of the featuremap disconnect code while performing an attended tranfer */
			ast_rwlock_rdlock(&features_lock);
			for (x = 0; x < FEATURES_COUNT; x++) {
				if (strcasecmp(builtin_features[x].sname, "disconnect"))
					continue;

				disconnect_code = builtin_features[x].exten;
				len = strlen(disconnect_code) + 1;
				dialed_code = alloca(len);
				memset(dialed_code, 0, len);
				break;
			}
			ast_rwlock_unlock(&features_lock);
			x = 0;
			started = ast_tvnow();
			to = timeout;
			while (!ast_check_hangup(caller) && timeout && (chan->_state != AST_STATE_UP)) {
				struct ast_frame *f = NULL;

				monitor_chans[0] = caller;
				monitor_chans[1] = chan;
				active_channel = ast_waitfor_n(monitor_chans, 2, &to);

				/* see if the timeout has been violated */
				if(ast_tvdiff_ms(ast_tvnow(), started) > timeout) {
					state = AST_CONTROL_UNHOLD;
					ast_log(LOG_NOTICE, "We exceeded our AT-timeout\n");
					break; /*doh! timeout*/
				}

				if (!active_channel)
					continue;

				if (chan && (chan == active_channel)){
					f = ast_read(chan);
					if (f == NULL) { /*doh! where'd he go?*/
						state = AST_CONTROL_HANGUP;
						res = 0;
						break;
					}
					
					if (f->frametype == AST_FRAME_CONTROL || f->frametype == AST_FRAME_DTMF || f->frametype == AST_FRAME_TEXT) {
						if (f->subclass == AST_CONTROL_RINGING) {
							state = f->subclass;
							if (option_verbose > 2)
								ast_verbose( VERBOSE_PREFIX_3 "%s is ringing\n", chan->name);
							ast_indicate(caller, AST_CONTROL_RINGING);
						} else if ((f->subclass == AST_CONTROL_BUSY) || (f->subclass == AST_CONTROL_CONGESTION)) {
							state = f->subclass;
							if (option_verbose > 2)
								ast_verbose( VERBOSE_PREFIX_3 "%s is busy\n", chan->name);
							ast_indicate(caller, AST_CONTROL_BUSY);
							ast_frfree(f);
							f = NULL;
							break;
						} else if (f->subclass == AST_CONTROL_ANSWER) {
							/* This is what we are hoping for */
							state = f->subclass;
							ast_frfree(f);
							f = NULL;
							ready=1;
							break;
						} else {
							ast_log(LOG_NOTICE, "Don't know what to do about control frame: %d\n", f->subclass);
						}
						/* else who cares */
					}

				} else if (caller && (active_channel == caller)) {
					f = ast_read(caller);
					if (f == NULL) { /*doh! where'd he go?*/
						if (caller->_softhangup && !chan->_softhangup) {
							/* make this a blind transfer */
							ready = 1;
							break;
						}
						state = AST_CONTROL_HANGUP;
						res = 0;
						break;
					}
					
					if (f->frametype == AST_FRAME_DTMF) {
						dialed_code[x++] = f->subclass;
						dialed_code[x] = '\0';
						if (strlen(dialed_code) == len) {
							x = 0;
						} else if (x && strncmp(dialed_code, disconnect_code, x)) {
							x = 0;
							dialed_code[x] = '\0';
						}
						if (*dialed_code && !strcmp(dialed_code, disconnect_code)) {
							/* Caller Canceled the call */
							state = AST_CONTROL_UNHOLD;
							ast_frfree(f);
							f = NULL;
							break;
						}
					}
				}
				if (f)
					ast_frfree(f);
			} /* end while */
		} else
			ast_log(LOG_NOTICE, "Unable to call channel %s/%s\n", type, (char *)data);
	} else {
		ast_log(LOG_NOTICE, "Unable to request channel %s/%s\n", type, (char *)data);
		switch(cause) {
		case AST_CAUSE_BUSY:
			state = AST_CONTROL_BUSY;
			break;
		case AST_CAUSE_CONGESTION:
			state = AST_CONTROL_CONGESTION;
			break;
		}
	}
	
	ast_indicate(caller, -1);
	if (chan && ready) {
		if (chan->_state == AST_STATE_UP) 
			state = AST_CONTROL_ANSWER;
		res = 0;
	} else if(chan) {
		res = -1;
		ast_hangup(chan);
		chan = NULL;
	} else {
		res = -1;
	}
	
	if (outstate)
		*outstate = state;

	if (chan && res <= 0) {
		if (chan->cdr || (chan->cdr = ast_cdr_alloc())) {
			char tmp[256];
			ast_cdr_init(chan->cdr, chan);
			snprintf(tmp, 256, "%s/%s", type, (char *)data);
			ast_cdr_setapp(chan->cdr,"Dial",tmp);
			ast_cdr_update(chan);
			ast_cdr_start(chan->cdr);
			ast_cdr_end(chan->cdr);
			/* If the cause wasn't handled properly */
			if (ast_cdr_disposition(chan->cdr,chan->hangupcause))
				ast_cdr_failed(chan->cdr);
		} else {
			ast_log(LOG_WARNING, "Unable to create Call Detail Record\n");
		}
	}
	
	return chan;
}

int ast_bridge_call(struct ast_channel *chan,struct ast_channel *peer,struct ast_bridge_config *config)
{
	/* Copy voice back and forth between the two channels.  Give the peer
	   the ability to transfer calls with '#<extension' syntax. */
	struct ast_frame *f;
	struct ast_channel *who;
	char chan_featurecode[FEATURE_MAX_LEN + 1]="";
	char peer_featurecode[FEATURE_MAX_LEN + 1]="";
	int res;
	int diff;
	int hasfeatures=0;
	int hadfeatures=0;
	struct ast_option_header *aoh;
	struct ast_bridge_config backup_config;
	struct ast_cdr *bridge_cdr;

	memset(&backup_config, 0, sizeof(backup_config));

	config->start_time = ast_tvnow();

	if (chan && peer) {
		pbx_builtin_setvar_helper(chan, "BRIDGEPEER", peer->name);
		pbx_builtin_setvar_helper(peer, "BRIDGEPEER", chan->name);
	} else if (chan)
		pbx_builtin_setvar_helper(chan, "BLINDTRANSFER", NULL);

	if (monitor_ok) {
		const char *monitor_exec;
		struct ast_channel *src = NULL;
		if (!monitor_app) { 
			if (!(monitor_app = pbx_findapp("Monitor")))
				monitor_ok=0;
		}
		if ((monitor_exec = pbx_builtin_getvar_helper(chan, "AUTO_MONITOR"))) 
			src = chan;
		else if ((monitor_exec = pbx_builtin_getvar_helper(peer, "AUTO_MONITOR")))
			src = peer;
		if (monitor_app && src) {
			char *tmp = ast_strdupa(monitor_exec);
			pbx_exec(src, monitor_app, tmp);
		}
	}
	
	set_config_flags(chan, peer, config);
	config->firstpass = 1;

	/* Answer if need be */
	if (ast_answer(chan))
		return -1;
	peer->appl = "Bridged Call";
	peer->data = chan->name;

	/* copy the userfield from the B-leg to A-leg if applicable */
	if (chan->cdr && peer->cdr && !ast_strlen_zero(peer->cdr->userfield)) {
		char tmp[256];
		if (!ast_strlen_zero(chan->cdr->userfield)) {
			snprintf(tmp, sizeof(tmp), "%s;%s", chan->cdr->userfield, peer->cdr->userfield);
			ast_cdr_appenduserfield(chan, tmp);
		} else
			ast_cdr_setuserfield(chan, peer->cdr->userfield);
		/* free the peer's cdr without ast_cdr_free complaining */
		free(peer->cdr);
		peer->cdr = NULL;
	}

	for (;;) {
		struct ast_channel *other;	/* used later */

		res = ast_channel_bridge(chan, peer, config, &f, &who);

		if (config->feature_timer) {
			/* Update time limit for next pass */
			diff = ast_tvdiff_ms(ast_tvnow(), config->start_time);
			config->feature_timer -= diff;
			if (hasfeatures) {
				/* Running on backup config, meaning a feature might be being
				   activated, but that's no excuse to keep things going 
				   indefinitely! */
				if (backup_config.feature_timer && ((backup_config.feature_timer -= diff) <= 0)) {
					if (option_debug)
						ast_log(LOG_DEBUG, "Timed out, realtime this time!\n");
					config->feature_timer = 0;
					who = chan;
					if (f)
						ast_frfree(f);
					f = NULL;
					res = 0;
				} else if (config->feature_timer <= 0) {
					/* Not *really* out of time, just out of time for
					   digits to come in for features. */
					if (option_debug)
						ast_log(LOG_DEBUG, "Timed out for feature!\n");
					if (!ast_strlen_zero(peer_featurecode)) {
						ast_dtmf_stream(chan, peer, peer_featurecode, 0);
						memset(peer_featurecode, 0, sizeof(peer_featurecode));
					}
					if (!ast_strlen_zero(chan_featurecode)) {
						ast_dtmf_stream(peer, chan, chan_featurecode, 0);
						memset(chan_featurecode, 0, sizeof(chan_featurecode));
					}
					if (f)
						ast_frfree(f);
					hasfeatures = !ast_strlen_zero(chan_featurecode) || !ast_strlen_zero(peer_featurecode);
					if (!hasfeatures) {
						/* Restore original (possibly time modified) bridge config */
						memcpy(config, &backup_config, sizeof(struct ast_bridge_config));
						memset(&backup_config, 0, sizeof(backup_config));
					}
					hadfeatures = hasfeatures;
					/* Continue as we were */
					continue;
				} else if (!f) {
					/* The bridge returned without a frame and there is a feature in progress.
					 * However, we don't think the feature has quite yet timed out, so just
					 * go back into the bridge. */
					continue;
 				}
			} else {
				if (config->feature_timer <=0) {
					/* We ran out of time */
					config->feature_timer = 0;
					who = chan;
					if (f)
						ast_frfree(f);
					f = NULL;
					res = 0;
				}
			}
		}
		if (res < 0) {
			ast_log(LOG_WARNING, "Bridge failed on channels %s and %s\n", chan->name, peer->name);
			return -1;
		}
		
		if (!f || (f->frametype == AST_FRAME_CONTROL &&
				(f->subclass == AST_CONTROL_HANGUP || f->subclass == AST_CONTROL_BUSY || 
					f->subclass == AST_CONTROL_CONGESTION ) ) ) {
			res = -1;
			break;
		}
		/* many things should be sent to the 'other' channel */
		other = (who == chan) ? peer : chan;
		if (f->frametype == AST_FRAME_CONTROL) {
			switch (f->subclass) {
			case AST_CONTROL_RINGING:
			case AST_CONTROL_FLASH:
			case -1:
				ast_indicate(other, f->subclass);
				break;
			case AST_CONTROL_HOLD:
			case AST_CONTROL_UNHOLD:
				ast_indicate_data(other, f->subclass, f->data, f->datalen);
				break;
			case AST_CONTROL_OPTION:
				aoh = f->data;
				/* Forward option Requests */
				if (aoh && aoh->flag == AST_OPTION_FLAG_REQUEST) {
					ast_channel_setoption(other, ntohs(aoh->option), aoh->data, 
						f->datalen - sizeof(struct ast_option_header), 0);
				}
				break;
			}
		} else if (f->frametype == AST_FRAME_DTMF_BEGIN) {
			/* eat it */
		} else if (f->frametype == AST_FRAME_DTMF) {
			char *featurecode;
			int sense;

			hadfeatures = hasfeatures;
			/* This cannot overrun because the longest feature is one shorter than our buffer */
			if (who == chan) {
				sense = FEATURE_SENSE_CHAN;
				featurecode = chan_featurecode;
			} else  {
				sense = FEATURE_SENSE_PEER;
				featurecode = peer_featurecode;
			}
			/*! append the event to featurecode. we rely on the string being zero-filled, and
			 * not overflowing it. 
			 * \todo XXX how do we guarantee the latter ?
			 */
			featurecode[strlen(featurecode)] = f->subclass;
			/* Get rid of the frame before we start doing "stuff" with the channels */
			ast_frfree(f);
			f = NULL;
			config->feature_timer = backup_config.feature_timer;
			res = ast_feature_interpret(chan, peer, config, featurecode, sense);
			switch(res) {
			case FEATURE_RETURN_PASSDIGITS:
				ast_dtmf_stream(other, who, featurecode, 0);
				/* Fall through */
			case FEATURE_RETURN_SUCCESS:
				memset(featurecode, 0, sizeof(chan_featurecode));
				break;
			}
			if (res >= FEATURE_RETURN_PASSDIGITS) {
				res = 0;
			} else 
				break;
			hasfeatures = !ast_strlen_zero(chan_featurecode) || !ast_strlen_zero(peer_featurecode);
			if (hadfeatures && !hasfeatures) {
				/* Restore backup */
				memcpy(config, &backup_config, sizeof(struct ast_bridge_config));
				memset(&backup_config, 0, sizeof(struct ast_bridge_config));
			} else if (hasfeatures) {
				if (!hadfeatures) {
					/* Backup configuration */
					memcpy(&backup_config, config, sizeof(struct ast_bridge_config));
					/* Setup temporary config options */
					config->play_warning = 0;
					ast_clear_flag(&(config->features_caller), AST_FEATURE_PLAY_WARNING);
					ast_clear_flag(&(config->features_callee), AST_FEATURE_PLAY_WARNING);
					config->warning_freq = 0;
					config->warning_sound = NULL;
					config->end_sound = NULL;
					config->start_sound = NULL;
					config->firstpass = 0;
				}
				config->start_time = ast_tvnow();
				config->feature_timer = featuredigittimeout;
				if (option_debug)
					ast_log(LOG_DEBUG, "Set time limit to %ld\n", config->feature_timer);
			}
		}
		if (f)
			ast_frfree(f);

	}

	/* arrange the cdrs */
	bridge_cdr = ast_cdr_alloc();
	if (bridge_cdr) {
		if (chan->cdr && peer->cdr) { /* both of them? merge */
			ast_cdr_init(bridge_cdr,chan); /* seems more logicaller to use the  destination as a base, but, really, it's random */
			ast_cdr_start(bridge_cdr); /* now is the time to start */

			/* absorb the channel cdr */
			ast_cdr_merge(bridge_cdr, chan->cdr);
			if (!ast_test_flag(chan->cdr, AST_CDR_FLAG_LOCKED))
				ast_cdr_discard(chan->cdr); /* if locked cdrs are in chan, they are taken over in the merge */
			
			/* absorb the peer cdr */
			ast_cdr_merge(bridge_cdr, peer->cdr);
			if (ast_test_flag(peer->cdr, AST_CDR_FLAG_LOCKED))
				ast_cdr_discard(peer->cdr); /* if locked cdrs are in peer, they are taken over in the merge */
			
			peer->cdr = NULL;
			chan->cdr = bridge_cdr; /* make this available to the rest of the world via the chan while the call is in progress */
		} else if (chan->cdr) {
			/* take the cdr from the channel - literally */
			ast_cdr_init(bridge_cdr,chan);
			/* absorb this data */
			ast_cdr_merge(bridge_cdr, chan->cdr);
			if (!ast_test_flag(chan->cdr, AST_CDR_FLAG_LOCKED))
				ast_cdr_discard(chan->cdr); /* if locked cdrs are in chan, they are taken over in the merge */
			chan->cdr = bridge_cdr; /* make this available to the rest of the world via the chan while the call is in progress */
		} else if (peer->cdr) {
			/* take the cdr from the peer - literally */
			ast_cdr_init(bridge_cdr,peer);
			/* absorb this data */
			ast_cdr_merge(bridge_cdr, peer->cdr);
			if (!ast_test_flag(peer->cdr, AST_CDR_FLAG_LOCKED))
				ast_cdr_discard(peer->cdr); /* if locked cdrs are in chan, they are taken over in the merge */
			peer->cdr = NULL;
			peer->cdr = bridge_cdr; /* make this available to the rest of the world via the chan while the call is in progress */
		} else {
			/* make up a new cdr */
			ast_cdr_init(bridge_cdr,chan); /* eh, just pick one of them */
			chan->cdr = bridge_cdr; /*  */
		}
		if (ast_strlen_zero(bridge_cdr->dstchannel)) {
			if (strcmp(bridge_cdr->channel, peer->name) != 0)
				ast_cdr_setdestchan(bridge_cdr, peer->name);
			else
				ast_cdr_setdestchan(bridge_cdr, chan->name);
		}
	}
	return res;
}

static void post_manager_event(const char *s, char *parkingexten, struct ast_channel *chan)
{
	manager_event(EVENT_FLAG_CALL, s,
		"Exten: %s\r\n"
		"Channel: %s\r\n"
		"CallerID: %s\r\n"
		"CallerIDName: %s\r\n\r\n",
		parkingexten, 
		chan->name,
		S_OR(chan->cid.cid_num, "<unknown>"),
		S_OR(chan->cid.cid_name, "<unknown>")
		);
}

/*! \brief Take care of parked calls and unpark them if needed */
static void *do_parking_thread(void *ignore)
{
	fd_set rfds, efds;	/* results from previous select, to be preserved across loops. */
	FD_ZERO(&rfds);
	FD_ZERO(&efds);

	for (;;) {
		struct parkeduser *pu, *pl, *pt = NULL;
		int ms = -1;	/* select timeout, uninitialized */
		int max = -1;	/* max fd, none there yet */
		fd_set nrfds, nefds;	/* args for the next select */
		FD_ZERO(&nrfds);
		FD_ZERO(&nefds);

		ast_mutex_lock(&parking_lock);
		pl = NULL;
		pu = parkinglot;
		/* navigate the list with prev-cur pointers to support removals */
		while (pu) {
			struct ast_channel *chan = pu->chan;	/* shorthand */
			int tms;        /* timeout for this item */
			int x;          /* fd index in channel */
			struct ast_context *con;

			if (pu->notquiteyet) { /* Pretend this one isn't here yet */
				pl = pu;
				pu = pu->next;
				continue;
			}
			tms = ast_tvdiff_ms(ast_tvnow(), pu->start);
			if (tms > pu->parkingtime) {
				ast_indicate(chan, AST_CONTROL_UNHOLD);
				/* Get chan, exten from derived kludge */
				if (pu->peername[0]) {
					char *peername = ast_strdupa(pu->peername);
					char *cp = strrchr(peername, '-');
					if (cp) 
						*cp = 0;
					con = ast_context_find(parking_con_dial);
					if (!con) {
						con = ast_context_create(NULL, parking_con_dial, registrar);
						if (!con)
							ast_log(LOG_ERROR, "Parking dial context '%s' does not exist and unable to create\n", parking_con_dial);
					}
					if (con) {
						char returnexten[AST_MAX_EXTENSION];
						snprintf(returnexten, sizeof(returnexten), "%s||t", peername);
						ast_add_extension2(con, 1, peername, 1, NULL, NULL, "Dial", strdup(returnexten), ast_free, registrar);
					}
					set_c_e_p(chan, parking_con_dial, peername, 1);
				} else {
					/* They've been waiting too long, send them back to where they came.  Theoretically they
					   should have their original extensions and such, but we copy to be on the safe side */
					set_c_e_p(chan, pu->context, pu->exten, pu->priority);
				}

				post_manager_event("ParkedCallTimeOut", pu->parkingexten, chan);

				if (option_verbose > 1) 
					ast_verbose(VERBOSE_PREFIX_2 "Timeout for %s parked on %d. Returning to %s,%s,%d\n", chan->name, pu->parkingnum, chan->context, chan->exten, chan->priority);
				/* Start up the PBX, or hang them up */
				if (ast_pbx_start(chan))  {
					ast_log(LOG_WARNING, "Unable to restart the PBX for user on '%s', hanging them up...\n", chan->name);
					ast_hangup(chan);
				}
				/* And take them out of the parking lot */
				if (pl) 
					pl->next = pu->next;
				else
					parkinglot = pu->next;
				pt = pu;
				pu = pu->next;
				con = ast_context_find(parking_con);
				if (con) {
					if (ast_context_remove_extension2(con, pt->parkingexten, 1, NULL))
						ast_log(LOG_WARNING, "Whoa, failed to remove the extension!\n");
					else
						notify_metermaids(pt->parkingexten, parking_con);
				} else
					ast_log(LOG_WARNING, "Whoa, no parking context?\n");
				free(pt);
			} else {	/* still within parking time, process descriptors */
				for (x = 0; x < AST_MAX_FDS; x++) {
					struct ast_frame *f;

					if (chan->fds[x] == -1 || (!FD_ISSET(chan->fds[x], &rfds) && !FD_ISSET(chan->fds[x], &efds)))
						continue;	/* nothing on this descriptor */

					if (FD_ISSET(chan->fds[x], &efds))
						ast_set_flag(chan, AST_FLAG_EXCEPTION);
					else
						ast_clear_flag(chan, AST_FLAG_EXCEPTION);
					chan->fdno = x;

					/* See if they need servicing */
					f = ast_read(chan);
					if (!f || (f->frametype == AST_FRAME_CONTROL && f->subclass ==  AST_CONTROL_HANGUP)) {
						if (f)
							ast_frfree(f);
						post_manager_event("ParkedCallGiveUp", pu->parkingexten, chan);

						/* There's a problem, hang them up*/
						if (option_verbose > 1) 
							ast_verbose(VERBOSE_PREFIX_2 "%s got tired of being parked\n", chan->name);
						ast_hangup(chan);
						/* And take them out of the parking lot */
						if (pl) 
							pl->next = pu->next;
						else
							parkinglot = pu->next;
						pt = pu;
						pu = pu->next;
						con = ast_context_find(parking_con);
						if (con) {
							if (ast_context_remove_extension2(con, pt->parkingexten, 1, NULL))
								ast_log(LOG_WARNING, "Whoa, failed to remove the extension!\n");
						else
							notify_metermaids(pt->parkingexten, parking_con);
						} else
							ast_log(LOG_WARNING, "Whoa, no parking context?\n");
						free(pt);
						break;
					} else {
						/*! \todo XXX Maybe we could do something with packets, like dial "0" for operator or something XXX */
						ast_frfree(f);
						if (pu->moh_trys < 3 && !chan->generatordata) {
							if (option_debug)
								ast_log(LOG_DEBUG, "MOH on parked call stopped by outside source.  Restarting.\n");
							ast_indicate_data(pu->chan, AST_CONTROL_HOLD, 
								S_OR(parkmohclass, NULL),
								!ast_strlen_zero(parkmohclass) ? strlen(parkmohclass) + 1 : 0);
							pu->moh_trys++;
						}
						goto std;	/*! \todo XXX Ick: jumping into an else statement??? XXX */
					}

				} /* end for */
				if (x >= AST_MAX_FDS) {
std:					for (x=0; x<AST_MAX_FDS; x++) {	/* mark fds for next round */
						if (chan->fds[x] > -1) {
							FD_SET(chan->fds[x], &nrfds);
							FD_SET(chan->fds[x], &nefds);
							if (chan->fds[x] > max)
								max = chan->fds[x];
						}
					}
					/* Keep track of our shortest wait */
					if (tms < ms || ms < 0)
						ms = tms;
					pl = pu;
					pu = pu->next;
				}
			}
		} /* end while */
		ast_mutex_unlock(&parking_lock);
		rfds = nrfds;
		efds = nefds;
		{
			struct timeval tv = ast_samp2tv(ms, 1000);
			/* Wait for something to happen */
			ast_select(max + 1, &rfds, NULL, &efds, (ms > -1) ? &tv : NULL);
		}
		pthread_testcancel();
	}
	return NULL;	/* Never reached */
}

/*! \brief Park a call */
static int park_call_exec(struct ast_channel *chan, void *data)
{
	/* Data is unused at the moment but could contain a parking
	   lot context eventually */
	int res = 0;
	struct ast_module_user *u;

	u = ast_module_user_add(chan);

	/* Setup the exten/priority to be s/1 since we don't know
	   where this call should return */
	strcpy(chan->exten, "s");
	chan->priority = 1;
	/* Answer if call is not up */
	if (chan->_state != AST_STATE_UP)
		res = ast_answer(chan);
	/* Sleep to allow VoIP streams to settle down */
	if (!res)
		res = ast_safe_sleep(chan, 1000);
	/* Park the call */
	if (!res)
		res = ast_park_call(chan, chan, 0, NULL);

	ast_module_user_remove(u);

	return !res ? AST_PBX_KEEPALIVE : res;
}

/*! \brief Pickup parked call */
static int park_exec(struct ast_channel *chan, void *data)
{
	int res = 0;
	struct ast_module_user *u;
	struct ast_channel *peer=NULL;
	struct parkeduser *pu, *pl=NULL;
	struct ast_context *con;

	int park;
	struct ast_bridge_config config;

	if (!data) {
		ast_log(LOG_WARNING, "Parkedcall requires an argument (extension number)\n");
		return -1;
	}
	
	u = ast_module_user_add(chan);

	park = atoi((char *)data);
	ast_mutex_lock(&parking_lock);
	pu = parkinglot;
	while(pu) {
		if (pu->parkingnum == park) {
			if (pl)
				pl->next = pu->next;
			else
				parkinglot = pu->next;
			break;
		}
		pl = pu;
		pu = pu->next;
	}
	ast_mutex_unlock(&parking_lock);
	if (pu) {
		peer = pu->chan;
		con = ast_context_find(parking_con);
		if (con) {
			if (ast_context_remove_extension2(con, pu->parkingexten, 1, NULL))
				ast_log(LOG_WARNING, "Whoa, failed to remove the extension!\n");
			else
				notify_metermaids(pu->parkingexten, parking_con);
		} else
			ast_log(LOG_WARNING, "Whoa, no parking context?\n");

		manager_event(EVENT_FLAG_CALL, "UnParkedCall",
			"Exten: %s\r\n"
			"Channel: %s\r\n"
			"From: %s\r\n"
			"CallerID: %s\r\n"
			"CallerIDName: %s\r\n",
			pu->parkingexten, pu->chan->name, chan->name,
			S_OR(pu->chan->cid.cid_num, "<unknown>"),
			S_OR(pu->chan->cid.cid_name, "<unknown>")
			);

		free(pu);
	}
	/* JK02: it helps to answer the channel if not already up */
	if (chan->_state != AST_STATE_UP)
		ast_answer(chan);

	if (peer) {
		/* Play a courtesy to the source(s) configured to prefix the bridge connecting */
		
		if (!ast_strlen_zero(courtesytone)) {
			int error = 0;
			ast_indicate(peer, AST_CONTROL_UNHOLD);
			if (parkedplay == 0) {
				error = ast_stream_and_wait(chan, courtesytone, chan->language, "");
			} else if (parkedplay == 1) {
				error = ast_stream_and_wait(peer, courtesytone, chan->language, "");
			} else if (parkedplay == 2) {
				if (!ast_streamfile(chan, courtesytone, chan->language) &&
						!ast_streamfile(peer, courtesytone, chan->language)) {
					/*! \todo XXX we would like to wait on both! */
					res = ast_waitstream(chan, "");
					if (res >= 0)
						res = ast_waitstream(peer, "");
					if (res < 0)
						error = 1;
				}
                        }
			if (error) {
				ast_log(LOG_WARNING, "Failed to play courtesy tone!\n");
				ast_hangup(peer);
				return -1;
			}
		} else
			ast_indicate(peer, AST_CONTROL_UNHOLD); 

		res = ast_channel_make_compatible(chan, peer);
		if (res < 0) {
			ast_log(LOG_WARNING, "Could not make channels %s and %s compatible for bridge\n", chan->name, peer->name);
			ast_hangup(peer);
			return -1;
		}
		/* This runs sorta backwards, since we give the incoming channel control, as if it
		   were the person called. */
		if (option_verbose > 2) 
			ast_verbose(VERBOSE_PREFIX_3 "Channel %s connected to parked call %d\n", chan->name, park);

		pbx_builtin_setvar_helper(chan, "PARKEDCHANNEL", peer->name);
		ast_cdr_setdestchan(chan->cdr, peer->name);
		memset(&config, 0, sizeof(struct ast_bridge_config));
		ast_set_flag(&(config.features_callee), AST_FEATURE_REDIRECT);
		ast_set_flag(&(config.features_caller), AST_FEATURE_REDIRECT);
		res = ast_bridge_call(chan, peer, &config);

		pbx_builtin_setvar_helper(chan, "PARKEDCHANNEL", peer->name);
		ast_cdr_setdestchan(chan->cdr, peer->name);

		/* Simulate the PBX hanging up */
		if (res != AST_PBX_NO_HANGUP_PEER)
			ast_hangup(peer);
		return res;
	} else {
		/*! \todo XXX Play a message XXX */
		if (ast_stream_and_wait(chan, "pbx-invalidpark", chan->language, ""))
			ast_log(LOG_WARNING, "ast_streamfile of %s failed on %s\n", "pbx-invalidpark", chan->name);
		if (option_verbose > 2) 
			ast_verbose(VERBOSE_PREFIX_3 "Channel %s tried to talk to nonexistent parked call %d\n", chan->name, park);
		res = -1;
	}

	ast_module_user_remove(u);

	return res;
}

static int handle_showfeatures(int fd, int argc, char *argv[])
{
	int i;
	struct ast_call_feature *feature;
	char format[] = "%-25s %-7s %-7s\n";

	ast_cli(fd, format, "Builtin Feature", "Default", "Current");
	ast_cli(fd, format, "---------------", "-------", "-------");

	ast_cli(fd, format, "Pickup", "*8", ast_pickup_ext());		/* default hardcoded above, so we'll hardcode it here */

	ast_rwlock_rdlock(&features_lock);
	for (i = 0; i < FEATURES_COUNT; i++)
		ast_cli(fd, format, builtin_features[i].fname, builtin_features[i].default_exten, builtin_features[i].exten);
	ast_rwlock_unlock(&features_lock);

	ast_cli(fd, "\n");
	ast_cli(fd, format, "Dynamic Feature", "Default", "Current");
	ast_cli(fd, format, "---------------", "-------", "-------");
	if (AST_LIST_EMPTY(&feature_list))
		ast_cli(fd, "(none)\n");
	else {
		AST_LIST_LOCK(&feature_list);
		AST_LIST_TRAVERSE(&feature_list, feature, feature_entry)
			ast_cli(fd, format, feature->sname, "no def", feature->exten);	
		AST_LIST_UNLOCK(&feature_list);
	}
	ast_cli(fd, "\nCall parking\n");
	ast_cli(fd, "------------\n");
	ast_cli(fd,"%-20s:	%s\n", "Parking extension", parking_ext);
	ast_cli(fd,"%-20s:	%s\n", "Parking context", parking_con);
	ast_cli(fd,"%-20s:	%d-%d\n", "Parked call extensions", parking_start, parking_stop);
	ast_cli(fd,"\n");
	
	return RESULT_SUCCESS;
}

static char showfeatures_help[] =
"Usage: feature list\n"
"       Lists currently configured features.\n";

static int handle_parkedcalls(int fd, int argc, char *argv[])
{
	struct parkeduser *cur;
	int numparked = 0;

	ast_cli(fd, "%4s %25s (%-15s %-12s %-4s) %-6s \n", "Num", "Channel"
		, "Context", "Extension", "Pri", "Timeout");

	ast_mutex_lock(&parking_lock);

	for (cur = parkinglot; cur; cur = cur->next) {
		ast_cli(fd, "%-10.10s %25s (%-15s %-12s %-4d) %6lds\n"
			,cur->parkingexten, cur->chan->name, cur->context, cur->exten
			,cur->priority, cur->start.tv_sec + (cur->parkingtime/1000) - time(NULL));

		numparked++;
	}
	ast_mutex_unlock(&parking_lock);
	ast_cli(fd, "%d parked call%s.\n", numparked, (numparked != 1) ? "s" : "");


	return RESULT_SUCCESS;
}

static char showparked_help[] =
"Usage: show parkedcalls\n"
"       Lists currently parked calls.\n";

static struct ast_cli_entry cli_show_features_deprecated = {
	{ "show", "features", NULL },
	handle_showfeatures, NULL,
	NULL };

static struct ast_cli_entry cli_features[] = {
	{ { "feature", "show", NULL },
	handle_showfeatures, "Lists configured features",
	showfeatures_help, NULL, &cli_show_features_deprecated },

	{ { "show", "parkedcalls", NULL },
	handle_parkedcalls, "Lists parked calls",
	showparked_help },
};

/*! \brief Dump lot status */
static int manager_parking_status( struct mansession *s, const struct message *m)
{
	struct parkeduser *cur;
	const char *id = astman_get_header(m, "ActionID");
	char idText[256] = "";

	if (!ast_strlen_zero(id))
		snprintf(idText, sizeof(idText), "ActionID: %s\r\n", id);

	astman_send_ack(s, m, "Parked calls will follow");

	ast_mutex_lock(&parking_lock);

	for (cur = parkinglot; cur; cur = cur->next) {
		astman_append(s, "Event: ParkedCall\r\n"
			"Exten: %d\r\n"
			"Channel: %s\r\n"
			"From: %s\r\n"
			"Timeout: %ld\r\n"
			"CallerID: %s\r\n"
			"CallerIDName: %s\r\n"
			"%s"
			"\r\n",
			cur->parkingnum, cur->chan->name, cur->peername,
			(long) cur->start.tv_sec + (long) (cur->parkingtime / 1000) - (long) time(NULL),
			S_OR(cur->chan->cid.cid_num, ""),	/* XXX in other places it is <unknown> */
			S_OR(cur->chan->cid.cid_name, ""),
			idText);
	}

	astman_append(s,
		"Event: ParkedCallsComplete\r\n"
		"%s"
		"\r\n",idText);

	ast_mutex_unlock(&parking_lock);

	return RESULT_SUCCESS;
}

static char mandescr_park[] =
"Description: Park a channel.\n"
"Variables: (Names marked with * are required)\n"
"	*Channel: Channel name to park\n"
"	*Channel2: Channel to announce park info to (and return to if timeout)\n"
"	Timeout: Number of milliseconds to wait before callback.\n";  

static int manager_park(struct mansession *s, const struct message *m)
{
	const char *channel = astman_get_header(m, "Channel");
	const char *channel2 = astman_get_header(m, "Channel2");
	const char *timeout = astman_get_header(m, "Timeout");
	char buf[BUFSIZ];
	int to = 0;
	int res = 0;
	int parkExt = 0;
	struct ast_channel *ch1, *ch2;

	if (ast_strlen_zero(channel)) {
		astman_send_error(s, m, "Channel not specified");
		return 0;
	}

	if (ast_strlen_zero(channel2)) {
		astman_send_error(s, m, "Channel2 not specified");
		return 0;
	}

	ch1 = ast_get_channel_by_name_locked(channel);
	if (!ch1) {
		snprintf(buf, sizeof(buf), "Channel does not exist: %s", channel);
		astman_send_error(s, m, buf);
		return 0;
	}

	ch2 = ast_get_channel_by_name_locked(channel2);
	if (!ch2) {
		snprintf(buf, sizeof(buf), "Channel does not exist: %s", channel2);
		astman_send_error(s, m, buf);
		ast_channel_unlock(ch1);
		return 0;
	}

	if (!ast_strlen_zero(timeout)) {
		sscanf(timeout, "%d", &to);
	}

	res = ast_masq_park_call(ch1, ch2, to, &parkExt);
	if (!res) {
		ast_softhangup(ch2, AST_SOFTHANGUP_EXPLICIT);
		astman_send_ack(s, m, "Park successful");
	} else {
		astman_send_error(s, m, "Park failure");
	}

	ast_channel_unlock(ch1);
	ast_channel_unlock(ch2);

	return 0;
}


int ast_pickup_call(struct ast_channel *chan)
{
	struct ast_channel *cur = NULL;
	int res = -1;

	while ( (cur = ast_channel_walk_locked(cur)) != NULL) {
		if (!cur->pbx && 
			(cur != chan) &&
			(chan->pickupgroup & cur->callgroup) &&
			((cur->_state == AST_STATE_RINGING) ||
			 (cur->_state == AST_STATE_RING))) {
			 	break;
		}
		ast_channel_unlock(cur);
	}
	if (cur) {
		if (option_debug)
			ast_log(LOG_DEBUG, "Call pickup on chan '%s' by '%s'\n",cur->name, chan->name);
		res = ast_answer(chan);
		if (res)
			ast_log(LOG_WARNING, "Unable to answer '%s'\n", chan->name);
		res = ast_queue_control(chan, AST_CONTROL_ANSWER);
		if (res)
			ast_log(LOG_WARNING, "Unable to queue answer on '%s'\n", chan->name);
		res = ast_channel_masquerade(cur, chan);
		if (res)
			ast_log(LOG_WARNING, "Unable to masquerade '%s' into '%s'\n", chan->name, cur->name);		/* Done */
		ast_channel_unlock(cur);
	} else	{
		if (option_debug)
			ast_log(LOG_DEBUG, "No call pickup possible...\n");
	}
	return res;
}

/*! \brief Add parking hints for all defined parking lots */
static void park_add_hints(char *context, int start, int stop)
{
	int numext;
	char device[AST_MAX_EXTENSION];
	char exten[10];

	for (numext = start; numext <= stop; numext++) {
		snprintf(exten, sizeof(exten), "%d", numext);
		snprintf(device, sizeof(device), "park:%s@%s", exten, context);
		ast_add_extension(context, 1, exten, PRIORITY_HINT, NULL, NULL, device, NULL, NULL, registrar);
	}
}


static int load_config(void) 
{
	int start = 0, end = 0;
	int res;
	struct ast_context *con = NULL;
	struct ast_config *cfg = NULL;
	struct ast_variable *var = NULL;
	char old_parking_ext[AST_MAX_EXTENSION];
	char old_parking_con[AST_MAX_EXTENSION] = "";

	if (!ast_strlen_zero(parking_con)) {
		strcpy(old_parking_ext, parking_ext);
		strcpy(old_parking_con, parking_con);
	} 

	/* Reset to defaults */
	strcpy(parking_con, "parkedcalls");
	strcpy(parking_con_dial, "park-dial");
	strcpy(parking_ext, "700");
	strcpy(pickup_ext, "*8");
	strcpy(parkmohclass, "default");
	courtesytone[0] = '\0';
	strcpy(xfersound, "beep");
	strcpy(xferfailsound, "pbx-invalid");
	parking_start = 701;
	parking_stop = 750;
	parkfindnext = 0;
	adsipark = 0;
	parkaddhints = 0;

	transferdigittimeout = DEFAULT_TRANSFER_DIGIT_TIMEOUT;
	featuredigittimeout = DEFAULT_FEATURE_DIGIT_TIMEOUT;
	atxfernoanswertimeout = DEFAULT_NOANSWER_TIMEOUT_ATTENDED_TRANSFER;

	cfg = ast_config_load("features.conf");
	if (!cfg) {
		ast_log(LOG_WARNING,"Could not load features.conf\n");
		return AST_MODULE_LOAD_DECLINE;
	}
	for (var = ast_variable_browse(cfg, "general"); var; var = var->next) {
		if (!strcasecmp(var->name, "parkext")) {
			ast_copy_string(parking_ext, var->value, sizeof(parking_ext));
		} else if (!strcasecmp(var->name, "context")) {
			ast_copy_string(parking_con, var->value, sizeof(parking_con));
		} else if (!strcasecmp(var->name, "parkingtime")) {
			if ((sscanf(var->value, "%d", &parkingtime) != 1) || (parkingtime < 1)) {
				ast_log(LOG_WARNING, "%s is not a valid parkingtime\n", var->value);
				parkingtime = DEFAULT_PARK_TIME;
			} else
				parkingtime = parkingtime * 1000;
		} else if (!strcasecmp(var->name, "parkpos")) {
			if (sscanf(var->value, "%d-%d", &start, &end) != 2) {
				ast_log(LOG_WARNING, "Format for parking positions is a-b, where a and b are numbers at line %d of parking.conf\n", var->lineno);
			} else {
				parking_start = start;
				parking_stop = end;
			}
		} else if (!strcasecmp(var->name, "findslot")) {
			parkfindnext = (!strcasecmp(var->value, "next"));
		} else if (!strcasecmp(var->name, "parkinghints")) {
			parkaddhints = ast_true(var->value);
		} else if (!strcasecmp(var->name, "adsipark")) {
			adsipark = ast_true(var->value);
		} else if (!strcasecmp(var->name, "transferdigittimeout")) {
			if ((sscanf(var->value, "%d", &transferdigittimeout) != 1) || (transferdigittimeout < 1)) {
				ast_log(LOG_WARNING, "%s is not a valid transferdigittimeout\n", var->value);
				transferdigittimeout = DEFAULT_TRANSFER_DIGIT_TIMEOUT;
			} else
				transferdigittimeout = transferdigittimeout * 1000;
		} else if (!strcasecmp(var->name, "featuredigittimeout")) {
			if ((sscanf(var->value, "%d", &featuredigittimeout) != 1) || (featuredigittimeout < 1)) {
				ast_log(LOG_WARNING, "%s is not a valid featuredigittimeout\n", var->value);
				featuredigittimeout = DEFAULT_FEATURE_DIGIT_TIMEOUT;
			}
		} else if (!strcasecmp(var->name, "atxfernoanswertimeout")) {
			if ((sscanf(var->value, "%d", &atxfernoanswertimeout) != 1) || (atxfernoanswertimeout < 1)) {
				ast_log(LOG_WARNING, "%s is not a valid atxfernoanswertimeout\n", var->value);
				atxfernoanswertimeout = DEFAULT_NOANSWER_TIMEOUT_ATTENDED_TRANSFER;
			} else
				atxfernoanswertimeout = atxfernoanswertimeout * 1000;
		} else if (!strcasecmp(var->name, "courtesytone")) {
			ast_copy_string(courtesytone, var->value, sizeof(courtesytone));
		}  else if (!strcasecmp(var->name, "parkedplay")) {
			if (!strcasecmp(var->value, "both"))
				parkedplay = 2;
			else if (!strcasecmp(var->value, "parked"))
				parkedplay = 1;
			else
				parkedplay = 0;
		} else if (!strcasecmp(var->name, "xfersound")) {
			ast_copy_string(xfersound, var->value, sizeof(xfersound));
		} else if (!strcasecmp(var->name, "xferfailsound")) {
			ast_copy_string(xferfailsound, var->value, sizeof(xferfailsound));
		} else if (!strcasecmp(var->name, "pickupexten")) {
			ast_copy_string(pickup_ext, var->value, sizeof(pickup_ext));
		} else if (!strcasecmp(var->name, "parkedmusicclass")) {
			ast_copy_string(parkmohclass, var->value, sizeof(parkmohclass));
		}
	}

	unmap_features();
	for (var = ast_variable_browse(cfg, "featuremap"); var; var = var->next) {
		if (remap_feature(var->name, var->value))
			ast_log(LOG_NOTICE, "Unknown feature '%s'\n", var->name);
	}

	/* Map a key combination to an application*/
	ast_unregister_features();
	for (var = ast_variable_browse(cfg, "applicationmap"); var; var = var->next) {
		char *tmp_val = ast_strdupa(var->value);
		char *exten, *activateon, *activatedby, *app, *app_args, *moh_class; 
		struct ast_call_feature *feature;

		/* strsep() sets the argument to NULL if match not found, and it
		 * is safe to use it with a NULL argument, so we don't check
		 * between calls.
		 */
		exten = strsep(&tmp_val,",");
		activatedby = strsep(&tmp_val,",");
		app = strsep(&tmp_val,",");
		app_args = strsep(&tmp_val,",");
		moh_class = strsep(&tmp_val,",");

		activateon = strsep(&activatedby, "/");	

		/*! \todo XXX var_name or app_args ? */
		if (ast_strlen_zero(app) || ast_strlen_zero(exten) || ast_strlen_zero(activateon) || ast_strlen_zero(var->name)) {
			ast_log(LOG_NOTICE, "Please check the feature Mapping Syntax, either extension, name, or app aren't provided %s %s %s %s\n",
				app, exten, activateon, var->name);
			continue;
		}

		AST_LIST_LOCK(&feature_list);
		if ((feature = find_dynamic_feature(var->name))) {
			AST_LIST_UNLOCK(&feature_list);
			ast_log(LOG_WARNING, "Dynamic Feature '%s' specified more than once!\n", var->name);
			continue;
		}
		AST_LIST_UNLOCK(&feature_list);
				
		if (!(feature = ast_calloc(1, sizeof(*feature))))
			continue;					

		ast_copy_string(feature->sname, var->name, FEATURE_SNAME_LEN);
		ast_copy_string(feature->app, app, FEATURE_APP_LEN);
		ast_copy_string(feature->exten, exten, FEATURE_EXTEN_LEN);
		
		if (app_args) 
			ast_copy_string(feature->app_args, app_args, FEATURE_APP_ARGS_LEN);

		if (moh_class)
			ast_copy_string(feature->moh_class, moh_class, FEATURE_MOH_LEN);
			
		ast_copy_string(feature->exten, exten, sizeof(feature->exten));
		feature->operation = feature_exec_app;
		ast_set_flag(feature, AST_FEATURE_FLAG_NEEDSDTMF);

		/* Allow caller and calle to be specified for backwards compatability */
		if (!strcasecmp(activateon, "self") || !strcasecmp(activateon, "caller"))
			ast_set_flag(feature, AST_FEATURE_FLAG_ONSELF);
		else if (!strcasecmp(activateon, "peer") || !strcasecmp(activateon, "callee"))
			ast_set_flag(feature, AST_FEATURE_FLAG_ONPEER);
		else {
			ast_log(LOG_NOTICE, "Invalid 'ActivateOn' specification for feature '%s',"
				" must be 'self', or 'peer'\n", var->name);
			continue;
		}

		if (ast_strlen_zero(activatedby))
			ast_set_flag(feature, AST_FEATURE_FLAG_BYBOTH);
		else if (!strcasecmp(activatedby, "caller"))
			ast_set_flag(feature, AST_FEATURE_FLAG_BYCALLER);
		else if (!strcasecmp(activatedby, "callee"))
			ast_set_flag(feature, AST_FEATURE_FLAG_BYCALLEE);
		else if (!strcasecmp(activatedby, "both"))
			ast_set_flag(feature, AST_FEATURE_FLAG_BYBOTH);
		else {
			ast_log(LOG_NOTICE, "Invalid 'ActivatedBy' specification for feature '%s',"
				" must be 'caller', or 'callee', or 'both'\n", var->name);
			continue;
		}

		ast_register_feature(feature);
			
		if (option_verbose >= 1)
			ast_verbose(VERBOSE_PREFIX_2 "Mapping Feature '%s' to app '%s(%s)' with code '%s'\n", var->name, app, app_args, exten);  
	}	 
	ast_config_destroy(cfg);

	/* Remove the old parking extension */
	if (!ast_strlen_zero(old_parking_con) && (con = ast_context_find(old_parking_con)))	{
		if(ast_context_remove_extension2(con, old_parking_ext, 1, registrar))
				notify_metermaids(old_parking_ext, old_parking_con);
		if (option_debug)
			ast_log(LOG_DEBUG, "Removed old parking extension %s@%s\n", old_parking_ext, old_parking_con);
	}
	
	if (!(con = ast_context_find(parking_con)) && !(con = ast_context_create(NULL, parking_con, registrar))) {
		ast_log(LOG_ERROR, "Parking context '%s' does not exist and unable to create\n", parking_con);
		return -1;
	}
	res = ast_add_extension2(con, 1, ast_parking_ext(), 1, NULL, NULL, parkcall, NULL, NULL, registrar);
	if (parkaddhints)
		park_add_hints(parking_con, parking_start, parking_stop);
	if (!res)
		notify_metermaids(ast_parking_ext(), parking_con);
	return res;

}

static int reload(void)
{
	return load_config();
}

static int load_module(void)
{
	int res;
	
	memset(parking_ext, 0, sizeof(parking_ext));
	memset(parking_con, 0, sizeof(parking_con));

	if ((res = load_config()))
		return res;
	ast_cli_register_multiple(cli_features, sizeof(cli_features) / sizeof(struct ast_cli_entry));
	ast_pthread_create(&parking_thread, NULL, do_parking_thread, NULL);
	res = ast_register_application(parkedcall, park_exec, synopsis, descrip);
	if (!res)
		res = ast_register_application(parkcall, park_call_exec, synopsis2, descrip2);
	if (!res) {
		ast_manager_register("ParkedCalls", 0, manager_parking_status, "List parked calls" );
		ast_manager_register2("Park", EVENT_FLAG_CALL, manager_park,
			"Park a channel", mandescr_park); 
	}

	res |= ast_devstate_prov_add("Park", metermaidstate);

	return res;
}


static int unload_module(void)
{
	ast_module_user_hangup_all();

	ast_manager_unregister("ParkedCalls");
	ast_manager_unregister("Park");
	ast_cli_unregister_multiple(cli_features, sizeof(cli_features) / sizeof(struct ast_cli_entry));
	ast_unregister_application(parkcall);
	ast_devstate_prov_del("Park");
	return ast_unregister_application(parkedcall);
}

AST_MODULE_INFO(ASTERISK_GPL_KEY, AST_MODFLAG_GLOBAL_SYMBOLS, "Call Features Resource",
		.load = load_module,
		.unload = unload_module,
		.reload = reload,
	       );
