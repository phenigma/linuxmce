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

#ifndef __IVTV_FUNCTIONS_H
#define __IVTV_FUNCTIONS_H

#define VIDIOC_STREAMOFF    _IOW  ('V', 19, int)

static inline int ivtv_api_enc_endgop(int ivtvfd, int gop)
{
	/* Send Stop at end of GOP API */
	if (ioctl(ivtvfd, IVTV_IOC_S_GOP_END, &gop) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_S_GOP_END failed\n");
	return 0;
}

static inline int ivtv_api_enc_stop(int ivtvfd)
{
	int dummy = 0;
	/* Send Stop Capture API */
	if (ioctl(ivtvfd, VIDIOC_STREAMOFF, &dummy) < 0)
		fprintf(stderr, "ioctl: VIDIOC_STREAMOFF failed\n");
	return 0;
}

static inline int ivtv_api_dec_pause(int ivtvfd, int blackframe)
{
	/* Send Pause Decoding API */
	if (ioctl(ivtvfd, IVTV_IOC_PAUSE, &blackframe) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_PAUSE failed\n");
	return 0;
}

static inline int ivtv_api_dec_play(int ivtvfd, int firstframe)
{
	/* Send Playback Decoding API */
	if (ioctl(ivtvfd, IVTV_IOC_PLAY, &firstframe) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_PLAY failed\n");
	return 0;
}

static inline int ivtv_api_dec_start(int ivtvfd)
{
	/* Send Start Decoding API */
	struct ivtv_cfg_start_decode sd;
	sd.gop_offset = 0;
	sd.muted_audio_frames = 0;
	if (ioctl(ivtvfd, IVTV_IOC_START_DECODE, &sd) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_START_DECODE failed\n");
	return 0;
}

static inline int ivtv_api_dec_splice(int ivtvfd, int offset, int mframes)
{
	/* Send Start Decoding API */
	struct ivtv_cfg_start_decode sd;
	sd.gop_offset = offset;
	sd.muted_audio_frames = mframes;
	if (ioctl(ivtvfd, IVTV_IOC_START_DECODE, &sd) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_START_DECODE failed\n");
	return 0;
}

static inline int ivtv_api_dec_stop(int ivtvfd, int blackframe, int ptslo,
				    int ptshi)
{
	/* Send Playback Stop Decoding API */
	struct ivtv_cfg_stop_decode sd;
	sd.hide_last = blackframe;
	sd.pts_stop = ptshi;
	if (ioctl(ivtvfd, IVTV_IOC_STOP_DECODE, &sd) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_STOP_DECODE failed\n");
	return 0;
}

static inline int ivtv_api_dec_flush(int ivtvfd)
{
	/* Send Flush Decoding Buffers API */
	if (ioctl(ivtvfd, IVTV_IOC_DEC_FLUSH, 0) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_DEC_FLUSH failed\n");
	return 0;
}

static inline int ivtv_api_dec_step(int ivtvfd, int mode)
{
	/* Send Playback Stop Decoding API */
	if (ioctl(ivtvfd, IVTV_IOC_DEC_STEP, &mode) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_DEC_STEP failed\n");
	return 0;
}

static inline int __ivtv_api_dec_speed(int ivtvfd, int scale, int smooth,
				       int _speed, int direction, int fr_mask,
				       int b_per_gop, int aud_mute,
				       int fr_field, int mute)
{
	struct ivtv_speed speed;

	speed.scale = scale;
	speed.smooth = smooth;
	speed.speed = _speed;
	speed.direction = direction;
	speed.fr_mask = fr_mask;
	speed.b_per_gop = b_per_gop;
	speed.aud_mute = aud_mute;
	speed.fr_field = fr_field;
	speed.mute = mute;

	if (ioctl(ivtvfd, IVTV_IOC_S_SPEED, &speed) < 0)
		fprintf(stderr, "ioctl: IVTV_IOC_S_SPEED failed\n");
	return 0;
}

#define FORWARD 0
#define REVERSE 1

#define NORMAL_SPEED 30
static inline int ivtv_api_dec_speed(int ivtvfd, int direction, float fps)
{
	int _speed = (int)fps;
	int b_per_gop = 1;
	int aud_mute = 0;
	int fr_field = 0;
	int mute = 0;

	int i_frames = 1;
	int ip_frames = 3;
	int ipb_frames = 7;

	/* Make input sane */
	if (direction > 1)
		direction = 1;
	else if (direction < 0)
		direction = 0;

	if (_speed < FORWARD)
		_speed = fps = NORMAL_SPEED;

	if (direction == 0) {	/* Forward */
		if (fps < NORMAL_SPEED) {
			_speed = NORMAL_SPEED / fps;
			__ivtv_api_dec_speed(ivtvfd,
					     _speed, 0, 0, direction,
					     ipb_frames, 0, aud_mute, fr_field,
					     mute);
		} else if (fps == NORMAL_SPEED) {
			__ivtv_api_dec_speed(ivtvfd,
					     0, 0, 0, direction, ipb_frames, 0,
					     aud_mute, fr_field, mute);
		} else if (fps == (1.5 * NORMAL_SPEED)) {
			_speed = 1;
			__ivtv_api_dec_speed(ivtvfd,
					     _speed, 0, 1, direction,
					     ipb_frames, 0, aud_mute, fr_field,
					     mute);
		} else if (fps > (2 * NORMAL_SPEED)) {
			__ivtv_api_dec_speed(ivtvfd,
					     0, 0, 0, direction, i_frames, 0,
					     aud_mute, fr_field, mute);
		} else {
			_speed = 2;
			__ivtv_api_dec_speed(ivtvfd,
					     _speed, 0, 1, direction,
					     ipb_frames, 0, aud_mute, fr_field,
					     mute);
		}
	} else {		/* Reverse */
		if (fps < NORMAL_SPEED) {
			_speed = NORMAL_SPEED / fps;
			__ivtv_api_dec_speed(ivtvfd,
					     _speed, 0, 0, direction, ip_frames,
					     b_per_gop, aud_mute, fr_field,
					     mute);
		} else if (fps == NORMAL_SPEED) {
			__ivtv_api_dec_speed(ivtvfd,
					     0, 0, 0, direction, ip_frames,
					     b_per_gop, aud_mute, fr_field,
					     mute);
		} else if (fps == (1.5 * NORMAL_SPEED)) {	/* 1.5x */
			_speed = 1;
			__ivtv_api_dec_speed(ivtvfd,
					     _speed, 0, 1, direction, ip_frames,
					     b_per_gop, aud_mute, fr_field,
					     mute);
		} else if (fps > (2 * NORMAL_SPEED)) {
			__ivtv_api_dec_speed(ivtvfd,
					     0, 0, 0, direction, i_frames,
					     b_per_gop, aud_mute, fr_field,
					     mute);
		} else {
			_speed = 2;
			__ivtv_api_dec_speed(ivtvfd,
					     _speed, 0, 1, direction,
					     ipb_frames, b_per_gop, aud_mute,
					     fr_field, mute);
		}
	}
	return 0;
}

#endif
