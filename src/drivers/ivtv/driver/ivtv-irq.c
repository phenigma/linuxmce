/* interrupt handling
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
#include "ivtv-firmware.h"
#include "ivtv-fileops.h"
#include "ivtv-queue.h"
#include "ivtv-irq.h"
#include "ivtv-ioctl.h"
#include "ivtv-mailbox.h"
#include "ivtv-vbi.h"
#include "ivtv-reset.h"

typedef unsigned long uintptr_t;

static void ivtv_FROM_DMA_done(struct ivtv *itv, int type);
static int dma_from_device(struct ivtv *itv, struct ivtv_stream *st);

IRQRETURN_T ivtv_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	struct ivtv *itv = (struct ivtv *)dev_id;
	u32 combo;
	u32 stat;

	spin_lock(&itv->DMA_slock);
	/* get contents of irq status register */
	stat = readl(itv->reg_mem + IVTV_REG_IRQSTATUS);

	combo = ~itv->irqmask & stat;

	if (0 == combo) {
		/* wasn't for us */
		spin_unlock(&itv->DMA_slock);
		return IRQ_NONE;
	}

	/* Clear out IRQ */
	writel(combo, (itv->reg_mem + IVTV_REG_IRQSTATUS));

	IVTV_DEBUG_IRQ(
		   "======= valid IRQ bits: 0x%08x ======\n", combo);

	if (combo & IVTV_IRQ_DEC_VBI_RE_INSERT2) {
		/* VBI User data reinsertion */
		IVTV_DEBUG_IRQ(
			"IRQ: IVTV_IRQ_DEC_VBI_RE_INSERT2 intr\n");

        	/* Send VBI to saa7127 */
        	if (test_and_clear_bit(IVTV_F_T_DEC_VBI_NEEDED, &itv->t_flags))
        	{
                	vbi_schedule_work(itv);
        	}
	}

	if (combo & IVTV_IRQ_DEC_DMA_23) {
		/* Mask */
		IVTV_DEBUG_WARN("IRQ: IVTV_IRQ_DEC_DMA_23 intr\n");
	}

	if (combo & IVTV_IRQ_DEC_DMA_15) {
		/* Mask */
		IVTV_DEBUG_WARN("IRQ: IVTV_IRQ_DEC_DMA_15 intr\n");
	}

	if (combo & (IVTV_IRQ_DMA_READ | IVTV_IRQ_DEC_DMA_COMPLETE)) {
		IVTV_DEBUG_IRQ("IRQ: IVTV_IRQ_DEC_DMA_COMPLETE\n");
		atomic_set(&itv->r_intr, 1);
		wake_up(&itv->r_intr_wq);
	}
	if (combo & IVTV_IRQ_ENC_DMA_COMPLETE) {
		IVTV_DEBUG_IRQ("IRQ: IVTV_IRQ_ENC_DMA_COMPLETE\n");
		atomic_set(&itv->w_intr, 1);
		wake_up(&itv->w_intr_wq);
	}
	if (combo & IVTV_IRQ_DMA_ERR) {
		IVTV_DEBUG_WARN(
			   "IRQ: IVTV_IRQ_DEC_DMA_ERR\n");
		writel(0x0, itv->reg_mem + IVTV_REG_DMAXFER);
	}

	/* Define irq bits */
#define IVTV_IRQ_ENC_DMA (IVTV_IRQ_ENC_START_CAP| \
			  IVTV_IRQ_ENC_VBI_CAP| \
			  IVTV_IRQ_DEC_VBI_RE_INSERT|\
			  IVTV_IRQ_ENC_EOS)

#define IVTV_IRQ_DEC_DMA (IVTV_IRQ_DEC_DATA_REQ)

	if (combo & IVTV_IRQ_ENC_DMA) {
		if (combo & IVTV_IRQ_ENC_START_CAP) {
			IVTV_DEBUG_IRQ("IRQ ENC DMA\n");

			/* set dma needed flag */
			set_bit(IVTV_F_T_ENC_DMA_NEEDED, &itv->t_flags);
			atomic_inc(&itv->enc_dma_stat_intr);
			enc_schedule_work(itv);
		}

		if (combo & IVTV_IRQ_ENC_VBI_CAP) {
			/* set vbi dma needed flag */
			IVTV_DEBUG_IRQ("IRQ ENC VBI DMA\n");

			set_bit(IVTV_F_T_ENC_VBI_DMA, &itv->t_flags);
			atomic_inc(&itv->enc_vbi_dma_stat_intr);
			enc_vbi_schedule_work(itv);
		}

		if (combo & IVTV_IRQ_DEC_VBI_RE_INSERT) {
			IVTV_DEBUG_IRQ("IRQ DEC VBI Reinsertion\n");

			set_bit(IVTV_F_T_DEC_VBI_INSERT, &itv->t_flags);
			atomic_inc(&itv->dec_dma_stat.vbi_intr);
			dec_vbi_schedule_work(itv);
		}

		if (combo & IVTV_IRQ_ENC_EOS) {
			IVTV_DEBUG_IRQ("Encoder End Of Stream\n");
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

		if (combo & IVTV_IRQ_DEC_DATA_REQ) {
			IVTV_DEBUG_IRQ("IRQ DEC Data Request\n");

			set_bit(IVTV_F_S_NEEDS_DATA,
				&itv->streams[type].s_flags);

			atomic_inc(&itv->dec_dma_stat.intr);
			IVTV_DEBUG_DEC(
		   		"* (%lu) DEC IRQ: intr %d wq_runs %d\n",
		   		jiffies,
		   		atomic_read(&itv->dec_dma_stat.intr),
		   		itv->dec_dma_stat.wq_runs);

			dec_schedule_work(itv);
		}
	}

	/* Decoder Vertical Sync */
	if (combo & IVTV_IRQ_DEC_VSYNC) {
		/* Vsyncs should happen once per field. They seem to happen more often so we read some of the registers
                 * that determine the line being displayed and determine whether it is a real vsync and from that we can 
                 * also determine if it is a frame sync 
                 */
		unsigned int thisVsyncFrame = 0;
                
                thisVsyncFrame =  readl((unsigned char *)itv->reg_mem +0x28c0) & 1;
                if ((thisVsyncFrame == 0 && itv->lastVsyncFrame == 1) ||
		     ((thisVsyncFrame != itv->lastVsyncFrame) && !itv->yuv_info.frame_interlaced))
                { 
                    int nextDmaFrame = atomic_read(&itv->yuv_info.next_dma_frame);
                    if (nextDmaFrame >= 0 && nextDmaFrame != atomic_read(&itv->yuv_info.next_fill_frame))
                    {
                        struct ivtv_stream *stream =	
                            &itv->streams[IVTV_DEC_STREAM_TYPE_YUV];
                        /* These must be writel. Using ivtv_write_reg causes the firmware to crash */
                        writel((yuv_offset[nextDmaFrame] >> 4), itv->reg_mem + 0x82c);
                        writel(((yuv_offset[nextDmaFrame] +
                                         IVTV_YUV_BUFFER_UV_OFFSET) >> 4),
                                       itv->reg_mem + 0x830);
                        writel((yuv_offset[nextDmaFrame] >> 4), itv->reg_mem + 0x834);
                        writel(((yuv_offset[nextDmaFrame] +
                                         IVTV_YUV_BUFFER_UV_OFFSET) >> 4),
                                       itv->reg_mem + 0x838);
                        nextDmaFrame = (nextDmaFrame + 1) & 0x3;
                        atomic_set(&itv->yuv_info.next_dma_frame, nextDmaFrame);
                        if (!test_bit(IVTV_F_S_DECODING_YUV, &stream->s_flags)) {
                            IVTV_DEBUG_WARN("Enable video output\n");
                            ivtv_write_reg(0x00108080, itv->reg_mem + 0x2898);
                            ivtv_write_reg(0x01, itv->reg_mem + 0x2800);
                            set_bit(IVTV_F_S_DECODING_YUV, &stream->s_flags);
                        }                        
                    }
                    atomic_inc(&itv->yuv_vsync);
                    wake_up(&itv->vsync_w_yuv);
                }
                if (thisVsyncFrame != itv->lastVsyncFrame)
                {
                    /* Framesync Vsync */
                    atomic_inc(&itv->fs_vsync);
                    wake_up_interruptible(&itv->vsync_w_fs);

                    /* VBI Vsync */
                    atomic_inc(&itv->vbi_vsync);
                    wake_up_interruptible(&itv->vsync_w_vbi);

                    /* OSD Vsync */	
                    atomic_inc(&itv->dec_dma_stat.vsync_flag_osd);
                    wake_up_interruptible(&itv->vsync_w_osd);
                    itv->lastVsyncFrame = thisVsyncFrame;	
                }

	}

	if (combo & IVTV_IRQ_ENC_VIM_RST) {
		IVTV_DEBUG_IRQ("VIM Restart\n");
		ivtv_vapi(itv, IVTV_API_REFRESH_INPUT, 0);
	}

	if (combo & IVTV_IRQ_DEC_COPY_PROTECT) {
		IVTV_DEBUG_IRQ("Copy Protection\n");
	}

	if (combo & IVTV_IRQ_DEC_AUD_MODE_CHG) {
		IVTV_DEBUG_IRQ("Audio mode change\n");
	}

	if (combo & IVTV_IRQ_DEC_IFRAME_DONE) {
		IVTV_DEBUG_IRQ("I-Frame done\n");
	}

	spin_unlock(&itv->DMA_slock);

	return IRQ_HANDLED;
}

static void ivtv_FROM_DMA_done(struct ivtv *itv, int stmtype)
{
	struct ivtv_stream *stream = NULL;
	struct ivtv_buffer *buf;
	int freed = 0;

	IVTV_DEBUG_DMA("ENC: DMA Done\n");

	stream = &itv->streams[stmtype];

	down(&stream->mlock);
	while ((buf = ivtv_deq_buf_nolock(itv, &stream->dma_q, stream->type))) {
		/* Sync Buffer */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
		pci_dma_sync_single_for_cpu((struct pci_dev *)itv->dev,
					    buf->dma_handle, buf->buffer.length,
					    stream->dma);
#else
		pci_dma_sync_single(itv->dev, buf->dma_handle,
				    buf->buffer.length, stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

		/* flag byteswap ABCD -> DCBA for MPG & VBI data outside irq */
		if (stmtype == IVTV_ENC_STREAM_TYPE_MPG)
			set_bit(IVTV_F_B_NEED_BUF_SWAP, &buf->b_flags);
		else if ((stmtype == IVTV_ENC_STREAM_TYPE_VBI ||
					stmtype == IVTV_DEC_STREAM_TYPE_VBI)) 
		{
			/* Parse and Groom VBI Data */
			ivtv_process_vbi_data(itv,
				buf, stream->pts, stream->type);
		}

                if (stream->id == -1) {
                        ivtv_enq_buf_nolock(&stream->free_q, buf);
                } else {
			ivtv_enq_buf_nolock(&stream->full_q, buf);
			ivtv_buf_fill_nolock(stream, 
				buf->buffer.bytesused, BUF_ADD);
		}
		freed++;
	}
	/* Add to buffer filled bytes */
	up(&stream->mlock);

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
}

void ivtv_sched_DMA(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	u32 type, size, offset, streamtype, pad = 0;
	u32 UVsize = 0, UVoffset = 0;
	u64 pts_stamp = 0;
	struct ivtv_stream *st;
	int x;
	int uvflag = 0;
	long sequence;
	u32 bytes_needed = 0, bytes_read = 0, bytes_received = 0;
	struct ivtv_buffer *buf;
	LIST_HEAD(free_list);
	int xfer_pad;
	unsigned long then;
	DECLARE_WAITQUEUE(wait, current);
	int rc = 0;
	int pio_mode = 0;
	/* Set these as you wish */
	int encode_mpeg_pio_mode = itv->dma_cfg.enc_pio;
	int encode_yuv_pio_mode = itv->dma_cfg.enc_pio;
	int encode_pcm_pio_mode = itv->dma_cfg.enc_pio;

	IVTV_DEBUG_DMA("ENC: Sched DMA\n");

        if (!test_bit(IVTV_F_T_ENC_VID_STARTED, &itv->t_flags) &&
		!test_bit(IVTV_F_T_ENC_RAWVID_STARTED, &itv->t_flags) &&
		!test_bit(IVTV_F_T_ENC_RAWAUD_STARTED, &itv->t_flags)) {
                IVTV_DEBUG_DMA("ENC: Capture not started\n");
                return;
        }

	/* Get DMA destination and size arguments from card */
	x = ivtv_api_getresult_nosleep(itv, &itv->enc_mbox[9], &result, &data[0]);
	if (x) {
		IVTV_DEBUG_WARN("error:%d getting DMA info\n", x);
		return;
	}
	type = data[0];

	if (data[1] == 0 || data[2] == 0)
		return;

	switch (type) {
	case 0:		/* MPEG */
		offset = data[1];
		size = data[2];
		IVTV_DEBUG_INFO(
			   "DMA/MPG type 0x%08x,size 0x%08x,offset 0x%08x\n",
			   type, size, offset);
		if (encode_mpeg_pio_mode)
			pio_mode = 1;
		streamtype = IVTV_ENC_STREAM_TYPE_MPG;
		break;

	case 1:		/* YUV */
		offset = data[1];
		size = data[2];
		UVoffset = data[3];
		UVsize = data[4];
		pts_stamp = ((u64) data[5] << 32) | data[6];

		IVTV_DEBUG_INFO(
			   "DMA/YUV type 0x%08x,Ysize 0x%08x,Yoffset 0x%08x,"
			   "UVsize 0x%08x,UVoffset 0x%08x,PTS 0x%09llx\n",
			   type, size, offset, UVsize, UVoffset, pts_stamp);

		if (encode_yuv_pio_mode)
			pio_mode = 1;

		streamtype = IVTV_ENC_STREAM_TYPE_YUV;
		break;

	case 2:		/* PCM (audio) */
		offset = data[1] + 12;
		size = data[2] - 12;
		pts_stamp = *(u32 *) (itv->dec_mem + offset - 8);
		pts_stamp |=
		    (u64) (*(u32 *) (itv->dec_mem + offset - 12)) << 32;
		offset += IVTV_DECODER_OFFSET;
		IVTV_DEBUG_INFO(
			   "DMA/PCM type 0x%08x,size 0x%08x,offset 0x%08x "
			   "PTS 0x%09llx\n", type, size, offset, pts_stamp);
		streamtype = IVTV_ENC_STREAM_TYPE_PCM;

		if (encode_pcm_pio_mode)
			pio_mode = 1;

		break;

	default:
		IVTV_DEBUG_WARN(
			   "DMA/UNKNOWN type 0x%08x, NOT SUPPORTED\n", type);
		return;
	}
	/* FIXME should check for null on the stream element */
	if (streamtype >= itv->streamcount) {
		IVTV_DEBUG_WARN(
			   "No stream handler for type %d\n", streamtype);
		return;
	}

	st = &itv->streams[streamtype];
	st->pts = pts_stamp;
	st->dmatype = type;

	if (st->id == -1) {
		return;
	}
	bytes_needed = size + UVsize;
	if (type == 1)
		bytes_needed +=
		    (UVsize % itv->dma_cfg.enc_yuv_buf_size) +
		    (size % itv->dma_cfg.enc_yuv_buf_size);

	add_wait_queue(&st->waitq, &wait);
	do {
		set_current_state(TASK_INTERRUPTIBLE);

		if (test_bit(FW_RESET_SHUTDOWN, &itv->r_flags)) {
			rc = -EINTR;
			break;
		}

		/* Lock Encoder */
		if ((readl((unsigned char *)
				   itv->reg_mem + IVTV_REG_DMASTATUS) & 0x02) &&
		    !(readl((unsigned char *)
				    itv->reg_mem + IVTV_REG_DMASTATUS) & 0x18)
		    && !test_and_set_bit(IVTV_F_S_DMAP, &st->s_flags)) {
			break;
		}

		if (ivtv_sleep_timeout(HZ / 100, 1)) {
			IVTV_DEBUG_WARN(
				   "ENC: User stopped waitq.\n");

			rc = -EINTR;
			break;
		}

                /* Check capture started bit */
        	if (!test_bit(IVTV_F_T_ENC_VID_STARTED, &itv->t_flags) &&
			!test_bit(IVTV_F_T_ENC_RAWVID_STARTED, &itv->t_flags) &&
			!test_bit(IVTV_F_T_ENC_RAWAUD_STARTED, &itv->t_flags)) 
		{
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
        	/* Check capture started bit */
        	if (!test_bit(IVTV_F_T_ENC_VID_STARTED, &itv->t_flags) &&
			!test_bit(IVTV_F_T_ENC_RAWVID_STARTED, &itv->t_flags) &&
			!test_bit(IVTV_F_T_ENC_RAWAUD_STARTED, &itv->t_flags)) 
		{
			clear_bit(IVTV_F_S_DMAP, &st->s_flags);
			return;
		}

		wake_up(&st->waitq);	/* wake up reading */

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > HZ) {
			IVTV_DEBUG_INFO(
				   "ENC: Only 0x%08x bytes available need %d.\n",
				   ivtv_buf_fill_lock(st, 0,
						      BUF_EMPTY), bytes_needed);
			break;
		}
	}

	down(&st->mlock);
	bytes_received =
		enc_gather_free_buffers(itv, streamtype, &free_list, bytes_needed);
	up(&st->mlock);

	if (bytes_received == 0) {
		set_bit(IVTV_F_S_OVERFLOW, &st->s_flags);
		clear_bit(IVTV_F_S_DMAP, &st->s_flags);
		wake_up(&st->waitq);
		return;
	}

	/* increment the sequence # */
	sequence = ++st->seq;

	st->ubytes = size;

	for (x = 0; bytes_read < bytes_needed; x++) {
		/* extract the buffers we procured earlier */
		buf = list_entry(free_list.next, struct ivtv_buffer, list);
		list_del_init(&buf->list);

		buf->readpos = 0;
		buf->b_flags = 0;
		buf->buffer.index = x;
		buf->buffer.sequence = sequence;
		buf->ts = jiffies;

		bytes_read += buf->buffer.length;

		if (size < buf->buffer.length) {
			xfer_pad = buf->buffer.length /*256 */ ;
			pad = size;
			buf->buffer.bytesused = size;
			if (size > xfer_pad && size % xfer_pad)	/* Align */
				size = size + (size % xfer_pad);
			if (size < xfer_pad)	/* Too small */
				size = xfer_pad;
			st->SGarray[x].size = size;
			size = 0;

		} else {
			pad = 0;
			buf->buffer.bytesused = buf->buffer.length;
			st->SGarray[x].size = buf->buffer.length;
			size -= st->SGarray[x].size;
		}
		st->SGarray[x].src = offset;	/* Encoder Addr */
		st->SGarray[x].dst = buf->dma_handle;	/* Encoder Addr */

		/* PIO Mode */
		if (pio_mode) {
			memcpy_fromio((void *)buf->buffer.m.userptr,
				      (void *)(itv->enc_mem + offset),
				      st->SGarray[x].size);
		}
		offset += st->SGarray[x].size;	/* Increment Enc Addr */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
		/* Sync SG buffers */
		pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
					       buf->dma_handle,
					       buf->buffer.length, st->dma);
#else
		/* Sync SG buffers */
		pci_dma_sync_single(itv->dev, buf->dma_handle,
				    buf->buffer.length, st->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

		ivtv_enq_buf_nolock(&st->dma_q, buf);

		if ((size == 0) && (type == 1) && (uvflag == 0)) {	/* YUV */
			/* process the UV section */
			offset = UVoffset;
			size = UVsize;
			uvflag = 1;
		}
	}
	st->SG_length = x;

	/* This should wrap gracefully */
	st->trans_id++;

	/* Mark last buffer size for Interrupt flag */
	st->SGarray[st->SG_length - 1].size |= 0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
	/* Sync Hardware SG List of buffers */
	pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
				       st->SG_handle,
				       sizeof(struct ivtv_SG_element) *
				       (st->buf_min / st->bufsize), st->dma);
#else
	/* Sync Hardware SG List of buffers */
	pci_dma_sync_single(itv->dev, st->SG_handle,
			    sizeof(struct ivtv_SG_element) * (st->buf_min /
							      st->bufsize),
			    st->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

	IVTV_DEBUG_DMA(
		   "Sched dma: addr: 0x%08llx, SG_length: %d, array_size 0x%08x,"
		   " type 0x%08x\n", (u64) st->SG_handle,
		   st->SG_length, bytes_read, type);

	if (pio_mode) {
		ivtv_vapi(itv, IVTV_API_SCHED_DMA_TO_HOST, 3, 0, 0, st->type);

		/* Gather Buffers from DMA xfer to read */
		ivtv_FROM_DMA_done(itv, st->type);
	} else {
		//down(&itv->DMA_lock);
		if (dma_from_device(itv, st) != 0) {
			IVTV_DEBUG_WARN("Error Encoder DMA\n");
		}
		//up(&itv->DMA_lock);

		/* Gather Buffers from DMA xfer to read */
		ivtv_FROM_DMA_done(itv, st->type);
	}

	clear_bit(IVTV_F_S_DMAP, &st->s_flags);

	wake_up(&st->waitq);
}

static int dma_from_device(struct ivtv *itv, struct ivtv_stream *st)
{
	unsigned long then;
	unsigned long flags;
	int redo_dma = 0;
	int ret = 0;
	int sg_offset;

	/* wait for DMA complete status */
	then = jiffies;
	while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x03)) {
		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "ENC: REG_DMASTATUS wait failed\n");
			break;
		}
	}

	/* wait for DMA to finish */
	then = jiffies;
	while ((readl(itv->reg_mem + IVTV_REG_DMAXFER) & 0x03)) {
		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "ENC: REG_DMAXFER wait failed\n");
			break;
		}
	}

      redo_dma:

	spin_lock_irqsave(&itv->DMA_slock, flags);
	/* put SG Handle into register 0x0c */
	ivtv_write_reg(st->SG_handle, itv->reg_mem + IVTV_REG_ENCDMAADDR);
	if (readl(itv->reg_mem + IVTV_REG_ENCDMAADDR) != st->SG_handle)
		ivtv_write_reg(st->SG_handle,
			       itv->reg_mem + IVTV_REG_ENCDMAADDR);

	/* Send DMA with register 0x00, using the enc DMA bit */
	if (readl(itv->reg_mem + IVTV_REG_ENCDMAADDR) == st->SG_handle)
		ivtv_write_reg(readl(itv->reg_mem + IVTV_REG_DMAXFER) |
			       0x02, itv->reg_mem + IVTV_REG_DMAXFER);
	spin_unlock_irqrestore(&itv->DMA_slock, flags);

	/* wait for DMA to start */
	then = jiffies;
	while ((readl(itv->reg_mem + IVTV_REG_DMAXFER) & 0x02)) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x1A) {
			break;
		}

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "ENC: REG_DMAXFER 2 wait failed\n");
			break;
		}
	}

	/* wait for DMA to be finished, check interrupt bit */
	if (st->SG_length <= 8)
		sg_offset = ((12 * st->SG_length) - 4);
	else {
		sg_offset = ((12 * (st->SG_length % 8)) - 4);
	}
	then = jiffies;
	while (!(readl(itv->reg_mem +
			       IVTV_REG_ENCSG1SRC + sg_offset) & 0x80000000)) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x1A)
			break;

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "DEC: REG_ENCSG1LEN wait failed\n");
			break;
		}
	}

	/* Wait for Write Interrupt */
	while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x1A) &&
	       wait_event_interruptible(itv->w_intr_wq,
					atomic_read(&itv->w_intr))) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x1A)
			break;

		if (atomic_read(&itv->w_intr))
			break;
	}

	atomic_set(&itv->w_intr, 0);

	/* wait for DMA complete status */
	then = jiffies;
	while (!(readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x02)) {
		/* DMA Error */
		if (readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x1A) {
			break;
		}

		ivtv_sleep_timeout(HZ / 100, 1);
		if ((jiffies - then) > (HZ * 3)) {
			IVTV_DEBUG_WARN(
				   "ENC: REG_DMASTATUS2 wait failed\n");
			break;
		}
	}

	/* DMA Error */
	if ((readl(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x18)) {
		IVTV_DEBUG_WARN(
			   "ENC: (%d) DMA Error 0x%08x\n",
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
			   "ENC: REDO DMA took too many tries.\n");
		ret = -ENOMEM;
	}

	return ret;
}

void ivtv_sched_VBI(struct ivtv *itv, int streamtype)
{
	u32 size, offset;
	u64 pts_stamp;
	struct ivtv_stream *st;
	int x;
	struct ivtv_buffer *buf = 0;
	LIST_HEAD(free_list);
	long sequence;
	u32 bytes_needed = 0, bytes_read = 0, bytes_received = 0;
	int pio_mode = itv->dma_cfg.vbi_pio;
	DECLARE_WAITQUEUE(wait, current);
	int rc = 0;
	unsigned long then;
	int xfer_pad;
	u32 pad;

	IVTV_DEBUG_DMA("VBI: Sched DMA\n");

	st = &itv->streams[streamtype];

	if (streamtype == IVTV_ENC_STREAM_TYPE_VBI) {
		u32 bufptr = 
			readl(itv->enc_mem + (itv->vbi_enc_start-4))+12;	
		if (!test_bit(IVTV_F_T_ENC_VBI_STARTED, &itv->t_flags)) {
			IVTV_DEBUG_INFO(
				"VBI: Capture not started\n");
			return;
		}

                if (itv->vbi_insert_mpeg && itv->vbi_sliced_in->service_set)
                        pio_mode = 1;

		itv->vbi_frame += itv->vbi_fpi;
		size = itv->vbi_enc_size * itv->vbi_fpi;
		offset = bufptr;

		IVTV_DEBUG_DMA(
			"VBI(%d) BufPTR: 0x%08x offset 0x%08x size 0x%08x\n",
			size, bufptr, offset, size);

               	pts_stamp = *(u32 *) (itv->enc_mem + offset - 4);
              	pts_stamp |= 
			(u64) (*(u32 *) (itv->enc_mem + offset - 8)) 
			<< 32;

		IVTV_DEBUG_DMA(
			   "DMA/VBI type 0x%08x, size 0x%08x, offset 0x%08x,"
			   "PTS 0x%09llx\n",
			   streamtype, size, offset, pts_stamp);
	} else {
		/* Decoder not running */
		if (!test_bit(IVTV_F_T_DEC_STARTED, &itv->t_flags))
			return;
		pio_mode = 1; /* Seems DMA mode has errors */
		offset = readl(itv->dec_mem + itv->vbi_dec_start);
		offset += itv->vbi_dec_start;
		size = readl(itv->dec_mem + itv->vbi_dec_start + 4);
		size += 8;
		pts_stamp = 0;
		IVTV_DEBUG_DMA(
			   "DMA/VBI-Reinsertion type 0x%08x, size 0x%08x, offset 0x%08x\n",
			   streamtype, size, offset);
	}

	st->pts = pts_stamp;

	if (pio_mode)
		goto skip_vbi_wait;
	else {
		if (streamtype != IVTV_ENC_STREAM_TYPE_VBI)
			offset += IVTV_DECODER_OFFSET;
	}

        add_wait_queue(&st->waitq, &wait);
        do {
                set_current_state(TASK_INTERRUPTIBLE);

                if (test_bit(FW_RESET_SHUTDOWN, &itv->r_flags)) {
                        rc = -EINTR;
                        break;
                }

                /* Lock Encoder */
       		if ((readl((unsigned char *)
                           itv->reg_mem + IVTV_REG_DMASTATUS) & 0x02) &&
              			!(readl((unsigned char *)
                           itv->reg_mem + IVTV_REG_DMASTATUS) & 0x18)
              		&& !test_and_set_bit(IVTV_F_S_DMAP, &st->s_flags)) 
		{
               		break;
       		}

                if (ivtv_sleep_timeout(HZ / 100, 1)) {
                        IVTV_DEBUG_WARN(
                                   "ENC: User stopped waitq.\n");

                        rc = -EINTR;
                        break;
                }

                /* Check capture started bit */
                if (!test_bit(IVTV_F_T_ENC_VBI_STARTED, &itv->t_flags)) {
			rc = -EINTR;
                        break;
		}
        } while (!rc);
        set_current_state(TASK_RUNNING);
        remove_wait_queue(&st->waitq, &wait);

        if (rc)
                return;

skip_vbi_wait:
	bytes_needed = size;

        /* Wait till buffers are available */
       	then = jiffies;
       	while (ivtv_buf_fill_lock(st, 0, BUF_EMPTY) < bytes_needed) {
                /* Check vbi capture started bit */
		if (streamtype == IVTV_ENC_STREAM_TYPE_VBI) {
                	if (!test_bit(IVTV_F_T_ENC_VBI_STARTED, &itv->t_flags)) {
				clear_bit(IVTV_F_S_DMAP, &st->s_flags);
                        	return;
			}
		}
               	wake_up(&st->waitq);    /* wake up reading */

               	ivtv_sleep_timeout(HZ / 100, 1);
               	if ((jiffies - then) > HZ) {
                       	IVTV_DEBUG_INFO(
                                  "VBI: Only 0x%08x bytes available need %d.\n",
                                  ivtv_buf_fill_lock(st, 0,
                                                     BUF_EMPTY), bytes_needed);
                       	break;
               	}
       	}

	down(&st->mlock);
	bytes_received =
   	 	enc_gather_free_buffers(itv, streamtype, &free_list, bytes_needed);
	up(&st->mlock);

	if (bytes_received == 0) {
		set_bit(IVTV_F_S_OVERFLOW, &st->s_flags);
		clear_bit(IVTV_F_S_DMAP, &st->s_flags);
		wake_up(&st->waitq);
		return;
	}

	/* increment the sequence # */
	sequence = ++st->seq;

	st->ubytes = bytes_received;

	for (x = 0; bytes_read < bytes_received; x++) {
		/* extract the buffers we procured earlier */
		buf = list_entry(free_list.next, struct ivtv_buffer, list);
		list_del_init(&buf->list);
		buf->readpos = 0;
		buf->b_flags = 0;
		buf->buffer.index = x;
		buf->buffer.sequence = sequence;
		buf->ts = jiffies;

		bytes_read += buf->buffer.length;

		if (size < buf->buffer.length) {
			xfer_pad = buf->buffer.length /*256 */ ;
			pad = size;
			buf->buffer.bytesused = size;
			if (size > xfer_pad && size % xfer_pad)	/* Align */
				size = size + (size % xfer_pad);
			if (size < xfer_pad)	/* Too small */
				size = xfer_pad;
			st->SGarray[x].size = size;
			size = 0;

		} else {
			pad = 0;
			buf->buffer.bytesused = buf->buffer.length;
			st->SGarray[x].size = buf->buffer.length;
			size -= st->SGarray[x].size;
		}

		st->SGarray[x].src = offset;	/* Encoder Addr */
		st->SGarray[x].dst = buf->dma_handle;/* Encoder Addr */

		/* PIO Mode */
		if (pio_mode) {
			if (streamtype == IVTV_ENC_STREAM_TYPE_VBI) {
				memcpy_toio((void *)buf->buffer.m.userptr,
				       (void *)(itv->enc_mem + offset),
				       buf->buffer.bytesused);
			} else {
				memcpy_toio((void *)buf->buffer.m.userptr,
			       		(void *)(itv->dec_mem + offset),
			       		buf->buffer.bytesused);
			}
		}
		offset += st->SGarray[x].size;	/* Increment Enc Addr */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
		/* Sync SG buffers */
		pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
					       buf->dma_handle,
					       buf->buffer.length, st->dma);
#else
		/* Sync SG buffers */
		pci_dma_sync_single(itv->dev, buf->dma_handle,
				    buf->buffer.length, st->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

		ivtv_enq_buf_nolock(&st->dma_q, buf);
	}
	st->SG_length = x;

	/* This should wrap gracefully */
	st->trans_id++;

	/* Mark last buffer size for Interrupt flag */
	st->SGarray[st->SG_length - 1].size |= 0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
	/* Sync Hardware SG List of buffers */
	pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
				       st->SG_handle,
				       sizeof(struct ivtv_SG_element) *
				       (st->buf_min / st->bufsize), st->dma);
#else
	/* Sync Hardware SG List of buffers */
	pci_dma_sync_single(itv->dev, st->SG_handle,
			    sizeof(struct ivtv_SG_element) * (st->buf_min /
							      st->bufsize),
			    st->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

	IVTV_DEBUG_DMA(
		   "Sched dma: addr: 0x%08llx, SG_length: %d, array_size 0x%08x,"
		   " type 0x%08x\n", (u64) st->SG_handle,
		   st->SG_length, bytes_read, st->type);

	if (pio_mode) {
		/* Gather Buffers from DMA xfer to read */
		ivtv_FROM_DMA_done(itv, st->type);
	} else {
		//down(&itv->DMA_lock);
		if (dma_from_device(itv, st) != 0) {
			IVTV_DEBUG_WARN("Error VBI DMA\n");
		}
		//up(&itv->DMA_lock);

		ivtv_FROM_DMA_done(itv, st->type);
	}

	clear_bit(IVTV_F_S_DMAP, &st->s_flags);

        if (st->id == -1)
                return;

	wake_up(&st->waitq);
}
