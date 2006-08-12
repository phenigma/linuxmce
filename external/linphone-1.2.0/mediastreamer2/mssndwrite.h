/*
  The mediastreamer library aims at providing modular media processing and I/O
	for linphone, but also for any telephony application.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org
  										
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef MSSNDWRITE_H
#define MSSNDWRITE_H

#include "sndcard.h"
#include "msfilter.h"

/*this is the class that implements snd writing sink filter*/



struct _MSSndWrite
{
	MSFilter base;
	gint devid;  /* the sound device id it depends on*/
	SndCard *sndcard;
	gint bsize;
	gint freq;
	gint channels;
	gdouble lowfreq;
	gdouble highfreq;
	gint dtmf_time;
	gint dtmf_duration;
};

typedef struct _MSSndWrite MSSndWrite;

/* PUBLIC */
#define MS_SND_WRITE(filter) ((MSSndWrite*)(filter))

MSFilter * ms_snd_write_new(void);
gint ms_snd_write_set_device(MSSndWrite *w,gint devid);
void ms_snd_write_start(MSSndWrite *w);
void ms_snd_write_stop(MSSndWrite *w);
void ms_snd_write_set_level(MSSndWrite *w, gint level);
void ms_snd_write_play_dtmf(MSSndWrite *w, char dtmf);




#endif
