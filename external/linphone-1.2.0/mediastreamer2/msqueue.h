/***************************************************************************
 *            msqueue.h
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
#ifndef MSQUEUE_H
#define MSQUEUE_H

#include "msbuffer.h"
#include "mslink.h"

/* for the moment these are stupid queues limited to one element*/

typedef struct _MSQueue
{
	MSLink link;
	MSMessage *first;
	MSMessage *last;
	gint size;
}MSQueue;


MSQueue * ms_queue_new(struct _MSFilter *f1, gint pin1, struct _MSFilter *f2, int pin2 );

MSMessage *ms_queue_get(MSQueue *q);

void ms_queue_put(MSQueue *q, MSMessage *m);

#define ms_queue_can_get(q) ( (q)->size!=0 )

#define ms_queue_destroy(q) g_free(q)


#endif
