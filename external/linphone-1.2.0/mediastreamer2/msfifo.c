/***************************************************************************
 *            msfifo.c
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

#include <errno.h>
#include <string.h>
#include "msutils.h"
#include "msfifo.h"

MSFifo * ms_fifo_new(struct _MSFilter *f1, gint pin1, struct _MSFilter *f2, int pin2 ){
	MSFifo *fifo;
	fifo=g_malloc0(sizeof(MSFifo));
	ms_link_init(&fifo->link,f1,pin1,f2,pin2);
	return fifo;
}

void __ms_fifo_parametrize_with_buffer(MSFifo *fifo, MSBuffer *buf,gint r_gran, gint w_gran, gint r_offset, gint w_offset)
{
	gint saved_offset=MAX(r_gran+r_offset,w_offset);
	
	g_return_if_fail(saved_offset<=(buf->size));
	/* delete previous buffer; if any */
	if (fifo->buffer!=NULL) ms_buffer_destroy(fifo->buffer);
	fifo->buffer=buf;
	fifo->r_gran=r_gran;
	fifo->w_gran=w_gran;
	fifo->begin=fifo->wr_ptr=fifo->rd_ptr=buf->buffer+saved_offset;
	fifo->readsize=0;
	fifo->size=fifo->writesize=buf->size-saved_offset;
	fifo->saved_offset= saved_offset;
	fifo->r_end=fifo->w_end=buf->buffer+buf->size;
	fifo->pre_end=fifo->w_end-saved_offset;
	ms_trace("fifo base=%x, begin=%x, end=%x, saved_offset=%i, size=%i"
			,fifo->buffer->buffer,fifo->begin,fifo->w_end,fifo->saved_offset,fifo->size);
}

void ms_fifo_parametrize(MSFifo *fifo, gint r_gran, gint w_gran, gint r_offset, gint w_offset,
																 gint min_fifo_size)
{
	MSBuffer *buf;
	gint saved_offset=MAX(r_gran+r_offset,w_offset);
	gint fifo_size;
	gint tmp;
	if (min_fifo_size==0) min_fifo_size=w_gran;
	
	/* we must allocate a fifo with a size multiple of min_fifo_size,
	with a saved_offset */
	if (min_fifo_size>MS_BUFFER_LARGE)
		fifo_size=(min_fifo_size) + saved_offset;
	else fifo_size=(6*min_fifo_size) + saved_offset;
	buf=ms_buffer_new(fifo_size);
	__ms_fifo_parametrize_with_buffer(fifo,buf,r_gran,w_gran,r_offset,w_offset);
	fifo->bpt=min_fifo_size;
	ms_trace("fifo_size=%i",fifo_size);
}

void ms_fifo_destroy( MSFifo *fifo)
{
	ms_buffer_destroy(fifo->buffer);
	g_free(fifo);
}


gchar * ms_fifo_get_read_ptr(MSFifo *fifo, gint bsize)
{
	gchar *rnext;
	gchar *ret_ptr=NULL;
	//ms_trace("ms_fifo_get_read_ptr: entering.");
	g_return_val_if_fail(bsize<=fifo->r_gran,NULL);
	
	if (bsize>fifo->readsize)
	{
		ms_trace("Not enough data: bsize=%i, readsize=%i",bsize,fifo->readsize);
		return NULL;
	}
	
	rnext=fifo->rd_ptr+bsize;
	if (rnext<=fifo->r_end){
		
		ret_ptr=fifo->rd_ptr;
		fifo->rd_ptr=rnext;
	}else{
		int unread=fifo->r_end-fifo->rd_ptr;
		ret_ptr=fifo->begin-unread;
		memcpy(fifo->buffer->buffer,fifo->r_end-fifo->saved_offset,fifo->saved_offset);
		fifo->rd_ptr=ret_ptr + bsize;
		fifo->r_end=fifo->w_end;	/* this is important ! */
		ms_trace("moving read ptr to %x",fifo->rd_ptr);
		
	}
	/* update write size*/
	fifo->writesize+=bsize;
	fifo->readsize-=bsize;
	return ret_ptr;
}


gchar * ms_fifo_get_write_ptr(MSFifo *fifo, gint bsize)
{
	gchar *wnext;
	gchar *ret_ptr=NULL;
	//ms_trace("ms_fifo_get_write_ptr: Entering.");
	g_return_val_if_fail(bsize<=fifo->w_gran,NULL);
	if (bsize>fifo->writesize)
	{
		ms_trace("Not enough space: bsize=%i, writesize=%i",bsize,fifo->writesize);
		return ret_ptr;
	}
	wnext=fifo->wr_ptr+bsize;
	if (wnext<=fifo->w_end){
		ret_ptr=fifo->wr_ptr;
		fifo->wr_ptr=wnext;
	}else{
		ret_ptr=fifo->begin;
		fifo->r_end=fifo->wr_ptr;
		fifo->wr_ptr=fifo->begin+bsize;
		ms_trace("moving write ptr to %x",fifo->wr_ptr);
	}
	fifo->prev_wr_ptr=ret_ptr;
	/* update readsize*/
	fifo->readsize+=bsize;
	fifo->writesize-=bsize;
	//ms_trace("ms_fifo_get_write_ptr: readsize=%i, writesize=%i",fifo->readsize,fifo->writesize);
	return ret_ptr;
}

void ms_fifo_update_write_ptr(MSFifo *fifo, gint written){
	gint reserved=fifo->wr_ptr-fifo->prev_wr_ptr;
	gint unwritten;
	g_return_if_fail(reserved>=0);
	unwritten=reserved-written;
	g_return_if_fail(unwritten>=0);
	/* fix readsize and writesize */
	fifo->readsize-=unwritten;
	fifo->writesize+=unwritten;
	fifo->wr_ptr+=written;
}
