/*
    gpio functions.
    Copyright (C) 2004  <name of author>

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

/* MPG600/MPG160/M179 audio stuff */
#define IVTV_GPIO_AUDIO_INIT    0
#define IVTV_GPIO_AUDIO_TUNER   1
#define IVTV_GPIO_AUDIO_LINE    2
#define IVTV_GPIO_AUDIO_MUTE    3
#define IVTV_GPIO_AUDIO_UNMUTE  4

/* GPIO stuff */
void ivtv_set_gpio_audio(struct ivtv *itv, int command);
int ivtv_get_gpio_audio(struct ivtv *itv, int command);
void ivtv_set_gpio_audiomode(struct ivtv *itv, int audio_mode);
int ivtv_get_gpio_audiomode(struct ivtv *itv);
void ivtv_set_gpio_audiobitrate(struct ivtv *itv, int bitrate);
