/*
    buffer queues.
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>
    Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org

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
int ivtv_buf_fill_lock_bh(struct ivtv_stream *stream, u32 count, int inc);
int ivtv_buf_fill_lock_irq(struct ivtv_stream *stream, u32 count, int inc);
int ivtv_buf_fill_lock(struct ivtv_stream *stream, u32 count, int inc);
void ivtv_enq_buf_nolock(struct ivtv_buffer_list *queue, struct ivtv_buffer *buf);
int ivtv_enq_buf_lock_irq(struct ivtv_buffer_list *queue,
		 struct ivtv_buffer *buf, struct ivtv_stream *stream);
int ivtv_enq_buf_lock_bh(struct ivtv_buffer_list *queue,
		 struct ivtv_buffer *buf, struct ivtv_stream *stream);
int ivtv_enq_buf_lock(struct ivtv_buffer_list *queue,
		 struct ivtv_buffer *buf, struct ivtv_stream *stream);

struct ivtv_buffer *ivtv_deq_buf_nolock(struct ivtv_buffer_list *queue, int type);
struct ivtv_buffer *ivtv_deq_buf_lock_irq(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream);
struct ivtv_buffer *ivtv_deq_buf_lock_bh(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream);
struct ivtv_buffer *ivtv_deq_buf_lock(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream);

struct ivtv_buffer *ivtv_deq_peek_head_nolock(struct ivtv_buffer_list *queue, int type);
struct ivtv_buffer *ivtv_deq_peek_head_lock_irq(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream);
struct ivtv_buffer *ivtv_deq_peek_head_lock_bh(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream);
struct ivtv_buffer *ivtv_deq_peek_head_lock(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream);

void ivtv_move_buf_lock_irq(struct ivtv_buffer_list *from,
		   struct ivtv_buffer_list *to, struct ivtv_buffer *buffer,
		   struct ivtv_stream *stream);
void ivtv_move_buf_lock_bh(struct ivtv_buffer_list *from,
		   struct ivtv_buffer_list *to, struct ivtv_buffer *buffer,
		   struct ivtv_stream *stream);
void ivtv_move_buf_lock(struct ivtv_buffer_list *from,
		   struct ivtv_buffer_list *to, struct ivtv_buffer *buffer,
		   struct ivtv_stream *stream);

void ivtv_del_buf_nolock(struct ivtv_buffer_list *queue,
			   struct ivtv_buffer *buffer);
void ivtv_del_buf_lock_irq(struct ivtv_buffer_list *queue,
			   struct ivtv_buffer *buffer, 
			   struct ivtv_stream *stream);
void ivtv_del_buf_lock_bh(struct ivtv_buffer_list *queue,
			   struct ivtv_buffer *buffer, 
			   struct ivtv_stream *stream);
void ivtv_del_buf_lock(struct ivtv_buffer_list *queue,
			   struct ivtv_buffer *buffer, 
			   struct ivtv_stream *stream);

void ivtv_flush_queues_lock_irq(struct ivtv *itv, int type);
void ivtv_flush_queues_lock_bh(struct ivtv *itv, int type);
void ivtv_flush_queues_lock(struct ivtv *itv, int type);
void ivtv_flush_queues_nolock(struct ivtv *itv, int type);

/* frees all queues of this stream */
void ivtv_free_queues_nolock(struct ivtv *itv, int type);
void ivtv_free_queues_lock(struct ivtv *itv, int type);
void ivtv_free_queues_lock_irq(struct ivtv *itv, int type);
void ivtv_free_queues_lock_bh(struct ivtv *itv, int type);

/* initialized all queues of this stream */
int ivtv_init_queues_nolock(struct ivtv *itv, int stream_type, int buffers, int bufsize);
int ivtv_init_queues_lock_irq(struct ivtv *itv, int stream_type, int buffers, int bufsize);
int ivtv_init_queues_lock_bh(struct ivtv *itv, int stream_type, int buffers, int bufsize);
int ivtv_init_queues_lock(struct ivtv *itv, int stream_type, int buffers, int bufsize);

/* moves all items in queue 'src' to queue 'dst' */
int ivtv_move_queue(struct ivtv_buffer_list *src,
			   struct ivtv_buffer_list *dst, struct ivtv_stream *stream);
int ivtv_prep_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma,
                                   unsigned long ivtv_dest_addr,
                                   char *userbuf, int size_in_bytes);
int ivtv_unmap_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma);
int ivtv_check_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma, int count);
void ivtv_free_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma);
int ivtv_alloc_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma, int size);


struct ivtv_buffer *ivtv_init_buffer(struct ivtv *itv, 
			struct ivtv_stream *stream);
void ivtv_free_buffer(struct ivtv *itv, 
	struct ivtv_buffer *item, struct ivtv_stream *stream);
