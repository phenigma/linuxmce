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

#include "ivtv-driver.h"
#include "ivtv-queue.h"

int ivtv_buf_fill_nolock(struct ivtv_stream *stream, u32 count, int inc) {
	if (inc == BUF_ADD) /* Add */
		stream->buf_fill += count;	
	else if (inc == BUF_SUB) /* Sub */
		stream->buf_fill -= count;	
	else if (inc == BUF_EMPTY) /* Space Left */
		return (stream->buf_total - stream->buf_fill);

	/* Return Count */	
	return stream->buf_fill;
}

int ivtv_buf_fill_lock_irq(struct ivtv_stream *stream, u32 count, int inc) {
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&stream->slock, flags);
	ret = ivtv_buf_fill_nolock(stream, count, inc);
	spin_unlock_irqrestore(&stream->slock, flags);

	return ret;
}

int ivtv_buf_fill_lock_bh(struct ivtv_stream *stream, u32 count, int inc) {
	int ret;

	spin_lock_bh(&stream->slock);
	ret = ivtv_buf_fill_nolock(stream, count, inc);
	spin_unlock_bh(&stream->slock);

	return ret;
}

int ivtv_buf_fill_lock(struct ivtv_stream *stream, u32 count, int inc) {
	int ret;

	down(&stream->mlock);
	ret = ivtv_buf_fill_nolock(stream, count, inc);
	up(&stream->mlock);

	return ret;
}

void ivtv_enq_buf_nolock(struct ivtv_buffer_list *queue, struct ivtv_buffer *buf)
{
	WARN_ON(!list_empty(&buf->list));
	list_add_tail(&buf->list, &queue->list);
	atomic_inc(&queue->elements);
}


/* Adds buffers to the tail, effectively making a queue */
int ivtv_enq_buf_lock_irq(struct ivtv_buffer_list *queue,
		 struct ivtv_buffer *buf, struct ivtv_stream *stream)
{
	unsigned long flags;

	spin_lock_irqsave(&stream->slock, flags);
	ivtv_enq_buf_nolock(queue, buf);
	spin_unlock_irqrestore(&stream->slock, flags);

	return 0;
}

/* Adds buffers to the tail, effectively making a queue */
int ivtv_enq_buf_lock_bh(struct ivtv_buffer_list *queue,
		 struct ivtv_buffer *buf, struct ivtv_stream *stream)
{
	spin_lock_bh(&stream->slock);
	ivtv_enq_buf_nolock(queue, buf);
	spin_unlock_bh(&stream->slock);

	return 0;
}

/* Adds buffers to the tail, effectively making a queue */
int ivtv_enq_buf_lock(struct ivtv_buffer_list *queue,
		 struct ivtv_buffer *buf, struct ivtv_stream *stream)
{
	down(&stream->mlock);
	ivtv_enq_buf_nolock(queue, buf);
	up(&stream->mlock);

	return 0;
}

void ivtv_del_buf_nolock(struct ivtv_buffer_list *queue,
			   struct ivtv_buffer *buffer)
{
	WARN_ON(list_empty(&buffer->list));
	list_del_init(&buffer->list);
	atomic_dec(&queue->elements);
}

void ivtv_del_buf_lock_irq(struct ivtv_buffer_list *queue,
                           struct ivtv_buffer *buffer, struct ivtv_stream *stream)
{
	unsigned long flags;	

	spin_lock_irqsave(&stream->slock, flags);
	ivtv_del_buf_nolock(queue, buffer);
	spin_unlock_irqrestore(&stream->slock, flags);
}

void ivtv_del_buf_lock_bh(struct ivtv_buffer_list *queue,
                           struct ivtv_buffer *buffer, struct ivtv_stream *stream)
{
	spin_lock_bh(&stream->slock);
	ivtv_del_buf_nolock(queue, buffer);
	spin_unlock_bh(&stream->slock);
}

void ivtv_del_buf_lock(struct ivtv_buffer_list *queue,
                           struct ivtv_buffer *buffer, struct ivtv_stream *stream)
{
	down(&stream->mlock);
	ivtv_del_buf_nolock(queue, buffer);
	up(&stream->mlock);
}

void ivtv_move_buf_lock_irq(struct ivtv_buffer_list *from,
                   struct ivtv_buffer_list *to, struct ivtv_buffer *buffer,
                   struct ivtv_stream *stream)
{
	unsigned long flags;

        WARN_ON(list_empty(&buffer->list));

        spin_lock_irqsave(&stream->slock, flags);
        list_move_tail(&buffer->list, &to->list);
        atomic_dec(&from->elements);
        atomic_inc(&to->elements);
        spin_unlock_irqrestore(&stream->slock, flags);
}

void ivtv_move_buf_lock_bh(struct ivtv_buffer_list *from,
		   struct ivtv_buffer_list *to, struct ivtv_buffer *buffer,
		   struct ivtv_stream *stream)
{
	WARN_ON(list_empty(&buffer->list));

	spin_lock_bh(&stream->slock);
	list_move_tail(&buffer->list, &to->list);
	atomic_dec(&from->elements);
	atomic_inc(&to->elements);
	spin_unlock_bh(&stream->slock);
}

void ivtv_move_buf_lock(struct ivtv_buffer_list *from,
		   struct ivtv_buffer_list *to, struct ivtv_buffer *buffer,
		   struct ivtv_stream *stream)
{
	WARN_ON(list_empty(&buffer->list));

	down(&stream->mlock);
	list_move_tail(&buffer->list, &to->list);
	atomic_dec(&from->elements);
	atomic_inc(&to->elements);
	up(&stream->mlock);
}

/* returns first item in queue, doesn't dequeue */
struct ivtv_buffer *ivtv_deq_peek_head_nolock(struct ivtv_buffer_list *queue, int type)
{
	/* make sure list has something to DeQ */
	if (!list_empty(&queue->list))
		return list_entry(queue->list.next, struct ivtv_buffer, list);
		
	IVTV_DEBUG(IVTV_DEBUG_DMA,
		"deqpeek: %d DeQ from empty list\n", type);
	return NULL;
}

struct ivtv_buffer *ivtv_deq_peek_head_lock(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream)
{
	struct ivtv_buffer *buffer;

	down(&stream->mlock);
	buffer = ivtv_deq_peek_head_nolock(queue, stream->type);
	up(&stream->mlock);

	return buffer;
}

struct ivtv_buffer *ivtv_deq_peek_head_lock_bh(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream)
{
	struct ivtv_buffer *buffer;

	spin_lock_bh(&stream->slock);
	buffer = ivtv_deq_peek_head_nolock(queue, stream->type);
	spin_unlock_bh(&stream->slock);

	return buffer;
}

struct ivtv_buffer *ivtv_deq_peek_head_lock_irq(struct ivtv_buffer_list *queue,
                struct ivtv_stream *stream)
{
        struct ivtv_buffer *buffer;
	unsigned long flags;

        spin_lock_irqsave(&stream->slock, flags);
        buffer = ivtv_deq_peek_head_nolock(queue, stream->type);
        spin_unlock_irqrestore(&stream->slock, flags);

        return buffer;
}

/* removes buffer from the head */
struct ivtv_buffer *ivtv_deq_buf_nolock(struct ivtv_buffer_list *queue, int type)
{
	struct ivtv_buffer *buf;

	/* make sure list has something to DeQ */
	if (!list_empty(&queue->list)) {
		buf = list_entry(queue->list.next, struct ivtv_buffer, list);
		list_del_init(queue->list.next);
		atomic_dec(&queue->elements);
		return buf;
	}

	IVTV_DEBUG(IVTV_DEBUG_DMA, 
		"deqbuf: %d DeQ from empty list!\n", type);
	return NULL;
}

struct ivtv_buffer *ivtv_deq_buf_lock(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream)
{
	struct ivtv_buffer *buf;

	down(&stream->mlock);
	buf = ivtv_deq_buf_nolock(queue, stream->type);
	up(&stream->mlock);

	return buf;
}

struct ivtv_buffer *ivtv_deq_buf_lock_irq(struct ivtv_buffer_list *queue,
                struct ivtv_stream *stream)
{
        struct ivtv_buffer *buf;
	unsigned long flags;	

        spin_lock_irqsave(&stream->slock, flags);
        buf = ivtv_deq_buf_nolock(queue, stream->type);
        spin_unlock_irqrestore(&stream->slock, flags);

        return buf;
}

struct ivtv_buffer *ivtv_deq_buf_lock_bh(struct ivtv_buffer_list *queue,
	       	struct ivtv_stream *stream)
{
	struct ivtv_buffer *buf;

	spin_lock_bh(&stream->slock);
	buf = ivtv_deq_buf_nolock(queue, stream->type);
	spin_unlock_bh(&stream->slock);

	return buf;
}

void ivtv_free_buffer(struct ivtv *itv, struct ivtv_buffer *item, struct ivtv_stream *st)
{
	if (item->buffer.m.userptr != 0) {
        	/* UnMap Buffer */
		if (st->dma != PCI_DMA_NONE) {
        		pci_unmap_single(itv->dev, item->dma_handle,
                		item->buffer.length, st->dma);
		}

		kfree((void *)item->buffer.m.userptr);
		item->buffer.m.userptr = 0;

        	atomic_dec(&st->allocated_buffers);
        	st->buf_total -= item->buffer.length;
	}

	if (item != NULL) {
		kfree(item);
		item = NULL;
	}
}

void ivtv_free_queues_nolock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];
        struct ivtv_buffer *item;

	while ((item = ivtv_deq_buf_nolock(&st->free_q, type)))
		ivtv_free_buffer(itv, item, st);
	while ((item = ivtv_deq_buf_nolock(&st->full_q, type)))
		ivtv_free_buffer(itv, item, st);
	while ((item = ivtv_deq_buf_nolock(&st->io_q, type)))
		ivtv_free_buffer(itv, item, st);
	while ((item = ivtv_deq_buf_nolock(&st->dma_q, type)))
		ivtv_free_buffer(itv, item, st);
}

void ivtv_free_queues_lock_irq(struct ivtv *itv, int type)
{
        struct ivtv_stream *st = &itv->streams[type];
	unsigned long flags;

        spin_lock_irqsave(&st->slock, flags);
        ivtv_free_queues_nolock(itv, type);
        spin_unlock_irqrestore(&st->slock, flags);
}

void ivtv_free_queues_lock_bh(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];

	spin_lock_bh(&st->slock);
	ivtv_free_queues_nolock(itv, type);
	spin_unlock_bh(&st->slock);
}

void ivtv_free_queues_lock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];

	down(&st->mlock);
	ivtv_free_queues_nolock(itv, type);
	up(&st->mlock);
}

struct ivtv_buffer *ivtv_init_buffer(struct ivtv *itv, struct ivtv_stream *st)
{
	struct ivtv_buffer *buf;

	buf = kmalloc(sizeof(struct ivtv_buffer), GFP_KERNEL);
	if (buf == NULL) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "No memory on ivtv_buffer alloc!\n");
		return NULL;
	}

	buf->buffer.m.userptr = (unsigned long)kmalloc(st->bufsize, GFP_KERNEL);

	/* Try till down to a page size chunk */
	while (buf->buffer.m.userptr == 0 && st->bufsize >= PAGE_SIZE) {
		st->bufsize = (st->bufsize/2);

		buf->buffer.m.userptr = (unsigned long)kmalloc(st->bufsize, GFP_KERNEL);
	}

	if (buf->buffer.m.userptr == 0) {
		kfree(buf);
		IVTV_DEBUG(IVTV_DEBUG_ERR, "No memory on buffer alloc!\n");
		return NULL;
	}

	buf->b_flags = 0;
	buf->ts = 0;
	buf->buffer.length = st->bufsize;
	buf->buffer.bytesused = 0;
	buf->readpos = 0;

        /* setup buffer */
        buf->buffer.index = 0;

	/* Type */
        if (st->type == IVTV_ENC_STREAM_TYPE_MPG) {
        	buf->buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        } else if (st->type == IVTV_ENC_STREAM_TYPE_YUV) {
        	buf->buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        } else if (st->type == IVTV_ENC_STREAM_TYPE_VBI) {
        	buf->buffer.type = V4L2_BUF_TYPE_VBI_CAPTURE;
        } else if (st->type == IVTV_ENC_STREAM_TYPE_PCM) {
        	buf->buffer.type = V4L2_BUF_TYPE_PRIVATE;
        } else if (st->type == IVTV_ENC_STREAM_TYPE_RAD) {
        	buf->buffer.type = V4L2_BUF_TYPE_PRIVATE;
        } else if (st->type == IVTV_DEC_STREAM_TYPE_MPG) {
        	buf->buffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        } else if (st->type == IVTV_DEC_STREAM_TYPE_VBI) {
        	buf->buffer.type = V4L2_BUF_TYPE_VBI_CAPTURE;
        } else if (st->type == IVTV_DEC_STREAM_TYPE_VOUT) {
        	buf->buffer.type = V4L2_BUF_TYPE_VBI_OUTPUT;
        } else if (st->type == IVTV_DEC_STREAM_TYPE_YUV) {
        	buf->buffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        } else {
        	buf->buffer.type = V4L2_BUF_TYPE_PRIVATE;
        }

        buf->buffer.field = V4L2_FIELD_INTERLACED;
        buf->buffer.memory = V4L2_MEMORY_MMAP;

        atomic_inc(&st->allocated_buffers);
        st->buf_total += buf->buffer.length;

	INIT_LIST_HEAD(&buf->list);

	/* Map Buffer */
	if (st->dma != PCI_DMA_NONE) {
        	buf->dma_handle = pci_map_single(itv->dev, 
			(void *)buf->buffer.m.userptr, 
			buf->buffer.length, st->dma);
	}

	return buf;
}

static int ivtv_init_queue_nolock(struct ivtv *itv, struct ivtv_buffer_list *queue, 
		int buffers, int bufsize, struct ivtv_stream *st)
{
	struct ivtv_buffer *item;
	int x;
	int bytes_allocated = 0;

	for (x = 0; bytes_allocated < (buffers * bufsize); x++) {
		/* allocate buffer */
		item = ivtv_init_buffer(itv, st);
		if (item == NULL) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "Buffer alloc failed!\n");
			return -ENOMEM;
		}

		bytes_allocated += item->buffer.length;

		/* setup buffer */
		item->buffer.index = x;

		/* enqueue buffer */
		ivtv_enq_buf_nolock(queue, item);
	}

	return x;
}

int ivtv_init_queues_nolock(struct ivtv *itv, int streamtype, int buffers, int bufsize)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	int x;

	/* The full and dma queues are initially empty, so we only have to
	   allocate the free queue. */
	if ((x = ivtv_init_queue_nolock(itv, &st->free_q, 
			(buffers/bufsize), bufsize, st)) < 0) 
	{
		ivtv_free_queues_nolock(itv, streamtype);
		return -ENOMEM;
	}

	return 0;
}

int ivtv_init_queues_lock_bh(struct ivtv *itv, int streamtype, int buffers, int bufsize)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	int ret;

	spin_lock_bh(&st->slock);
	ret = ivtv_init_queues_nolock(itv, streamtype, buffers, bufsize); 
	spin_unlock_bh(&st->slock);
	return ret;
}

int ivtv_init_queues_lock(struct ivtv *itv, int streamtype, int buffers, int bufsize)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	int ret;

	down(&st->mlock);
	ret = ivtv_init_queues_nolock(itv, streamtype, buffers, bufsize); 
	up(&st->mlock);
	return ret;
}

int ivtv_move_queue(struct ivtv_buffer_list *src,
		    struct ivtv_buffer_list *dst, struct ivtv_stream *stream)
{
	struct ivtv_buffer *buf;

	down(&stream->mlock);
  	while ((buf = ivtv_deq_buf_nolock(src, stream->type)))
		ivtv_enq_buf_nolock(dst, buf);
	up(&stream->mlock);

	return 0;
}

void ivtv_flush_queues_nolock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];
	struct ivtv_buffer *buf;

	IVTV_DEBUG(IVTV_DEBUG_DMA, 
		"Flush Queue Stream %d, 0x%08x bytes full, 0x%08x total\n",
		st->type, st->buf_fill, st->buf_total);

        while ((buf = ivtv_deq_buf_nolock(&st->io_q, st->type))) {
		/* Only read will count io_q as full buffers */
		if (st->dma == PCI_DMA_FROMDEVICE)
			st->buf_fill -= buf->buffer.bytesused;

                buf->buffer.bytesused = 0;
                buf->readpos = 0;
                buf->b_flags = 0;
                buf->ts = 0;

		if (st->buf_total <= st->buf_min)
                	ivtv_enq_buf_nolock(&st->free_q, buf);
		else {
			ivtv_free_buffer(itv, buf, st);
			buf = NULL;
		}
	}

	/* Either free or requeue full Queue */
        while ((buf = ivtv_deq_buf_nolock(&st->full_q, st->type))) {
		st->buf_fill -= buf->buffer.bytesused;

                buf->buffer.bytesused = 0;
                buf->readpos = 0;
                buf->b_flags = 0;
                buf->ts = 0;

		if (st->buf_total <= st->buf_min)
                	ivtv_enq_buf_nolock(&st->free_q, buf);
		else {
			ivtv_free_buffer(itv, buf, st);
			buf = NULL;
		}
	}

	/* Either free or requeue DMA Queue */
        while ((buf = ivtv_deq_buf_nolock(&st->dma_q, st->type))) {
		/* Only write will count dma_q as full buffers */
		if (st->dma == PCI_DMA_TODEVICE)
			st->buf_fill -= buf->readpos;

                buf->buffer.bytesused = 0;
                buf->readpos = 0;
                buf->b_flags = 0;
                buf->ts = 0;

		if (st->buf_total <= st->buf_min)
                	ivtv_enq_buf_nolock(&st->free_q, buf);
		else {
			ivtv_free_buffer(itv, buf, st);
			buf = NULL;
		}
        }

	IVTV_DEBUG(IVTV_DEBUG_DMA, 
		"Flush Queue Stream %d, 0x%08x bytes full, 0x%08x total\n",
		st->type, st->buf_fill, st->buf_total);

	/* Zero out buffer fill count */
	if (st->buf_fill) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"Error flushing queues, still 0x%08x left\n",
			st->buf_fill);
		st->buf_fill = 0;
	}		
}

void ivtv_flush_queues_lock_bh(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];

	spin_lock_bh(&st->slock);
	ivtv_flush_queues_nolock(itv, type);
	spin_unlock_bh(&st->slock);
}

void ivtv_flush_queues_lock_irq(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];
	unsigned long flags;

	spin_lock_irqsave(&st->slock, flags);
	ivtv_flush_queues_nolock(itv, type);
	spin_unlock_irqrestore(&st->slock, flags);
}

void ivtv_flush_queues_lock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];

	down(&st->mlock);
	ivtv_flush_queues_nolock(itv, type);
	up(&st->mlock);
}

/* User DMA Buffers */
int ivtv_alloc_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma, int size)
{
    IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtvfb_alloc_user_dma_to_device\n");

    /* Allocate SG Array */
    dma->SGarray = kmalloc(sizeof(struct ivtv_SG_element)* IVTV_DMA_SG_OSD_ENT,
                          GFP_KERNEL);

    if (!dma->SGarray) {
        printk(KERN_ERR
               "ivtvfb: cannot allocate scatter/gather list for %d pages\n",
               (int)IVTV_DMA_SG_OSD_ENT);
        return -ENOMEM;
    }

    /* Allocate SG List */
    dma->SGlist = kmalloc(sizeof(struct scatterlist)*IVTV_DMA_SG_OSD_ENT,
                          GFP_KERNEL);

    if (!dma->SGlist) {
        printk(KERN_ERR
               "ivtvfb: cannot allocate scatter/gather list for %d pages\n",
               (int)IVTV_DMA_SG_OSD_ENT);
        kfree(dma->SGarray);
        return -ENOMEM;
    }

    /* Allocate DMA Page Array Buffer */
    dma->map = kmalloc(sizeof(struct page *) * IVTV_DMA_SG_OSD_ENT, GFP_KERNEL);
    if (!dma->map) {
        IVTV_DEBUG(IVTV_DEBUG_ERR, "can't alloc dma page array\n");
        kfree(dma->SGlist);
        kfree(dma->SGarray);
        return -ENOMEM;
    }

    /* Clear DMA Page Array Size */
    dma->page_count = 0;

    /* Map DMA Page Array Buffer */
    dma->SG_handle =
        pci_map_single(itv->dev, (void *) dma->SGarray,
                       (sizeof(struct ivtv_SG_element) *
                         IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);

    return 0;
}

int ivtv_prep_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma,
                                   unsigned long ivtv_dest_addr,
                                   char *userbuf, int size_in_bytes)
{
    unsigned long first, last; 	
    int i, offset;
    unsigned long uaddr;
    int err;	
    struct scatterlist *sg;

    IVTV_DEBUG(IVTV_DEBUG_DMA, "ivtvfb_prep_user_dma, dst: 0x%08x\n",
                    (unsigned int) ivtv_dest_addr);

    /* Not Allocated */ 
    if (dma->map == NULL || dma->SGlist == NULL || dma->SGarray == NULL) {
	return -ENOMEM;	
    }	

    /* Still in USE */
    if (dma->SG_length || dma->page_count) {
	IVTV_DEBUG(IVTV_DEBUG_ERR, 
		"prep_user_dma: SG_length %d page_count %d still full?\n",
		dma->SG_length, dma->page_count);
	return -EBUSY;
    }	

    /* User Input */
    uaddr = ((unsigned long) userbuf & PAGE_MASK);
    first = (uaddr & PAGE_MASK) >> PAGE_SHIFT;
    last = (((unsigned long)userbuf+size_in_bytes-1) & PAGE_MASK) >> PAGE_SHIFT;

    /* Align to PAGE_SIZE */
    offset = dma->offset = (unsigned long)userbuf & ~PAGE_MASK;
    dma->page_count = last-first+1;

    if (dma->page_count <= 0) {
      IVTV_DEBUG(IVTV_DEBUG_ERR, "prep_user_dma: Error %d page_count from %d bytes %d offset\n",
	dma->page_count, size_in_bytes, offset);
        return -EINVAL;
    }	 

    /* Get user pages for DMA Xfer */
    down_read(&current->mm->mmap_sem);
    err = get_user_pages(current, current->mm, 
		uaddr , dma->page_count, 0, 1, dma->map, NULL);
    up_read(&current->mm->mmap_sem);

    if (dma->page_count != err) {
        IVTV_DEBUG(IVTV_DEBUG_ERR, 
		"failed to map user pages, returned %d instead of %d\n",
		dma->page_count, err);
        return -EINVAL;
    }

    /* Fill SG Array with new values */
    for (i = 0; i < dma->page_count; i++) {
        dma->SGlist[i].length = PAGE_SIZE - offset;
        dma->SGlist[i].offset = offset;
        dma->SGlist[i].page  = dma->map[i];
#ifndef LINUX26
	dma->SGlist[i].address = 0;
#endif
        offset = 0;
    }

    /* Map SG List */	
    dma->SG_length =
        pci_map_sg(itv->dev, dma->SGlist, dma->page_count, PCI_DMA_TODEVICE);

    /* Fill SG Array with new values */
    dma->sg_size = 0;
    for (i = 0, sg = dma->SGlist; i < dma->SG_length; i++, sg++) {
        dma->SGarray[i].size = sg_dma_len(sg);
	dma->SGarray[i].src = sg_dma_address(sg);
        dma->SGarray[i].dst = ivtv_dest_addr + dma->sg_size;
	dma->sg_size += dma->SGarray[i].size;
    }

    /* Tag SG Array with Interrupt Bit */
    dma->SGarray[dma->SG_length - 1].size |= 0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
    pci_dma_sync_single_for_device((struct pci_dev*)itv->dev, dma->SG_handle,
        (sizeof(struct ivtv_SG_element) *
	IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#else
    pci_dma_sync_single(itv->dev, dma->SG_handle,
        (sizeof(struct ivtv_SG_element) *
	IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#endif

    return dma->page_count;
}

int ivtv_unmap_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma)
{
    int i;
    IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtvfb_unmap_user_dma\n");

    if (dma->map == NULL || dma->SGarray == NULL || dma->SGlist == NULL)
	return 2;	

    /* Nothing to free */	
    if (dma->page_count == 0)
	return 0;	 	

    /* Unmap Scatterlist */
    if (dma->SG_length) {
    	pci_unmap_sg(itv->dev, dma->SGlist, dma->page_count, PCI_DMA_TODEVICE); 
	dma->SG_length = 0;
    }	

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
    /* sync DMA */
    pci_dma_sync_single_for_cpu((struct pci_dev*)itv->dev, dma->SG_handle,
        (sizeof(struct ivtv_SG_element) *
        IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#else
    /* sync DMA */
    pci_dma_sync_single(itv->dev, dma->SG_handle,
        (sizeof(struct ivtv_SG_element) *
        IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#endif

    /* Release User Pages */
    for (i = 0; i < dma->page_count; i++) {
	flush_dcache_page(dma->map[i]);
	page_cache_release(dma->map[i]);
    }	
    dma->page_count = 0;

    return 0;
}

void ivtv_free_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma)
{
    IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtvfb_free_user_dma\n");

    if (dma->map == NULL || dma->SGarray == NULL || dma->SGlist == NULL)
	return;	

    /* Unmap SG Array */
    if (dma->SG_handle != IVTV_DMA_UNMAPPED) {
        pci_unmap_single(itv->dev,  dma->SG_handle,
                (sizeof(struct ivtv_SG_element)) * IVTV_DMA_SG_OSD_ENT,
                PCI_DMA_TODEVICE);

        dma->SG_handle = IVTV_DMA_UNMAPPED;
    }

    /* Free SG Array */
    kfree(dma->SGarray);
    dma->SGarray = NULL;

    /* Unmap Scatterlist */
    if (dma->SG_length) {
    	pci_unmap_sg(itv->dev, dma->SGlist, dma->page_count, PCI_DMA_TODEVICE); 
	dma->SG_length = 0;
	dma->page_count = 0;
    }	

    /* Free DMA Buffer of Pages */
    kfree(dma->SGlist);
    dma->SGlist = NULL;

    /* Free DMA Buffer of Pages */
    kfree(dma->map);
    dma->map = NULL;

    return;
}

