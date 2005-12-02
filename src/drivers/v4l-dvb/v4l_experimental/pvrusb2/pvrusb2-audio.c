/*
 *
 *  $Id: pvrusb2-audio.c,v 1.2 2005/11/29 14:10:44 mchehab Exp $
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

#include "pvrusb2-i2c.h"
#include "pvrusb2-audio.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-debug.h"
#include "msp3400.h"
#include <linux/videodev.h>
#include <media/audiochip.h>

/*

  MCI <isely@pobox.com> 10-Mar-2005 - Rather than operate the msp34xx
  directly, we rely on the msp3400.ko module to do it for us.  We
  really have to do this because that $##@!! module is going to attach
  itself to us anyway, so we really can't operate the chip ourselves.
  Unfortunately msp3400.ko is a real train wreck of a piece of code.
  Most of the code below tries to tickle that module in just the right
  way to get the results we need.  Yuck.  msp3400.c should be taken
  out back and shot.  Based on my reading of the actual chip datasheet
  it should in theory be possible to write a far cleaner and simpler
  driver than what is currently there right now.

*/

static int xlat_audiomode_to_v4l2(int id)
{
	switch (id) {
	case PVR2_CVAL_AUDIOMODE_MONO:
		return V4L2_TUNER_MODE_MONO;
	case PVR2_CVAL_AUDIOMODE_STEREO:
		return V4L2_TUNER_MODE_STEREO;
	case PVR2_CVAL_AUDIOMODE_SAP:
		return V4L2_TUNER_MODE_SAP;
	case PVR2_CVAL_AUDIOMODE_LANG1:
		return V4L2_TUNER_MODE_LANG1;
	case PVR2_CVAL_AUDIOMODE_LANG2:
		return V4L2_TUNER_MODE_LANG2;
	}
	return V4L2_TUNER_MODE_STEREO;
}

/* Relay the video standard into the msp3400 module so that it can use
   that information as additional clue(s) for correctly detecting
   audio. */
int pvr2_audio_set_standard(struct pvr2_hdw *hdw)
{
	struct video_channel vc;
	memset(&vc,0,sizeof(vc));
	switch (hdw->controls[PVR2_CID_VIDEOSTANDARD].value) {
	default:
	case PVR2_CVAL_VIDEOSTANDARD_NTSC_M:
	case PVR2_CVAL_VIDEOSTANDARD_PAL_M: /* Hack */
		vc.norm = VIDEO_MODE_NTSC;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_SECAM_L:
		vc.norm = VIDEO_MODE_SECAM;
		break;
	case PVR2_CVAL_VIDEOSTANDARD_PAL_BG:
	case PVR2_CVAL_VIDEOSTANDARD_PAL_I:
	case PVR2_CVAL_VIDEOSTANDARD_PAL_DK:
		vc.norm = VIDEO_MODE_PAL;
		break;
	}
	hdw->subsys_enabled_mask |= PVR2_SUBSYS_AUDIO_CFG_STD;
	return pvr2_i2c_msp3400_cmd(hdw,VIDIOCSCHAN,&vc);
}

/* This function selects the correct audio input source */
int pvr2_audio_set_stereo(struct pvr2_hdw *hdw)
{
	int stat;
	unsigned short sarg = 0;
	struct msp_matrix mspm;

	pvr2_trace(PVR2_TRACE_AUDIO,"pvr_audio_set_stereo");

	if (hdw->controls[PVR2_CID_INPUT].value == PVR2_CVAL_INPUT_TV) {
		struct v4l2_tuner vt;
		memset(&vt,0,sizeof(vt));
		vt.audmode = xlat_audiomode_to_v4l2(
			hdw->controls[PVR2_CID_AUDIOMODE].value);
		stat = pvr2_i2c_msp3400_cmd(hdw,VIDIOC_S_TUNER,&vt);
		if (stat < 0) return stat;
	}

	sarg = AUDIO_TUNER;
	switch (hdw->controls[PVR2_CID_INPUT].value) {
	case PVR2_CVAL_INPUT_TV:
		sarg = AUDIO_TUNER;
		break;
	case PVR2_CVAL_INPUT_RADIO:
		/* Assume that msp34xx also handle FM decoding, in which case
		   we're still using the tuner. */
		sarg = AUDIO_TUNER;
		break;
	case PVR2_CVAL_INPUT_SVIDEO:
	case PVR2_CVAL_INPUT_COMPOSITE:
		sarg = AUDIO_EXTERN;
		break;
	}
	stat = pvr2_i2c_msp3400_cmd(hdw,AUDC_SET_INPUT,&sarg);
	if (stat < 0) return stat;

	/* The above should have been enough to do the job, however
	   msp3400.ko does an incomplete job of handling the scart
	   routing.  Really.  It doesn't even bother to initialize the
	   SC1 output at all.  So we have to help it here...
	   Unfortunately this also means that we have include
	   msp3400.c's header file and it currently isn't in a public
	   place.  Damnit! */

	mspm.input = SCART_IN1_DA;
	switch (hdw->controls[PVR2_CID_INPUT].value) {
	case PVR2_CVAL_INPUT_SVIDEO:
	case PVR2_CVAL_INPUT_COMPOSITE:
		/* Bypass the DSP and just use IN1.  In theory we
		   should be able to permanent just use IN1_DA, but to
		   do that msp3400.ko should be adjusting the DSP
		   input SCART routing correctly when doing video in.
		   Unfortunately that appears not to be the case.  I
		   really hate that module. */
		mspm.input = SCART_IN1;
		break;
	}
	mspm.output = 1;
	stat = pvr2_i2c_msp3400_cmd(hdw,MSP_SET_MATRIX,&mspm);
	if (stat < 0) return stat;

	hdw->subsys_enabled_mask |= PVR2_SUBSYS_AUDIO_CFG_MODE;

	return 0;
}

/* This sets the audio volume parameters */
int pvr2_audio_setvolume(struct pvr2_hdw *hdw)
{
	struct video_audio vt;
	memset(&vt,0,sizeof(vt));
	vt.flags = (VIDEO_AUDIO_VOLUME |
		    VIDEO_AUDIO_BALANCE |
		    VIDEO_AUDIO_BASS |
		    VIDEO_AUDIO_TREBLE |
		    VIDEO_AUDIO_MUTABLE);
	if (hdw->controls[PVR2_CID_MUTE].value) vt.flags |= VIDEO_AUDIO_MUTE;
	vt.volume = hdw->controls[PVR2_CID_VOLUME].value;
	vt.balance = hdw->controls[PVR2_CID_BALANCE].value;
	vt.bass = hdw->controls[PVR2_CID_BASS].value;
	vt.treble = hdw->controls[PVR2_CID_TREBLE].value;
	pvr2_trace(PVR2_TRACE_AUDIO,
		   "pvr_audio_setvolume(vol=%d bal=%d bas=%d treb=%d mute=%d)",
		   vt.volume,vt.balance,vt.bass,vt.treble,
		   (vt.flags & VIDEO_AUDIO_MUTE) != 0);

	hdw->subsys_enabled_mask |= PVR2_SUBSYS_AUDIO_CFG_VBBTM;

	return pvr2_i2c_msp3400_cmd(hdw,VIDIOCSAUDIO,&vt);
}

/* This reads back the current volume parameters and signal type */
int pvr2_audio_update_status(struct pvr2_hdw *hdw)
{
	struct video_audio vt;
	int stat;

	stat = pvr2_i2c_msp3400_cmd(hdw,VIDIOCGAUDIO,&vt);
	if (stat < 0) return stat;

#ifdef notdef
	if (vt.flags & VIDEO_AUDIO_MUTABLE) {
		hdw->controls[PVR2_CID_MUTE].value =
			(vt.flags & VIDEO_AUDIO_MUTE) ? 1 : 0;
	}
	if (vt.flags & VIDEO_AUDIO_VOLUME) {
		pvr2_trace(PVR2_TRACE_AUDIO,
			   "pvr_audio_update_status: got volume");
		hdw->controls[PVR2_CID_VOLUME].value = vt.volume;
	}
	if (vt.flags & VIDEO_AUDIO_BASS) {
		pvr2_trace(PVR2_TRACE_AUDIO,
			   "pvr_audio_update_status: got bass");
		hdw->controls[PVR2_CID_BASS].value = vt.bass;
	}
	if (vt.flags & VIDEO_AUDIO_TREBLE) {
		pvr2_trace(PVR2_TRACE_AUDIO,
			   "pvr_audio_update_status: got treble");
		hdw->controls[PVR2_CID_TREBLE].value = vt.treble;
	}
	if (vt.flags & (VIDEO_AUDIO_BALANCE|VIDEO_AUDIO_VOLUME)) {
		pvr2_trace(PVR2_TRACE_AUDIO,
			   "pvr_audio_update_status: got balance");
		hdw->controls[PVR2_CID_BALANCE].value = vt.balance;
	}
#endif

	hdw->flag_stereo = (vt.mode & VIDEO_SOUND_STEREO) != 0;
	hdw->flag_bilingual = (vt.mode &
			       (VIDEO_SOUND_LANG1|VIDEO_SOUND_LANG2)) != 0;
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
