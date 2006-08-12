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


#include "msAlawenc.h"
#include "g711common.h"

extern MSCodecInfo ALAWinfo;


/* FOR INTERNAL USE*/
void ms_ALAWencoder_init(MSALAWEncoder *r);
void ms_ALAWencoder_process(MSALAWEncoder *r);

/* tuning parameters :*/
#define ALAW_ENCODER_WMAXGRAN 160
#define ALAW_ENCODER_RMAXGRAN 320

MSFilter * ms_ALAWencoder_new(void)
{
	MSALAWEncoder *r;
	r=g_new(MSALAWEncoder,1);
	ms_ALAWencoder_init(r);
	return(MS_FILTER(r));
}

declare_simple_static_fifo_setup(alaw_encoder,ALAW_ENCODER_RMAXGRAN,ALAW_ENCODER_WMAXGRAN)

MSFilterVTable ms_ALAWencoder_vtable={
	process: ms_ALAWencoder_process,
	setup_link: simple_static_fifo_setup_ref(alaw_encoder)
};

/* FOR INTERNAL USE*/
void ms_ALAWencoder_init(MSALAWEncoder *r)
{
	ms_filter_init(MS_FILTER(r),&ms_ALAWencoder_vtable,"MSALAWEncoder",1,0,1,0);
	
}
	
void ms_ALAWencoder_process(MSALAWEncoder *r)
{
	MSFifo *fi,*fo;
	gchar *s,*d;
	int i;
	
	fi=r->base.infifos[0];
	fo=r->base.outfifos[0];
	
 	s=ms_fifo_get_read_ptr(fi,ALAW_ENCODER_RMAXGRAN);
 	d=ms_fifo_get_write_ptr(fo,ALAW_ENCODER_WMAXGRAN);
 	for(i=0;i<ALAW_ENCODER_WMAXGRAN;i++)
 	{
 		d[i]=s16_to_alaw( *((gint16*)s) );
 		s+=2;
 	}
}
