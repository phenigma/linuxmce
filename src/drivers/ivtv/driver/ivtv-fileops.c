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

typedef unsigned long uintptr_t;

int ivtv_claim_stream(struct ivtv_open_id *id, int type)
{
	struct ivtv_stream *stream = &id->itv->streams[type];

	if (!test_and_set_bit(IVTV_F_S_IN_USE, &stream->s_flags)) {
		/* claim this stream if it wasn't already claimed */
		stream->id = id->open_id;
	}
	if (stream->id != id->open_id) {
		/* someone else is using this stream already */
		return -EBUSY;
	}
	return 0;
}

void ivtv_release_stream(struct ivtv *itv, int type)
{
	struct ivtv_stream *stream = &itv->streams[type];

	stream->id = -1;
	clear_bit(IVTV_F_S_IN_USE, &stream->s_flags);
}

static int ivtv_check_digitizer_sync(struct ivtv *itv)
{
	int sig=0, ret=0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Checking digitizer\n");

	ret = ivtv_saa7115(itv, DECODER_GET_STATUS, &sig);

	if (ret) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,
			   "I2C call failed. is saa7115 loaded?\n");
	}
	if (sig & DECODER_STATUS_GOOD) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "Digitizer sync GOOD\n");
	} else {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "Digitizer sync BAD\n");
		return -EIO;
	}

        return 0; 
}

static int ivtv_reset_digitizer(struct ivtv *itv)
{
        int x=0, ret=0;

        /* x is just a placeholder. It's unused */
        /* this just resets the scaler.. shouldn't need to re-do any settings */
        ret = ivtv_saa7115(itv, DECODER_RESET, &x);

        return ret;
}


#ifdef YUV_FIXUP
static int ivtv_YUV_fixup(struct ivtv_stream *st, int count,
			  char *ubuf, struct ivtv_buffer *buf)
{
/*  
 * int count = # of bytes to transfer to client 
 * st->ubytes = # of bytes written this frame
 * ubuf = buffer to write to (user's buffer)
 * buf = read buffer
 *
 */
	int src_width=720; /* all known formats have src width of 720 */
	int Hoff, Voff; /* collectors for offsets to read position */
	int width, height; /* resolution of the capture stream */
	int curline; /* vertical line currently being processed */
	int maxline; /* height of combined frame */
	int cur_m_block; /* current horizontal offset of working mblock in this row */
	int maxblock; /* # of macroblocks in a row */
	int Hbytes; /* # of bytes to write to user this time around */
	int retval=0; /* accumulator for total bytes written */
	int start; /* position in buf to read from */
	int buf_start; /* byte offset of first byte in this *buf */

	height = st->format.fmt.pix.height;
	width  = st->format.fmt.pix.width;
	maxblock = (width + 0xf) >> 4;
	maxline = (int)(1.5 * height); /* 1 for Y, .5 for UV */
	/* Offset is always bufsize * buffer index 
	buf_start = (st->ubytes - buf->readpos);  tested/works */

	buf_start = itv->dma_cfg.enc_yuv_buf_size * buf->buffer.index;

	/* FIXME it may not be possible to get YUV width > 720 */
	// if (width > src_width) src_width=width;

	curline = (int) (st->ubytes / width);

	while (curline < maxline) {
// 	printk(" cl: %d, ml: %d\n", curline, maxline);
	Voff =	16 * (curline & 0xf) +		   /* Voffset within MBlock */
		  ((curline & 0xfff0) * src_width); /* Voffset of Mblock */

	cur_m_block = (st->ubytes - (curline * width)) >> 4;

/*	printk("voff %d, macroVoff %d, Voff %d, cmb %d\n", (16 * (curline & 0xf)), 
			((curline & 0xff0) * src_width), Voff, cur_m_block);
*/

	while ((cur_m_block < maxblock) && (count > 0)) {
		Hoff   = (cur_m_block * 256) +	/* mblock offset within line */
			 /* Hoffset within mblock, usually 0 */
			 ((st->ubytes - (curline * width)) & 0xf);
		Hbytes = 16 - ((st->ubytes - (curline * width)) & 0xf);

		if (Hbytes > count) Hbytes = count;

		start = Hoff + Voff;

		if (copy_to_user((char *)((uintptr_t)ubuf + retval),
				 (u32 *)((uintptr_t)buf->buffer.m.userptr +
				 (start - buf_start)),
				 Hbytes)) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "copy to user failed\n");
			return -EFAULT;
		}
		
		count -= Hbytes;
		retval += Hbytes;
		st->ubytes += Hbytes;

		cur_m_block++;
	}

	/* if user can't handle anymore data or buffer empty */
	curline++;
	if ((count == 0) ) /*|| ((curline * src_width) % itv->dma_cfg.enc_yuv_buf_size) == 0)*/
		return retval;
	}

	/* FIXME i don't think you should ever get here */
	IVTV_DEBUG(IVTV_DEBUG_ERR, 
		"You've just sailed off the edge of this function\n");
	return retval;
}
#endif

static int gather_free_buffers(struct ivtv *itv, int streamtype,
                struct list_head *free_list, int bytes_needed, int enc)
{
        struct ivtv_stream *st = &itv->streams[streamtype];
        struct ivtv_buffer *buf;
        int bytes_received = 0;
        int x;

        /* gather the needed buffers first, so we don't have to bail
         * in mid-air. put them on a list on the stack */
        for (x = 0; (bytes_received < bytes_needed); x++) {
		if (enc) {
                	buf = ivtv_deq_buf_nolock(&st->io_q, streamtype);
			if (!buf)
                		buf = ivtv_deq_buf_nolock(&st->full_q, 
					streamtype);
			if (!buf)
				break;
                	bytes_received += buf->buffer.bytesused;
		} else {
			int free_q = 0;

                	buf = ivtv_deq_buf_nolock(&st->io_q, streamtype);
			if (!buf) {
                		buf = ivtv_deq_buf_nolock(&st->free_q, 
					streamtype);
				free_q = 1;	
			}
                	if (free_q && buf) {
                        	buf->buffer.bytesused   = 0;
                        	buf->readpos = 0;
                        	buf->ts = 0;
                        	buf->b_flags = 0;

                        	IVTV_DEBUG(IVTV_DEBUG_DMA,
                                	"%d: Got FREE Buff with %d bytes.\n",
                                 	enc, buf->buffer.bytesused);
                	} else if (!buf) {
                        	/* Too many allocated? */
                        	if (st->buf_total >= st->buf_max)
                                	break;

                        	buf = ivtv_init_buffer(itv, st);
                        	if (buf) {
                                	IVTV_DEBUG(IVTV_DEBUG_DMA,
                                        	"%d: Alloc Buffer. (%d)\n",
                                        	enc, 
					atomic_read(&st->allocated_buffers));
                        	}
                	}
                	if (!buf)
                        	break;
                	bytes_received += buf->buffer.length;
		}

                list_add_tail(&buf->list, free_list);
        }

        IVTV_DEBUG(IVTV_DEBUG_DMA,
                "Gather %d: x=%d bytes_needed 0x%08x buf_fill 0x%08x\n",
                enc, x, bytes_needed, st->buf_fill);

        return bytes_received;
}

static int ivtv_read(struct file *filp, char *ubuf, size_t count, loff_t *pos) 
{
        struct ivtv_open_id *id = filp->private_data;
        struct ivtv *itv = id->itv;
	int x = 0, datalen, retval = 0, freed = 0;
	size_t newcount, bufsize = 0;
	int type = id->type;
	struct ivtv_buffer *buf = NULL;
	struct ivtv_stream *st = &itv->streams[type];
	DECLARE_WAITQUEUE(wait, current);
	int idx = itv->vbi_inserted_frame % 20;
	int blocking = !(filp->f_flags & O_NONBLOCK);
	u32 old_trans_id, then;
        u32 bytes_received = 0;
        LIST_HEAD(full_list);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv_read: stream %d.. \n", type);

        IVTV_DEBUG(IVTV_DEBUG_INFO,
                   "ENC: %d bufs, 0x%08x fill; %d free %d dma %d full %d io\n",
                   atomic_read(&st->allocated_buffers),
		   st->buf_fill, atomic_read(&st->free_q.elements),
		   atomic_read(&st->dma_q.elements),
		   atomic_read(&st->full_q.elements),
 		   atomic_read(&st->io_q.elements));

	st->seq = 0;
	st->ubytes = 0;

	if (atomic_read(&itv->capturing) == 0 && (st->id == -1)) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,
		   "Stream not initialized before read (shouldn't happen)\n");
		return -EIO;
	}

        /* Gather buffers */
	down(&st->mlock);
        bytes_received =
                gather_free_buffers(itv,
                        type, &full_list, count, 1/*enc*/);
	up(&st->mlock);

	/* If no IO Queue, grab buffer from list */
	if (bytes_received > 0) {
                buf = list_entry(full_list.next,
                        struct ivtv_buffer, list);
		list_del_init(&buf->list);
	}

	/* Did we find any buffers to read */
	if (buf)
		goto no_wait;
	else if (!blocking) /* Non Blocking, just return */
		return -EAGAIN;

	old_trans_id = itv->trans_id; 
	then = jiffies;

	set_current_state(TASK_INTERRUPTIBLE);
	add_wait_queue(&st->waitq, &wait);
	do {
        	/* Gather buffers */
		down(&st->mlock);
        	bytes_received =
                	gather_free_buffers(itv,
                        	type, &full_list, count, 1/*enc*/);
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
                                !test_bit(IVTV_F_S_DMAP, &st->s_flags)) 
		{
			IVTV_DEBUG(IVTV_DEBUG_INFO,
					"ENC: EOS received in waitq\n");
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&st->waitq, &wait);

			retval = 0;
			goto read_ok;
		}
		
		/* Done capturing? */
		if (type != IVTV_DEC_STREAM_TYPE_VBI && 
				!test_bit(IVTV_F_S_CAPTURING, &st->s_flags)) {
			IVTV_DEBUG(IVTV_DEBUG_INFO,
				"ENC: No more data to send, returning 0\n");
			set_current_state(TASK_RUNNING);
			remove_wait_queue(&st->waitq, &wait);

			retval = 0;
			goto read_ok;
		}

 		if (type != IVTV_DEC_STREAM_TYPE_VBI &&
 		    type != IVTV_DEC_STREAM_TYPE_VOUT &&
		    old_trans_id == itv->trans_id &&
		    ivtv_buf_fill_lock(st, 0, BUF_USED) == 0 &&
		    ((jiffies - then) >= HZ) &&
			!test_bit(FW_RESET_LOCK, &itv->r_flags)) 
		{
			IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"Timeout waiting for data! "
				"Encoder buffers have 0x%08x bytes.\n",
				(ivtv_buf_fill_lock(st, 
					0, BUF_USED)-retval));

			/* Failed, already been here */
			if (x) {
				/* guess we couldn't reset it */
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"Capture failed!!!.\n");
                		retval = -EIO;

        			set_current_state(TASK_RUNNING);
        			remove_wait_queue(&st->waitq, &wait);

        			ivtv_stop_capture(itv, type);

				goto read_fail;
			}

			/* Possible digitizer sync issue? */
			if (itv->trans_id == 0)
				x = ivtv_check_digitizer_sync(itv);

			if (x) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"Resetting digitizer\n");
				x = ivtv_reset_digitizer(itv);
			}

			/* Total Failure */
			if (x) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"Failed reseting digitizer!\n");

				/* guess we couldn't reset it */
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"Capture has failed!!! Try again.\n");
                		retval = -EIO;

        			set_current_state(TASK_RUNNING);
        			remove_wait_queue(&st->waitq, &wait);

        			ivtv_stop_capture(itv, type);
				goto read_fail;
			} else if (!x && 
				ivtv_buf_fill_lock(st, 0, BUF_USED) == 0 &&
				test_and_clear_bit(IVTV_F_S_OVERFLOW, 
					&st->s_flags)) 
			{
               			/* failure, clean up */
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"Trying to restart capture.\n");

				/* Stop old capture */
				if (test_bit(IVTV_F_S_CAPTURING, &st->s_flags))
        				ivtv_stop_capture(itv, type);

				set_bit(IVTV_F_S_CAPTURING, &st->s_flags);
        			if (ivtv_start_v4l2_encode_stream(itv, type)) {
                			IVTV_DEBUG(IVTV_DEBUG_ERR,
                        			"Failed to start capturing "
						"for stream %d "
                        			"count %d ret %d\n",
                        			type, (int)count, retval);

					clear_bit(IVTV_F_S_CAPTURING, 
						&st->s_flags);
                			retval = -EIO;
					x = 1;
        			}
			}
			/* Don't check digitizer again, bail next time */
		}
		old_trans_id = itv->trans_id; 
		then = jiffies;

		/* Wait for available data */
		schedule();

               	if (signal_pending(current)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
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
	itv->first_read = 0;	
        bufsize = buf->buffer.bytesused;

	/* copy buffer to the client */
        while (count && (buf->buffer.bytesused || (!list_empty(&full_list) &&
                type == IVTV_ENC_STREAM_TYPE_MPG && itv->vbi_sliced_mpeg_offset[idx])))
	{
		u8 *p;
		static u8 ba[] = { 0, 0, 1, 0xba };
		static u8 be[] = { 0, 0, 1, 0xbe };
		int read_from_buf = 1;
                int sliced_mpeg_offset = (type == IVTV_ENC_STREAM_TYPE_MPG) ? 
                                         itv->vbi_sliced_mpeg_offset[idx] : 0;

                /* byteswap ABCD->DCBA for MPG data,if bit set*/
                if (test_and_clear_bit(IVTV_F_B_NEED_BUF_SWAP, 
                            &buf->b_flags))  
                {
                        int y;
                        for (y = 0; y < buf->buffer.length; y+= 4) {
                                swab32s((u32 *)((uintptr_t) buf->buffer.m.userptr + y));
                        }
                }
		newcount = count;
		if (sliced_mpeg_offset) {
			datalen = itv->vbi_sliced_mpeg_size[idx] - sliced_mpeg_offset;
		}
		else {
			datalen = buf->buffer.bytesused;
		}
		if (newcount > datalen)
		       	newcount = datalen;

		if (sliced_mpeg_offset) {
			p = itv->vbi_sliced_mpeg_data[idx] + sliced_mpeg_offset;

			if (copy_to_user((char *)((uintptr_t)ubuf+retval), 
					p, newcount)) 
			{
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"ENC: VBI copy to user failed\n");
				retval = -EFAULT;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
			read_from_buf = 0;
			itv->vbi_sliced_mpeg_offset[idx] += newcount;
			if (itv->vbi_sliced_mpeg_offset[idx] == 
				itv->vbi_sliced_mpeg_size[idx]) 
			{
				itv->vbi_sliced_mpeg_offset[idx] = 
					itv->vbi_sliced_mpeg_size[idx] = 0;
				itv->vbi_inserted_frame++;
			}
		}
#ifdef YUV_FIXUP
		else if ((type == IVTV_ENC_STREAM_TYPE_YUV) && (itv->options.yuv_fixup)) {
			newcount = ivtv_YUV_fixup(st,newcount,ubuf+retval,buf);
			if (newcount < 0) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"ENC: Error fixing up YUV!\n");
				retval = newcount;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
		} 
#endif
		else if (type == IVTV_ENC_STREAM_TYPE_MPG && itv->vbi_sliced_mpeg_size[idx]) {
			int n = newcount;
			u8 *f;

			f = p = (u8 *)((uintptr_t)buf->buffer.m.userptr + 
						buf->readpos);

			while (n >= 4) {
				if (!itv->vbi_search_ba) {
					if (!memcmp(f, be, 4)) 
						itv->vbi_search_ba = 1;
					f++; n--;
					continue;
				}
				if (memcmp(f, ba, 4)) {
					f++; n--;
					continue;
				}
				newcount = f - p;
				itv->vbi_search_ba = 0;
				itv->vbi_sliced_mpeg_offset[idx] = 1;
				break;
			}
			if (copy_to_user((char *)((uintptr_t)ubuf+retval), 
					p, newcount)) 
			{
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"ENC: VBI2 copy to user failed\n");
				retval = -EFAULT;
				if (buf && buf->buffer.bytesused > 0)
					ivtv_enq_buf_nolock(&st->io_q, buf);
				goto read_requeue;
			}
		}
		else {
			int field_lines = 
				(itv->std & V4L2_STD_625_50) ? 312 : 263;

			p = (u8 *)((uintptr_t)buf->buffer.m.userptr + 
				buf->readpos);

                    	if (type == IVTV_ENC_STREAM_TYPE_VBI) {
				if (itv->vbi_service_set_in > 0) {
					int cnt = 
						ivtv_SLICED_VBI_fixup(itv, 
							p, newcount, 
							field_lines);
					buf->readpos += newcount - cnt;
					buf->buffer.bytesused -= newcount - cnt;
					newcount = cnt;
					p = (u8 *)itv->vbi_sliced_data;
				}
			}
			else if (type == IVTV_DEC_STREAM_TYPE_VBI) {
				int cnt = ivtv_DEC_VBI_fixup(itv, 
						p, newcount, field_lines);
				buf->readpos += newcount - cnt;
				buf->buffer.bytesused -= newcount - cnt;
				newcount = cnt;
				p = (u8 *)itv->vbi_sliced_dec_data;
			}
			if (newcount && copy_to_user((char *)((uintptr_t)ubuf+retval), 
					p, newcount)) 
			{
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"ENC: MPG copy to user failed"
					" addr 0x%08lx bytes 0x%08x\n", 
					(unsigned long)p, buf->buffer.bytesused);
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
                        if (buf/* && (st->buf_total <= st->buf_min)*/) {
                        	buf->readpos            = 0;
                        	buf->ts                 = jiffies;
                        	buf->b_flags            = 0;

				down(&st->mlock);
                               	ivtv_enq_buf_nolock(&st->free_q, buf);
       				ivtv_buf_fill_nolock(st, bufsize, BUF_SUB);
				up(&st->mlock);
                       	} /*else if (buf) {
				down(&st->mlock);
                               	ivtv_free_buffer(itv, buf, st);
       				ivtv_buf_fill_nolock(st, bufsize, BUF_SUB);
                               	buf = NULL;
				up(&st->mlock);
                       	}*/

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
                        ivtv_buf_fill_nolock(st, bufsize - buf->buffer.bytesused, BUF_SUB);
                        ivtv_enq_buf_nolock(&st->io_q, buf);
			up(&st->mlock);
		}
	} /* end of while */

read_fail:

	/* Did we not empty all the buffers */
        if (!list_empty(&full_list))
		goto read_requeue;	

	if (retval > 0) {
	        *pos += retval;
        }
	return retval;

read_ok:

        if (retval == 0 
		&& test_and_clear_bit(IVTV_F_S_STREAMOFF, &st->s_flags)) 
	{
		clear_bit(IVTV_F_S_CAPTURING, &st->s_flags);
                IVTV_DEBUG(IVTV_DEBUG_INFO,
                       "Returning from STREAMOFF w/%d ret\n", retval);
        }

	IVTV_DEBUG(IVTV_DEBUG_INFO, "ENC: read %d, %d bytes left\n", 
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
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "ENC: write Requeue data %d %d ret\n", retval, (int)count);
        }

	/* Requeue in IO Queue, failed */ 
	down(&st->mlock);
        while (!list_empty(&full_list)) {
                buf = list_entry(full_list.next,
                        struct ivtv_buffer, list);
                list_del_init(&buf->list);
		if (buf->buffer.bytesused > 0)
                	ivtv_enq_buf_nolock(&st->io_q, buf);
        }
	up(&st->mlock);	

	if (retval > 0)
		*pos += retval;

	return retval;
}

ssize_t ivtv_v4l2_read(struct file *filp, char *buf, size_t count, loff_t *pos) 
{
        struct ivtv_open_id *id = filp->private_data;
        struct ivtv *itv = id->itv;
	struct ivtv_stream *stream, *vbi_stream;
	int type = id->type;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 read\n");

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

	/* Tell the card to start capturing */
	if (ivtv_start_v4l2_encode_stream(itv, type)) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"Failed to start capturing for stream %d\n", type);

		/* failure, clean up */
		clear_bit(IVTV_F_S_CAPTURING, &stream->s_flags);
		return -EIO;
	}

	vbi_stream = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];

	/* Note: yes, we could combine the ifs below into one
	   big if, but this is more readable. */

	/* We're done if this is any stream but the mpg stream */
	if (type != IVTV_ENC_STREAM_TYPE_MPG)
		return ivtv_read(filp, buf, count, pos);

	/* We're also done if we do not want to insert vbi data in
	   the mpeg stream. */
	if (!itv->vbi_insert_mpeg)
		return ivtv_read(filp, buf, (count), pos);

	/* In order to insert vbi data in the mpeg stream, the vbi
	   stream must also be started. So try to claim that stream
	   first. On failure, we just continue without vbi insertion.
	   This is debatable. It might be better to just give an
	   error instead. */
	if (ivtv_claim_stream(id, IVTV_ENC_STREAM_TYPE_VBI)) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, 
			"No VBI insertion, VBI stream already in use.\n");
		return ivtv_read(filp, buf, count, pos);
	}

	/* Mark as having no reader process, the data is copied internally */
	set_bit(IVTV_F_S_NO_READ, &vbi_stream->s_flags);
	set_bit(IVTV_F_S_CAPTURING, &vbi_stream->s_flags);
	if (ivtv_start_v4l2_encode_stream(itv, IVTV_ENC_STREAM_TYPE_VBI)) {
		/* Failure, clean up and return an error */
		return -EIO;
	}
	return ivtv_read(filp, buf, count, pos);
}

static int ivtv_wait_dec_buffer(struct ivtv *itv, struct ivtv_stream *st, int type, int bytesneeded, struct list_head *full_list, int block)
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
                        	type, full_list, bytesneeded, 0/*dec*/);
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
        	if (((!test_bit(FW_RESET_LOCK, &itv->r_flags)) &&
			(ivtv_buf_fill_lock(st, 0, BUF_USED) >= 
				itv->dma_cfg.dec_max_xfer &&
			!test_and_set_bit(IVTV_F_S_DECODING, &st->s_flags))) ||
			(ivtv_buf_fill_lock(st, 0, BUF_USED) >= st->buf_max &&
			test_bit(IVTV_F_S_OVERFLOW, &st->s_flags)))
        	{
			if (test_and_clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags))
			{
				ivtv_stop_v4l2_decode_stream(itv, type);
				set_bit(IVTV_F_S_DECODING, &st->s_flags);
			}
                	if (ivtv_start_v4l2_decode_stream(itv, type) != 0) {
                        	IVTV_DEBUG(IVTV_DEBUG_ERR,
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

	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"ivtv_fill_dec_buffers, %d bytes\n", (int)count);

	/* Grab buffers */
	do {
               	retval = 
			ivtv_wait_dec_buffer(itv, st, type, 
				(count-bytesread), &full_list, block);
               	if (retval) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, 
				"wait_dec_buf: "
				"%zd bytes read, %zd left, ret(%d).\n", 
				(size_t)bytesread, (size_t)(count-bytesread), retval);

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
                        buf->buffer.bytesused   = 0;
                        buf->readpos            = 0;
                        buf->ts                 = jiffies;
                        buf->b_flags            = 0;
		}

                IVTV_DEBUG(IVTV_DEBUG_DMA,
                        "DEC: Got FREE Buff with %d bytes ts %lu\n",
                        offset, buf->ts);

		/* bytes left to send > bytes in a free buffer */
		if ((count - bytesread) > (buf->buffer.length - offset)) {
			copybytes = (buf->buffer.length - offset);
		} else {
			copybytes = count - bytesread;
		}

		/* copy data */
		IVTV_DEBUG(IVTV_DEBUG_DMA, "copying %d bytes to 0x%08x\n",
			copybytes, (int)buf->buffer.m.userptr);

		/* Make sure we really got all the user data */	
		if (copy_from_user(((char *)buf->buffer.m.userptr+offset),
			(char *)((uintptr_t)ubuf + bytesread), copybytes)) {
			if (st->buf_total <= st->buf_min)
			{
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
                	for (y = 0; y < buf->buffer.bytesused; y += 4) {
                       		swab32s((u32 *)	
					((uintptr_t)buf->buffer.m.userptr + y));
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

        IVTV_DEBUG(IVTV_DEBUG_INFO,
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

ssize_t ivtv_v4l2_write(struct file *filp, const char *buf, size_t count, loff_t *pos)
{
	struct ivtv_open_id *id = filp->private_data;
        struct ivtv *itv = id->itv;
	struct ivtv_stream *stream;
	int bytes_written = 0, ret = 0;
	int type = id->type;
	int blocking = !(filp->f_flags & O_NONBLOCK);
	
	if (type == IVTV_ENC_STREAM_TYPE_RAD ||
	    type == IVTV_ENC_STREAM_TYPE_YUV ||
	    type == IVTV_ENC_STREAM_TYPE_VBI ||
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
	IVTV_DEBUG(IVTV_DEBUG_INFO, "ivtv_write\n");

	while (count - bytes_written > 0) { /* completely use up user data 
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

	if ((count - bytes_written) > 0)
		*pos -= (count - bytes_written);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "DEC: returning %d\n", bytes_written);
	return bytes_written ? bytes_written : ret;
}

unsigned int ivtv_v4l2_dec_poll(struct file *filp, poll_table *wait)
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
		test_bit(IVTV_F_S_OVERFLOW, &st->s_flags)))
        {
                if (test_and_clear_bit(IVTV_F_S_OVERFLOW, &st->s_flags)) {
                        ivtv_stop_v4l2_decode_stream(itv, type);
			set_bit(IVTV_F_S_DECODING, &st->s_flags);
		}
                if (ivtv_start_v4l2_decode_stream(itv, type) != 0) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "Failed to start decoding for stream %d\n",
                                type);

                        /* failure, clean up */
                        clear_bit(IVTV_F_S_DECODING, &st->s_flags);
                        return -EIO;
                }
        }

	/* Allow write if buffers are not at the limit or some available */
        if (ivtv_buf_fill_lock(st, 0, BUF_USED) <= st->buf_max-itv->dma_cfg.dec_min_xfer)
	{
		mask |= POLLOUT | POLLWRNORM; /* Writable */
	}

	if (mask) {
		IVTV_DEBUG(IVTV_DEBUG_IRQ, 
			"DEC: Poll returning 0x%x\n", mask);

        	IVTV_DEBUG(IVTV_DEBUG_INFO,
                   "DEC: %d bufs, 0x%08x fill; %d free %d dma %d full %d io\n",
                   atomic_read(&st->allocated_buffers),
                   st->buf_fill, atomic_read(&st->free_q.elements),
                   atomic_read(&st->dma_q.elements),
                   atomic_read(&st->full_q.elements),
                   atomic_read(&st->io_q.elements));
	}
	return mask;
}

unsigned int ivtv_v4l2_enc_poll(struct file *filp, poll_table *wait)
{
	struct ivtv_open_id *id = filp->private_data;
	struct ivtv_stream *st;
	unsigned int mask = 0;
	struct ivtv *itv = id->itv;

	/* Are we capturing */
	if (atomic_read(&itv->capturing) == 0) {
		mask |= POLLERR;
       		goto poll_exit;
	}

	st = &itv->streams[id->type];

	/* add stream's waitq to the poll list */
	poll_wait(filp, &st->waitq, wait);

	if ((ivtv_buf_fill_lock(st, 0, BUF_USED) > 0))
	{
		mask |= POLLIN | POLLRDNORM;
	}

        if (mask) {
                IVTV_DEBUG(IVTV_DEBUG_IRQ,
                        "ENC: Poll returning 0x%x\n", mask);

                IVTV_DEBUG(IVTV_DEBUG_INFO,
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
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 close\n");
	
	if (NULL == id) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "invalid id on v4l2 close\n");
		return -ENODEV;
	}

	itv = id->itv;

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
		ivtv_tuner(itv, VIDIOCSCHAN, &vc);
		/* Mark that the radio is no longer in use */
		clear_bit(IVTV_F_I_RADIO_USER, &itv->i_flags);
		/* Select TV frequency */
		ivtv_tuner(itv, VIDIOCSFREQ, &itv->freq_tv);
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
		IVTV_DEBUG(IVTV_DEBUG_INFO, "close stopping decode\n");

		ivtv_stop_v4l2_decode_stream(itv, id->type);
		ivtv_flush_queues_lock(itv, id->type);
	}

	/* Stop capturing */
	if (test_bit(IVTV_F_S_CAPTURING, &stream->s_flags)) {
		vbi_stream = &itv->streams[IVTV_ENC_STREAM_TYPE_VBI];

		IVTV_DEBUG(IVTV_DEBUG_INFO, "close stopping capture\n");
		ivtv_stop_capture(itv, id->type);

		/* Special case: a running VBI capture for VBI insertion
		   in the mpeg stream. Need to stop that too. */
		if (id->type == IVTV_ENC_STREAM_TYPE_MPG &&
		    vbi_stream->id == id->open_id) {
			ivtv_stop_capture(itv, IVTV_ENC_STREAM_TYPE_VBI);
			ivtv_flush_queues_lock(itv, IVTV_ENC_STREAM_TYPE_VBI);
			ivtv_release_stream(itv, IVTV_ENC_STREAM_TYPE_VBI);
		}
		ivtv_flush_queues_lock(itv, id->type);
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

	IVTV_DEBUG(IVTV_DEBUG_INFO, "v4l2 open on minor %d\n", minor);

	/* Find which card this open was on */
	spin_lock(&ivtv_cards_lock);
	for (x = 0; itv == NULL && x < ivtv_cards_active; x++) {
		/* find out which stream this open was on */
		for (y = 0; y < ivtv_cards[x].streamcount; y++) {
			stream = &ivtv_cards[x].streams[y];
			IVTV_DEBUG(IVTV_DEBUG_INFO, "current minor %d\n",
				       stream->v4l2dev->minor);
			if (stream->v4l2dev->minor == minor) {
				itv = &ivtv_cards[x];
				break;
			}
		}
	}
	spin_unlock(&ivtv_cards_lock);

	if (itv == NULL) {
		/* Couldn't find a device registered 
			on that minor, shouldn't happen! */
		IVTV_KERN_ERR("No ivtv device found on minor %d!\n", minor);
		return -ENXIO;
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "opened card # %d, stream %d\n", x, y);

	// Allocate memory
	item = kmalloc(sizeof(struct ivtv_open_id), GFP_KERNEL);
	if (NULL == item) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,"nomem on v4l2 open\n");
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
		ivtv_tuner(itv, AUDC_SET_RADIO, 0);
		/* Mark that the radio is being used. */
		set_bit(IVTV_F_I_RADIO_USER, &itv->i_flags);
		/* Select radio frequency */
		ivtv_tuner(itv, VIDIOCSFREQ, &itv->freq_radio);
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
        	set_bit(IVTV_F_I_DEC_YUV, &itv->i_flags);

	return 0;
}

static u32 ivtv_pause_encoder(struct ivtv *itv, int cmd)
{
	u32 data[IVTV_MBOX_MAX_DATA], result=0;
	int x;

        /* Check if needed to resume */
	switch (cmd) {
		case 0: /* pause */
			break;
		case 1: /* unpause */
			break;
		default: /* wrong input */
			return -EIO;
	}	

        data[0] = 0; /* 0 = pause, 1 = unpause */
	if (cmd) data[0] = 1;

        x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_PAUSE_ENCODER,
		     &result,1, &data[0]);

	return result;
}

void mute_and_pause(struct ivtv *itv)
{
	int dig;

	/* Mute sound to avoid pop */
	ivtv_audio_set_mute(itv, 1);

	if (0 != ivtv_pause_encoder(itv, 0)) 
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"Mute: Error pausing stream\n");

       	IVTV_DEBUG(IVTV_DEBUG_INFO, "Disabling digitizer\n");
       	dig = 0;
	ivtv_saa7115(itv, DECODER_ENABLE_OUTPUT, &dig);
}

void unmute_and_resume(struct ivtv *itv, int sleep)
{
	int x, dig;
	int reset_trys = 100; /* 1 second */

	/* this seems to be needed when in mid encode, instead of init
       	   refresh input (no args) */
       	x = ivtv_vapi(itv, IVTV_API_REFRESH_INPUT, 0);
       	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, 
		"Error refreshing input. Code %d\n\n",x);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Enabling digitizer\n");
       	dig = 1;
       	ivtv_saa7115(itv, DECODER_ENABLE_OUTPUT, &dig);

	if (0 != ivtv_pause_encoder(itv, 1)) 
		IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"Unmute: Error unpausing stream\n");

	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"Finished with reset in %d ms\n", reset_trys);

	if (sleep)
		ivtv_sleep_timeout(HZ/10, 0);

	/* Unmute */
	ivtv_audio_set_mute(itv, 0);
       	IVTV_DEBUG(IVTV_DEBUG_INFO, "Finished with Mute\n");
}

