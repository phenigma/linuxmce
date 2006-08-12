/***************************************************************************
 *            mslink.h
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

#ifndef MSLINK_H
#define MSLINK_H

struct _MSLink{
	struct _MSFilter *prev;   /*the writing MSFilter*/
	struct _MSFilter *next;   /*the reading MSFilter */
	guchar prevpin;
	guchar nextpin;
	guchar pad[2];
};

typedef struct _MSLink MSLink;

void ms_link_init(MSLink *l, struct _MSFilter *f1, gint pin1, struct _MSFilter *f2, int pin2 );

#define MS_LINK_INPUT 0
#define MS_LINK_OUTPUT 1
#define MS_LINK_TYPE_FIFO 0
#define MS_LINK_TYPE_QUEUE 1

struct _MSLinkParams{
	int maxreadgran;
	int maxwritegran;
	int maxreadoff;
	int maxwriteoff;
	char dir;
	char type;
	guchar pin;
};

typedef struct _MSLinkParams MSLinkParams;
#endif
