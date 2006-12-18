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

#include <ffmpeg/avcodec.h>
#include "msfilter.h"
#include "msvideo.h"
#include "msticker.h"

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netinet/in.h>			/* ntohl(3) */
#endif

#include "rfc2429.h"

static bool_t avcodec_initialized=FALSE;

void ms_ffmpeg_check_init(){
	if(!avcodec_initialized){
		avcodec_init();
		avcodec_register_all();
		avcodec_initialized=TRUE;
	}
}

typedef struct EncState{
	AVCodecContext av_context;
	AVCodec *av_codec;
	int input_pix_fmt; /* in ffmpeg enum*/
	mblk_t *yuv_msg;
	mblk_t *comp_buf;
}EncState;

static int enc_set_fps(MSFilter *f, void *arg){
	EncState *s=(EncState*)f->data;
	float fps=*(float*)arg;
	s->av_context.time_base.num = (int)fps;
	s->av_context.time_base.den = 1;
	return 0;
}

static int enc_set_format(MSFilter *f,void *arg){
	EncState *s=(EncState*)f->data;
	MSPixFmt fmt=*(MSPixFmt*)arg;
	int pix_fmt;
	switch(fmt){
		case MS_RGB24:
			pix_fmt=PIX_FMT_RGB24;
			break;
		case MS_YUV420P:
			pix_fmt=PIX_FMT_YUV420P;
			break;
		default:
			pix_fmt=MS_YUV420P;
			ms_fatal("Unsupported pixel format");
	}
	s->input_pix_fmt=pix_fmt;
	return 0;
}

static void enc_init(MSFilter *f)
{
	EncState *s=ms_new(EncState,1);
	AVCodecContext *c=&s->av_context;
	float fps=15;
	MSPixFmt pixfmt;
	f->data=s;
	ms_ffmpeg_check_init();
	avcodec_get_context_defaults(c);

	/* put sample parameters */
	c->bit_rate = 20000;
	/* resolution must be a multiple of two */
	c->width = MS_VIDEO_SIZE_CIF_W;  
	c->height = MS_VIDEO_SIZE_CIF_H;
	enc_set_fps(f,&fps);
	c->gop_size = 30; /* emit one intra frame every x frames */
	/* we don't use the rtp_callback but use rtp_mode that forces ffmpeg to insert PSC
	(Picture Start Code) as far as possible in the bitstream */
	c->rtp_mode = 1;
	c->rtp_payload_size = 1000;
	c->pix_fmt=PIX_FMT_YUV420P;
	
	s->av_codec=avcodec_find_encoder(CODEC_ID_H263);
	/*set default input format */
	pixfmt=MS_RGB24;
	enc_set_format(f,&pixfmt);
	s->yuv_msg=NULL;
	s->comp_buf=allocb(32000,0);
}


static void enc_uninit(MSFilter  *f){
	EncState *s=(EncState*)f->data;
	if (s->comp_buf!=NULL)	freemsg(s->comp_buf);
	if (s->yuv_msg!=NULL) 	freemsg(s->yuv_msg);
	ms_free(s);
}

static void enc_preprocess(MSFilter *f){
	EncState *s=(EncState*)f->data;
	int error=avcodec_open(&s->av_context, s->av_codec);
	if (error!=0) {
		ms_error("avcodec_open() failed: %i",error);
		return;
	}
	ms_debug("image format is %i.",s->av_context.pix_fmt);
}

static void enc_postprocess(MSFilter *f){
	EncState *s=(EncState*)f->data;
	if (s->av_context.codec!=NULL){
		avcodec_close(&s->av_context);
		s->av_context.codec=NULL;
	}
}

static void generate_packets(MSFilter *f, EncState *s, mblk_t *frame, uint32_t timestamp, char *psc, char *end, bool_t last_packet){
	mblk_t *packet;
	packet=dupb(frame);
	packet->b_rptr=psc;
	packet->b_wptr=end;
	/*ms_message("generating packet of size %i",end-psc);*/
	rfc2429_set_P(psc,1);
	mblk_set_timestamp_info(packet,timestamp);
	mblk_set_marker_info(packet,last_packet);
	ms_queue_put(f->outputs[0],packet);
}

static void split_and_send(MSFilter *f, EncState *s, mblk_t *frame){
	char *p;
	char *lastpsc=frame->b_rptr;
	int len;
	uint32_t timestamp=f->ticker->time*90LL;
	char pm=frame->b_rptr[0];
	
	ms_debug("processing frame of size %i",frame->b_wptr-frame->b_rptr);
	
	for (p=frame->b_rptr+1;p<frame->b_wptr;++p){
		if (pm==0 && p[0]==0){
			char *psc=p-1;
			len=psc-lastpsc;
			ms_debug("PSC detected at %i",psc-frame->b_rptr);
			if (len>0){
				generate_packets(f,s,frame, timestamp,lastpsc,psc,FALSE);
			}
			lastpsc=psc;
			p++;/* to skip possible 0 after the PSC that would make a double detection */
		}
		pm=p[0];
	}
	/* send the end of frame */
	generate_packets(f,s,frame, timestamp,lastpsc,frame->b_wptr,TRUE);
}

static void enc_process(MSFilter *f){
	EncState *s=(EncState*)f->data;
	AVFrame orig;
	AVFrame pict;
	AVCodecContext *c=&s->av_context;
	mblk_t *inm;
	int error;
	/* get a picture from the input queue */
	inm=ms_queue_get(f->inputs[0]);
	
	/* allocate a new image */
	if (s->yuv_msg==NULL){
		int bsize = avpicture_get_size(c->pix_fmt,c->width,c->height);
		s->yuv_msg=allocb(bsize,0);
		s->yuv_msg->b_wptr+=bsize;
	}
	if (s->av_context.codec != NULL){
		mblk_t *comp_buf=s->comp_buf;
		int comp_buf_sz=comp_buf->b_datap->db_lim-comp_buf->b_datap->db_base;
		/* convert image if necessary */
		if (s->input_pix_fmt!=c->pix_fmt){
			ms_debug("Changing picture format.");
			avpicture_fill((AVPicture*)&orig,(uint8_t*)inm->b_rptr,s->input_pix_fmt,c->width,c->height);
			avpicture_fill((AVPicture*)&pict,(uint8_t*)s->yuv_msg->b_rptr,c->pix_fmt,c->width,c->height);
			if (img_convert((AVPicture*)&pict,c->pix_fmt,(AVPicture*)&orig,s->input_pix_fmt,c->width,c->height) < 0) {
				ms_warning("img_convert failed");
				return;
			}
			if (f->outputs[1]!=NULL) ms_queue_put(f->outputs[1],dupmsg(s->yuv_msg));
		}
		else 
		{
			avpicture_fill((AVPicture*)&pict,(uint8_t*)inm->b_rptr,c->pix_fmt,c->width,c->height);
			if (f->outputs[1]!=NULL) ms_queue_put(f->outputs[1],dupmsg(inm));
		}
		/* timestamp used by ffmpeg, unset here */
		pict.pts=AV_NOPTS_VALUE;
		comp_buf->b_rptr=comp_buf->b_wptr=comp_buf->b_datap->db_base;
		error=avcodec_encode_video(c, (uint8_t*)s->comp_buf->b_wptr,comp_buf_sz, &pict);
		if (error<=0) ms_warning("ms_AVencoder_process: error %i.",error);
		else{
			comp_buf->b_wptr+=error;
			split_and_send(f,s,comp_buf);
		}
		freemsg(inm);
	}
}

static MSFilterMethod methods[]={
	{	MS_FILTER_SET_FPS	,	enc_set_fps	},
	{	MS_FILTER_SET_PIX_FMT	,	enc_set_format	},
	{	0										,	NULL	}
};

MSFilterDesc ms_h263_enc_desc={
	.id=MS_H263_ENC_ID,
	.name="MSH263Enc",
	.text="A video H.263 encoder using ffmpeg library.",
	.category=MS_FILTER_ENCODER,
	.enc_fmt="H263-1998",
	.ninputs=1,
	.noutputs=2,
	.init=enc_init,
	.preprocess=enc_preprocess,
	.process=enc_process,
	.postprocess=enc_postprocess,
	.uninit=enc_uninit,
	.methods=methods
};

MS_FILTER_DESC_EXPORT(ms_h263_enc_desc)
