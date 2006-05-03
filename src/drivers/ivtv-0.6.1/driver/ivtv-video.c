/*
    saa7127 interface functions
    Copyright (C) 2004  Hans Verkuil <hverkuil@xs4all.nl>

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
#include "ivtv-video.h"
#include "ivtv-i2c.h"
#include "ivtv-gpio.h"
#include "ivtv-cards.h"

static int ivtv_set_video_for_gpio(struct ivtv *itv, u32 video_type)
{
	return ivtv_gpio(itv, VIDIOC_S_INPUT, &video_type);
}

void ivtv_set_vps(struct ivtv *itv, int enabled, u8 vps1, u8 vps2, u8 vps3,
		  u8 vps4, u8 vps5)
{
	struct v4l2_sliced_vbi_data data;

	if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
		return;
	data.id = V4L2_SLICED_VPS;
	data.field = 0;
	data.line = enabled ? 16 : 0;
	data.data[4] = vps1;
	data.data[10] = vps2;
	data.data[11] = vps3;
	data.data[12] = vps4;
	data.data[13] = vps5;
	ivtv_saa7127(itv, VIDIOC_INT_S_VBI_DATA, &data);
}

void ivtv_set_cc(struct ivtv *itv, int mode, u8 cc1, u8 cc2, u8 cc3, u8 cc4)
{
	struct v4l2_sliced_vbi_data data;

	if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
		return;
	data.id = V4L2_SLICED_CAPTION_525;
	data.field = 0;
	data.line = (mode & 1) ? 21 : 0;
	data.data[0] = cc1;
	data.data[1] = cc2;
	ivtv_saa7127(itv, VIDIOC_INT_S_VBI_DATA, &data);
	data.field = 1;
	data.line = (mode & 2) ? 21 : 0;
	data.data[0] = cc3;
	data.data[1] = cc4;
	ivtv_saa7127(itv, VIDIOC_INT_S_VBI_DATA, &data);
}

void ivtv_set_wss(struct ivtv *itv, int enabled, int mode)
{
	struct v4l2_sliced_vbi_data data;

	if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
		return;
	/* When using a 50 Hz system, always turn on the
	   wide screen signal with 4x3 ratio as the default.
	   Turning this signal on and off can confuse certain
	   TVs. As far as I can tell there is no reason not to
	   transmit this signal. */
	if ((itv->std & V4L2_STD_625_50) && !enabled) {
		enabled = 1;
		mode = 0x08;  /* 4x3 full format */
	}
	data.id = V4L2_SLICED_WSS_625;
	data.field = 0;
	data.line = enabled ? 23 : 0;
	data.data[0] = mode & 0xff;
	data.data[1] = (mode >> 8) & 0xff;
	ivtv_saa7127(itv, VIDIOC_INT_S_VBI_DATA, &data);
}

void ivtv_encoder_enable(struct ivtv *itv, int enabled)
{
	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
		ivtv_saa7127(itv, enabled ? VIDIOC_STREAMON : VIDIOC_STREAMOFF,
			       	&enabled);
	}
}

void ivtv_video_set_io(struct ivtv *itv)
{
	int inp = itv->active_input;
	int decoder_input_num;
	u32 type;

	decoder_input_num = itv->card->video_inputs[inp].video_input;
	itv->video_dec_func(itv, VIDIOC_S_INPUT, &decoder_input_num);

	type = itv->card->video_inputs[inp].video_type;

	if (type == IVTV_CARD_INPUT_VID_TUNER) {
		type = 0;  /* Tuner */
	} else if (type < IVTV_CARD_INPUT_COMPOSITE1) {
		type = 2;  /* S-Video */
	} else {
		type = 1;  /* Composite */
	}

	if (itv->card->hw_video & IVTV_HW_GPIO) 
		ivtv_set_video_for_gpio(itv,type);

	if (itv->card->hw_video & IVTV_HW_UPD64031A)
		ivtv_upd64031a(itv, VIDIOC_S_INPUT, &type);
	if (itv->card->hw_video & IVTV_HW_UPD6408X)
		ivtv_upd64083(itv, VIDIOC_S_INPUT, &type);
}
