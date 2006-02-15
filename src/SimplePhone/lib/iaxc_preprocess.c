/*
 * iaxc_preprocess.c
 *
 * Copyrights:
 * Copyright (C) 2005, Tipic, Inc.
 *
 * Contributors:
 * Francesco Delfino <pluto@tipic.com>
 *
 *
 * This program is free software, distributed under the terms of
 * the GNU Lesser (Library) General Public License
 */
//#define USE_AEC_NLMS

#include "iaxclient_lib.h"
#include "pablio.h"
#ifdef USE_AEC_NLMS
#include "aec_nlms/aec_cwrap.h"
#endif

#ifndef WIN32
  #define min(x,y)	(x) >= (y) ? (y) : (x)
#endif


// Robert Bristow-Johnson 's Cookbook formulae for audio EQ biquad filter coefficients
struct  biquad_filter
{
	float b0, b1, b2, a0, a1, a2, x1, x2, y1, y2;
};

struct preprocess_state {
	// Freeing this pointer, frees all the buffers
	char *ptr_to_free ;

	SpeexEchoState *st1 ;
	SpeexEchoState *st2 ;
	SpeexEchoState *st3 ;
	SpeexPreprocessState *den ;
#ifdef WIN32
	long long pref, pe1, pe2, pe3, penlms;
#else
	long pref, pe1, pe2, pe3, penlms;
#endif
#ifdef USE_AEC_NLMS
	struct aec_nlms *nlms;
#endif

	// in/out buffers
	char *outRingBuf;
	char *inRingBuf;
	short *echo_buf, *ref_buf, *e_buf, *e1_buf, *e2_buf, *e3_buf, *e_nlms;
	float *noise1, *noise2, *noise3;

	float /* *e_buf, */ *noise;
	RingBuffer outRing;
	RingBuffer inRing;

	//bias
	long in_bias, out_bias;

	//bass_boost filter
	struct biquad_filter out_bass_boost;
};

static int compute_biquad_filter(struct biquad_filter *f, short *buffer, int n)
{
   /* initialise the filter */
   float out, in = 0;
   int i;

   for (i = 0; i < n; i++) {
      in = buffer[i];
      out = (f->b0 * in + f->b1 * f->x1 + f->b2 * f->x2 - f->a1 * f->y1 - f->a2 * f->y2) / f->a0;
      f->x2 = f->x1;
      f->x1 = in;
      f->y2 = f->y1;
      f->y1 = out;

      if (out < -32767)
         out = (float)-32767;
      else if (out > 32768)
         out = (float)32768;        //Prevents clipping
      buffer[i] = out ;
   }
   return 0;
}

static int init_lowpass(struct biquad_filter *f,float frequency, float sample_rate)
{
	float omega, sn, cs, alpha;

	/* Compute parameters */
	omega = 2 * 3.141592653589 * frequency / (sample_rate);
	sn = sin(omega);
	cs = cos(omega);
	alpha = sn / 2;

	/*  Init coefficients */
	f->b0 = (1- cs)/2;
	f->b1 = 1-cs;
	f->b2 = (1-cs)/2;
	f->a0 = 1 + alpha;
	f->a1 = -2 * cs;
	f->a2 = 1 - alpha;

	return 0;
}

static int init_bass_boost(struct biquad_filter *f,float frequency, float dB_boost, float sample_rate)
{
	float omega, sn, cs, A, shape, beta;

	if (dB_boost == 0) {
		f->b0 = 1;
		f->b1 = f->b2 = 0;
		f->a0 = 1;
		f->a1 = f->a2 = 0;
	}
	else {
		/* Compute parameters */
		omega = 2 * 3.141592653589 * frequency / (sample_rate);
		sn = sin(omega);
		cs = cos(omega);
		A = exp(log(10.0f) * dB_boost / 40);
		shape = (float)1.0;
		beta = sqrt((A * A + 1) / shape - (pow((A - 1), 2)));

		/*  Init coefficients */

		f->b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
		f->b1 = 2 * A * ((A - 1) - (A + 1) * cs);
		f->b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
		f->a0 = ((A+ 1) + (A - 1) * cs + beta * sn);
		f->a1 = -2 * ((A - 1) + (A + 1) * cs);
		f->a2 = (A + 1) + (A - 1) * cs - beta * sn;
	}

	return 0;
}

#ifdef _MSC_VER
#define open _open
#define write _write
#define O_RDONLY _O_RDONLY 
#define O_WRONLY _O_WRONLY 
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define O_BINARY  _O_BINARY 
#endif

static void iaxc_preprocessor_reset(struct iaxc_preprocessor *c, int filters)
{
	int i;
    float f;
	struct preprocess_state *s = c->state;

    //i = 1; /* always make VAD decision */
    //speex_preprocess_ctl(s->den, SPEEX_PREPROCESS_SET_VAD, &i);
    i = (filters & IAXC_FILTER_AGC) ? 1 : 0;
    speex_preprocess_ctl(s->den, SPEEX_PREPROCESS_SET_AGC, &i);
    i = (filters & IAXC_FILTER_DENOISE) ? 1 : 0;
    speex_preprocess_ctl(s->den, SPEEX_PREPROCESS_SET_DENOISE, &i);

	if ( !(c->filters & IAXC_FILTER_ECHO) && (filters & IAXC_FILTER_ECHO) )
	{
		speex_echo_state_reset(s->st1);
		speex_echo_state_reset(s->st2);
		speex_echo_state_reset(s->st3);
	}

	if ( (c->filters & IAXC_FILTER_BASS_BOOST) != (filters & IAXC_FILTER_BASS_BOOST) )
	{
		if (filters & IAXC_FILTER_BASS_BOOST)
			init_bass_boost(&(s->out_bass_boost), 400, 6, c->bitrate);
		else
			init_bass_boost(&(s->out_bass_boost), 400, 0, c->bitrate);
		//init_lowpass(&(s->out_bass_boost), 8000, c->bitrate);
	}
	c->filters = filters;
}

static void iaxc_preprocessor_destroy(struct iaxc_preprocessor *c)
{
	if (c->state) {
		struct preprocess_state *s = c->state;
		if (s->ptr_to_free)
		{
			free(s->ptr_to_free);
			s->ptr_to_free = 0;
		}
		if (s->st1) {
			speex_echo_state_destroy(s->st1);
			s->st1 = NULL;	
		}
		if (s->st2) {
			speex_echo_state_destroy(s->st2);
			s->st2 = NULL;	
		}
		if (s->st3) {
			speex_echo_state_destroy(s->st3);
			s->st3 = NULL;	
		}
		if (s->den) {
			speex_preprocess_state_destroy(s->den);
			s->den = NULL;	
		}
	}
	free(c->state);
	free(c);
}

static void iaxc_preprocessor_preprocess(struct iaxc_preprocessor *c, short *inputBuffer, short *outputBuffer, int n, void *udata)
{
	struct preprocess_state *s = c->state;
	int i;

	if (inputBuffer!=NULL) {
		for(i = 0; i < n; i++) {
			s->in_bias +=  ((((long)inputBuffer[i]) << 15) - s->in_bias) >> 14;
			inputBuffer[i] -= (s->in_bias >> 15);
		}
		RingBuffer_Write(&(s->inRing), inputBuffer, n * sizeof(short) );
	}
	if (outputBuffer!=NULL) {
		for(i = 0; i < n; i++) {
			s->out_bias +=  ((((long)outputBuffer[i]) << 15) - s->out_bias) >> 14;
			outputBuffer[i] -= (s->out_bias >> 15);
			//if(c->filters & IAXC_FILTER_BASS_BOOST)
			//	outputBuffer[i] /= 15; 
		}

		compute_biquad_filter(&(s->out_bass_boost), outputBuffer, n);
		RingBuffer_Write(&(s->outRing), outputBuffer, n * sizeof(short) );
	}

	if (inputBuffer) {
		int inLen =RingBuffer_GetReadAvailable(&(s->inRing));
		int outLen = RingBuffer_GetReadAvailable(&(s->outRing));
		float maxgain = 1, maxgain1 = 1, maxgain2 = 1, maxgain3 = 1;

		while (inLen >= c->NN*sizeof(short) && outLen >= c->NN*sizeof(short))
		{
			RingBuffer_Read(&(s->inRing),s->ref_buf, c->NN*sizeof(short));
			RingBuffer_Read(&(s->outRing),s->echo_buf, c->NN*sizeof(short));
			if (c->filters & IAXC_FILTER_ECHO) {
				float pe1= 0, pe2 = 0, pe3 =0, pref = 0;

				speex_echo_cancel(s->st1, s->ref_buf, s->echo_buf, s->e1_buf, s->noise1);
				//speex_echo_cancel(s->st2, s->ref_buf, s->echo_buf, s->e2_buf, s->noise2);
				speex_echo_cancel(s->st3, s->ref_buf, s->echo_buf, s->e3_buf, s->noise3);

				for(i=0; i<c->NN; i++)
					pe1 += s->e1_buf[i]*s->e1_buf[i];
				//for(i=0; i<c->NN; i++)
				//	pe2 += s->e2_buf[i]*s->e2_buf[i];
				for(i=0; i<c->NN; i++)
					pe3 += s->e3_buf[i]*s->e3_buf[i];

				for(i=0; i<c->NN; i++)
					pref += s->ref_buf[i]*s->ref_buf[i];

				s->pe1 = c->power_decay * s->pe1 + (1- c->power_decay) * pe1;
				//s->pe2 = c->power_decay * s->pe2 + (1- c->power_decay) * pe2;
				s->pe3 = c->power_decay * s->pe3 + (1- c->power_decay) * pe3;
				s->pref = c->power_decay * s->pref + (1- c->power_decay) * pref;

				maxgain1 = 1 + 1/fabs(((double)s->pref-s->pe1) / s->pe1) * (sqrt(s->pe1)/c->NN) / 32768;
				maxgain2 = maxgain1;// 1 + 1/fabs(((double)s->pref-s->pe2) / s->pe2) * (sqrt(s->pe2)/c->NN) / 32768;
				maxgain3 = 1 + 1/fabs(((double)s->pref-s->pe3) / s->pe3) * (sqrt(s->pe3)/c->NN) / 32768;
				maxgain = min( maxgain1, min(maxgain2,maxgain3));

				speex_preprocess_ctl(s->den, SPEEX_PREPROCESS_SET_AGC_MAXGAIN, &maxgain);
				if (maxgain == maxgain1)
				{
					s->e_buf = s->e1_buf; s->noise = s->noise1;
				} /*else	if (maxgain == maxgain2)
				{
					s->e_buf = s->e2_buf; s->noise = s->noise2;
				} else   if (maxgain == maxgain3) */
				{
					s->e_buf = s->e3_buf; s->noise = s->noise3;
				}
				speex_preprocess(s->den, s->e_buf, s->noise);

#ifdef USE_AEC_NLMS
				s->nlms->setambientdb(s->nlms, -50 );
				s->nlms->process(s->nlms, s->echo_buf, s->ref_buf, c->NN); 
#endif
				c->callback( udata, s->e_buf, c->NN );
			}
			else {
				maxgain = 200; //this is the maximum gain hard-coded in speex

				speex_preprocess_ctl(s->den, SPEEX_PREPROCESS_SET_AGC_MAXGAIN, &maxgain);
				speex_preprocess(s->den, s->ref_buf, NULL);

				c->callback( udata, s->ref_buf, c->NN );
			}

			inLen =RingBuffer_GetReadAvailable(&(s->inRing));
			outLen = RingBuffer_GetReadAvailable(&(s->outRing));
		}
	}
}

struct iaxc_preprocessor *iaxc_preprocessor_initialize(int NN, int delay, int echo_tail, int bitrate, float power_decay, void (*callback) ( void *udata, short *inputBuffer, int n ) )
{
	struct iaxc_preprocessor *c = calloc(sizeof(struct iaxc_preprocessor),1);
	struct preprocess_state *s = NULL;
	int ring_buffer_size = 1;
	char *tmpptr;

	if(!c) return NULL;

	// init base struct
	c->destroy      = iaxc_preprocessor_destroy;
	c->reset        = iaxc_preprocessor_reset;
	c->preprocess   = iaxc_preprocessor_preprocess;
	c->callback     = callback;
	c->NN      = NN;
	c->delay   = delay;
	c->echo_tail    = echo_tail;
	c->bitrate = bitrate;
	c->power_decay = power_decay;

	s = calloc(sizeof(struct preprocess_state),1);
	if(!s) {	
		c->destroy(c);
		return NULL;
	}
	c->state = s;

	//create memory for all the buffer with just one call
	while(ring_buffer_size < 4*delay)
		ring_buffer_size *=2;

	s->ptr_to_free = calloc( 
		2 * ring_buffer_size * sizeof(short)+ // inRingBuf && outRingBuf
		5 * NN * sizeof(short)+ // echo_buf,ref_buf, e_buf, e2_buf, e3_buf
		NN * sizeof(short)+     // e_nlms
		3 * (NN+1)* sizeof(float) //noise1, noise2, noise3
		, 1);
	if (s->ptr_to_free==NULL) {
		c->destroy(c);
		return NULL;
	}

	// Init buffers
	tmpptr = s->ptr_to_free ;
	s->outRingBuf = tmpptr;
	tmpptr +=ring_buffer_size*sizeof(short);

	s->inRingBuf  = tmpptr;
	tmpptr +=ring_buffer_size*sizeof(short);

	s->echo_buf = (short *)tmpptr;
	tmpptr +=(NN)*sizeof(short);

	s->ref_buf = (short *)tmpptr;
	tmpptr +=(NN)*sizeof(short);

	s->e1_buf = (short *)tmpptr;
	tmpptr +=(NN)*sizeof(short);

	s->e2_buf = (short *)tmpptr;
	tmpptr +=(NN)*sizeof(short);

	s->e3_buf = (short *)tmpptr;
	tmpptr +=(NN)*sizeof(short);

	s->e_nlms = (short *)tmpptr;
	tmpptr +=(NN)*sizeof(short);

	s->noise1 = (float *)tmpptr;
	tmpptr +=(NN+1)*sizeof(float);

	s->noise2 = (float *)tmpptr;
	tmpptr +=(NN+1)*sizeof(float);

	s->noise3 = (float *)tmpptr;
	tmpptr +=(NN+1)*sizeof(float);

	RingBuffer_Init(&(s->outRing), ring_buffer_size, s->outRingBuf);
	memset(s->inRingBuf, 0, (delay) * sizeof(short) );
	RingBuffer_Write(&(s->outRing), s->inRingBuf, (delay) * sizeof(short) );

	RingBuffer_Init(&(s->inRing), ring_buffer_size, (s->inRingBuf));

	// Init speex structs
	s->st1 = speex_echo_state_init(NN, echo_tail);
	s->st2 = speex_echo_state_init(NN, echo_tail * 3);
	s->st3 = speex_echo_state_init(NN, echo_tail * 9);
#ifdef USE_AEC_NLMS
	s->nlms = aec_nlms_init();
#endif

	s->den = speex_preprocess_state_init(NN, bitrate);
	if (!s->st1 || !s->st2 || !s->st3 || !s->den)
	{
		c->destroy(c);
		return NULL;
	}

	// initing bass boost struture to off
	init_bass_boost(&(s->out_bass_boost),200,0,bitrate);

	return c;
}

/*
#include "fcntl.h"

#ifdef _MSC_VER
#define open _open
#define write _write
#define O_RDONLY _O_RDONLY 
#define O_WRONLY _O_WRONLY 
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#define O_BINARY  _O_BINARY 
#define longlong __int64
#endif

#define NN     160
#define DELAY  (NN*12/2)

int iaxc_echo_can_reset = 0;
static void iaxc_echo_can (short *inputBuffer, short *outputBuffer, int n, RingBuffer *resultBuffer)
{
	static RingBuffer outRing;
	static char outRingBuf[EC_RING_SZ];
	static RingBuffer inRing;
	static char inRingBuf[EC_RING_SZ];

	FILE *echo_fd, *ref_fd, *e_fd, *net_fd;
	float noise[NN+1];
	short echo_buf[NN], ref_buf[NN], e_buf[NN], net_buf[NN];
	static SpeexEchoState *st = NULL;
	static SpeexPreprocessState *den = NULL;
	static SpeexPreprocessState *den2 = NULL;
	static longlong pe=0, pref=0;
	int i;

	if (iaxc_echo_can_reset == TRUE)
	{
		if (st!=NULL) {
			speex_echo_state_destroy(st);
			st = NULL;
		}
		if (den!=NULL) {
			speex_preprocess_state_destroy(den);
			den = NULL;
		}
		iaxc_echo_can_reset = FALSE;
	}

	if (st == NULL ) {
		st = speex_echo_state_init(NN, 6 * NN);
		RingBuffer_Init(&outRing, EC_RING_SZ, &outRingBuf);
		// use zero-ed inRingBuf to put some data on the outBuffer
		memset(inRingBuf, 0, (DELAY) * sizeof(short) );
		RingBuffer_Write(&outRing, inRingBuf, (DELAY) * sizeof(short) );
		RingBuffer_Init(&inRing, EC_RING_SZ, &inRingBuf);
		
		remove("speaker_last.sw");
		remove("mic_last.sw");
		remove("cancelled_last.sw");
		remove("net_last.sw");
		rename("mic.sw","mic_last.sw");
		rename("speaker.sw","speaker_last.sw");
		rename("cancelled.sw","cancelled_last.sw");
		rename("net.sw","net_last.sw");
	}

	if (den == NULL ) {
		den = speex_preprocess_state_init(NN, 8000);
		i = 1;
		speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_DENOISE, &i);
		i = 1;
		speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_AGC, &i);
	}

	if (inputBuffer!=NULL)
		RingBuffer_Write(&inRing, inputBuffer, n * sizeof(short) );

	if (outputBuffer!=NULL)
		RingBuffer_Write(&outRing, outputBuffer, n * sizeof(short) );

	if (inputBuffer) {
		int inLen =RingBuffer_GetReadAvailable(&inRing);
		int outLen = RingBuffer_GetReadAvailable(&outRing);
		static int count = 1;
		static int diff = 0 ;
		static int meanIn = 0 ;
		static int meanOut = 0 ;
		
		while (inLen >= NN*sizeof(short) && outLen >= NN*sizeof(short))
		{
			ref_fd	= fopen ("mic.sw",  "ab");
			echo_fd	= fopen ("speaker.sw",  "ab" );
			e_fd	= fopen ("cancelled.sw", "ab" );
			net_fd	= fopen ("net.sw", "ab" );

			RingBuffer_Read(&inRing,ref_buf, NN*sizeof(short));
			//memcpy(ref_buf, tmp_buf, NN*sizeof(short));
			//memcpy(tmp_buf, echo_buf, NN*sizeof(short));
			RingBuffer_Read(&outRing,echo_buf, NN*sizeof(short));

			//speex_preprocess(den2, ref_buf, NULL);
			speex_echo_cancel(st, ref_buf, echo_buf, e_buf, noise);
			//speex_preprocess(den, e_buf, noise);
			{
				float maxgain = 1;

				pe /= 1.1;
				pref /= 1.1;

				for(i=0; i<NN; i++)
					pe += e_buf[i]*e_buf[i];
				for(i=0; i<NN; i++)
					pref += ref_buf[i]*ref_buf[i];

				maxgain = 1 + 2 * 1/fabs(((double)pref-pe) / pe) * (sqrt(pe)/NN) / 32768;

				speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_AGC_MAXGAIN, &maxgain);
				/*if ( (sqrt(pe)/NN)<(32768/512) || (  ((double)pref-pe) / pe) >= 0.5 )
				{				
					
				}
				else
				{
					i = 1;
					speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_AGC, &i);
				}*/
	/*			for (i=0; i<NN; i++)
					net_buf[i] = (sqrt(pref)-sqrt(pe))/sqrt(pe)*4096;*//*

				meanIn += inLen / sizeof(short);
				meanOut += outLen / sizeof(short);
				diff += abs(inLen-outLen) / sizeof(short);
				if (count%100==0) {
					printf("> samples: diff %f (in: %f out:%f) pe: %f pref: %f pdiff: %f pratio:%f\n", diff / 100.0, meanIn / 100.0, meanOut / 100.0, (double)pe, (double)pref, (double)(pref-pe), ((double)(pref-pe)/(pe+1)) );
					diff = 0;
					meanIn = 0;
					meanOut = 0;
				}
				count ++;
			}
			speex_preprocess(den, e_buf, noise);


			fwrite(echo_buf, NN*sizeof(short),1,echo_fd);
			fwrite(ref_buf, NN*sizeof(short),1,ref_fd);
			fwrite(e_buf, NN*sizeof(short),1,e_fd);
			fwrite(net_buf, NN*sizeof(short),1,net_fd);

			RingBuffer_Write(resultBuffer, e_buf, NN * sizeof(short) );

			inLen =RingBuffer_GetReadAvailable(&inRing);
			outLen = RingBuffer_GetReadAvailable(&outRing);

			fclose(net_fd);
			fclose(e_fd);
			fclose(echo_fd);
			fclose(ref_fd);
		}
	}
}
*/
