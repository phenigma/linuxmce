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
#ifndef MSSNDREAD_H
#define MSSNDREAD_H


#include "sndcard.h"
#include "msfilter.h"


/*this is the class that implements sndcard reading source filter*/



struct _MSSndRead
{
	MSFilter base;
	SndCard *sndcard;
	gint freq;
	gint devid;  /* the sound device id it depends on*/
	gint gran;
	gint flags;
#define START_REQUESTED 1
#define STOP_REQUESTED  2
};

typedef struct _MSSndRead MSSndRead;

/* PUBLIC */
#define MS_SND_READ(filter) ((MSSndRead*)(filter))
MSFilter * ms_snd_read_new(void);
gint ms_snd_read_set_device(MSSndRead *w,gint devid);
void ms_snd_read_start(MSSndRead *w);
void ms_snd_read_stop(MSSndRead *w);




#endif
