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

int ivtv_audio_set_io(struct ivtv *itv);
void ivtv_audio_set_volume(struct ivtv *itv, int volume);
void ivtv_audio_set_mute(struct ivtv *itv, int mute);
int ivtv_audio_get_volume(struct ivtv *itv);
int ivtv_audio_get_mute(struct ivtv *itv);
void ivtv_audio_freq_changed(struct ivtv *itv);
int ivtv_audio_set_matrix(struct ivtv *itv, struct ivtv_msp_matrix *matrix);
void ivtv_audio_set_channel(struct ivtv *itv, struct video_channel *v);
void ivtv_audio_set_audmode(struct ivtv *itv, u32 audmode);
u32 ivtv_audio_get_rxsubchans(struct ivtv *itv, struct v4l2_tuner *vt);
void ivtv_audio_set_bitrate(struct ivtv *itv, int bitrate);

