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

#include "mssndread.h"
#include "mssync.h"
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#define MS_SND_READ_MAX_GRAN (512*2) /* the maximum granularity*/

/* FOR INTERNAL USE*/
void ms_snd_read_init(MSSndRead *r);
void ms_snd_read_process(MSSndRead *f);
void ms_snd_read_notify_start(MSSndRead *f, MSSync *sync);
void ms_snd_read_notify_stop(MSSndRead *f);


MSFilterInfo snd_read_info={
	"OSS read",
	0,
	MS_FILTER_AUDIO_IO,
	ms_snd_read_new,
	NULL
};

gint ms_snd_read_set_property(MSSndRead *f,MSFilterProperty prop, void *value)
{
	switch(prop){
		case MS_FILTER_PROPERTY_FREQ:
			f->freq=((gint*)value)[0];
		break;
	}
	return 0;
}
void ms_snd_read_notify_start(MSSndRead *f, MSSync *sync)
{
	ms_oss_read_start(f);
}

void ms_snd_read_notify_stop(MSSndRead *f)
{
	ms_snd_read_stop(f);
}

MSFilter * ms_snd_read_new()
{
	MSSndRead *w;
	
	w=g_new(MSSndRead,1);
	ms_snd_read_init(w);
	
	return(MS_FILTER(w));
}

declare_simple_static_fifo_setup(ms_snd_read,0,MS_SND_READ_MAX_GRAN)

MSFilterVTable ms_snd_read_vtable={
	process: (MSFilterProcessFunc)ms_snd_read_process,
	set_property: (MSFilterPropertyFunc)ms_snd_read_set_property,
	setup_link: simple_static_fifo_setup_ref(ms_snd_read),
	notify_start: (MSFilterNotifyStartFunc)ms_snd_read_notify_start,
	notify_stop: (MSFilterNotifyStopFunc)ms_snd_read_notify_stop
};

/* FOR INTERNAL USE*/
void ms_snd_read_init(MSSndRead *w)
{
	ms_filter_init((MSFilter*)w,&ms_snd_read_vtable,"MsSndCardRead",0,0,1,0);
	w->devid=0;
	w->sndcard=NULL;
	w->freq=8000;
}
	


void ms_snd_read_process(MSSndRead *f)
{
	MSFifo *fifo;
	char *p;
	fifo=f->base.outfifos[0];
	
	g_return_if_fail(f->sndcard!=NULL);
	g_return_if_fail(f->gran>0);
	
	if (snd_card_can_read(f->sndcard)){
		int got;
		p=ms_fifo_get_write_ptr(fifo,f->gran);
		got=snd_card_read(f->sndcard,p,f->gran);
		if (got>0 && got!=f->gran) ms_fifo_update_write_ptr(fifo,got);
	}		
}


void ms_snd_read_start(MSSndRead *r)
{
	g_return_if_fail(r->devid!=-1);
	r->sndcard=snd_card_manager_get_card(snd_card_manager,r->devid);
	g_return_if_fail(r->sndcard!=NULL);
	/* open the device for an audio telephony signal with minimum latency */
	snd_card_open_r(r->sndcard,16,0,r->freq);
	r->gran=(512*r->freq)/8000;
	
}

void ms_snd_read_stop(MSSndRead *w)
{
	g_return_if_fail(w->devid!=-1);
	g_return_if_fail(w->sndcard!=NULL);
	snd_card_close_r(w->sndcard);
	w->sndcard=NULL;
}





gint ms_snd_read_set_device(MSSndRead *r,gint devid)
{
	r->devid=devid;
	return 0;
}
