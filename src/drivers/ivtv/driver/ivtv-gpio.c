/*
    gpio functions.
    Copyright (C) 2004  <name of author>

    Merging GPIO support into driver:
    Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org

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
 *   BR* : Audio Bitrate
 *          BR0 BR1
 *           0   0   32 kHz
 *           0   1   44.1 kHz
 *           1   0   48 kHz
 *
 *   DM* : Detected tuner audio Mode
 *         Unknown currently
 *
 * GPIO Initial Settings
 *           MPG600   MPG160
 *     DIR   0x3080   0x7080
 *  OUTPUT   0x000C   0x400C
 *
 * Special thanks to AVerMedia Technologies, Inc. and Jiun-Kuei Jung at
 * AVerMedia for providing the GPIO information used to add support
 * for the M179 cards.
 */

#define IVTV_GPIO_IN_ADDR    0x9008
#define IVTV_GPIO_OUT_ADDR   0x900c
#define IVTV_GPIO_DIREC_ADDR 0x9020

struct ivtv_gpio_data_st {
	int card_type;
	int need_init;		/* need to init GPIO? 1:yes 0:no */
	u32 direction;		/* GPIO direction (GPIO[0..16]) 0:Input 1:Output */
	u32 out_init;		/* GPIO output port init data */
	u32 input_mask;		/* for input selection (tuner/line-in/mute) */
	u32 input_tuner;
	u32 input_line;
	u32 mute_mask;   	/* for muting */
	u32 mute_mute;
	u32 mute_unmute;
	u32 audio_mask;		/* for tuner's audio mode selection */
	u32 audio_mono;
	u32 audio_stereo;
	u32 audio_main;
	u32 audio_sub;
	u32 audio_mixed;
	u32 audio_freq_mask;
	u32 audio_32000;
	u32 audio_44100;
	u32 audio_48000;
	u32 tuner_mask;		/* for tuner audio status */
	u32 tuner_stereo;
	u32 tuner_mono;
	u32 tuner_multiplex;
};

static const struct ivtv_gpio_data_st ivtv_gpio_data[] = {
	/* card_type,need_init,direction,out_init,
	 * input_mask,input_tuner,input_line,mute_mask,mute_mute,mute_unmute,
	 * audio_mask,audio_mono,audio_stereo,audio_main,audio_sub,audio_mixed,
	 * audio_freq_mask, audio_32000, audio_44100, audio_48000,
	 * tuner_mask,tuner_stereo,tuner_mono,tuner_multiples
	 */
	{IVTV_CARD_MPG600, 1, 0x3080, 0x0004, /* Normal/main ch./Stereo */
	 0xcfff, 0x0000, 0x2000, 0xfffe, 0x0001, 0x0000,
	 0xfff1, 0x0006, 0x0004, 0x0004, 0x0000, 0x0008,
	 0xffff, 0x0000, 0x0000, 0x0000,
	 0xf6ff, 0x0100, 0x0900, 0x0100},
	{IVTV_CARD_MPG160, 1, 0x7080, 0x400c, /* Normal/main ch./Stereo */
	 0xcfff, 0x0000, 0x2000, 0xfffe, 0x0001, 0x0000,
	 0xfff1, 0x0006, 0x0004, 0x0004, 0x0000, 0x0008,
	 0xffff, 0x0000, 0x0000, 0x0000,
	 0xf6ff, 0x0100, 0x0900, 0x0100},
	{IVTV_CARD_M179, 1, 0xE380, 0x8290, /* Normal/main ch./Stereo, 48kHz */
	 ~0x8040, 0x8000, 0x0000, ~0x2000, 0x2000, 0x0000,
	 ~0x4300, 0x4000, 0x0200, 0x0200, 0x0100, 0x0000,
	 ~0x0018, 0x0000, 0x0008, 0x0010,
	 ~0x4000, 0x0000, 0x4000, 0x0000},
	{-1}
};

static const struct ivtv_gpio_data_st *ivtv_get_gpio_data(struct ivtv *itv)
{
	int i;

	for (i = 0; ivtv_gpio_data[i].card_type != -1; i++)
		if (ivtv_gpio_data[i].card_type == itv->card->type)
			return &ivtv_gpio_data[i];
	return NULL;
}

/********************* GPIO stuffs *********************/

static void ivtv_gpio_write(u8 *gpio_addr, u32 data)
{
	writel(data, gpio_addr);
}

static u32 ivtv_gpio_read(u8 *gpio_addr)
{
	return readl(gpio_addr);
}

static void ivtv_chg_gpio(u8 *address, u32 mask, u32 write_data)
{
	ivtv_gpio_write(address, (readl(address) & mask) | write_data);
}

static u32 ivtv_chk_gpio(u8 *address, u32 mask)
{
	return ivtv_gpio_read(address) & (mask ^ 0xffff);
}

/* audio control by GPIO */
void ivtv_set_gpio_audio(struct ivtv *itv, int command)
{
	const struct ivtv_gpio_data_st *gpio_data = ivtv_get_gpio_data(itv);

	if (gpio_data == NULL)
		return;

	switch (command) {
	case IVTV_GPIO_AUDIO_INIT:
		if (!gpio_data->need_init) {
			return;
		}

		IVTV_DEBUG(IVTV_DEBUG_INFO,
			   "GPIO initial dir:%08x, out:%08x\n",
			   ivtv_gpio_read(itv->reg_mem + IVTV_GPIO_DIREC_ADDR),
			   ivtv_gpio_read(itv->reg_mem + IVTV_GPIO_OUT_ADDR));

		/* init output data then direction */
		ivtv_gpio_write(itv->reg_mem + IVTV_GPIO_OUT_ADDR, gpio_data->out_init);
		ivtv_gpio_write(itv->reg_mem + IVTV_GPIO_DIREC_ADDR, gpio_data->direction);
		break;
	case IVTV_GPIO_AUDIO_TUNER:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->input_mask, gpio_data->input_tuner);
		break;
	case IVTV_GPIO_AUDIO_LINE:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->input_mask, gpio_data->input_line);
		break;
	case IVTV_GPIO_AUDIO_MUTE:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->mute_mask, gpio_data->mute_mute);
		break;
	case IVTV_GPIO_AUDIO_UNMUTE:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->mute_mask, gpio_data->mute_unmute);
		break;
	}
}

int ivtv_get_gpio_audio(struct ivtv *itv, int command)
{
	const struct ivtv_gpio_data_st *gpio_data = ivtv_get_gpio_data(itv);
	int out, data;

	out = 0;
	data = -1;   /* 'data' must be different from 'out' here */
	
	if (gpio_data == NULL)
		return 0;

	switch (command) {
	case IVTV_GPIO_AUDIO_INIT:
		/* do nothing*/
		break;
	case IVTV_GPIO_AUDIO_TUNER:
		out = ivtv_chk_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
				    gpio_data->input_mask);
		data = gpio_data->input_tuner;
		break;
	case IVTV_GPIO_AUDIO_LINE:
	       out = ivtv_chk_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
				   gpio_data->input_mask);
	       data = gpio_data->input_line;
	       break;
	case IVTV_GPIO_AUDIO_MUTE:
		out = ivtv_chk_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
				    gpio_data->mute_mask);
		data = gpio_data->mute_mute;
		break;
	case IVTV_GPIO_AUDIO_UNMUTE:
		out = ivtv_chk_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
				    gpio_data->mute_mask);
		data = gpio_data->mute_unmute;
		break;
	}

	if (out == data)
		return 1;

	return 0;
}

/* Set tuner audio bitrate. Only the M179 uses this, it
   is ignored by the others. */
void ivtv_set_gpio_audiobitrate(struct ivtv *itv, int bitrate)
{
	const struct ivtv_gpio_data_st *gpio_data = ivtv_get_gpio_data(itv);

	if (gpio_data == NULL)
		return;

	switch (bitrate) {
	case IVTV_AUDIO_32000:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_freq_mask, gpio_data->audio_32000);
		break;
	case IVTV_AUDIO_44100:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_freq_mask, gpio_data->audio_44100);
		break;
	case IVTV_AUDIO_48000:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_freq_mask, gpio_data->audio_48000);
		break;
	}
}

/* set MPG600/MPG160/M179 tuner audio mode */
void ivtv_set_gpio_audiomode(struct ivtv *itv, int audio_mode)
{
	const struct ivtv_gpio_data_st *gpio_data = ivtv_get_gpio_data(itv);
	
	if (gpio_data == NULL)
		return;

	switch (audio_mode) {
	case V4L2_TUNER_MODE_MONO:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_mask, gpio_data->audio_mono);
		break;
	case V4L2_TUNER_MODE_STEREO:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_mask, gpio_data->audio_stereo);
		break;
	case V4L2_TUNER_MODE_LANG1:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_mask, gpio_data->audio_main);
		break;
	case V4L2_TUNER_MODE_LANG2:
		ivtv_chg_gpio(itv->reg_mem + IVTV_GPIO_OUT_ADDR,
			      gpio_data->audio_mask, gpio_data->audio_sub);
		break;
	}

	/* GPIO debug interface */
	if (audio_mode & 0x10000) {
		u32 gpio_dir, gpio_out, gpio_in;

		ivtv_gpio_write(itv->reg_mem + IVTV_GPIO_OUT_ADDR, audio_mode & 0xffff);

		gpio_dir = ivtv_gpio_read(itv->reg_mem + IVTV_GPIO_DIREC_ADDR);
		gpio_out = ivtv_gpio_read(itv->reg_mem + IVTV_GPIO_OUT_ADDR);
		gpio_in = ivtv_gpio_read(itv->reg_mem + IVTV_GPIO_IN_ADDR);

		IVTV_DEBUG(IVTV_DEBUG_INFO,
			   "GPIO status: DIR=0x%04x OUT=0x%04x IN=0x%04x\n",
			   gpio_dir, gpio_out, gpio_in);
	}
}

/* get MPG600/MPG160/M179 tuner audio mode */
int ivtv_get_gpio_audiomode(struct ivtv *itv)
{
	const struct ivtv_gpio_data_st *gpio_data = ivtv_get_gpio_data(itv);
	u32 st;

	if (gpio_data == NULL)
		return 1;

	st = ivtv_chk_gpio(itv->reg_mem + IVTV_GPIO_IN_ADDR, gpio_data->tuner_mask);

	if (st == gpio_data->tuner_multiplex)
		return 2;
	if (st == gpio_data->tuner_stereo)
		return 1;
	return 0; /* mono */
}
