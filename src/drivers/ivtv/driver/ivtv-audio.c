/*
    Audio-related ivtv functions.
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
#include "ivtv-mailbox.h"
#include "ivtv-i2c.h"
#include "ivtv-gpio.h"
#include "ivtv-cards.h"
#include "ivtv-audio.h"
#include "ivtv-kthreads.h"
#include "msp3400.h"
#include "wm8775.h"
#include "cs53l32a.h"

/* Switching audio input for WIN-PVR250/350 and MSP34XX based card */
/* Note that this will always override any changes to the audio input made
   with ivtv_audio_set_matrix(). Since that function is for testing only
   this is acceptable. */
static int ivtv_set_audio_for_msp34xx(struct ivtv *itv, u32 input, u32 output)
{
	struct msp_matrix mspm;
	struct video_audio va;

	if (input >= 0 && input <= 2) {
		IVTV_DEBUG_INFO("Setting audio to input %d\n", input);
		mspm.output = output;
		switch (input) {
		case 0:	/* Tuner Audio */
			mspm.input = 3;
			break;
		case 1:	/* Audio-In (Line-in) */
			mspm.input = 1;
			break;
		case 2:	/* Radio-In */
			mspm.input = 2;
			break;
		default:
			mspm.input = 3;
			break;
		}
		IVTV_DEBUG_INFO(
			   "Setting audio matrix to input %d, output %d\n",
			   mspm.input, mspm.output);
		ivtv_msp34xx(itv, MSP_SET_MATRIX, &mspm);

		memset(&va, 0, sizeof(struct video_audio));
		ivtv_msp34xx(itv, VIDIOCGAUDIO, &va);
		va.flags &= ~VIDEO_AUDIO_MUTE;
		ivtv_msp34xx(itv, VIDIOCSAUDIO, &va);
		return 0;
	}

	IVTV_ERR("Invalid audio input, shouldn't happen!\n");

	return -EINVAL;
}

/* switching audio input for MPG600/MPG160/M179 (controlled by GPIO) */
static int ivtv_set_audio_for_gpio(struct ivtv *itv, u32 audio_input)
{
	if (audio_input <= 1) {
		IVTV_DEBUG_INFO(
			   "Setting audio to input %d\n", audio_input);
		switch (audio_input) {
		case 0:	/* Tuner Audio */
			ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_TUNER);
			break;
		case 1:	/* Audio-In (Line-in) */
			ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_LINE);
			break;;
		}
		ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_UNMUTE);
		return 0;
	}

	IVTV_ERR("Invalid audio input, shouldn't happen!\n");

	return -EINVAL;
}

static int ivtv_set_audio_for_cx25840(struct ivtv *itv, u32 audio_input)
{
	if (audio_input <= 4) {
		int cx_input = (int)audio_input;

		IVTV_DEBUG_INFO(
			"Setting audio to input %d\n", audio_input);

		ivtv_cx25840(itv, DECODER_SET_AUDIO_INPUT, &cx_input);

		return 0;
	}

	IVTV_ERR("Invalid audio input, shouldn't happen!\n");

	return -EINVAL;
}


/* switching audio input on PVR-150/500, only tested on 150 thus far */
static int ivtv_set_audio_for_pvr150(struct ivtv *itv, u32 audio_input)
{
        int wm_input = 0;

        switch (audio_input) {
                case 0: // TV
                        wm_input = 0; // TV audio bypasses wm8775
                        break;

                case 1: // external line in 1 (???)
                        wm_input = 0x01;
                        break;

                case 2: // external line in 2 (back panel)
                        wm_input = 0x02;
                        break;
                case 3: // external line in 3 (internal connector)
                        wm_input = 0x04;
                        break;

                case 4: // external line in 4 FM Radio
                        wm_input = 0x08;
                        break;

                default:
                        return -EINVAL;
        }

        ivtv_wm8775(itv, WM8775_SET_INPUT, &wm_input);
        return 0;
}

static int ivtv_set_audio_for_adaptec(struct ivtv *itv, u32 audio_input)
{
	int cs_input = 0;

	switch (audio_input) {
		case 0: // TV
			cs_input = 0x01;
			break;
			
		case 1: // external line in
			cs_input = 0x21;
			break;
			
		default:
			return -EINVAL;
	}

	ivtv_cs53l32a(itv, CS53L32A_SET_INPUT, &cs_input);
	return 0;
}

/* Selects the audio input and output according to the current
   settings. Call this function after these settings change. */
int ivtv_audio_set_io(struct ivtv *itv)
{
	u32 audio_input;

	/* Determine which input to use */
	if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
		audio_input = itv->audio_input_radio;
	} else {
		audio_input = itv->audio_input_tv;
	}

	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		if (itv->card->type == IVTV_CARD_AVC2410) {
			ivtv_set_audio_for_adaptec(itv, audio_input);
		}
		/* Select input and output.
		   Output is normally 1 (SCART1), but can be set with
		   the ivtvctl utility. */
		return ivtv_set_audio_for_msp34xx(itv,
						  audio_input,
						  itv->msp34xx_audio_output);
	case USE_CS53L32A:
		return 0;	/* Do nothing, only one audio input */
        case USE_PVR150:
                ivtv_set_audio_for_pvr150(itv, audio_input);
                /* fall through */
	case USE_CX25840:
		return ivtv_set_audio_for_cx25840(itv, audio_input);
	case USE_GPIO:
		return ivtv_set_audio_for_gpio(itv, audio_input);
	}

	IVTV_ERR("Invalid card type [ivtv_set_audio]!\n");
	return -EINVAL;
}

void ivtv_audio_set_volume(struct ivtv *itv, int volume)
{
	struct video_audio va;
	memset(&va, 0, sizeof(struct video_audio));

	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		ivtv_msp34xx(itv, VIDIOCGAUDIO, &va);
		va.volume = volume;
		va.mode = 0;	/* keep stereo mode at automatic stereo detection */
		ivtv_msp34xx(itv, VIDIOCSAUDIO, &va);
		break;
	case USE_CS53L32A:
		break;		/* Need volume control adjust here */
        case USE_PVR150:
	case USE_CX25840:
		ivtv_cx25840(itv, VIDIOCGAUDIO, &va);
		va.volume = volume;
		ivtv_cx25840(itv, VIDIOCSAUDIO, &va);
		break;
	case USE_GPIO:
		/* do nothing, there's no volume control */
		break;
	}
}

int ivtv_audio_get_volume(struct ivtv *itv)
{
	struct video_audio va;
	memset(&va, 0, sizeof(struct video_audio));

	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		ivtv_msp34xx(itv, VIDIOCGAUDIO, &va);
		break;
	case USE_CS53L32A: /* need to get volume control here */
		break;
        case USE_PVR150:
	case USE_CX25840:
		ivtv_cx25840(itv, VIDIOCGAUDIO, &va);
		break;
	case USE_GPIO:
		/* do nothing, there's no volume control */
		va.volume = 65535;	/* dummy code */
		break;
	}
	return va.volume;
}

void ivtv_audio_set_mute(struct ivtv *itv, int mute)
{
	struct video_audio va;

	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		memset(&va, 0, sizeof(struct video_audio));
		ivtv_msp34xx(itv, VIDIOCGAUDIO, &va);
		if (mute)
			va.flags |= VIDEO_AUDIO_MUTE;
		else
			va.flags &= ~VIDEO_AUDIO_MUTE;
		va.mode = 0;	/* keep stereo mode at automatic stereo detection */
		ivtv_msp34xx(itv, VIDIOCSAUDIO, &va);
		break;
 	case USE_CS53L32A: /* need to get mute here */
                break;
        case USE_PVR150:
	case USE_CX25840:
		memset(&va, 0, sizeof(struct video_audio));
		ivtv_cx25840(itv, VIDIOCGAUDIO, &va);
		if (mute)
			va.flags |= VIDEO_AUDIO_MUTE;
		else
			va.flags &= ~VIDEO_AUDIO_MUTE;
		ivtv_cx25840(itv, VIDIOCSAUDIO, &va);
		break;
	case USE_GPIO:
		if (mute)
			ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_MUTE);
		else
			ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_UNMUTE);
		break;
	}
}

int ivtv_audio_get_mute(struct ivtv *itv)
{
	struct video_audio va;

	memset(&va, 0, sizeof(struct video_audio));
	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		ivtv_msp34xx(itv, VIDIOCGAUDIO, &va);
		break;
	case USE_CS53L32A:	/* need to mute here */
		break;
        case USE_PVR150:
	case USE_CX25840:
		ivtv_cx25840(itv, VIDIOCGAUDIO, &va);
		break;
	case USE_GPIO:
		if (ivtv_get_gpio_audio(itv, IVTV_GPIO_AUDIO_MUTE))
			va.flags = VIDEO_AUDIO_MUTE;
		break;
	}
	return va.flags & VIDEO_AUDIO_MUTE;
}

void ivtv_audio_set_norm(struct ivtv *itv, struct video_channel *v)
{
	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		ivtv_msp34xx(itv, VIDIOCSCHAN, v);
		break;
	case USE_CS53L32A:
		break;
        case USE_PVR150:
	case USE_CX25840:
                // Already handled by DECODER_SET_NORM
		break;
	case USE_GPIO:
		ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_TUNER);
		break;
	}
}

void ivtv_audio_set_audmode(struct ivtv *itv, u32 audmode)
{
	struct v4l2_tuner vt;

	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		vt.index = 0;
		vt.audmode = audmode;
		ivtv_msp34xx(itv, VIDIOC_S_TUNER, &vt);
		ivtv_dualwatch_set_enabled(itv,
					   audmode == V4L2_TUNER_MODE_STEREO);
		break;
	case USE_CS53L32A:
		break;
        case USE_PVR150:
	case USE_CX25840:
		vt.index = 0;
		vt.audmode = audmode;
		ivtv_cx25840(itv, VIDIOC_S_TUNER, &vt);
                break;
	case USE_GPIO:
		ivtv_set_gpio_audiomode(itv, audmode);
		break;
	}
}

u32 ivtv_audio_get_rxsubchans(struct ivtv *itv, struct v4l2_tuner *vt)
{
	if ((itv->card->type == IVTV_CARD_MPG600)
	    || (itv->card->type == IVTV_CARD_MPG160)) {
		/* MPGxxx can read tuner audio status */
		switch (ivtv_get_gpio_audiomode(itv)) {
		case 0:	/* mono */
			return V4L2_TUNER_SUB_MONO;
		case 1:	/* stereo */
			if ((vt->audmode == V4L2_TUNER_MODE_STEREO) ||
			    (vt->audmode == V4L2_TUNER_MODE_LANG1))
				return V4L2_TUNER_SUB_STEREO;
			return V4L2_TUNER_SUB_MONO;
		case 2:	/* multiplex */
			if (vt->audmode == V4L2_TUNER_MODE_LANG2)
				return V4L2_TUNER_SUB_LANG2 |
				    V4L2_TUNER_SUB_MONO;
			return V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_MONO;
		}
	}
	if (itv->card->audio_selector == USE_MSP34XX) {
		struct v4l2_tuner msp_vt;

		msp_vt.index = 0;
		ivtv_msp34xx(itv, VIDIOC_G_TUNER, &msp_vt);
		return msp_vt.rxsubchans;
	}
	if (itv->card->audio_selector == USE_CX25840 ||
            itv->card->audio_selector == USE_PVR150) {
		struct v4l2_tuner vt;

		vt.index = 0;
		ivtv_cx25840(itv, VIDIOC_G_TUNER, &vt);
		return vt.rxsubchans;
        }
	/* Unknown, so anything goes. */
	return vt->capability;
}

/* This starts the automatic sound detection of the msp34xx that detects
   mono/stereo/multilang. */
void ivtv_audio_freq_changed(struct ivtv *itv)
{
	unsigned long freq = 0;		/* Value is ignored by msp34xx */

	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		ivtv_msp34xx(itv, VIDIOCSFREQ, &freq);
		break;
	case USE_CS53L32A:
		break;
        case USE_PVR150:
		ivtv_wm8775(itv, VIDIOCSFREQ, &freq);
                /* fall through */
	case USE_CX25840:
		ivtv_cx25840(itv, VIDIOCSFREQ, &freq);
                break;
	case USE_GPIO:
		break;
	}
}

int ivtv_audio_set_matrix(struct ivtv *itv, struct ivtv_msp_matrix *matrix)
{
	switch (itv->card->audio_selector) {
	case USE_MSP34XX:
		if ((matrix->input < 1) || (matrix->input > 8)) {
			IVTV_DEBUG_WARN("Invalid audio input!\n");
			return -EINVAL;
		}
		if ((matrix->output < 0) || (matrix->output > 2)) {
			IVTV_DEBUG_WARN("Invalid audio output!\n");
			return -EINVAL;
		}
		itv->msp34xx_audio_output = matrix->output;

		IVTV_DEBUG_INFO(
			   "v4l2 ioctl: set matrix in = %d, out = %d\n",
			   matrix->input, matrix->output);

		ivtv_msp34xx(itv, MSP_SET_MATRIX, matrix);
		break;
	case USE_CS53L32A:
        case USE_PVR150:
	case USE_CX25840:
	case USE_GPIO:
		/* do nothing, msp34xx specific function */
		break;
	}
	return 0;
}

void ivtv_audio_set_samplerate(struct ivtv *itv, int samplerate)
{
        switch (itv->card->audio_selector) {
        case USE_MSP34XX:
                break;
	case USE_CS53L32A:
		break;
        case USE_PVR150:
        case USE_CX25840:
                ivtv_cx25840(itv, DECODER_SET_AUDIO, &samplerate);
                break;
        case USE_GPIO:
                ivtv_set_gpio_audiosamplerate(itv, samplerate);
                break;
        }
}

