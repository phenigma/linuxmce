/* interrupt handling
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
#include "ivtv-dma.h"
#include "ivtv-firmware.h"
#include "ivtv-fileops.h"
#include "ivtv-queue.h"
#include "ivtv-irq.h"
#include "ivtv-ioctl.h"
#include "ivtv-mailbox.h"
#include "ivtv-vbi.h"
#include "ivtv-reset.h"

static void ivtv_DMA_done(struct ivtv *itv, int type);

static void ivtv_swap_copy(const char *buf, const char *ubuf, size_t count)
{
	u32 *src, *dst;

	src = (u32 *)ubuf;
	dst = (u32 *)buf;

#ifdef CONFIG_X86
	while ((unsigned long)src <= (unsigned long)ubuf + count) { /* byteswap while copying */
		__asm__ __volatile__ ("bswap %0" :
				      "=r" (*dst) :
				      "0" (*src) );
		src++; dst++;
	}
#else
	{
		int y;

		/* Old slow memcpy then swab */
		memcpy((void *)buf, (void *)ubuf, count);
		for (y = 0; y < count; y += 4) {
			swab32s((u32 *)(buf + y));
		}
	}
#endif
}

IRQRETURN_T ivtv_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	struct ivtv *itv = (struct ivtv *)dev_id;
	u32 combo;
	u32 stat;

	spin_lock(&itv->DMA_slock);
	/* get contents of irq status register*/
	stat = readl(itv->reg_mem + IVTV_REG_IRQSTATUS);

	combo = ~itv->irqmask & stat;

	if (0 == combo) {
		/* wasn't for us*/
		spin_unlock(&itv->DMA_slock);
		return IRQ_NONE;
	}

	/* Clear out IRQ */
	writel(combo, (itv->reg_mem + IVTV_REG_IRQSTATUS));
	spin_unlock(&itv->DMA_slock);
        
	IVTV_DEBUG(IVTV_DEBUG_IRQ, 
		"======= valid IRQ bits: 0x%08x ======\n", combo);

        if (combo & IVTV_IRQ_DEC_DMA_ERR2) {
                /* Mask Decoder DMA Error */
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"IRQ: IVTV_IRQ_DEC_DMA_ERR2 intr\n");
	}

        if (combo & IVTV_IRQ_DEC_DMA_ERR3) {
                /* Mask Decoder DMA Error */
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"IRQ: IVTV_IRQ_DEC_DMA_ERR3 intr\n");
	}

        if (combo & IVTV_IRQ_DEC_DMA_26) {
                /* Mask */
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"IRQ: IVTV_IRQ_DEC_DMA_26 intr\n");
	}

        if (combo & IVTV_IRQ_DEC_DMA_23) {
                /* Mask */
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"IRQ: IVTV_IRQ_DEC_DMA_23 intr\n");
	}

        if (combo & IVTV_IRQ_DEC_DMA_15) {
                /* Mask */
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"IRQ: IVTV_IRQ_DEC_DMA_15 intr\n");
	}

        if (combo & IVTV_IRQ_DEC_DMA_ERR) {
                /* Mask Decoder DMA Error */
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"IRQ: IVTV_IRQ_DEC_DMA_ERR intr 1\n");
		ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);
		spin_lock(&itv->DMA_slock);
		writel(0x00000000, itv->reg_mem + IVTV_REG_DMAXFER);
		spin_unlock(&itv->DMA_slock);

                /* Check if firmware needs to be reloaded */
                set_bit(FW_SOFT_RESET_NEEDED, &itv->r_flags);

                /* Reload Firmware Work Queue */
                fw_schedule_work(itv);
	}

	/* Define irq bits */
	#define IVTV_IRQ_ENC_DMA (IVTV_IRQ_ENC_DMA_COMPLETE| \
			  IVTV_IRQ_ENC_START_CAP| \
			  IVTV_IRQ_ENC_VBI_CAP| \
			  IVTV_IRQ_DEC_VBI_RE_INSERT|\
			  IVTV_IRQ_ENC_EOS)

	#define IVTV_IRQ_DEC_DMA (IVTV_IRQ_DEC_DMA_COMPLETE| \
			  IVTV_IRQ_DEC_DATA_REQ)

        if (combo & IVTV_IRQ_ENC_DMA) {
		/* Encoder */
                if (combo & IVTV_IRQ_ENC_DMA_COMPLETE) {
                        /* set the dma done flag */
                        IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ ENC DMA Complete\n");
                }

                if (combo & IVTV_IRQ_ENC_START_CAP) {
                        /* set dma needed flag */
                        IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ ENC DMA\n");


                        set_bit(IVTV_F_T_ENC_DMA_NEEDED, &itv->t_flags);
                	atomic_inc(&itv->enc_dma_stat_intr);
                        enc_schedule_work(itv);
                }

                if (combo & IVTV_IRQ_ENC_VBI_CAP) {
                        /* set vbi dma needed flag */
                        IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ ENC VBI DMA\n");


                        set_bit(IVTV_F_T_ENC_VBI_DMA, &itv->t_flags);
                	atomic_inc(&itv->enc_dma_stat_intr);
                        enc_schedule_work(itv);
                }
 
                if (combo & IVTV_IRQ_DEC_VBI_RE_INSERT) {
                        IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ DEC VBI Reinsertion\n");


                        set_bit(IVTV_F_T_DEC_VBI_INSERT, &itv->t_flags);
                	atomic_inc(&itv->enc_dma_stat_intr);
                        enc_schedule_work(itv);
                }

		if (combo & IVTV_IRQ_ENC_EOS) {
			IVTV_DEBUG(IVTV_DEBUG_IRQ, "Encoder End Of Stream\n");
			set_bit(IVTV_F_I_EOS, &itv->i_flags);

			wake_up(&itv->cap_w);
		}
	}

        if (combo & IVTV_IRQ_DEC_DMA) {
		int type = IVTV_DEC_STREAM_TYPE_MPG;

        	/* YUV or MPG */
        	if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))
                	type = IVTV_DEC_STREAM_TYPE_YUV;
        	else
                	type = IVTV_DEC_STREAM_TYPE_MPG;

                /* Decoder */
                if (combo & IVTV_IRQ_DEC_DMA_COMPLETE) {
                        IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ DEC DMA Done\n");
                }

                if (combo & IVTV_IRQ_DEC_DATA_REQ) {
                        IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ DEC Data Request\n");

			set_bit(IVTV_F_S_NEEDS_DATA, 
				&itv->streams[type].s_flags);

                	atomic_inc(&itv->dec_dma_stat.intr);
                        IVTV_DEBUG(IVTV_DEBUG_DEC, 
				"* (%lu) DEC IRQ: intr %d wq_runs %d\n", 
				jiffies,
				atomic_read(&itv->dec_dma_stat.intr),
				itv->dec_dma_stat.wq_runs);

                        dec_schedule_work(itv);
                }
        }

	/* Decoder Vertical Sync */
	if (combo & IVTV_IRQ_DEC_VSYNC) {
		IVTV_DEBUG(IVTV_DEBUG_IRQ, "IRQ Decoder Vsync\n");
		itv->dec_dma_stat.vsync_count++;
		wake_up_interruptible(&itv->vsync_w);
	}

	if (combo & IVTV_IRQ_ENC_VIM_RST) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "VIM Restart\n");

		/* Signal action was done */
                set_bit(IVTV_F_I_DIG_RST, &itv->i_flags);
	}

	if (combo & IVTV_IRQ_DEC_COPY_PROTECT) {
		IVTV_DEBUG(IVTV_DEBUG_IRQ, "Copy Protection\n");
	}

	if (combo & IVTV_IRQ_DEC_AUD_MODE_CHG) {
		IVTV_DEBUG(IVTV_DEBUG_IRQ, "Audio mode change\n");
	}

	if (combo & IVTV_IRQ_DEC_IFRAME_DONE) {
		IVTV_DEBUG(IVTV_DEBUG_IRQ, "I-Frame done\n");
	}

        if (combo & IVTV_IRQ_DEC_DMA_ERR) {
                IVTV_DEBUG(IVTV_DEBUG_ERR, "IRQ: IVTV_IRQ_DEC_DMA_ERR intr 2\n");
		ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);
        }

	return IRQ_HANDLED;
}

static void ivtv_DMA_done(struct ivtv *itv, int stmtype) 
{
	struct ivtv_stream *stream=NULL;
	struct ivtv_buffer *buf;
	int freed = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "ENC: DMA Done\n");

	stream = &itv->streams[stmtype];

	down(&stream->mlock);
	while ((buf = ivtv_deq_buf_nolock(&stream->dma_q, stream->type))) {
		/* flag byteswap ABCD -> DCBA for MPG & VBI data outside irq */
		if (stmtype == IVTV_ENC_STREAM_TYPE_MPG)
			set_bit (IVTV_F_B_NEED_BUF_SWAP, &buf->b_flags);

		/* Sync Buffer */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
        	pci_dma_sync_single_for_cpu((struct pci_dev*)itv->dev, buf->dma_handle,
               		buf->buffer.length, stream->dma);
#else
        	pci_dma_sync_single(itv->dev, buf->dma_handle,
               		buf->buffer.length, stream->dma);
#endif

		ivtv_enq_buf_nolock(&stream->full_q, buf);
		ivtv_buf_fill_nolock(stream, buf->buffer.bytesused, BUF_ADD);

		freed++;
	}
	/* Add to buffer filled bytes */
	up(&stream->mlock);	

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
        pci_dma_sync_single_for_cpu((struct pci_dev*)itv->dev, stream->SG_handle,
               sizeof(struct ivtv_SG_element) * (stream->buf_max/PAGE_SIZE),
               stream->dma);
#else
        pci_dma_sync_single(itv->dev, stream->SG_handle,
               sizeof(struct ivtv_SG_element) * (stream->buf_max/PAGE_SIZE),
               stream->dma);
#endif
}

/* Must be spinlocked */
static int gather_free_buffers(struct ivtv *itv, int streamtype,
		struct list_head *free_list, int bytes_needed)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	struct ivtv_buffer *buf;
	int bytes_received = 0;
	int x;

	/* gather the needed buffers first, so we don't have to bail
	 * in mid-air. put them on a list on the stack */
	for (x = 0; bytes_received < bytes_needed; x++) {
                /* Check free_q or allocate a new buffer */
                buf = ivtv_deq_buf_nolock(&st->free_q, streamtype);
                if (buf) {
                        buf->buffer.bytesused   = 0;
                        buf->readpos = 0;
                        buf->ts = 0;
                        buf->b_flags = 0;

			bytes_received += buf->buffer.length;

                        IVTV_DEBUG(IVTV_DEBUG_DMA,
                                "ENC: Got FREE Buff with %d bytes.\n",
                                 buf->buffer.bytesused);
                } else {
			/* Too many allocated? */
			if (st->buf_total >= st->buf_max) {
				/* Drop Buffers to allow things
                                   to continue to work */
                		buf = ivtv_deq_buf_nolock(&st->full_q, streamtype);
				if (!buf) {
                                	IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        	"ENC IRQ OVERFLOW: Allocating a Buffer. (%d)\n",
                                        	atomic_read(&st->allocated_buffers));

                			buf = ivtv_init_buffer(itv, st);
					/* Failed to even allocate */
					if (!buf)
						break;
				} else {
                                	IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        	"ENC IRQ OVERFLOW: #%d Stealing a Buffer, %d currently allocated\n",
						atomic_read(&st->stolen_bufs),
                                        	atomic_read(&st->allocated_buffers));
					atomic_inc(&st->stolen_bufs);
				}

				/* Subtract buffer we stole */
				ivtv_buf_fill_nolock(st, buf->buffer.bytesused, BUF_SUB);
				bytes_received += buf->buffer.length;
			} else {
                		buf = ivtv_init_buffer(itv, st);
                        	if (buf) {
					bytes_received += buf->buffer.length;

                                	IVTV_DEBUG(IVTV_DEBUG_DMA,
                                        	"ENC IRQ: Allocating a Buffer. (%d)\n",
                                        	atomic_read(&st->allocated_buffers));
                        	} else
					break;
			}
                }
		list_add_tail(&buf->list, free_list);
	}

	/* damn, failed */
	if (bytes_received < bytes_needed) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,
			"needed %d bufs for stream %d, received %d "
			"differ by (%d) bufs\n",
			bytes_needed, streamtype, bytes_received, 
			(bytes_needed - bytes_received));

        	IVTV_DEBUG(IVTV_DEBUG_INFO,
                   	"SCHED: allocated_buffers: (%d)\n",
                   	atomic_read(&st->allocated_buffers));

		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"SCHED: free_q: %d elements\n", 
			atomic_read(&st->free_q.elements));
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"SCHED: dma_q: %d elements\n", 
			atomic_read(&st->dma_q.elements));
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"SCHED: full_q: %d elements\n", 
			atomic_read(&st->full_q.elements));
		
                /* Either requeue or free */
		while (!list_empty(free_list)) {
			buf = list_entry(free_list->next, 
				struct ivtv_buffer, list);
			list_del_init(&buf->list);

                        if (st->buf_total <= st->buf_min) {
                		ivtv_enq_buf_nolock(&st->free_q, buf);
			} else {
				ivtv_free_buffer(itv, buf, st);
				buf = NULL;
			}
		}

		return 0;
	}
	return bytes_received;
}

/* FIXME this function is getting too long. split it up? */
void ivtv_sched_DMA(struct ivtv *itv) 
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	u32 type, size, offset, streamtype, pad = 0;
	u32 UVsize=0, UVoffset=0;
	u64 pts_stamp = 0;
	struct ivtv_stream *st;
	int x;
	int uvflag=0;
	long sequence;
	u32 bytes_needed = 0, bytes_read = 0, bytes_received = 0, sg_size = 0;
        struct ivtv_buffer *buf;
        LIST_HEAD(free_list);
	int xfer_pad;
	u32 then;
	int redo_dma = 0;
	DECLARE_WAITQUEUE(wait, current);
	int rc = 0;
	unsigned long flags;
	int sg_offset = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "ENC: Sched DMA\n");

	/* Get DMA destination and size arguments from card */
	x = ivtv_api_getresult_nosleep(&itv->enc_mbox[9], &result, &data[0]);
	if (x) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "error:%d getting DMA info\n", x);
		return;
	}
	type = data[0];

	if (data[1] == 0 || data[2] == 0)
		return;
	
	switch (type) {
		case 0:  /* MPEG */
			offset = data[1];
			size   = data[2];
			IVTV_DEBUG(IVTV_DEBUG_INFO,
				   "DMA/MPG type 0x%08x,size 0x%08x,offset 0x%08x\n",
	 			   type, size, offset);
			streamtype = IVTV_ENC_STREAM_TYPE_MPG;
			break;

		case 1: /* YUV */
			offset		= data[1];
			size		= data[2];
			UVoffset	= data[3];
			UVsize		= data[4];
			pts_stamp	= ((u64)data[5] << 32) | data[6];
			IVTV_DEBUG(IVTV_DEBUG_INFO,
				   "DMA/YUV type 0x%08x,Ysize 0x%08x,Yoffset 0x%08x,"
				   "UVsize 0x%08x,UVoffset 0x%08x,PTS 0x%09llx\n",
	 			   type, size, offset, UVsize, UVoffset, pts_stamp);
			streamtype = IVTV_ENC_STREAM_TYPE_YUV;
			break;

		case 2:  /* PCM (audio) */
			offset = data[1] + 12;
			size   = data[2] - 12;
			pts_stamp = *(u32 *)(itv->dec_mem + offset - 8);
			pts_stamp |= (u64)(*(u32 *)(itv->dec_mem + offset - 12)) << 32;
			offset += IVTV_DECODER_OFFSET;
			IVTV_DEBUG(IVTV_DEBUG_INFO,
				"DMA/PCM type 0x%08x,size 0x%08x,offset 0x%08x "
				"PTS 0x%09llx\n", 
				type, size, offset, pts_stamp);
                        streamtype = IVTV_ENC_STREAM_TYPE_PCM;
			break;

		default:
			IVTV_DEBUG(IVTV_DEBUG_ERR,
				"DMA/UNKNOWN type 0x%08x, NOT SUPPORTED\n", 
				type);
			return;
	}
	/* FIXME should check for null on the stream element */
	if (streamtype >= itv->streamcount) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,
			"No stream handler for type %d\n", streamtype);
		return;
	}

	st = &itv->streams[streamtype];
	st->pts = pts_stamp;
	st->dmatype = type;

	if (test_bit(IVTV_F_S_NO_READ, &st->s_flags)) {
		return;
	}
	bytes_needed = size + UVsize;
	if (type == 1)
		bytes_needed += (UVsize%itv->dma_cfg.enc_yuv_buf_size) + (size%itv->dma_cfg.enc_yuv_buf_size);

        then = jiffies;
        add_wait_queue(&st->waitq, &wait);
        do {
                set_current_state(TASK_INTERRUPTIBLE);

                if (test_bit(FW_RESET_SHUTDOWN, &itv->r_flags)) {
                        rc = -EINTR;
                        break;
                }

                /* Lock Encoder */
                if ((ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) & 0x02) &&
                        ! (ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) & 0x18) &&
                        !test_and_set_bit(IVTV_F_S_DMAP, &st->s_flags))
                {
                        break;
                }

		if (ivtv_sleep_timeout(HZ/100, 1)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "ENC: User stopped waitq.\n");

                        rc = -EINTR;
                        break;
		}
        } while (!rc);
        set_current_state(TASK_RUNNING);
        remove_wait_queue(&st->waitq, &wait);

        if (rc)
                return;

        /* Wait till buffers are available */
        then = jiffies;
        while (ivtv_buf_fill_lock(st, 0, BUF_EMPTY) < bytes_needed) {
                ivtv_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > HZ) {
                        IVTV_DEBUG(IVTV_DEBUG_INFO,
                        "ENC: Only 0x%08x bytes available need %d.\n",
                                ivtv_buf_fill_lock(st, 0,
                                	BUF_EMPTY), bytes_needed);
			break;
                }
        }

	down(&st->mlock);
        bytes_received =
                gather_free_buffers(itv, streamtype, &free_list, bytes_needed);
	up(&st->mlock);

        if (bytes_received == 0) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "ENC: ENC got (%d) needed %d bytes for stream %d, "
                        "only %d differ by (%d) bytes\n",
                        bytes_received,
                        bytes_needed, streamtype,
                        (st->buf_total - st->buf_fill),
                        (bytes_needed - (st->buf_total - st->buf_fill)));

                set_bit(IVTV_F_S_OVERFLOW, &st->s_flags);

		clear_bit(IVTV_F_S_DMAP, &st->s_flags);
                return;
        }
	
	/* increment the sequence # */
	sequence = ++st->seq;

	st->ubytes = size;

	down(&st->mlock);
	for (x = 0;bytes_read < bytes_needed; x++) {
                if (list_empty(&free_list)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "ENC: Error, No more Buffers!!!.\n");
                        break;
                }

                /* extract the buffers we procured earlier */
                buf = list_entry(free_list.next, struct ivtv_buffer, list);
                list_del_init(&buf->list);

                buf->readpos = 0;
                buf->buffer.index = x;
                buf->buffer.sequence = sequence;
                buf->ts = jiffies;
                buf->b_flags = 0;

               	bytes_read += buf->buffer.length;

		if (size < buf->buffer.length) {
			xfer_pad = buf->buffer.length/*256*/;
			pad = size;
                        buf->buffer.bytesused = size;
			if (size > xfer_pad && size%xfer_pad) /* Align */
				size = size+(size%xfer_pad);
			if (size < xfer_pad) /* Too small */
				size = xfer_pad;
			st->SGarray[x].size	= size;
			size 			= 0;
		}
		else {
			pad 			= 0;
                        buf->buffer.bytesused 	= buf->buffer.length;
			st->SGarray[x].size	= buf->buffer.length;
			size		       -= st->SGarray[x].size;
		}
		st->SGarray[x].src = offset;		/* Encoder Addr */
		st->SGarray[x].dst = buf->dma_handle;	/* Encoder Addr */
		offset += st->SGarray[x].size;		/* Increment Enc Addr */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
        	/* Sync SG buffers */
        	pci_dma_sync_single_for_device((struct pci_dev*)itv->dev, buf->dma_handle,
               		buf->buffer.length, st->dma);
#else
        	/* Sync SG buffers */
        	pci_dma_sync_single(itv->dev, buf->dma_handle,
               		buf->buffer.length, st->dma);
#endif

		ivtv_enq_buf_nolock(&st->dma_q, buf);

		if ((size == 0) && (type == 1) && (uvflag == 0)) { /* YUV */
			/* process the UV section */
			offset	= UVoffset;
			size	= UVsize;
			uvflag	= 1;
		}
	}
	up(&st->mlock);

	st->SG_length = x;
 
	/* This should wrap gracefully */
	st->trans_id = itv->trans_id++;

	/* Mark last buffer size for Interrupt flag */
	st->SGarray[st->SG_length-1].size |= 0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
        /* Sync Hardware SG List of buffers */
        pci_dma_sync_single_for_device((struct pci_dev*)itv->dev, st->SG_handle,
               	sizeof(struct ivtv_SG_element) * (st->buf_max/PAGE_SIZE),
               	st->dma);
#else
        /* Sync Hardware SG List of buffers */
        pci_dma_sync_single(itv->dev, st->SG_handle,
               	sizeof(struct ivtv_SG_element) * (st->buf_max/PAGE_SIZE),
               	st->dma);
#endif

	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"Sched dma: addr: 0x%08llx, SG_length: %d, array_size 0x%08x,"
		" type 0x%08x\n", (u64)st->SG_handle, 
		st->SG_length, bytes_read, type);

	/* Calculate sg array bytes */
	sg_size = st->SG_length*(3*4);

	down(&itv->DMA_lock);

        /* wait for DMA complete status */
        then = jiffies;
        while (!(ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMASTATUS) & 0x00000002))
        {
                ivtv_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > (HZ*3)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "ENC: REG_DMASTATUS wait failed\n");
			ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);
                        break;
		}
        }
redo_dma:

	spin_lock_irqsave(&itv->DMA_slock, flags);
        /* put SG Handle into register 0x0c */
        ivtv_write_reg(st->SG_handle,
                itv->reg_mem + IVTV_REG_ENCDMAADDR);

        /* Send DMA with register 0x00, using the enc DMA bit */
        ivtv_write_reg(0x02, itv->reg_mem + IVTV_REG_DMAXFER);
	spin_unlock_irqrestore(&itv->DMA_slock, flags);

        /* wait for DMA to be finished, check interrupt bit */
        if (st->SG_length <= 8)
                sg_offset = ((12*st->SG_length)-4);
        else {
                sg_offset = ((12*(st->SG_length%8))-4);
        }
        then = jiffies;
        while (!(ivtv_read_reg(itv->reg_mem +
                       IVTV_REG_ENCSG1SRC + sg_offset) & 0x80000000))
        {
               /* DMA Error */
               if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000018)
                       break;

               ivtv_sleep_timeout(HZ/100, 1);
               if ((jiffies - then) > HZ) {
                       IVTV_DEBUG(IVTV_DEBUG_ERR,
                               "DEC: REG_ENCSG1LEN wait failed\n");

		       ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                       /* Reset DMA, cancel last DMA? */
		       spin_lock_irqsave(&itv->DMA_slock, flags);
                       ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
		       spin_unlock_irqrestore(&itv->DMA_slock, flags);
                       break;
               }
       }

        /* wait for DMA xfer to finish */
        then = jiffies;
        while ((ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMAXFER) & 0x00000002))
        {
                /* DMA Error */
                if (ivtv_read_reg(itv->reg_mem + 
				IVTV_REG_DMASTATUS) & 0x00000018) 
		{
                        	break;
		}

                ivtv_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > (HZ*3)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "ENC: REG_DMAXFER wait failed\n");

		        ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        /* Reset DMA, cancel last DMA? */
		        spin_lock_irqsave(&itv->DMA_slock, flags);
                        ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
		        spin_unlock_irqrestore(&itv->DMA_slock, flags);
                        break;
		}
        }

        /* wait for DMA complete status */
        then = jiffies;
        while (!(ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMASTATUS) & 0x00000002))
        {
                /* DMA Error */
                if (ivtv_read_reg(itv->reg_mem + 
				IVTV_REG_DMASTATUS) & 0x00000018) 
		{
                        	break;
		}

                ivtv_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > (HZ*3)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "ENC: REG_DMASTATUS2 wait failed\n");

		        ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        break;
		}
        }

        /* DMA Error */
        if ((ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMASTATUS) & 0x00000018))
        {
                IVTV_DEBUG(IVTV_DEBUG_DMA, 
				"ENC: (%d) DMA Error 0x%08x\n",
                        redo_dma, ivtv_read_reg(itv->reg_mem +
						IVTV_REG_DMASTATUS));

                /* Reset DMA Error, cancel last DMA? */
	        spin_lock_irqsave(&itv->DMA_slock, flags);
                ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
	        spin_unlock_irqrestore(&itv->DMA_slock, flags);
                if (redo_dma < 3) {
                        redo_dma++;
                        goto redo_dma;
                } else
			IVTV_DEBUG(IVTV_DEBUG_ERR,
				"ENC: REDO DMA took too many tries.\n");
        }

	/* Gather Buffers from DMA xfer to read */
	ivtv_DMA_done(itv, streamtype);

	/* Unlock ENC DMA */
	up(&itv->DMA_lock);

	clear_bit(IVTV_F_S_DMAP, &st->s_flags);

	wake_up(&st->waitq);
}

/* FIXME this function is getting too long. split it up? */
void ivtv_sched_VBI(struct ivtv *itv, int streamtype) 
{
	u32 size, offset;
	u64 pts_stamp;
	struct ivtv_stream *st;
	int x;
	struct ivtv_buffer *buf;
	LIST_HEAD(free_list);
	long sequence;
	u32 bytes_needed = 0, bytes_read = 0, bytes_received = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "VBI: Sched DMA\n");

	if (streamtype == IVTV_ENC_STREAM_TYPE_VBI) {
		size   = itv->vbi_enc_size;
		offset = itv->vbi_enc_start + itv->vbi_index * (size + 12) + 12;
		itv->vbi_index = (itv->vbi_index + 1) % itv->vbi_total_frames;
		pts_stamp = *(u32 *)(itv->enc_mem + offset - 4);
		pts_stamp |= (u64)(*(u32 *)(itv->enc_mem + offset - 8)) << 32;
		IVTV_DEBUG(IVTV_DEBUG_DMA,
			"DMA/VBI type 0x%08x, size 0x%08x, offset 0x%08x,"
			"PTS 0x%09llx\n", 
			streamtype, size, offset, pts_stamp);
		itv->vbi_frame++;
	}
	else {
		offset = ivtv_read_reg(itv->dec_mem + itv->vbi_dec_start);
		offset += itv->vbi_dec_start;
		size = ivtv_read_reg(itv->dec_mem + itv->vbi_dec_start + 4);
		pts_stamp = 0;
		IVTV_DEBUG(IVTV_DEBUG_DMA,
			   "DMA/VBI-Reinsertion type 0x%08x, size 0x%08x, offset 0x%08x\n",
			   streamtype, size, offset);
	}
	
	st = &itv->streams[streamtype];
	st->pts = pts_stamp;

	bytes_needed = size;

	down(&st->mlock);
        bytes_received =
                gather_free_buffers(itv, streamtype, &free_list, bytes_needed);

	if (bytes_received == 0) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "ENC: VBI got (%d) needed %d bytes for stream %d, "
			"only %d differ by (%d) bytes\n",
			bytes_received,
                        bytes_needed, streamtype,
                        (st->buf_total - st->buf_fill),
                        (bytes_needed - (st->buf_total - st->buf_fill)));

                set_bit(IVTV_F_S_OVERFLOW, &st->s_flags);

		up(&st->mlock);
		return;
	}

	/* increment the sequence # */
	sequence = ++st->seq;

	for (x = 0; bytes_read < bytes_received; x++) {
                if (list_empty(&free_list)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: Error, No more Buffers!!!.\n");
                        break;
                }

		/* extract the buffers we procured earlier */
		buf = list_entry(free_list.next, struct ivtv_buffer, list);
		list_del_init(&buf->list);

		buf->readpos = 0;
		buf->buffer.index = x;
		buf->buffer.sequence = sequence;
		buf->ts = jiffies;
		buf->b_flags = 0;

		if (size < buf->buffer.length) {
			buf->buffer.bytesused = size;
			size = 0;
		}
		else {
			buf->buffer.bytesused = buf->buffer.length;
			size -= buf->buffer.length;
		}

		bytes_read += buf->buffer.length;

		if (streamtype == IVTV_ENC_STREAM_TYPE_VBI) {
			ivtv_swap_copy((void *)buf->buffer.m.userptr, 
				(void *)(itv->enc_mem + offset), 
				buf->buffer.bytesused);
		} else {
			ivtv_swap_copy((void *)buf->buffer.m.userptr, 
				(void *)(itv->dec_mem + offset), 
				buf->buffer.bytesused);
		}
		ivtv_process_vbi_data(itv, buf, pts_stamp, streamtype);

		if (test_bit(IVTV_F_S_NO_READ, &st->s_flags) || st->id == -1) {
                        if (st->buf_total <= st->buf_min) {
                                ivtv_enq_buf_nolock(&st->free_q, buf);
                        } else {
                                ivtv_free_buffer(itv, buf, st);
				buf = NULL;
                        }
		}
		else {
			ivtv_enq_buf_nolock(&st->full_q, buf);
			ivtv_buf_fill_nolock(st, 
				buf->buffer.bytesused, BUF_ADD);
		}
	}
	up(&st->mlock);

	if (test_bit(IVTV_F_S_NO_READ, &st->s_flags))
	       	return;

	wake_up(&st->waitq);
}
 
/* only called from process context in decoder work queue*/
int  ivtv_dec_DMA_done(struct ivtv *itv, struct ivtv_stream *stream) 
{
	int freed = 0;
	struct ivtv_buffer *buf;
	u32 freed_bytes = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
        pci_dma_sync_single_for_cpu((struct pci_dev*)itv->dev, stream->SG_handle,
                sizeof(struct ivtv_SG_element) * (stream->buf_max/PAGE_SIZE),
                stream->dma);
#else
        pci_dma_sync_single(itv->dev, stream->SG_handle,
                sizeof(struct ivtv_SG_element) * (stream->buf_max/PAGE_SIZE),
                stream->dma);
#endif

        /* Free last DMA call */
	down(&stream->mlock);
        while ((buf = 
		ivtv_deq_buf_nolock(&stream->dma_q, stream->type)) != NULL)
	{	
	      freed_bytes += buf->readpos;
              buf->buffer.bytesused   = 0;
              buf->readpos = 0;
	      buf->ts = 0;	
              buf->b_flags = 0;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
	      /* Sync Buffer */
              pci_dma_sync_single_for_cpu((struct pci_dev*)itv->dev, buf->dma_handle,
               		buf->buffer.length, stream->dma);
#else
	      /* Sync Buffer */
              pci_dma_sync_single(itv->dev, buf->dma_handle,
               		buf->buffer.length, stream->dma);
#endif

              if (stream->buf_total <= stream->buf_min) {
                        ivtv_enq_buf_nolock(&stream->free_q,
                               buf);
              } else {
                        ivtv_free_buffer(itv, buf, stream);
			buf = NULL;
              }
	      freed++;
        }
	if (freed_bytes)
		ivtv_buf_fill_nolock(stream, freed_bytes, BUF_SUB);
	up(&stream->mlock);

	return freed;
}

