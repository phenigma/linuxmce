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

#include "msrtp.h"
#include "msticker.h"

#include "ortp/telephonyevents.h"

struct SenderData{
	RtpSession *session;
	uint32_t tsoff;
	uint32_t skip_until;
	int rate;
	char dtmf;
	bool_t skip;
};

typedef struct SenderData SenderData;

static void sender_init(MSFilter *f){
	SenderData *d=ms_new(SenderData,1);
	d->session=0;
	d->tsoff=0;
	d->skip_until=0;
	d->skip=FALSE;
	d->rate=8000;
	d->dtmf=0;
	f->data=d;
}

static void sender_uninit(MSFilter *f){
	SenderData *d=(SenderData *)f->data;
	ms_free(d);
}

static int sender_send_dtmf(MSFilter *f, void *arg){
	const char *dtmf=(const char *)arg;
	SenderData *d=(SenderData *)f->data;
	d->dtmf=dtmf[0];
	return 0;
}

static int sender_set_session(MSFilter *f, void *arg){
	SenderData *d=(SenderData *)f->data;
	RtpSession *s=(RtpSession*)arg;
	PayloadType *pt=rtp_profile_get_payload(rtp_session_get_profile(s),
			rtp_session_get_send_payload_type(s));
	if (pt!=NULL){
		d->rate=pt->clock_rate;
	}else{
		ms_warning("Sending undefined payload type ?");
	}
	d->session=s;
	return 0;
}

/* the goal of that function is to return a absolute timestamp closest to real time, with respect of given packet_ts, which is a relative to an undefined origin*/
static uint32_t get_cur_timestamp(MSFilter *f, uint32_t packet_ts){
	SenderData *d=(SenderData *)f->data;
	uint32_t curts=(f->ticker->time*d->rate)/1000LL;
	int diff;
	int delta=d->rate/50;       /*20 ms at 8000Hz*/
	uint32_t netts;
	
	netts=packet_ts+d->tsoff;
	diff=curts-netts;

	if ( (diff > delta ) || (diff<-(delta*5) )){
			d->tsoff=curts-packet_ts;
			netts=packet_ts+d->tsoff;
			ms_message("synchronizing timestamp, diff=%i",diff);
	}
	
	/*ms_message("returned ts=%u, orig_ts=%u",netts,packet_ts);*/
	return netts;
}

static void sender_process(MSFilter *f){
	SenderData *d=(SenderData *)f->data;
	RtpSession *s=d->session;
	mblk_t *im;
	uint32_t timestamp;
	if (s==NULL) return;
	if (d->dtmf!=0){
		timestamp=(f->ticker->time*d->rate)/1000LL;
		rtp_session_send_dtmf(s,d->dtmf,timestamp);
		d->dtmf=0;
		d->skip_until=timestamp+(3*160);
		d->skip=TRUE;
		ms_queue_flush(f->inputs[0]);
		return ;
	}
	if (d->skip){
		timestamp=(f->ticker->time*d->rate)/1000LL;
		if (RTP_TIMESTAMP_IS_NEWER_THAN(timestamp,d->skip_until)){
			d->skip=FALSE;
		}
		ms_queue_flush(f->inputs[0]);
		return;
	}
	while ((im=ms_queue_get(f->inputs[0]))!=NULL){
		mblk_t *header;
		header=rtp_session_create_packet(s,12,NULL,0);
		rtp_set_markbit(header,mblk_get_marker_info(im));
		header->b_cont=im;
		timestamp=get_cur_timestamp(f,mblk_get_timestamp_info(im));
		rtp_session_sendm_with_ts(s,header,timestamp);
	}
}

static MSFilterMethod sender_methods[]={
	{MS_RTP_SEND_SET_SESSION, sender_set_session},
	{MS_RTP_SEND_SEND_DTMF,	sender_send_dtmf},
	{0,NULL}
};

MSFilterDesc ms_rtp_send_desc={
	.id=MS_RTP_SEND_ID,
	.name="MSRtpSend",
	.text="RTP output filter",
	.category=MS_FILTER_OTHER,
	.ninputs=1,
	.noutputs=0,
	.init=sender_init,
	.process=sender_process,
	.uninit=sender_uninit,
	.methods=sender_methods
};

struct ReceiverData{
	RtpSession *session;
	int rate;
};

typedef struct ReceiverData ReceiverData;

static void receiver_init(MSFilter *f){
	ReceiverData *d=ms_new(ReceiverData,1);
	d->session=NULL;
	d->rate=8000;
	f->data=d;
}

static void receiver_uninit(MSFilter *f){
	ms_free(f->data);
}

static int receiver_set_session(MSFilter *f, void *arg){
	ReceiverData *d=(ReceiverData*)f->data;
	RtpSession *s=(RtpSession*)arg;
	PayloadType *pt=rtp_profile_get_payload(rtp_session_get_profile(s),
			rtp_session_get_recv_payload_type(s));
	if (pt!=NULL){
		d->rate=pt->clock_rate;
	}else{
		ms_warning("Receiving undefined payload type ?");
	}
	d->session=s;
	
	return 0;
}

static void receiver_process(MSFilter *f){
	ReceiverData *d=(ReceiverData*)f->data;
	mblk_t *m;
	uint32_t timestamp;
	if (d->session==NULL) return;
	timestamp=(f->ticker->time*d->rate)/1000LL;
	while ((m=rtp_session_recvm_with_ts(d->session,timestamp))!=NULL){
		mblk_t *payload=m->b_cont;
		mblk_set_timestamp_info(payload,rtp_get_timestamp(m));
		mblk_set_marker_info(payload,rtp_get_markbit(m));
		freeb(m);
		ms_queue_put(f->outputs[0],payload);
	}
}

static MSFilterMethod receiver_methods[]={
	{MS_RTP_RECV_SET_SESSION, receiver_set_session},
	{0,NULL}
};

MSFilterDesc ms_rtp_recv_desc={
	.id=MS_RTP_RECV_ID,
	.name="MSRtpRecv",
	.text="RTP input filter",
	.category=MS_FILTER_OTHER,
	.ninputs=0,
	.noutputs=1,
	.init=receiver_init,
	.process=receiver_process,
	.uninit=receiver_uninit,
	.methods=receiver_methods
};

MS_FILTER_DESC_EXPORT(ms_rtp_send_desc)
MS_FILTER_DESC_EXPORT(ms_rtp_recv_desc)
