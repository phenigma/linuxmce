/*
  The mediastreamer library aims at providing modular media processing and I/O
	for linphone, but also for any telephony application.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org
  										
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef MS_SYNC_H
#define MS_SYNC_H


#include "msfilter.h"

struct _MSSync
{
	GMutex *lock;
	GCond *cond;
	GList *execution_list;     /* the list of source filters to be executed.*/
	GThread * thread;   /* the thread ressource*/
	gint interval; /* in miliseconds*/
	gint exec_id;
	guint32 ticks;
	guint32 time;	/* a time since the start of the sync expressed in milisec*/
	gboolean run;       /* flag to indicate whether the sync must be run or not */
};

typedef struct _MSSync MSSync;
	

MSSync *ms_sync_new();

/**
 * function_name:ms_sync_attach
 * @sync:  A #MSSync object.
 * @f:  A #MSFilter object.
 *
 * Attach a chain of filters to a synchronisation source. Filter @f must be the first filter of the processing chain.
 *
 * Returns: 0 if successfull, a negative value reprensenting the errno.h error.
 */
int ms_sync_attach(MSSync *sync,MSFilter *f);

/**
 * ms_sync_detach:
 * @sync:  A #MSSync object.
 * @f:  A #MSFilter object.
 *
 * Dettach a chain of filters to a synchronisation source.
 * The processing chain will no more be executed.
 *
 * Returns: 0 if successfull, a negative value reprensenting the errno.h error.
 */
int ms_sync_detach(MSSync *sync,MSFilter *f);

void ms_sync_destroy(MSSync *sync);


/* private functions:*/




#endif
