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

#include "ivtv-driver.h"
#include "ivtv-streams.h"
#include "ivtv-queue.h"
#include "ivtv-mailbox.h"

typedef unsigned long uintptr_t;

/* Generic utility functions */
int ivtv_sleep_timeout(int timeout, int intr)
{
	int sleep = timeout;
	int ret = 0;

	do {
		if (intr)
			set_current_state(TASK_INTERRUPTIBLE);
		else
			set_current_state(TASK_UNINTERRUPTIBLE);

		sleep = schedule_timeout(sleep);

		if (intr)
			if ((ret = signal_pending(current)))
				break;
	} while (sleep);
	return ret;
}

int ivtv_buf_fill_nolock(struct ivtv_stream *stream, u32 count, int inc)
{
	if (inc == BUF_ADD)	/* Add */
		stream->buf_fill += count;
	else if (inc == BUF_SUB)	/* Sub */
		stream->buf_fill -= count;
	else if (inc == BUF_EMPTY)	/* Space Left */
		return (stream->buf_total - stream->buf_fill);

	/* Return Count */
	return stream->buf_fill;
}

int ivtv_buf_fill_lock(struct ivtv_stream *stream, u32 count, int inc)
{
	int ret;

	down(&stream->mlock);
	ret = ivtv_buf_fill_nolock(stream, count, inc);
	up(&stream->mlock);

	return ret;
}

void ivtv_enq_buf_nolock(struct ivtv_buffer_list *queue,
			 struct ivtv_buffer *buf)
{
	WARN_ON(!list_empty(&buf->list));
	list_add_tail(&buf->list, &queue->list);
	atomic_inc(&queue->elements);
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

/* removes buffer from the head */
struct ivtv_buffer *ivtv_deq_buf_nolock(struct ivtv *itv, struct ivtv_buffer_list *queue,
					int type)
{
	struct ivtv_buffer *buf;

	/* make sure list has something to DeQ */
	if (!list_empty(&queue->list)) {
		buf = list_entry(queue->list.next, struct ivtv_buffer, list);
		list_del_init(queue->list.next);
		atomic_dec(&queue->elements);
		return buf;
	}

	IVTV_DEBUG_DMA("deqbuf: %d DeQ from empty list!\n", type);
	return NULL;
}

struct ivtv_buffer *ivtv_deq_buf_lock(struct ivtv *itv, struct ivtv_buffer_list *queue,
				      struct ivtv_stream *stream)
{
	struct ivtv_buffer *buf;

	down(&stream->mlock);
	buf = ivtv_deq_buf_nolock(itv, queue, stream->type);
	up(&stream->mlock);

	return buf;
}

void ivtv_free_buffer(struct ivtv *itv, struct ivtv_buffer *item,
		      struct ivtv_stream *st)
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

static void ivtv_free_queues_nolock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];
	struct ivtv_buffer *item;

	while ((item = ivtv_deq_buf_nolock(itv, &st->free_q, type)))
		ivtv_free_buffer(itv, item, st);
	while ((item = ivtv_deq_buf_nolock(itv, &st->full_q, type)))
		ivtv_free_buffer(itv, item, st);
	while ((item = ivtv_deq_buf_nolock(itv, &st->io_q, type)))
		ivtv_free_buffer(itv, item, st);
	while ((item = ivtv_deq_buf_nolock(itv, &st->dma_q, type)))
		ivtv_free_buffer(itv, item, st);
}

static void ivtv_free_queues_lock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];

	down(&st->mlock);
	ivtv_free_queues_nolock(itv, type);
	up(&st->mlock);
}

void * ivtv_page_buf(struct ivtv_stream *st, struct ivtv_buffer *buf) {
	const unsigned int tail_size = st->bufsize % PAGE_SIZE;
	const unsigned int tails_per_buf = (tail_size > 0) ? 1 : 0;
	const unsigned int pointers_per_buf = 
		st->bufsize / PAGE_SIZE + tails_per_buf;	
	const unsigned int total_pointers = pointers_per_buf * 1;
	const unsigned int full_pages = 
		st->bufsize / PAGE_SIZE * 1;
	const unsigned int tails_per_page = (tail_size > 0) ?
		PAGE_SIZE / tail_size : 0;
	const unsigned int tail_pages = (tail_size > 0) ?
		1 / tails_per_page +
		((1 % tails_per_page) ? 1 : 0) : 0;
	const unsigned int total_pages = full_pages + tail_pages;
	unsigned int i, bufnum;

        /* Allocate an array of pointers to pages */
        if ((buf->page = (unsigned long *)kmalloc (
                total_pages * sizeof (unsigned long),
                GFP_KERNEL)) == NULL) {
                goto NO_PAGE_PTR;
        }

        /* Allocate an array of pointers to locations in the page array */
        if ((buf->vpage = (unsigned char **)kmalloc (
                total_pointers * sizeof (unsigned char *),
                GFP_KERNEL)) == NULL) {
                goto NO_VPAGE_PTR;
        }

        /* Allocate pages */
        for (i = 0; i < total_pages; i++) {
                if ((buf->page[i] = get_zeroed_page (GFP_KERNEL)) == 0) {
                        int j;

                        for (j = 0; j < i; j++) {
                                free_page (buf->page[j]);
                        }
                        goto NO_BUF;
                }
        }

        /* Fill iface->vpage[] with pointers to pages and parts of pages
         * in iface->page[] */
        for (i = 0; i < total_pointers; i++) {
                bufnum = i / pointers_per_buf;
                buf->vpage[i] = (unsigned char *)(((tail_size > 0) &&
                        ((i % pointers_per_buf) == (pointers_per_buf - 1))) ?
                        buf->page[full_pages + bufnum / tails_per_page] +
                        (bufnum % tails_per_page) * tail_size :
                        buf->page[i - bufnum * tails_per_buf]);
        }

	return buf->vpage;
NO_BUF:
        kfree (buf->vpage);
NO_VPAGE_PTR:
        kfree (buf->page);
NO_PAGE_PTR:
        return 0;
}

struct ivtv_buffer *ivtv_init_buffer(struct ivtv *itv, struct ivtv_stream *st)
{
	struct ivtv_buffer *buf;

	buf = kmalloc(sizeof(struct ivtv_buffer), GFP_KERNEL);
	if (buf == NULL) {
		IVTV_DEBUG_WARN("No memory on ivtv_buffer alloc!\n");
		return NULL;
	}
	buf->buffer.m.userptr = (unsigned long)kmalloc(st->bufsize, GFP_KERNEL);

	if (buf->buffer.m.userptr == 0) {
		kfree(buf);
		IVTV_DEBUG_WARN("No memory on buffer alloc!\n");
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

static int ivtv_init_queue_nolock(struct ivtv *itv,
				  struct ivtv_buffer_list *queue, int buffers,
				  int bufsize, struct ivtv_stream *st)
{
	struct ivtv_buffer *item;
	int x;
	int bytes_allocated = 0;

	for (x = 0; bytes_allocated < (buffers * bufsize); x++) {
		/* allocate buffer */
		item = ivtv_init_buffer(itv, st);
		if (item == NULL) {
			IVTV_DEBUG_WARN("Buffer alloc failed!\n");
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

static int ivtv_init_queues_nolock(struct ivtv *itv, int streamtype, int buffers,
			    int bufsize)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	int x;

	/* The full and dma queues are initially empty, so we only have to
	   allocate the free queue. */
	if ((x = ivtv_init_queue_nolock(itv, &st->free_q,
					(buffers / bufsize), bufsize, st)) < 0)
	{
		ivtv_free_queues_nolock(itv, streamtype);
		return -ENOMEM;
	}

	return 0;
}

void ivtv_flush_queues_nolock(struct ivtv *itv, int type)
{
	struct ivtv_stream *st = &itv->streams[type];
	struct ivtv_buffer *buf;

	IVTV_DEBUG_DMA(
		   "Flush Queue Stream %d, 0x%08x bytes full, 0x%08x total\n",
		   st->type, st->buf_fill, st->buf_total);

	/* No Queues to flush */
	if (!atomic_read(&st->allocated_buffers))
		return;

	while ((buf = ivtv_deq_buf_nolock(itv, &st->io_q, st->type))) {
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
	while ((buf = ivtv_deq_buf_nolock(itv, &st->full_q, st->type))) {
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
	while ((buf = ivtv_deq_buf_nolock(itv, &st->dma_q, st->type))) {
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

	IVTV_DEBUG_DMA(
		   "Flush Queue Stream %d, 0x%08x bytes full, 0x%08x total\n",
		   st->type, st->buf_fill, st->buf_total);

	/* Zero out buffer fill count */
	if (st->buf_fill) {
		IVTV_DEBUG_WARN(
			   "Error flushing queues, still 0x%08x left\n",
			   st->buf_fill);
		st->buf_fill = 0;
	}
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
	IVTV_DEBUG_INFO("ivtv_alloc_user_dma\n");

	/* Allocate SG Array */
	dma->SGarray =
	    kmalloc(sizeof(struct ivtv_SG_element) * IVTV_DMA_SG_OSD_ENT,
		    GFP_KERNEL);

	if (!dma->SGarray) {
		IVTV_ERR("cannot allocate scatter/gather list for %d pages\n",
		       (int)IVTV_DMA_SG_OSD_ENT);
		return -ENOMEM;
	}

	/* Allocate SG List */
	dma->SGlist = kmalloc(sizeof(struct scatterlist) * IVTV_DMA_SG_OSD_ENT,
			      GFP_KERNEL);

	if (!dma->SGlist) {
		IVTV_ERR("cannot allocate scatter/gather list for %d pages\n",
		       (int)IVTV_DMA_SG_OSD_ENT);
		kfree(dma->SGarray);
		return -ENOMEM;
	}

	/* Allocate DMA Page Array Buffer */
	dma->map =
	    kmalloc(sizeof(struct page *) * IVTV_DMA_SG_OSD_ENT, GFP_KERNEL);
	if (!dma->map) {
		IVTV_DEBUG_WARN("can't alloc dma page array\n");
		kfree(dma->SGlist);
		kfree(dma->SGarray);
		return -ENOMEM;
	}

	/* Clear DMA Page Array Size */
	dma->page_count = 0;

	/* Map DMA Page Array Buffer */
	dma->SG_handle =
	    pci_map_single(itv->dev, (void *)dma->SGarray,
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

	IVTV_DEBUG_DMA("ivtv_prep_user_dma, dst: 0x%08x\n",
		   (unsigned int)ivtv_dest_addr);

	/* Not Allocated */
	if (dma->map == NULL || dma->SGlist == NULL || dma->SGarray == NULL) {
		IVTV_DEBUG_WARN("User DMA not allocated!!!\n");
		return -ENOMEM;
	}

	/* Still in USE */
	if (dma->SG_length || dma->page_count) {
		IVTV_DEBUG_WARN(
			   "prep_user_dma: SG_length %d page_count %d still full?\n",
			   dma->SG_length, dma->page_count);
		return -EBUSY;
	}

	/* User Input */
	uaddr = ((unsigned long)userbuf & PAGE_MASK);
	first = (uaddr & PAGE_MASK) >> PAGE_SHIFT;
	last =
	    (((unsigned long)userbuf + size_in_bytes -
	      1) & PAGE_MASK) >> PAGE_SHIFT;

	/* Align to PAGE_SIZE */
	offset = dma->offset = (unsigned long)userbuf & ~PAGE_MASK;
	dma->page_count = last - first + 1;

	if (dma->page_count <= 0) {
		IVTV_DEBUG_WARN(
			   "prep_user_dma: Error %d page_count from %d bytes %d offset\n",
			   dma->page_count, size_in_bytes, offset);
		return -EINVAL;
	}

	/* Get user pages for DMA Xfer */
	down_read(&current->mm->mmap_sem);
	err = get_user_pages(current, current->mm,
			     uaddr, dma->page_count, 0, 1, dma->map, NULL);
	up_read(&current->mm->mmap_sem);

	if (dma->page_count != err) {
		IVTV_DEBUG_WARN(
			   "failed to map user pages, returned %d instead of %d\n",
			   dma->page_count, err);
		return -EINVAL;
	}

	/* Fill SG Array with new values */
	for (i = 0; i < dma->page_count; i++) {
		dma->SGlist[i].length = PAGE_SIZE - offset;
		dma->SGlist[i].offset = offset;
		dma->SGlist[i].page = dma->map[i];
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
		dma->SGlist[i].address = 0;
#endif
		offset = 0;
	}

	/* Map SG List */
	dma->SG_length =
	    pci_map_sg(itv->dev, dma->SGlist, dma->page_count,
		       PCI_DMA_TODEVICE);

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
	pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
				       dma->SG_handle,
				       (sizeof(struct ivtv_SG_element) *
					IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#else
	pci_dma_sync_single(itv->dev, dma->SG_handle,
			    (sizeof(struct ivtv_SG_element) *
			     IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

	return dma->page_count;
}

int ivtv_unmap_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma)
{
	int i;
	IVTV_DEBUG_INFO("ivtv_unmap_user_dma\n");

	if (dma->map == NULL || dma->SGarray == NULL || dma->SGlist == NULL)
		return 2;

	/* Nothing to free */
	if (dma->page_count == 0)
		return 0;

	/* Unmap Scatterlist */
	if (dma->SG_length) {
		pci_unmap_sg(itv->dev, dma->SGlist, dma->page_count,
			     PCI_DMA_TODEVICE);
		dma->SG_length = 0;
	}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
	/* sync DMA */
	pci_dma_sync_single_for_cpu((struct pci_dev *)itv->dev, dma->SG_handle,
				    (sizeof(struct ivtv_SG_element) *
				     IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#else
	/* sync DMA */
	pci_dma_sync_single(itv->dev, dma->SG_handle,
			    (sizeof(struct ivtv_SG_element) *
			     IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

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
	IVTV_DEBUG_INFO("ivtv_free_user_dma\n");

	if (dma->map == NULL || dma->SGarray == NULL || dma->SGlist == NULL)
		return;

	/* Unmap SG Array */
	if (dma->SG_handle != IVTV_DMA_UNMAPPED) {
		pci_unmap_single(itv->dev, dma->SG_handle,
				 (sizeof(struct ivtv_SG_element)) *
				 IVTV_DMA_SG_OSD_ENT, PCI_DMA_TODEVICE);

		dma->SG_handle = IVTV_DMA_UNMAPPED;
	}

	/* Free SG Array */
	kfree(dma->SGarray);
	dma->SGarray = NULL;

	/* Unmap Scatterlist */
	if (dma->SG_length) {
		pci_unmap_sg(itv->dev, dma->SGlist, dma->page_count,
			     PCI_DMA_TODEVICE);
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

int dec_gather_free_buffers(struct ivtv *itv, int streamtype,
			    struct list_head *free_list, int bytes_needed)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	struct ivtv_buffer *buf;
	int bytes_received = 0;
	int x;

	/* Not allocated */
	if (!test_bit(IVTV_F_S_IN_USE, &st->s_flags)) {
		IVTV_DEBUG_WARN("DEC: Gather buffers on stream %d not in use!\n", st->type);
		return 0;
	}	

	/* gather the needed buffers first, so we don't have to bail
	 * in mid-air. put them on a list on the stack */
	for (x = 0; bytes_received < bytes_needed; x++) {
		/* Check full_q for a buffer */
		buf = ivtv_deq_buf_nolock(itv, &st->full_q, streamtype);
		if (buf == NULL)
			break;
		bytes_received += buf->buffer.length;
		list_add_tail(&buf->list, free_list);
	}

	if (bytes_received >= bytes_needed)
		return bytes_received;

	/* damn, failed */
	IVTV_DEBUG_WARN("Needed %d bufs for %s stream, received %d (buffers free %d, dma %d, full %d)\n",
		   bytes_needed, ivtv_stream_name(streamtype), bytes_received,
		   atomic_read(&st->free_q.elements),
		   atomic_read(&st->dma_q.elements),
		   atomic_read(&st->full_q.elements));

	/* Requeue */
	while (!list_empty(free_list)) {
		buf = list_entry(free_list->next, struct ivtv_buffer, list);
		list_del_init(&buf->list);
		ivtv_enq_buf_nolock(&st->full_q, buf);
	}

	return 0;
}

int enc_gather_free_buffers(struct ivtv *itv, int streamtype,
			    struct list_head *free_list, int bytes_needed)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	struct ivtv_buffer *buf;
	int bytes_received = 0;
	int stolen_bufs = 0;  /* keep track if we had to steal buffers */
	int x;

        /* Not allocated */
        if (!test_bit(IVTV_F_S_IN_USE, &st->s_flags)) {
		IVTV_DEBUG_WARN("ENC: Gather buffers on stream %d not in use!\n", st->type);
                return 0;
	}

	/* gather the needed buffers first, so we don't have to bail
	 * in mid-air. put them on a list on the stack */
	for (x = 0; bytes_received < bytes_needed; x++) {
		/* If possible use a buffer from free_q */
		buf = ivtv_deq_buf_nolock(itv, &st->free_q, streamtype);
		if (buf) {
			bytes_received += buf->buffer.length;
			list_add_tail(&buf->list, free_list);
			continue;
		}
			
		/* allocate a new buffer unless the maximum has already been allocated */
		if (st->buf_total < st->buf_max) {
			buf = ivtv_init_buffer(itv, st);
			/* Out of memory? Then break off */
			if (buf == NULL)
				break;
			bytes_received += buf->buffer.length;
			list_add_tail(&buf->list, free_list);
			continue;
		}

		/* Drop Buffers to allow things to continue to work */
		buf = ivtv_deq_buf_nolock(itv, &st->full_q, streamtype);
		if (!buf)
			break;
		stolen_bufs = 1;  /* mark that we had to steal a buffer */

		/* Subtract buffer we stole */
		ivtv_buf_fill_nolock(st, buf->buffer.bytesused, BUF_SUB);
		bytes_received += buf->buffer.length;
		list_add_tail(&buf->list, free_list);
	}

	/* print a warning the first time we have to steal buffers */
	if (st->stolen_bufs == 0 && stolen_bufs) {
		IVTV_WARN("All %s stream buffers are full. Dropping data.\n",
				 ivtv_stream_name(streamtype));
		IVTV_WARN("Cause: the application is not reading fast enough.\n");
	}
	st->stolen_bufs = stolen_bufs;

	/* Got all buffers? */
	if (bytes_received >= bytes_needed) {
		return bytes_received;
	}

	/* Failed to get all buffers */
	IVTV_DEBUG_WARN("Needed %d bufs for %s stream, received %d (buffers free %d, dma %d, full %d)\n",
		   bytes_needed, ivtv_stream_name(streamtype), bytes_received,
		   atomic_read(&st->free_q.elements),
		   atomic_read(&st->dma_q.elements),
		   atomic_read(&st->full_q.elements));

	/* Requeue */
	while (!list_empty(free_list)) {
		buf = list_entry(free_list->next, struct ivtv_buffer, list);
		list_del_init(&buf->list);
		ivtv_enq_buf_nolock(&st->free_q, buf);
	}

	return 0;
}

void ivtv_TO_DMA_done(struct ivtv *itv, int stmtype)
{
	struct ivtv_stream *stream = NULL;
	struct ivtv_buffer *buf;
	int freed = 0;
	u32 freed_bytes = 0;

	IVTV_DEBUG_DMA("TO_DMA: DMA Done\n");

	stream = &itv->streams[stmtype];

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
	pci_dma_sync_single_for_cpu((struct pci_dev *)itv->dev,
				    stream->SG_handle,
				    sizeof(struct ivtv_SG_element) *
				    (stream->buf_min / stream->bufsize), stream->dma);
#else
	pci_dma_sync_single(itv->dev, stream->SG_handle,
			    sizeof(struct ivtv_SG_element) * (stream->buf_min /
							      stream->bufsize),
			    stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

	/* Free last DMA call */
	down(&stream->mlock);
	while ((buf =
		ivtv_deq_buf_nolock(itv, &stream->dma_q, stream->type)) != NULL) {
		freed_bytes += buf->readpos;
		buf->buffer.bytesused = 0;
		buf->readpos = 0;
		buf->ts = 0;
		buf->b_flags = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
		/* Sync Buffer */
		pci_dma_sync_single_for_cpu((struct pci_dev *)itv->dev,
					    buf->dma_handle, buf->buffer.length,
					    stream->dma);
#else
		/* Sync Buffer */
		pci_dma_sync_single(itv->dev, buf->dma_handle,
				    buf->buffer.length, stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

		//if (stream->buf_total <= stream->buf_min) {
			ivtv_enq_buf_nolock(&stream->free_q, buf);
		/*} else {
			ivtv_free_buffer(itv, buf, stream);
			buf = NULL;
		}*/
		freed++;
	}
	if (freed_bytes)
		ivtv_buf_fill_nolock(stream, freed_bytes, BUF_SUB);
	up(&stream->mlock);

	return;
}


int dma_to_device(struct ivtv *itv, struct ivtv_stream *st,
			dma_addr_t SG_handle, int waitVsync)
{
	unsigned long then;
	unsigned long flags;
	int redo_dma = 0;
	int ret = 0;
	/* wait for DMA complete status */
	then = jiffies;
	while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x03)) {
		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "DMA_TO: REG_DMASTATUS wait failed\n");
			break;
		}
	}

	/* wait for DMA register clear */
	then = jiffies;
	while ((readl(itv->reg_mem + IVTV_REG_DMAXFER) & 0x03)) {
		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "DMA_TO: REG_DMAXFER wait failed\n");
			break;
		}
	}

	if (waitVsync)
        {
            wait_event_interruptible(itv->vsync_w_yuv,
                       atomic_read(&itv->yuv_vsync));

            atomic_set(&itv->yuv_vsync, 0);
        }

      redo_dma:
	spin_lock_irqsave(&itv->DMA_slock, flags);
	/* put SG Handle into register 0x0c */
	ivtv_write_reg(SG_handle, itv->reg_mem + IVTV_REG_DECDMAADDR);
	/* If didn't write, try again */
	if (readl(itv->reg_mem + IVTV_REG_DECDMAADDR) != SG_handle)
		ivtv_write_reg(st->SG_handle,
			       itv->reg_mem + IVTV_REG_DECDMAADDR);
	/* Send DMA with register 0x00, using the enc DMA bit */
	if (readl(itv->reg_mem + IVTV_REG_DECDMAADDR) == SG_handle)
		ivtv_write_reg(readl(itv->reg_mem + IVTV_REG_DMAXFER) |
			       0x01, itv->reg_mem + IVTV_REG_DMAXFER);
	spin_unlock_irqrestore(&itv->DMA_slock, flags);

	/* wait for DMA to start */
	then = jiffies;
	while ((readl(itv->reg_mem + IVTV_REG_DMAXFER) & 0x01)) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x15) {
			break;
		}

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "DMA_TO: REG_DMAXFER 2 wait failed\n");
			break;
		}
	}

	then = jiffies;
	while (!(readl(itv->reg_mem + IVTV_REG_DECSG1LEN) & 0x80000000)) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x15)
			break;

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "DEC: REG_DECSG1LEN wait failed\n");
			break;
		}
	}
	/* Wait for Read Interrupt */
	while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x15) &&
	       wait_event_interruptible(itv->r_intr_wq,
					atomic_read(&itv->r_intr))) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x15)
			break;

		if (atomic_read(&itv->r_intr))
			break;
	}
	atomic_set(&itv->r_intr, 0);
	/* wait for DMA complete status */
	then = jiffies;
	while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x01)) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x15) {
			break;
		}

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "DMA_TO: REG_DMASTATUS2 wait failed\n");
			break;
		}
	}

	/* DMA Error */
	if ((readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x14)) {
		IVTV_DEBUG_DMA(
			   "DMA_TO: (%d) DMA Error 0x%08x\n",
			   redo_dma, readl(itv->reg_mem +
						   IVTV_REG_DMASTATUS));

		/* Reset DMA Error, cancel last DMA? */
		spin_lock_irqsave(&itv->DMA_slock, flags);
		ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
		ivtv_write_reg(readl(itv->reg_mem + IVTV_REG_DMASTATUS)
			       & 0x03, itv->reg_mem + IVTV_REG_DMASTATUS);
		spin_unlock_irqrestore(&itv->DMA_slock, flags);
		if (redo_dma < 3) {
			redo_dma++;
			goto redo_dma;
		}
		IVTV_DEBUG_WARN(
			   "DMA_TO: REDO DMA took too many tries.\n");
		ret = -ENOMEM;
	}

	return ret;
}

int ivtv_stream_alloc(struct ivtv *itv, int streamtype)
{
	struct ivtv_stream *s = &itv->streams[streamtype];
        int SGsize = 0;

	if (atomic_read(&s->allocated_buffers)) {
		IVTV_ERR("Stream %d already allocated!\n", streamtype);
		return -EBUSY;
	}

	if (s->buffers) {
                SGsize = sizeof(struct ivtv_SG_element) * (s->buf_min / s->bufsize);
		IVTV_INFO("Allocate %s%s stream: "
				"%d x %d buffers (%dKB total)\n",
				s->dma != PCI_DMA_NONE ? "DMA " : "",
				ivtv_stream_name(streamtype), 
				(s->buf_min / s->bufsize), s->bufsize,
				(s->buf_min / 1024));
	} else {
		IVTV_DEBUG_INFO("Allocate %s%s stream\n",
			       s->dma != PCI_DMA_NONE ? "DMA " : "",
			       ivtv_stream_name(streamtype)); 
	}

	/* Allocate DMA SG Arrays */
	if (s->dma != PCI_DMA_NONE) {
		if (s->SGarray == NULL) {
			s->SGarray = (struct ivtv_SG_element *)
			    kmalloc(SGsize, GFP_KERNEL);
			if (s->SGarray == NULL) {
				IVTV_ERR("Could not allocate SGarray\n");
				return -ENOMEM;
			}
			s->SG_handle = IVTV_DMA_UNMAPPED;
			s->SG_length = 0;
		}
		/* Set SGarray to all 0's */
		memset(s->SGarray, 0, SGsize);

		s->SG_handle = pci_map_single(itv->dev, s->SGarray, SGsize, s->dma);
	}

	/* allocate stream buffers */
	if (s->buffers && ivtv_init_queues_nolock(itv,
				  streamtype, s->buf_min, s->bufsize) < 0) {
		IVTV_ERR("Couldn't allocate buffers for %s stream\n",
			      ivtv_stream_name(streamtype));

		/* Free SG Array/Lists */
		if (s->SGarray != NULL) {
			if (s->SG_handle != IVTV_DMA_UNMAPPED) {
				pci_unmap_single(itv->dev, s->SG_handle, SGsize,
						 s->dma);
				s->SG_handle = IVTV_DMA_UNMAPPED;
			}
			s->SG_length = 0;
			kfree(s->SGarray);
		}
		return -ENOMEM;
	}

	return 0;
}

void ivtv_stream_free(struct ivtv *itv, int stream)
{
	struct ivtv_stream *s = &itv->streams[stream];

	/* Free Buffers */
	if (atomic_read(&s->allocated_buffers)) {
		ivtv_free_queues_lock(itv, stream);
		atomic_set(&s->allocated_buffers, 0);
	}

	/* Free User SG Array/Lists */
	if (s->udma.map != NULL) {
		kfree(s->udma.map);
		s->udma.map = NULL;
	}

	/* Free User SG Array/Lists */
	if (s->udma.SGlist != NULL) {
		kfree(s->udma.SGlist);
		s->udma.SGlist = NULL;
	}

	/* Free User SG Array/Lists */
	if (s->udma.SGarray != NULL) {
		kfree(s->udma.SGarray);
		s->udma.SGarray = NULL;
	}

	/* Free SG Array/Lists */
	if (s->SGarray != NULL) {
		if (s->SG_handle != IVTV_DMA_UNMAPPED) {
			pci_unmap_single(itv->dev, s->SG_handle,
					 (sizeof(struct ivtv_SG_element) *
					  (s->buf_min / s->bufsize)), s->dma);
			s->SG_handle = IVTV_DMA_UNMAPPED;
		}
		s->SG_length = 0;

		kfree(s->SGarray);
		s->SGarray = NULL;
	}
	return;
}

int unlock_TO_dma(struct ivtv *itv, int stream_type)
{
	struct ivtv_stream *stream = &itv->streams[stream_type];
	struct ivtv_stream *dec_stream = NULL;
	struct ivtv_stream *osd_stream = NULL;

	/* IF we have OSD/DEC Streams to unlock */
	if (itv->has_cx23415) {
		osd_stream = &itv->streams[IVTV_DEC_STREAM_TYPE_OSD];
		dec_stream = &itv->streams[IVTV_DEC_STREAM_TYPE_MPG];

		/* Unlock DMA */
		clear_bit(DMA_IN_USE, &osd_stream->udma.u_flags);
		clear_bit(DMA_IN_USE, &dec_stream->udma.u_flags);
		clear_bit(IVTV_F_S_DMAP, &osd_stream->s_flags);
		clear_bit(IVTV_F_S_DMAP, &dec_stream->s_flags);
	}

	/* Unlock streams if not already unlocked above */
	if (stream_type != IVTV_DEC_STREAM_TYPE_OSD && 
		(stream_type != IVTV_DEC_STREAM_TYPE_MPG))
	{
		clear_bit(DMA_IN_USE, &stream->udma.u_flags);
		clear_bit(IVTV_F_S_DMAP, &stream->s_flags);
	}

        /* Wake up waitq's for DMA */
        if (itv->has_cx23415 &&
		!test_bit(IVTV_F_S_NEEDS_DATA, &stream->s_flags)) 
	{
		/* Wake up OSD streams, maybe DEC if no OSD needed */
               	wake_up(&osd_stream->waitq);
               	wake_up(&osd_stream->udma.waitq);

		if (!test_bit(IVTV_F_S_OSD_NEEDS_DATA, &stream->s_flags)) {
               		wake_up(&dec_stream->waitq);
               		wake_up(&dec_stream->udma.waitq);
		}

		/* Wake up streams if not already unlocked above */
		if (stream_type != IVTV_DEC_STREAM_TYPE_OSD &&
			(stream_type != IVTV_DEC_STREAM_TYPE_MPG))
		{
                	wake_up(&stream->udma.waitq);
                	wake_up(&stream->waitq);
		}
	} else if (itv->has_cx23415) {/* Wake up next DEC DMA in wait */
               	wake_up(&dec_stream->waitq);
               	wake_up(&dec_stream->udma.waitq);

		/* Wake up streams if not already unlocked above */
		if (stream_type != IVTV_DEC_STREAM_TYPE_OSD &&
			(stream_type != IVTV_DEC_STREAM_TYPE_MPG))
		{
                	wake_up(&stream->udma.waitq);
                	wake_up(&stream->waitq);
		}
	} else { /* Not Decoder/OSD */
		wake_up(&stream->waitq);
		wake_up(&stream->udma.waitq);
	}	

	return 0;
}

const char *ivtv_stream_name(int streamtype)
{
	switch (streamtype)  {
		case IVTV_ENC_STREAM_TYPE_MPG:
			return "encoder MPEG";
		case IVTV_ENC_STREAM_TYPE_YUV:
			return "encoder YUV";
		case IVTV_ENC_STREAM_TYPE_VBI:
			return "encoder VBI";
		case IVTV_ENC_STREAM_TYPE_PCM:
			return "encoder PCM audio";
		case IVTV_ENC_STREAM_TYPE_RAD:
			return "encoder radio";
		case IVTV_DEC_STREAM_TYPE_MPG:
			return "decoder MPEG";
		case IVTV_DEC_STREAM_TYPE_VBI:
			return "decoder VBI";
		case IVTV_DEC_STREAM_TYPE_VOUT:
			return "decoder VOUT";
		case IVTV_DEC_STREAM_TYPE_YUV:
			return "decoder YUV";
		case IVTV_DEC_STREAM_TYPE_OSD:
			return "decoder OSD";
		default:
			return "unknown"; 
	}
}
