/*
    init/start/stop/exit stream functions
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

    Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* License: GPL
 * Author: Kevin Thayer <nufan_wfk at yahoo dot com>
 *
 * This file will hold API related functions, both internal (firmware api)
 * and external (v4l2, etc)
 *
 * -----
 * MPG600/MPG160 support by  T.Adachi <tadachi@tadachi-net.com>
 *                      and Takeru KOMORIYA<komoriya@paken.org>
 *
 * AVerMedia M179 GPIO info by Chris Pinkham <cpinkham@bc2va.org>
 *                using information provided by Jiun-Kuei Jung @ AVerMedia.
 */

#include "ivtv-driver.h"
#include "ivtv-dma.h"
#include "ivtv-reset.h"
#include "ivtv-fileops.h"
#include "ivtv-i2c.h"
#include "ivtv-reset.h"
#include "ivtv-queue.h"
#include "ivtv-mailbox.h"
#include "ivtv-audio.h"
#include "ivtv-video.h"
#include "ivtv-kthreads.h"
#include "ivtv-vbi.h"
#include "ivtv-ioctl.h"
#include "ivtv-irq.h"
#include "ivtv-streams.h"
#include "ivtv-cards.h"
#include "audiochip.h"
#include <linux/smp_lock.h>

#define IVTV_V4L2_MAX_MINOR 15

static struct file_operations ivtv_v4l2_enc_fops = {
	owner:		THIS_MODULE,
	read:           ivtv_v4l2_read,
	write:          ivtv_v4l2_write,
	open:           ivtv_v4l2_open,
	ioctl:          ivtv_v4l2_ioctl,
	release:        ivtv_v4l2_close,
	poll:		ivtv_v4l2_enc_poll,
};

static struct file_operations ivtv_v4l2_dec_fops = {
	owner:		THIS_MODULE,
	read:           ivtv_v4l2_read,
	write:          ivtv_v4l2_write,
	open:           ivtv_v4l2_open,
	ioctl:          ivtv_v4l2_ioctl,
	release:        ivtv_v4l2_close,
	poll:		ivtv_v4l2_dec_poll,
};

static struct video_device ivtv_v4l2dev_template = {
        .owner  = THIS_MODULE,
        .name	= "Vanilla iTVC15 card",
        .type	= VID_TYPE_CAPTURE | VID_TYPE_TUNER | VID_TYPE_TELETEXT |
                  VID_TYPE_CLIPPING | VID_TYPE_SCALES | VID_TYPE_MPEG_ENCODER,
        .fops	= &ivtv_v4l2_enc_fops,
        .minor  = -1,
};

static int ivtv_stream_init(struct ivtv *itv, int streamtype, int buffers, int bufsize,
			    int minor, int reg_type, int dma)
{
	struct ivtv_stream *s = &itv->streams[streamtype];

        /* Translate streamtype to buffers limit */
        if (streamtype == IVTV_ENC_STREAM_TYPE_MPG) {
                s->buf_min = IVTV_DEFAULT_MPG_BUFFERS;
                s->buf_max = IVTV_MAX_MPG_BUFFERS;
        } else if (streamtype == IVTV_ENC_STREAM_TYPE_YUV) {
                s->buf_min = IVTV_DEFAULT_YUV_BUFFERS;
                s->buf_max = IVTV_MAX_YUV_BUFFERS;
        } else if (streamtype == IVTV_ENC_STREAM_TYPE_VBI) {
                s->buf_min = IVTV_DEFAULT_VBI_BUFFERS;
                s->buf_max = IVTV_MAX_VBI_BUFFERS;
        } else if (streamtype == IVTV_ENC_STREAM_TYPE_PCM) {
                s->buf_min = IVTV_DEFAULT_PCM_BUFFERS;
                s->buf_max = IVTV_MAX_PCM_BUFFERS;
        } else if (streamtype == IVTV_ENC_STREAM_TYPE_RAD) {
                s->buf_min = IVTV_DEFAULT_PCM_BUFFERS;
                s->buf_max = IVTV_MAX_PCM_BUFFERS;
        } else if (streamtype == IVTV_DEC_STREAM_TYPE_MPG) {
                s->buf_min = IVTV_DEFAULT_DEC_MPG_BUFFERS;
                s->buf_max = IVTV_MAX_DEC_MPG_BUFFERS;
        } else if (streamtype == IVTV_DEC_STREAM_TYPE_VBI) {
                s->buf_min = IVTV_DEFAULT_DEC_VBI_BUFFERS;
                s->buf_max = IVTV_MAX_DEC_VBI_BUFFERS;
        } else if (streamtype == IVTV_DEC_STREAM_TYPE_VOUT) {
                s->buf_min = IVTV_DEFAULT_DEC_VBI_BUFFERS;
                s->buf_max = IVTV_MAX_DEC_VBI_BUFFERS;
        } else if (streamtype == IVTV_DEC_STREAM_TYPE_YUV) {
                s->buf_min = IVTV_DEFAULT_DEC_YUV_BUFFERS;
                s->buf_max = IVTV_MAX_DEC_YUV_BUFFERS;
        } else {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "Stream Init: Unknown stream: %d\n", streamtype);
                return -EIO;
        }

	/* Make it easier to know what type it is */
	s->type = streamtype;

	minor += ivtv_first_minor;  /* skip any other TV cards */
	if (buffers) {
		IVTV_KERN_INFO("Create %sstream %d using %d %d byte buffers "
				" %d kbytes total\n",
	       		dma != PCI_DMA_NONE ? "DMA " : "", 
			streamtype, (buffers/bufsize), bufsize, s->buf_min);
	} else {
		IVTV_KERN_INFO("Create %sstream %d \n", 
			dma != PCI_DMA_NONE ? "DMA " : "", 
			streamtype);
	}

	/* Step 1: allocate and initialize the v4l2 video device structure */
	s->v4l2dev = video_device_alloc();
	if (NULL == s->v4l2dev) {
		IVTV_KERN_ERR("Couldn't allocate v4l2 video_device\n");
		return -ENOMEM;
	}
	memcpy(s->v4l2dev, &ivtv_v4l2dev_template, sizeof(struct video_device));
	s->v4l2dev->priv = itv;
	s->v4l2dev->minor = minor;
#ifdef LINUX26
	s->v4l2dev->dev = &itv->dev->dev;
	s->v4l2dev->release = video_device_release;
#endif

	/* Step 2: initialize ivtv_stream fields */
	init_waitqueue_head(&s->waitq);
	init_waitqueue_head(&s->udma.waitq);
	init_MUTEX(&s->mlock);
	s->slock = SPIN_LOCK_UNLOCKED;
	s->ubytes = 0;
	s->dma = dma;
	s->id = -1;
        s->SG_handle = IVTV_DMA_UNMAPPED;
	s->SGarray = NULL;
        s->SGlist = NULL;
        s->SGlen = 0;
        s->SG_length = 0;
	s->buffers = buffers;
	s->bufsize = bufsize;
	s->buf_total = 0;
	s->buf_fill = 0;
	s->dmatype = 0;
	atomic_set(&s->allocated_buffers, 0);
	INIT_LIST_HEAD(&s->free_q.list);
	INIT_LIST_HEAD(&s->full_q.list);
	INIT_LIST_HEAD(&s->dma_q.list);
	INIT_LIST_HEAD(&s->io_q.list);

        s->udma.SGarray = NULL;
        s->udma.SGlist = NULL;
        s->udma.map = NULL;
        s->udma.SGlen = 0;
        s->udma.SG_length = 0;
        s->udma.page_count = 0;
        s->udma.offset = 0;
        s->udma.sg_size = 0;

	/* Allocate DMA SG Arrays */
	if (s->dma != PCI_DMA_NONE) {
                /* Step 1: if this stream uses DMA,
                        then allocate the scatter-gather array */
                if (s->SGarray == NULL) {
                        s->SGarray = (struct ivtv_SG_element *)
                                kmalloc(sizeof(struct ivtv_SG_element) *
                                (s->buf_max/PAGE_SIZE), GFP_KERNEL);
                        if (s->SGarray == NULL) {
                                IVTV_KERN_ERR(
                                        "Could not allocate SGarray\n");
        			video_device_release(s->v4l2dev);
				s->v4l2dev = NULL;
                                return -ENOMEM;
                        }
                        s->SG_handle = IVTV_DMA_UNMAPPED;
                        s->SG_length = 0;
                }
                /* Set SGarray to all 0's */
                memset(s->SGarray, 0,
                        sizeof(struct ivtv_SG_element) *
                        (s->buf_max/PAGE_SIZE));
        
                s->SG_handle = pci_map_single(itv->dev, s->SGarray,
                        (sizeof(struct ivtv_SG_element) * 
				(s->buf_max/PAGE_SIZE)),
                        	s->dma);
	}

        /* allocate stream buffers */
        if (s->buffers && ivtv_init_queues_nolock(itv, 
			streamtype, s->buf_min, s->bufsize) < 0) 
	{
		IVTV_KERN_ERR("Couldn't allocate buffers, "
			"streamtype %d, minor %d\n",
			streamtype, minor);

        	video_device_release(s->v4l2dev);
		s->v4l2dev = NULL;

		/* Free SG Array/Lists */	
		if (s->SGarray != NULL) {
			if (s->SG_handle != IVTV_DMA_UNMAPPED) {
                		pci_unmap_single(itv->dev, s->SG_handle,
                        		(sizeof(struct ivtv_SG_element) *
                       	 		(s->buf_max/PAGE_SIZE)), s->dma);
                		s->SG_handle = IVTV_DMA_UNMAPPED;
			}
                	s->SG_length = 0;

			kfree(s->SGarray);
		}

                return -ENOMEM;
        }

       	/* We're done if this is a 'hidden' stream (OSD) */
	if (minor < 0) return 0;

	/* Step 3: register device. First try the desired minor, 
		then any free one. */
	if (video_register_device(s->v4l2dev, reg_type, minor) &&
	    video_register_device(s->v4l2dev, reg_type, -1)) {
		IVTV_KERN_ERR("Couldn't register v4l2 device, "
			"streamtype %d, minor %d\n",
			streamtype, minor);

        	video_device_release(s->v4l2dev);
		s->v4l2dev = NULL;

		/* Free SG Array/Lists */	
		if (s->SGarray != NULL) {
			if (s->SG_handle != IVTV_DMA_UNMAPPED) {
                		pci_unmap_single(itv->dev, s->SG_handle,
                        		(sizeof(struct ivtv_SG_element) *
                       	 		(s->buf_max/PAGE_SIZE)), s->dma);
                		s->SG_handle = IVTV_DMA_UNMAPPED;
			}
                	s->SG_length = 0;

			kfree(s->SGarray);
		}

		return -ENOMEM;
	}
        IVTV_KERN_INFO("Registered v4l2 device, streamtype %d minor %d\n",
                streamtype, s->v4l2dev->minor);

	/* Success! All done. */

	return 0;
}

/* Initialize v4l2 variables and register v4l2 device */
int ivtv_streams_setup(struct ivtv *itv)
{
	int x;
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 streams setup\n");

	itv->streamcount = 4;
	if (itv->v4l2_cap & V4L2_CAP_RADIO)
		itv->streamcount++;

	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
		itv->streamcount += 4;
		ivtv_v4l2dev_template.type |= VID_TYPE_MPEG_DECODER;
        }

        /* fill in appropriate v4l2 device */
        IVTV_KERN_INFO("Configuring %s card with %d streams\n",
                  itv->card->name, itv->streamcount);
        
	/* Allocate streams */
        itv->streams = (struct ivtv_stream *)
                        kmalloc(itv->streamcount * sizeof(struct ivtv_stream),
                                GFP_KERNEL);
        if (NULL == itv->streams) {
                IVTV_KERN_ERR("Couldn't allocate v4l2 streams\n");
                return -ENOMEM;
        }
	memset(itv->streams, 0, itv->streamcount * sizeof(struct ivtv_stream));

	x = 0;

	x |= ivtv_stream_init(itv, IVTV_ENC_STREAM_TYPE_MPG,
		         itv->options.mpg_buffers, itv->dma_cfg.enc_buf_size,
		       	 itv->num, VFL_TYPE_GRABBER, PCI_DMA_FROMDEVICE);

	x |= ivtv_stream_init(itv, IVTV_ENC_STREAM_TYPE_YUV,
		         itv->options.yuv_buffers, itv->dma_cfg.enc_yuv_buf_size,
		       	 itv->num + IVTV_V4L2_ENC_YUV_OFFSET, VFL_TYPE_GRABBER,
			 PCI_DMA_FROMDEVICE);

	x |= ivtv_stream_init(itv, IVTV_ENC_STREAM_TYPE_VBI,
		         itv->options.vbi_buffers, itv->dma_cfg.vbi_buf_size,
		       	 itv->num, VFL_TYPE_VBI, PCI_DMA_NONE);

	x |= ivtv_stream_init(itv, IVTV_ENC_STREAM_TYPE_PCM,
		         itv->options.pcm_buffers, itv->dma_cfg.enc_pcm_buf_size,
		       	 itv->num + IVTV_V4L2_ENC_PCM_OFFSET, VFL_TYPE_GRABBER,
			 PCI_DMA_FROMDEVICE);

	if (itv->v4l2_cap & V4L2_CAP_RADIO) {
		x |= ivtv_stream_init(itv, IVTV_ENC_STREAM_TYPE_RAD, 0, 0,
		       	 itv->num, VFL_TYPE_RADIO, PCI_DMA_NONE);
	}

	/* Set some card-specific per-stream stuff here */
	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
		x |= ivtv_stream_init(itv, IVTV_DEC_STREAM_TYPE_MPG,
			 itv->options.dec_mpg_buffers, 
			 itv->dma_cfg.dec_buf_size,
		       	 itv->num + IVTV_V4L2_DEC_MPG_OFFSET, VFL_TYPE_GRABBER,
			 PCI_DMA_TODEVICE);
		if (x == 0)
			itv->streams[IVTV_DEC_STREAM_TYPE_MPG].v4l2dev->fops =
			       				&ivtv_v4l2_dec_fops;

		x |= ivtv_stream_init(itv, IVTV_DEC_STREAM_TYPE_VBI,
			 itv->options.vbi_buffers, 
			 itv->dma_cfg.dec_vbi_buf_size,
		       	 itv->num + IVTV_V4L2_DEC_VBI_OFFSET, 
				VFL_TYPE_VBI, PCI_DMA_NONE);

		x |= ivtv_stream_init(itv, IVTV_DEC_STREAM_TYPE_VOUT, 0, 0,
		       	 itv->num + IVTV_V4L2_DEC_VOUT_OFFSET, 
				VFL_TYPE_VBI, PCI_DMA_NONE);
		if (x == 0)
			itv->streams[IVTV_DEC_STREAM_TYPE_VOUT].v4l2dev->fops = 
							&ivtv_v4l2_dec_fops;

		x |= ivtv_stream_init(itv, IVTV_DEC_STREAM_TYPE_YUV,
			itv->options.dec_yuv_buffers, 
			itv->dma_cfg.dec_yuv_buf_size,
			itv->num + IVTV_V4L2_DEC_YUV_OFFSET,
			VFL_TYPE_GRABBER, PCI_DMA_TODEVICE);
		if (x == 0)
			itv->streams[IVTV_DEC_STREAM_TYPE_YUV].v4l2dev->fops =
							&ivtv_v4l2_dec_fops;
	}

	if (x == 0) {
		atomic_set(&itv->streams_setup, 1);
		return 0;
	}

	/* One or more streams could not be initialized. Clean 'em all up. */
	ivtv_streams_cleanup(itv);
	return -ENOMEM;
}

void ivtv_streams_cleanup(struct ivtv *itv)
{
	int x;

        for (x = 0; x < itv->streamcount; x++) {
		struct ivtv_stream *s = &itv->streams[x];

		/* Free Buffers */
		if (atomic_read(&s->allocated_buffers)) {
			ivtv_free_queues_lock(itv, x);
		}

		/* Free User SG Array/Lists */	
		if (s->udma.map != NULL) {
			kfree(s->udma.map);
		}

		/* Free User SG Array/Lists */	
		if (s->udma.SGlist != NULL) {
			kfree(s->udma.SGlist);
		}

		/* Free User SG Array/Lists */	
		if (s->udma.SGarray != NULL) {
			kfree(s->udma.SGarray);
		}

		/* Free SG Array/Lists */	
		if (s->SGarray != NULL) {
			if (s->SG_handle != IVTV_DMA_UNMAPPED) {
                		pci_unmap_single(itv->dev, s->SG_handle,
                        		(sizeof(struct ivtv_SG_element) *
                       	 		(s->buf_max/PAGE_SIZE)), s->dma);
                		s->SG_handle = IVTV_DMA_UNMAPPED;
			}
                	s->SG_length = 0;

			kfree(s->SGarray);
		}

                if (s->v4l2dev == NULL) 
			continue;

		/* Free Device */
		if (s->v4l2dev->minor == -1) {
			// 'Hidden' never registered stream (OSD)
			video_device_release(s->v4l2dev);
		}
		else {
			// All others, just unregister.
			video_unregister_device(s->v4l2dev);
		}
	}
	kfree(itv->streams);
	itv->streams = NULL;

	atomic_set(&itv->streams_setup, 0);
}

void ivtv_setup_v4l2_encode_stream(struct ivtv *itv, int type) 
{
	/* streams_lock must be held */
	IVTV_ASSERT(ivtv_sem_count(&itv->streams[type].mlock) <= 0);

        /* assign stream type */
        ivtv_vapi(itv, IVTV_API_ASSIGN_STREAM_TYPE, 1, itv->codec.stream_type);

        /* assign output port */
        ivtv_vapi(itv, IVTV_API_ASSIGN_OUTPUT_PORT, 1, 0); /* 0 = Memory */

        /* assign framerate */
        ivtv_vapi(itv, IVTV_API_ASSIGN_FRAMERATE, 1, (itv->std & V4L2_STD_625_50) ? 1 : 0);

        /* assign frame size */
        ivtv_vapi(itv, IVTV_API_ASSIGN_FRAME_SIZE, 2, itv->height, itv->width);

        /* assign aspect ratio */
        ivtv_vapi(itv, IVTV_API_ASSIGN_ASPECT_RATIO, 1, itv->codec.aspect);

	/* automatically calculate bitrate on the fly */
	if (itv->codec.bitrate > 15000000) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"Bitrate too high, adjusted 15mbit, see mpeg specs\n");
		itv->codec.bitrate = 15000000; 
	}

        /* assign bitrates */
        ivtv_vapi(itv, IVTV_API_ASSIGN_BITRATES, 5,
		itv->codec.bitrate_mode,         /* mode */
		itv->codec.bitrate,              /* bps */
		itv->codec.bitrate_peak / 400,   /* peak/400 */
		0, 0x70);                        /* encoding buffer, ckennedy */

        /* assign gop properties */
        ivtv_vapi(itv, IVTV_API_ASSIGN_GOP_PROPERTIES, 2,
        	itv->codec.framespergop, itv->codec.bframes);

        /* assign 3 2 pulldown */
        ivtv_vapi(itv, IVTV_API_ASSIGN_3_2_PULLDOWN, 1, itv->codec.pulldown);

        /* assign gop closure */
        ivtv_vapi(itv, IVTV_API_ASSIGN_GOP_CLOSURE, 1, itv->codec.gop_closure);

        /* assign audio properties */
        ivtv_vapi(itv, IVTV_API_ASSIGN_AUDIO_PROPERTIES, 1, itv->codec.audio_bitmask);
	ivtv_dualwatch_start_encoding(itv);

        /* assign dnr filter mode */
        ivtv_vapi(itv, IVTV_API_ASSIGN_DNR_FILTER_MODE, 2,
		       	itv->codec.dnr_mode, itv->codec.dnr_type);

        /* assign dnr filter props*/
        ivtv_vapi(itv, IVTV_API_ASSIGN_DNR_FILTER_PROPS, 2,
			itv->codec.dnr_spatial, itv->codec.dnr_temporal);

        /* assign coring levels (luma_h, luma_l, chroma_h, chroma_l) */
        ivtv_vapi(itv, IVTV_API_ASSIGN_CORING_LEVELS, 4, 0, 255, 0, 255);

	/* assign spatial filter type: luma_t: 1 = horiz_only, chroma_t: 1 = horiz_only */
        ivtv_vapi(itv, IVTV_API_ASSIGN_SPATIAL_FILTER_TYPE, 2, 1, 1);

        /* assign frame drop rate */
        ivtv_vapi(itv, IVTV_API_ASSIGN_FRAME_DROP_RATE, 1, 0);
}

int ivtv_passthrough_mode(struct ivtv *itv, int enable)
{ 
        int x;
        struct ivtv_stream *yuv_stream =
                &itv->streams[IVTV_ENC_STREAM_TYPE_YUV];
        struct ivtv_stream *dec_stream =
                &itv->streams[IVTV_DEC_STREAM_TYPE_YUV];

        IVTV_DEBUG(IVTV_DEBUG_INFO,"ivtv ioctl: Select passthrough mode\n");

        /* Prevent others from starting/stopping streams while we
           initiate/terminate passthrough mode */
        if (enable) {
                if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) {
                        return 0;
                }
                if (atomic_read(&itv->decoding) > 0) {
                        return -EBUSY;
                }

                down(&dec_stream->mlock);
                set_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags);

                /* Fully initialize stream, and then unflag init */
                set_bit(IVTV_F_S_NO_DMA, &dec_stream->s_flags);
                set_bit(IVTV_F_S_DECODING, &dec_stream->s_flags);

		/* Setup YUV Decoder */
        	ivtv_setup_v4l2_decode_stream(itv, IVTV_DEC_STREAM_TYPE_YUV);

		/* Start Decoder */
               	ivtv_vapi(itv, 
			IVTV_API_DEC_START_PLAYBACK, 2, 0, 1);
               	atomic_inc(&itv->decoding);
               	ivtv_start_vbi_timer(itv);

                /* Setup capture if not already done */
                if (atomic_read(&itv->capturing) == 0) {
                        down(&yuv_stream->mlock);
                        ivtv_setup_v4l2_encode_stream(itv,
                                IVTV_ENC_STREAM_TYPE_YUV);
                        up(&yuv_stream->mlock);
                }

                /* Start Passthrough Mode */
                x = ivtv_vapi(itv, IVTV_API_BEGIN_CAPTURE, 2, 2, 11);

                if (x) IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "passthrough start capture error. Code %d\n",x);
                up(&dec_stream->mlock);

                return 0;
        }

        if (!test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) {
                return 0;
        }

        /* Stop Passthrough Mode */
        down(&dec_stream->mlock);
        x = ivtv_vapi(itv, IVTV_API_END_CAPTURE, 3, 1, 1, 11);
        x = ivtv_vapi(itv, IVTV_API_DEC_STOP_PLAYBACK, 3, 1, 0, 0);
        if (x) IVTV_DEBUG(IVTV_DEBUG_ERR,
                "passthrough stop decode error. Code %d\n",x);

        atomic_dec(&itv->decoding);
        clear_bit(IVTV_F_S_NO_DMA, &dec_stream->s_flags);
        clear_bit(IVTV_F_S_DECODING, &dec_stream->s_flags);

        clear_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags);
        up(&dec_stream->mlock);

        return 0;
}

static void ivtv_vbi_setup(struct ivtv *itv)
{
	int raw = itv->vbi_service_set_in == 0;
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int lines;
	int x;
	int i;
        int h = (itv->std & V4L2_STD_NTSC) ? 480 : 576;

        if (itv->vbi_service_set_in == 0 && itv->height != h)
        {
            int vbi_off = 1 << 31;

            // VBI combined with scaling doesn't work.
	    ivtv_saa7115(itv, DECODER_SET_VBI, &vbi_off);
	    for (i = 2; i <= 24; i++) {
		ivtv_vapi(itv, IVTV_API_SELECT_VBI_LINE, 5, i - 1, 0, 0, 0, 0);
		ivtv_vapi(itv, IVTV_API_SELECT_VBI_LINE, 5, (i - 1) | (1 << 31), 0, 0, 0, 0);
            }
            return;
        }
	
	// setup VBI registers
	ivtv_saa7115(itv, DECODER_SET_VBI, &itv->vbi_service_set_in);

	itv->vbi_search_ba = 0;

	// determine number of lines and total number of VBI bytes.
	// A raw line takes 1443 bytes: 2 * 720 + 4 byte frame header - 1
	// The '- 1' byte is probably an unused U or V byte. Or something...
	// A sliced line takes 51 bytes: 4 byte frame header, 4 byte internal
	// header, 42 data bytes + checksum (to be confirmed) 
	if (raw) {
		lines = itv->vbi_count * 2;
	}
	else {
		lines = ivtv_calc_sliced_lines(itv->vbi_service_set_in);
	}
	itv->vbi_enc_size = 4 * ((lines * (raw ? VBI_RAW_SIZE : VBI_SLICED_SIZE) + 3) / 4);

	// Note: sliced vs raw flag doesn't seem to have any effect
	data[0] = raw | (7 << 1) | (0xbd << 8);
	// Every X number of frames a VBI interrupt arrives (frames as in 25 or 30 fps)
	data[1] = 1;
	// The VBI frames are stored in a ringbuffer with this size (with a VBI frame as unit)
	data[2] = 1; 
	// The start/stop codes determine which VBI lines end up in the raw VBI data area.
	// The codes are from table 24 in the saa7115 datasheet. Each raw/sliced/video line
	// is framed with codes FF0000XX where XX is the SAV/EAV (Start/End of Active Video)
	// code. These values for raw VBI are obtained from a driver disassembly. The sliced
	// start/stop codes was deduced from this, but they do not appear in the driver.
	// Other code pairs that I found are: 0x250E6249/0x13545454 and 0x25256262/0x38137F54.
	// However, I have no idea what these values are for.
	if (raw) {
		data[3] = 0x25256262;
		data[4] = 0x387F7F7F;
	}
	else {
		data[3] = 0xABABECEC;
		data[4] = 0xB6F1F1F1;
	}
	// Lines per frame
	data[5] = lines; 
	// bytes per line
	data[6] = itv->vbi_enc_size / lines;
	x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_CONFIG_VBI,
			&result, 7, &data[0]);
	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "init error 21. Code %d\n",x);

	// returns the VBI encoder memory area.
	itv->vbi_enc_start = data[2];
	itv->vbi_total_frames = data[1];
	itv->vbi_index = 0;

	// select VBI lines.
	// Note that the sliced argument seems to have no effect.
	for (i = 2; i <= 24; i++) {
		int f1 = ivtv_used_line(itv, i, 0);
		int f2 = ivtv_used_line(itv, i, 1);

		ivtv_vapi(itv, IVTV_API_SELECT_VBI_LINE, 5, i - 1, f1, !raw && f1, 0, 0);
		ivtv_vapi(itv, IVTV_API_SELECT_VBI_LINE, 5, (i - 1) | (1 << 31),
			       	f2, !raw && f2, 0, 0);
	}
		
	// Remaining VBI questions:
	// - Is it possible to select particular VBI lines only for inclusion in the MPEG
	// stream? Currently you can only get the first X lines.
	// - Is mixed raw and sliced VBI possible? 
	// - What's the meaning of the raw/sliced flag?
	// - What's the meaning of params 2, 3 & 4 of the Select VBI command?
}

int ivtv_start_v4l2_encode_stream (struct ivtv *itv, int type) 
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x;
	int captype = 0, subtype = 0;
	unsigned int dig;
	struct ivtv_stream *st = &itv->streams[type];
	int total_trys, start_trys = 1000; /* 1 second */
	int retrys = 0;
	int enable_passthrough = 0;

	/* Lock */
	down(&st->mlock); 

	total_trys = start_trys;

	IVTV_DEBUG(IVTV_DEBUG_INFO,"ivtv start v4l2 stream\n");

	captype = type;

	switch (type) {
		case IVTV_ENC_STREAM_TYPE_MPG:
			captype = 0;
			subtype = 3;

			/* Stop Passthrough */
			if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) {
				ivtv_passthrough_mode(itv, 0);
				enable_passthrough = 1;
			}
			break;
	
		case IVTV_ENC_STREAM_TYPE_YUV:
			captype = 1;
			if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) {
				captype = 2;
				subtype = 11; /* video+audio+decoder */
				break;
			}
			subtype = 1;
			break;
		case IVTV_ENC_STREAM_TYPE_PCM:
			captype = 1;
			subtype = 2;
			break;
		case IVTV_ENC_STREAM_TYPE_VBI:
			captype = 1;
			subtype = 4;
			break;
		default:
			break;
	}
	st->subtype = subtype;

	/*assign dma block len*/
	data[0] = itv->dma_cfg.fw_enc_dma_xfer; /* 2^9(512), 2^8(256), 2^7(128) 
				bytes ((2^9)1024) or num frames */
			  /* 131072, 262144, 524288. 0 defaults to 131072 */	
	data[1] = itv->dma_cfg.fw_enc_dma_type; /* 0=bytes, 1=frames */
        x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem,
		       IVTV_API_ASSIGN_DMA_BLOCKLEN, &result, 2, &data[0]);
	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "init error 1. Code %d\n",x);
		
	/* assign digitizer : 0x00EF = saa7114 0x00FO = saa7115 0x0106 = mic */
        ivtv_vapi(itv, IVTV_API_ASSIGN_NUM_VSYNC_LINES, 
		2, itv->digitizer, itv->digitizer);
		
	/* assign program index info. Mask 0: Disable, Num_req: 400 max */
	/* FIXME we have no clue what format the index data is stored in */
	data[0] = itv->idx_sdf_mask;
	data[1] = itv->idx_sdf_num;
        ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_ASSIGN_PGM_INDEX_INFO,
                     &result, 2, &data[0]);
	itv->idx_sdf_offset = data[0];
	itv->idx_sdf_num = data[1];

	IVTV_DEBUG(IVTV_DEBUG_INFO, "ENC: PGM Index at 0x%08x with 0x%08x elements\n",
		itv->idx_sdf_offset, itv->idx_sdf_num);
		
	/* Setup API for Stream */
	ivtv_setup_v4l2_encode_stream(itv, type);

        /* assign placeholder */
        ivtv_vapi(itv, IVTV_API_ASSIGN_PLACEHOLDER, 
		12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	/* mute/unmute video */
	ivtv_vapi(itv, IVTV_API_MUTE_VIDEO, 1,
		       	test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags) ? 1 : 0);
		
	/* Clear Streamoff flags in case left from last capture */
	clear_bit(IVTV_F_S_STREAMOFF, &st->s_flags);
	clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags);
	atomic_set(&st->stolen_bufs, 0);

	if (atomic_read(&itv->capturing) == 0) {
		itv->trans_id = 0;
		itv->first_read = 1;
		atomic_set(&itv->enc_dma_stat_intr, 0);

                /* Clear all Pending Interrupts */
                ivtv_set_irq_mask(itv, IVTV_IRQ_MASK_CAPTURE);

                clear_bit(IVTV_F_I_EOS, &itv->i_flags);

		if ((itv->v4l2_cap & V4L2_CAP_VBI_CAPTURE)) {
			ivtv_vbi_setup(itv);
		}

		/* event notification (on) */
		/* type: 0 = refresh, on/off: 1 = on, 
		   intr_bit: 0x10000000 = digitizer,
		   mbox_id: -1: none */
        	ivtv_vapi(itv, IVTV_API_EVENT_NOTIFICATION, 
			4, 0, 1, 0x10000000, -1);

                /* Disable digitizer (saa7115) */
                IVTV_DEBUG(IVTV_DEBUG_INFO, "Disabling digitizer\n");
                dig=0;
                ivtv_saa7115(itv, DECODER_ENABLE_OUTPUT, &dig);

                /* initialize input (no args) */
                ivtv_vapi(itv, IVTV_API_INITIALIZE_INPUT, 0);

                /* enable digitizer (saa7115) */
                IVTV_DEBUG(IVTV_DEBUG_INFO, "Enabling digitizer\n");
                dig=1;
                ivtv_saa7115(itv, DECODER_ENABLE_OUTPUT, &dig);

                /*IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 100ms\n");*/
                ivtv_sleep_timeout(HZ/10, 0);
	}

	/* begin_capture */
	retrys = 2;
	while (retrys-- && 
		ivtv_vapi(itv, IVTV_API_BEGIN_CAPTURE, 2, captype, subtype)) 
	{
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"ENC: Error starting, retrying (%d)...\n", retrys);
	}

	/* Start Passthrough */
	if (enable_passthrough) {
		ivtv_passthrough_mode(itv, 1);
	}

	if (atomic_read(&itv->capturing) == 0) {
	        /* Clear the Capture Interrupt mask bits */
		ivtv_clear_irq_mask(itv, IVTV_IRQ_MASK_CAPTURE);
	        IVTV_DEBUG(IVTV_DEBUG_IRQ, 
			"IRQ Mask is now: 0x%08x\n",itv->irqmask);
	}

	/* you're live! sit back and await interrupts :) */
	atomic_inc(&itv->capturing);

	/* Unlock */
	up(&st->mlock); 

	return x;
}

int ivtv_setup_v4l2_decode_stream(struct ivtv *itv, int type) 
{
        u32 data[IVTV_MBOX_MAX_DATA], result;
        int x;

        IVTV_DEBUG(IVTV_DEBUG_INFO,
                   "Setting some initial decoder settings\n");

	/* streams_lock must be held */
	IVTV_ASSERT(ivtv_sem_count(&itv->streams[type].mlock) <= 0);

	/* disable VBI signals, if the MPEG stream contains VBI data,
	   then that data will be processed automatically for you. */
	ivtv_disable_vbi(itv);
	
        /* set audio mode to stereo */
        ivtv_vapi(itv, IVTV_API_DEC_SELECT_AUDIO, 2, 0, 0);

        /* set number of internal decoder buffers */
        ivtv_vapi(itv, 
		IVTV_API_DEC_DISPLAY_BUFFERS, 1, itv->dec_options.decbuffers);

        /* prebuffering*/
        data[0] = itv->dec_options.prebuffer; /* 0 = no prebuffering,
                                                 1 = enabled, see docs */
        ivtv_vapi(itv, IVTV_API_DEC_BUFFER, 1, itv->dec_options.prebuffer);

        /* extract from private packets */
	data[0] = 1;
        ivtv_api_result(itv, IVTV_API_DEC_EXTRACT_VBI, 1, data);
        itv->vbi_dec_start = data[0];
        itv->vbi_dec_size = data[1];

        /* set decoder source settings */
        /* Data type: 0 = mpeg from host,
                      1 = yuv from encoder,
                      2 = yuv_from_host */
        switch (type) {
                case IVTV_DEC_STREAM_TYPE_YUV:
			if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) {
				data[0] = 1;
				break;
			}
                        data[0] = 2;
        		IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"Setup DEC YUV Stream data[0] = %d\n", data[0]);
                        break;
                case IVTV_DEC_STREAM_TYPE_MPG:
                default:
                        data[0] = 0;
                        break;
        }
        data[1] = itv->width; /* YUV source width*/
        data[2] = itv->height;
        data[3] = itv->codec.audio_bitmask & 0xffff; /* Audio settings to use,
                                                   bitmap. see docs.*/
        x = ivtv_api(itv, itv->dec_mbox,
                     &itv->dec_msem,
                     IVTV_API_DEC_DECODE_SOURCE,
                     &result,
                     4,
                     &data[0]);
        if (x) IVTV_DEBUG(IVTV_DEBUG_ERR,
                          "COULDN'T INITIALIZE DECODER SOURCE %d\n",x);
        return 0;
}

int ivtv_start_v4l2_decode_stream(struct ivtv *itv, int type)
{
	struct ivtv_stream *stream;
	int x, retrys = 3;

	stream = &itv->streams[type];

	/* Lock Stream */
	down(&stream->mlock);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Starting v4l2_decode \n");

        ivtv_setup_v4l2_decode_stream(itv, type);

	/* set dma size */
	if (itv->dma_cfg.fw_dec_dma_xfer > 0) {
		x = ivtv_vapi(itv, 
			IVTV_API_DEC_DMA_BLOCKSIZE, 1, 
			itv->dma_cfg.fw_dec_dma_xfer);
		if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"COULDN'T INITIALIZE DMA SIZE %d\n",x);
	}

        /* Clear Streamoff */
        clear_bit(IVTV_F_S_STREAMOFF, &stream->s_flags);
	clear_bit(IVTV_F_S_OVERFLOW, &stream->s_flags);

       	/* Decoder DMA State */
       	itv->dec_dma_stat.last_addr     = 0;
       	itv->dec_dma_stat.bytes_filled  = 0;
       	itv->dec_dma_stat.bytes_needed  = 0;
       	itv->dec_dma_stat.total_xfer    = 0;
       	itv->dec_dma_stat.sg_bytes      = 0;
	itv->dec_dma_stat.vsync_count	= 0;
	itv->dec_dma_stat.ts		= jiffies;

	itv->dec_dma_stat.data[0]	= 0x0;
	itv->dec_dma_stat.data[1]	= 0x0;
	itv->dec_dma_stat.data[2]	= 0x80000000;
	itv->dec_dma_stat.data[3]	= 0x0;
       	itv->dec_dma_stat.wq_runs       = 0;
       	atomic_set(&itv->dec_dma_stat.intr, 0);

        /* Zero out decoder counters */
        writel(0, &itv->dec_mbox[IVTV_MBOX_FIELD_DISPLAYED].data[0]);
        writel(0, &itv->dec_mbox[IVTV_MBOX_FIELD_DISPLAYED].data[1]);
        writel(0, &itv->dec_mbox[IVTV_MBOX_FIELD_DISPLAYED].data[2]);
        writel(0, &itv->dec_mbox[IVTV_MBOX_FIELD_DISPLAYED].data[3]);
        writel(0, &itv->dec_mbox[9].data[0]);
        writel(0, &itv->dec_mbox[9].data[1]);
        writel(0, &itv->dec_mbox[9].data[2]);
        writel(0, &itv->dec_mbox[9].data[3]);

	/*Clear the following Interrupt mask bits for decoding */
	ivtv_clear_irq_mask(itv, IVTV_IRQ_MASK_DECODE);
 	IVTV_DEBUG(IVTV_DEBUG_IRQ, 
		"IRQ Mask is now: 0x%08x\n",itv->irqmask);

	/* start playback */
	while (retrys-- && ivtv_vapi(itv, IVTV_API_DEC_START_PLAYBACK, 2, 
		itv->dec_options.gop_offset, itv->dec_options.mute_frames)) {
		ivtv_sleep_timeout(HZ/100, 1);
	}	

	/* you're live! sit back and await interrupts :) */
	atomic_inc(&itv->decoding);

	/* Unlock */
	up(&stream->mlock);

	/* Start Work Queue */
	atomic_inc(&itv->dec_dma_stat.intr);
	dec_schedule_work(itv);

	return 0;
}

int ivtv_stop_all_captures(struct ivtv *itv) 
{
	int x;


	for (x = 0; x < itv->streamcount;x++) {
		if (test_bit(IVTV_F_S_CAPTURING, &itv->streams[x].s_flags)) {
			ivtv_stop_capture(itv, x);
		}
	}
	
	return 0;
}

int ivtv_stop_capture(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];
	int cap_type, then;
	int x;
	int stopmode;
	u32 data[IVTV_MBOX_MAX_DATA], result;

	DECLARE_WAITQUEUE(wait, current);

	/* This function assumes that you are allowed to stop the capture
	   and that we are actually capturing */

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stop Capture\n");

	if (type == IVTV_DEC_STREAM_TYPE_VBI) return 0;
	if (type == IVTV_DEC_STREAM_TYPE_VOUT) return 0;
        if (atomic_read(&itv->capturing) == 0) return 0;

        /* Lock */
	down(&st->mlock);

	switch (type) {
		case IVTV_ENC_STREAM_TYPE_YUV:
			cap_type = 1;
			break;
		case IVTV_ENC_STREAM_TYPE_PCM:
			cap_type = 1;
			break;
		case IVTV_ENC_STREAM_TYPE_VBI:
			cap_type = 1;
			break;
		case IVTV_ENC_STREAM_TYPE_MPG:
		default:
			cap_type = 0;
			break;
	}

	/* clear Overflow */
	clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags);

	ivtv_dualwatch_stop_encoding(itv); 

        /* Stop Capture Mode */
        if(type == IVTV_ENC_STREAM_TYPE_MPG && itv->end_gop == 1) {
                stopmode = 0;
        } else {
                stopmode = 1;
        }

        /* end_capture */
        x = ivtv_vapi(itv, IVTV_API_END_CAPTURE, 3,
		stopmode, /* when: 0 =  end of GOP  1 = NOW! */
		cap_type, /* type: 0 = mpeg */
		st->subtype); /* subtype: 3 = video+audio */
	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "ENC: Failed stopping capture %d\n",x);

        /* only run these if we're shutting down the last cap */
        if (atomic_read(&itv->capturing) - 1 == 0) {
                /* event notification (off)*/
                x = ivtv_vapi(itv, IVTV_API_EVENT_NOTIFICATION, 4,
			0, /* type: 0 = refresh */
			0, /* on/off: 0 = off */
			0x10000000, /* intr_bit: 0x10000000 = digitizer */
			-1); /* mbox_id: -1: none */
                if (x) IVTV_DEBUG(IVTV_DEBUG_ERR,
                       "ENC: Failed stopping event notification %d\n",x);
        }

	then = jiffies;

       	/* UnLock */
	up(&st->mlock);

	if (!test_bit(IVTV_F_S_NO_DMA, &st->s_flags)) {
                then = jiffies;
                if ((type == IVTV_ENC_STREAM_TYPE_MPG) && itv->end_gop == 1) {
                        /* only run these if we're shutting down the last cap */
                        add_wait_queue(&itv->cap_w, &wait);

                        set_current_state(TASK_INTERRUPTIBLE);

                        /* wait 2s for EOS interrupt */
                        while((!test_bit(IVTV_F_I_EOS,
                                &itv->i_flags)) &&
                                (jiffies < (then + (1 * HZ))))
                        {
                                schedule_timeout(HZ/100);
                        }
                        then = jiffies - then;

                        if (!test_bit(IVTV_F_I_EOS, &itv->i_flags)) {
                                IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        "ENC: EOS interrupt not "
                                        "received! stopping anyway.\n");
                                IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        "ENC: waited %d ms.\n",
                                        (1000/HZ)*then);
                        } else {
                                IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        "ENC: EOS took %d "
                                        "ms to occur.\n",
                                        (1000/HZ)*then);
                        }
                        set_current_state(TASK_RUNNING);
                        remove_wait_queue(&itv->cap_w, &wait);

                        if (test_bit(IVTV_F_S_DMAP, &st->s_flags)) {
                                IVTV_DEBUG(IVTV_DEBUG_INFO,
                                        "ENC: EOS DMA still Pending!!!\n");
                        }
                }

                then = jiffies;
                /* Make sure DMA is complete */
                add_wait_queue(&st->waitq, &wait);
                set_current_state(TASK_INTERRUPTIBLE);
                do {
                        /* check if DMA is pending */
                        if ((type == IVTV_ENC_STREAM_TYPE_MPG) && /* MPG Only */
				(ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) &
                               0x02) && !test_bit(IVTV_F_T_ENC_DMA_NEEDED, &itv->t_flags) &&
                                !test_bit(IVTV_F_S_DMAP, &st->s_flags))
                        {
                               /* Check for last DMA */
                               data[0] = data[1] = 0;
                                       ivtv_api(itv, itv->enc_mbox,
                                               &itv->enc_msem,
                                               0xC6, &result, 2, &data[0]);

                               if (data[0] == 1) {
                                       IVTV_DEBUG(IVTV_DEBUG_DMA, 
					"ENC: Last DMA of size 0x%08x\n",
                                        data[1]);
                                        break;
                                }
                        } else if (!test_bit(IVTV_F_S_DMAP, &st->s_flags) && /* VBI and other streams */
				(ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) &
				0x02))
			{
				break;
			}

                        ivtv_sleep_timeout(HZ/100, 1);
                } while (((then + (HZ*2)) > jiffies));

                set_current_state(TASK_RUNNING);
                remove_wait_queue(&st->waitq, &wait);

                if (test_bit(IVTV_F_S_DMAP, &st->s_flags)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "ENC: DMA still Pending while stoping capture!\n");
                }
	}

	atomic_dec(&itv->capturing);

	/* Clear capture and no-read bits */
	if (!test_bit(IVTV_F_S_STREAMOFF, &st->s_flags))
        	clear_bit(IVTV_F_S_CAPTURING, &st->s_flags);
        clear_bit(IVTV_F_S_NO_READ, &st->s_flags);

	if (atomic_read(&itv->capturing) > 0) {
		return 0;
	}

	/*Set the following Interrupt mask bits for capture */
	ivtv_set_irq_mask(itv, IVTV_IRQ_MASK_CAPTURE);
	IVTV_DEBUG(IVTV_DEBUG_IRQ, 
		"ENC: IRQ Mask is now: 0x%08x\n",itv->irqmask);

	wake_up(&st->waitq);

	return 0;
}

static void ivtv_stop_wait(struct ivtv *itv, int type) {
        DECLARE_WAITQUEUE(wait, current);
        int rc = 0;
        u32 then;

        then = jiffies;
        add_wait_queue(&itv->streams[type].waitq, &wait);
        do {
                set_current_state(TASK_INTERRUPTIBLE);

                /* check DMA status register */
                if (!test_bit(IVTV_F_S_STREAMOFF, &itv->streams[type].s_flags) ||
			!test_bit(IVTV_F_S_DMAP, &itv->streams[type].s_flags)) 
		{
                        break;
		}

		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(HZ/100);

                if (signal_pending(current))
                        rc = -EINTR;

                if ((jiffies - then) > HZ) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"Decoder stop wait failed.\n");
                        break;
                }

        } while (!rc);
        set_current_state(TASK_RUNNING);
        remove_wait_queue(&itv->streams[type].waitq, &wait);

        if (rc)
        	IVTV_DEBUG(IVTV_DEBUG_INFO, "Decoder Stop DMA wait failed.\n");
}

int ivtv_stop_v4l2_decode_stream(struct ivtv *itv, int type)
{
       	struct ivtv_stream *st = &itv->streams[type];
	int x, stop_trys = 3;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stop Decode.\n");
#if 0
	/* event notification (off)*/
        data[0] = 0; /* Event: 0 = audio change between stereo and mono */
        data[1] = 0; /* Enable/Disable: 0 = disabled, 1 = enabled */
        data[2] = 0x00010000; /* Bit: interrupt bit to fire */
        data[3] = -1; /* Mailbox to use: -1 = no mailbox needed */
        x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		     IVTV_API_DEC_EVENT_NOTIFICATION, &result, 4, &data[0]);
	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "stopDEC error 1. Code %d\n",x);
#endif

	/* turn off interrupts, we must leave the DMA done clear, else the
           OSD will break possibly, it turns itself off anyways, if not
           it still doesn't matter since if there are any pending it's better */
	ivtv_set_irq_mask(itv, IVTV_IRQ_MASK_DECODE);
	IVTV_DEBUG(IVTV_DEBUG_IRQ, 
		"DEC: IRQ Mask is now: 0x%08x\n",itv->irqmask);

	set_bit(IVTV_F_S_STREAMOFF, &st->s_flags);
	ivtv_stop_wait(itv, type);

	/* Lock */
	down(&st->mlock);

	/* Stop Decoder, must have failed, probably won't get here */
	if (test_and_clear_bit(IVTV_F_S_STREAMOFF, &st->s_flags)) {
		x = ivtv_vapi(itv, IVTV_API_DEC_STOP_PLAYBACK, 3,
			/*  0 = last frame, 1 = black */
			itv->dec_options.hide_last_frame,
			itv->dec_options.pts_low, /* when: pts low */
			itv->dec_options.pts_hi);  /* when: pts hi */
	
		if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"DEC: Error stopping decode (%d) on try number %d\n",
			x, stop_trys);
	}

	clear_bit(IVTV_F_S_NEEDS_DATA, &st->s_flags);
	clear_bit(IVTV_F_S_DECODING, &st->s_flags);
	clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags);

	if (!test_bit(IVTV_F_S_NO_DMA, &st->s_flags)) {
		/* disable VBI on TV-out */
		ivtv_disable_vbi(itv);
	}

        /* Decoder DMA State */
        itv->dec_dma_stat.last_addr     = 0;
        itv->dec_dma_stat.bytes_filled  = 0;
        itv->dec_dma_stat.bytes_needed  = 0;
        itv->dec_dma_stat.total_xfer    = 0;
        itv->dec_dma_stat.sg_bytes      = 0;
	itv->dec_dma_stat.data[0]	= 0x0;
	itv->dec_dma_stat.data[1]	= 0x0;
	itv->dec_dma_stat.data[2]	= 0x80000000;
	itv->dec_dma_stat.data[3]	= 0x0;
	itv->dec_dma_stat.vsync_count   = 0;
	itv->dec_dma_stat.ts   		= jiffies;
	itv->dec_dma_stat.wq_runs       = 0;
	atomic_set(&itv->dec_dma_stat.intr, 0);

	/* deincrement decoding */
	atomic_dec(&itv->decoding);

	/* UnLock */
	up(&st->mlock);

	/* wake up wait queues */
	wake_up(&st->waitq);

	return 0;
}

