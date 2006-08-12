/***************************************************************************
 *            msringplayer.h
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


#ifndef MSRINGPLAYER_H
#define MSRINGPLAYER_H

#include "msfilter.h"
#include "mssync.h"


#define MS_RING_PLAYER_DEF_GRAN 8192 /* the default granularity*/

#define MS_RING_PLAYER_END_OF_RING_EVENT 1

struct _MSRingPlayer
{
	MSFilter filter;
	gint gran;
	gint freq;
	gint rate;
	gint channel;	/* number of interleaved channels */
	gint silence;	/* silence time between each ring, in seconds */
	gint state;
	gint fd;  /* the file descriptor of the file being read*/
	gint silence_bytes; /*silence in number of bytes between each ring */
	gint current_pos;
	gint need_swap;
};

typedef struct _MSRingPlayer MSRingPlayer;


/* PUBLIC */
#define MS_RING_PLAYER(filter) ((MSRingPlayer*)(filter))
MSFilter * ms_ring_player_new(char *name, gint seconds);
gint ms_ring_player_get_sample_freq(MSRingPlayer *obj);


/* FOR INTERNAL USE*/
void ms_ring_player_init(MSRingPlayer *r);
void ms_ring_player_uninit( MSRingPlayer *obj);
void ms_ring_player_process(MSRingPlayer *r);
#define ms_ring_player_set_bufsize(filter,sz) (filter)->gran=(sz)
void ms_ring_player_setup(MSRingPlayer *r,MSSync *sync);
#endif
