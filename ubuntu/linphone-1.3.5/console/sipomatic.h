/***************************************************************************
                          linphone  - sipomatic.c
This is a test program for linphone. It acts as a sip server and answers to linphone's
call.
                             -------------------
    begin                : ven mar  30
    copyright            : (C) 2001 by Simon MORLAT
    email                : simon.morlat@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "../coreapi/linphonecore.h"
#include "../coreapi/sdphandler.h"

#undef PACKAGE
#undef VERSION
#include "../mediastreamer/mediastream.h"

#include <ortp/ortp.h>
#include <ortp/telephonyevents.h>


#define ANNOUCE_FILE8000HZ	"hello8000.wav"
#define ANNOUCE_FILE16000HZ	"hello16000.wav"

struct _Sipomatic
{
	GMutex *lock;
	GList *calls;
	gdouble acceptance_time;
	gdouble max_call_time;
	gchar *file_path8000hz;
	gchar *file_path16000hz;
};

typedef struct _Sipomatic Sipomatic;
	
void sipomatic_init(Sipomatic *obj, gchar *url, gboolean ipv6);
void sipomatic_uninit(Sipomatic *obj);
void sipomatic_iterate(Sipomatic *obj);
#define sipomatic_lock(obj) g_mutex_lock((obj)->lock);
#define sipomatic_unlock(obj) g_mutex_unlock((obj)->lock);

void sipomatic_set_annouce_file(Sipomatic *obj, char *file);

struct stream_params{
	gint ncodecs;
	gint line;
	gint localport;
	gint remoteport;
	gint pt;
	gchar *remaddr;
};

struct _Call
{
	Sipomatic *root;
	sdp_context_t *sdpc;
	GTimer *timer;
	int did;
	AudioStream *audio_stream;
#ifdef VIDEO_ENABLED
	VideoStream *video_stream;
#endif
	gint state;
	struct _CallParams *params;
	gint eof;
	RtpProfile *profile;
	struct stream_params audio;
	struct stream_params video;
};

#define CALL_STATE_INIT 0
#define CALL_STATE_RUNNING 1
#define CALL_STATE_FINISHED 2

typedef struct _Call Call;

	
Call * call_new(Sipomatic *obj,int cid,char *sdp);
void call_accept(Call *call);
void call_release(Call *call);
void call_destroy(Call *call);

Call* sipomatic_find_call(Sipomatic *obj,int cid);
