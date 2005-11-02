/*
    cx25840 audio functions

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

#include "compat.h"
#include <linux/i2c.h>

#include "decoder.h"

#include "cx25840-driver.h"
#include "cx25840-audio.h"
#include "cx25840-registers.h"
#include "cx25840.h"

int cx25840_set_audio_input(struct i2c_client *client,
			    struct decoder_state *state, int audio_input)
{
	if (audio_input <= 4) {
		CX25840_DEBUG("set audio input (%d)\n", audio_input);

                cx25840_and_or(client, 0x803, ~0x10, 0); // stop microcontroller

		// Mute everything to prevent the PFFT!
                cx25840_write(client, 0x8D3, 0x1F);     // Mute control

		switch (audio_input) {
		case 0:	// TV audio
                        // Set Path1 to Analog Demod Main Channel
                        cx25840_write4(client, 0x8D0, 0x7038061F);

			// When the microcontroller detects the
			// audio format, it will unmute the lines       
                        cx25840_and_or(client, 0x803, ~0x10, 0x10); // start microcontroller
			break;

		case 1:	// external line-in (???)
		case 2:	// external line-in (external)
		case 3:	// external line-in (internal)
		case 4:	// FM Radio
                        // Set Path1 to Serial Audio Input
                        cx25840_write4(client, 0x8D0, 0x12100101);

                        // The microcontroller should not be started for the
                        // non-tuner inputs: autodetection is specific for
                        // TV audio.
			break;
		}

		state->audio_input = audio_input;
		return 0;
	}

	CX25840_DEBUG("Invalid audio input selection %d\n", audio_input);
	return -EINVAL;
}

int cx25840_set_audio_rate(struct i2c_client *client,
			   struct decoder_state *state, int audio)
{
	if ((state->audio_input > 4) || (state->audio_input < 0)) {
		CX25840_DEBUG("Invalid audio input selected %d\n", state->audio_input);
		return -EINVAL;
	}
        cx25840_and_or(client, 0x810, ~1, 1); // assert soft reset

	// common for all inputs and rates
        cx25840_write(client, 0x127, 0x50);  // SA_MCLK_SEL=1, SA_MCLK_DIV=0x10

	switch (state->audio_input) {
	case 0:		// TV audio
		switch (audio) {
		case DECODER_AUDIO_32_KHZ:
                        // VID_PLL and AUX_PLL
                        cx25840_write4(client, 0x108, 0x0f040610);

			// AUX_PLL_FRAC
                        cx25840_write4(client, 0x110, 0xee39bb01);

			// src3/4/6_ctl = 0x0801f77f;
                        cx25840_write4(client, 0x900, 0x7ff70108);
                        cx25840_write4(client, 0x904, 0x7ff70108);
                        cx25840_write4(client, 0x90C, 0x7ff70108);
			break;

		case DECODER_AUDIO_441_KHZ:
                        // VID_PLL and AUX_PLL
                        cx25840_write4(client, 0x108, 0x0f040910);

			// AUX_PLL_FRAC
                        cx25840_write4(client, 0x110, 0xd66bec00);

			// src3/4/6_ctl = 0x08016d59;
                        cx25840_write4(client, 0x900, 0x596d0108);
                        cx25840_write4(client, 0x904, 0x596d0108);
                        cx25840_write4(client, 0x90C, 0x596d0108);
			break;
		case DECODER_AUDIO_48_KHZ:
                        // VID_PLL and AUX_PLL
                        cx25840_write4(client, 0x108, 0x0f040a10);

			// AUX_PLL_FRAC
                        cx25840_write4(client, 0x110, 0xe5d69800);

			// src3/4/6_ctl = 0x08014faa;
                        cx25840_write4(client, 0x900, 0xaa4f0108);
                        cx25840_write4(client, 0x904, 0xaa4f0108);
                        cx25840_write4(client, 0x90C, 0xaa4f0108);
			break;
		}
		break;

	case 1:		// external line in (???)
	case 2:		// external line in (external)
	case 3:		// external line in (internal)
	case 4:		// FM radio
		switch (audio) {
		case DECODER_AUDIO_32_KHZ:
                        // VID_PLL and AUX_PLL
                        cx25840_write4(client, 0x108, 0x0f04081e);

			// AUX_PLL_FRAC
                        cx25840_write4(client, 0x110, 0x69082a01);

			// src1_ctl = 0x08010000;
                        cx25840_write4(client, 0x8F8, 0x00000108);

			// src3/4/6_ctl = 0x08020000;
                        cx25840_write4(client, 0x900, 0x00000208);
                        cx25840_write4(client, 0x904, 0x00000208);
                        cx25840_write4(client, 0x90C, 0x00000208);

                        // SA_MCLK_SEL=1, SA_MCLK_DIV=0x14
                        cx25840_write(client, 0x127, 0x54);
			break;

		case DECODER_AUDIO_441_KHZ:
                        // VID_PLL and AUX_PLL
                        cx25840_write4(client, 0x108, 0x0f040918);

			// AUX_PLL_FRAC
                        cx25840_write4(client, 0x110, 0xd66bec00);

			// src1_ctl = 0x08010000;
                        cx25840_write4(client, 0x8F8, 0xcd600108);

			// src3/4/6_ctl = 0x08020000;
                        cx25840_write4(client, 0x900, 0x85730108);
                        cx25840_write4(client, 0x904, 0x85730108);
                        cx25840_write4(client, 0x90C, 0x85730108);
			break;

		case DECODER_AUDIO_48_KHZ:
                        // VID_PLL and AUX_PLL
                        cx25840_write4(client, 0x108, 0x0f040a18);

			// AUX_PLL_FRAC
                        cx25840_write4(client, 0x110, 0xe5d69800);

			// src1_ctl = 0x08010000;
                        cx25840_write4(client, 0x8F8, 0x00800108);

			// src3/4/6_ctl = 0x08020000;
                        cx25840_write4(client, 0x900, 0x55550108);
                        cx25840_write4(client, 0x904, 0x55550108);
                        cx25840_write4(client, 0x90C, 0x55550108);
			break;
		}
		break;
	}

	// deassert soft reset  
        cx25840_and_or(client, 0x810, ~1, 0); // assert soft reset

	state->audio = audio;

	return 0;
}

void cx25840_get_v4l_audio(struct i2c_client *client, struct video_audio *va)
{
	/* MUTABLE isn't included, although it is possible if
	 * we stop the microcontroller and mute all inputs */
	va->flags = VIDEO_AUDIO_TREBLE | VIDEO_AUDIO_BASS | 
                    VIDEO_AUDIO_MUTABLE | VIDEO_AUDIO_VOLUME;
	
	/* Volume runs +18dB to -96dB in 1/2dB steps
	 * change to fit the msp3400 -114dB to +12dB range */
	va->step = 228;
	va->volume = 228 - cx25840_read(client, 0x8D4);   // PATH1_VOLUME
	va->volume = (va->volume / 2) + 23; 
	va->volume = va->volume << 9;
	/* bass is 49 steps +12dB to -12dB */
	va->bass = cx25840_read(client, 0x8D9) & 0x3f;    // PATH1_EQ_BASS_VOL
	va->bass = (((48 - va->bass) * 0xffff) + 47) / 48;
	/* treble is 49 steps +12dB to -12dB */
	va->treble = cx25840_read(client, 0x8DB) & 0x3f;  // PATH1_EQ_TREBLE_VOL
	va->treble = (((48 - va->treble) * 0xffff) + 47) / 48;
	/* balance is 7 bit, 0 to -96dB */
	va->balance = cx25840_read(client, 0x8D5) & 0x7f; // PATH1_BAL_LEVEL
	if ((cx25840_read(client, 0x8D5) & 0x80) == 0)    // PATH1_BAL_LEFT
		va->balance = 0x80 - va->balance;
	else
		va->balance = 0x80 + va->balance;
	va->balance = va->balance << 8;
        if (cx25840_read(client, 0x8D3) & 2) // SRC1_MUTE_EN
                va->flags |= VIDEO_AUDIO_MUTE;
}

void cx25840_set_v4l_audio(struct i2c_client *client, struct video_audio *va)
{
        struct cx25840_info *info = i2c_get_clientdata(client);

	// First convert the volume to msp3400 values (0-127)
	int volume = va->volume >> 9;
	int balance = va->balance >> 8;
	// now scale it up to cx25840 values
	// -114dB to -96dB maps to 0
	// this should be 19, but in my testing that was 4dB too loud
	if (volume <= 23)
		volume = 0;
	else 
		volume -= 23;

	cx25840_write(client, 0x8D4, 228 - (volume * 2));                      // PATH1_VOLUME
        cx25840_and_or(client, 0x8D9, ~0x3f, 48 - (va->bass * 48 / 0xffff));   // PATH1_EQ_BASS_VOL
	cx25840_and_or(client, 0x8DB, ~0x3f, 48 - (va->treble * 48 / 0xffff)); // PATH1_EQ_TREBLE_VOL
	if (balance > 0x80) {
		cx25840_and_or(client, 0x8D5, 0x7f, 0x80);                    // PATH1_BAL_LEFT
		cx25840_and_or(client, 0x8D5, ~0x7f, balance & 0x7f);          // PATH1_BAL_LEVEL
	} else {
		cx25840_and_or(client, 0x8D5, 0x7f, 0);                       // PATH1_BAL_LEFT
		cx25840_and_or(client, 0x8D5, ~0x7f, 0x80 - balance);          // PATH1_BAL_LEVEL
	}
        if (info->state.audio_input == 0) {
                // Must turn off microcontroller in order to mute sound.
                // Not sure if this is the best method, but it does work.
                // If the microcontroller is running, then it will undo any
                // changes to the mute register.
                if (va->flags & VIDEO_AUDIO_MUTE) {
                        // disable microcontroller
                        cx25840_and_or(client, 0x803, ~0x10, 0);
                        cx25840_write(client, 0x8D3, 0x1f);
                }
                else {
                        // enable microcontroller
                        cx25840_and_or(client, 0x803, ~0x10, 0x10);
                }
        }
        else {
                // SRC1_MUTE_EN
                cx25840_and_or(client, 0x8D3, ~2, (va->flags & VIDEO_AUDIO_MUTE) ? 2 : 0);
        }
}

