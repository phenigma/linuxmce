/*
    file operation functions
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

/* Main Driver file for the ivtv project:
 * Driver for the iTVC15 chip.
 * Author: Kevin Thayer (nufan_wfk at yahoo.com)
 * License: GPL
 * http://www.sourceforge.net/projects/ivtv/
 * 
 * -----
 * MPG600/MPG160 support by  T.Adachi <tadachi@tadachi-net.com>
 *                      and Takeru KOMORIYA<komoriya@paken.org>
 *
 * AVerMedia M179 GPIO info by Chris Pinkham <cpinkham@bc2va.org>
 *                using information provided by Jiun-Kuei Jung @ AVerMedia.
 */

#include "ivtv-driver.h"
#include "ivtv-dma.h"
#include "ivtv-fileops.h"
#include "ivtv-i2c.h"
#include "ivtv-queue.h"
#include "ivtv-irq.h"
#include "ivtv-reset.h"
#include "ivtv-vbi.h"
#include "ivtv-mailbox.h"
#include "ivtv-kthreads.h"
#include "ivtv-audio.h"
#include "ivtv-video.h"
#include "ivtv-streams.h"
#include "ivtv-cards.h"
#include "audiochip.h"
#include "cx25840.h"
#include "ivtv-ioctl.h"

typedef unsigned long uintptr_t;

static int allocate_stream(struct ivtv *itv, int type)
{
	if (!itv->options.dynbuf) {
                // statically allocated
                return 0;
        }
	// allocate stream
	return ivtv_stream_alloc(itv, type);
}

/* This function tries to claim the stream for a specific file descriptor.
   If no one else is using this stream then the stream is claimed, any
   memory is allocated and associated VBI streams are also automatically
   claimed. Possible error returns: -EBUSY if someone else has claimed
   the stream, -ENOMEM when out of memory or 0 on success. */
int ivtv_claim_stream(struct ivtv_open_id *id, int type)
{
	struct ivtv *itv = id->itv;
	struct ivtv_stream *stream = &itv->streams[type];
	struct ivtv_stream *vbi_stream;
        int vbi_type;

	if (test_and_set_bit(IVTV_F_S_IN_USE, &stream->s_flags)) {
                // someone already claimed this stream
                if (stream->id == id->open_id) {
                        // yes, this file descriptor did. So that's OK.
                        return 0;
                }
                if (stream->id == -1 && (type == IVTV_DEC_STREAM_TYPE_VBI ||
                                         type == IVTV_ENC_STREAM_TYPE_VBI)) {
                        /* VBI is handled already internally, now also assign
                           the file descriptor to this stream for external
                           reading of the stream. */
                        stream->id = id->open_id;
                        IVTV_DEBUG_INFO("Start Read VBI\n");
                        return 0;
                }
                /* someone else is using this stream already */
                IVTV_DEBUG_INFO("Stream %d is busy\n", type);
                return -EBUSY;
        }
        if (allocate_stream(itv, type)) {
                clear_bit(IVTV_F_S_IN_USE, &stream->s_flags);
                return -ENOMEM;
        }
	stream->id = id->open_id;
	if (type == IVTV_DEC_STREAM_TYPE_VBI) {
                // Enable reinsertion interrupt
                ivtv_clear_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);
        }

	/* IVTV_DEC_STREAM_TYPE_MPG needs to claim IVTV_DEC_STREAM_TYPE_VBI,
	   IVTV_ENC_STREAM_TYPE_MPG needs to claim IVTV_ENC_STREAM_TYPE_VBI
           (provided VBI insertion is on and sliced VBI is selected), for all
           other streams we're done */	 
	if (type == IVTV_DEC_STREAM_TYPE_MPG) {
                vbi_type = IVTV_DEC_STREAM_TYPE_VBI;
        } else if (type == IVTV_ENC_STREAM_TYPE_MPG &&
                   itv->vbi_insert_mpeg && !itv->vbi_in.raw) {
                vbi_type = IVTV_ENC_STREAM_TYPE_VBI;
        } else {
                return 0;
        }
	vbi_stream = &itv->streams[vbi_type];

        if (!test_and_set_bit(IVTV_F_S_IN_USE, &vbi_stream->s_flags)) {
                if (allocate_stream(itv, vbi_type)) {
                        // could not allocate VBI stream, so free the lot
                        clear_bit(IVTV_F_S_IN_USE, &vbi_stream->s_flags);
                        ivtv_stream_free(itv, type);
                        clear_bit(IVTV_F_S_IN_USE, &stream->s_flags);
                        return -ENOMEM;
                }
                // Enable reinsertion interrupt
                if (vbi_type == IVTV_DEC_STREAM_TYPE_VBI)
                        ivtv_clear_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);
        }
        // mark that it is used internally
        set_bit(IVTV_F_S_INTERNAL_USE, &vbi_stream->s_flags);
	return 0;
}

/* This function releases a previously claimed stream. It will take into
   account associated VBI streams. */
void ivtv_release_stream(struct ivtv *itv, int type)
{
	struct ivtv_stream *stream = &itv->streams[type];
        struct ivtv_stream *vbi_stream;
        int vbi_type;

	stream->id = -1;
        if ((type == IVTV_DEC_STREAM_TYPE_VBI ||
             type == IVTV_ENC_STREAM_TYPE_VBI) &&
            test_bit(IVTV_F_S_INTERNAL_USE, &stream->s_flags)) {
                // this stream is still in use internally
                return;
        }
	if (!test_and_clear_bit(IVTV_F_S_IN_USE, &stream->s_flags)) {
		IVTV_DEBUG_WARN("Release stream %d not in use!\n", type);
                return;
        }

        ivtv_flush_queues_lock(itv, type);
        if (itv->options.dynbuf) {
                ivtv_stream_free(itv, type);
        }

	/* IVTV_DEC_STREAM_TYPE_MPG needs to release IVTV_DEC_STREAM_TYPE_VBI,
	   IVTV_ENC_STREAM_TYPE_MPG needs to release IVTV_ENC_STREAM_TYPE_VBI,
           for all other streams we're done */	 
        if (type == IVTV_DEC_STREAM_TYPE_MPG) {
                vbi_type = IVTV_DEC_STREAM_TYPE_VBI;
        } else if (type == IVTV_ENC_STREAM_TYPE_MPG) {
                vbi_type = IVTV_ENC_STREAM_TYPE_VBI;
        } else {
                return;
        }
        vbi_stream = &itv->streams[vbi_type];

        // clear internal use flag
        if (!test_and_clear_bit(IVTV_F_S_INTERNAL_USE, &vbi_stream->s_flags)) {
                // was already cleared
                return;
        }
        if (vbi_stream->id != -1) {
                // VBI stream still claimed by a file descriptor
                return;
        }
        // disable reinsertion interrupt
        if (vbi_type == IVTV_DEC_STREAM_TYPE_VBI)
                ivtv_set_irq_mask(itv, IVTV_IRQ_DEC_VBI_RE_INSERT);
        clear_bit(IVTV_F_S_IN_USE, &vbi_stream->s_flags);
        ivtv_flush_queues_lock(itv, vbi_type);

        if (itv->options.dynbuf)
                ivtv_stream_free(itv, vbi_type);
}

static int ivtv_check_digitizer_sync(struct ivtv *itv)
{
	int sig = 0, ret = 0;

	IVTV_DEBUG_INFO("Checking digitizer\n");

	ret = itv->card->video_dec_func(itv, DECODER_GET_STATUS, &sig);

	if (ret) {
		IVTV_ERR("DECODER_GET_STATUS call failed. Is decoder loaded?\n");
                return 0;
	}
	if (sig & DECODER_STATUS_GOOD) {
		IVTV_DEBUG_INFO("Digitizer sync GOOD\n");
                return 0;
        }
	IVTV_DEBUG_INFO("Digitizer sync BAD\n");
	return -EIO;
}

static int ivtv_reset_digitizer(struct ivtv *itv)
{
	int x = 0, ret = 0;
	unsigned int reset = 1;

	if (itv->card->type == IVTV_CARD_PVR_150 ||
                    itv->card->type == IVTV_CARD_PG600) {
		ivtv_cx25840(itv, DECODER_GET_VIDEO_STATUS, &reset);
		if (!reset)	
			IVTV_DEBUG_WARN("Digitizer video is bad\n");
	}

	/* If video is bad or not new cx25840 digitizer */
	if (reset) {
		/* x is just a placeholder. It's unused */
		/* this just resets the scaler.. 
			shouldn't need to re-do any settings */
		ret = itv->card->video_dec_func(itv, DECODER_RESET, &x);
	}

	/* Re-Init Digitizer */
	ivtv_set_irq_mask(itv, IVTV_IRQ_MASK_CAPTURE);

        /* Initialize Digitizer for Capture */
        ivtv_init_digitizer(itv);

        /*IVTV_DEBUG_INFO("Sleeping for 100ms\n"); */
        ivtv_sleep_timeout(HZ / 10, 0);

	ivtv_clear_irq_mask(itv, IVTV_IRQ_MASK_CAPTURE);

	return ret;
}

static int ivtv_YUV_fixup(struct ivtv *itv, struct ivtv_stream *st, int count,
			  char *ubuf, struct ivtv_buffer *buf)
{
/*  
 * int count = # of bytes to transfer to client 
 * st->ubytes = # of bytes written this frame
 * ubuf = buffer to write to (user's buffer)
 * buf = read buffer
 *
 */
	int src_width = 720;	/* all known formats have src width of 720 */
	int Hoff, Voff;		/* collectors for offsets to read position */
	int width, height;	/* resolution of the capture stream */
	int curline;		/* vertical line currently being processed */
	int maxline;		/* height of combined frame */
	int cur_m_block;	/* current horizontal offset of working mblock in this row */
	int maxblock;		/* # of macroblocks in a row */
	int Hbytes;		/* # of bytes to write to user this time around */
	int retval = 0;		/* accumulator for total bytes written */
	int start;		/* position in buf to read from */
	int buf_start;		/* byte offset of first byte in this *buf */

	height = itv->height;
	width = itv->width;
	maxblock = (width + 0xf) >> 4;
	maxline = (int)height + (height/2);	/* 1 for Y, .5 for UV */
	/* Offset is always bufsize * buffer index 
	   buf_start = (st->ubytes - buf->readpos);  tested/works */

	buf_start = itv->dma_cfg.enc_yuv_buf_size * buf->buffer.index;

	/* FIXME it may not be possible to get YUV width > 720 */
	// if (width > src_width) src_width=width;

	curline = (int)(st->ubytes / width);

	while (curline < maxline) {
//      printk(" cl: %d, ml: %d\n", curline, maxline);
		Voff = 16 * (curline & 0xf) +	/* Voffset within MBlock */
		    ((curline & 0xfff0) * src_width);	/* Voffset of Mblock */

		cur_m_block = (st->ubytes - (curline * width)) >> 4;

/*	printk("voff %d, macroVoff %d, Voff %d, cmb %d\n", (16 * (curline & 0xf)), 
			((curline & 0xff0) * src_width), Voff, cur_m_block);
*/

		while ((cur_m_block < maxblock) && (count > 0)) {
			Hoff = (cur_m_block * 256) +	/* mblock offset within line */
			    /* Hoffset within mblock, usually 0 */
			    ((st->ubytes - (curline * width)) & 0xf);
			Hbytes = 16 - ((st->ubytes - (curline * width)) & 0xf);

			if (Hbytes > count)
				Hbytes = count;

			start = Hoff + Voff;

			if (copy_to_user((char *)((uintptr_t) ubuf + retval),
					 (u32 *) ((uintptr_t) buf->buffer.m.
						  userptr + (start -
							     buf_start)),
					 Hbytes)) {
				IVTV_DEBUG_WARN(
					   "copy to user failed\n");
				return -EFAULT;
			}

			count -= Hbytes;
			retval += Hbytes;
			st->ubytes += Hbytes;

			cur_m_block++;
		}

		/* if user can't handle anymore data or buffer empty */
		curline++;
		if ((count == 0))	/*|| ((curline * src_width) % itv->dma_cfg.enc_yuv_buf_size) == 0) */
			return retval;
	}

	/* FIXME i don't think you should ever get here */
	IVTV_DEBUG_WARN(
		   "You've just sailed off the edge of this function\n");
	return retval;
}

static int gather_free_buffers(struct ivtv *itv, int streamtype,
			       struct list_head *free_list, int bytes_needed,
			       int enc)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	struct ivtv_buffer *buf;
	int bytes_received = 0;
	int x;

	/* gather the needed buffers first, so we don't have to bail
	 * in mid-air. put them on a list on the stack */
	for (x = 0; (bytes_received < bytes_needed); x++) {
		if (enc) {
			buf = ivtv_deq_buf_nolock(itv, &st->io_q, streamtype);
			if (!buf)
				buf = ivtv_deq_buf_nolock(itv, &st->full_q,
							  streamtype);
			if (!buf)
				break;
			bytes_received += buf->buffer.bytesused;
		} else {
			int free_q = 0;

			buf = ivtv_deq_buf_nolock(itv, &st->io_q, streamtype);
			if (!buf) {
				buf = ivtv_deq_buf_nolock(itv, &st->free_q,
							  streamtype);
				free_q = 1;
			} else
				buf->buffer.index = x;
			if (free_q && buf) {
				buf->buffer.bytesused = 0;
				buf->readpos = 0;
				buf->ts = 0;
				buf->buffer.index = x;
				buf->b_flags = 0;

				IVTV_DEBUG_DMA(
					   "%d: Got FREE Buff with %d bytes.\n",
					   enc, buf->buffer.bytesused);
			} else if (!buf) {
				/* Too many allocated? */
				if (st->buf_total >= st->buf_max)
					break;

				buf = ivtv_init_buffer(itv, st);
				buf->buffer.index = x;
				if (buf) {
					IVTV_DEBUG_DMA(
						   "%d: Alloc Buffer. (%d)\n",
						   enc,
						   atomic_read(&st->
							       allocated_buffers));
				}
			}
			if (!buf)
				break;
			bytes_received += buf->buffer.length;
		}

		list_add_tail(&buf->list, free_list);
	}

	IVTV_DEBUG_DMA(
		   "Gather %d: x=%d bytes_needed 0x%08x buf_fill 0x%08x\n",
		   enc, x, bytes_needed, st->buf_fill);

	return bytes_received;
}

static int ivtv_read(struct file *filp, char *ubuf, size_t count, loff_t * pos)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv *itv = id->itv;
	int x = 0, datalen, retval = 0, freed = 0;
	size_t newcount, bufsize = 0;
	int type = id->type;
	struct ivtv_buffer *buf = NULL;
	struct ivtv_stream *st = &itv->streams[type];
	DECLARE_WAITQUEUE(wait, current);
	int idx = itv->vbi_inserted_frame % IVTV_VBI_FRAMES;
	int blocking = !(filp->f_flags & O_NONBLOCK);
	u32 old_trans_id;
	unsigned long then;
	u32 bytes_received = 0;
        int inserted = 0;
	LIST_HEAD(full_list);

	IVTV_DEBUG_INFO("ivtv_read: stream %d.. \n", type);

	IVTV_DEBUG_INFO(
		   "ENC: %d bufs, 0x%08x fill; %d free %d dma %d full %d io\n",
		   atomic_read(&st->allocated_buffers),
		   st->buf_fill, atomic_read(&st->free_q.elements),
		   atomic_read(&st->dma_q.elements),
		   atomic_read(&st->full_q.elements),
		   atomic_read(&st->io_q.elements));

	st->seq = 0;
	st->ubytes = 0;

	if (atomic_read(&itv->capturing) == 0 && (st->id == -1)) {
		IVTV_DEBUG_WARN(
			   "Stream not initialized before read (shouldn't happen)\n");
		return -EIO;
	}

	/* Gather buffers */
	down(&st->mlock);
	bytes_received =
	    gather_free_buffers(itv, type, &full_list, count, 1 /*enc */ );
	up(&st->mlock);

	/* If no IO Queue, grab buffer from list */
	if (bytes_received > 0) {
		buf = list_entry(full_list.next, struct ivtv_buffer, list);
		list_del_init(&buf->list);
	}

	/* Did we find any buffers to read */
	if (buf)
		goto no_wait;
	else if (!blocking)	/* Non Blocking, just return */
		return -EAGAIN;

	old_trans_id = st->trans_id;
	then = jiffies;

	set_current_state(TASK_INTERRUPTIBLE);
	add_wait_queue(&st->waitq, &wait);
	do {
		/* Gather buffers */
		down(&st->mlock);
		bytes_received =
		    gather_free_buffers(itv,
					type, &full_list, count, 1 /*enc */ );
		up(&st->mlock);

		/* grab buffer from list */
		if (bytes_received > 0) {
			buf = list_entry(full_list.next,
					 struct ivtv_buffer, list);
			list_del_init(&buf->list);
			if (buf)
				break;
		}

		if (test_bit(IVTV_F_S_STREAMOFF, &st->s_flags) &&
		    test_bit(IVTV_F_I_EOS, &itv->i_flags) &&
		    !test_bit(IVTV_F_S_DMAP, &st->s_flags)) {
			IVTV_DEBUG_INFO(
				   "ENC: EOS received in waitq\n");
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&st->waitq, &wait);

			retval = 0;
			goto read_ok;
		}

		/* Done capturing? */
		if (type != IVTV_DEC_STREAM_TYPE_VBI &&
		    !test_bit(IVTV_F_S_CAPTURING, &st->s_flags)) {
			IVTV_DEBUG_INFO(
				   "ENC: No more data to send, returning 0\n");
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&st->waitq, &wait);

			retval = 0;
			goto read_ok;
		}

		if (type != IVTV_DEC_STREAM_TYPE_VBI &&
		    type != IVTV_DEC_STREAM_TYPE_VOUT &&
		    ivtv_buf_fill_lock(st, 0, BUF_USED) == 0 &&
		    atomic_read(&st->allocated_buffers) &&
		    ((jiffies - then) >= (HZ*6)) &&
		    !test_bit(FW_RESET_LOCK, &itv->r_flags)) {
			IVTV_DEBUG_WARN(
				   "Timeout waiting for data! "
				   "Encoder buffers have 0x%08x bytes "
					"trans_id = %lu.\n",
				   (ivtv_buf_fill_lock(st,
						       0, BUF_USED) - retval),
							st->trans_id);

			/* Failed, already been here */
			if (x) {
				/* guess we couldn't reset it */
				IVTV_DEBUG_WARN(
					   "Capture failed!!!.\n");
				retval = -EIO;

				set_current_state(TASK_RUNNING);
				remove_wait_queue(&st->waitq, &wait);

				ivtv_stop_capture(itv, type);

				goto read_fail;
			}

			/* Possible digitizer sync issue? */
			IVTV_DEBUG_WARN(
				   "Check digitizer Sync\n");
        		if ((itv->card->type == IVTV_CARD_PVR_150) ||
            			(itv->card->type == IVTV_CARD_PG600))
			{
				x = 1;
			} else {
				x = ivtv_check_digitizer_sync(itv);
			}
			if (x) {
				IVTV_DEBUG_WARN(
					   "Resetting digitizer\n");
				x = ivtv_reset_digitizer(itv);
			}

			/* Total Failure */
			if (x) {
				IVTV_DEBUG_WARN(
					   "Failed resetting digitizer!\n");

				/* guess we couldn't reset it */
				IVTV_DEBUG_WARN(
					   "Capture has failed!!! Try again.\n");
				retval = -EIO;

				set_current_state(TASK_RUNNING);
				remove_wait_queue(&st->waitq, &wait);

				ivtv_stop_capture(itv, type);
				goto read_fail;
			} else if (!x &&
				   ivtv_buf_fill_lock(st, 0, BUF_USED) == 0) 
			{
				/* failure, clean up */
				IVTV_DEBUG_WARN(
					   "Trying to restart capture.\n");

				/* Stop old capture */
				if (test_bit(IVTV_F_S_CAPTURING, &st->s_flags))
					ivtv_stop_capture(itv, type);

				set_bit(IVTV_F_S_CAPTURING, &st->s_flags);
				if (ivtv_start_v4l2_encode_stream(itv, type)) {
					IVTV_DEBUG_WARN(
						   "Failed to start capturing "
						   "for stream %d "
						   "count %d ret %d\n",
						   type, (int)count, retval);

					clear_bit(IVTV_F_S_CAPTURING,
						  &st->s_flags);
					retval = -EIO;
					break;
					x = 1;
				}
			}
			/* Don't check digitizer again, bail next time */
		}
#if 0
		if (ivtv_sleep_timeout(HZ, 1)) {
			IVTV_DEBUG_WARN(
				   "ENC: Sleep timeout interrupted.\n");
		}
#else	
		schedule();
#endif /* 0 */
		if (signal_pending(current)) {
			IVTV_DEBUG_INFO(
				   "ENC: User stopped capture.\n");

			retval = -EINTR;
			break;
		}
	} while (!retval);

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&st->waitq, &wait);

	/* an error (or signal) occured */
	if (retval) {
		goto read_fail;
	}
/* If we get buffers without waiting */
      no_wait:
	st->first_read = 0;
	bufsize = buf->buffer.bytesused;

	/* copy buffer to the client */
	while (count && (buf->buffer.bytesused || (!list_empty(&full_list) &&
						   type ==
						   IVTV_ENC_STREAM_TYPE_MPG
						   && itv->
						   vbi_sliced_mpeg_offset
						   [idx]))) {
		u8 *p;
		static const u8 comp00[] = { 0, 0, 1, 0x00 };  /* start of frame code */
		static const u8 compba[] = { 0, 0, 1, 0xba };  /* start of pack data code */
		int read_from_buf = 1;
		int sliced_mpeg_offset = (type == IVTV_ENC_STREAM_TYPE_MPG) ?
		    itv->vbi_sliced_mpeg_offset[idx] : 0;

		/* byteswap ABCD->DCBA for MPG data,if bit set */
		if (test_and_clear_bit(IVTV_F_B_NEED_BUF_SWAP, &buf->b_flags)) {
			int y;
			for (y = 0; y < buf->buffer.length; y += 4) {
				swab32s((u32 *) ((uintptr_t) buf->buffer.m.
						 userptr + y));
			}
		}
		newcount = count;
		if (sliced_mpeg_offset) {
			datalen =
			    itv->vbi_sliced_mpeg_size[idx] - sliced_mpeg_offset;
		} else {
			datalen = buf->buffer.bytesused;
		}
		if (newcount > datalen)
			newcount = datalen;

		if (sliced_mpeg_offset) {
			p = itv->vbi_sliced_mpeg_data[idx] + sliced_mpeg_offset;

			if (copy_to_user((char *)((uintptr_t) ubuf + retval),
					 p, newcount)) {
				IVTV_DEBUG_WARN(
					   "ENC: VBI copy to user failed\n");
				retval = -EFAULT;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
                        inserted++;
			read_from_buf = 0;
			itv->vbi_sliced_mpeg_offset[idx] += newcount;
			if (itv->vbi_sliced_mpeg_offset[idx] ==
			    itv->vbi_sliced_mpeg_size[idx]) {
				itv->vbi_sliced_mpeg_offset[idx] =
				    itv->vbi_sliced_mpeg_size[idx] = 0;
				itv->vbi_inserted_frame++;
	                        idx = itv->vbi_inserted_frame % IVTV_VBI_FRAMES;
				//IVTV_DEBUG_INFO("vbi inserted %d (%d, %d) %d - %d = %d\n", idx, itv->vbi_sliced_mpeg_offset[idx], itv->vbi_sliced_mpeg_size[idx], itv->vbi_frame, itv->vbi_inserted_frame, itv->vbi_frame - itv->vbi_inserted_frame);
			}
		}
		else if ((type == IVTV_ENC_STREAM_TYPE_YUV)
			 && (itv->options.yuv_fixup)) {
			newcount =
			    ivtv_YUV_fixup(itv, 
				st, newcount, ubuf + retval, buf);
			if (newcount < 0) {
				IVTV_DEBUG_WARN(
					   "ENC: Error fixing up YUV!\n");
				retval = newcount;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
		}
		else if (type == IVTV_ENC_STREAM_TYPE_MPG
			 && itv->vbi_sliced_mpeg_size[idx]) {
			int n = newcount;
			u8 *f;
			int i = 0;

			f = p = (u8 *) ((uintptr_t) buf->buffer.m.userptr +
					buf->readpos);

			while (n >= 4) {
				i++;
				if (!itv->vbi_search_ba) {
					if (!memcmp(f, comp00, 4))
						itv->vbi_search_ba = 1;
					f++;
					n--;
					//msleep(1);	
					continue;
				}
				if (memcmp(f, compba, 4)) {
					f++;
					n--;
					//msleep(1);	
					continue;
				}
				newcount = f - p;
				itv->vbi_search_ba = 0;
				itv->vbi_sliced_mpeg_offset[idx] = 1;
				break;
			}
			/*IVTV_DEBUG_WARN(
				"VBI Read: looped %d times "
				"to %d adjusted to %d and results=%d\n",
				i, n, newcount, itv->vbi_search_ba);*/
			if (copy_to_user((char *)((uintptr_t) ubuf + retval),
					 p, newcount)) {
				IVTV_DEBUG_WARN(
					   "ENC: VBI2 copy to user failed\n");
				retval = -EFAULT;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
		} else {
			p = (u8 *) ((uintptr_t) buf->buffer.m.userptr +
				    buf->readpos);

			if (type == IVTV_DEC_STREAM_TYPE_VBI) {
				int cnt = ivtv_DEC_VBI_fixup(itv,
							     p, newcount);
				buf->readpos += newcount - cnt;
				buf->buffer.bytesused -= newcount - cnt;
				newcount = cnt;
				p = (u8 *) itv->vbi_sliced_dec_data;
			}
			if (newcount
			    && copy_to_user((char *)((uintptr_t) ubuf + retval),
					    p, newcount)) {
				IVTV_DEBUG_WARN(
					   "ENC: MPG copy to user failed"
					   " addr 0x%08lx bytes 0x%08x\n",
					   (unsigned long)p,
					   buf->buffer.bytesused);
				retval = -EFAULT;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
		}

		retval += newcount;
		count -= newcount;
		if (read_from_buf) {
			buf->readpos += newcount;
			buf->buffer.bytesused -= newcount;
		}

		/* if buffer is empty or we've read the whole frame */
		if (buf->buffer.bytesused == 0) {
			freed++;

			/* Free or Queue */
			if (buf /* && (st->buf_total <= st->buf_min) */ ) {
				buf->readpos = 0;
				buf->ts = jiffies;
				buf->b_flags = 0;

				down(&st->mlock);
				ivtv_enq_buf_nolock(&st->free_q, buf);
				ivtv_buf_fill_nolock(st, bufsize, BUF_SUB);
				up(&st->mlock);
			}

			/*else if (buf) {
			   down(&st->mlock);
			   ivtv_free_buffer(itv, buf, st);
			   ivtv_buf_fill_nolock(st, bufsize, BUF_SUB);
			   buf = NULL;
			   up(&st->mlock);
			   } */
			/* grab next buffer from list */
			if (!list_empty(&full_list)) {
				buf = list_entry(full_list.next,
						 struct ivtv_buffer, list);
				list_del_init(&buf->list);
				if (!buf)
					goto read_ok;
				bufsize = buf->buffer.bytesused;
			} else
				goto read_ok;
		}
		if (!count) {
			/* Still has data left */
			down(&st->mlock);
			ivtv_buf_fill_nolock(st,
					     bufsize - buf->buffer.bytesused,
					     BUF_SUB);
			ivtv_enq_buf_nolock(&st->io_q, buf);
			up(&st->mlock);
		}
	}			/* end of while */

      read_fail:

	/* Did we not empty all the buffers */
	if (!list_empty(&full_list))
		goto read_requeue;

	if (retval > 0) {
		*pos += retval;
	}
	return retval;

      read_ok:

	if (retval == 0 && test_and_clear_bit(IVTV_F_S_STREAMOFF, &st->s_flags)) {
		clear_bit(IVTV_F_S_CAPTURING, &st->s_flags);
		IVTV_DEBUG_INFO(
			   "Returning from STREAMOFF w/%d ret\n", retval);
	}

	IVTV_DEBUG_INFO("ENC: read %d, %d bytes left\n",
		   retval, (int)count);

	/* Did we not empty all the buffers */
	if (!list_empty(&full_list))
		goto read_requeue;

	if (retval > 0) {
		*pos += retval;
	}
	return retval;

      read_requeue:
	if (retval < 0) {
		IVTV_DEBUG_WARN(
			   "ENC: write Requeue data %d %d ret\n", retval,
			   (int)count);
	}

	/* Requeue in IO Queue, failed */
	down(&st->mlock);
	while (!list_empty(&full_list)) {
		buf = list_entry(full_list.next, struct ivtv_buffer, list);
		list_del_init(&buf->list);
		if (buf->buffer.bytesused > 0)
			ivtv_enq_buf_nolock(&st->io_q, buf);
	}
	up(&st->mlock);

	if (retval > 0)
		*pos += retval;

	return retval;
}

ssize_t ivtv_v4l2_read(struct file * filp, char *buf, size_t count,
		       loff_t * pos)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv_stream *stream;
       	struct ivtv_stream *vbi_stream;
	struct ivtv *itv = id->itv;
	int type = id->type;

	IVTV_DEBUG_INFO("v4l2 read\n");

	if (type == IVTV_ENC_STREAM_TYPE_RAD ||
	    type == IVTV_DEC_STREAM_TYPE_MPG ||
	    type == IVTV_DEC_STREAM_TYPE_YUV ||
	    type == IVTV_DEC_STREAM_TYPE_VOUT) {
		/* you cannot read from these stream types. */
		return -EPERM;
	}

	stream = &itv->streams[type];

	/* Try to claim this stream. */
	if (ivtv_claim_stream(id, type))
		return -EBUSY;

	/* This stream does not need to start capturing */
	if (type == IVTV_DEC_STREAM_TYPE_VBI)
		return ivtv_read(filp, buf, count, pos);

	/* If capture is already in progress, then we also have to
	   do nothing extra. */
	if (test_and_set_bit(IVTV_F_S_CAPTURING, &stream->s_flags))
		return ivtv_read(filp, buf, count, pos);

	/* Start VBI capture if required */
       	vbi_stream = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];
        if (type == IVTV_ENC_STREAM_TYPE_MPG &&
            test_bit(IVTV_F_S_INTERNAL_USE, &vbi_stream->s_flags) &&
            !test_and_set_bit(IVTV_F_S_CAPTURING, &vbi_stream->s_flags)) {
                /* Note: the IVTV_ENC_STREAM_TYPE_VBI is claimed
                   automatically when the MPG stream is claimed. 
                   We only need to start the VBI capturing. */
        	if (ivtv_start_v4l2_encode_stream(itv, IVTV_ENC_STREAM_TYPE_VBI)) {
               		IVTV_DEBUG_WARN("VBI capture start failed\n");

                	/* Failure, clean up and return an error */
        		clear_bit(IVTV_F_S_CAPTURING, &vbi_stream->s_flags);
			clear_bit(IVTV_F_S_CAPTURING, &stream->s_flags);
                        /* also releases the associated VBI stream */
			ivtv_release_stream(itv, type);
                	return -EIO;
                }
               	IVTV_DEBUG_INFO("VBI insertion started\n");
	}

	/* Tell the card to start capturing */
	if (!ivtv_start_v4l2_encode_stream(itv, type)) {
        	/* We're done */
	        return ivtv_read(filp, buf, count, pos);
        }

        /* failure, clean up */
        IVTV_DEBUG_WARN("Failed to start capturing for stream %d\n", type);

        /* Note: the IVTV_ENC_STREAM_TYPE_VBI is released
           automatically when the MPG stream is released. 
           We only need to stop the VBI capturing. */
        if (type == IVTV_ENC_STREAM_TYPE_MPG &&
            test_bit(IVTV_F_S_CAPTURING, &vbi_stream->s_flags)) {
                ivtv_stop_capture(itv, IVTV_ENC_STREAM_TYPE_VBI);
                clear_bit(IVTV_F_S_CAPTURING, &vbi_stream->s_flags);
        }
        clear_bit(IVTV_F_S_CAPTURING, &stream->s_flags);
        ivtv_release_stream(itv, type);
        return -EIO;
}

static int ivtv_wait_dec_buffer(struct ivtv *itv, struct ivtv_stream *st,
				int type, int bytesneeded,
				struct list_head *full_list, int block)
{
	DECLARE_WAITQUEUE(wait, current);
	int retval = 0;
	int bytes_received = 0;

	add_wait_queue(&st->waitq, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);

		/* Gather buffers */
		down(&st->mlock);
		bytes_received =
		    gather_free_buffers(itv,
					type, full_list, bytesneeded,
					0 /*dec */ );
		up(&st->mlock);
		if (bytes_received > 0)
			break;
		else if (bytes_received < 0) {
			retval = bytes_received;
			break;
		}

		if (!block)
			break;

		/* If we need to start decoding, do so after the first write */
		if ((((!test_bit(FW_RESET_LOCK, &itv->r_flags)) &&
		      (ivtv_buf_fill_lock(st, 0, BUF_USED) >=
		       itv->dma_cfg.dec_max_xfer &&
		       !test_and_set_bit(IVTV_F_S_DECODING, &st->s_flags)))) ||
		    (ivtv_buf_fill_lock(st, 0, BUF_USED) >= st->buf_max &&
		     test_bit(IVTV_F_S_OVERFLOW, &st->s_flags))) {
			if (test_and_clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags)) 
			{
				ivtv_stop_v4l2_decode_stream(itv, type);
				set_bit(IVTV_F_S_DECODING, &st->s_flags);
			}
			if (ivtv_start_v4l2_decode_stream(itv, type) != 0) {
				IVTV_DEBUG_WARN(
					   "Failed start decode stream %d\n",
					   type);

				/* failure, clean up */
				clear_bit(IVTV_F_S_DECODING, &st->s_flags);
				retval = -EIO;
				break;
			}
		}

		schedule();

		if (signal_pending(current)) {
			retval = -EINTR;
			break;
		}
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&st->waitq, &wait);

	return retval;
}

static int ivtv_fill_dec_buffers(struct ivtv *itv, int type, const char *ubuf,
				 size_t count, int block)
{
	struct ivtv_stream *st = &itv->streams[type];
	struct ivtv_buffer *buf = NULL;
	int copybytes = 0, bytesread = 0, retval = 0;
	int allocated_bufs = 0;
	int offset = 0;
	LIST_HEAD(full_list);

	IVTV_DEBUG_INFO(
		   "ivtv_fill_dec_buffers, %d bytes\n", (int)count);

	/* Grab buffers */
	do {
		retval =
		    ivtv_wait_dec_buffer(itv, st, type,
					 (count - bytesread), &full_list,
					 block);
		if (retval) {
			IVTV_DEBUG_INFO(
				   "wait_dec_buf: "
				   "%zd bytes read, %zd left, ret(%d).\n",
				   (size_t) bytesread,
				   (size_t) (count - bytesread), retval);

			if (!block)
				return bytesread;
		}
	} while (retval && (retval != -EINTR));

	/* User interrupted, return like we finished */
	if (retval == -EINTR)
		return count;

	/* Write data to buffers */
	while (count - bytesread > 0) {
		/* grab next buffer from list */
		if (!list_empty(&full_list)) {
			buf = list_entry(full_list.next,
					 struct ivtv_buffer, list);
			list_del_init(&buf->list);
			if (!buf)
				break;
		} else
			break;

		offset = buf->buffer.bytesused;
		if (!offset) {
			buf->buffer.bytesused = 0;
			buf->readpos = 0;
			buf->ts = jiffies;
			buf->b_flags = 0;
		}

		IVTV_DEBUG_DMA(
			   "DEC: Got FREE Buff with %d bytes ts %lu\n",
			   offset, buf->ts);

		/* bytes left to send > bytes in a free buffer */
		if ((count - bytesread) > (buf->buffer.length - offset)) {
			copybytes = (buf->buffer.length - offset);
		} else {
			copybytes = count - bytesread;
		}

		/* copy data */
		IVTV_DEBUG_DMA("copying %d bytes to 0x%08x\n",
			   copybytes, (int)buf->buffer.m.userptr);

		/* Make sure we really got all the user data */
		if (copy_from_user(((char *)buf->buffer.m.userptr + offset),
				   (char *)((uintptr_t) ubuf + bytesread),
				   copybytes)) {
			if (st->buf_total <= st->buf_min) {
				ivtv_enq_buf_lock(&st->free_q, buf, st);
			} else {
				ivtv_free_buffer(itv, buf, st);
				buf = NULL;
			}
			return -EFAULT;
		}

		bytesread += copybytes;
		buf->buffer.bytesused += copybytes;

		/* Send DMA or IO Queue Buffer */
		if (buf->buffer.bytesused == buf->buffer.length) {
			int y;

			/* Byteswap IO buffer */
			if (st->type == IVTV_DEC_STREAM_TYPE_MPG) {
				for (y = 0; y < buf->buffer.bytesused; y += 4) {
					swab32s((u32 *)
						((uintptr_t) buf->buffer.m.
						 userptr + y));
				}
			}

			down(&st->mlock);
			/* Send to the FULL Queue */
			ivtv_enq_buf_nolock(&st->full_q, buf);
			ivtv_buf_fill_nolock(st,
					     buf->buffer.bytesused, BUF_ADD);
			up(&st->mlock);
			allocated_bufs++;
		} else {
			/* Send to the IO Queue */
			down(&st->mlock);
			ivtv_enq_buf_nolock(&st->io_q, buf);
			up(&st->mlock);
		}
	}

	IVTV_DEBUG_INFO(
		   "DEC: write(%d) %d bufs, 0x%08x fill; "
		   "%d free %d dma %d full %d io\n",
		   allocated_bufs,
		   atomic_read(&st->allocated_buffers),
		   st->buf_fill, atomic_read(&st->free_q.elements),
		   atomic_read(&st->dma_q.elements),
		   atomic_read(&st->full_q.elements),
		   atomic_read(&st->io_q.elements));

	return bytesread;
}

ssize_t ivtv_v4l2_write(struct file * filp, const char *buf, size_t count,
			loff_t * pos)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv *itv = id->itv;
	struct ivtv_stream *stream;
	int bytes_written = 0, ret = 0;
	int type = id->type;
	int blocking = !(filp->f_flags & O_NONBLOCK);

	if (type == IVTV_ENC_STREAM_TYPE_RAD ||
	    type == IVTV_ENC_STREAM_TYPE_VBI ||
	    type == IVTV_ENC_STREAM_TYPE_YUV ||
	    type == IVTV_ENC_STREAM_TYPE_PCM ||
	    type == IVTV_DEC_STREAM_TYPE_VBI ||
	    type == IVTV_ENC_STREAM_TYPE_MPG) {
		/* you cannot read from these stream types. */
		return -EPERM;
	}

	stream = &itv->streams[type];

	if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags))
		return -EBUSY;

	/* Try to claim this stream */
	if (ivtv_claim_stream(id, type))
		return -EBUSY;

	/* This stream does not need to start any decoding */
	if (type == IVTV_DEC_STREAM_TYPE_VOUT) {
		return ivtv_write_vbi(itv, buf, count);
	}
	IVTV_DEBUG_INFO("ivtv_write\n");

	while (count - bytes_written > 0) {	/* completely use up user data 
						 * before returning */

		/* buffer the data - this may block waiting on free buffers */
		ret = ivtv_fill_dec_buffers(itv, type, buf + bytes_written,
					    (count - bytes_written), blocking);

		if (ret <= 0) {
			if (ret == 0 && bytes_written == 0)
				ret = -EAGAIN;
			break;
		}

		bytes_written += ret;
	}

	IVTV_DEBUG_INFO("DEC: returning %d\n", bytes_written);
	return bytes_written ? bytes_written : ret;
}

unsigned int ivtv_v4l2_dec_poll(struct file *filp, poll_table * wait)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv *itv = id->itv;
	struct ivtv_stream *st;
	unsigned int mask = 0;
	int type;

	/* YUV or MPG Stream? */
	if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) {
		type = IVTV_DEC_STREAM_TYPE_YUV;
	} else {
		type = IVTV_DEC_STREAM_TYPE_MPG;
	}

	st = &itv->streams[type];

	/* add stream's waitq to the poll list */
	poll_wait(filp, &st->waitq, wait);

	/* If we need to start decoding, do so after the first write */
	if ((!test_bit(FW_RESET_LOCK, &itv->r_flags) &&
	     (ivtv_buf_fill_lock(st, 0, BUF_USED) >= st->buf_max &&
	      !test_and_set_bit(IVTV_F_S_DECODING, &st->s_flags))) ||
	    (ivtv_buf_fill_lock(st, 0, BUF_USED) >= st->buf_max &&
	     test_bit(IVTV_F_S_OVERFLOW, &st->s_flags))) {
		if (test_and_clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags)) {
			ivtv_stop_v4l2_decode_stream(itv, type);
			set_bit(IVTV_F_S_DECODING, &st->s_flags);
		}
		if (ivtv_start_v4l2_decode_stream(itv, type) != 0) {
			IVTV_DEBUG_WARN(
				   "Failed to start decoding for stream %d\n",
				   type);

			/* failure, clean up */
			clear_bit(IVTV_F_S_DECODING, &st->s_flags);
			return -EIO;
		}
	}

	/* Allow write if buffers are not at the limit or some available */
	if (ivtv_buf_fill_lock(st, 0, BUF_USED) <=
	    st->buf_max - itv->dma_cfg.dec_min_xfer) {
		mask |= POLLOUT | POLLWRNORM;	/* Writable */
	}

	if (mask) {
		IVTV_DEBUG_IRQ("DEC: Poll returning 0x%x\n", mask);

		IVTV_DEBUG_INFO(
			   "DEC: %d bufs, 0x%08x fill; %d free %d dma %d full %d io\n",
			   atomic_read(&st->allocated_buffers),
			   st->buf_fill, atomic_read(&st->free_q.elements),
			   atomic_read(&st->dma_q.elements),
			   atomic_read(&st->full_q.elements),
			   atomic_read(&st->io_q.elements));
	}
	return mask;
}

unsigned int ivtv_v4l2_enc_poll(struct file *filp, poll_table * wait)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv_stream *st;
	unsigned int mask = 0;
	struct ivtv *itv = id->itv;

	st = &itv->streams[id->type];

	/* Are we capturing */
	if (!test_bit(IVTV_F_S_CAPTURING, &st->s_flags)) {
		mask |= POLLERR;
		goto poll_exit;
	}

	/* add stream's waitq to the poll list */
	poll_wait(filp, &st->waitq, wait);

	if ((ivtv_buf_fill_lock(st, 0, BUF_USED) > 0)) {
		mask |= POLLIN | POLLRDNORM;
	}

	if (mask) {
		IVTV_DEBUG_IRQ("ENC: Poll returning 0x%x\n", mask);

		IVTV_DEBUG_INFO(
			   "ENC: %d bufs, 0x%08x fill; %d free %d dma %d full %d io\n",
			   atomic_read(&st->allocated_buffers),
			   st->buf_fill, atomic_read(&st->free_q.elements),
			   atomic_read(&st->dma_q.elements),
			   atomic_read(&st->full_q.elements),
			   atomic_read(&st->io_q.elements));
	}
      poll_exit:
	return mask;
}

int ivtv_v4l2_close(struct inode *inode, struct file *filp)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv_stream *stream, *vbi_stream;
	struct ivtv *itv;

	if (NULL == id) {
		printk(KERN_WARNING "ivtv:  invalid id on v4l2 close\n");
		return -ENODEV;
	}

	itv = id->itv;
	
	IVTV_DEBUG_INFO("v4l2 close\n");

	stream = &itv->streams[id->type];

	/* Easy case first: this stream was never claimed by us */
	if (stream->id != id->open_id) {
                kfree(id);
		return 0;
	}

	/* 'Unclaim' this stream */

	/* Stop radio */
	if (id->type == IVTV_ENC_STREAM_TYPE_RAD) {
		struct video_channel vc = { 0 };

		/* We're going to pause, change inputs and frequencies, and
		   resume, so make sure no one else is doing similar things. */
		down(&stream->mlock);
		// Closing radio device, return to TV mode
		vc.norm = std2norm(itv->std);
		mute_and_pause(itv);
		/* Switch tuner to TV */
		ivtv_tv_tuner(itv, VIDIOCSCHAN, &vc);
		/* Mark that the radio is no longer in use */
		clear_bit(IVTV_F_I_RADIO_USER, &itv->i_flags);
		/* Select TV frequency */
		ivtv_tv_tuner(itv, VIDIOCSFREQ, &itv->freq_tv);
		/* Make sure IF demodulator is updated properly, or we'll get static */
		if (itv->options.tda9887 >= 0) ivtv_tda9887(itv, VIDIOC_S_STD, &itv->std);
		/* Select correct audio input (i.e. TV tuner or Line in) */
		ivtv_audio_set_io(itv);
		/* Start automatic sound detection (if supported) */
		ivtv_audio_freq_changed(itv);
		/* Done! Unmute and continue. */
		unmute_and_resume(itv, 1);
		up(&stream->mlock);
		ivtv_release_stream(itv, id->type);
		kfree(id);
		return 0;
	}

	/* Stop decoding */
	if (test_bit(IVTV_F_S_DECODING, &stream->s_flags)) {
		IVTV_DEBUG_INFO("close stopping decode\n");

		ivtv_stop_v4l2_decode_stream(itv, id->type);
	}

	/* Stop capturing */
	if (test_bit(IVTV_F_S_CAPTURING, &stream->s_flags)) {
		vbi_stream = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];

		IVTV_DEBUG_INFO("close stopping capture\n");
		/* Special case: a running VBI capture for VBI insertion
		   in the mpeg stream. Need to stop that too. */
		if (id->type == IVTV_ENC_STREAM_TYPE_MPG &&
	            test_bit(IVTV_F_S_CAPTURING, &vbi_stream->s_flags) &&
                    vbi_stream->id == -1) {
			IVTV_DEBUG_INFO(
				"close stopping embedded VBI capture\n");
			ivtv_stop_capture(itv, IVTV_ENC_STREAM_TYPE_VBI);
		}
		ivtv_stop_capture(itv, id->type);
	}
	if (id->type == IVTV_DEC_STREAM_TYPE_YUV  && test_bit(IVTV_F_S_DECODING_YUV, &stream->s_flags)) { /* Reset registers we have changed so mpeg playback works */
                int h_filter, v_filter_1, v_filter_2;

		IVTV_DEBUG_INFO("close YUV restore registers\n");
                wait_event_interruptible(itv->vsync_w_yuv,
                                         atomic_read(&itv->yuv_vsync));

                atomic_set(&itv->yuv_vsync, 0);
                atomic_set(&itv->yuv_info.next_dma_frame,-1);
                atomic_set(&itv->yuv_info.next_fill_frame,0);
                writel(itv->yuv_info.reg_2834, itv->reg_mem + 0x02834);
                writel(itv->yuv_info.reg_2838, itv->reg_mem + 0x02838);
                writel(itv->yuv_info.reg_283c, itv->reg_mem + 0x0283c);
                writel(itv->yuv_info.reg_2840, itv->reg_mem + 0x02840);
                writel(itv->yuv_info.reg_2844, itv->reg_mem + 0x02844);
                writel(itv->yuv_info.reg_2848, itv->reg_mem + 0x02848);
                writel(itv->yuv_info.reg_2854, itv->reg_mem + 0x02854);
                writel(itv->yuv_info.reg_285c, itv->reg_mem + 0x0285c);
                writel(itv->yuv_info.reg_2864, itv->reg_mem + 0x02864);
                writel(itv->yuv_info.reg_2870, itv->reg_mem + 0x02870);
                writel(itv->yuv_info.reg_2874, itv->reg_mem + 0x02874);
                writel(itv->yuv_info.reg_2890, itv->reg_mem + 0x02890);
                writel(itv->yuv_info.reg_289c, itv->reg_mem + 0x0289c);

                writel(itv->yuv_info.reg_2918, itv->reg_mem + 0x02918);
                writel(itv->yuv_info.reg_291c, itv->reg_mem + 0x0291c);
                writel(itv->yuv_info.reg_2920, itv->reg_mem + 0x02920);
                writel(itv->yuv_info.reg_2924, itv->reg_mem + 0x02924);
                writel(itv->yuv_info.reg_2928, itv->reg_mem + 0x02928);
                writel(itv->yuv_info.reg_292c, itv->reg_mem + 0x0292c);
                writel(itv->yuv_info.reg_2930, itv->reg_mem + 0x02930);
                writel(itv->yuv_info.reg_2934, itv->reg_mem + 0x02934);
                writel(itv->yuv_info.reg_2938, itv->reg_mem + 0x02938);
                writel(itv->yuv_info.reg_293c, itv->reg_mem + 0x0293c);
                writel(itv->yuv_info.reg_2940, itv->reg_mem + 0x02940);
                writel(itv->yuv_info.reg_2944, itv->reg_mem + 0x02944);
                writel(itv->yuv_info.reg_2948, itv->reg_mem + 0x02948);
                writel(itv->yuv_info.reg_294c, itv->reg_mem + 0x0294c);
                writel(itv->yuv_info.reg_2950, itv->reg_mem + 0x02950);
                writel(itv->yuv_info.reg_2954, itv->reg_mem + 0x02954);
                writel(itv->yuv_info.reg_2958, itv->reg_mem + 0x02958);
                writel(itv->yuv_info.reg_295c, itv->reg_mem + 0x0295c);
                writel(itv->yuv_info.reg_2960, itv->reg_mem + 0x02960);
                writel(itv->yuv_info.reg_2964, itv->reg_mem + 0x02964);
                writel(itv->yuv_info.reg_2968, itv->reg_mem + 0x02968);
                writel(itv->yuv_info.reg_296c, itv->reg_mem + 0x0296c);
                writel(itv->yuv_info.reg_2970, itv->reg_mem + 0x02970);

		// Restore the filter

		// First the horizontal filter
		if ((itv->yuv_info.reg_2834 & 0x0000FFFF) == (itv->yuv_info.reg_2834 >> 16)) {
			// An exact size match uses filter 0
			h_filter = 0;
		}
		else {
			// Figure out which filter to use
			h_filter = ((itv->yuv_info.reg_2834 << 16) / (itv->yuv_info.reg_2834 >> 16)) >> 15;
			h_filter = (h_filter >> 1) + (h_filter & 1);
			// Only an exact size match can use filter 0.
			if (h_filter < 1) h_filter = 1;
		}

		// Now the vertical filter
		if ((itv->yuv_info.reg_2918 & 0x0000FFFF) == (itv->yuv_info.reg_2918 >> 16)) {
			// An exact size match uses filter 0/1
			v_filter_1 = 0;
			v_filter_2 = 1;
		}
		else {
			// Figure out which filter to use
			v_filter_1 = ((itv->yuv_info.reg_2918 << 16) / (itv->yuv_info.reg_2918 >> 16)) >> 15;
			v_filter_1 = (v_filter_1 >> 1) + (v_filter_1 & 1);
			// Only an exact size match can use filter 0.
			if (v_filter_1 == 0) v_filter_1 = 1;
			v_filter_2 = v_filter_1;
		}

		// No we restore the filter
		ivtv_yuv_filter (itv,h_filter,v_filter_1,v_filter_2);

		// and clear a few registers
		writel(0, itv->reg_mem + 0x02814);
		writel(0, itv->reg_mem + 0x0282c);
		writel(0, itv->reg_mem + 0x02904);
		writel(0, itv->reg_mem + 0x02910);
            clear_bit(IVTV_F_S_DECODING_YUV, &stream->s_flags);
        }


	ivtv_release_stream(itv, id->type);

	kfree(id);
	return 0;
}

int ivtv_v4l2_open(struct inode *inode, struct file *filp)
{
	int x, y = 0;
	struct ivtv_open_id *item;
	struct ivtv *itv = NULL;
	struct ivtv_stream *stream = NULL;
	int minor = MINOR(inode->i_rdev);

	IVTV_DEBUG_INFO("v4l2 open on minor %d\n", minor);

	/* Find which card this open was on */
	spin_lock(&ivtv_cards_lock);
	for (x = 0; itv == NULL && x < ivtv_cards_active; x++) {
		/* find out which stream this open was on */
		for (y = 0; y < ivtv_cards[x]->streamcount; y++) {
			stream = &ivtv_cards[x]->streams[y];
			IVTV_DEBUG_INFO("current minor %d\n",
				   stream->v4l2dev->minor);
			if (stream->v4l2dev->minor == minor) {
				itv = ivtv_cards[x];
				break;
			}
		}
	}
	spin_unlock(&ivtv_cards_lock);

	if (itv == NULL) {
		/* Couldn't find a device registered 
		   on that minor, shouldn't happen! */
		IVTV_ERR("No ivtv device found on minor %d!\n", minor);
		return -ENXIO;
	}

	if (y == IVTV_DEC_STREAM_TYPE_YUV) {
		atomic_set(&itv->yuv_info.next_dma_frame,0);
		if (ivtv_read_reg(itv->reg_mem + 0x82c) == 0) {
			IVTV_ERR
			    ("Tried to open YUV output device but need to send data to mpeg decoder before it can be used\n");
                        //			return -ENODEV;
		}
                ivtv_alloc_user_dma(itv,
                                    &itv->streams[IVTV_DEC_STREAM_TYPE_YUV].udma,
                                    IVTV_DMA_SG_OSD_ENT);
                
	}

	// Allocate memory
	item = kmalloc(sizeof(struct ivtv_open_id), GFP_KERNEL);
	if (NULL == item) {
		IVTV_DEBUG_WARN("nomem on v4l2 open\n");
		return -ENOMEM;
	}
	item->itv = itv;
	item->type = y;

	item->open_id = itv->open_id++;
	filp->private_data = item;

	if (item->type == IVTV_ENC_STREAM_TYPE_RAD) {
		/* Try to claim this stream */
		if (ivtv_claim_stream(item, item->type)) {
			/* No, it's already in use */
			kfree(item);
			return -EBUSY;
		}

		/* We're going to pause, change inputs and frequencies, and
		   resume, so make sure no one else is doing similar things. */
		down(&stream->mlock);
		/* We have the radio */
		mute_and_pause(itv);
		/* Switch tuner to radio */
		ivtv_radio_tuner(itv, AUDC_SET_RADIO, 0);
		/* Switch IF demodulator to radio */
		if(itv->options.tda9887 >= 0) ivtv_tda9887(itv, AUDC_SET_RADIO, 0);
		/* Mark that the radio is being used. */
		set_bit(IVTV_F_I_RADIO_USER, &itv->i_flags);
		/* Select radio frequency */
		ivtv_radio_tuner(itv, VIDIOCSFREQ, &itv->freq_radio);
		/* Select the correct audio input (i.e. radio tuner) */
		ivtv_audio_set_io(itv);
		/* Start automatic sound detection (if supported) */
		ivtv_audio_freq_changed(itv);
		/* Done! Unmute and continue. */
		unmute_and_resume(itv, 1);
		up(&stream->mlock);
	}

	/* YUV or MPG Decoding Mode? */
	if (y == IVTV_DEC_STREAM_TYPE_MPG)
		clear_bit(IVTV_F_I_DEC_YUV, &itv->i_flags);
	else if (y == IVTV_DEC_STREAM_TYPE_YUV)
        {
		set_bit(IVTV_F_I_DEC_YUV, &itv->i_flags);
                itv->yuv_info.yuv_src_h = 0;
                itv->yuv_info.yuv_src_w = 0;
		itv->yuv_info.v_filter_1 = -1;
		itv->yuv_info.v_filter_2 = -1;
		itv->yuv_info.h_filter = -1;
		itv->yuv_info.osd_x_offset = ivtv_read_reg ( itv->reg_mem + 0x02a04 ) & 0x00000FFF;
		itv->yuv_info.osd_y_offset = (ivtv_read_reg ( itv->reg_mem + 0x02a04 ) >> 16) & 0x00000FFF;
		
		// Bit 2 of reg 2878 indicates current decoder output format
		// 0 : NTSC    1 : PAL
		if (ivtv_read_reg (itv->reg_mem + 0x2878) & 4)
			itv->yuv_info.decode_height = 576;
		else
			itv->yuv_info.decode_height = 480;
        }

	return 0;
}

static u32 ivtv_pause_encoder(struct ivtv *itv, int cmd)
{
	u32 data[IVTV_MBOX_MAX_DATA], result = 0;
	int x;

	/* Check if needed to resume */
	switch (cmd) {
	case 0:		/* pause */
		break;
	case 1:		/* unpause */
		break;
	default:		/* wrong input */
		return -EIO;
	}

	data[0] = 0;		/* 0 = pause, 1 = unpause */
	if (cmd)
		data[0] = 1;

	x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_PAUSE_ENCODER,
		     &result, 1, &data[0]);

	return result;
}

void mute_and_pause(struct ivtv *itv)
{
	int dig;

	/* Mute sound to avoid pop */
	ivtv_audio_set_mute(itv, 1);

	if (0 != ivtv_pause_encoder(itv, 0))
		IVTV_DEBUG_WARN("Mute: Error pausing stream\n");

	IVTV_DEBUG_INFO("Disabling digitizer\n");
	dig = 0;
	itv->card->video_dec_func(itv, DECODER_ENABLE_OUTPUT, &dig);
}

void unmute_and_resume(struct ivtv *itv, int sleep)
{
	int x, dig;

	/* this seems to be needed when in mid encode, instead of init
	   refresh input (no args) */
	x = ivtv_vapi(itv, IVTV_API_REFRESH_INPUT, 0);
	if (x)
		IVTV_DEBUG_WARN(
			   "Error refreshing input. Code %d\n\n", x);

	IVTV_DEBUG_INFO("Enabling digitizer\n");
	dig = 1;
	itv->card->video_dec_func(itv, DECODER_ENABLE_OUTPUT, &dig);

	if (0 != ivtv_pause_encoder(itv, 1))
		IVTV_DEBUG_WARN("Unmute: Error unpausing stream\n");

	if (sleep)
		ivtv_sleep_timeout(HZ / 10, 0);

	/* Unmute */
	ivtv_audio_set_mute(itv, 0);
	IVTV_DEBUG_INFO("Finished with Mute\n");
}
