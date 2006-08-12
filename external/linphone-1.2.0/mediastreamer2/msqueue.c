/***************************************************************************
 *            msqueue.c
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
#include "msqueue.h"
#include <string.h>

void ms_link_init(MSLink *l, struct _MSFilter *f1, gint pin1, struct _MSFilter *f2, int pin2 )
{
	l->prev=f1;
	l->prevpin=pin1;
	l->next=f2;
	l->nextpin=pin2;
}

MSQueue * ms_queue_new(struct _MSFilter *f1, gint pin1, struct _MSFilter *f2, int pin2 )
{
	MSQueue *q=g_malloc0(sizeof(MSQueue));
	ms_link_init(&q->link,f1,pin1,f2,pin2);

	return q;
}

MSMessage *ms_queue_get(MSQueue *q)
{
	MSMessage *b=q->last;
	if (b==NULL) return NULL;
	q->last=b->prev;
	if (b->prev==NULL) q->first=NULL; /* it was the only element of the queue*/
     q->size--;
     b->next=b->prev=NULL;
	return(b);
}

void ms_queue_put(MSQueue *q, MSMessage *m)
{
   MSMessage *mtmp=q->first;
   g_return_if_fail(m!=NULL);
   q->first=m;
   m->next=mtmp;
   if (mtmp!=NULL)
   {
      mtmp->prev=m;
   }
   else q->last=m; /* it was the first element of the q */
   q->size++;
}
