/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

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

#include "msfilter.h"

#include <speex/speex.h>

typedef struct EncState{
	int rate;
	int bitrate;
	int frame_size;
	void *state;
	uint32_t ts;
	MSBufferizer *bufferizer;
} EncState;

static void enc_init(MSFilter *f){
	EncState *s=ms_new(EncState,1);
	s->rate=8000;
	s->bitrate=-1;
	s->state=0;
	s->frame_size=0;
	s->ts=0;
	s->bufferizer=ms_bufferizer_new();
	f->data=s;
}

static void enc_uninit(MSFilter *f){
	EncState *s=(EncState*)f->data;
	ms_bufferizer_destroy(s->bufferizer);
	ms_free(s);
}

static void enc_preprocess(MSFilter *f){
	EncState *s=(EncState*)f->data;
	const SpeexMode *mode=NULL;
	int vbr=1;
	switch(s->rate){
		case 8000:
			mode=speex_mode_list[SPEEX_MODEID_NB];
			break;
		case 16000:
			mode=speex_mode_list[SPEEX_MODEID_WB];
			break;
		default:
			ms_fatal("Unsupported rate for speex encoder.");
	}
	s->state=speex_encoder_init(mode);
	if (speex_encoder_ctl(s->state,SPEEX_SET_VBR,&vbr)!=0){
		ms_error("Could not set vbr mode to speex encoder.");
	}
	if (s->bitrate!=-1){
		if (speex_encoder_ctl(s->state,SPEEX_SET_BITRATE,&s->rate)!=0){
			ms_error("Could not set bitrate %i to speex encoder.",s->rate);
		}
	}
	if (speex_encoder_ctl(s->state,SPEEX_SET_VBR,&vbr)!=0){
		ms_error("Could not set vbr mode to speex encoder.");
	}
	speex_mode_query(mode,SPEEX_MODE_FRAME_SIZE,&s->frame_size);
}

static void enc_process(MSFilter *f){
	EncState *s=(EncState*)f->data;
	mblk_t *im;
	int nbytes=s->frame_size*2;
	uint8_t *buf=(uint8_t*)alloca(nbytes);
	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		ms_bufferizer_put(s->bufferizer,im);
	}
	while(ms_bufferizer_read(s->bufferizer,buf,nbytes)==nbytes){
		mblk_t *om=allocb(nbytes,0);//too large...
		SpeexBits bits;
		speex_bits_init_buffer(&bits,om->b_wptr,nbytes);
		speex_encode_int(s->state,(int16_t*)buf,&bits);
		om->b_wptr+=speex_bits_nbytes(&bits);
		mblk_set_timestamp_info(om,s->ts);
		ms_queue_put(f->outputs[0],om);
		s->ts+=s->frame_size;
	}
}

static void enc_postprocess(MSFilter *f){
	EncState *s=(EncState*)f->data;
	speex_encoder_destroy(s->state);
	s->state=NULL;
}

static int enc_set_sr(MSFilter *f, void *arg){
	EncState *s=(EncState*)f->data;
	s->rate=((int*)arg)[0];
	return 0;
}

static int enc_set_br(MSFilter *f, void *arg){
	EncState *s=(EncState*)f->data;
	s->bitrate=((int*)arg)[0];
	return 0;
}


static MSFilterMethod enc_methods[]={
	{	MS_FILTER_SET_SAMPLE_RATE	,	enc_set_sr	},
	{	MS_FILTER_SET_BITRATE		,	enc_set_br	},
	{	0				,	NULL		}
};

MSFilterDesc ms_speex_enc_desc={
	.id=MS_SPEEX_ENC_ID,
	.name="MSSpeexEnc",
	.text="The free and wonderful speex codec",
	.category=MS_FILTER_ENCODER,
	.enc_fmt="speex",
	.ninputs=1,
	.noutputs=1,
	.init=enc_init,
	.preprocess=enc_preprocess,
	.postprocess=enc_postprocess,
	.process=enc_process,
	.uninit=enc_uninit,
	.methods=enc_methods
};


typedef struct DecState{
	int rate;
	int frsz;
	void *state;
} DecState;

static void dec_init(MSFilter *f){
	DecState *s=ms_new(DecState,1);
	s->rate=8000;
	s->frsz=0;
	s->state=NULL;
	f->data=s;
}

static void dec_uninit(MSFilter *f){
	DecState *s=(DecState*)f->data;
	ms_free(s);
}

static void dec_preprocess(MSFilter *f){
	DecState *s=(DecState*)f->data;
	const SpeexMode *mode=NULL;
	switch(s->rate){
		case 8000:
			mode=speex_mode_list[SPEEX_MODEID_NB];
			break;
		case 16000:
			mode=speex_mode_list[SPEEX_MODEID_WB];
			break;
		default:
			ms_fatal("Unsupported rate for speex decoder.");
	}
	s->state=speex_decoder_init(mode);
	speex_mode_query(mode,SPEEX_MODE_FRAME_SIZE,&s->frsz);
}

static void dec_postprocess(MSFilter *f){
	DecState *s=(DecState*)f->data;
	speex_decoder_destroy(s->state);
	s->state=NULL;
}

static int dec_set_sr(MSFilter *f, void *arg){
	DecState *s=(DecState*)f->data;
	s->rate=((int*)arg)[0];
	return 0;
}

static void dec_process(MSFilter *f){
	DecState *s=(DecState*)f->data;
	mblk_t *im;
	mblk_t *om;
	int err;
	SpeexBits bits;
	int bytes=s->frsz*2;
	speex_bits_init(&bits);
	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		speex_bits_read_from(&bits,im->b_rptr,im->b_wptr-im->b_rptr);
		om=allocb(bytes,0);
		err=speex_decode_int(s->state,&bits,(int16_t*)om->b_wptr);
		if (err==0){
			om->b_wptr+=bytes;
			ms_queue_put(f->outputs[0],om);
		}else {
			if (err==-1)
				ms_warning("speex end of stream");
			else if (err==-2)
				ms_warning("speex corrupted stream");
			freemsg(om);
		}
		freemsg(im);
	}
	speex_bits_destroy(&bits);
}

static MSFilterMethod dec_methods[]={
	{	MS_FILTER_SET_SAMPLE_RATE	,	dec_set_sr	},
	{	0				,	NULL		}
};


MSFilterDesc ms_speex_dec_desc={
	.id=MS_SPEEX_DEC_ID,
	.name="MSSpeexDec",
	.text="The free and wonderful speex codec",
	.category=MS_FILTER_DECODER,
	.enc_fmt="speex",
	.ninputs=1,
	.noutputs=1,
	.init=dec_init,
	.preprocess=dec_preprocess,
	.postprocess=dec_postprocess,
	.process=dec_process,
	.uninit=dec_uninit,
	.methods=dec_methods
};

MS_FILTER_DESC_EXPORT(ms_speex_dec_desc)
MS_FILTER_DESC_EXPORT(ms_speex_enc_desc)
