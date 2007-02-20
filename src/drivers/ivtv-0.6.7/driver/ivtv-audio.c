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
#include <media/audiochip.h>
#include <linux/videodev.h>

/* Switching audio input for WIN-PVR250/350 and MSP34XX based card */
/* Note that this will always override any changes to the audio input made
   with ivtv_audio_set_matrix(). Since that function is for testing only
   this is acceptable. */
static void ivtv_set_audio_for_msp34xx(struct ivtv *itv, u32 input, u32 output)
{
	struct msp_matrix mspm;
	struct v4l2_control ctrl;

	mspm.output = output;
	mspm.input = input;
	IVTV_DEBUG_INFO("Setting audio matrix to input %d, output %d\n",
		   mspm.input, mspm.output);
	ivtv_msp34xx(itv, MSP_SET_MATRIX, &mspm);

	ctrl.id = V4L2_CID_AUDIO_MUTE;
	ctrl.value = 0;
	ivtv_msp34xx(itv, VIDIOC_S_CTRL, &ctrl);
}

/* switching audio input for MPG600/MPG160/M179 (controlled by GPIO) */
static int ivtv_set_audio_for_gpio(struct ivtv *itv, u32 audio_input)
{
	struct v4l2_audio input;
	struct v4l2_control ctrl;
	int result;

	input.index = audio_input;
	result = ivtv_gpio(itv, VIDIOC_S_AUDIO, &input);
	if (result == 0) {
		ctrl.id = V4L2_CID_AUDIO_MUTE;
		ctrl.value = 0;
		ivtv_gpio(itv, VIDIOC_S_CTRL, &ctrl);
	}
	return result;
}

static int ivtv_set_audio_for_cx25840(struct ivtv *itv, u32 audio_input)
{
	struct v4l2_audio input;

	IVTV_DEBUG_INFO("Setting audio to input %d\n", audio_input);
	input.index = audio_input;
	return ivtv_cx25840(itv, VIDIOC_S_AUDIO, &input);
}

static int ivtv_set_audio_for_saa717x(struct ivtv *itv, u32 audio_input)
{
	struct v4l2_audio input;

	IVTV_DEBUG_INFO("Setting audio to input %d\n", audio_input);
	input.index = audio_input;
	return ivtv_saa717x(itv, VIDIOC_S_AUDIO, &input);
}

/* Selects the audio input and output according to the current
   settings. Call this function after these settings change. */
int ivtv_audio_set_io(struct ivtv *itv)
{
	struct v4l2_audio v4l2_input;
	u32 audio_input;
	int mux_input;

	/* Determine which input to use */
	if (test_bit(IVTV_F_I_RADIO_USER, &itv->i_flags)) {
		audio_input = itv->card->radio_input.audio_input;
		mux_input = itv->card->radio_input.muxer_input;
	} else {
		audio_input = itv->card->audio_inputs[itv->audio_input].audio_input;
		mux_input = itv->card->audio_inputs[itv->audio_input].muxer_input;
	}

	/* handle muxer chips */
	v4l2_input.index = mux_input;
	ivtv_i2c_hw(itv, itv->card->hw_muxer, VIDIOC_S_AUDIO, &v4l2_input);

	if (itv->card->hw_audio & IVTV_HW_MSP34XX) {
		/* Select input and output.
		   Output is normally 1 (SCART1), but can be set with
		   the ivtvctl utility. */
		ivtv_set_audio_for_msp34xx(itv, audio_input, 1);
		return 0;
	}
	if (itv->card->hw_audio & IVTV_HW_CX25840)
		return ivtv_set_audio_for_cx25840(itv, audio_input);
	if (itv->card->hw_audio & IVTV_HW_SAA717X)
		return ivtv_set_audio_for_saa717x(itv, audio_input);
	if (itv->card->hw_audio & IVTV_HW_GPIO)
		return ivtv_set_audio_for_gpio(itv, audio_input);
	return 0;
}

int ivtv_audio_set_matrix(struct ivtv *itv, struct ivtv_msp_matrix *matrix)
{
	if ((itv->hw_flags & IVTV_HW_MSP34XX) == 0)
		return 0;

	if ((matrix->input < 1) || (matrix->input > 8)) {
		IVTV_DEBUG_WARN("Invalid audio input!\n");
		return -EINVAL;
	}
	if ((matrix->output < 0) || (matrix->output > 2)) {
		IVTV_DEBUG_WARN("Invalid audio output!\n");
		return -EINVAL;
	}

	IVTV_DEBUG_INFO("v4l2 ioctl: set matrix in = %d, out = %d\n",
		   matrix->input, matrix->output);

	ivtv_msp34xx(itv, MSP_SET_MATRIX, matrix);
	return 0;
}

void ivtv_audio_set_audio_clock_freq(struct ivtv *itv, u8 freq)
{
	static u32 freqs[3] = { 44100, 48000, 32000 };

	/* The audio clock of the digitizer must match the codec sample
	   rate otherwise you get some very strange effects. */
	if (freq > 2)
		return;
	ivtv_call_i2c_clients(itv, VIDIOC_INT_AUDIO_CLOCK_FREQ, &freqs[freq]);
}

