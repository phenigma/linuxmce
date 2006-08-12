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


#include <msAlawdec.h>
#include <g711common.h>

/* tuning parameters :*/

#define ALAW_DECODER_RMAXGRAN 160
#define ALAW_DECODER_WMAXGRAN 320
extern MSFilter * ms_ALAWencoder_new(void);

MSCodecInfo ALAWinfo={
	{
		"ALAW codec",
		0,
		MS_FILTER_AUDIO_CODEC,
		ms_ALAWencoder_new,
		"This is the classic A-law codec. Good quality, but only usable with high speed network connections."
	},
	ms_ALAWencoder_new,
	ms_ALAWdecoder_new,
	ALAW_DECODER_RMAXGRAN,
	ALAW_DECODER_WMAXGRAN,
	64000,
	8000,
	8,
	"PCMA",
	1,
	1,
};

declare_simple_static_fifo_setup(alaw_decoder,ALAW_DECODER_RMAXGRAN,ALAW_DECODER_WMAXGRAN)

MSFilterVTable ms_ALAWdecoder_vtable=
{
	process: ms_ALAWdecoder_process,
	setup_link: simple_static_fifo_setup_ref(alaw_decoder)
};

MSFilter * ms_ALAWdecoder_new(void)
{
	MSALAWDecoder *r;
	
	r=g_new(MSALAWDecoder,1);
	ms_ALAWdecoder_init(r);
	return(MS_FILTER(r));
}
	

/* FOR INTERNAL USE*/
void ms_ALAWdecoder_init(MSALAWDecoder *r)
{
	ms_filter_init(MS_FILTER(r),&ms_ALAWdecoder_vtable,"MSALAWDecoder",1,0,1,0);
}

	
void ms_ALAWdecoder_process(MSALAWDecoder *r)
{
	MSFifo *fi,*fo;
	int inlen,outlen;
	gchar *s,*d;
	int i;
	
	fi=r->base.infifos[0];
	fo=r->base.outfifos[0];
	g_return_if_fail(fi!=NULL);
	g_return_if_fail(fo!=NULL);
	
 	s=ms_fifo_get_read_ptr(fi,ALAW_DECODER_RMAXGRAN);
 	d=ms_fifo_get_write_ptr(fo,ALAW_DECODER_WMAXGRAN);
 	
 	for(i=0;i<ALAW_DECODER_RMAXGRAN;i++)
 	{
 		((gint16*)d)[i]=alaw_to_s16( (unsigned char) s[i]);
 	}
}
