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
#include "saa7127.h"

void ivtv_set_vps(struct ivtv *itv, int enabled, u8 vps1, u8 vps2, u8 vps3, u8 vps4, u8 vps5)
{
	struct saa7127_vps_data data;

	if (!(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
		return;
	ivtv_saa7127(itv, ENCODER_ENABLE_VPS, &enabled);
	if (!enabled)
	       	return;
	data.data[0] = vps1;
	data.data[1] = vps2;
	data.data[2] = vps3;
	data.data[3] = vps4;
	data.data[4] = vps5;
        ivtv_saa7127(itv, ENCODER_SET_VPS_DATA, &data);
}

void ivtv_set_cc(struct ivtv *itv, int mode, u8 cc1, u8 cc2, u8 cc3, u8 cc4)
{
	int ccdata = ((cc4 << 24) | (cc3 << 16) | (cc2 << 8) | cc1);

	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
		ivtv_saa7127(itv, ENCODER_ENABLE_CC, &mode);
		ivtv_saa7127(itv, ENCODER_SET_CC_DATA, &ccdata);
	}
}

void ivtv_set_wss(struct ivtv *itv, int enabled, int mode) 
{
	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
		ivtv_saa7127(itv, ENCODER_SET_WSS_MODE, &mode);
		ivtv_saa7127(itv, ENCODER_ENABLE_WSS, &enabled);
	}
}

void ivtv_encoder_enable(struct ivtv *itv, int enabled)
{
	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT)
        	ivtv_saa7127(itv, ENCODER_ENABLE_OUTPUT, &enabled);
}

