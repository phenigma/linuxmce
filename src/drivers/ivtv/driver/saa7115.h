/*
    saa7115 API header
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

    VBI additions:
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

struct saa7115_state {
	int norm;
	int input;
	int enable;
	int bright;
	int contrast;
	int hue;
	int sat;
	int playback;
	int audio;
};

struct saa7115_reg {
	u8 reg;
	u8 val;
};

/* Possible audio bit rates for DECODER_SET_AUDIO */
#define DECODER_AUDIO_441_KHZ	0
#define DECODER_AUDIO_48_KHZ	1
#define DECODER_AUDIO_32_KHZ	2

/* Need to be able to set the audio bitrates */
#define DECODER_SET_AUDIO   _IOW('d', 100, int)

/* Digitizer Reset */
#define DECODER_RESET       _IO ('d', 101)

#define DECODER_SET_VBI     _IOW('d', 102, int)
#define DECODER_SET_SIZE    _IOW('d', 103, struct video_window)
#define	DECODER_GET_PICTURE _IOR('d', 104, struct saa7115_state)
#define	DECODER_SET_REG     _IOR('d', 105, struct saa7115_reg)
#define	DECODER_GET_REG     _IOWR('d', 106, struct saa7115_reg)
#define DECODER_GET_WSS     _IOR('d', 107, int *)
#define DECODER_GET_CC_ODD  _IOR('d', 108, int *)
#define DECODER_GET_CC_EVEN _IOR('d', 109, int *)
