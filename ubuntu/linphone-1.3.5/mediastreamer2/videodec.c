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
#include "msvideo.h"
#include "rfc2429.h"

#include <ffmpeg/avcodec.h>

extern void ms_ffmpeg_check_init();

typedef struct DecState{
	AVCodecContext av_context;
	AVCodec *av_codec;
	mblk_t *input;
	mblk_t *yuv_msg;
	int output_pix_fmt;
	MSVideoSize vsize;
}DecState;

static int dec_set_pix_fmt(MSFilter *f, void *arg){
	DecState *s=(DecState*)f->data;
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
	s->output_pix_fmt=pix_fmt;
	return 0;
}

static int dec_set_vsize(MSFilter *f, void *arg){
	DecState *s=(DecState*)f->data;
	MSVideoSize *vsize=(MSVideoSize*)arg;
	s->vsize=*vsize;
	return 0;
}

static void dec_init(MSFilter *f){
	DecState *s=ms_new(DecState,1);
	MSPixFmt fmt;
	ms_ffmpeg_check_init();
	
	avcodec_get_context_defaults(&s->av_context);
	s->av_codec=avcodec_find_decoder(CODEC_ID_H263);
	fmt=MS_YUV420P;
	s->input=NULL;
	s->yuv_msg=NULL;
	s->vsize=MS_VIDEO_SIZE_CIF;
	f->data=s;
	dec_set_pix_fmt(f,&fmt);
	
}

static void dec_uninit(MSFilter *f){
	DecState *s=(DecState*)f->data;
	if (s->input!=NULL) freemsg(s->input);
	if (s->yuv_msg!=NULL) freemsg(s->yuv_msg);
	ms_free(s);
}

static void dec_preprocess(MSFilter *f){
	DecState *s=(DecState*)f->data;
	if (s->av_context.codec==NULL){
		int error=avcodec_open(&s->av_context, s->av_codec);
		if (error!=0) ms_error("avcodec_open() failed: %i",error);
	}
}

static void dec_postprocess(MSFilter *f){
	DecState *s=(DecState*)f->data;
	if (s->av_context.codec!=NULL){
		avcodec_close(&s->av_context);
		s->av_context.codec=NULL;
	}
}

static void dec_process(MSFilter *f){
	DecState *s=(DecState*)f->data;
	AVFrame orig;
	AVFrame transformed;
	mblk_t *inm;
	int got_picture;
	AVCodecContext *ctx=&s->av_context;
	/* get a picture from the input queue */
	inm=ms_queue_get(f->inputs[0]);
	if (msgdsize(inm) >= 2){
		uint32_t *p = (uint32_t*)inm->b_rptr;
		char *ph=inm->b_rptr;
		int PLEN;
		int gob_num;
		bool_t P;
		
		P=rfc2429_get_P(ph);
		PLEN=rfc2429_get_PLEN(ph);
		/*printf("receiving new packet; P=%i; V=%i; PLEN=%i; PEBIT=%i\n",P,rfc2429_get_V(ph),PLEN,rfc2429_get_PEBIT(ph));
		*/
		gob_num = (ntohl(*p) >> 10) & 0x1f;
		/*ms_message("gob %i, size %i", gob_num, msgdsize(inm));
		ms_message("ms_AVdecoder_process: received %08x %08x", ntohl(p[0]), ntohl(p[1]));*/
		
		/* remove H.263 Payload Header */
		if (PLEN>0){
			/* we ignore the redundant picture header and
			directly go to the bitstream */
			inm->b_rptr+=PLEN;
		}
		if (P){
			inm->b_rptr[0]=inm->b_rptr[1]=0;
		}else{
			/* no PSC omitted */
			inm->b_rptr+=2;
		}
		
		/* accumulate the video packet until we have the rtp markbit*/
		if (s->input==NULL){
			s->input=inm;
		}else{
			concatb(s->input,inm);
		}
		
		if (mblk_get_marker_info(inm)){
			mblk_t *frame;
			int remain,len;
			/*ms_message("got marker bit !");*/
			
			msgpullup(s->input,-1);
			frame=s->input;
			s->input=NULL;
			while ( (remain=frame->b_wptr-frame->b_rptr)> 0) {
				len=avcodec_decode_video(&s->av_context,&orig,&got_picture,(uint8_t*)frame->b_rptr,remain );
				if (len<0) {
					ms_warning("ms_AVdecoder_process: error %i.",len);
					break;
				}
				if (got_picture) {
					/*ms_message(" got_picture: width=%i height=%i fmt=%i",
							ctx->width,ctx->height,ctx->pix_fmt);*/
					/* set the image in the wanted format */
					if (s->yuv_msg==NULL){
						s->yuv_msg=allocb(avpicture_get_size(s->output_pix_fmt,s->vsize.width,s->vsize.height),0);
						s->yuv_msg->b_wptr=s->yuv_msg->b_datap->db_lim;
					}
					avpicture_fill((AVPicture*)&transformed,(unsigned char *)s->yuv_msg->b_rptr,s->output_pix_fmt,s->vsize.width,s->vsize.height);
					img_convert((AVPicture*)&transformed, s->output_pix_fmt,
							(AVPicture*)&orig,ctx->pix_fmt,ctx->width,ctx->height);
					ms_queue_put(f->outputs[0],dupmsg(s->yuv_msg));
				}
				frame->b_rptr+=len;
			}
			freemsg(frame);
		}
	}else freemsg(inm);
}

static MSFilterMethod methods[]={
	{		MS_FILTER_SET_VIDEO_SIZE		,		dec_set_vsize	},
	{		MS_FILTER_SET_PIX_FMT			,		dec_set_pix_fmt},
	{		0												,		NULL				}
};

MSFilterDesc ms_h263_dec_desc={
	.id=MS_H263_DEC_ID,
	.name="MSH263Dec",
	.text="A H.263 decoder using ffmpeg library",
	.category=MS_FILTER_DECODER,
	.enc_fmt="H263-1998",
	.ninputs=1,
	.noutputs=1,
	.init=dec_init,
	.preprocess=dec_preprocess,
	.process=dec_process,
	.postprocess=dec_postprocess,
	.uninit=dec_uninit,
	.methods= methods
};

MS_FILTER_DESC_EXPORT(ms_h263_dec_desc)
