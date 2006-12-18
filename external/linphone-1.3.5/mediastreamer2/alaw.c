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
#include "g711common.h"

typedef struct _AlawEncData{
	MSBufferizer *bz;
	uint32_t ts;
} AlawEncData;

static AlawEncData * alaw_enc_data_new(){
	AlawEncData *obj=ms_new(AlawEncData,1);
	obj->bz=ms_bufferizer_new();
	obj->ts=0;
	return obj;
}

static void alaw_enc_data_destroy(AlawEncData *obj){
	ms_bufferizer_destroy(obj->bz);
	ms_free(obj);
}

static void alaw_enc_init(MSFilter *obj){
	obj->data=alaw_enc_data_new();
}

static void alaw_enc_uninit(MSFilter *obj){
	alaw_enc_data_destroy((AlawEncData*)obj->data);
}

static void alaw_enc_process(MSFilter *obj){
	AlawEncData *dt=(AlawEncData*)obj->data;
	MSBufferizer *bz=dt->bz;
	
	mblk_t *m;
	while((m=ms_queue_get(obj->inputs[0]))!=NULL){
		uint8_t buffer[320];
		ms_bufferizer_put(bz,m);
		while (ms_bufferizer_read(bz,buffer,sizeof(buffer))==sizeof(buffer)){
			mblk_t *o=allocb(sizeof(buffer)/2,0);
			int i;
			for (i=0;i<sizeof(buffer)/2;i++){
				*o->b_wptr=s16_to_alaw(((int16_t*)buffer)[i]);
				o->b_wptr++;
			}
			mblk_set_timestamp_info(o,dt->ts);
			dt->ts+=sizeof(buffer)/2;
			ms_queue_put(obj->outputs[0],o);
		}
	}
}

MSFilterDesc ms_alaw_enc_desc={
	.id=MS_ALAW_ENC_ID,
	.name="MSAlawEnc",
	.text="ITU-G.711 alaw encoder",
	.category=MS_FILTER_ENCODER,
	.enc_fmt="pcma",
	.ninputs=1,
	.noutputs=1,
	.init=alaw_enc_init,
	.process=alaw_enc_process,
	.uninit=alaw_enc_uninit
};


static void alaw_dec_process(MSFilter *obj){
	mblk_t *m;
	while((m=ms_queue_get(obj->inputs[0]))!=NULL){
		mblk_t *o;
		msgpullup(m,-1);
		o=allocb((m->b_wptr-m->b_rptr)*2,0);
		for(;m->b_rptr<m->b_wptr;m->b_rptr++,o->b_wptr+=2){
			*((int16_t*)(o->b_wptr))=alaw_to_s16(*m->b_rptr);
		}
		freemsg(m);
		ms_queue_put(obj->outputs[0],o);
	}
}


MSFilterDesc ms_alaw_dec_desc={
	.id=MS_ALAW_DEC_ID,
	.name="MSAlawDec",
	.text="ITU-G.711 alaw decoder",
	.category=MS_FILTER_DECODER,
	.enc_fmt="pcma",
	.ninputs=1,
	.noutputs=1,
	.process=alaw_dec_process,
};

MS_FILTER_DESC_EXPORT(ms_alaw_dec_desc)
MS_FILTER_DESC_EXPORT(ms_alaw_enc_desc)
