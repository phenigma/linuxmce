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


#include "mediastream.h"
#include "msfilter.h"
#include "msvideo.h"
#include "msrtp.h"
#include "msv4l.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern RtpSession * create_duplex_rtpsession(RtpProfile *profile, int locport,const char *remip,int remport,
				int payload,int jitt_comp);

#define MAX_RTP_SIZE	UDP_MAX_SIZE

/* this code is not part of the library itself, it is part of the mediastream program */
void video_stream_free (VideoStream * stream)
{
	if (stream->session!=NULL)
		rtp_session_destroy(stream->session);
	if (stream->rtprecv != NULL)
		ms_filter_destroy (stream->rtprecv);
	if (stream->rtpsend!=NULL) 
		ms_filter_destroy (stream->rtpsend);
	if (stream->source != NULL)
		ms_filter_destroy (stream->source);
	if (stream->output != NULL)
		ms_filter_destroy (stream->output);
	if (stream->decoder != NULL)
		ms_filter_destroy (stream->decoder);
	if (stream->encoder != NULL)
		ms_filter_destroy (stream->encoder);
	if (stream->ticker != NULL)
		ms_ticker_destroy (stream->ticker);
	ms_free (stream);
}


VideoStream *
video_stream_start (RtpProfile *profile, int locport, const char *remip, int remport,
			 int payload, int jitt_comp,  const char *device)
{
	VideoStream *stream = ms_new0 (VideoStream, 1);
	PayloadType *pt;
	MSPixFmt format;
	/*MSVideoSize vsize=MS_VIDEO_SIZE_CIF;*/
	float fps=7;

	stream->session=create_duplex_rtpsession(profile,locport,remip,remport,payload,jitt_comp);
	rtp_session_enable_adaptive_jitter_compensation(stream->session,TRUE);
	
	/* creates two rtp filters to recv send streams (remote part) */
	rtp_session_set_recv_buf_size(stream->session,MAX_RTP_SIZE);
	stream->rtpsend =ms_filter_new(MS_RTP_SEND_ID);
	if (remport>0) ms_filter_call_method(stream->rtpsend,MS_RTP_SEND_SET_SESSION,stream->session);
	
	stream->rtprecv = ms_filter_new (MS_RTP_RECV_ID);
	ms_filter_call_method(stream->rtprecv,MS_RTP_RECV_SET_SESSION,stream->session);
	
	pt=rtp_profile_get_payload(profile,payload);
	if (pt==NULL){
		video_stream_free(stream);
		ms_error("videostream.c: undefined payload type.");
		return NULL;
	}

	/* creates the filters */
	stream->source = ms_filter_new(MS_V4L_ID);
	stream->output=ms_filter_new(MS_SDL_OUT_ID);

	stream->encoder=ms_filter_create_encoder(pt->mime_type);
	stream->decoder=ms_filter_create_decoder(pt->mime_type);
	if ((stream->encoder==NULL) || (stream->decoder==NULL)){
		/* big problem: we have not a registered codec for this payload...*/
		ms_error("videostream.c: No codecs available for payload %i:%s.",payload,pt->mime_type);
		video_stream_free(stream);
		return NULL;
	}

	/* configure the filters */
	ms_filter_call_method_noarg(stream->source,MS_V4L_START);
	ms_filter_call_method(stream->source,MS_FILTER_SET_FPS,&fps);
	/* get the output format for webcam reader */
	ms_filter_call_method(stream->source,MS_FILTER_GET_PIX_FMT,&format);
	/*set it to the encoder */
	ms_filter_call_method(stream->encoder,MS_FILTER_SET_PIX_FMT,&format);
	/*force the decoder to output YUV420P */
	format=MS_YUV420P;
	ms_filter_call_method(stream->decoder,MS_FILTER_SET_PIX_FMT,&format);
	ms_filter_call_method(stream->output,MS_FILTER_SET_PIX_FMT,&format);

	if (pt->send_fmtp!=NULL) ms_filter_call_method(stream->encoder,MS_FILTER_SET_FMTP, (void*)pt->send_fmtp);
	if (pt->recv_fmtp!=NULL) ms_filter_call_method(stream->decoder,MS_FILTER_SET_FMTP,(void*)pt->recv_fmtp);
	
	/* and then connect all */
	ms_filter_link (stream->source, 0, stream->encoder, 0);
	ms_filter_link (stream->encoder,0, stream->rtpsend,0);
	
	ms_filter_link (stream->rtprecv, 0, stream->decoder, 0);
	ms_filter_link (stream->decoder,0 , stream->output, 0);
	/* encoder outputs YUV420P on pin1 that can be used for preview */
	ms_filter_link(stream->encoder,1,stream->output,1);

	/* create the ticker */
	stream->ticker = ms_ticker_new(); 
	/* attach it the graph */
	ms_ticker_attach (stream->ticker, stream->source);
	return stream;
}



void
video_stream_stop (VideoStream * stream)
{

	ms_ticker_detach(stream->ticker,stream->source);
	ms_filter_call_method_noarg(stream->source,MS_V4L_STOP);
	
	ms_filter_unlink(stream->source,0,stream->encoder,0);
	ms_filter_unlink(stream->encoder, 0, stream->rtpsend,0);
	ms_filter_unlink(stream->rtprecv, 0, stream->decoder, 0);
	ms_filter_unlink(stream->decoder,0,stream->output,0);
	ms_filter_unlink(stream->encoder,1,stream->output,1);
	video_stream_free (stream);
}


void video_stream_set_rtcp_information(VideoStream *st, const char *cname){
	if (st->session!=NULL){
		rtp_session_set_source_description(st->session,cname,NULL,NULL,NULL,NULL,"linphone-" LINPHONE_VERSION,
											"This is free software (GPL) !");
	}
}



VideoStream * video_preview_start(const char *device){
	VideoStream *stream = ms_new0 (VideoStream, 1);
	MSPixFmt format;

	/* creates the filters */
	stream->source = ms_filter_new(MS_V4L_ID);
	stream->output = ms_filter_new(MS_SDL_OUT_ID);

	/* configure the filters */
	ms_filter_call_method_noarg(stream->source,MS_V4L_START);
	ms_filter_call_method(stream->source,MS_FILTER_GET_PIX_FMT,&format);
	ms_filter_call_method(stream->output,MS_FILTER_SET_PIX_FMT,&format);
	
	/* and then connect all */
	ms_filter_link (stream->source,0, stream->output,0);
	/* create the ticker */
	stream->ticker = ms_ticker_new(); 
	ms_ticker_attach (stream->ticker, stream->source);
	return stream;
}

void video_preview_stop(VideoStream *stream){
	ms_ticker_detach(stream->ticker, stream->source);
	ms_filter_call_method_noarg(stream->source,MS_V4L_STOP);
	ms_filter_unlink(stream->source,0,stream->output,0);
	video_stream_free(stream);
}


VideoStream * video_stream_send_only_start(RtpProfile *profile, int locport, const char *remip, int remport,
			 int payload, const char *device)
{
	VideoStream *stream = ms_new0 (VideoStream, 1);
	PayloadType *pt;
	MSPixFmt format;
	/*MSVideoSize vsize=MS_VIDEO_SIZE_CIF;*/
	float fps=7;

	stream->session=create_duplex_rtpsession(profile,locport,remip,remport,payload,60);
	/*rtp_session_enable_adaptive_jitter_compensation(stream->session,FALSE);*/
	
	/* creates two rtp filters to recv send streams (remote part) */
	rtp_session_set_recv_buf_size(stream->session,MAX_RTP_SIZE);
	stream->rtpsend =ms_filter_new(MS_RTP_SEND_ID);
	if (remport>0) ms_filter_call_method(stream->rtpsend,MS_RTP_SEND_SET_SESSION,stream->session);
	
	pt=rtp_profile_get_payload(profile,payload);
	if (pt==NULL){
		video_stream_free(stream);
		ms_error("videostream.c: undefined payload type.");
		return NULL;
	}

	/* creates the filters */
	stream->source = ms_filter_new(MS_V4L_ID);

	stream->encoder=ms_filter_create_encoder(pt->mime_type);
	if ((stream->encoder==NULL)){
		/* big problem: we have not a registered codec for this payload...*/
		video_stream_free(stream);
		ms_error("videostream.c: No codecs available for payload %i.",payload);
		return NULL;
	}

	/* configure the filters */
	ms_filter_call_method_noarg(stream->source,MS_V4L_START);
	ms_filter_call_method(stream->source,MS_FILTER_SET_FPS,&fps);
	/* get the output format for webcam reader */
	ms_filter_call_method(stream->source,MS_FILTER_GET_PIX_FMT,&format);
	/*set it to the encoder */
	ms_filter_call_method(stream->encoder,MS_FILTER_SET_PIX_FMT,&format);
	/*force the decoder to output YUV420P */

	/* and then connect all */
	ms_filter_link (stream->source, 0, stream->encoder, 0);
	ms_filter_link (stream->encoder,0, stream->rtpsend,0);

	/* create the ticker */
	stream->ticker = ms_ticker_new(); 
	/* attach it the graph */
	ms_ticker_attach (stream->ticker, stream->source);
	return stream;
}

void video_stream_send_only_stop(VideoStream *stream){
	ms_ticker_detach (stream->ticker, stream->source);
	ms_filter_call_method_noarg(stream->source,MS_V4L_STOP);
	ms_filter_unlink(stream->source,0,stream->encoder,0);
	ms_filter_unlink(stream->encoder,0,stream->rtpsend,0);
	
	video_stream_free(stream);
}
