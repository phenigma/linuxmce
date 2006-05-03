/*
    ioctl system call
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

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

#include "ivtv-driver.h"
#include "ivtv-version.h"
#include "ivtv-reset.h"
#include "ivtv-mailbox.h"
#include "ivtv-i2c.h"
#include "ivtv-queue.h"
#include "ivtv-fileops.h"
#include "ivtv-vbi.h"
#include "ivtv-audio.h"
#include "ivtv-video.h"
#include "ivtv-streams.h"
#include "ivtv-yuv.h"
#include "ivtv-ioctl.h"
#include "ivtv-gpio.h"
#include "ivtv-controls.h"
#include "ivtv-cards.h"
#include "ivtv-kthreads.h"
#include <media/audiochip.h>
#include <media/tveeprom.h>
#include <linux/i2c-id.h>

u16 service2vbi(int type)
{
        switch (type) {
                case V4L2_SLICED_TELETEXT_B:
                        return IVTV_SLICED_TYPE_TELETEXT_B;
                case V4L2_SLICED_CAPTION_525:
                        return IVTV_SLICED_TYPE_CAPTION_525;
                case V4L2_SLICED_WSS_625:
                        return IVTV_SLICED_TYPE_WSS_625;
                case V4L2_SLICED_VPS:
                        return IVTV_SLICED_TYPE_VPS;
                default:
                        return 0;
        }
}

static int valid_service_line(int field, int line, int is_pal)
{
        return (is_pal && line >= 6 && (line != 23 || field == 0)) ||
               (!is_pal && line >= 10 && line < 22);
}

static u16 select_service_from_set(int field, int line, u16 set, int is_pal)
{
        u16 valid_set = (is_pal ? V4L2_SLICED_VBI_625 : V4L2_SLICED_VBI_525);
        int i;

        set = set & valid_set;
        if (set == 0 || !valid_service_line(field, line, is_pal)) {
                return 0;
        }
        if (!is_pal) {
                if (line == 21 && (set & V4L2_SLICED_CAPTION_525))
                        return V4L2_SLICED_CAPTION_525;
        }
        else {
                if (line == 16 && field == 0 && (set & V4L2_SLICED_VPS))
                        return V4L2_SLICED_VPS;
                if (line == 23 && field == 0 && (set & V4L2_SLICED_WSS_625))
                        return V4L2_SLICED_WSS_625;
                if (line == 23)
                        return 0;
        }
        for (i = 0; i < 32; i++) {
                if ((1 << i) & set)
                        return 1 << i;
        }
        return 0;
}

static void expand_service_set(struct v4l2_sliced_vbi_format *fmt, int is_pal)
{
        u16 set = fmt->service_set;
        int f, l;

        fmt->service_set = 0;
        for (f = 0; f < 2; f++) {
                for (l = 0; l < 24; l++) {
                        fmt->service_lines[f][l] = select_service_from_set(f, l, set, is_pal);
                }
        }
}

static int check_service_set(struct v4l2_sliced_vbi_format *fmt, int is_pal)
{
        int f, l;
        u16 set = 0;

        for (f = 0; f < 2; f++) {
                for (l = 0; l < 24; l++) {
                        fmt->service_lines[f][l] = select_service_from_set(f, l, fmt->service_lines[f][l], is_pal);
                        set |= fmt->service_lines[f][l];
                }
        }
        return set != 0;
}

u16 get_service_set(struct v4l2_sliced_vbi_format *fmt)
{
        int f, l;
        u16 set = 0;

        for (f = 0; f < 2; f++) {
                for (l = 0; l < 24; l++) {
                        set |= fmt->service_lines[f][l];
                }
        }
        return set;
}

static const struct {
	v4l2_std_id  std;
	char        *name;
} enum_stds[] = {
	{ V4L2_STD_PAL_BG | V4L2_STD_PAL_H, "PAL-BGH" },
	{ V4L2_STD_PAL_DK,    "PAL-DK"    },
	{ V4L2_STD_PAL_I,     "PAL-I"     },
	{ V4L2_STD_PAL_M,     "PAL-M"     },
	{ V4L2_STD_PAL_N,     "PAL-N"     },
	{ V4L2_STD_PAL_Nc,    "PAL-Nc"    },
	{ V4L2_STD_SECAM_B | V4L2_STD_SECAM_G | V4L2_STD_SECAM_H, "SECAM-BGH" },
	{ V4L2_STD_SECAM_DK,  "SECAM-DK"  },
	{ V4L2_STD_SECAM_L,   "SECAM-L"   },
	{ V4L2_STD_SECAM_LC,  "SECAM-L'"  },
	{ V4L2_STD_NTSC_M,    "NTSC-M"    },
	{ V4L2_STD_NTSC_M_JP, "NTSC-J"    },
	{ V4L2_STD_NTSC_M_KR, "NTSC-K"    },
};

static const struct v4l2_standard ivtv_std_60hz = 
{
	.frameperiod = {.numerator = 1001, .denominator = 30000},
	.framelines = 525,
};

static const struct v4l2_standard ivtv_std_50hz = 
{
	.frameperiod = {.numerator = 1, .denominator = 25},
	.framelines = 625,
};

static void ivtv_stream_off(struct ivtv *itv, struct ivtv_stream *stream)
{
        IVTV_DEBUG_INFO("Entering STREAMOFF\n");

       /* Special case: a running VBI capture for VBI insertion
           in the mpeg stream. Need to stop that too. */
        if (itv->vbi_insert_mpeg) {
                ivtv_stop_capture(itv, IVTV_ENC_STREAM_TYPE_VBI);
                ivtv_release_stream(itv, IVTV_ENC_STREAM_TYPE_VBI);
        }

        /* Prevent others from messing around with streams until
           we've finished here. */
        set_bit(IVTV_F_S_STREAMOFF, &stream->s_flags);
        ivtv_stop_capture(itv, stream->type);
        return;
}

static int ivtv_s_speed(struct ivtv *itv, struct ivtv_speed *speed)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int type;
	struct ivtv_stream *st;

	/* YUV or MPG decoding stream */
	if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))
		type = IVTV_DEC_STREAM_TYPE_YUV;
	else
		type = IVTV_DEC_STREAM_TYPE_MPG;
	st = &itv->streams[type];

	/* Initialize */
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;

	if ((speed->scale < 0) || (speed->scale > 50)) {
		IVTV_DEBUG_WARN(
			   "DEC: Error with Scale 0x%08x\n", speed->scale);
		return -EINVAL;
	}

	if ((speed->speed < 0) || (speed->speed > 1)) {
		IVTV_DEBUG_WARN(
			   "DEC: Error with Speed 0x%08x\n", speed->speed);
		return -EINVAL;
	}

	if ((speed->fr_mask < 0) || (speed->fr_mask > 2)) {
		IVTV_DEBUG_WARN(
			   "DEC: Error with Frame Mask 0x%08x\n",
			   speed->fr_mask);
		return -EINVAL;
	}

	data[0] = speed->scale;

	if (speed->smooth)	/* smooth ff */
		data[0] |= 0x40000000;

	if (speed->speed)	/* fast forward */
		data[0] |= 0x80000000;

	if (speed->direction) {
		IVTV_DEBUG_WARN(
			   "DEC: Warning, Reverse not supported\n");

		/* B Frames when in reverse */
		if (speed->b_per_gop > itv->codec.bframes)
			speed->b_per_gop = itv->codec.bframes;
		else if (speed->b_per_gop < 0)
			speed->b_per_gop = itv->codec.bframes;
	}

	data[1] = speed->direction;	/* Forward. Reverse not supported */

	/* Unfortunately we translate and the original i/bp settings conflict */
	switch (speed->fr_mask) {
	case 0:
		data[2] = 1;	/* I */
		break;
	case 1:
	case 3:
		data[2] = 3;	/* BP */
		break;
	case 2:
	case 7:
	default:
		data[2] = 7;	/* IBP */
		break;
	}

	/* Store speed settings, may have to send with dec DMA */
	itv->dec_dma_stat.speed_data[0] = data[0];
	itv->dec_dma_stat.speed_data[1] = data[1];
	itv->dec_dma_stat.speed_data[2] = data[2];
	itv->dec_dma_stat.speed_data[3] = data[3] = speed->b_per_gop;
	itv->dec_dma_stat.speed_data[4] = data[4] = speed->aud_mute;	/* mute while fast/slow */
	itv->dec_dma_stat.speed_data[5] = data[5] = speed->fr_field;	/* frame or field at a time */
	itv->dec_dma_stat.speed_data[6] = data[6] = speed->mute;	/* # of frames to mute on normal speed resume */

	/* If not decoding, just change speed setting */
	if (atomic_read(&itv->decoding) == 0) {
		if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			     CX2341X_DEC_SET_PLAYBACK_SPEED, &result, 7, &data[0]))
		{
			IVTV_DEBUG_WARN(
				   "DEC: error changing speed\n");
			return (int)result;
		}

		return 0;
	}

        /* Stop all DMA and decoding activity */
	if (test_bit(IVTV_F_T_DEC_DONE, &itv->t_flags)) {
        	if (ivtv_vapi(itv, CX2341X_DEC_PAUSE_PLAYBACK, 1, 0)) {
                	IVTV_DEBUG_WARN("DEC: error pausing\n");
        	}
	}

        /* Wait for Read Interrupt */
        while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x15) &&
               (!test_bit(IVTV_F_S_DMAP, &st->s_flags)||
			wait_event_interruptible(itv->r_intr_wq,
                                        atomic_read(&itv->r_intr)))) {
                /* DMA Error */
                if (!test_bit(IVTV_F_S_DMAP, &st->s_flags) && 
			(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 
				0x15))
		{
                        break;
		}
        }

	/* Change Speed safely */
	if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		     CX2341X_DEC_SET_PLAYBACK_SPEED, &result, 7, &data[0])) {
		IVTV_DEBUG_WARN("DEC: error changing speed\n");
		return (int)result;
	}
	/* Save speed options if call succeeded */
	memcpy(&itv->dec_options.speed, &speed, sizeof(speed));

	IVTV_DEBUG_INFO(
		   "DEC: Setting Speed to "
		   "0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		   data[0], data[1], data[2], data[3], data[4], data[5],
		   data[6]);

	return 0;
}

static int ivtv_api_show_boxes(struct ivtv *itv, struct ivtv_mailbox *mbox,
	       			int args, int start, int stop)
{
	int x, i;
	struct ivtv_mailbox *local_box;

	for (x = start; x <= stop; x++) {
		local_box = &mbox[x];
		IVTV_INFO("Mailbox[%d] flags 0x%08x cmd 0x%08x timeout 0x%08x\n",
		       x, readl(&local_box->flags),
		       readl(&local_box->cmd), readl(&local_box->timeout));

		for (i = 0; i < args; i++) {
			IVTV_INFO(" data[%d] = 0x%08x\n",
			       i, readl(&local_box->data[i]));
		}
	}
	return 0;
}

static int ivtv_itvc(struct ivtv *itv, unsigned int cmd, void *arg)
{
	struct v4l2_register *regs = arg;
        unsigned long flags;
        unsigned char *reg_start;

        if (regs->reg >= 0x02000000 && regs->reg < IVTV_IOREMAP_SIZE)
                reg_start = itv->reg_mem - 0x02000000;
        else if (regs->reg >= 0x01000000 && regs->reg < 0x01800000)
                reg_start = itv->dec_mem - 0x01000000;
        else if (regs->reg >= 0x00000000 && regs->reg < 0x00800000)
                reg_start = itv->enc_mem;
        else 
                return -EINVAL;

	spin_lock_irqsave(&ivtv_cards_lock, flags);
	if (cmd == IVTV_IOC_G_ITVC_REG) {
		regs->val = readl(regs->reg + reg_start);
	} else {
		writel(regs->val, regs->reg + reg_start);
	}
	spin_unlock_irqrestore(&ivtv_cards_lock, flags);
        return 0;
}

static void ivtv_dec_VSYNC(struct ivtv *itv)
{
	u32 *tdata;
	u32 newframe;
	u64 newpts;
	u64 newscr;

	if (atomic_read(&itv->decoding) == 0)
		return;

	tdata = itv->dec_mbox[IVTV_MBOX_FIELD_DISPLAYED].data;
	newframe = readl(&tdata[0]);
	newpts = ((u64) readl(&tdata[2]) << 32) | (u64) (readl(&tdata[1]));
	newscr = ((u64) readl(&tdata[4]) << 32) | (u64) (readl(&tdata[3]));

	/* Return if zeros while decoding */
	if (newframe == 0 && newpts == 0 && newscr == 0) {
		return;
	}

	itv->dec_timestamp.frame = newframe;
	itv->dec_timestamp.pts = newpts;
	itv->dec_timestamp.scr = newscr;

	IVTV_DEBUG_INFO(
		   "DEC VSYNC: frames %d, pts %ld, scr %ld\n",
		   itv->dec_timestamp.frame,
		   (long int)itv->dec_timestamp.pts,
		   (long int)itv->dec_timestamp.scr);
}

static int ivtv_get_fmt(struct ivtv *itv, int streamtype, struct v4l2_format *fmt)
{
        switch (fmt->type) {
        /* case 0:	 Works around a bug in MythTV (already fixed).
           This case should be removed once post 0.1.10
           versions of ivtv have become mainstream. */
        case V4L2_BUF_TYPE_VIDEO_CAPTURE:
        case V4L2_BUF_TYPE_VIDEO_OUTPUT:
                fmt->fmt.pix.width = itv->width;
                fmt->fmt.pix.height = itv->height;
                fmt->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
                fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
                if (streamtype == IVTV_ENC_STREAM_TYPE_YUV ||
                                streamtype == IVTV_DEC_STREAM_TYPE_YUV) {
                        fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
                        /* YUV size is (Y=(h*w) + UV=(h*(w/2))) */
                        fmt->fmt.pix.sizeimage =
                                fmt->fmt.pix.height * fmt->fmt.pix.width +
                                fmt->fmt.pix.height * (fmt->fmt.pix.width / 2);
                } else {
                        fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_MPEG;
                        fmt->fmt.pix.sizeimage = 128 * 1024;
                }
                break;

        case V4L2_BUF_TYPE_VBI_CAPTURE:
                fmt->fmt.vbi.sampling_rate = 27000000;
                fmt->fmt.vbi.offset = 248;
                fmt->fmt.vbi.samples_per_line = itv->vbi_raw_decoder_line_size - 4;
                fmt->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
                fmt->fmt.vbi.start[0] = itv->vbi_start[0];
                fmt->fmt.vbi.start[1] = itv->vbi_start[1];
                fmt->fmt.vbi.count[0] = fmt->fmt.vbi.count[1] = itv->vbi_count;
                /* If this ioctl is used, then the caller
                   wants RAW format. Ugly hack until
                   the sliced VBI API is in V4L2. 
                   [ I think this is no longer required ]
                if (atomic_read(&itv->capturing) == 0) {
                        itv->vbi_in.raw = 1;
                }*/
                break;

        case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
        {
                struct v4l2_sliced_vbi_format *vbifmt = &fmt->fmt.sliced;

                vbifmt->io_size = sizeof(struct v4l2_sliced_vbi_data) * 36;
                memset(vbifmt->reserved, 0, sizeof(vbifmt->reserved));
                memset(vbifmt->service_lines, 0, sizeof(vbifmt->service_lines));
                if (itv->is_60hz) {
                        vbifmt->service_lines[0][21] = V4L2_SLICED_CAPTION_525;
                        vbifmt->service_lines[1][21] = V4L2_SLICED_CAPTION_525;
                } else {
                        vbifmt->service_lines[0][23] = V4L2_SLICED_WSS_625;
                        vbifmt->service_lines[0][16] = V4L2_SLICED_VPS;
                }
                vbifmt->service_set = get_service_set(vbifmt);
                break;
        }

        case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE: 
        {
                struct v4l2_sliced_vbi_format *vbifmt = &fmt->fmt.sliced;

                vbifmt->io_size = sizeof(struct v4l2_sliced_vbi_data) * 36;
                memset(vbifmt->reserved, 0, sizeof(vbifmt->reserved));
                memset(vbifmt->service_lines, 0, sizeof(vbifmt->service_lines));

                if (streamtype == IVTV_DEC_STREAM_TYPE_VBI) {
                        vbifmt->service_set = itv->is_50hz ? V4L2_SLICED_VBI_625 :
                                                 V4L2_SLICED_VBI_525;
                        expand_service_set(vbifmt, itv->is_50hz);
                        break;
                }

                itv->video_dec_func(itv, VIDIOC_G_FMT, fmt);
                vbifmt->service_set = get_service_set(vbifmt);
                break;
        }
        case V4L2_BUF_TYPE_VBI_OUTPUT:
        case V4L2_BUF_TYPE_VIDEO_OVERLAY:
        default:
                return -EINVAL;
        }
        return 0;
}

static int ivtv_try_or_set_fmt(struct ivtv *itv, int streamtype,
                struct v4l2_format *fmt, int set_fmt)
{
        struct v4l2_sliced_vbi_format *vbifmt = &fmt->fmt.sliced;
        u16 set;

        if (fmt->type == V4L2_BUF_TYPE_VIDEO_OUTPUT) 
                return ivtv_get_fmt(itv, streamtype, fmt);

        // set window size
        if (fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
                struct v4l2_format pix;

                if (!set_fmt)
                        return ivtv_get_fmt(itv, streamtype, fmt);

                if ((itv->width != fmt->fmt.pix.width ||
                     itv->height != fmt->fmt.pix.height) &&
                    atomic_read(&itv->capturing) > 0) {
                        return -EBUSY;
                }

                /* FIXME: only sets resolution for now */
		pix.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                pix.fmt.pix.width = itv->width = fmt->fmt.pix.width;
                pix.fmt.pix.height = itv->height = fmt->fmt.pix.height;
                itv->video_dec_func(itv, VIDIOC_S_FMT, &pix);

                if (itv->codec.stream_type == IVTV_STREAM_MPEG1 ||
                    itv->codec.stream_type == IVTV_STREAM_VCD) {
                        /* this is an MPEG1 stream */
                        IVTV_DEBUG_INFO("VIDIOC_S_FMT: "
                            "the current codec stream type is MPEG1 or VCD, "
                            "you have to do a S_CODEC after this ioctl\n");
                }
                return ivtv_get_fmt(itv, streamtype, fmt);
        }
       
        // set raw VBI format
        if (fmt->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
                if (set_fmt && streamtype == IVTV_ENC_STREAM_TYPE_VBI &&
                    itv->vbi_sliced_in->service_set &&
                    atomic_read(&itv->capturing) > 0) {
                        return -EBUSY;
                }
		itv->vbi_sliced_in->service_set = 0;
                itv->video_dec_func(itv, VIDIOC_S_FMT, &itv->vbi_in);
                return ivtv_get_fmt(itv, streamtype, fmt);
        }
       
        // set sliced VBI output
        // In principle the user could request that only certain
        // VBI types are output and that the others are ignored.
        // I.e., suppress CC in the even fields or only output
        // WSS and no VPS. Currently though there is no choice.
        if (fmt->type == V4L2_BUF_TYPE_SLICED_VBI_OUTPUT) 
                return ivtv_get_fmt(itv, streamtype, fmt);

        // any else but sliced VBI capture is an error
        if (fmt->type != V4L2_BUF_TYPE_SLICED_VBI_CAPTURE) 
                return -EINVAL;

        if (streamtype == IVTV_DEC_STREAM_TYPE_VBI) 
                return ivtv_get_fmt(itv, streamtype, fmt);

        // set sliced VBI capture format
        vbifmt->io_size = sizeof(struct v4l2_sliced_vbi_data) * 36;
        memset(vbifmt->reserved, 0, sizeof(vbifmt->reserved));

        if (vbifmt->service_set)
                expand_service_set(vbifmt, itv->is_50hz);
        set = check_service_set(vbifmt, itv->is_50hz);
        vbifmt->service_set = get_service_set(vbifmt);

        if (!set_fmt)
                return 0;
        if (set == 0)
                return -EINVAL;
        if (atomic_read(&itv->capturing) > 0 && itv->vbi_sliced_in->service_set == 0) {
                return -EBUSY;
        }
        itv->video_dec_func(itv, VIDIOC_S_FMT, fmt);
	memcpy(itv->vbi_sliced_in, vbifmt, sizeof(*itv->vbi_sliced_in));
        return 0;
}


static int ivtv_internal_ioctls(struct ivtv *itv, int streamtype, unsigned int cmd,
			 void *arg)
{
	struct v4l2_register *reg = arg;

	switch (cmd) {
	case IVTV_IOC_FWAPI:{
		struct ivtv_ioctl_fwapi *fwapi = arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_FWAPI\n");

		/* Encoder */
		if (fwapi->cmd >= 128)
			return ivtv_api(itv, itv->enc_mbox, &itv->enc_msem,
					fwapi->cmd, &fwapi->result, fwapi->args,
					fwapi->data);

		/* Decoder */
		return ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, fwapi->cmd,
				&fwapi->result, fwapi->args, fwapi->data);
	}

	case IVTV_IOC_EVENT_SETUP:{
		struct ivtv_ioctl_event *event = arg;
		int x;
		int mbox;

		IVTV_DEBUG_IOCTL("IVTV_IOC_EVENT_SETUP\n");

		mbox = event->mbox;

		if (213 == event->api.cmd && 1 == event->api.data[1])
			ivtv_clear_irq_mask(itv, event->api.data[2]);
		else if (213 == event->api.cmd)
			ivtv_set_irq_mask(itv, event->api.data[2]);

		if (event->type) {
			IVTV_DEBUG_WARN("sending event to encoder\n");
			/* Encoder */
			x = ivtv_api(itv, &itv->enc_mbox[mbox], &itv->enc_msem,
				     event->api.cmd, &event->api.result,
				     event->api.args, event->api.data);
		} else {
			IVTV_DEBUG_WARN("sending event to decoder\n");
			/* Decoder */
			x = ivtv_api(itv, &itv->dec_mbox[mbox], &itv->dec_msem,
				     event->api.cmd, &event->api.result,
				     event->api.args, event->api.data);
		}

		ivtv_api_show_boxes(itv, &itv->dec_mbox[mbox], 4, mbox, mbox);
		return x;
	}

	case IVTV_IOC_RELOAD_FW:{
		int ret = 0;

		IVTV_DEBUG_IOCTL("IVTV_IOC_RELOAD_FW\n");

		/* Force Full Reload */
		ret = ivtv_reset_card(itv, 1, IVTV_CMD_FULL_RESET);

		return ret;
	}

	/* ioctls to allow direct access to the ITVC registers for testing */
	case IVTV_IOC_G_ITVC_REG:
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_ITVC_REG\n");
		return ivtv_itvc(itv, cmd, arg);

	case IVTV_IOC_S_ITVC_REG:
		IVTV_DEBUG_IOCTL("IVTV_IOC_S_ITVC_REG\n");
		return ivtv_itvc(itv, cmd, arg);

	/* ioctls to allow direct access to the decoder registers for testing */
	case IVTV_IOC_G_DECODER_REG:
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_DECODER_REG\n");
		if (itv->hw_flags & IVTV_HW_CX25840)
			reg->i2c_id = I2C_DRIVERID_CX25840;
		else if (itv->hw_flags & IVTV_HW_SAA711X)
			reg->i2c_id = I2C_DRIVERID_SAA711X;
		else
			reg->i2c_id = I2C_DRIVERID_SAA717X;
		return itv->video_dec_func(itv, VIDIOC_INT_G_REGISTER, reg);

	case IVTV_IOC_S_DECODER_REG:
		IVTV_DEBUG_IOCTL("IVTV_IOC_S_DECODER_REG\n");
		if (itv->hw_flags & IVTV_HW_CX25840)
			reg->i2c_id = I2C_DRIVERID_CX25840;
		else if (itv->hw_flags & IVTV_HW_SAA711X)
			reg->i2c_id = I2C_DRIVERID_SAA711X;
		else
			reg->i2c_id = I2C_DRIVERID_SAA717X;
		return itv->video_dec_func(itv, VIDIOC_INT_S_REGISTER, reg);

	/* ioctls to allow direct access to the encoder registers for testing */
	case IVTV_IOC_G_ENCODER_REG:
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_ENCODER_REG\n");
		reg->i2c_id = I2C_DRIVERID_SAA7127;
		return ivtv_saa7127(itv, VIDIOC_INT_G_REGISTER, reg);

	case IVTV_IOC_S_ENCODER_REG:
		IVTV_DEBUG_IOCTL("IVTV_IOC_S_ENCODER_REG\n");
		reg->i2c_id = I2C_DRIVERID_SAA7127;
		return ivtv_saa7127(itv, VIDIOC_INT_S_REGISTER, reg);

	case IVTV_IOC_ZCOUNT:{
		/* Zeroes out usage count so it can be unloaded in case of
		 * drastic error */

		IVTV_DEBUG_IOCTL("ZCOUNT\n");

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
		while (MOD_IN_USE)
			MOD_DEC_USE_COUNT;

		MOD_INC_USE_COUNT;
#endif
		break;
	}

	case IVTV_IOC_S_MSP_MATRIX:{
		struct ivtv_msp_matrix *matrix = arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_S_MSP_MATRIX\n");
		return ivtv_audio_set_matrix(itv, matrix);
	}

	case IVTV_IOC_G_DEBUG_LEVEL:
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_DEBUG_LEVEL\n");
		*(int *)arg = ivtv_debug;
		break;

	case IVTV_IOC_S_DEBUG_LEVEL:
		IVTV_DEBUG_IOCTL("IVTV_IOC_S_DEBUG_LEVEL\n");
		ivtv_debug = (*(int *)arg) | IVTV_DBGFLG_WARN;
		*(int *)arg = ivtv_debug;
		break;

	case IVTV_IOC_RESET_IR:
		IVTV_DEBUG_IOCTL("IVTV_IOC_RESET_IR\n");
		ivtv_reset_ir_gpio(itv);
		break;	
        
	default:
		IVTV_DEBUG_IOCTL( "Unknown internal IVTV command %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}

int ivtv_v4l2_ioctls(struct ivtv *itv, struct ivtv_open_id *id,
		     int streamtype, unsigned int cmd, void *arg)
{
	struct ivtv_stream *stream = &itv->streams[streamtype];

	switch (cmd) {
	case VIDIOC_QUERYCAP:{
		struct v4l2_capability *vcap = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_QUERYCAP\n");

		memset(vcap, 0, sizeof(*vcap));
		strcpy(vcap->driver, IVTV_DRIVER_NAME);     /* driver name */
		strcpy(vcap->card, itv->card_name); 	    /* card type */
                strcpy(vcap->bus_info, pci_name(itv->dev)); /* bus info... */
		vcap->version = IVTV_DRIVER_VERSION; 	    /* version */
		vcap->capabilities = itv->v4l2_cap; 	    /* capabilities */

		/* reserved.. must set to 0! */
		vcap->reserved[0] = vcap->reserved[1] = 
			vcap->reserved[2] = vcap->reserved[3] = 0;
		break;
	}

	case VIDIOC_ENUMAUDIO:{
		struct v4l2_audio *vin = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_ENUMAUDIO\n");

		return ivtv_get_audio_input(itv, vin->index, vin);
	}

	case VIDIOC_G_AUDIO:{
		struct v4l2_audio *vin = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_G_AUDIO\n");
		vin->index = itv->audio_input;
		return ivtv_get_audio_input(itv, vin->index, vin);
	}

	case VIDIOC_S_AUDIO:{
		struct v4l2_audio *vout = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_AUDIO\n");

		if (vout->index >= itv->nof_audio_inputs)
			return -EINVAL;
		itv->audio_input = vout->index;
		ivtv_audio_set_io(itv);
		break;
	}

	case VIDIOC_ENUMAUDOUT:{
		struct v4l2_audioout *vin = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_ENUMAUDOUT\n");

		/* set it to defaults from our table */
		return ivtv_get_audio_output(itv, vin->index, vin);
	}

	case VIDIOC_G_AUDOUT:{
		struct v4l2_audioout *vin = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_G_AUDOUT\n");
		vin->index = 0;
		return ivtv_get_audio_output(itv, vin->index, vin);
	}

	case VIDIOC_S_AUDOUT:{
		struct v4l2_audioout *vout = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_AUDOUT\n");

		return ivtv_get_audio_output(itv, vout->index, vout);
	}

	case VIDIOC_ENUMINPUT:{
		struct v4l2_input *vin = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_ENUMINPUT\n");

		/* set it to defaults from our table */
		return ivtv_get_input(itv, vin->index, vin);
	}

	case VIDIOC_ENUMOUTPUT:{
		struct v4l2_output *vout = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_ENUMOUTPUT\n");

		return ivtv_get_output(itv, vout->index, vout);
	}

	case VIDIOC_TRY_FMT:
	case VIDIOC_S_FMT: {
		struct v4l2_format *fmt = arg;

                if (cmd == VIDIOC_S_FMT) {
                        IVTV_DEBUG_IOCTL("VIDIOC_S_FMT\n");
                } else {
                        IVTV_DEBUG_IOCTL("VIDIOC_TRY_FMT\n");
                }
                return ivtv_try_or_set_fmt(itv, streamtype, fmt, cmd == VIDIOC_S_FMT);
	}

	case VIDIOC_G_FMT: {
		struct v4l2_format *fmt = arg;
		int type = fmt->type;

		IVTV_DEBUG_IOCTL("VIDIOC_G_FMT\n");
		memset(fmt, 0, sizeof(*fmt));
		fmt->type = type;
                return ivtv_get_fmt(itv, streamtype, fmt);
        }

	case VIDIOC_G_INPUT:{
		IVTV_DEBUG_IOCTL("VIDIOC_G_INPUT\n");

		*(int *)arg = itv->active_input;
		break;
	}

	case VIDIOC_S_INPUT:{
		int inp = *(int *)arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_INPUT\n");

		if (inp >= itv->nof_inputs)
			return -EINVAL;

		if (inp == itv->active_input) {
			IVTV_DEBUG_INFO("Input unchanged\n");
			break;
		}
		IVTV_DEBUG_INFO("Changing input from %d to %d\n",
			       	itv->active_input, inp);

		itv->active_input = inp;
		/* Set the audio input to whatever is appropriate for the
		   input type. */
		itv->audio_input = itv->card->video_inputs[inp].audio_index;

		/* prevent others from messing with the streams until
		   we're finished changing inputs. */
		down(&stream->mlock);
		mute_and_pause(itv);

		ivtv_video_set_io(itv);

		/* Select new audio input */
		ivtv_audio_set_io(itv);
		unmute_and_resume(itv, 1);
		up(&stream->mlock);
		break;
	}

	case VIDIOC_G_OUTPUT:{
		IVTV_DEBUG_IOCTL("VIDIOC_G_OUTPUT\n");

		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		*(int *)arg = itv->active_output;
		break;
	}

	case VIDIOC_S_OUTPUT:{
		int outp = *(int *)arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_OUTPUT\n");

		if (outp >= itv->card->nof_outputs)
			return -EINVAL;

		if (outp == itv->active_output) {
			IVTV_DEBUG_INFO("Output unchanged\n");
			break;
		}
		IVTV_DEBUG_INFO("Changing output from %d to %d\n",
			   itv->active_output, outp);

		itv->active_output = outp;
		outp = itv->card->video_outputs[outp].video_output;

		ivtv_saa7127(itv, VIDIOC_S_OUTPUT, &outp);
		break;
	}

	case VIDIOC_G_FREQUENCY:{
		struct v4l2_frequency *vf = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_G_FREQUENCY\n");

		if (vf->tuner != 0)
			return -EINVAL;
		ivtv_call_i2c_clients(itv, cmd, arg);
		break;
	}

	case VIDIOC_S_FREQUENCY:{
		struct v4l2_frequency vf = *(struct v4l2_frequency *)arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_FREQUENCY\n");

		if (vf.tuner != 0)
			return -EINVAL;

		/* Set frequency, prevent others from messing around with
		   streams until we're done. */
		down(&stream->mlock);
		mute_and_pause(itv);
		IVTV_DEBUG_INFO("v4l2 ioctl: set frequency %d\n", vf.frequency);
		ivtv_call_i2c_clients(itv, cmd, &vf);
		unmute_and_resume(itv, 1);
		up(&stream->mlock);
		break;
	}

	case VIDIOC_ENUMSTD:{
		struct v4l2_standard *vs = arg;
		int idx = vs->index;

		IVTV_DEBUG_IOCTL("VIDIOC_ENUMSTD\n");

		if (idx < 0 || idx >= ARRAY_SIZE(enum_stds))
			return -EINVAL;

		*vs = (enum_stds[idx].std & V4L2_STD_525_60) ? 
				ivtv_std_60hz : ivtv_std_50hz;
		vs->index = idx;
		vs->id = enum_stds[idx].std;
		strcpy(vs->name, enum_stds[idx].name);
		break;
	}

	case VIDIOC_G_STD:{
		IVTV_DEBUG_IOCTL("VIDIOC_G_STD\n");
		*(v4l2_std_id *) arg = itv->std;
		break;
	}

	case VIDIOC_S_STD:{
		v4l2_std_id std = *(v4l2_std_id *) arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_STD\n");

		if ((std & V4L2_STD_ALL) == 0)
			return -EINVAL;

		if (std == itv->std)
			break;

		/* Prevent others from messing around with streams while
		   we change standard. */
		down(&stream->mlock);
		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags) ||
		    atomic_read(&itv->capturing) > 0 ||
		    atomic_read(&itv->decoding) > 0) {
			/* Switching standard would turn off the radio or mess
			   with already running streams, prevent that by
			   returning EBUSY. */
			up(&stream->mlock);
			return -EBUSY;
		}

		itv->std = std;
		itv->width = 720;
		if (itv->std & V4L2_STD_525_60) {
                        itv->is_50hz = 0;
                        itv->is_60hz = 1;
			itv->height = 480;
			itv->vbi_count = 12;
			itv->vbi_start[0] = 10;
			itv->vbi_start[1] = 273;
		} else {	/* PAL/SECAM */
                        itv->is_50hz = 1;
                        itv->is_60hz = 0;
			itv->height = 576;
			itv->vbi_count = 18;
			itv->vbi_start[0] = 6;
			itv->vbi_start[1] = 318;
		}
		IVTV_DEBUG_INFO("Switching standard to %llx.\n", itv->std);

		/* Tuner */
		ivtv_call_i2c_clients(itv, VIDIOC_S_STD, &itv->std);

		if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
			/* set display standard */
			ivtv_vapi(itv, CX2341X_DEC_SET_STANDARD, 1, itv->is_50hz);
		}
		up(&stream->mlock);
		break;
	}

	case VIDIOC_S_TUNER: {	/* Setting tuner can only set audio mode */
		struct v4l2_tuner *vt = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_S_TUNER\n");

		if (vt->index != 0)
			return -EINVAL;

		ivtv_call_i2c_clients(itv, VIDIOC_S_TUNER, vt);
		if (itv->hw_flags & IVTV_HW_MSP34XX) {
			ivtv_dualwatch_set_enabled(itv,
				       	vt->audmode == V4L2_TUNER_MODE_STEREO);
		}
		break;
	}

	case VIDIOC_G_TUNER: {
		struct v4l2_tuner *vt = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_G_TUNER\n");

		if (vt->index != 0)
			return -EINVAL;

		memset(vt, 0, sizeof(*vt));
		ivtv_call_i2c_clients(itv, VIDIOC_G_TUNER, vt);

		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			strcpy(vt->name, "ivtv Radio Tuner");
			vt->type = V4L2_TUNER_RADIO;
		} else {
			strcpy(vt->name, "ivtv TV Tuner");
			vt->type = V4L2_TUNER_ANALOG_TV;
		}
		break;
	}

	case VIDIOC_STREAMOFF:{
		IVTV_DEBUG_IOCTL("VIDIOC_STREAMOFF\n");
		if (id->open_id != stream->id)
			return -EBUSY;

		ivtv_stream_off(itv, stream);
		break;
	}

        case VIDIOC_G_SLICED_VBI_CAP: {
		struct v4l2_sliced_vbi_cap *cap = arg;
                int set = itv->is_50hz ? V4L2_SLICED_VBI_625 : V4L2_SLICED_VBI_525;
                int f, l;

		IVTV_DEBUG_IOCTL("VIDIOC_G_SLICED_VBI_CAP\n");
                memset(cap, 0, sizeof(*cap));
		if (streamtype == IVTV_ENC_STREAM_TYPE_VBI ||
		    streamtype == IVTV_DEC_STREAM_TYPE_VBI) {
                        for (f = 0; f < 2; f++) {
                                for (l = 0; l < 24; l++) {
                                        if (valid_service_line(f, l, itv->is_50hz)) {
                                                cap->service_lines[f][l] = set;
                                        }
                                }
                        }
		} else if (streamtype == IVTV_DEC_STREAM_TYPE_VOUT) {
                        if (itv->is_60hz) {
                                cap->service_lines[0][21] = V4L2_SLICED_CAPTION_525;
                                cap->service_lines[1][21] = V4L2_SLICED_CAPTION_525;
                        } else {
                                cap->service_lines[0][23] = V4L2_SLICED_WSS_625;
                                cap->service_lines[0][16] = V4L2_SLICED_VPS;
                        }
		}
                for (f = 0; f < 2; f++) {
                        for (l = 0; l < 24; l++) {
                                cap->service_set |= cap->service_lines[f][l];
                        }
                }
                break;
        }

        case VIDIOC_G_FBUF:
        {
		struct v4l2_framebuffer *fb = arg;

		IVTV_DEBUG_IOCTL("VIDIOC_G_FBUF\n");
		memset(fb, 0, sizeof(*fb));
                break;
        }

        case VIDIOC_LOG_STATUS:
                IVTV_INFO("=================  START STATUS CARD #%d  =================\n", itv->num);
                if (itv->hw_flags & IVTV_HW_TVEEPROM) {
			struct tveeprom tv;

			ivtv_read_eeprom(itv, &tv);
                }
		ivtv_call_i2c_clients(itv, VIDIOC_LOG_STATUS, 0);
                IVTV_INFO("==================  END STATUS CARD #%d  ==================\n", itv->num);
                break;

	default:
		IVTV_DEBUG_WARN("unknown VIDIOC command %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}

static int ivtv_ivtv_ioctls(struct ivtv *itv, struct ivtv_open_id *id,
		     int streamtype, unsigned int cmd, void *arg)
{
	struct ivtv_stream *stream = &itv->streams[streamtype];

	switch (cmd) {
	case IVTV_IOC_PASSTHROUGH:{
		int enable = *(int *)arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_PASSTHROUGH\n");
		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		return ivtv_passthrough_mode(itv, enable);
	}

	case IVTV_IOC_GET_FB:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_GET_FB\n");
		if (itv->fb_id < 0)
			return -EINVAL;
		*(int *)arg = itv->fb_id;

		break;
	}

	case IVTV_IOC_FRAMESYNC:{
		struct ivtv_ioctl_framesync *fs = arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_FRAMESYNC\n");
		/* Wait for next field, seems to give us 1 per field */
            	wait_event_interruptible(itv->vsync_w_fs,
                       (atomic_read(&itv->fs_vsync) > 0));
		atomic_set(&itv->fs_vsync, 0);

		ivtv_dec_VSYNC(itv);
		*fs = itv->dec_timestamp;

		if (atomic_read(&itv->decoding) == 0) {
			u32 data[IVTV_MBOX_MAX_DATA], result;

			memset(fs, 0, sizeof(fs));
			/* scr should be valid anytime */
			if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
				     CX2341X_DEC_GET_TIMING_INFO, &result, 5,
				     &data[0])) {
				IVTV_DEBUG_WARN(
					   "DEC: couldn't read clock\n");
				return -EIO;
			}

			fs->frame = 0;
			fs->pts = 0;
			fs->scr =
			    (u64) (((u64) data[4] << 32) | (u64) (data[3]));
			break;
		}

		if (signal_pending(current))
			return -EINTR;

		break;
	}

	case IVTV_IOC_PLAY:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_PLAY\n");
		if (ivtv_vapi(itv, CX2341X_DEC_START_PLAYBACK, 2, 0, 0))
			IVTV_DEBUG_WARN(
				   "DEC: error starting playback\n");
		break;

	}

	case IVTV_IOC_START_DECODE:
	case IVTV_IOC_S_START_DECODE:{
		struct ivtv_cfg_start_decode *sd = arg;
		struct ivtv_stream *stream =
		    &itv->streams[IVTV_DEC_STREAM_TYPE_MPG];

                if (cmd == IVTV_IOC_STOP_DECODE)
                        IVTV_DEBUG_IOCTL("IVTV_IOC_START_DECODE\n");
                else
                        IVTV_DEBUG_IOCTL("IVTV_IOC_S_START_DECODE\n");
		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		if ((sd->gop_offset < 0) || (sd->gop_offset > 15))
			return -EINVAL;
		if (sd->muted_audio_frames < 0)
			return -EINVAL;

		itv->dec_options.gop_offset = sd->gop_offset;
		itv->dec_options.mute_frames = sd->muted_audio_frames;

		/* FIXME this is new, so not compatible with old apps */
		/* We need to enable it in Myth, or have another ioctl */
		/* These allow to splice and stop decoding entire buffers */
		/*itv->dec_options.fast_stop = sd->fast_stop;
		itv->dec_options.no_stop = sd->no_stop;*/

		if (cmd == IVTV_IOC_S_START_DECODE)
			break;

		if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags))
			return -EBUSY;

		if (ivtv_claim_stream(id, IVTV_DEC_STREAM_TYPE_MPG)) {
			/* someone else is using this stream already */
			IVTV_DEBUG_WARN(
				   "DEC: start_decode, stream already claimed\n");
			return -EBUSY;
		}

		/* stop decoding if already happening */
		if (test_bit(IVTV_F_S_DECODING, &stream->s_flags)) {
			IVTV_DEBUG_WARN(
				   "DEC: start_decode, stream already started!!!, stopping\n");
			ivtv_stop_v4l2_decode_stream(itv,
						     IVTV_DEC_STREAM_TYPE_MPG);
		}

		/* Run Work Queue */
		atomic_inc(&itv->dec_dma_stat.intr);
		dec_schedule_work(itv);

		return 0;
	}

	case IVTV_IOC_STOP_DECODE:
	case IVTV_IOC_S_STOP_DECODE:{
		struct ivtv_cfg_stop_decode *sd = arg;
		struct ivtv_stream *stream =
		    &itv->streams[IVTV_DEC_STREAM_TYPE_MPG];

                if (cmd == IVTV_IOC_STOP_DECODE)
                        IVTV_DEBUG_IOCTL("IVTV_IOC_STOP_DECODE\n");
                else
                        IVTV_DEBUG_IOCTL("IVTV_IOC_S_STOP_DECODE\n");
		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		if ((sd->hide_last < 0) || (sd->hide_last > 1))
			return -EINVAL;
		itv->dec_options.hide_last_frame = sd->hide_last;

		itv->dec_options.pts_low = (u32) (sd->pts_stop & 0xFFFFFFFF);
		itv->dec_options.pts_hi = (u32) (sd->pts_stop >> 32);

		if (cmd == IVTV_IOC_S_STOP_DECODE)
			break;

		if (!test_bit(IVTV_F_S_DECODING, &stream->s_flags))
			break;

		if (id->open_id != stream->id) {
			IVTV_DEBUG_WARN(
				   "DEC: STOP_DECODE:: "
				   "id->open_id 0x%08x !=stream->id 0x%08x\n",
				   id->open_id, (unsigned int)stream->id);
			return -EBUSY;
		}

		ivtv_stop_v4l2_decode_stream(itv, IVTV_DEC_STREAM_TYPE_MPG);
		ivtv_flush_queues_lock(itv, IVTV_DEC_STREAM_TYPE_MPG);

		break;
	}

	case IVTV_IOC_DEC_FLUSH:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_DEC_FLUSH\n");
		ivtv_flush_queues_lock(itv, IVTV_DEC_STREAM_TYPE_MPG);
		break;
	}

	case IVTV_IOC_DEC_STEP:{
		int howfar = *(int *)arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_DEC_STEP\n");
		if (howfar < 0 || howfar > 2)
			return -EINVAL;

		/* howfar: 0 = 1 frame, 1 = top field, 2 = bottom field */
		if (ivtv_vapi(itv, CX2341X_DEC_STEP_VIDEO, 1, howfar)) {
			IVTV_DEBUG_WARN("DEC: error stepping\n");
		}
		break;
	}

	case IVTV_IOC_DEC_SPLICE:{
		int splice = *(int *)arg;
		IVTV_DEBUG_IOCTL("IVTV_IOC_DEC_SPLICE\n");
		itv->dec_options.no_stop = splice;
		break;
	}

	case IVTV_IOC_DEC_FAST_STOP:{
		int fast_stop = *(int *)arg;
		IVTV_DEBUG_IOCTL("IVTV_IOC_DEC_FAST_STOP\n");
		itv->dec_options.fast_stop = fast_stop;
		break;
	}

	case IVTV_IOC_G_SPEED:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_SPEED\n");
		*(struct ivtv_speed *)arg = itv->dec_options.speed;
		break;
	}

	case IVTV_IOC_S_SPEED:{
		struct ivtv_speed *speed = arg;
		int ret;

		IVTV_DEBUG_IOCTL("IVTV_IOC_S_SPEED\n");
		ret = ivtv_s_speed(itv, speed);

		if (ret) {
			IVTV_DEBUG_WARN(
				   "DEC: error in slow/fast mode\n");
			return ret;
		}

		break;
	}

	case IVTV_IOC_S_SLOW_FAST:{
		struct ivtv_slow_fast *sf = arg;
		struct ivtv_speed speed;
		int ret;

		IVTV_DEBUG_IOCTL("IVTV_IOC_S_SLOW_FAST\n");
		if ((sf->scale < 0) || (sf->scale > 50))
			return -EINVAL;
		if ((sf->speed < 0) || (sf->speed > 1))
			return -EINVAL;

		speed = itv->dec_options.speed;
		speed.scale = sf->scale;
		speed.speed = sf->speed;

		ret = ivtv_s_speed(itv, &speed);

		if (ret) {
			IVTV_DEBUG_WARN(
				   "DEC: error in set slow/fast mode\n");
			return ret;
		}

		break;
	}

	case IVTV_IOC_PAUSE:{
        	struct ivtv_stream *st;
		int type;

		IVTV_DEBUG_IOCTL("IVTV_IOC_PAUSE\n");
        	/* YUV or MPG decoding stream */
        	if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))
                	type = IVTV_DEC_STREAM_TYPE_YUV;
        	else
                	type = IVTV_DEC_STREAM_TYPE_MPG;
        	st = &itv->streams[type];

		/* We really don't want to get stuck inbetween DMA calls */
		if ((atomic_read(&itv->decoding) > 0) &&
			test_bit(IVTV_F_T_DEC_DONE, &itv->t_flags)) 
		{
			if (ivtv_vapi(itv, CX2341X_DEC_PAUSE_PLAYBACK, 1, 0)) {
				IVTV_DEBUG_WARN(
					"DEC: error pausing\n");
			}
		} else
			IVTV_DEBUG_INFO(
					"DEC: Pause Error, "
					"Decoder isn't running\n");

		break;
	}

	case IVTV_IOC_GET_TIMING:{
		struct ivtv_ioctl_framesync *timing = arg;
		u32 data[IVTV_MBOX_MAX_DATA], result;

		IVTV_DEBUG_IOCTL("IVTV_IOC_GET_TIMING\n");
		if (atomic_read(&itv->decoding) == 0) {
			memset(timing, 0, sizeof(timing));
			/* scr should be valid anytime */
			if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
				     CX2341X_DEC_GET_TIMING_INFO, &result, 5,
				     &data[0])) {
				IVTV_DEBUG_WARN(
					   "DEC: couldn't read clock\n");
				return -EIO;
			}

			timing->frame = 0;
			timing->pts = 0;
			timing->scr =
			    (u64) (((u64) data[4] << 32) | (u64) (data[3]));

		} else {
			if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
				     CX2341X_DEC_GET_TIMING_INFO, &result, 5,
				     &data[0])) {
				IVTV_DEBUG_WARN(
					   "DEC: couldnt read clock\n");
				return -EIO;
			}
			timing->frame = data[0];
			timing->pts =
			    (u64) ((u64) data[2] << 32) | (u64) data[1];
			timing->scr =
			    (u64) (((u64) data[4] << 32) | (u64) (data[3]));
		}
		break;
	}

	case IVTV_IOC_S_VBI_EMBED:{
                int embed = *(int *)arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_S_VBI_EMBED\n");
		if (!(itv->v4l2_cap & V4L2_CAP_SLICED_VBI_CAPTURE))
			return -EINVAL;
                if (embed != 0 && embed != 1)
                        return -EINVAL;
		if (atomic_read(&itv->capturing) > 0)
			return -EBUSY;

                // First try to allocate sliced VBI buffers if needed.
                if (embed && itv->vbi_sliced_mpeg_data[0] == NULL) {
                        int i;

                        for (i = 0; i < IVTV_VBI_FRAMES; i++) {
                                // Yuck, hardcoded. Needs to be a define
                                itv->vbi_sliced_mpeg_data[i] =
                                        kmalloc(2049, GFP_KERNEL);
                                if (itv->vbi_sliced_mpeg_data[i] == NULL) {
                                        while (--i >= 0) {
                                                kfree(itv->vbi_sliced_mpeg_data[i]);
                                                itv->vbi_sliced_mpeg_data[i] = NULL;
                                        }
                                        return -ENOMEM;
                                }
                        }
                }

		itv->vbi_insert_mpeg = embed;

		if (itv->vbi_insert_mpeg == 0) {
                        break;
                }
		/* Need sliced data for mpeg insertion */
                if (get_service_set(itv->vbi_sliced_in) == 0) {
                        if (itv->is_60hz)
        			itv->vbi_sliced_in->service_set = V4L2_SLICED_CAPTION_525;
                        else
        			itv->vbi_sliced_in->service_set = V4L2_SLICED_WSS_625;
        		expand_service_set(itv->vbi_sliced_in, itv->is_50hz);
                }
		break;
	}

	case IVTV_IOC_G_VBI_EMBED:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_VBI_EMBED\n");
		*(int *)arg = itv->vbi_insert_mpeg;
		break;
	}

	case IVTV_IOC_S_VBI_PASSTHROUGH:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_S_VBI_PASSTHROUGH\n");
		if (!(itv->v4l2_cap & V4L2_CAP_SLICED_VBI_OUTPUT))
			return -EINVAL;
		itv->vbi_passthrough = *(int *)arg;
		break;
	}

	case IVTV_IOC_G_VBI_PASSTHROUGH:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_G_VBI_PASSTHROUGH\n");
		*(int *)arg = itv->vbi_passthrough;
		break;
	}

	case IVTV_IOC_G_CODEC:{
		struct ivtv_ioctl_codec *codec = arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_G_CODEC\n");
		*codec = itv->codec;
		codec->framerate = itv->codec.framerate;
		codec->framespergop = itv->codec.framespergop;
		break;
	}

	case IVTV_IOC_S_CODEC:{
		struct ivtv_ioctl_codec *codec = arg;
		struct v4l2_format fmt;

		IVTV_DEBUG_IOCTL("IVTV_IOC_S_CODEC\n");
		/* FIXME: insert abundant parameter validation here */
		if ((codec->bitrate == 0) || (codec->bitrate_peak == 0) ||
		    (codec->bitrate > codec->bitrate_peak)) {
			IVTV_DEBUG_WARN("ivtv ioctl: set "
				   "bitrate=%u < peak=%u: failed\n",
				   codec->bitrate, codec->bitrate_peak);
			return -EINVAL;
		}

                /* check if it is an MPEG1 stream */
                if ((codec->stream_type == IVTV_STREAM_MPEG1) ||
                    (codec->stream_type == IVTV_STREAM_VCD)) {
                        /* this is an MPEG1 stream */
                        int tmp_height = itv->is_60hz ? 480 : 576;

                        /* set vertical mpeg encoder resolution */
                        if (itv->height != tmp_height / 2) {
                                itv->height = tmp_height / 2;
                        }
                        /* mpeg1 is cbr */
                        codec->bitrate_mode = 1;

                        IVTV_DEBUG_INFO(
                                   "ivtv ioctl: set codec: "
                                   "stream_type is MPEG1 or VCD. resolution %dx%d.\n",
                                   itv->width, itv->height);

                        /* fix videodecoder resolution */
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        fmt.fmt.pix.width = itv->width;
                        fmt.fmt.pix.height = tmp_height;
                        if (atomic_read(&itv->capturing) > 0) {
                                IVTV_DEBUG_WARN(
                                        "v4l2 ioctl: set size failed, "
                                        "capture in progress\n");
                        } else {
                                itv->video_dec_func(itv, VIDIOC_S_FMT, &fmt);
                        }
                }

                /* Passed the garbage check */
                itv->codec = *codec;

		ivtv_audio_set_audio_clock_freq(itv, codec->audio_bitmask & 0x03);
		break;
	}

	case IVTV_IOC_S_GOP_END:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_S_GOP_END\n");
		itv->end_gop = *(int *)arg;
		*(int *)arg = itv->end_gop;
		break;
	}

	case IVTV_IOC_PAUSE_ENCODE:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_PAUSE_ENCODE\n");
		down(&stream->mlock);
		mute_and_pause(itv);
		up(&stream->mlock);
		break;
	}

	case IVTV_IOC_RESUME_ENCODE:{
		IVTV_DEBUG_IOCTL("IVTV_IOC_RESUME_ENCODE\n");
		down(&stream->mlock);
		unmute_and_resume(itv, 1);
		up(&stream->mlock);
		break;
	}
	case IVTV_IOC_PREP_FRAME_YUV: {
                struct ivtvyuv_ioctl_dma_host_to_ivtv_args *args = arg;
                int ret = 0;

                IVTV_DEBUG_IOCTL("IVTV_IOC_PREP_FRAME_YUV\n");
                if (ivtv_claim_stream(id, IVTV_DEC_STREAM_TYPE_YUV))
                    return -EBUSY;

                ret = ivtv_yuv_prep_frame(itv, args);

                if (ret) {
                	if (ret == EWOULDBLOCK) {
                        	IVTV_OSD_DEBUG_INFO(
                                           "YUV: DMA xfer dropping frame returning EWOULDBLOCK\n");
                        } else if (ret == -ERESTARTSYS) {
                                IVTV_OSD_DEBUG_INFO(
                                           "YUV: DMA xfer interrupted so restarting\n");
                        } else {
                                IVTV_OSD_DEBUG_WARN(
                                           "YUV: DMA xfer failed\n");
                        }
                }
                return ret;
        }

	case IVTV_IOC_G_YUV_INTERLACE:{
		struct ivtv_ioctl_yuv_interlace *yuv_mode = arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_G_YUV_INTERLACE\n");
                yuv_mode->interlace_mode = itv->yuv_info.lace_mode;
                yuv_mode->threshold = itv->yuv_info.lace_threshold;
		break;
	}
	case IVTV_IOC_S_YUV_INTERLACE:{
		struct ivtv_ioctl_yuv_interlace *yuv_mode = arg;

		IVTV_DEBUG_IOCTL("IVTV_IOC_S_YUV_INTERLACE\n");
                if (yuv_mode->interlace_mode < 0 || yuv_mode->interlace_mode > 2)
                {
                    IVTV_DEBUG_WARN("ivtv ioctl set YUV_INTERLACE invalid mode %d\n",yuv_mode->interlace_mode);
                    return -EINVAL;
                }
                itv->yuv_info.lace_mode = yuv_mode->interlace_mode;
                itv->yuv_info.lace_threshold = yuv_mode->threshold;
		break;
	}
	case IVTV_IOC_G_PTS: {
		u64 *pts = arg;

		*pts = stream->pts;
		break;
	}

	default:
		IVTV_DEBUG_WARN("unknown IVTV command %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}

static int ivtv_v4l2_do_ioctl(struct inode *inode, struct file *filp,
			      unsigned int cmd, void *arg)
{
	struct ivtv_open_id *id = (struct ivtv_open_id *)filp->private_data;
	struct ivtv *itv = id->itv;
	int streamtype = id->type;

	IVTV_DEBUG_IOCTL("v4l2 ioctl 0x%08x\n", cmd);

	switch (cmd) {
	case IVTV_IOC_EVENT_SETUP:
	case IVTV_IOC_RELOAD_FW:
	case IVTV_IOC_G_ITVC_REG:
	case IVTV_IOC_S_ITVC_REG:
	case IVTV_IOC_G_ENCODER_REG:
	case IVTV_IOC_S_ENCODER_REG:
	case IVTV_IOC_G_DECODER_REG:
	case IVTV_IOC_S_DECODER_REG:
	case IVTV_IOC_ZCOUNT:
	case IVTV_IOC_S_MSP_MATRIX:
	case IVTV_IOC_G_DEBUG_LEVEL:
	case IVTV_IOC_S_DEBUG_LEVEL:
	case IVTV_IOC_FWAPI:
	case IVTV_IOC_RESET_IR:
		return ivtv_internal_ioctls(itv, streamtype, cmd, arg);

	case VIDIOC_QUERYCAP:
	case VIDIOC_ENUMINPUT:
	case VIDIOC_G_INPUT:
	case VIDIOC_S_INPUT:
	case VIDIOC_ENUMOUTPUT:
	case VIDIOC_G_OUTPUT:
	case VIDIOC_S_OUTPUT:
	case VIDIOC_G_FMT:
	case VIDIOC_S_FMT:
	case VIDIOC_TRY_FMT:
	case VIDIOC_G_FREQUENCY:
	case VIDIOC_S_FREQUENCY:
	case VIDIOC_ENUMSTD:
	case VIDIOC_G_STD:
	case VIDIOC_S_STD:
	case VIDIOC_S_TUNER:
	case VIDIOC_G_TUNER:
	case VIDIOC_ENUMAUDIO:
	case VIDIOC_S_AUDIO:
	case VIDIOC_G_AUDIO:
	case VIDIOC_ENUMAUDOUT:
	case VIDIOC_S_AUDOUT:
	case VIDIOC_G_AUDOUT:
        case VIDIOC_G_SLICED_VBI_CAP:
        case VIDIOC_G_FBUF:
        case VIDIOC_LOG_STATUS:
		return ivtv_v4l2_ioctls(itv, id, streamtype, cmd, arg);

	case VIDIOC_QUERYMENU:
	case VIDIOC_QUERYCTRL:
	case VIDIOC_S_CTRL:
	case VIDIOC_G_CTRL:
		return ivtv_control_ioctls(itv, cmd, arg);

	case IVTV_IOC_PASSTHROUGH:
	case IVTV_IOC_GET_FB:
	case IVTV_IOC_FRAMESYNC:
	case IVTV_IOC_PLAY:
	case IVTV_IOC_START_DECODE:
	case IVTV_IOC_S_START_DECODE:
	case IVTV_IOC_STOP_DECODE:
	case IVTV_IOC_S_STOP_DECODE:
	case IVTV_IOC_DEC_FLUSH:
	case IVTV_IOC_DEC_STEP:
	case IVTV_IOC_DEC_SPLICE:
	case IVTV_IOC_DEC_FAST_STOP:
	case IVTV_IOC_G_SPEED:
	case IVTV_IOC_S_SPEED:
	case IVTV_IOC_S_SLOW_FAST:
	case IVTV_IOC_PAUSE:
	case IVTV_IOC_GET_TIMING:
	case IVTV_IOC_S_VBI_EMBED:
	case IVTV_IOC_G_VBI_EMBED:
	case IVTV_IOC_S_VBI_PASSTHROUGH:
	case IVTV_IOC_G_VBI_PASSTHROUGH:
	case IVTV_IOC_G_CODEC:
	case IVTV_IOC_S_CODEC:
	case IVTV_IOC_S_GOP_END:
	case IVTV_IOC_PAUSE_ENCODE:
	case IVTV_IOC_RESUME_ENCODE:
        case IVTV_IOC_PREP_FRAME_YUV:
        case IVTV_IOC_G_YUV_INTERLACE:
        case IVTV_IOC_S_YUV_INTERLACE:
        case IVTV_IOC_G_PTS:
                return ivtv_ivtv_ioctls(itv, id, streamtype, cmd, arg);

	case 0x00005401:	/* Handle isatty() calls */
		return -EINVAL;
	default:
		return v4l_compat_translate_ioctl(inode, filp, cmd, arg,
						   ivtv_v4l2_do_ioctl);
	}
	return 0;
}

int ivtv_v4l2_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
		    unsigned long arg)
{
        struct ivtv_open_id *id = (struct ivtv_open_id *)filp->private_data;
        struct ivtv *itv = id->itv;
	int streamtype = id->type;

	switch (cmd) {

		/* This is temporary, I guess, it won't work unless here, so it's
		   here till someone can show me a EOS mpeg captured without it here */
	case VIDIOC_STREAMOFF:
		IVTV_DEBUG_IOCTL("VIDIOC_STREAMOFF\n");
                if (id->open_id != itv->streams[streamtype].id)
                        return -EBUSY;
                ivtv_stream_off(itv, &itv->streams[streamtype]);
		return 0;

	/* Converts old (0.1.9) non-conforming ioctls that were using
	   'just some values I picked for now'. I hoped this would not be
	   necessary, but too many people were already using existing apps
	   (MythTV) written for this version of the driver. */
	case 0xFFEE7703:
		cmd = IVTV_IOC_G_CODEC;
		break;
	case 0xFFEE7704:
		cmd = IVTV_IOC_S_CODEC;
		break;
	case 0xFFEE7781:
		cmd = IVTV_IOC_PLAY;
		break;
	case 0xFFEE7782:
		cmd = IVTV_IOC_PAUSE;
		break;
	case 0xFFEE7783:
		cmd = IVTV_IOC_FRAMESYNC;
		break;
	case 0xFFEE7784:
		cmd = IVTV_IOC_GET_TIMING;
		break;
	case 0xFFEE7785:
		cmd = IVTV_IOC_S_SLOW_FAST;
		break;
	case 0xFFEE7786:
		cmd = IVTV_IOC_S_START_DECODE;
		break;
	case 0xFFEE7787:
		cmd = IVTV_IOC_S_STOP_DECODE;
		break;
	case 0xFFEE7789:
		cmd = IVTV_IOC_GET_FB;
		break;
	case IVTV_IOC_G_DRIVER_INFO:{
		// AEW - put here becuase we can't do this after video_usercopy
		// has done its work of copying a tiny portion of what we really
		// need.
		struct ivtv_driver_info driver_info;
		uint32_t *driver_info_size_ptr = (uint32_t *) arg;
		uint32_t driver_info_size = 0;

		IVTV_DEBUG_IOCTL("IVTV_IOC_G_DRIVER_INFO\n");
		if (driver_info_size_ptr == NULL) {
			IVTV_DEBUG_WARN(
				   "Error: IVTV_IOC_G_DRIVER_INFO called with "
				   "invalid ivtv_driver_info address.\n");
			return -EINVAL;
		}

                memset(&driver_info, 0,
                       sizeof(struct ivtv_driver_info));
                get_user(driver_info_size, driver_info_size_ptr);
                if (driver_info_size > sizeof(struct ivtv_driver_info))
                        driver_info_size = sizeof(struct ivtv_driver_info);

                switch (driver_info_size) {
                case IVTV_DRIVER_INFO_V3_SIZE:
                        driver_info.hw_flags = itv->hw_flags;
                        /* fall through */

                case IVTV_DRIVER_INFO_V2_SIZE:
                        driver_info.cardnr = itv->num;
                        /* fall through */

                case IVTV_DRIVER_INFO_V1_SIZE:
                        driver_info.size = driver_info_size;
                        driver_info.version =
                            IVTV_VERSION_NUMBER(IVTV_VERSION_INFO_NAME);
                        strncpy(driver_info.comment,
                                IVTV_VERSION_COMMENT
                                (IVTV_VERSION_INFO_NAME),
                                IVTV_DRIVER_INFO_MAX_COMMENT_LENGTH);
                        driver_info.
                            comment[IVTV_DRIVER_INFO_MAX_COMMENT_LENGTH
                                    - 1] = '\0';
                        break;
                
                default:
                        IVTV_DEBUG_WARN(
                                   "Error: Unknown size "
                                   "(0x%08x) passed to IVTV_IOC_G_DRIVER_INFO.\n",
                                   driver_info_size);
                        return -EINVAL;
                }

                if (copy_to_user((struct ivtv_driver_info *)arg,
                                 &driver_info,
                                 driver_info_size)) {
                        IVTV_DEBUG_WARN(
                                   "Error: IVTV_IOC_G_DRIVER_INFO Unable "
                                   "to copy data to user space.\n");
                        return -EINVAL;
                }
		return 0;
	}

	case IVTV_IOC_G_STREAM_INFO:{
		// AEW - put here because we can't do this after video_usercopy
		// has done its work of copying a tiny portion of what we really
		// need.
		struct ivtv_stream_info stream_info;
		uint32_t *stream_info_size_ptr = (uint32_t *) arg;
		uint32_t stream_info_size = 0;

		IVTV_DEBUG_IOCTL("IVTV_IOC_G_STREAM_INFO\n");
		if (stream_info_size_ptr == NULL) {
			IVTV_DEBUG_WARN(
				   "Error: IVTV_IOC_G_STREAM_INFO called with "
				   "invalid ivtv_stream_info address.\n");
			return -EINVAL;
		}

                memset(&stream_info, 0,
                       sizeof(struct ivtv_stream_info));
                get_user(stream_info_size, stream_info_size_ptr);
                if (stream_info_size > sizeof(struct ivtv_stream_info))
                        stream_info_size = sizeof(struct ivtv_stream_info);

                switch (stream_info_size) {
                case IVTV_STREAM_INFO_V1_SIZE:
                        stream_info.size = stream_info_size;
                        stream_info.type = streamtype;
                        break;
                
                default:
                        IVTV_DEBUG_WARN(
                                   "Error: Unknown size "
                                   "(0x%08x) passed to IVTV_IOC_G_STREAM_INFO.\n",
                                   stream_info_size);
                        return -EINVAL;
                }

                if (copy_to_user((struct ivtv_stream_info *)arg,
                                 &stream_info,
                                 stream_info_size)) {
                        IVTV_DEBUG_WARN(
                                   "Error: IVTV_IOC_G_STREAM_INFO Unable "
                                   "to copy data to user space.\n");
                        return -EINVAL;
                }
		return 0;
	}
	}
	return video_usercopy(inode, filp, cmd, arg, ivtv_v4l2_do_ioctl);
}
