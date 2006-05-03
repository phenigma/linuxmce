/*
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

#ifndef __ENCODER_H
#define __ENCODER_H

int setup_encoder(int dev, int settings[]);

/* Encoder Settings */
#define BFRAMES 3
#define GOPCL   1

int setup_encoder(int fd, int settings[])
{
	struct ivtv_ioctl_codec in_codec;
	struct v4l2_control ctrl;	/* set_ctrl/get_ctrls */

	/* Open Device */
	if (fd < 0) {
		fprintf(stderr, "Device not open\n");
		return -1;
	}

	/* Setup Brightness, Contrast, Saturation, Hue */
	ctrl.id = V4L2_CID_BRIGHTNESS;
	ctrl.value = settings[11];
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0)
		fprintf(stderr, "ioctl: VIDIOC_S_CTRL failed\n");

	ctrl.id = V4L2_CID_CONTRAST;
	ctrl.value = settings[12];
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0)
		fprintf(stderr, "ioctl: VIDIOC_S_CTRL failed\n");

	ctrl.id = V4L2_CID_SATURATION;
	ctrl.value = settings[13];
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0)
		fprintf(stderr, "ioctl: VIDIOC_S_CTRL failed\n");

	ctrl.id = V4L2_CID_HUE;
	ctrl.value = settings[14];
	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0)
		fprintf(stderr, "ioctl: VIDIOC_S_CTRL failed\n");

	/* Setup Input Port */
	if (ioctl(fd, VIDIOC_S_INPUT, &settings[15]) < 0)
		fprintf(stderr, "ioctl: VIDIOC_S_INPUT failed\n");

	/* Setup Codecs */
	if (ioctl(fd, IVTV_IOC_G_CODEC, &in_codec) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_G_CODEC failed\n");
	else {
		in_codec.aspect = settings[0];
		in_codec.bitrate_mode = settings[1];
		in_codec.bitrate = settings[2];
		in_codec.bitrate_peak = settings[3];
		in_codec.stream_type = settings[4];
		in_codec.audio_bitmask = settings[5];
		in_codec.dnr_mode = settings[6];
		in_codec.dnr_type = settings[7];
		in_codec.dnr_spatial = settings[8];
		in_codec.dnr_temporal = settings[9];
		in_codec.pulldown = settings[10];
		in_codec.framerate = settings[16];
		in_codec.framespergop = settings[17];
		in_codec.bframes = BFRAMES;
		in_codec.gop_closure = GOPCL;
		if (ioctl(fd, IVTV_IOC_S_CODEC, &in_codec) < 0)
			fprintf(stderr, "ioctl: IVTV_IOC_S_CODEC failed\n");
	}
	return 0;
}

#endif
