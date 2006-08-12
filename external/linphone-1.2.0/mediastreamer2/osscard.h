/***************************************************************************
 *            osscard.h
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

/* An implementation of SndCard : the OssCard */

#ifndef OSS_CARD_H
#define OSS_CARD_H

#include "sndcard.h"

#define OSS_CARD_BUFFERS 3
struct _OssCard
{
	SndCard parent;
	gchar *dev_name;            /* /dev/dsp0 for example */
	gchar *mixdev_name;         /* /dev/mixer0 for example */
	gint fd;   /* the file descriptor of the open soundcard, 0 if not open*/
	gint ref;
	gchar *readbuf;
	gint readpos;
	gchar *writebuf;
	gint writepos; 
};

typedef struct _OssCard OssCard;
typedef OssCard HpuxSndCard;	
SndCard * oss_card_new(char *devname, char *mixdev_name);

#endif
