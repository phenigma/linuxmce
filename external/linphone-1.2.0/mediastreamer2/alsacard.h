/***************************************************************************
 *            alsacard.h
 *
 *  Fri Oct 15 16:13:08 2004
 *  Copyright  2004  Simon Morlat 
 *  Email <simon.morlat@linphone.org>
 ****************************************************************************/
/* The mediastreamer library creates and run multimedia streams in a modular
	way.
*/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <config.h>

#ifdef HAVE_ALSA_ASOUNDLIB_H

#include "sndcard.h"
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
struct _AlsaCard
{
	SndCard parent;
	gchar *pcmdev;
	gchar *mixdev;
	snd_pcm_t *read_handle;
	snd_pcm_t *write_handle;
	gint frame_size;
	gint frames;
	gchar *readbuf;
	gint readpos;
	gchar *writebuf;
	gint writepos; 
	snd_mixer_t *mixer;
};

typedef struct _AlsaCard AlsaCard;

SndCard *alsa_card_new(gint dev_id);
gint alsa_card_manager_init(SndCardManager *m, gint index);
void alsa_card_manager_set_default_pcm_device(gchar *pcmdev);

#endif
