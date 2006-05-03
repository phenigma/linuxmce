/*
    buffer queues.
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>
    Copyright (C) 2004  Chris Kennedy <c@groovy.org>

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

#define IVTV_DMA_UNMAPPED	((u32) -1)

#define BUF_USED	0
#define BUF_ADD		1
#define BUF_SUB		2
#define BUF_EMPTY	3

int ivtv_buf_fill_nolock(struct ivtv_stream *stream, u32 count, int inc);
int ivtv_buf_fill_lock(struct ivtv_stream *stream, u32 count, int inc);
void ivtv_enq_buf_nolock(struct ivtv_buffer_list *queue,
			 struct ivtv_buffer *buf);
int ivtv_enq_buf_lock(struct ivtv_buffer_list *queue, struct ivtv_buffer *buf,
		      struct ivtv_stream *stream);

struct ivtv_buffer *ivtv_deq_buf_nolock(struct ivtv *itv, struct ivtv_buffer_list *queue,
					int type);
struct ivtv_buffer *ivtv_deq_buf_lock(struct ivtv *itv, struct ivtv_buffer_list *queue,
				      struct ivtv_stream *stream);

void ivtv_flush_queues_lock(struct ivtv *itv, int type);
void ivtv_flush_queues_nolock(struct ivtv *itv, int type);

int ivtv_prep_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma,
		       unsigned long ivtv_dest_addr,
		       char *userbuf, int size_in_bytes);
int ivtv_unmap_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma);
void ivtv_free_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma);
int ivtv_alloc_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma, int size);

struct ivtv_buffer *ivtv_init_buffer(struct ivtv *itv,
				     struct ivtv_stream *stream);
void ivtv_free_buffer(struct ivtv *itv,
		      struct ivtv_buffer *item, struct ivtv_stream *stream);

int ivtv_sleep_timeout(int timeout, int intr);
int dec_gather_free_buffers(struct ivtv *itv, int streamtype,
			    struct list_head *free_list, int bytes_needed);
int enc_gather_free_buffers(struct ivtv *itv, int streamtype,
			    struct list_head *free_list, int bytes_needed);
void ivtv_TO_DMA_done(struct ivtv *itv, int stmtype);
int dma_to_device(struct ivtv *itv, struct ivtv_stream *st,
		  dma_addr_t SG_handle, int waitVsync);
int ivtv_stream_alloc(struct ivtv *itv, int streamtype);
void ivtv_stream_free(struct ivtv *itv, int stream);
int unlock_TO_dma(struct ivtv *itv, int stream_type);
const char *ivtv_stream_name(int streamtype);
