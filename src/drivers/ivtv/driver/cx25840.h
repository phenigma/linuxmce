/*
    cx25840 API header
    Copyright (C) 2003-2004  Chris Kennedy

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

struct get_picture_struct {
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

struct cx25840_reg 
{
	/** Index of register if known. Otherwise -1 for unknown (on set). */
	int	index;
	/** Setting name. If settingIndex is known on set, this value is ignored. */
	char	name[ 80 ];
	/** Setting value. */
	int	value;
	/** Value description (for get). */
	char	description[ 120 ];
};



#define DECODER_AUDIO_441_KHZ	0
#define DECODER_AUDIO_48_KHZ	1
#define DECODER_AUDIO_32_KHZ	2

#define DECODER_SET_AUDIO   _IOW('d', 100, int)
#define DECODER_RESET       _IO ('d', 101)
#define DECODER_SET_VBI     _IOW('d', 102, int)
#define DECODER_SET_SIZE    _IOW('d', 103, struct video_window)
#define	DECODER_GET_PICTURE _IOR('d', 104, struct get_picture_struct)
/** NOTE: A copy of this declaration exists in ivtv-ioctl.c */
#define	DECODER_SET_REG_TEXT _IOR('d', 105, struct cx25840_reg)
/** NOTE: A copy of this declaration exists in ivtv-ioctl.c */
#define	DECODER_GET_REG_TEXT _IOWR('d', 106, struct cx25840_reg)
#define DECODER_GET_WSS     _IOR('d', 107, int *)
#define DECODER_GET_CC_ODD  _IOR('d', 108, int *)
#define DECODER_GET_CC_EVEN _IOR('d', 109, int *)

#define AUDIO_FW_FILES {      \
  "/lib/modules/HcwMakoA.ROM" \
}
