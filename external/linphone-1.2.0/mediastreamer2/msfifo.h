/***************************************************************************
 *            msfifo.h
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

#ifndef MSFIFO_H
#define MSFIFO_H

#ifdef HAVE_GLIB
#include <glib.h>
#else
#include "glist.h"
#endif


#include "msbuffer.h"
#include "mslink.h"


typedef struct _MSFifo
{
	MSLink link;
	gint r_gran;  					/*maximum granularity for reading*/
	gint w_gran;						/*maximum granularity for writing*/
	gchar * rd_ptr;        /* read pointer on the position where there is something to read on the MSBuffer */
	guint32 readsize;
	gchar * wr_ptr;
	gchar * prev_wr_ptr;
	guint32 writesize;      /* write pointer on the position where it is possible to write on the MSBuffer */
	gchar * begin;  /* rd_ptr et wr_ptr must all be >=begin*/
	guint32 size;        /* the length of the fifo, but this may not be equal to buffer->size*/
	guint32 saved_offset;
	gchar * pre_end;  /* the end of the buffer that is copied at the begginning when we wrap around*/
	gchar * w_end;    /* when a wr ptr is expected to exceed end_offset,
											it must be wrapped around to go at the beginning of the buffer. This is the end of the buffer*/
	gchar * r_end;    /* this is the last position written at the end of the fifo. If a read ptr is expected to
											exceed this pointer, it must be put at the begginning of the buffer */
	gint bpt;	/* bytes per tick */
	MSBuffer *buffer;
} MSFifo;


MSFifo * ms_fifo_new(struct _MSFilter *f1, gint pin1, struct _MSFilter *f2, int pin2 );

void ms_fifo_destroy( MSFifo *fifo);
void ms_fifo_parametrize(MSFifo *fifo, gint r_gran, gint w_gran, gint r_offset, gint w_offset,
																 gint min_fifo_size);

/* public: */
/* get data to read */
gchar * ms_fifo_get_read_ptr(MSFifo *fifo, gint bsize);

/* get a buffer to write*/
gchar * ms_fifo_get_write_ptr(MSFifo *fifo, gint bsize);

/* in case you didn't write as much as expected, tell it using this:*/
void ms_fifo_update_write_ptr(MSFifo *fifo, gint written);

#endif
