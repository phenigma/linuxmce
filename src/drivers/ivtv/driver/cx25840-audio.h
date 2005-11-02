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

#include <linux/i2c.h>

#include "decoder.h"

int cx25840_set_audio_input(struct i2c_client *client,
			    struct decoder_state *state, int audio_input);
int cx25840_set_audio_rate(struct i2c_client *client,
			   struct decoder_state *state, int audio);
void cx25840_get_v4l_audio(struct i2c_client *client, struct video_audio *va);
void cx25840_set_v4l_audio(struct i2c_client *client, struct video_audio *va);
