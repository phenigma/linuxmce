/***************************************************************************
 *            msringplayer.c
 *
 *  Fri Oct 15 16:13:08 2004
 *  Copyright  2004  Simon Morlat 
 *  Email <simon.morlat@linphone.org>
 ****************************************************************************/
/* The mediastreamer library creates and run multimedia streams in a modular
	way.
*/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "msringplayer.h"
#include "mssync.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "waveheader.h"

#define WAVE_HEADER_OFFSET sizeof(wave_header_t)

enum { PLAY_RING, PLAY_SILENCE};

static int supported_freq[6]={8000,11025,16000,22050,32000,44100};

gint freq_is_supported(gint freq){
	int i;
	for (i=0;i<6;i++){
		if (abs(supported_freq[i]-freq)<50) return supported_freq[i];
	}
	return 0;
}

gint ms_ring_player_set_property(MSRingPlayer *f,MSFilterProperty prop, void *value)
{
	switch(prop){
		case MS_FILTER_PROPERTY_FREQ:
			f->rate=((gint*)value)[0]*2;
			f->silence_bytes=f->silence*f->rate;
			if (f->filter.sync!=NULL)
				f->gran=(f->rate*f->filter.sync->interval/1000)*2;
		break;
	}
	return 0;
}

gint ms_ring_player_get_property(MSRingPlayer *f,MSFilterProperty prop, void *value)
{
	switch(prop){
		case MS_FILTER_PROPERTY_FREQ:
			((gint*)value)[0]=f->freq;
			
		break;
		case MS_FILTER_PROPERTY_CHANNELS:
			((gint*)value)[0]=f->channel;
		break;
	}
	return 0;
}

declare_simple_static_fifo_setup(ms_ring_player,0,MS_RING_PLAYER_DEF_GRAN)

MSFilterVTable ms_ring_player_vtable={
	process: (MSFilterProcessFunc)ms_ring_player_process,
	setup_link: simple_static_fifo_setup_ref(ms_ring_player),
	notify_start: (MSFilterNotifyStartFunc)ms_ring_player_setup,
	set_property: (MSFilterPropertyFunc)ms_ring_player_set_property,
	get_property: (MSFilterPropertyFunc)ms_ring_player_get_property,
	destroy: (MSFilterUninitFunc)ms_ring_player_uninit
};



/**
 * ms_ring_player_new:
 * @name:  The path to the 16-bit 8khz raw file to be played as a ring.
 * @seconds: The number of seconds that separates two rings.
 *
 * Allocates a new MSRingPlayer object.
 *
 *
 * Returns: a pointer the the object, NULL if name could not be open.
 */
MSFilter * ms_ring_player_new(char *name, gint seconds)
{
	MSRingPlayer *r;
	int fd=-1;
	
	if ((name!=NULL) && (strlen(name)!=0))
	{
		fd=open(name,O_RDONLY);
		if (fd<0) 
		{
			g_warning("ms_ring_player_new: failed to open %s.\n",name);
			return NULL;
		}
		
	}else {
		g_warning("ms_ring_player_new: Bad file name");
		return NULL;
	}
	
	r=g_new(MSRingPlayer,1);
	ms_ring_player_init(r);
	
	r->fd=fd;
	r->silence=seconds;
	r->freq=8000;
	if (strstr(name,".wav")!=NULL){
		wave_header_t header;
		int freq,freq2;
		/* read the header */
		read(fd,&header,sizeof(wave_header_t));
		freq=wave_header_get_rate(&header);
		if ((freq2=freq_is_supported(freq))>0){
			r->freq=freq2;
		}else {
			g_warning("Unsupported sampling rate %i",freq);
			r->freq=8000;
		}
		r->channel=wave_header_get_channel(&header);
		lseek(fd,WAVE_HEADER_OFFSET,SEEK_SET);
#ifdef WORDS_BIGENDIAN
		r->need_swap=1;	
#else
		r->need_swap=0;
#endif
	}
	ms_ring_player_set_property(r, MS_FILTER_PROPERTY_FREQ,&r->freq);
	r->state=PLAY_RING;
	return(MS_FILTER(r));
}
	

/* FOR INTERNAL USE*/
void ms_ring_player_init(MSRingPlayer *r)
{
	ms_filter_init(MS_FILTER(r),&ms_ring_player_vtable,"MSRingPlayer",0,0,1,0);
	r->fd=-1;
	r->current_pos=0;
	r->need_swap=0;
}



gint ms_ring_player_get_sample_freq(MSRingPlayer *obj){
	return obj->freq;
}


	
void ms_ring_player_process(MSRingPlayer *r)
{
	MSFifo *f;
	gint err;
	gint processed=0;
	gint gran=r->gran;
	char *p;
	
	g_return_if_fail(gran>0);
	/* process output fifos*/
	
	f=MS_FILTER(r)->outfifos[0];
	p=ms_fifo_get_write_ptr(f,gran);
	g_return_if_fail(p!=NULL);
	for (processed=0;processed<gran;){
		switch(r->state){
			case PLAY_RING:
				err=read(r->fd,&p[processed],gran-processed);
				if (err<0)
				{
					memset(&p[processed],0,gran-processed);
					processed=gran;
					g_warning("ms_ring_player_process: failed to read: %s.\n",strerror(errno));
					return;
				}
				else if (err<gran)
				{/* end of file */
					
					r->current_pos=r->silence_bytes;
					lseek(r->fd,WAVE_HEADER_OFFSET,SEEK_SET);
					r->state=PLAY_SILENCE;
          			ms_filter_notify_event(MS_FILTER(r),MS_RING_PLAYER_END_OF_RING_EVENT,NULL);
				}
				if (r->need_swap) swap_buffer(&p[processed],err);
				processed+=err;
			break;
			case PLAY_SILENCE:
				err=gran-processed;
				if  (r->current_pos>err){
					memset(&p[processed],0,err);
					r->current_pos-=gran;
					processed=gran;
				}else{
					memset(&p[processed],0,r->current_pos);
					processed+=r->current_pos;
					r->state=PLAY_RING;
				}
			break;
		}
	}
}

/**
 * ms_ring_player_destroy:
 * @obj: A valid MSRingPlayer object.
 *
 * Destroy a MSRingPlayer object.
 *
 *
 */

void ms_ring_player_uninit( MSRingPlayer *obj)
{
	if (obj->fd!=0) close(obj->fd);
}

void ms_ring_player_setup(MSRingPlayer *r,MSSync *sync)
{
	r->gran=(r->rate*sync->interval/1000)*r->channel;
}
