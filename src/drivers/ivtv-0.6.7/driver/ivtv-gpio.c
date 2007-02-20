/*
    gpio functions.
    Merging GPIO support into driver:
    Copyright (C) 2004  Chris Kennedy <c@groovy.org>

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
#include "ivtv-cards.h"
#include "ivtv-gpio.h"

/*
 * GPIO assignment of Yuan MPG600/MPG160
 *
 *    bit 15  14  13  12 |  11  10   9   8 |   7   6   5   4 |   3   2   1   0
 * OUTPUT         IN1 IN0                                       AM3 AM2 AM1 AM0
 *  INPUT                   DM1         DM0
 *
 *   IN* : Input selection
 *          IN1 IN0
 *           1   1  N/A
 *           1   0  Line
 *           0   1  N/A
 *           0   0  Tuner
 *
 *   AM* : Audio Mode
 *          AM3  0: Normal        1: Mixed(Sub+Main channel)
 *          AM2  0: Subchannel    1: Main channel
 *          AM1  0: Stereo        1: Mono
 *          AM0  0: Normal        1: Mute
 *
 *   DM* : Detected tuner audio Mode
 *          DM1  0: Stereo        1: Mono
 *          DM0  0: Multiplex     1: Normal
 *
 * GPIO Initial Settings
 *           MPG600   MPG160
 *     DIR   0x3080   0x7080
 *  OUTPUT   0x000C   0x400C
 *
 *  Special thanks to Makoto Iguchi <iguchi@tahoo.org> and Mr. Anonymous
 *  for analyzing GPIO of MPG160.
 *
 *****************************************************************************
 *
 * GPIO assignment of Avermedia M179 (per information direct from AVerMedia)
 *
 *    bit 15  14  13  12 |  11  10   9   8 |   7   6   5   4 |   3   2   1   0
 * OUTPUT IN0 AM0 IN1               AM1 AM2       IN2     BR0   BR1
 *  INPUT
 *
 *   IN* : Input selection
 *          IN0 IN1 IN2
 *           *   1   *  Mute
 *           0   0   0  Line-In
 *           1   0   0  TV Tuner Audio
 *           0   0   1  FM Audio
 *           1   0   1  Mute
 *
 *   AM* : Audio Mode
 *          AM0 AM1 AM2
 *           0   0   0  TV Tuner Audio: L_OUT=(L+R)/2, R_OUT=SAP
 *           0   0   1  TV Tuner Audio: L_OUT=R_OUT=SAP   (SAP)
 *           0   1   0  TV Tuner Audio: L_OUT=L, R_OUT=R   (stereo)
 *           0   1   1  TV Tuner Audio: mute
 *           1   *   *  TV Tuner Audio: L_OUT=R_OUT=(L+R)/2   (mono)
 *
 *   BR* : Audio Sample Rate (BR stands for bitrate for some reason)
 *          BR0 BR1
 *           0   0   32 kHz
 *           0   1   44.1 kHz
 *           1   0   48 kHz
 *
 *   DM* : Detected tuner audio Mode
 *         Unknown currently
 *
 * Special thanks to AVerMedia Technologies, Inc. and Jiun-Kuei Jung at
 * AVerMedia for providing the GPIO information used to add support
 * for the M179 cards.
 */

/********************* GPIO stuffs *********************/

static void ivtv_gpio_and_or(u8 *address, u16 and, u16 or)
{
	writel((readl(address) & ~and) | (or & and), address);
}

void ivtv_reset_ir_gpio(struct ivtv *itv)
{
	int curdir, curout;

	if (itv->card->type != IVTV_CARD_PVR_150)
		return;
	IVTV_DEBUG_INFO("Resetting PVR150 IR\n");
	curout = readl(itv->gpio_out);
	curdir = readl(itv->gpio_dir);
	curdir |= 0x80;
	writel(curdir, itv->gpio_dir);
	curout = (curout & ~0xF) | 1;
	writel(curout, itv->gpio_out);
	// We could use something else for smaller time
	current->state = TASK_INTERRUPTIBLE;
	schedule_timeout(1);
	curout |= 2;
	writel(curout, itv->gpio_out);
	curdir &= ~0x80;
	writel(curdir, itv->gpio_dir);
}

void ivtv_gpio_init(struct ivtv *itv)
{
	if (itv->card->gpio_init.direction == 0)
		return;

	IVTV_DEBUG_INFO("GPIO initial dir: %08x out: %08x\n",
		   readl(itv->gpio_dir), readl(itv->gpio_out));

	/* init output data then direction */
	writel(itv->card->gpio_init.initial_value, itv->gpio_out);
	writel(itv->card->gpio_init.direction, itv->gpio_dir);
}

static struct v4l2_queryctrl gpio_ctrl_mute = {
	.id            = V4L2_CID_AUDIO_MUTE,
	.type          = V4L2_CTRL_TYPE_BOOLEAN,
	.name          = "Mute",
	.minimum       = 0,
	.maximum       = 1,
	.step          = 1,
	.default_value = 1,
	.flags         = 0,
};

int ivtv_gpio(struct ivtv *itv, unsigned int command, void *arg)
{
	struct v4l2_tuner *tuner = arg;
	struct v4l2_control *ctrl = arg;
	struct v4l2_audio *audio = arg;
	int *video_type = arg;
	u16 mask, data;

	switch (command) {
	case VIDIOC_INT_AUDIO_CLOCK_FREQ:
		mask = itv->card->gpio_audio_freq.mask;
		switch (*(u32 *)arg) {
		case 32000:
			data = itv->card->gpio_audio_freq.f32000;
			break;
		case 44100:
			data = itv->card->gpio_audio_freq.f44100;
			break;
		case 48000:
		default:
			data = itv->card->gpio_audio_freq.f48000;
			break;
		}
		break;

	case VIDIOC_G_TUNER:
		mask = itv->card->gpio_audio_detect.mask;
		if (mask == 0 || (readl(itv->gpio_in) & mask))
			tuner->rxsubchans = V4L2_TUNER_MODE_STEREO |
			       V4L2_TUNER_MODE_LANG1 | V4L2_TUNER_MODE_LANG2;
		else
			tuner->rxsubchans = V4L2_TUNER_SUB_MONO;
		return 0;

	case VIDIOC_S_TUNER:
		mask = itv->card->gpio_audio_mode.mask;
		switch (tuner->audmode) {
		case V4L2_TUNER_MODE_LANG1:
			data = itv->card->gpio_audio_mode.lang1;
			break;
		case V4L2_TUNER_MODE_LANG2:
			data = itv->card->gpio_audio_mode.lang2;
			break;
		case V4L2_TUNER_MODE_MONO:
			data = itv->card->gpio_audio_mode.mono;
			break;
		case V4L2_TUNER_MODE_STEREO:
		default:
			data = itv->card->gpio_audio_mode.stereo;
			break;
		}
		break;

	case AUDC_SET_RADIO:
		mask = itv->card->gpio_audio_input.mask;
		data = itv->card->gpio_audio_input.radio;
		break;

	case VIDIOC_S_STD:
		mask = itv->card->gpio_audio_input.mask;
		data = itv->card->gpio_audio_input.tuner;
		break;

	case VIDIOC_S_AUDIO:
		if (audio->index > 2)
			return -EINVAL;
		mask = itv->card->gpio_audio_input.mask;
		switch (audio->index) {
			case 0:
				data = itv->card->gpio_audio_input.tuner;
			       	break;
			case 1:
				data = itv->card->gpio_audio_input.linein;
			       	break;
			case 2:
			default:
				data = itv->card->gpio_audio_input.radio;
			       	break;
		}
		break;

	case VIDIOC_G_CTRL:
		if (ctrl->id != V4L2_CID_AUDIO_MUTE)
			return -EINVAL;
		mask = itv->card->gpio_audio_mute.mask;
		data = itv->card->gpio_audio_mute.mute;
		ctrl->value = (readl(itv->gpio_out) & mask) == data;
		return 0;

	case VIDIOC_S_CTRL:
		if (ctrl->id != V4L2_CID_AUDIO_MUTE)
			return -EINVAL;
		mask = itv->card->gpio_audio_mute.mask;
		data = ctrl->value ? itv->card->gpio_audio_mute.mute : 0;
		break;

	case VIDIOC_QUERYCTRL:
	{
		struct v4l2_queryctrl *qc = arg;

		if (qc->id != V4L2_CID_AUDIO_MUTE)
			return -EINVAL;
		*qc = gpio_ctrl_mute;
		return 0;
	}

	case VIDIOC_LOG_STATUS:
		IVTV_INFO("GPIO status: DIR=0x%04x OUT=0x%04x IN=0x%04x\n",
			readl(itv->gpio_dir), readl(itv->gpio_out), readl(itv->gpio_in));
		return 0;

	case VIDIOC_S_INPUT:
		if (*video_type > 2) // 0:Tuner 1-2:(Composite/S-Video)
			return -EINVAL;
		mask = itv->card->gpio_video_input.mask;
		if  (*video_type == 0)
			data = itv->card->gpio_video_input.tuner;
		else
			data = itv->card->gpio_video_input.linein;
		break;

	default:
		return -EINVAL;
	}
	if (mask)
		ivtv_gpio_and_or(itv->gpio_out, mask, data);
	return 0;
}
