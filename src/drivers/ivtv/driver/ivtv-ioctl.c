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
#include "ivtv-dma.h"
#include "ivtv-reset.h"
#include "ivtv-mailbox.h"
#include "ivtv-i2c.h"
#include "ivtv-queue.h"
#include "ivtv-fileops.h"
#include "ivtv-vbi.h"
#include "ivtv-audio.h"
#include "ivtv-video.h"
#include "ivtv-streams.h"
#include "ivtv-ioctl.h"
#include "ivtv-controls.h"
#include "ivtv-cards.h"
#include "audiochip.h"
#include "saa7127.h"

/* from v4l1_compat.c */
extern int
ivtv_compat_translate_ioctl(struct inode         *inode,
			   struct file		*file,
			   int			cmd,
			   void			*arg,
			   v4l2_kioctl          drv);


static const struct v4l2_standard ivtv_stds[] = {
	{
	.index		= 0,
	.id		= V4L2_STD_NTSC,
	.name		= "NTSC",
	.frameperiod	= { .numerator	= 1001,
			    .denominator= 30000},
	.framelines	= 525,
	.reserved	= {0,0,0,0}
	}, {
	.index		= 1,
	.id		= V4L2_STD_PAL,
	.name		= "PAL",
	.frameperiod	= { .numerator	= 1,
			    .denominator= 25},
	.framelines	= 625,
	.reserved	= {0,0,0,0}
	}, {
	.index		= 2,
	.id		= V4L2_STD_SECAM,
	.name		= "SECAM",
	.frameperiod	= { .numerator	= 1,
			    .denominator= 25},
	.framelines	= 625,
	.reserved	= {0,0,0,0}
	}
};
static const int ivtv_stds_size = sizeof(ivtv_stds) / sizeof(ivtv_stds[0]);


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
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"DEC: Error with Scale 0x%08x\n", speed->scale);
		return -EINVAL;
	}

	if ((speed->speed < 0) || (speed->speed > 1)) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"DEC: Error with Speed 0x%08x\n", speed->speed);
		return -EINVAL;
	}
		
	if ((speed->fr_mask < 0) || (speed->fr_mask > 2)) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"DEC: Error with Frame Mask 0x%08x\n", speed->fr_mask);
		return -EINVAL;
	}
		
	data[0] = speed->scale;

	if (speed->smooth) /* smooth ff */
		data[0] |= 0x40000000;

	if (speed->speed) /* fast forward */
		data[0] |= 0x80000000;

	if (speed->direction) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"DEC: Warning, Reverse not supported\n");

		/* B Frames when in reverse */
		if (speed->b_per_gop > itv->codec.bframes)
			speed->b_per_gop = itv->codec.bframes;
		else if (speed->b_per_gop < 0)
			speed->b_per_gop = itv->codec.bframes;	
	}

	data[1] = speed->direction; /* Forward. Reverse not supported */
	
	/* Unfortunately we translate and the original i/bp settings conflict */
	switch (speed->fr_mask) {
		case 0:
			data[2] = 1; /* I */
			break;
		case 1:
		case 3:
			data[2] = 3; /* BP */
			break;
		case 2:
		case 7:
		default: 
			data[2] = 7; /* IBP */
			break;
	}
	
	/* Store speed settings, may have to send with dec DMA */
	itv->dec_dma_stat.speed_data[0] = data[0];
	itv->dec_dma_stat.speed_data[1] = data[1];
	itv->dec_dma_stat.speed_data[2] = data[2];
	itv->dec_dma_stat.speed_data[3] = data[3] = speed->b_per_gop;
	itv->dec_dma_stat.speed_data[4] = data[4] = speed->aud_mute; /* mute while fast/slow */
	itv->dec_dma_stat.speed_data[5] = data[5] = speed->fr_field; /* frame or field at a time */
	itv->dec_dma_stat.speed_data[6] = data[6] = speed->mute; /* # of frames to mute on normal speed resume */

	/* If not decoding, just change speed setting */
	if (atomic_read(&itv->decoding) == 0) {
		if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		     	IVTV_API_DEC_PLAYBACK_SPEED, &result, 7, &data[0])) 
		{
			IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"DEC: error changing speed\n");
			return (int)result;
		}

		return 0;
	}

	/* Stop all DMA and decoding activity */
	if (ivtv_vapi(itv, IVTV_API_DEC_PAUSE_PLAYBACK, 1, 0)) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: error pausing\n");
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"DEC: Setting Speed to "
		"0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", 
		data[0], data[1], data[2], data[3], data[4], data[5], data[6]);

	/* Change Speed safely */
	if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
	     IVTV_API_DEC_PLAYBACK_SPEED, &result, 7, &data[0])) 
	{
		IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: error changing speed\n");
		return (int)result;
	}
	/* Save speed options if call succeeded */
	memcpy(&itv->dec_options.speed, &speed, sizeof(speed));

	return 0;
}

static int ivtv_api_show_boxes(struct ivtv_mailbox *mbox, int args, int start, int stop)
{
        int x, i;
        struct ivtv_mailbox *local_box;

        for(x=start;x<=stop;x++) {
                local_box = &mbox[x];
                printk("Mailbox[%d] flags 0x%08x cmd 0x%08x timeout 0x%08x\n",
                                   x, readl(&local_box->flags),
                                   readl(&local_box->cmd),
                                   readl(&local_box->timeout));

                for (i = 0; i < args; i++) {
                        printk(" data[%d] = 0x%08x\n",
                                        i, readl(&local_box->data[i]));
                }
        }
        return 0;
}

unsigned long ivtv_itvc(struct ivtv *itv, unsigned int cmd, void *arg)
{
	struct ivtv_itvc_reg *regs = arg;
	unsigned long flags;
	int retval = 0;	

	spin_lock_irqsave(&ivtv_cards_lock, flags);
	if (cmd == ITVC_GET_REG) {
		if (regs->reg >= 0x02000000 && regs->reg < IVTV_IOREMAP_SIZE)	
        		regs->val = readl(itv->reg_mem +(regs->reg-0x02000000));
		else if (regs->reg >= 0x01000000 && regs->reg < 0x01800000)
			regs->val = readl(itv->dec_mem + regs->reg-0x01000000);
		else if (regs->reg >=0x00000000 && regs->reg < 0x00800000)
			regs->val = readl(itv->enc_mem + regs->reg);
		else {
			IVTV_DEBUG(IVTV_DEBUG_ERR,"itvc_ioctl: Addr Invalid\n");
			retval = -EIO;
			goto itvc_fail;
		}
		spin_unlock_irqrestore(&ivtv_cards_lock, flags);
		return regs->val;
	} else if (cmd == ITVC_SET_REG) {
                printk("Write Register: 0x%08x = 0x%08x\n", 
			regs->reg, regs->val);
		if (regs->reg >= 0x02000000 && regs->reg < IVTV_IOREMAP_SIZE) {
        		writel(regs->val, 
					(regs->reg + (itv->reg_mem-0x02000000)));
		} else if (regs->reg >= 0x01000000 && regs->reg < 0x01800000) {
        		writel(regs->val, 
					(regs->reg + itv->dec_mem-0x01000000));
		} else if (regs->reg >=0x00000000 && regs->reg < 0x00800000) {
        		writel(regs->val, 
					(regs->reg + itv->enc_mem));
		} else {
			IVTV_DEBUG(IVTV_DEBUG_ERR,"itvc_ioctl: Addr Invalid\n");
			retval = -EIO;
			goto itvc_fail;
		}
	}
itvc_fail:	
	spin_unlock_irqrestore(&ivtv_cards_lock, flags);
	return retval;
}

void ivtv_dec_VSYNC(struct ivtv *itv)
{
        u32 *tdata;
        u32 newframe;
        u64 newpts;
        u64 newscr;

        if (atomic_read(&itv->decoding) == 0)
                return;

        tdata = itv->dec_mbox[IVTV_MBOX_FIELD_DISPLAYED].data;
        newframe = readl(&tdata[0]);
        newpts = ((u64)readl(&tdata[2]) << 32) | (u64)(readl(&tdata[1]));
        newscr = ((u64)readl(&tdata[4]) << 32) | (u64)(readl(&tdata[3]));

        /* Return if zeros while decoding */
        if (newframe == 0 && newpts == 0 && newscr == 0)
        {
                return;
        }

        itv->dec_timestamp.frame = newframe;
        itv->dec_timestamp.pts = newpts;
        itv->dec_timestamp.scr = newscr;

        IVTV_DEBUG(IVTV_DEBUG_INFO,
                "DEC VSYNC: frames %d, pts %ld, scr %ld\n",
                itv->dec_timestamp.frame,
                (long int)itv->dec_timestamp.pts,
                (long int)itv->dec_timestamp.scr);
}

int ivtv_internal_ioctls(struct ivtv *itv, int streamtype, unsigned int cmd, void *arg)
{
	switch (cmd) {
	case IVTV_IOC_FWAPI: {
		struct ivtv_ioctl_fwapi *fwapi = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO,"ivtv ioctl: FWAPI\n");

                /* Encoder */
                if(cmd>=128)
                        return ivtv_api(itv, itv->enc_mbox, &itv->enc_msem,
				fwapi->cmd, &fwapi->result, fwapi->args, 
				fwapi->data);

		/* Decoder */
		return ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, fwapi->cmd,
	                      &fwapi->result, fwapi->args, fwapi->data);
	}

        case IVTV_IOC_EVENT_SETUP: {
                struct ivtv_ioctl_event *event = arg;
                int x;
                int mbox;

                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: EVENT_SETUP\n");

                mbox = event->mbox;

                if (213 == event->api.cmd && 1 == event->api.data[1])
                        ivtv_clear_irq_mask(itv, event->api.data[2]);
                else if (213 == event->api.cmd)
                        ivtv_set_irq_mask(itv, event->api.data[2]);

                if (event->type) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR, "sending event to encoder\n");
                        /* Encoder */
                        x = ivtv_api(itv, &itv->enc_mbox[mbox], &itv->enc_msem, event->api.cmd,
                              &event->api.result, event->api.args, event->api.data);
                } else {
                        IVTV_DEBUG(IVTV_DEBUG_ERR, "sending event to decoder\n");
                        /* Decoder */
                        x = ivtv_api(itv, &itv->dec_mbox[mbox], &itv->dec_msem, event->api.cmd,
                              &event->api.result, event->api.args, event->api.data);
                }

		ivtv_api_show_boxes(&itv->dec_mbox[mbox], 4, mbox, mbox);
                return x;
        }

        case IVTV_IOC_RELOAD_FW: {
                int ret = 0;

                IVTV_DEBUG(IVTV_DEBUG_INFO,"ivtv ioctl: Reload Firmware\n");    

		/* Force Full Reload */
                ret = ivtv_reset_card(itv, 1, IVTV_CMD_FULL_RESET); 

                return ret;
        }

	/* ioctls to allow direct access to the ITVC registers for testing */
	case IVTV_IOC_G_ITVC_REG:
		ivtv_itvc(itv, ITVC_GET_REG, arg);
		break;

	case IVTV_IOC_S_ITVC_REG:
                ivtv_itvc(itv, ITVC_SET_REG, arg);
		break;

	/* ioctls to allow direct access to the saa7115 registers for testing */
	case IVTV_IOC_G_SAA7115_REG:
		ivtv_saa7115(itv, DECODER_GET_REG, arg);
		break;

	case IVTV_IOC_S_SAA7115_REG:
                ivtv_saa7115(itv, DECODER_SET_REG, arg);
		break;

	/* ioctls to allow direct access to the saa7127 registers for testing */
	case IVTV_IOC_G_SAA7127_REG:
		ivtv_saa7127(itv, ENCODER_GET_REG, arg);
		break;

	case IVTV_IOC_S_SAA7127_REG:
                ivtv_saa7127(itv, ENCODER_SET_REG, arg);
		break;

	case IVTV_IOC_ZCOUNT: {
		/* Zeroes out usage count so it can be unloaded in case of
		 * drastic error */

		IVTV_DEBUG(IVTV_DEBUG_INFO,"ivtv ioctl: ZCOUNT\n");

#ifndef LINUX26
		while (MOD_IN_USE)
			MOD_DEC_USE_COUNT;

		MOD_INC_USE_COUNT;
#endif
		break;
	}

	case IVTV_IOC_S_MSP_MATRIX: {
		struct ivtv_msp_matrix *matrix = arg;

		return ivtv_audio_set_matrix(itv, matrix);
	}

	case IVTV_IOC_G_DEBUG_LEVEL:
		*(int *)arg = ivtv_debug;
		break;

	case IVTV_IOC_S_DEBUG_LEVEL: 
		ivtv_debug = (*(int *)arg) | IVTV_DEBUG_ERR;
		*(int *)arg = ivtv_debug;
		break;

	default:
                IVTV_DEBUG(IVTV_DEBUG_ERR, "unknown internal IVTV command %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}

int ivtv_v4l2_ioctls(struct ivtv *itv, struct ivtv_open_id *id,
	             int streamtype, unsigned int cmd, void *arg)
{
	struct ivtv_stream *stream = &itv->streams[streamtype];

	switch (cmd) {
	case VIDIOC_QUERYCAP: {
		struct v4l2_capability *vcap = arg;
		memset(vcap,0,sizeof(*vcap));
		
		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: querycap\n");

		/* driver name */
		strcpy(vcap->driver, IVTV_DRIVER_NAME);

		/* card type */
		strcpy(vcap->card, stream->v4l2dev->name);

		/* bus info.. card # will do */
		sprintf(vcap->bus_info, "%d", itv->num);

		/* version */
		vcap->version = IVTV_DRIVER_VERSION;

		/* capabilities */
		vcap->capabilities = itv->v4l2_cap;

		/* reserved.. must set to 0! */
		vcap->reserved[0] = vcap->reserved[1] = vcap->reserved[2] =
			vcap->reserved[3] = 0;
		break;
	}
	
	case VIDIOC_ENUMAUDIO: {
		struct v4l2_audio *vin = arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: enumaudio\n");

		if (ivtv_get_audio_input(itv, vin->index) == NULL) {
			return -EINVAL;
		}

		/* set it to defaults from our table */
		*vin = *ivtv_get_audio_input(itv, vin->index);
		break;
	} 

	case VIDIOC_G_AUDIO: {
		struct v4l2_audio *vin = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: get audio input\n");
		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			vin->index = itv->audio_input_radio;
			/* msp3400 input 2 maps to audio input 0 (tuner) */
			if (vin->index == 2)
				vin->index = 0;
		} else {
			vin->index = itv->audio_input_tv;
		}

		if (ivtv_get_audio_input(itv, vin->index) == NULL)
			return -EINVAL;
		*vin = *ivtv_get_audio_input(itv, vin->index);
		break;
	}

	case VIDIOC_S_AUDIO: {
		struct v4l2_audio *vout = arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: set audio input\n");

                if (ivtv_get_audio_input(itv, vout->index) == NULL)
		       	return -EINVAL;
		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			/* audio input 0 (tuner) maps to msp3400 input 2 for the radio */
			if (vout->index == 0)
			       	vout->index = 2;
			itv->audio_input_radio = vout->index;
		} else {
			itv->audio_input_tv = vout->index;
		}
		ivtv_audio_set_io(itv);
		break;
	}
	
	case VIDIOC_ENUMAUDOUT: {
		struct v4l2_audioout *vin = arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: enumaudioout\n");

		if (ivtv_get_audio_output(itv, vin->index) == NULL) {
			return -EINVAL;
		}

		/* set it to defaults from our table */
		*vin = *ivtv_get_audio_output(itv, vin->index);
		break;
	} 

	case VIDIOC_G_AUDOUT: {
		struct v4l2_audioout *vin = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: get audio output\n");
		vin->index = 0;
		if (ivtv_get_audio_output(itv, vin->index) == NULL)
			return -EINVAL;
		*vin = *ivtv_get_audio_output(itv, vin->index);
		break;
	}

	case VIDIOC_S_AUDOUT: {
		struct v4l2_audioout *vout = arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: set audio input\n");

                if (ivtv_get_audio_output(itv, vout->index) == NULL)
		       	return -EINVAL;
		/* Do nothing. */
		break;
	}
	
	case VIDIOC_ENUMINPUT: {
		struct v4l2_input *vin = arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: enuminput\n");

		if (ivtv_get_input(itv, vin->index) == NULL) {
			return -EINVAL;
		}

		/* set it to defaults from our table */
		*vin = *ivtv_get_input(itv, vin->index);

		/* The tuner is a special case */
		if (vin->type == V4L2_INPUT_TYPE_TUNER)
			vin->std = itv->tuner_std;
		break;
	} 
					       
	case VIDIOC_ENUMOUTPUT: {
		struct v4l2_output *vout = arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: enumoutput\n");

		if (ivtv_get_output(itv, vout->index) == NULL) {
			return -EINVAL;
		}

		/* set it to defaults from our table */
		*vout = *ivtv_get_output(itv, vout->index);

		break;
	} 
					       
	case VIDIOC_TRY_FMT:
	case VIDIOC_S_FMT: {
		struct v4l2_format *fmt = arg;

		if (cmd == VIDIOC_S_FMT) {
			IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: set format\n");
		} else {
			IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: try format\n");
		}

		if (fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			struct video_window wind;

			if ((itv->width != fmt->fmt.pix.width ||
			     itv->height != fmt->fmt.pix.height) &&
			    atomic_read(&itv->capturing) > 0) {
				return -EBUSY;
			}

			if (cmd != VIDIOC_TRY_FMT) {
				/* FIXME: only sets resolution for now */
				wind.width = itv->width = fmt->fmt.pix.width;
				wind.height = itv->height = fmt->fmt.pix.height;
				ivtv_saa7115(itv, DECODER_SET_SIZE, &wind);
		
				if ( ( itv->codec.stream_type == IVTV_STREAM_MPEG1)||
				     ( itv->codec.stream_type == IVTV_STREAM_VCD)){
					  /* this is an MPEG1 stream */
					  IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: set "
						     "format : the current stream_type is MPEG1 "
						     "or VCD, you have to do a S_CODEC after this "
						     "ioctl\n ");

				}
			}
		}
		else if (fmt->type == V4L2_BUF_TYPE_VBI_CAPTURE) {
			if (streamtype == IVTV_ENC_STREAM_TYPE_VBI &&
				itv->vbi_service_set_in && atomic_read(&itv->capturing) > 0) {
				return -EBUSY;
			}
		}
		/* fall through */
	}

	case VIDIOC_G_FMT: {
		struct v4l2_format *fmt = arg;
		int type = fmt->type;
		
		memset(fmt, 0, sizeof(*fmt));
		fmt->type = type;
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: get format\n");

		switch (type) {
			case 0:  /* Works around a bug in MythTV (already fixed).
				    This case should be removed once post 0.1.10
				    versions of ivtv have become mainstream. */
			case V4L2_BUF_TYPE_VIDEO_CAPTURE:
			case V4L2_BUF_TYPE_VIDEO_OUTPUT:
				if (cmd != VIDIOC_TRY_FMT) {
					fmt->fmt.pix.width = itv->width;
					fmt->fmt.pix.height = itv->height;
				}
				fmt->fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
				fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
				if (streamtype == IVTV_ENC_STREAM_TYPE_YUV ||
				    streamtype == IVTV_DEC_STREAM_TYPE_YUV) {
					/* FIXME: these values can't be right
					   fmt->fmt.pix.pixelformat = 0; */
					fmt->fmt.pix.sizeimage = 720 * 720;
				} else {
					fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_MPEG;
					fmt->fmt.pix.sizeimage = 128 * 1024;
				}
				break;
			case V4L2_BUF_TYPE_VBI_CAPTURE:
				fmt->fmt.vbi.sampling_rate = 27000000;
                                fmt->fmt.vbi.offset = 248;
                        	fmt->fmt.vbi.samples_per_line = VBI_RAW_SIZE;
                	        fmt->fmt.vbi.sample_format = V4L2_PIX_FMT_GREY;
        	                fmt->fmt.vbi.start[0] = itv->vbi_start[0];
        	                fmt->fmt.vbi.start[1] = itv->vbi_start[1];
	                        fmt->fmt.vbi.count[0] = fmt->fmt.vbi.count[1] = itv->vbi_count;
				/* If this ioctl is used, then the caller
				   wants RAW format. Ugly hack until
				   the sliced VBI API is in V4L2. */
				itv->vbi_service_set_in = 0;
				break;
			case V4L2_BUF_TYPE_VBI_OUTPUT:
			case V4L2_BUF_TYPE_VIDEO_OVERLAY:
			default:
				return -EINVAL;
		}
		break;
	}

	case VIDIOC_G_INPUT: {
		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: get input\n");

		*(int *)arg = itv->active_input;
		break;
	}

	case VIDIOC_S_INPUT: {
		int inp = *(int *)arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: set input\n");

                if (ivtv_get_input(itv, inp) == NULL)
		       	return -EINVAL;

		if (inp == itv->active_input) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "Input unchanged\n");
		} else {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "Changing input from %d to %d\n",
				   itv->active_input, inp);

			itv->active_input = inp;
			/* Set the audio input to whatever is appropriate for the
			   input type. */
			if (ivtv_get_input(itv, inp)->type == V4L2_INPUT_TYPE_TUNER) {
				itv->audio_input_tv = 0;  /* TV tuner */
			} else {
				itv->audio_input_tv = 1;  /* Line in */
			}

			/* prevent others from messing with the streams until
			   we're finished changing inputs. */
			down(&stream->mlock);	
			mute_and_pause(itv);
			ivtv_saa7115(itv, DECODER_SET_INPUT, &inp);
			/* Select new audio input */
			ivtv_audio_set_io(itv);
			unmute_and_resume(itv, 1);
			up(&stream->mlock);	
		}
		break;
	}

	case VIDIOC_G_OUTPUT: {
		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: get output\n");

		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		*(int *)arg = itv->active_output;
		break;
	}

	case VIDIOC_S_OUTPUT: {
		int outp = *(int *)arg;
		
		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: set output\n");

                if (ivtv_get_output(itv, outp) == NULL)
		       	return -EINVAL;

		if (outp == itv->active_output) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "Output unchanged\n");
			break;
		}
		IVTV_DEBUG(IVTV_DEBUG_INFO, "Changing output from %d to %d\n",
			   itv->active_output, outp);

		itv->active_output = outp;

		ivtv_saa7127(itv, ENCODER_SET_OUTPUT, &outp);
		break;
	}

	case VIDIOC_G_FREQUENCY: {
		struct v4l2_frequency *vf = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: get frequency\n");

		if (vf->tuner != 0)
			return -EINVAL;
		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			vf->type = V4L2_TUNER_RADIO;
			vf->frequency = itv->freq_radio;
		} else {
			vf->type = V4L2_TUNER_ANALOG_TV;
			vf->frequency = itv->freq_tv;
		}
		memset(vf->reserved, 0, sizeof(vf->reserved));
		break;
	}

	case VIDIOC_S_FREQUENCY: {
		struct v4l2_frequency *vf = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: set frequency\n");

		if (vf->tuner != 0)
			return -EINVAL;
		
		/* Set frequency, prevent others from messing around with
		   streams until we're done. */
		down(&stream->mlock);	
		
		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			itv->freq_radio = vf->frequency;
		} else {
			itv->freq_tv = vf->frequency;
		}

		mute_and_pause(itv);
		IVTV_DEBUG(IVTV_DEBUG_INFO, 
			"v4l2 ioctl: set frequency %d\n", vf->frequency);
		ivtv_tuner(itv, VIDIOCSFREQ, &vf->frequency);
		ivtv_audio_freq_changed(itv);
		unmute_and_resume(itv, 1);

		up(&stream->mlock);
		break;
	}

	case VIDIOC_ENUMSTD: {
		struct v4l2_standard *vs = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: enum standard\n");

		if (vs->index < 0 || vs->index >= ivtv_stds_size)
			return -EINVAL;
		
		*vs = ivtv_stds[vs->index];
		break;
	}

	case VIDIOC_G_STD: {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: get standard\n");
		*(v4l2_std_id *)arg = itv->std;
		break;
	}

	case VIDIOC_S_STD: {
		v4l2_std_id std = *(v4l2_std_id *)arg;
                struct video_channel v;
		int norm;
		int x;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: set standard\n");

		for (x = 0; x < ivtv_stds_size; x++) {
			if (ivtv_stds[x].id & std) 
				break;
		}
		if (x == ivtv_stds_size)
			return -EINVAL;

		if (ivtv_stds[x].id == itv->std)
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

		IVTV_DEBUG(IVTV_DEBUG_ERR,
			   "Switching standard to %s.\n", ivtv_stds[x].name);
		itv->std = ivtv_stds[x].id;
		itv->width = 720;
		if (itv->std & V4L2_STD_NTSC) {
			itv->height = 480;
			itv->vbi_count = 12;
			itv->vbi_start[0] = 10;
			itv->vbi_start[1] = 273;
		}
		else {  /* PAL/SECAM */
			itv->height = 576;
			itv->vbi_count = 18;
			itv->vbi_start[0] = 6;
			itv->vbi_start[1] = 318;
		}
		norm = v.norm = std2norm(itv->std);

		/* Tuner */
		ivtv_tuner(itv, VIDIOCSCHAN, &v);

		/* Tuner Audio */
		ivtv_audio_set_channel(itv, &v);

		/* Microtune support */
		if (tda9887 > 0) {
			ivtv_tda9887(itv, AUDC_CONFIG_PINNACLE, &tda9887);
		} else if (tda9887 == 0) {
			ivtv_tda9887(itv, VIDIOCSCHAN, &v);
		}

		/* Digitizer */
		ivtv_saa7115(itv, DECODER_SET_NORM, &norm);

		if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
			/* Encoder */
			ivtv_saa7127(itv, ENCODER_SET_NORM, &norm);
        
			/* set display standard */
		        ivtv_vapi(itv, IVTV_API_DEC_DISP_STANDARD, 1,
				       	!(itv->std & V4L2_STD_NTSC));
		}
		up(&stream->mlock);
		break;
	}

	case VIDIOC_S_TUNER: { /* Setting tuner can only set audio mode */
		struct v4l2_tuner *vt = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO,"v4l2 ioctl: set tuner\n");

		if (vt->index != 0)
			return -EINVAL;

		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
			itv->audmode_radio = vt->audmode;
		} else {
			itv->audmode_tv = vt->audmode;
		}
		ivtv_audio_set_audmode(itv, vt->audmode);
		break;
	}

	case VIDIOC_G_TUNER: {
		struct v4l2_tuner *vt = arg;

		IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl: get tuner\n");

		if (vt->index != 0)
			return -EINVAL;

		vt->afc = 0;
		memset(vt->reserved, 0, sizeof(vt->reserved));

		if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
		        struct video_tuner va;

			strcpy(vt->name, "ivtv Radio Tuner");
			vt->type = V4L2_TUNER_RADIO;
			vt->capability = V4L2_TUNER_CAP_STEREO;
			/* Japan:          76.0 MHz -  89.9 MHz
			   Western Europe: 87.5 MHz - 108.0 MHz
			   Russia:         65.0 MHz - 108.0 MHz */
			vt->rangelow = 65 * 16;
			vt->rangehigh = 108 * 16;
			ivtv_tuner(itv, VIDIOCGTUNER, &va);
			vt->signal = va.signal;
			vt->audmode = itv->audmode_radio;
		} else {
		        int sig = 0;

			strcpy(vt->name, "ivtv TV Tuner");
			vt->type = V4L2_TUNER_ANALOG_TV;
			vt->capability = V4L2_TUNER_CAP_NORM | V4L2_TUNER_CAP_STEREO |
			  		 V4L2_TUNER_CAP_LANG1 | V4L2_TUNER_CAP_LANG2;
			vt->rangelow = 0;
			vt->rangehigh = 0xffffffffUL;

                        ivtv_saa7115(itv, DECODER_GET_STATUS, &sig);
                        vt->signal = (sig & DECODER_STATUS_GOOD) ? 65535 : 0;
			vt->audmode = itv->audmode_tv;
                }
		vt->rxsubchans = ivtv_audio_get_rxsubchans(itv, vt);
		break;
	}

	case VIDIOC_STREAMOFF: {
                IVTV_DEBUG(IVTV_DEBUG_ERR, "Entering STREAMOFF %08x\n", cmd);
		if (id->open_id != stream->id) 
			return -EBUSY;

		/* Prevent others from messing around with streams until
		   we've finished here. */
		set_bit(IVTV_F_S_STREAMOFF, &stream->s_flags);
		ivtv_stop_capture(itv, streamtype);
		break;
	}

	default:
                IVTV_DEBUG(IVTV_DEBUG_ERR, "unknown VIDIOC command %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}

int ivtv_ivtv_ioctls(struct ivtv *itv, struct ivtv_open_id *id,
		     int streamtype, unsigned int cmd, void *arg)
{
	struct ivtv_stream *stream = &itv->streams[streamtype];

	switch (cmd) {
        case IVTV_IOC_PASSTHROUGH: {
		int enable = *(int *)arg;

		return ivtv_passthrough_mode(itv, enable);
        }

	case IVTV_IOC_GET_FB: {
		if (itv->fb_id < 0)
			return -EINVAL;
		*(int *)arg = itv->fb_id;

		break;
	}

        case IVTV_IOC_FRAMESYNC: {
                struct ivtv_ioctl_framesync *fs = arg;

                /* Wait for next field, seems to give us 1 per field */
                interruptible_sleep_on(&itv->vsync_w);
		ivtv_dec_VSYNC(itv);
                *fs = itv->dec_timestamp;

                if (atomic_read(&itv->decoding) == 0) {
                        u32 data[IVTV_MBOX_MAX_DATA], result;

                        memset(fs, 0, sizeof(fs));
                        /* scr should be valid anytime */
                        if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
                                     IVTV_API_DEC_TIMING_INFO, &result, 5, &data[0])) {
                                IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: couldn't read clock\n");
                                return -EIO;
                        }

                        fs->frame = 0;
                        fs->pts   = 0;
                        fs->scr   =  (u64)(((u64)data[4] << 32) | (u64)(data[3]));
                        break;
                }


                if (signal_pending(current))
                        return -EINTR;

                break;
        }

	case IVTV_IOC_PLAY: {
		if (ivtv_vapi(itv, IVTV_API_DEC_START_PLAYBACK, 2, 0, 0))
			IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: error starting playback\n");
		break;

	}

	case IVTV_IOC_START_DECODE:
	case IVTV_IOC_S_START_DECODE: {
		struct ivtv_cfg_start_decode *sd = arg;
		struct ivtv_stream *stream = &itv->streams[IVTV_DEC_STREAM_TYPE_MPG];

		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		if ((sd->gop_offset < 0) || (sd->gop_offset > 15))
			return -EINVAL;
		if  (sd->muted_audio_frames < 0)
			return -EINVAL;
		
		itv->dec_options.gop_offset = sd->gop_offset;
		itv->dec_options.mute_frames = sd->muted_audio_frames;

		if (cmd == IVTV_IOC_S_START_DECODE) break;

		if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags))
			return -EBUSY;

		if (ivtv_claim_stream(id, IVTV_DEC_STREAM_TYPE_MPG)) {
			/* someone else is using this stream already */
			IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"DEC: start_decode, stream already claimed\n");
			return -EBUSY;
		}

		/* stop decoding if already happening */
		if (test_bit(IVTV_F_S_DECODING, &stream->s_flags)) 
		{
			IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"DEC: start_decode, stream already started!!!, stopping\n");
			ivtv_stop_v4l2_decode_stream(itv, IVTV_DEC_STREAM_TYPE_MPG);
		}

		/* Run Work Queue */
		atomic_inc(&itv->dec_dma_stat.intr);
		dec_schedule_work(itv);

		return 0;
        }

	case IVTV_IOC_STOP_DECODE: 
	case IVTV_IOC_S_STOP_DECODE: {
		struct ivtv_cfg_stop_decode *sd = arg;
		struct ivtv_stream *stream = &itv->streams[IVTV_DEC_STREAM_TYPE_MPG];

		if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
			return -EINVAL;
		if ((sd->hide_last < 0) || (sd->hide_last > 1))
			return -EINVAL;
		itv->dec_options.hide_last_frame = sd->hide_last;

		itv->dec_options.pts_low = (u32)(sd->pts_stop & 0xFFFFFFFF);
		itv->dec_options.pts_hi  = (u32)(sd->pts_stop >> 32);

		if (cmd == IVTV_IOC_S_STOP_DECODE) break;

                if (!test_bit(IVTV_F_S_DECODING, &stream->s_flags))
                        break;

		if (id->open_id != stream->id) { 
			IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: STOP_DECODE:: "
				"id->open_id 0x%08x !=stream->id 0x%08x\n",
				id->open_id, (unsigned int)stream->id);
			return -EBUSY;
		}
		
		ivtv_stop_v4l2_decode_stream(itv, IVTV_DEC_STREAM_TYPE_MPG);
		ivtv_flush_queues_lock(itv, IVTV_DEC_STREAM_TYPE_MPG);

		break;
	}

	case IVTV_IOC_DEC_FLUSH: {
		ivtv_flush_queues_lock(itv, IVTV_DEC_STREAM_TYPE_MPG);
		break;
	}

	case IVTV_IOC_DEC_STEP: {
		int howfar = *(int *)arg;

		if (howfar < 0 || howfar > 2)
			return -EINVAL;

		/* howfar: 0 = 1 frame, 1 = top field, 2 = bottom field */
		if (ivtv_vapi(itv, IVTV_API_DEC_STEP_VIDEO, 1, howfar)) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: error stepping\n");
		}
		break;
	}

	case IVTV_IOC_G_SPEED: {
	        *(struct ivtv_speed *)arg = itv->dec_options.speed;
		break;
	}

	case IVTV_IOC_S_SPEED: {
		struct ivtv_speed *speed = arg;
		int ret;

		ret = ivtv_s_speed(itv, speed);

		if (ret) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: error in slow/fast mode\n");
			return ret;
		}

		break;
	}

	case IVTV_IOC_S_SLOW_FAST: {
		struct ivtv_slow_fast *sf = arg;
		struct ivtv_speed speed;
		int ret;

		if ((sf->scale < 0) || (sf->scale > 50))
			return -EINVAL;
		if ((sf->speed < 0) || (sf->speed > 1))
			return -EINVAL;
		
		speed = itv->dec_options.speed;
		speed.scale = sf->scale;
		speed.speed = sf->speed;

		ret = ivtv_s_speed(itv, &speed);

		if (ret) {
			IVTV_DEBUG(IVTV_DEBUG_ERR,
				   "DEC: error in set slow/fast mode\n");
			return ret;
		}

		break;
	}

	case IVTV_IOC_PAUSE: {
		/* We really don't want to get stuck inbetween DMA calls */
		if (ivtv_vapi(itv, IVTV_API_DEC_PAUSE_PLAYBACK, 1, 0)) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: error pausing\n");
		}

		break;
	}

        case IVTV_IOC_GET_TIMING: {
                struct ivtv_ioctl_framesync *timing = arg;
                u32 data[IVTV_MBOX_MAX_DATA], result;

                if (atomic_read(&itv->decoding) == 0) {
                        memset(timing, 0, sizeof(timing));
                        /* scr should be valid anytime */
                        if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
                                     IVTV_API_DEC_TIMING_INFO, &result, 5, &data[0])) {
                                IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: couldn't read clock\n");
                                return -EIO;
                        }

                        timing->frame = 0;
                        timing->pts   = 0;
                        timing->scr   =  (u64)(((u64)data[4] << 32) | (u64)(data[3]));

                } else {
                        if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
                                     IVTV_API_DEC_TIMING_INFO, &result, 5, &data[0])) {
                                IVTV_DEBUG(IVTV_DEBUG_ERR, "DEC: couldnt read clock\n");
                                return -EIO;
                        }
                        timing->frame   = data[0];
                        timing->pts     = (u64)((u64)data[2] << 32) | (u64)data[1];
                        timing->scr     = (u64)(((u64)data[4] << 32) | (u64)(data[3]));
                }
                break;
        }

	case IVTV_IOC_S_VBI_MODE: {
		struct ivtv_sliced_vbi_format *fmt = arg;

                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: Set VBI mode\n");

		fmt->packet_size = 8 + 44;
		fmt->io_size = fmt->packet_size * 36;
		fmt->reserved = 0;

	        /* The VBI mode of the MPEG embedded VBI stream is fixed */	
		if (streamtype == IVTV_DEC_STREAM_TYPE_VBI) {
			fmt->service_set = IVTV_SLICED_VPS | IVTV_SLICED_CAPTION_525 |
				IVTV_SLICED_CAPTION_625 | IVTV_SLICED_WSS_625 |
				IVTV_SLICED_TELETEXT_B;
			break;
		}

		/* Only a subset of the available sliced data types is available
		   for the VBI output stream (only those supported by the saa7127). */
		if (streamtype == IVTV_DEC_STREAM_TYPE_VOUT) {
			fmt->service_set &= (IVTV_SLICED_VPS | IVTV_SLICED_CAPTION_525 |
				         IVTV_SLICED_CAPTION_625 | IVTV_SLICED_WSS_625);
			itv->vbi_service_set_out = fmt->service_set;
			break;
		}
		if (itv->vbi_service_set_in != fmt->service_set &&
			 atomic_read(&itv->capturing) > 0) {
			return -EBUSY;
		}
		itv->vbi_service_set_in = fmt->service_set;
		break;
	}

	case IVTV_IOC_G_VBI_MODE: {
		struct ivtv_sliced_vbi_format *fmt = arg;

                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: Get VBI mode\n");    
		fmt->packet_size = 8 + 44;
		fmt->io_size = fmt->packet_size * 36;
		fmt->reserved = 0;

		if (streamtype == IVTV_DEC_STREAM_TYPE_VBI) {
			fmt->service_set = IVTV_SLICED_VPS | IVTV_SLICED_CAPTION_525 |
				IVTV_SLICED_CAPTION_625 | IVTV_SLICED_WSS_625 |
				IVTV_SLICED_TELETEXT_B;
		}
		else if (streamtype == IVTV_DEC_STREAM_TYPE_VOUT) {
			fmt->service_set = itv->vbi_service_set_out;
		}
		else {
			fmt->service_set = itv->vbi_service_set_in;
		}
		break;
	}
        
	case IVTV_IOC_S_VBI_EMBED: {
                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: Select VBI MPEG embedding mode\n");    
		itv->vbi_insert_mpeg = *(int *)arg;
		break;
        }

	case IVTV_IOC_G_VBI_EMBED: {
                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: Get VBI MPEG embedding mode\n");    
		*(int *)arg = itv->vbi_insert_mpeg;
		break;
        }

	case IVTV_IOC_S_VBI_PASSTHROUGH: {
                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: Select VBI passthrough mode\n");    
		itv->vbi_passthrough = *(int *)arg;
		break;
        }

	case IVTV_IOC_G_VBI_PASSTHROUGH: {
                IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv ioctl: Get VBI passthrough mode\n");    
		*(int *)arg = itv->vbi_passthrough;
		break;
        }

	case IVTV_IOC_G_CODEC: {
		struct ivtv_ioctl_codec *codec = arg;

		*codec = itv->codec;
        	codec->framerate = itv->codec.framerate;
        	codec->framespergop = itv->codec.framespergop;
		break;
	}

	case IVTV_IOC_S_CODEC: {
		struct ivtv_ioctl_codec *codec = arg;
		struct video_window wind;

		/* FIXME: insert abundant parameter validation here */
		if((codec->bitrate == 0) || (codec->bitrate_peak == 0) ||
		   (codec->bitrate > codec->bitrate_peak) ) {
			IVTV_DEBUG(IVTV_DEBUG_ERR,"ivtv ioctl: set "
				   "bitrate=%u < peak=%u: failed\n",
				   codec->bitrate, codec->bitrate_peak);
			return -EINVAL;
		} else {

		  /* check if it is an MPEG1 stream */
		if ( ( codec->stream_type == IVTV_STREAM_MPEG1)||
		     ( codec->stream_type == IVTV_STREAM_VCD)){
		  /* this is an MPEG1 stream */
  		  int tmp_height = (itv->std & V4L2_STD_NTSC) ? 480 : 576;
		  
		  /* set vertical mpeg encoder resolution */
		  if ( itv->height != tmp_height/2 ){
		    itv->height = tmp_height/2;
		  }
		  /* mpeg1 is cbr */
		  codec->bitrate_mode = 1;



		  IVTV_DEBUG(IVTV_DEBUG_INFO,"ivtv ioctl: set codec: "
			     "stream_type is MPEG1 or VCD. resolution %dx%d.\n",itv->width,itv->height);
		  

		    /* fix videodecoder resolution */
		  wind.width = itv->width;
		  wind.height = tmp_height;
		  ivtv_saa7115(itv, DECODER_SET_SIZE, &wind);
	
		}

	  		/* Passed the garbage check */
	  		itv->codec = *codec;
		}

		ivtv_audio_set_bitrate(itv, codec->audio_bitmask & 0x03);
                break;
	}

	case IVTV_IOC_S_GOP_END: { 
		itv->end_gop = *(int *)arg;
		*(int *)arg = itv->end_gop;
		break;
	}

        case IVTV_IOC_PAUSE_ENCODE :{
                down(&stream->mlock);
                mute_and_pause ( itv );
                up(&stream->mlock);
                break;
        }

        case IVTV_IOC_RESUME_ENCODE :{
                down(&stream->mlock);
                unmute_and_resume ( itv,1 );
                up(&stream->mlock);
                break;
        }

	default:
                IVTV_DEBUG(IVTV_DEBUG_ERR, "unknown IVTV command %08x\n", cmd);
		return -EINVAL;
	}
	return 0;
}

static int ivtv_v4l2_do_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
		                void *arg)
{
        struct ivtv_open_id *id = (struct ivtv_open_id *)filp->private_data;
        struct ivtv *itv = id->itv;
	int streamtype = id->type;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 ioctl 0x%08x\n", cmd);

	switch (cmd) {
        case IVTV_IOC_EVENT_SETUP:
        case IVTV_IOC_RELOAD_FW:
	case IVTV_IOC_G_ITVC_REG:
	case IVTV_IOC_S_ITVC_REG:
	case IVTV_IOC_G_SAA7115_REG:
	case IVTV_IOC_S_SAA7115_REG:
	case IVTV_IOC_G_SAA7127_REG:
	case IVTV_IOC_S_SAA7127_REG:
	case IVTV_IOC_ZCOUNT:
	case IVTV_IOC_S_MSP_MATRIX:
	case IVTV_IOC_G_DEBUG_LEVEL:
	case IVTV_IOC_S_DEBUG_LEVEL:
	case IVTV_IOC_FWAPI:
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
	/*case VIDIOC_STREAMOFF:*/
	case VIDIOC_ENUMAUDIO:
	case VIDIOC_S_AUDIO:
	case VIDIOC_G_AUDIO:
	case VIDIOC_ENUMAUDOUT:
	case VIDIOC_S_AUDOUT:
	case VIDIOC_G_AUDOUT:
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
	case IVTV_IOC_G_SPEED: 
	case IVTV_IOC_S_SPEED:
	case IVTV_IOC_S_SLOW_FAST:
	case IVTV_IOC_PAUSE:
	case IVTV_IOC_GET_TIMING:
	case IVTV_IOC_S_VBI_MODE:
	case IVTV_IOC_G_VBI_MODE:
	case IVTV_IOC_S_VBI_EMBED:
	case IVTV_IOC_G_VBI_EMBED:
	case IVTV_IOC_S_VBI_PASSTHROUGH:
	case IVTV_IOC_G_VBI_PASSTHROUGH:
	case IVTV_IOC_G_CODEC:
	case IVTV_IOC_S_CODEC:
	case IVTV_IOC_S_GOP_END: 
        case IVTV_IOC_PAUSE_ENCODE:
        case IVTV_IOC_RESUME_ENCODE:
		return ivtv_ivtv_ioctls(itv, id, streamtype, cmd, arg);

	case 0x00005401: /* Handle isatty() calls */
		return -EINVAL;
	default:
		return ivtv_compat_translate_ioctl(inode, filp, cmd, arg, ivtv_v4l2_do_ioctl);
	}
	return 0;
}

int ivtv_v4l2_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
		                unsigned long arg)
{
       	/* Converts old (0.1.9) non-conforming ioctls that were using
	   'just some values I picked for now'. I hoped this would not be
	   necessary, but too many people were already using existing apps
	   (MythTV) written for this version of the driver. */
	switch (cmd) {

	/* This is temporary, I guess, it won't work unless here, so it's
           here till someone can show me a EOS mpeg captured without it here */
        case VIDIOC_STREAMOFF: {
        	struct ivtv_open_id *id = (struct ivtv_open_id *)filp->private_data;
        	struct ivtv *itv = id->itv;
        	int streamtype = id->type;

                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "Entering Early STREAMOFF %08x\n", cmd);
                if (id->open_id != itv->streams[streamtype].id)
                        return -EBUSY;

                /* Prevent others from messing around with streams until
                   we've finished here. */
                set_bit(IVTV_F_S_STREAMOFF, &itv->streams[streamtype].s_flags);
                ivtv_stop_capture(itv, streamtype);
                return 0;
        }

		case 0xFFEE7703: cmd = IVTV_IOC_G_CODEC; break;
		case 0xFFEE7704: cmd = IVTV_IOC_S_CODEC; break;
		case 0xFFEE7781: cmd = IVTV_IOC_PLAY; break;
		case 0xFFEE7782: cmd = IVTV_IOC_PAUSE; break;
		case 0xFFEE7783: cmd = IVTV_IOC_FRAMESYNC; break;
		case 0xFFEE7784: cmd = IVTV_IOC_GET_TIMING; break;
		case 0xFFEE7785: cmd = IVTV_IOC_S_SLOW_FAST; break;
		case 0xFFEE7786: cmd = IVTV_IOC_S_START_DECODE; break;
		case 0xFFEE7787: cmd = IVTV_IOC_S_STOP_DECODE; break;
		case 0xFFEE7789: cmd = IVTV_IOC_GET_FB; break;
#ifdef V4L2_NO_COMPAT
		// Only needed for V4L2_NO_COMPAT, otherwise it is handled by
		// the V4L2 video_usercopy.
#define VIDIOC_OVERLAY_OLD	_IOWR ('V', 14, int)
#define VIDIOC_S_PARM_OLD	_IOW  ('V', 22, struct v4l2_streamparm)
#define VIDIOC_S_CTRL_OLD	_IOW  ('V', 28, struct v4l2_control)
#define VIDIOC_G_AUDIO_OLD	_IOWR ('V', 33, struct v4l2_audio)
#define VIDIOC_G_AUDOUT_OLD	_IOWR ('V', 49, struct v4l2_audioout)
		case VIDIOC_S_CTRL_OLD:   cmd = VIDIOC_S_CTRL; break;
		case VIDIOC_OVERLAY_OLD:  cmd = VIDIOC_OVERLAY; break;
		case VIDIOC_S_PARM_OLD:   cmd = VIDIOC_S_PARM; break;
		case VIDIOC_G_AUDIO_OLD:  cmd = VIDIOC_G_AUDIO; break;
		case VIDIOC_G_AUDOUT_OLD: cmd = VIDIOC_G_AUDOUT; break;
#endif
		case IVTV_IOC_G_DRIVER_INFO: {
			// AEW - put here becuase we can't do this after video_usercopy
			// has done its work of copying a tiny portion of what we really
			// need.
			struct ivtv_driver_info driver_info;
			uint32_t *driver_info_size_ptr = (uint32_t *) arg;
			uint32_t driver_info_size = 0;
			int request_valid = 1;

			if (driver_info_size_ptr) {
				memset(&driver_info, 0, sizeof(struct ivtv_driver_info));
				get_user(driver_info_size, driver_info_size_ptr);
				switch (driver_info_size) {
					case IVTV_DRIVER_INFO_V1_SIZE: {
						driver_info.size = driver_info_size;
						driver_info.version = IVTV_VERSION_NUMBER(
								IVTV_VERSION_INFO_NAME);
						strncpy(driver_info.comment,
								IVTV_VERSION_COMMENT(IVTV_VERSION_INFO_NAME),
								IVTV_DRIVER_INFO_MAX_COMMENT_LENGTH);
						driver_info.comment
								[IVTV_DRIVER_INFO_MAX_COMMENT_LENGTH - 1] = '\0';
						break;
					}
					default: {
						request_valid = 0;
						IVTV_DEBUG(IVTV_DEBUG_ERR, "Error: Unknown size "
								"(0x%08x) passed to IVTV_IOC_G_DRIVER_INFO.", driver_info_size);
						return -EINVAL;
					}
				}

				if (request_valid) {
					if (copy_to_user((struct ivtv_driver_info *) arg,
									&driver_info, driver_info_size)) {
						IVTV_DEBUG(IVTV_DEBUG_ERR,
								"Error: IVTV_IOC_G_DRIVER_INFO Unable "
								"to copy data to user space.\n");
						return -EINVAL;
					}
				}
			} else {
				IVTV_DEBUG(IVTV_DEBUG_ERR,
						"Error: IVTV_IOC_G_DRIVER_INFO called with "
						"invalid ivtv_driver_info address.\n");
				return -EINVAL;
			}
			return 0;
		}
	}
	return video_usercopy(inode, filp, cmd, arg, ivtv_v4l2_do_ioctl);
}
