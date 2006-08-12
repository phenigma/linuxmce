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

#include "mssndwrite.h"
#include "mssync.h"
#include <unistd.h>
#include <math.h>

#define MS_SND_WRITE_DEF_GRAN (512*2) /* the default granularity*/

MSFilterInfo snd_write_info={
	"SND write",
	0,
	MS_FILTER_OTHER,
	ms_snd_write_new,
	NULL
};

/* FOR INTERNAL USE*/
void ms_snd_write_init(MSSndWrite *r);
void ms_snd_write_setup(MSSndWrite *r);
void ms_snd_write_process(MSSndWrite *f);


MSFilter * ms_snd_write_new()
{
	MSSndWrite *w;
	w=g_new0(MSSndWrite,1);
	ms_snd_write_init(w);
	return MS_FILTER(w);
}

gint ms_snd_write_set_property(MSSndWrite *f,MSFilterProperty prop, void *value)
{
	switch(prop){
		case MS_FILTER_PROPERTY_FREQ:
			f->freq=((gint*)value)[0];
		break;
		case MS_FILTER_PROPERTY_CHANNELS:
			f->channels=((gint*)value)[0];
		break;
	}
	return 0;
}
void ms_snd_write_notify_start(MSSndWrite *r, MSSync *sync)
{
	//g_message("starting MSSndWrite..");
	ms_snd_write_start(r);
}
void ms_snd_write_notify_stop(MSSndWrite *r){
	ms_snd_write_stop(r);
}

declare_simple_static_fifo_setup(snd_write,MS_SND_WRITE_DEF_GRAN,0)

MSFilterVTable ms_snd_write_vtable={
	process: (MSFilterProcessFunc)ms_snd_write_process,
	set_property: (MSFilterPropertyFunc)ms_snd_write_set_property,
	notify_start: (MSFilterNotifyStartFunc) ms_snd_write_notify_start,
	notify_stop: (MSFilterNotifyStopFunc) ms_snd_write_notify_stop,
	setup_link: simple_static_fifo_setup_ref(snd_write)
};

/* FOR INTERNAL USE*/
void ms_snd_write_init(MSSndWrite *w)
{
	ms_filter_init(MS_FILTER(w),&ms_snd_write_vtable,"MSSndWrite",1,0,0,0);
	MS_FILTER(w)->r_mingran=512;  /* very few cards can do that...*/
	w->devid=0;
	w->sndcard=NULL;
	w->freq=8000;
	w->channels=1;
	w->dtmf_time=-1;
}


void ms_snd_write_process(MSSndWrite *f)
{
	MSFifo *fifo;
	char *p;
	int i;
	gint gran=ms_filter_get_mingran(MS_FILTER(f));
	
	/* always consume something */
	fifo=f->base.infifos[0];
	p=ms_fifo_get_read_ptr(fifo,gran);
	g_return_if_fail(f->sndcard!=NULL);
	if (f->dtmf_time!=-1){
		gint16 *buf=(gint16*)p;
		/* generate a DTMF*/
		for(i=0;i<gran/2;i++){
			buf[i]=(gint16)(10000.0*sin(2*M_PI*(double)f->dtmf_time*f->lowfreq));
			buf[i]+=(gint16)(10000.0*sin(2*M_PI*(double)f->dtmf_time*f->highfreq));
			f->dtmf_time++;
			//printf("buf[%i]=%i\n",i,buf[i]);
		}
		if (f->dtmf_time>f->dtmf_duration) f->dtmf_time=-1; /*finished*/
	}
	snd_card_write(f->sndcard,p,gran);
}

void ms_snd_write_start(MSSndWrite *w)
{
	gint bsize;
	g_return_if_fail(w->devid!=-1);
	w->sndcard=snd_card_manager_get_card(snd_card_manager,w->devid);
	g_return_if_fail(w->sndcard!=NULL);
	/* open the device for an audio telephony signal with minimum latency */
	snd_card_open_w(w->sndcard,16,w->channels==2,w->freq);
	w->bsize=snd_card_get_bsize(w->sndcard);
}

void ms_snd_write_stop(MSSndWrite *w)
{
	g_return_if_fail(w->devid!=-1);
	g_return_if_fail(w->sndcard!=NULL);
	snd_card_close_w(w->sndcard);
	w->sndcard=NULL;
}

gint ms_snd_write_set_device(MSSndWrite *w, gint devid)
{
	w->devid=devid;
	return 0;
}


void ms_snd_write_play_dtmf(MSSndWrite *w, char dtmf){
	
	w->dtmf_duration=0.1*w->freq;
	switch(dtmf){
		case '0':
			w->lowfreq=941;
			w->highfreq=1336;
			break;
		case '1':
			w->lowfreq=697;
			w->highfreq=1209;
			break;
		case '2':
			w->lowfreq=697;
			w->highfreq=1336;
			break;
		case '3':
			w->lowfreq=697;
			w->highfreq=1477;
			break;
		case '4':
			w->lowfreq=770;
			w->highfreq=1209;
			break;
		case '5':
			w->lowfreq=770;
			w->highfreq=1336;
			break;
		case '6':
			w->lowfreq=770;
			w->highfreq=1477;
			break;
		case '7':
			w->lowfreq=852;
			w->highfreq=1209;
			break;
		case '8':
			w->lowfreq=852;
			w->highfreq=1336;
			break;
		case '9':
			w->lowfreq=852;
			w->highfreq=1477;
			break;
		case '*':
			w->lowfreq=941;
			w->highfreq=1209;
			break;
		case '#':
			w->lowfreq=941;
			w->highfreq=1477;
			break;
		case 'A':
			w->lowfreq=697;
			w->highfreq=1633;
			break;
		case 'B':
			w->lowfreq=770;
			w->highfreq=1633;
			break;
		case 'C':
			w->lowfreq=852;
			w->highfreq=1633;
			break;
		case 'D':
			w->lowfreq=941;
			w->highfreq=1633;
			break;	
		default:
			g_warning("Not a dtmf key.");
			return;
	}
	w->lowfreq=w->lowfreq/w->freq;
	w->highfreq=w->highfreq/w->freq;
	w->dtmf_time=0;
}
