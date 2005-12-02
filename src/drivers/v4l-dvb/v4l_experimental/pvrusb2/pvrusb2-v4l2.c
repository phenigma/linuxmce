/*
 *
 *  $Id: pvrusb2-v4l2.c,v 1.2 2005/11/29 14:10:44 mchehab Exp $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *  Copyright (C) 2004 Aurelien Alleaume <slts@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "compat.h"
#include <linux/kernel.h>
#include <linux/videodev.h>

#include "pvrusb2-context.h"
#include "pvrusb2-hdw.h"
#include "pvrusb2.h"
#include "pvrusb2-debug.h"
#include "pvrusb2-v4l2.h"
#include "pvrusb2-ioread.h"
#include <linux/videodev.h>


#define PVR_WIDTH_DVD      720
#define PVR_WIDTH_SVCD     480
#define PVR_WIDTH_VCD      352

#define PVR_HEIGHT_PAL      480
#define PVR_HEIGHT_NTSC     576

struct pvr2_v4l2_dev;
struct pvr2_v4l2_fh;
struct pvr2_v4l2;

struct pvr2_v4l2_dev {
	struct pvr2_v4l2 *v4lp;
	struct video_device *vdev;
	struct pvr2_context_stream *stream;
	enum pvr2_config config;
};

struct pvr2_v4l2_fh {
	struct pvr2_channel channel;
	struct pvr2_v4l2_dev *dev_info;
	enum v4l2_priority prio;
	struct pvr2_ioread *rhp;
	struct file *file;
	struct pvr2_v4l2 *vhead;
	struct pvr2_v4l2_fh *vnext;
	struct pvr2_v4l2_fh *vprev;
	wait_queue_head_t wait_data;
	int fw_mode_flag;
	struct semaphore sem;
};

struct pvr2_v4l2 {
	struct pvr2_channel channel;
	struct pvr2_v4l2_fh *vfirst;
	struct pvr2_v4l2_fh *vlast;

	struct v4l2_prio_state prio;

	/* streams */
	struct pvr2_v4l2_dev video_dev;
};

static int video_nr[PVR_NUM] = {[0 ... PVR_NUM-1] = -1};
module_param_array(video_nr, int, NULL, 0444);
MODULE_PARM_DESC(video_nr, "Offset for device's minor");

#define V4L2_CID_PVR_SRATE          (V4L2_CID_PRIVATE_BASE)
#define V4L2_CID_PVR_AUDIOBITRATE   (V4L2_CID_PRIVATE_BASE+1)
#define V4L2_CID_PVR_AUDIOCRC       (V4L2_CID_PRIVATE_BASE+2)
#define V4L2_CID_PVR_AUDIOEMPHASIS  (V4L2_CID_PRIVATE_BASE+3)
#define V4L2_CID_PVR_VBR            (V4L2_CID_PRIVATE_BASE+4)
#define V4L2_CID_PVR_VIDEOBITRATE   (V4L2_CID_PRIVATE_BASE+5)
#define V4L2_CID_PVR_VIDEOPEAK      (V4L2_CID_PRIVATE_BASE+6)
#define V4L2_CID_PVR_VIDEOSTANDARD  (V4L2_CID_PRIVATE_BASE+7)
#define V4L2_CID_PVR_INPUT          (V4L2_CID_PRIVATE_BASE+8)
#define V4L2_CID_PVR_AUDIOMODE      (V4L2_CID_PRIVATE_BASE+9)
#define V4L2_CID_PVR_FREQUENCY      (V4L2_CID_PRIVATE_BASE+10)
#define V4L2_CID_PVR_HRES           (V4L2_CID_PRIVATE_BASE+11)
#define V4L2_CID_PVR_VRES           (V4L2_CID_PRIVATE_BASE+12)

#define V4L2_CID_PVR_MAX            (V4L2_CID_PRIVATE_BASE+12)

struct v4l2_capability pvr_capability ={
	.driver         = "pvrusb2",
	.card           = "Hauppauge WinTV pvr-usb2",
	.bus_info       = "usb",
	.version        = KERNEL_VERSION(0,8,0),
	.capabilities   = (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VBI_CAPTURE |
			   V4L2_CAP_TUNER | V4L2_CAP_AUDIO |
			   V4L2_CAP_READWRITE),
	.reserved       = {0,0,0,0}
};

static struct v4l2_tuner pvr_v4l2_tuners[]= {
	{
		.index      = 0,
		.name       = "TV Tuner",
		.type           = V4L2_TUNER_ANALOG_TV,
		.capability     = (V4L2_TUNER_CAP_NORM |
				   V4L2_TUNER_CAP_STEREO |
				   V4L2_TUNER_CAP_LANG1 |
				   V4L2_TUNER_CAP_LANG2),
		.rangelow   = 0,
		.rangehigh  = 0,
		.rxsubchans     = V4L2_TUNER_SUB_STEREO,
		.audmode        = V4L2_TUNER_MODE_STEREO,
		.signal         = 0,
		.afc            = 0,
		.reserved       = {0,0,0,0}
	}
#ifdef notdef
	{
		.index    = 1,
		.name   = "Radio Tuner",
		.type             = V4L2_TUNER_RADIO,
		.capability       = (V4L2_TUNER_CAP_STEREO),
		.rangelow   = 0,
		.rangehigh  = 0,
		.rxsubchans       = 0,
		.audmode  = V4L2_TUNER_MODE_STEREO,
		.signal           = 0,
		.afc              = 0,
		.reserved = {0,0,0,0}
	}
#endif
};

struct v4l2_standard pvr_standards[] = {
	[PVR2_CVAL_VIDEOSTANDARD_PAL_BG] = {
		.id             = V4L2_STD_PAL_BG,
		.frameperiod    =
		{
			.numerator  = 1,
			.denominator= 25
		},
		.framelines     = 625,
		.reserved       = {0,0,0,0}
	},
	[PVR2_CVAL_VIDEOSTANDARD_PAL_I] = {
		.id             = V4L2_STD_PAL_I,
		.frameperiod    =
		{
			.numerator  = 1,
			.denominator= 25
		},
		.framelines     = 625,
		.reserved       = {0,0,0,0}
	},
	[PVR2_CVAL_VIDEOSTANDARD_PAL_DK] = {
		.id             = V4L2_STD_PAL_DK,
		.frameperiod    =
		{
			.numerator  = 1,
			.denominator= 25
		},
		.framelines     = 625,
		.reserved       = {0,0,0,0}
	},
	[PVR2_CVAL_VIDEOSTANDARD_SECAM_L] = {
		.id             = V4L2_STD_SECAM,
		.frameperiod    =
		{
			.numerator  = 1,
			.denominator= 25
		},
		.framelines     = 625,
		.reserved       = {0,0,0,0}
	},
	[PVR2_CVAL_VIDEOSTANDARD_NTSC_M] = {
		.id             = V4L2_STD_NTSC,
		.frameperiod    =
		{
			.numerator =  1001,
			.denominator= 30000
		},
		.framelines     = 525,
		.reserved       = {0,0,0,0}
	},
	[PVR2_CVAL_VIDEOSTANDARD_PAL_M] = {
		.id             = V4L2_STD_PAL_M,
		.frameperiod    =
		{
			.numerator =  1001,
			.denominator= 30000
		},
		.framelines     = 525,
		.reserved       = {0,0,0,0}
	}
};

struct v4l2_fmtdesc pvr_fmtdesc [] = {
	{
		.index          = 0,
		.type           = V4L2_BUF_TYPE_VIDEO_CAPTURE,
		.flags          = V4L2_FMT_FLAG_COMPRESSED,
		.description    = "MPEG1/2",
		// This should really be V4L2_PIX_FMT_MPEG, but xawtv
		// breaks when I do that.
		.pixelformat    = 0, // V4L2_PIX_FMT_MPEG,
		.reserved       = { 0, 0, 0, 0 }
	}
};

#define PVR_FORMAT_PIX  0
#define PVR_FORMAT_VBI  1

struct v4l2_format pvr_format [] = {
	[PVR_FORMAT_PIX] = {
		.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE,
		.fmt    = {
			.pix        = {
				.width          = 720,
				.height             = 576,
				// This should really be V4L2_PIX_FMT_MPEG,
				// but xawtv breaks when I do that.
				.pixelformat    = 0, // V4L2_PIX_FMT_MPEG,
				.field          = V4L2_FIELD_INTERLACED,
				.bytesperline   = 0,  // doesn't make sense
						      // here
				//FIXME : Don't know what to put here...
				.sizeimage          = (32*1024),
				.colorspace     = 0, // doesn't make sense here
				.priv           = 0
			}
		}
	},
	[PVR_FORMAT_VBI] = {
		.type   = V4L2_BUF_TYPE_VBI_CAPTURE,
		.fmt    = {
			.vbi        = {
				.sampling_rate = 27000000,
				.offset = 248,
				.samples_per_line = 1443,
				.sample_format = V4L2_PIX_FMT_GREY,
				.start = { 0, 0 },
				.count = { 0, 0 },
				.flags = 0,
				.reserved = { 0, 0 }
			}
		}
	}
};

static int cnv_cid_v4l2_pvr2(int id)
{
	switch (id) {
	case V4L2_CID_BRIGHTNESS:
		return PVR2_CID_BRIGHTNESS;
	case V4L2_CID_SATURATION:
		return PVR2_CID_SATURATION;
	case V4L2_CID_CONTRAST:
		return PVR2_CID_CONTRAST;
	case V4L2_CID_HUE:
		return PVR2_CID_HUE;
	case V4L2_CID_AUDIO_VOLUME:
		return PVR2_CID_VOLUME;
	case V4L2_CID_AUDIO_BALANCE:
		return PVR2_CID_BALANCE;
	case V4L2_CID_AUDIO_BASS:
		return PVR2_CID_BASS;
	case V4L2_CID_AUDIO_TREBLE:
		return PVR2_CID_TREBLE;
	case V4L2_CID_AUDIO_MUTE:
		return PVR2_CID_MUTE;
	case V4L2_CID_PVR_SRATE:
		return PVR2_CID_SRATE;
	case V4L2_CID_PVR_AUDIOBITRATE:
		return PVR2_CID_AUDIOBITRATE;
	case V4L2_CID_PVR_AUDIOCRC:
		return PVR2_CID_AUDIOCRC;
	case V4L2_CID_PVR_AUDIOEMPHASIS:
		return PVR2_CID_AUDIOEMPHASIS;
	case V4L2_CID_PVR_VBR:
		return PVR2_CID_VBR;
	case V4L2_CID_PVR_VIDEOBITRATE:
		return PVR2_CID_AVERAGEVIDEOBITRATE;
	case V4L2_CID_PVR_VIDEOPEAK:
		return PVR2_CID_PEAKVIDEOBITRATE;
	case V4L2_CID_PVR_INPUT:
		return PVR2_CID_INPUT;
	case V4L2_CID_PVR_AUDIOMODE:
		return PVR2_CID_AUDIOMODE;
	case V4L2_CID_PVR_FREQUENCY:
		return PVR2_CID_FREQUENCY;
	case V4L2_CID_PVR_HRES:
		return PVR2_CID_HRES;
	case V4L2_CID_PVR_VRES:
		return PVR2_CID_VRES;
	}
	return -1;
}

#ifdef notdef
static int cnv_cid_pvr2_v4l2(int id)
{
	switch (id) {
	case PVR2_CID_BRIGHTNESS:
		return V4L2_CID_BRIGHTNESS;
	case PVR2_CID_SATURATION:
		return V4L2_CID_SATURATION;
	case PVR2_CID_CONTRAST:
		return V4L2_CID_CONTRAST;
	case PVR2_CID_HUE:
		return V4L2_CID_HUE;
	case PVR2_CID_VOLUME:
		return V4L2_CID_AUDIO_VOLUME;
	case PVR2_CID_BALANCE:
		return V4L2_CID_AUDIO_BALANCE;
	case PVR2_CID_BASS:
		return V4L2_CID_AUDIO_BASS;
	case PVR2_CID_TREBLE:
		return V4L2_CID_AUDIO_TREBLE;
	case PVR2_CID_MUTE:
		return V4L2_CID_AUDIO_MUTE;

		return id + V4L2_CID_PRIVATE_BASE;
	}
	return -1;
}
#endif

static const char *get_cmd_code(int cmd)
{
	switch (cmd) {
	case VIDIOC_QUERYCAP:
		return "VIDIOC_QUERYCAP";
	case VIDIOC_G_PRIORITY:
		return "VIDIOC_G_PRIORITY";
	case VIDIOC_S_PRIORITY:
		return "VIDIOC_S_PRIORITY";
	case VIDIOC_ENUMSTD:
		return "VIDIOC_ENUMSTD";
	case VIDIOC_G_STD:
		return "VIDIOC_G_STD";
	case VIDIOC_S_STD:
		return "VIDIOC_S_STD";
	case VIDIOC_ENUMINPUT:
		return "VIDIOC_ENUMINPUT";
	case VIDIOC_G_INPUT:
		return "VIDIOC_G_INPUT";
	case VIDIOC_S_INPUT:
		return "VIDIOC_S_INPUT";
	case VIDIOC_ENUMAUDIO:
		return "VIDIOC_ENUMAUDIO";
	case VIDIOC_G_AUDIO:
		return "VIDIOC_G_AUDIO";
	case VIDIOC_S_AUDIO:
		return "VIDIOC_S_AUDIO";
	case VIDIOC_G_TUNER:
		return "VIDIOC_G_TUNER";
	case VIDIOC_S_TUNER:
		return "VIDIOC_S_TUNER";
	case VIDIOC_S_FREQUENCY:
		return "VIDIOC_S_FREQUENCY";
	case VIDIOC_G_FREQUENCY:
		return "VIDIOC_G_FREQUENCY";
	case VIDIOC_ENUM_FMT:
		return "VIDIOC_ENUM_FMT";
	case VIDIOC_G_FMT:
		return "VIDIOC_G_FMT";
	case VIDIOC_TRY_FMT:
		return "VIDIOC_TRY_FMT";
	case VIDIOC_S_FMT:
		return "VIDIOC_S_FMT";
	case VIDIOC_QBUF:
		return "VIDIOC_QBUF";
	case VIDIOC_QUERYBUF:
		return "VIDIOC_QUERYBUF";
	case VIDIOC_DQBUF:
		return "VIDIOC_DQBUF";
	case VIDIOC_REQBUFS:
		return "VIDIOC_REQBUFS";
	case VIDIOC_STREAMON:
		return "VIDIOC_STREAMON";
	case VIDIOC_STREAMOFF:
		return "VIDIOC_STREAMOFF";
	case VIDIOC_QUERYCTRL:
		return "VIDIOC_QUERYCTRL";
	case VIDIOC_QUERYMENU:
		return "VIDIOC_QUERYMENU";
	case VIDIOC_G_CTRL:
		return "VIDIOC_G_CTRL";
	case VIDIOC_S_CTRL:
		return "VIDIOC_S_CTRL";
	case VIDIOCGMBUF:
		return "VIDIOCGMBUF";
	default :
		break;
	}
	return "unknown";
}


/*
 * pvr_ioctl()
 *
 * This is part of Video 4 Linux API. The procedure handles ioctl() calls.
 *
 */
static int pvr2_v4l2_do_ioctl(struct inode *inode, struct file *file,
                              unsigned int cmd, void *arg)
{
	struct pvr2_v4l2_fh *fh = file->private_data;
	struct pvr2_v4l2 *vp = fh->vhead;
	struct pvr2_v4l2_dev *dev_info = fh->dev_info;
	struct pvr2_hdw *hdw = fh->channel.mc_head->hdw;
	int ret = -EINVAL;

	pvr2_trace(PVR2_TRACE_V4LIOCTL,
		   "pvr2_v4l2_do_ioctl cmd=%s (%d)",get_cmd_code(cmd),cmd);

	if (!pvr2_hdw_dev_ok(hdw)) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "ioctl failed - bad or no context");
		return -EFAULT;
	}

	/* check priority */
	switch (cmd) {
	case VIDIOC_S_CTRL:
	case VIDIOC_S_STD:
	case VIDIOC_S_INPUT:
	case VIDIOC_S_TUNER:
	case VIDIOC_S_FREQUENCY:
		ret = v4l2_prio_check(&vp->prio, &fh->prio);
		if (ret)
			return ret;
	}

	switch (cmd) {
	case VIDIOC_QUERYCAP:
	{
		struct v4l2_capability *cap = arg;

		memcpy(cap, &pvr_capability, sizeof(struct v4l2_capability));

		ret = 0;
		break;
	}

	case VIDIOC_G_PRIORITY:
	{
		enum v4l2_priority *p = arg;

		*p = v4l2_prio_max(&vp->prio);
		ret = 0;
		break;
	}

	case VIDIOC_S_PRIORITY:
	{
		enum v4l2_priority *prio = arg;

		ret = v4l2_prio_change(&vp->prio, &fh->prio, *prio);
		break;
	}

	case VIDIOC_ENUMSTD:
	{

		struct v4l2_standard *vs = (struct v4l2_standard *)arg;
		int idx = vs->index;

		if ((vs->index < PVR2_CVAL_VIDEOSTANDARD_MIN) ||
		    (vs->index > PVR2_CVAL_VIDEOSTANDARD_MAX)) {
			break;
		}

		memcpy(vs, &pvr_standards[idx], sizeof(struct v4l2_standard));
		vs->index = idx;
		strlcpy(vs->name,
			pvr2_hdw_get_ctl_value_name(hdw,
						    PVR2_CID_VIDEOSTANDARD,
						    idx),
			sizeof(vs->name));

		ret = 0;
		break;
	}

	case VIDIOC_G_STD:
	{
		v4l2_std_id *vs = (v4l2_std_id *)arg;

		switch (pvr2_hdw_get_ctl_value(hdw,PVR2_CID_VIDEOSTANDARD)) {
		default:
		case PVR2_CVAL_VIDEOSTANDARD_NTSC_M:
			*vs = V4L2_STD_NTSC;
			break;
		case PVR2_CVAL_VIDEOSTANDARD_PAL_M:
			*vs = V4L2_STD_PAL_M;
			break;
		case PVR2_CVAL_VIDEOSTANDARD_SECAM_L:
			*vs = V4L2_STD_SECAM;
			break;
		case PVR2_CVAL_VIDEOSTANDARD_PAL_BG:
			*vs = V4L2_STD_PAL_BG;
			break;
		case PVR2_CVAL_VIDEOSTANDARD_PAL_I:
			*vs = V4L2_STD_PAL_I;
			break;
		case PVR2_CVAL_VIDEOSTANDARD_PAL_DK:
			*vs = V4L2_STD_PAL_DK;
			break;
		}
		ret = 0;
		break;
	}

	case VIDIOC_S_STD:
	{
		v4l2_std_id *vs = (v4l2_std_id *)arg;
		int val = PVR2_CVAL_VIDEOSTANDARD_NTSC_M;

		if (*vs & V4L2_STD_NTSC){
			val = PVR2_CVAL_VIDEOSTANDARD_NTSC_M;
		} else if (*vs & V4L2_STD_PAL_BG){
			val = PVR2_CVAL_VIDEOSTANDARD_PAL_BG;
		} else if (*vs & V4L2_STD_PAL_I){
			val = PVR2_CVAL_VIDEOSTANDARD_PAL_I;
		} else if (*vs & V4L2_STD_PAL_DK){
			val = PVR2_CVAL_VIDEOSTANDARD_PAL_DK;
		} else if (*vs & V4L2_STD_SECAM){
			val = PVR2_CVAL_VIDEOSTANDARD_SECAM_L;
		} else if (*vs & V4L2_STD_PAL_M){
			val = PVR2_CVAL_VIDEOSTANDARD_PAL_M;
		}

		pvr2_hdw_set_ctl_value(hdw,PVR2_CID_VIDEOSTANDARD,val);

		break;
	}

	case VIDIOC_ENUMINPUT:
	{
		struct v4l2_input *vi = (struct v4l2_input *)arg;
		struct v4l2_input tmp;

		if ((vi->index > PVR2_CVAL_INPUT_MAX) ||
		    (vi->index < PVR2_CVAL_INPUT_MIN)) {
			break;
		}

		memset(&tmp,0,sizeof(tmp));
		tmp.index = vi->index;
		switch (vi->index) {
		case PVR2_CVAL_INPUT_TV:
		case PVR2_CVAL_INPUT_RADIO:
			tmp.type = V4L2_INPUT_TYPE_TUNER;
			break;
		case PVR2_CVAL_INPUT_SVIDEO:
		case PVR2_CVAL_INPUT_COMPOSITE:
			tmp.type = V4L2_INPUT_TYPE_CAMERA;
			break;
		}

		strlcpy(tmp.name,
			pvr2_hdw_get_ctl_value_name(hdw,PVR2_CID_INPUT,
						    vi->index),
			sizeof(tmp.name));

		/* Don't bother with audioset, since this driver currently
		   always switches the audio whenever the video is
		   switched. */

		/* Handling std is a tougher problem.  It doesn't make
		   sense in cases where a device might be multi-standard.
		   We could just copy out the current value for the
		   standard, but it can change over time.  For now just
		   leave it zero. */

		memcpy(vi, &tmp, sizeof(tmp));

		ret = 0;
		break;
	}

	case VIDIOC_G_INPUT:
	{
		struct v4l2_input *vi = (struct v4l2_input *)arg;
		vi->index = pvr2_hdw_get_ctl_value(hdw,PVR2_CID_INPUT);
		ret = 0;
		break;
	}

	case VIDIOC_S_INPUT:
	{
		struct v4l2_input *vi = (struct v4l2_input *)arg;
		ret = 0;
		if (pvr2_hdw_set_ctl_value(hdw,PVR2_CID_INPUT,vi->index)) {
			ret = -EINVAL;
		}
		break;
	}

	case VIDIOC_ENUMAUDIO:
	{
		ret = -EINVAL;
		break;
	}

	case VIDIOC_G_AUDIO:
	{
		ret = -EINVAL;
		break;
	}

	case VIDIOC_S_AUDIO:
	{
		ret = -EINVAL;
		break;
	}
	case VIDIOC_G_TUNER:
	{
		struct v4l2_tuner *vt = (struct v4l2_tuner *)arg;
		unsigned int status_mask;
		if (vt->index !=0) break;

		status_mask = pvr2_hdw_get_signal_status(hdw);

		memcpy(vt, &pvr_v4l2_tuners[vt->index],
		       sizeof(struct v4l2_tuner));

		vt->signal = 0;
		if (status_mask & PVR2_SIGNAL_OK) {
			if (status_mask & PVR2_SIGNAL_STEREO) {
				vt->rxsubchans = V4L2_TUNER_SUB_STEREO;
			} else {
				vt->rxsubchans = V4L2_TUNER_SUB_MONO;
			}
			if (status_mask & PVR2_SIGNAL_SAP) {
				vt->rxsubchans |= (V4L2_TUNER_SUB_LANG1 |
						   V4L2_TUNER_SUB_LANG2);
			}
			vt->signal = 65535;
		}

		switch (pvr2_hdw_get_ctl_value(hdw,PVR2_CID_AUDIOMODE)) {
		case PVR2_CVAL_AUDIOMODE_MONO:
			vt->audmode = V4L2_TUNER_MODE_MONO;
			break;
		case PVR2_CVAL_AUDIOMODE_STEREO:
			vt->audmode = V4L2_TUNER_MODE_STEREO;
			break;
		case PVR2_CVAL_AUDIOMODE_LANG1:
			vt->audmode = V4L2_TUNER_MODE_LANG1;
			break;
		case PVR2_CVAL_AUDIOMODE_LANG2:
			vt->audmode = V4L2_TUNER_MODE_LANG2;
			break;
		case PVR2_CVAL_AUDIOMODE_SAP:
			vt->audmode = V4L2_TUNER_MODE_SAP;
			break;
		}

		ret = 0;
		break;
	}

	case VIDIOC_S_TUNER:
	{
		struct v4l2_tuner *vt=(struct v4l2_tuner *)arg;
		int val = PVR2_CVAL_AUDIOMODE_STEREO;

		if (vt->index != 0)
			break;

		switch (vt->audmode) {
		case V4L2_TUNER_MODE_MONO:
			val = PVR2_CVAL_AUDIOMODE_MONO;
			break;
		case V4L2_TUNER_MODE_STEREO:
			val = PVR2_CVAL_AUDIOMODE_STEREO;
			break;
		case V4L2_TUNER_MODE_LANG1:
			val = PVR2_CVAL_AUDIOMODE_LANG1;
			break;
		case V4L2_TUNER_MODE_SAP: // Also LANG2
			val = PVR2_CVAL_AUDIOMODE_SAP;
			break;
		}

		pvr2_hdw_set_ctl_value(hdw,PVR2_CID_AUDIOMODE,val);
		ret = 0;
	}

	case VIDIOC_S_FREQUENCY:
	{
		const struct v4l2_frequency *vf = (struct v4l2_frequency *)arg;

		pvr2_hdw_set_ctl_value(hdw,PVR2_CID_FREQUENCY,
				       vf->frequency * 62500);

		ret = 0;
		break;
	}

	case VIDIOC_G_FREQUENCY:
	{
		struct v4l2_frequency *vf = (struct v4l2_frequency *)arg;
		int val;

		val = pvr2_hdw_get_ctl_value(hdw,PVR2_CID_FREQUENCY);

		val /= 62500;
		vf->frequency = val;

		ret = 0;
		break;
	}

	case VIDIOC_ENUM_FMT:
	{
		struct v4l2_fmtdesc *fd = (struct v4l2_fmtdesc *)arg;

		/* Only one format is supported : mpeg.*/
		if (fd->index != 0)
			break;

		memcpy(fd, pvr_fmtdesc, sizeof(struct v4l2_fmtdesc));
		ret = 0;
		break;
	}

	case VIDIOC_G_FMT:
	{
		struct v4l2_format *vf = (struct v4l2_format *)arg;

		switch(vf->type) {
		case V4L2_BUF_TYPE_VIDEO_CAPTURE:
			memcpy(vf, &pvr_format[PVR_FORMAT_PIX],
			       sizeof(struct v4l2_format));
			vf->fmt.pix.width =
				pvr2_hdw_get_ctl_value(hdw,PVR2_CID_HRES);
			if (pvr2_hdw_get_ctl_value(hdw,PVR2_CID_INTERLACE)) {
				vf->fmt.pix.width /= 2;
			}
			vf->fmt.pix.height =
				pvr2_hdw_get_ctl_value(hdw,PVR2_CID_VRES);
			ret = 0;
			break;
		case V4L2_BUF_TYPE_VBI_CAPTURE:
			// ????? Still need to figure out to do VBI correctly
			ret = -EINVAL;
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	}

	case VIDIOC_TRY_FMT:
	case VIDIOC_S_FMT:
	{
		struct v4l2_format *vf = (struct v4l2_format *)arg;

		ret = 0;
		switch(vf->type) {
		case V4L2_BUF_TYPE_VIDEO_CAPTURE: {
			int h = vf->fmt.pix.height;
			int w = vf->fmt.pix.width;
			int vd_std, hf, hh;

			vd_std  = pvr2_hdw_get_ctl_value(hdw,
						PVR2_CID_VIDEOSTANDARD);
			switch (vd_std) {
			case PVR2_CVAL_VIDEOSTANDARD_NTSC_M:
			case PVR2_CVAL_VIDEOSTANDARD_PAL_M:
				hf=480;
				break;
			default:
				hf=576;
			}
			hh = (int) (hf / 2);

			memcpy(vf, &pvr_format[PVR_FORMAT_PIX],
			       sizeof(struct v4l2_format));
			if (w > 720)
				vf->fmt.pix.width = 720;
			vf->fmt.pix.width &= 0xff0;
			vf->fmt.pix.height = (h > hh) ? hf : hh;

			if (cmd == VIDIOC_S_FMT){
				pvr2_hdw_set_ctl_value(
					hdw,PVR2_CID_HRES,
					vf->fmt.pix.width);
				pvr2_hdw_set_ctl_value(
					hdw,PVR2_CID_VRES,
					vf->fmt.pix.height);
				pvr2_hdw_set_ctl_value(
					hdw,PVR2_CID_INTERLACE,
					(vf->fmt.pix.height != hf));
			}
		} break;
		case V4L2_BUF_TYPE_VBI_CAPTURE:
			// ????? Still need to figure out to do VBI correctly
			ret = -EINVAL;
			break;
		default:
			ret = -EINVAL;
			break;
		}
		break;
	}

	case VIDIOC_STREAMON:
	{
		ret = pvr2_hdw_set_stream_type(hdw,dev_info->config);
		if (ret < 0) return ret;
		ret = pvr2_hdw_set_streaming(hdw,!0);
		break;
	}

	case VIDIOC_STREAMOFF:
	{
		ret = pvr2_hdw_set_streaming(hdw,0);
		break;
	}

	case VIDIOC_QUERYCTRL:
	{
		struct v4l2_queryctrl *vc = (struct v4l2_queryctrl *)arg;
		int pvr2_id = cnv_cid_v4l2_pvr2(vc->id);
		if (pvr2_id < 0) {
			ret = -EINVAL;
			break;
		}

		if (pvr2_hdw_get_ctl_value_name(hdw,pvr2_id,0)) {
			vc->type = V4L2_CTRL_TYPE_MENU;
		} else {
			vc->type = V4L2_CTRL_TYPE_INTEGER;
		}
		strlcpy(vc->name,pvr2_hdw_get_ctl_name(hdw,pvr2_id),
			sizeof(vc->name));
		vc->minimum = pvr2_hdw_get_ctl_min_value(hdw,pvr2_id);
		vc->maximum = pvr2_hdw_get_ctl_max_value(hdw,pvr2_id);
		vc->step = 1;
		ret = 0;
		break;
	}

	case VIDIOC_QUERYMENU:
	{
		struct v4l2_querymenu *vm = (struct v4l2_querymenu *)arg;
		int pvr2_id = cnv_cid_v4l2_pvr2(vm->id);
		const char *value_name;
		if (pvr2_id < 0) {
			ret = -EINVAL;
			break;
		}

		value_name = pvr2_hdw_get_ctl_value_name(hdw,pvr2_id,
							 vm->index);
		if (value_name) {
			strlcpy(vm->name,value_name,sizeof(vm->name));
			ret = 0;
		} else {
			ret = -EINVAL;
		}

		break;
	}

	case VIDIOC_G_CTRL:
	{
		struct v4l2_control *vc = (struct v4l2_control *)arg;
		int pvr2_id;

		pvr2_id = cnv_cid_v4l2_pvr2(vc->id);
		if (pvr2_id < 0) {
			ret = -EINVAL;
			break;
		}
		ret = 0;
		vc->value = pvr2_hdw_get_ctl_value(hdw,pvr2_id);
		break;
	}

	case VIDIOC_S_CTRL:
	{
		struct v4l2_control *vc = (struct v4l2_control *)arg;
		int pvr2_id;

		pvr2_id = cnv_cid_v4l2_pvr2(vc->id);
		if (pvr2_id < 0) {
			ret = -EINVAL;
			break;
		}

		ret = pvr2_hdw_set_ctl_value(hdw,pvr2_id,vc->value);
		break;
	}

	default :
		ret = v4l_compat_translate_ioctl(inode,file,cmd,
						 arg,pvr2_v4l2_do_ioctl);
	}

	pvr2_hdw_commit_ctl(hdw);

	if (ret < 0) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "pvr2_v4l2_do_ioctl failure, ret=%d (0x%x)",
			   ret,ret);
	} else {
		pvr2_trace(PVR2_TRACE_V4LIOCTL,
			   "pvr2_v4l2_do_ioctl complete, ret=%d (0x%x)",
			   ret,ret);
	}
	return ret;
}


static void pvr2_v4l2_dev_destroy(struct pvr2_v4l2_dev *dip)
{
	pvr2_trace(PVR2_TRACE_INIT,
		   "Unregistering v4l video device (%s, minor=%d)",
		   pvr2_config_get_name(dip->config),dip->vdev->minor);
	video_unregister_device(dip->vdev);
}


static void pvr2_v4l2_destroy_no_lock(struct pvr2_v4l2 *vp)
{
	pvr2_v4l2_dev_destroy(&vp->video_dev);

	pvr2_trace(PVR2_TRACE_STRUCT,"Destroying pvr2_v4l2 id=%p",vp);
	pvr2_channel_done(&vp->channel);
	kfree(vp);
}


void pvr2_v4l2_internal_check(struct pvr2_channel *chp)
{
	struct pvr2_v4l2 *vp;
	vp = container_of(chp,struct pvr2_v4l2,channel);
	if (!vp->channel.mc_head->disconnect_flag) return;
	if (vp->vfirst) return;
	pvr2_v4l2_destroy_no_lock(vp);
}


int pvr2_v4l2_ioctl(struct inode *inode, struct file *file,
                    unsigned int cmd, unsigned long arg)
{

/* Temporary hack : use ivtv api until a v4l2 one is available. */
#define IVTV_IOC_G_CODEC        0xFFEE7703
#define IVTV_IOC_S_CODEC        0xFFEE7704
	if (cmd == IVTV_IOC_G_CODEC || cmd == IVTV_IOC_S_CODEC) return 0;
	return video_usercopy(inode, file, cmd, arg, pvr2_v4l2_do_ioctl);
}


int pvr2_v4l2_release(struct inode *inode, struct file *file)
{
	struct pvr2_v4l2_fh *fhp = file->private_data;
	struct pvr2_v4l2 *vp = fhp->vhead;
	struct pvr2_context *mp = fhp->vhead->channel.mc_head;

	pvr2_trace(PVR2_TRACE_OPEN_CLOSE,"pvr2_v4l2_release");

	if (fhp->rhp) {
		struct pvr2_stream *sp;
		struct pvr2_hdw *hdw;
		hdw = fhp->channel.mc_head->hdw;
		pvr2_hdw_set_streaming(hdw,0);
		sp = pvr2_ioread_get_stream(fhp->rhp);
		if (sp) pvr2_stream_set_callback(sp,0,0);
		pvr2_ioread_destroy(fhp->rhp);
		fhp->rhp = 0;
	}
	v4l2_prio_close(&vp->prio, &fhp->prio);
	file->private_data = NULL;

	pvr2_hdw_v4l_store_minor_number(fhp->channel.mc_head->hdw,-1);

	pvr2_context_enter(mp); do {
		if (fhp->vnext) {
			fhp->vnext->vprev = fhp->vprev;
		} else {
			vp->vlast = fhp->vprev;
		}
		if (fhp->vprev) {
			fhp->vprev->vnext = fhp->vnext;
		} else {
			vp->vfirst = fhp->vnext;
		}
		fhp->vnext = 0;
		fhp->vprev = 0;
		fhp->vhead = 0;
		pvr2_channel_done(&fhp->channel);
		pvr2_trace(PVR2_TRACE_STRUCT,
			   "Destroying pvr_v4l2_fh id=%p",fhp);
		kfree(fhp);
		if (vp->channel.mc_head->disconnect_flag && !vp->vfirst) {
			pvr2_v4l2_destroy_no_lock(vp);
		}
	} while (0); pvr2_context_exit(mp);
	return 0;
}


int pvr2_v4l2_open(struct inode *inode, struct file *file)
{
	struct video_device *vdev = video_devdata(file);
	struct pvr2_v4l2_dev *dip =
		(struct pvr2_v4l2_dev *)video_get_drvdata(vdev);
	struct pvr2_v4l2_fh *fhp;
	struct pvr2_v4l2 *vp = dip->v4lp;
	struct pvr2_hdw *hdw = vp->channel.hdw;

	pvr2_trace(PVR2_TRACE_OPEN_CLOSE,"pvr2_v4l2_open");

	if (!pvr2_hdw_dev_ok(hdw)) {
		pvr2_trace(PVR2_TRACE_OPEN_CLOSE,
			   "pvr2_v4l2_open: hardware not ready");
		return -EIO;
	}

	fhp = kmalloc(sizeof(*fhp),GFP_KERNEL);
	if (!fhp) {
		return -ENOMEM;
	}
	memset(fhp,0,sizeof(*fhp));

	init_MUTEX(&fhp->sem);
	init_waitqueue_head(&fhp->wait_data);
	fhp->dev_info = dip;

	pvr2_context_enter(vp->channel.mc_head); do {
		pvr2_trace(PVR2_TRACE_STRUCT,"Creating pvr_v4l2_fh id=%p",fhp);
		pvr2_channel_init(&fhp->channel,vp->channel.mc_head);
		fhp->vnext = 0;
		fhp->vprev = vp->vlast;
		if (vp->vlast) {
			vp->vlast->vnext = fhp;
		} else {
			vp->vfirst = fhp;
		}
		vp->vlast = fhp;
		fhp->vhead = vp;
	} while (0); pvr2_context_exit(vp->channel.mc_head);

	fhp->file = file;
	file->private_data = fhp;
	v4l2_prio_open(&vp->prio,&fhp->prio);

	fhp->fw_mode_flag = pvr2_hdw_cpufw_get_enabled(hdw);

	return 0;
}


static void pvr2_v4l2_notify(struct pvr2_v4l2_fh *fhp)
{
	wake_up(&fhp->wait_data);
}


static int pvr2_v4l2_iosetup(struct pvr2_v4l2_fh *fh)
{
	int ret;
	struct pvr2_stream *sp;
	struct pvr2_hdw *hdw;
	if (fh->rhp) return 0;

	/* First read() attempt.  Try to claim the stream and start
	   it... */
	if ((ret = pvr2_channel_claim_stream(&fh->channel,
					     fh->dev_info->stream)) != 0) {
		/* Someone else must already have it */
		return ret;
	}

	fh->rhp = pvr2_ioread_create();
	if (!fh->rhp) {
		pvr2_channel_claim_stream(&fh->channel,0);
		return -ENOMEM;
	}

	hdw = fh->channel.mc_head->hdw;
	sp = fh->dev_info->stream->stream;
	pvr2_ioread_setup(fh->rhp,sp);
	pvr2_stream_set_callback(sp,(pvr2_stream_callback)pvr2_v4l2_notify,fh);
	pvr2_hdw_set_stream_type(hdw,fh->dev_info->config);
	pvr2_hdw_set_streaming(hdw,!0);
	ret = pvr2_ioread_set_enabled(fh->rhp,!0);

	return ret;
}


static ssize_t pvr2_v4l2_read(struct file *file,
                              char __user *buff, size_t count, loff_t *ppos)
{
	struct pvr2_v4l2_fh *fh = file->private_data;
	int ret;

	if (fh->fw_mode_flag) {
		struct pvr2_hdw *hdw = fh->channel.mc_head->hdw;
		char *tbuf;
		int c1,c2;
		int tcnt = 0;
		unsigned int offs = *ppos;

		tbuf = kmalloc(PAGE_SIZE,GFP_KERNEL);
		if (!tbuf) return -ENOMEM;

		while (count) {
			c1 = count;
			if (c1 > PAGE_SIZE) c1 = PAGE_SIZE;
			c2 = pvr2_hdw_cpufw_get(hdw,offs,tbuf,c1);
			if (c2 < 0) {
				tcnt = c2;
				break;
			}
			if (!c2) break;
			copy_to_user(buff,tbuf,c2);
			offs += c2;
			tcnt += c2;
			buff += c2;
			count -= c2;
			*ppos += c2;
		}
		kfree(tbuf);
		return tcnt;
	}

	if (!fh->rhp) {
		ret = pvr2_v4l2_iosetup(fh);
		if (ret) {
			return ret;
		}
	}

	for (;;) {
		ret = pvr2_ioread_read(fh->rhp,buff,count);
		if (ret >= 0) break;
		if (ret != -EAGAIN) break;
		if (file->f_flags & O_NONBLOCK) break;
		/* Doing blocking I/O.  Wait here. */
		ret = wait_event_interruptible(
			fh->wait_data,
			pvr2_ioread_avail(fh->rhp) >= 0);
		if (ret < 0) break;
	}

	return ret;
}


static unsigned int pvr2_v4l2_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	struct pvr2_v4l2_fh *fh = file->private_data;
	int ret;

	if (fh->fw_mode_flag) {
		mask |= POLLIN | POLLRDNORM;
		return mask;
	}

	if (!fh->rhp) {
		ret = pvr2_v4l2_iosetup(fh);
		if (ret) return POLLERR;
	}

	poll_wait(file,&fh->wait_data,wait);

	if (pvr2_ioread_avail(fh->rhp) >= 0) {
		mask |= POLLIN | POLLRDNORM;
	}

	return mask;
}


static struct file_operations vdev_fops = {
	.owner      = THIS_MODULE,
	.open       = pvr2_v4l2_open,
	.release    = pvr2_v4l2_release,
	.read       = pvr2_v4l2_read,
	.ioctl      = pvr2_v4l2_ioctl,
	.llseek     = no_llseek,
	.poll       = pvr2_v4l2_poll,
};


#define VID_HARDWARE_PVRUSB2    38  /* FIXME : need a good value */

static struct video_device vdev_template = {
	.owner      = THIS_MODULE,
	.type       = VID_TYPE_CAPTURE | VID_TYPE_TUNER,
	.type2      = (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_VBI_CAPTURE
		       | V4L2_CAP_TUNER | V4L2_CAP_AUDIO
		       | V4L2_CAP_READWRITE),
	.hardware   = VID_HARDWARE_PVRUSB2,
	.fops       = &vdev_fops,
};


static void pvr2_v4l2_dev_init(struct pvr2_v4l2_dev *dip,
                               struct pvr2_v4l2 *vp,
                               enum pvr2_config cfg)
{
#ifdef notdef
	struct usb_device *usbdev;
#endif
	int mindevnum;
	int unit_number;
	int v4l_type;
	dip->v4lp = vp;
	dip->config = cfg;

#ifdef notdef
	usbdev = pvr2_hdw_get_dev(vp->channel.mc_head->hdw);
#endif

	switch (cfg) {
	case pvr2_config_mpeg:
		v4l_type = VFL_TYPE_GRABBER;
		dip->stream = &vp->channel.mc_head->video_stream;
		break;
	case pvr2_config_vbi:
		v4l_type = VFL_TYPE_VBI;
		break;
	case pvr2_config_radio:
		v4l_type = VFL_TYPE_RADIO;
		break;
	default:
		/* Bail out (this should be impossible) */
		err("Failed to set up pvrusb2 v4l dev"
		    " due to unrecognized config");
		return;
	}

	if (!dip->stream) {
		err("Failed to set up pvrusb2 v4l dev"
		    " due to missing stream instance");
		return;
	}

	dip->vdev = video_device_alloc();
	if (!dip->vdev) {
		err("Alloc of pvrusb2 v4l video device failed");
		return;
	}

	memcpy(dip->vdev,&vdev_template,sizeof(vdev_template));
#ifdef notdef
	/* ????? This relation may be problematic on a disconnect.  Is this
	   really needed?  I can't seem to find a reason for it.  This
	   can't be a required thing - what if the video device being set
	   up doesn't have a real hardware device under it? */
	dip->vdev->dev = &usbdev->dev;
#endif
	dip->vdev->release = video_device_release;
	video_set_drvdata(dip->vdev,dip);

	mindevnum = -1;
	unit_number = pvr2_hdw_get_unit_number(vp->channel.mc_head->hdw);
	if ((unit_number >= 0) && (unit_number < PVR_NUM)) {
		mindevnum = video_nr[unit_number];
	}
	if ((video_register_device(dip->vdev, v4l_type, mindevnum) < 0) &&
	    (video_register_device(dip->vdev, v4l_type, -1) < 0)) {
		err("Failed to register pvrusb2 v4l video device");
	} else {
		pvr2_trace(PVR2_TRACE_INIT,
			   "Registered pvrusb2 v4l device, minor=%d",
			   dip->vdev->minor);
	}
	pvr2_hdw_v4l_store_minor_number(vp->channel.mc_head->hdw,
					dip->vdev->minor);
}


struct pvr2_v4l2 *pvr2_v4l2_create(struct pvr2_context *mnp)
{
	struct pvr2_v4l2 *vp;

	vp = kmalloc(sizeof(*vp),GFP_KERNEL);
	if (!vp) return vp;
	memset(vp,0,sizeof(*vp));
	pvr2_channel_init(&vp->channel,mnp);
	pvr2_trace(PVR2_TRACE_STRUCT,"Creating pvr2_v4l2 id=%p",vp);

	vp->channel.check_func = pvr2_v4l2_internal_check;

	/* register streams */
	pvr2_v4l2_dev_init(&vp->video_dev,vp,pvr2_config_mpeg);


	return vp;
}

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
