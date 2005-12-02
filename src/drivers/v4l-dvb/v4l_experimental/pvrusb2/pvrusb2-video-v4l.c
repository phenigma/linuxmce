/*
 *
 *  $Id: pvrusb2-video-v4l.c,v 1.2 2005/11/29 14:10:44 mchehab Exp $
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

/*

   This module connects the pvrusb2 driver to the I2C chip level
   driver which handles device video processing.  This interface is
   used internally by the driver; higher level code should only
   interact through the interface provided by pvrusb2-hdw.h.

   This source file is specifically designed to interface with the
   saa711x support that is available in the v4l available starting
   with linux 2.6.15.

*/

#include "pvrusb2-video-v4l.h"

#ifndef PVR2_SUPPRESS_SAA711X

#include "pvrusb2-i2c.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include <linux/videodev.h>
#include <media/v4l2-common.h>
#include <linux/errno.h>
#include <linux/slab.h>


#define pvr2_decoder_trace(...) pvr2_trace(PVR2_TRACE_DECODER,__VA_ARGS__)


static int pvr2_decoder_v4l_enable_output(struct pvr2_decoder *dcp)
{
	int status;
	struct pvr2_hdw *hdw = dcp->hdw;
	pvr2_decoder_trace("pvr2_decoder_enable_output");
	status = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_STREAMON,0);
	if (status) return status;
	hdw->subsys_enabled_mask |= PVR2_SUBSYS_DIGITIZER_RUN;
	return 0;
}


static int pvr2_decoder_v4l_disable_output(struct pvr2_decoder *dcp)
{
	int status;
	struct pvr2_hdw *hdw = dcp->hdw;
	pvr2_decoder_trace("pvr2_decoder_disable_output");
	status = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_STREAMOFF,0);
	if (status) return status;
	hdw->subsys_enabled_mask &= ~PVR2_SUBSYS_DIGITIZER_RUN;
	return 0;
}


static int pvr2_decoder_v4l_set_input(struct pvr2_decoder *dcp)
{
	int status;
	struct pvr2_hdw *hdw = dcp->hdw;
	int v = 0;
	pvr2_decoder_trace("pvr2_decoder_set_input(%d)",
			   hdw->controls[PVR2_CID_INPUT].value);
	switch(hdw->controls[PVR2_CID_INPUT].value){
	case PVR2_CVAL_INPUT_TV:
		v = 4;
		break;
	case PVR2_CVAL_INPUT_COMPOSITE:
		v = 5;
		break;
	case PVR2_CVAL_INPUT_SVIDEO:
		v = 8;
		break;
	case PVR2_CVAL_INPUT_RADIO:
		// ????? No idea yet what to do here
	default:
		return -EINVAL;
	}
	status = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_INPUT,&v);
	if (!status) {
		hdw->subsys_enabled_mask |= PVR2_SUBSYS_DIGITIZER_CFG_INPUT;
	}
	return status;
}


static int pvr2_decoder_v4l_set_bcsh(struct pvr2_decoder *dcp)
{
	struct v4l2_control ctrl;
	struct pvr2_hdw *hdw = dcp->hdw;
	int ret;
	memset(&ctrl,0,sizeof(ctrl));

	pvr2_decoder_trace("pvr2_decoder_set_bcsh b=%d c=%d s=%d h=%d",
			   hdw->controls[PVR2_CID_BRIGHTNESS].value,
			   hdw->controls[PVR2_CID_CONTRAST].value,
			   hdw->controls[PVR2_CID_SATURATION].value,
			   hdw->controls[PVR2_CID_HUE].value);

	ctrl.id = V4L2_CID_BRIGHTNESS;
	ctrl.value = hdw->controls[PVR2_CID_BRIGHTNESS].value;
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_CTRL,&ctrl);
	if (ret) return ret;
	ctrl.id = V4L2_CID_CONTRAST;
	ctrl.value = hdw->controls[PVR2_CID_CONTRAST].value;
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_CTRL,&ctrl);
	if (ret) return ret;
	ctrl.id = V4L2_CID_SATURATION;
	ctrl.value = hdw->controls[PVR2_CID_SATURATION].value;
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_CTRL,&ctrl);
	if (ret) return ret;
	ctrl.id = V4L2_CID_HUE;
	ctrl.value = hdw->controls[PVR2_CID_HUE].value;
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_CTRL,&ctrl);
	if (ret) return ret;

	hdw->subsys_enabled_mask |= PVR2_SUBSYS_DIGITIZER_CFG_BCSH;
	return 0;
}


static int pvr2_decoder_v4l_is_tuned(struct pvr2_decoder *dcp)
{
	struct v4l2_tuner vt;
	struct pvr2_hdw *hdw = dcp->hdw;
	int ret;

	memset(&vt,0,sizeof(vt));
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_G_TUNER,&vt);
	if (ret < 0) return -EINVAL;
	return vt.signal ? 1 : 0;
}


static int pvr2_decoder_v4l_set_size(struct pvr2_decoder *dcp)
{
	struct v4l2_format fmt;
	struct pvr2_hdw *hdw = dcp->hdw;
	int ret;

	memset(&fmt,0,sizeof(fmt));

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = hdw->controls[PVR2_CID_HRES].value;
	fmt.fmt.pix.height = hdw->controls[PVR2_CID_VRES].value;

	pvr2_decoder_trace("pvr2_decoder_set_size(%dx%d)",
			   fmt.fmt.pix.width,fmt.fmt.pix.height);

	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_FMT,&fmt);
	if (ret) return ret;

	hdw->subsys_enabled_mask |= PVR2_SUBSYS_DIGITIZER_CFG_SIZE;
	return 0;
}


static int pvr2_decoder_v4l_set_audio(struct pvr2_decoder *dcp)
{
	int ret;
	struct pvr2_hdw *hdw = dcp->hdw;
	enum v4l2_audio_clock_freq val;

	pvr2_decoder_trace("pvr2_decoder_set_audio %d",
			   hdw->controls[PVR2_CID_SRATE].value);
	switch (hdw->controls[PVR2_CID_SRATE].value) {
	default:
	case PVR2_CVAL_SRATE_48:
		val = V4L2_AUDCLK_48_KHZ;
		break;
	case PVR2_CVAL_SRATE_44_1:
		val = V4L2_AUDCLK_441_KHZ;
		break;
	}
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_INT_AUDIO_CLOCK_FREQ,&val);
	if (ret) return ret;
	hdw->subsys_enabled_mask |= PVR2_SUBSYS_DIGITIZER_CFG_AUDIO;
	return 0;
}


static int pvr2_decoder_v4l_set_norm(struct pvr2_decoder *dcp)
{
	v4l2_std_id std;
	struct pvr2_hdw *hdw = dcp->hdw;
	int status;
	pvr2_decoder_trace("pvr2_decoder_set_norm %d",
			   hdw->controls[PVR2_CID_VIDEOSTANDARD].value);
	switch (hdw->controls[PVR2_CID_VIDEOSTANDARD].value) {
	default:
	case PVR2_CVAL_VIDEOSTANDARD_NTSC_M:
		std = V4L2_STD_NTSC;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_SECAM_L:
		std = V4L2_STD_SECAM;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_I:
		std = V4L2_STD_PAL_I;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_DK:
		std = V4L2_STD_PAL_DK;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_M:
		std = V4L2_STD_PAL_M;
		break;
	}
		status = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_S_STD,&std);
	if (status) return status;
	hdw->subsys_enabled_mask |= PVR2_SUBSYS_DIGITIZER_CFG_NORM;
	return 0;
}


static const struct pvr2_decoder_func_table pvr2_decoder_v4l_table = {
	(pvr2_decoder_func)0,
	(pvr2_decoder_func)pvr2_decoder_v4l_set_norm,
	(pvr2_decoder_func)pvr2_decoder_v4l_set_input,
	(pvr2_decoder_func)pvr2_decoder_v4l_set_size,
	(pvr2_decoder_func)pvr2_decoder_v4l_set_audio,
	(pvr2_decoder_func)pvr2_decoder_v4l_set_bcsh,
	(pvr2_decoder_func)pvr2_decoder_v4l_is_tuned,
	(pvr2_decoder_func)pvr2_decoder_v4l_enable_output,
	(pvr2_decoder_func)pvr2_decoder_v4l_disable_output,
};


struct pvr2_decoder *pvr2_decoder_v4l_create(struct pvr2_hdw *hdw)
{
	/* Attempt to query the decoder - let's see if it will answer */
	struct v4l2_tuner vt;
	int ret;
	struct pvr2_decoder *dcp;

	memset(&vt,0,sizeof(vt));
	ret = pvr2_i2c_saa7115_cmd(hdw,VIDIOC_G_TUNER,&vt);
	if (ret < 0) return 0; /* Nope, not there */

	/* Yes, it's there.  So initialize... */
	dcp = kmalloc(sizeof(*dcp),GFP_KERNEL);
	if (!dcp) return 0;
	memset(dcp,0,sizeof(*dcp));
	dcp->hdw = hdw;
	dcp->func_table = &pvr2_decoder_v4l_table;
	return dcp;
}

#endif  /* PVR2_SUPPRESS_SAA711X */


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
