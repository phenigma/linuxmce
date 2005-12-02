/*
 *
 *  $Id: pvrusb2-tuner.c,v 1.2 2005/11/29 14:10:44 mchehab Exp $
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

#include "pvrusb2.h"
#include "pvrusb2-util.h"
#include "pvrusb2-tuner.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-i2c.h"
#include "pvrusb2-debug.h"
#include "compat.h"
#include <linux/videodev.h>
#include <media/tuner.h>

#define PVR_STD_I2C_ADDR    0x43

static int tuner[PVR_NUM] = { [0 ... PVR_NUM-1] = -1 };

module_param_array(tuner,    int, NULL, 0444);
MODULE_PARM_DESC(tuner,"specify installed tuner type");

static u32 pvr_tbl_standard[] = {
	[PVR2_CVAL_VIDEOSTANDARD_PAL_BG]  = 0x00d47009, /* PVR_STD_PAL_BG */
	[PVR2_CVAL_VIDEOSTANDARD_PAL_I]   = 0x00d4700a, /* PVR_STD_PAL_I */
	[PVR2_CVAL_VIDEOSTANDARD_PAL_DK]  = 0x00d4700b, /* PVR_STD_PAL_DK */
	[PVR2_CVAL_VIDEOSTANDARD_SECAM_L] = 0x00c4100b, /* PVR_STD_SECAM */
	[PVR2_CVAL_VIDEOSTANDARD_NTSC_M]  = 0x00163044, /* PVR_STD_NTSC */
	[PVR2_CVAL_VIDEOSTANDARD_PAL_M]  = 0x00163044  /* PVR_STD_PAL_M */
};

#define trace_tuner(...) pvr2_trace(PVR2_TRACE_TUNER,__VA_ARGS__)

int pvr2_tuner_get_default_type(struct pvr2_hdw *hdw)
{
	int unit_number = hdw->unit_number;
	int tp = -1;
	if ((unit_number >= 0) && (unit_number < PVR_NUM)) {
		tp = tuner[unit_number];
	}
	if (tp < 0) return -EINVAL;
	hdw->tuner_type = tp;
	return 0;
}

int pvr2_tuner_set_freq(struct pvr2_hdw *hdw)
{
	int stat;
	struct v4l2_frequency freq;
	freq.frequency = hdw->controls[PVR2_CID_FREQUENCY].value / 62500;
	freq.tuner = 0;
	freq.type = V4L2_TUNER_ANALOG_TV;
	trace_tuner("pvr2_tuner_set_freq(%d)",freq.frequency);
	stat = pvr2_i2c_tuner_cmd(hdw,VIDIOC_S_FREQUENCY,&freq);
	if (stat < 0) return stat;
	/* This kicks the audio controller... */
	stat = pvr2_i2c_msp3400_cmd(hdw,VIDIOCSFREQ,0);
	if (stat < 0) return stat;
	hdw->subsys_enabled_mask |= PVR2_SUBSYS_TUNER_CFG_FREQ;
	return 0;
}

int pvr2_tuner_set_type(struct pvr2_hdw *hdw)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
	struct tuner_setup setup;
#endif
	trace_tuner("pvr2_tuner_set_type(%d)",hdw->tuner_type);
	if (((int)(hdw->tuner_type)) < 0) {
		trace_tuner("tuner type not set yet");
		return 0;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
	setup.addr = ADDR_UNSET;
	setup.type = hdw->tuner_type;
	setup.mode_mask = T_RADIO | T_ANALOG_TV;
	/* We may really want mode_mask to be T_ANALOG_TV for now */
	return pvr2_i2c_tuner_cmd(hdw,TUNER_SET_TYPE_ADDR,&setup);
#else
	return pvr2_i2c_tuner_cmd(hdw,TUNER_SET_TYPE,&hdw->tuner_type);
#endif
}

int pvr2_tuner_set_standard(struct pvr2_hdw *hdw)
{
	struct i2c_msg msg[1];
	u8 buff [4];
	v4l2_std_id vs;
	int cvstd = hdw->controls[PVR2_CID_VIDEOSTANDARD].value;

	trace_tuner("pvr2_tuner_set_standard(%d)",
		    hdw->controls[PVR2_CID_VIDEOSTANDARD].value);

	/* FIXME: Might be tuner dependant ? Need more hardware info... */
	/* MCI 15-May-2005: I have absolutely no idea what this is doing
	   or who it is talking to. */
	PVR2_DECOMPOSE_BE(
		buff, 0,pvr_tbl_standard[cvstd]);
	msg[0].addr = PVR_STD_I2C_ADDR;
	msg[0].flags = 0;
	msg[0].len = sizeof(buff);
	msg[0].buf = buff;
	i2c_transfer(&hdw->i2c_adap,msg,sizeof(msg)/sizeof(msg[0]));

	/* Also tell tuner.ko about the video standard. */
	switch (cvstd) {
	default:
	case PVR2_CVAL_VIDEOSTANDARD_NTSC_M:
		vs = V4L2_STD_NTSC;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_SECAM_L:
		vs = V4L2_STD_SECAM;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_BG:
		vs = V4L2_STD_PAL_BG;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_I:
		vs = V4L2_STD_PAL_I;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_DK:
		vs = V4L2_STD_PAL_DK;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_M:
		vs = V4L2_STD_PAL_M;
		break;
	}
	pvr2_i2c_tuner_cmd(hdw,VIDIOC_S_STD,&vs);

	hdw->subsys_enabled_mask |= PVR2_SUBSYS_TUNER_CFG_STD;
	return 0;
}

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
