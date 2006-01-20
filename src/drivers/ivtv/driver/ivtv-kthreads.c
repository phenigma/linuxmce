/*
    audio dual language watch thread and decoder thread.

    Dual language watch:
    Copyright (C) 2004  Stephane Zermatten

    Decoder thread:
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
#include "ivtv-queue.h"
#include "ivtv-streams.h"
#include "ivtv-reset.h"
#include "ivtv-mailbox.h"
#include "ivtv-i2c.h"
#include "ivtv-kthreads.h"
#include "ivtv-irq.h"
#include "ivtv-video.h"
#include "ivtv-compat.h"

#include <linux/smp_lock.h>
#include <linux/wait.h>

static void ivtv_dualwatch_start_kthread(struct ivtv *ivtv);
static void ivtv_dualwatch_stop_kthread(struct ivtv *ivtv);
static void ivtv_dualwatch_init_kthread(struct ivtv *ivtv);
static void ivtv_dualwatch_exit_kthread(struct ivtv *ivtv);
static void ivtv_dualwatch_set_dual(struct ivtv *itv);

static u32 ivtv_round_dma(u32 data)
{
	if (data >= (64 * 1024))
		data = (64 * 1024);
	else if (data >= (32 * 1024))
		data = (32 * 1024);

	return data;
}

/**
 * Change the DUAL flag 
 */
static void ivtv_dualwatch_set_dual(struct ivtv *itv)
{
	u32 audio_bitmask;
	int goal_stereo_flag;
	int stereo_flag;

	if (!itv->dualwatch.encoding)
		return;

	audio_bitmask = itv->dualwatch.audio_bitmask;
	stereo_flag = audio_bitmask & IVTV_CODEC_AUDIO_MPEG_STEREO_MASK;
	if (itv->dualwatch.enabled) {
		struct v4l2_tuner vt;
		memset(&vt, 0, sizeof(vt));
		ivtv_msp34xx(itv, VIDIOC_G_TUNER, &vt);
		if (vt.rxsubchans & V4L2_TUNER_SUB_LANG2) {
			goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_DUAL;
		} else {
			goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO;
		}
	} else {
		if (stereo_flag == IVTV_CODEC_AUDIO_MPEG_STEREO_DUAL) {
			goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO;
		} else {
			goal_stereo_flag = stereo_flag;
		}
	}

	if (stereo_flag == IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO
	    && ((itv->codec.audio_bitmask & IVTV_CODEC_AUDIO_MPEG_STEREO_MASK)
		== IVTV_CODEC_AUDIO_MPEG_STEREO_JOINT)) {
		goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_JOINT;
	}

	if (goal_stereo_flag != stereo_flag) {
		u32 new_bitmap =
		    goal_stereo_flag
		    | (itv->dualwatch.
		       audio_bitmask & ~IVTV_CODEC_AUDIO_MPEG_STEREO_MASK);

		IVTV_DEBUG_INFO(
			   "dualwatch: change stereo flag from 0x%x to 0x%x. new audio_bitmask=0x%ux\n",
			   stereo_flag, goal_stereo_flag, new_bitmap);

		if (ivtv_vapi
		    (itv, IVTV_API_ASSIGN_AUDIO_PROPERTIES, 1,
		     new_bitmap) == 0) {
			itv->dualwatch.audio_bitmask = new_bitmap;
		} else {
			IVTV_DEBUG_INFO(
				   "dualwatch: changing stereo flag failed\n");
		}
	}
}

/* start the thread if necessary */
static void ivtv_dualwatch_update(struct ivtv *itv)
{
	ivtv_dualwatch_set_dual(itv);

	/* start timer, but only if necessary (timer stops automatically) */
	if (itv->dualwatch.encoding && itv->dualwatch.enabled) {
		if (!itv->dualwatch.thread)
			ivtv_dualwatch_start_kthread(itv);
	} else {
		if (itv->dualwatch.thread)
			ivtv_dualwatch_stop_kthread(itv);
	}
}

/* enable/disable dualwatch (= enable/disable full stereo (not lang1/lang2 or mono) mode) */
void ivtv_dualwatch_set_enabled(struct ivtv *itv, int enabled)
{
	itv->dualwatch.enabled = enabled;
	ivtv_dualwatch_update(itv);
}

/* start encoding => start the thread if DUALWATCH is enabled */
void ivtv_dualwatch_start_encoding(struct ivtv *itv)
{
	if (itv->dualwatch.encoding)
		return;
	itv->dualwatch.encoding = 1;
	itv->dualwatch.audio_bitmask = itv->codec.audio_bitmask;
	ivtv_dualwatch_update(itv);
}

/* stop encoding => stop the thread if DUALWATCH is enabled */
void ivtv_dualwatch_stop_encoding(struct ivtv *itv)
{
	if (!itv->dualwatch.encoding)
		return;
	if (itv->dualwatch.thread)
		ivtv_dualwatch_stop_kthread(itv);
	itv->dualwatch.encoding = 0;	/* after stopping the thread */
}

/* function executed on the thread */
static int ivtv_dualwatch_kthread(void *ptr)
{
	DEFINE_WAIT(wait);
	struct ivtv *ivtv = (struct ivtv *)ptr;
	struct ivtv_dualwatch *kthread = &ivtv->dualwatch;

	ivtv_dualwatch_init_kthread(ivtv);

	for (;;) {
		prepare_to_wait(&kthread->queue, &wait, TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		finish_wait(&kthread->queue, &wait);

		mb();

		if (kthread->terminate)
			break;

		ivtv_dualwatch_set_dual(ivtv);
	}
	ivtv_dualwatch_exit_kthread(ivtv);
	return 0;
}

static void ivtv_dualwatch_start_kthread(struct ivtv *ivtv)
{
	/* adapted from example by Martin Frey 
	 * http://www.scs.ch/~frey/linux/kernelthreads.html
	 */
	struct ivtv_dualwatch *kthread = &ivtv->dualwatch;

	if (!kthread->thread) {
		init_MUTEX_LOCKED(&kthread->startstop_sem);
		kernel_thread(ivtv_dualwatch_kthread, ivtv, 0);
		down(&kthread->startstop_sem);
	}
}

static void ivtv_dualwatch_stop_kthread(struct ivtv *ivtv)
{
	struct ivtv_dualwatch *kthread = &ivtv->dualwatch;

	if (!kthread->thread || kthread->terminate)
		return;
	/* this function needs to be protected with the big
	   kernel lock (lock_kernel()). The lock must be
	   grabbed before changing the terminate
	   flag and released after the down() call. */
	lock_kernel();
	if (kthread->thread) {	/* paranoid mode */
		init_MUTEX_LOCKED(&kthread->startstop_sem);
		/* We need to do a memory barrier here to be sure that
		   the flags are visible on all CPUs. 
		 */
		mb();
		/* set flag to request thread termination */
		kthread->terminate = 1;

		/* We need to do a memory barrier here to be sure that
		   the flags are visible on all CPUs. 
		 */
		mb();
		kill_proc(kthread->thread->pid, SIGKILL, 1);
		down(&kthread->startstop_sem);
	}
	unlock_kernel();

	/* now we are sure the thread is in zombie state. We
	   notify init to clean the process up. */
	kill_proc(0, SIGCHLD, 1);

}

/* initialize new created thread. Called by the new thread. */
static void ivtv_dualwatch_init_kthread(struct ivtv *ivtv)
{
	struct ivtv_dualwatch *kthread = &ivtv->dualwatch;

#ifndef LINUX26
	daemonize();
	exit_files(current);
	reparent_to_init();
#else
	daemonize("ivtv_dualwatch");
#endif /* LINUX26 */

	/* lock the kernel. A new kernel thread starts without
	   the big kernel lock, regardless of the lock state
	   of the creator (the lock level is *not* inheritated)
	 */
	lock_kernel();

	/* fill in thread structure */
	kthread->thread = current;

	siginitsetinv(&current->blocked,
		      sigmask(SIGKILL) | sigmask(SIGINT) | sigmask(SIGTERM));
	init_waitqueue_head(&kthread->queue);

	kthread->terminate = 0;
	sprintf(current->comm, "ivtv_dualwatch");

	/* let others run */
	unlock_kernel();

	/* tell the creator that we are ready and let him continue */
	up(&kthread->startstop_sem);
}

/* cleanup of thread. Called by the exiting thread. */
static void ivtv_dualwatch_exit_kthread(struct ivtv *ivtv)
{
	struct ivtv_dualwatch *kthread = &ivtv->dualwatch;

	/* lock the kernel, the exit will unlock it */
	lock_kernel();

	kthread->thread = NULL;
	mb();

	/* notify the stop_kthread() routine that we are terminating. */
	up(&kthread->startstop_sem);

	/* there is no race here between execution of the "killer" and real termination
	   of the thread (race window between up and do_exit), since both the
	   thread and the "killer" function are running with the kernel lock held.
	   The kernel lock will be freed after the thread exited, so the code
	   is really not executed anymore as soon as the unload functions gets
	   the kernel lock back.
	   The init process may not have made the cleanup of the process here,
	   but the cleanup can be done safely with the module unloaded.
	 */
}

void fw_work_handler(void *arg)
{
	struct ivtv *itv = (struct ivtv *)arg;

	/* Check Firmware */
	ivtv_firmware_flags(itv, 100);

	return;
}

/* Must be spinlocked */
static int gather_free_buffers(struct ivtv *itv, int streamtype,
			       struct list_head *free_list, int bytes_needed)
{
	struct ivtv_stream *st = &itv->streams[streamtype];
	struct ivtv_buffer *buf;
	int bytes_received = 0;
	int x;

	if (streamtype == IVTV_DEC_STREAM_TYPE_MPG) {
		if (st->buf_fill >= (64 * 1024)
			 && bytes_needed >= (64 * 1024))
			bytes_needed = (64 * 1024);
		else if (st->buf_fill >= (32 * 1024)
			 && bytes_needed >= (32 * 1024))
			bytes_needed = (32 * 1024);
	}

	/* gather the needed buffers first, so we don't have to bail
	 * in mid-air. put them on a list on the stack */
	for (x = 0; (bytes_received < bytes_needed); x++) {
		buf = ivtv_deq_buf_nolock(itv, &st->full_q, streamtype);
		if (!buf)
			break;

		bytes_received += buf->buffer.bytesused;

		list_add_tail(&buf->list, free_list);
	}

	IVTV_DEBUG_DMA(
		   "DEC: x=%d bytes_needed 0x%08x buf_fill 0x%08x\n",
		   x, bytes_needed, st->buf_fill);

	/* damn, failed */
	if (bytes_received < bytes_needed) {
		IVTV_DEBUG_WARN(
			   "DEC: needed %d bytes for stream %d, "
			   "received %d differ by (%d) bytes\n",
			   bytes_needed, streamtype, bytes_received,
			   (bytes_needed - bytes_received));

		IVTV_DEBUG_WARN(
			   "DEC: free_q: %d elements\n",
			   atomic_read(&st->free_q.elements));
		IVTV_DEBUG_WARN(
			   "DEC: dma_q: %d elements\n",
			   atomic_read(&st->dma_q.elements));
		IVTV_DEBUG_WARN(
			   "DEC: full_q: %d elements\n",
			   atomic_read(&st->full_q.elements));

		while (!list_empty(free_list)) {
			buf = list_entry(free_list->next,
					 struct ivtv_buffer, list);
			list_del_init(&buf->list);
			ivtv_enq_buf_nolock(&st->full_q, buf);
		}

		return 0;
	}
	return bytes_received;
}

void enc_work_handler(void *arg)
{
	struct ivtv *itv = (struct ivtv *)arg;

	/* VIDEO or VBI */
        set_bit(IVTV_F_T_ENC_RUNNING, &itv->t_flags);
	if (test_and_clear_bit(IVTV_F_T_ENC_DMA_NEEDED, &itv->t_flags)) {
		ivtv_sched_DMA(itv);
	}
        clear_bit(IVTV_F_T_ENC_RUNNING, &itv->t_flags);

	return;
}

void enc_vbi_work_handler(void *arg)
{
        struct ivtv *itv = (struct ivtv *)arg;

        /* ENC VBI */
        set_bit(IVTV_F_T_ENC_VBI_RUNNING, &itv->t_flags);
        if (test_and_clear_bit(IVTV_F_T_ENC_VBI_DMA, &itv->t_flags)) {
                ivtv_sched_VBI(itv, IVTV_ENC_STREAM_TYPE_VBI);
        }
        clear_bit(IVTV_F_T_ENC_VBI_RUNNING, &itv->t_flags);

        return;
}

void dec_vbi_work_handler(void *arg)
{
        struct ivtv *itv = (struct ivtv *)arg;

        /* DEC VBI */
        set_bit(IVTV_F_T_DEC_VBI_RUNNING, &itv->t_flags);
        if (test_and_clear_bit(IVTV_F_T_DEC_VBI_INSERT, &itv->t_flags)) {
                ivtv_sched_VBI(itv, IVTV_DEC_STREAM_TYPE_VBI);
        }
        clear_bit(IVTV_F_T_DEC_VBI_RUNNING, &itv->t_flags);

        return;
}

void dec_work_handler(void *arg)
{
	struct ivtv *itv = (struct ivtv *)arg;
	int type = IVTV_DEC_STREAM_TYPE_MPG;
	struct ivtv_stream *stream;
	int ret = -1;
	unsigned long then = 0;
	int rc = 0;
	int x = 0, bytes_written = 0;
	struct ivtv_buffer *buf;
	u32 data[IVTV_MBOX_MAX_DATA], result;
	u32 mem_offset, mem_size, orig_mem_size, hw_stream_type, buffer_bytes;
	int stmtype = type;
	u32 bytes_needed = 0;
	u32 bytes_received;
	LIST_HEAD(free_list);
	long sequence;
	u32 offset, sg_size = 0;
	u32 uv_offset = (itv->height * itv->width);
	u32 yuv_buf = 0;
	int yuv_num = 0;	
	int yuv_frame_size = ((itv->width*itv->height)+ 
				(itv->width*(itv->height/2)));

	int y_done = 0;
	int retrys;
	DECLARE_WAITQUEUE(wait, current);
	int max_xfer = itv->dma_cfg.dec_max_xfer;

	if (!test_bit(IVTV_F_T_DEC_STARTED, &itv->t_flags))
        {
            IVTV_DEBUG_INFO("dec_work_handler: run when decoder not started\n");
            return;
	}

	set_bit(IVTV_F_T_DEC_RUNNING, &itv->t_flags);
	/* YUV or MPG Stream? */
	if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) {
		type = IVTV_DEC_STREAM_TYPE_YUV;
	} else {
		type = IVTV_DEC_STREAM_TYPE_MPG;
	}
	stream = &itv->streams[type];

	/* Wait for FW Reload */
	if (test_bit(FW_RESET_LOCK, &itv->r_flags)) {
		retrys = 500;

		IVTV_DEBUG_WARN("dec_work_handler: Sleeping for FW Reset");

		/* Check FW Reset Status  */
		while (test_bit(FW_RESET_LOCK, &itv->r_flags)) {
			ivtv_sleep_timeout(HZ / 100, 0);
			if (retrys-- == 0)
				break;
		}
	}

	IVTV_DEBUG_DMA(
		   "dec_work_queue: Schedule DMA %lu\n", jiffies);

	ret = ivtv_api_getresult_nosleep(itv, &itv->dec_mbox[9], &result, &data[0]);

	/* Did xfer status fail? */
	if (ret) {
		IVTV_DEBUG_WARN(
			   "DEC 1st DMA Xfer Info failed=0x%08x %lu\n",
			   ret, jiffies);

		goto exit_dec_wait;
	}

	if (type == IVTV_DEC_STREAM_TYPE_MPG)
		data[2] = ivtv_round_dma(data[2]);
	else if (type == IVTV_DEC_STREAM_TYPE_YUV)
		data[2] = yuv_frame_size; 

	then = jiffies;
	add_wait_queue(&stream->waitq, &wait);
	do {
		set_current_state(TASK_INTERRUPTIBLE);

		if (test_bit(FW_RESET_SHUTDOWN, &itv->r_flags)) {
			rc = -EINTR;
			break;
		}

		/* Lock Decoder */
		if (ivtv_buf_fill_lock(stream, 0, BUF_USED) >= data[2] &&
		    (readl
		     ((unsigned char *)itv->reg_mem +
		      IVTV_REG_DMASTATUS) & 0x01)
		    &&
		    !(readl
		      ((unsigned char *)itv->reg_mem +
		       IVTV_REG_DMASTATUS) & 0x14)
		    && !test_and_set_bit(IVTV_F_S_DMAP, &stream->s_flags)) {
			break;
		}

		if (ivtv_sleep_timeout(HZ / 100, 1)) {
			IVTV_DEBUG_WARN(
				   "DEC: User stopped waitq.\n");

			rc = -EINTR;
			break;
		}
		if (!test_bit(IVTV_F_T_DEC_STARTED, &itv->t_flags))
                {
			IVTV_DEBUG_INFO(
				   "DEC: Stop decoder has been called so exiting\n");
			rc = -EINTR;
			break;
		}
                        
	} while (!rc);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&stream->waitq, &wait);

	/* Failed, bail out */
	if (rc || test_bit(FW_RESET_LOCK, &itv->r_flags))
		goto exit_dec_wait;

	/* Start Decoder if not yet */
	if (!test_and_set_bit(IVTV_F_S_DECODING, &stream->s_flags)) {
		if (ivtv_start_v4l2_decode_stream(itv, type)) {
			clear_bit(IVTV_F_S_DECODING, &stream->s_flags);
			IVTV_DEBUG_WARN(
				   "DEC: start_decode, error starting\n");
			goto exit_dec_wait;
		}
		goto clear_dmap;
	}

	/* Is the decoder in need of data */
	if (test_and_clear_bit(IVTV_F_S_NEEDS_DATA, &stream->s_flags)) 
	{
#if 1
		if (!itv->dma_cfg.dec_pio) {
			/* Unmap Last DMA, still a bug if we have to here */
			if (atomic_read(&stream->dma_q.elements) > 0) {
				IVTV_DEBUG_WARN(
					   "DEC: Can't free %d buffers in DMA\n",
					   atomic_read(&stream->dma_q.
						       elements));
			}
		}
		ret = ivtv_api_getresult_nosleep(itv, &itv->dec_mbox[9],
						 &result, &data[0]);

		/* Did xfer status fail? */
		if (ret) {
			IVTV_DEBUG_WARN(
				   "DEC 2nd DMA Xfer Info failed=0x%08x %lu\n",
				   ret, jiffies);

			goto clear_dmap;
		}
		if (type == IVTV_DEC_STREAM_TYPE_MPG)
			data[2] = ivtv_round_dma(data[2]);
		else if (type == IVTV_DEC_STREAM_TYPE_YUV)
			data[2] = yuv_frame_size; 

		/* Wait till data input is ready */
		then = jiffies;
		while (ivtv_buf_fill_lock(stream, 0, BUF_USED) < data[2]) {
			ivtv_sleep_timeout(HZ / 100, 1);
			if ((jiffies - then) > (HZ * 2)) {
				IVTV_DEBUG_INFO(
					   "DEC: Only 0x%08x bytes available need %d.\n",
					   ivtv_buf_fill_lock(stream, 0,
							      BUF_USED),
					   data[2]);
				goto clear_dmap;
			}
			if (!test_bit(IVTV_F_T_DEC_STARTED, &itv->t_flags))					goto clear_dmap;	
		}

		/* Get DMA Xfer Info to know what to send */
		ret = ivtv_api_getresult_nosleep(itv, &itv->dec_mbox[9],
						 &result, &data[0]);
		/* Did xfer status fail? */
		if (ret) {
			IVTV_DEBUG_WARN(
				   "DEC DMA Xfer Info failed=0x%08x %lu\n",
				   ret, jiffies);

			goto clear_dmap;
		}
		/* Does it want enough */
		if (data[2] < itv->dma_cfg.dec_min_xfer) {
			IVTV_DEBUG_INFO(
				   "DEC: Not enough data needed "
				   "by decoder 0x%08x\n", data[2]);

			goto clear_dmap;
		}
#endif /* 1 */
		IVTV_DEBUG_INFO(
			   "DEC: Mailbox 10: 0x%08x 0x%08x 0x%08x 0x%08x\n",
			   data[0], data[1], data[2], data[3]);

		/* Xfer Type */
		if (type == IVTV_DEC_STREAM_TYPE_YUV)	
			hw_stream_type = data[0] = 2;
		else
			hw_stream_type = data[0];
			
		/* Check Xfer Type */
		if (hw_stream_type != 0 && hw_stream_type != 2) {
			IVTV_DEBUG_WARN(
				   "DEC: Decoder Invalid type 0x%08x?\n",
				   hw_stream_type);
			goto clear_dmap;
		}

		/* Xfer mem offset */
		mem_offset = data[1];
		if (type == IVTV_DEC_STREAM_TYPE_YUV) {
			data[2] = mem_size = orig_mem_size = yuv_frame_size;
		} else	
			mem_size = orig_mem_size = data[2];

		/* set mem offset value */
		if (!itv->dma_cfg.dec_pio)
			offset = (mem_offset + IVTV_DECODER_OFFSET);
		else
			offset = mem_offset;

		/* Xfer buffer fullness */
		buffer_bytes = data[3];	/* # bytes in card's buffer */

		/* Sanity Checks for the Decoder DMA Xfer Info */
		if (type == IVTV_DEC_STREAM_TYPE_MPG) {
			if (buffer_bytes < stream->buf_max &&
			    (stream->buf_max - buffer_bytes) <
			    itv->dma_cfg.dec_min_xfer) {
				IVTV_DEBUG_WARN(
					   "DEC: Wants more data 0x%08x left 0x%08x\n",
					   itv->dec_dma_stat.data[1],
					   (orig_mem_size - bytes_written));

				goto clear_dmap;
			}
			if ((mem_offset + itv->dma_cfg.dec_min_xfer) >
			    0x0050fb00) {
				IVTV_DEBUG_WARN(
					   "DEC: Sched Buffer end reached 0x%08x\n",
					   ((mem_offset +
					     itv->dma_cfg.dec_min_xfer)));

				IVTV_DEBUG_WARN(
					   " DEC: Mailbox 10: "
					   "0x%08x 0x%08x 0x%08x 0x%08x\n",
					   data[0], data[1], data[2], data[3]);

				goto clear_dmap;
			}
			/* Check if size request is too big for decoder buffer */
			if ((mem_size + buffer_bytes) > IVTV_DEC_MAX_BUF) {
				IVTV_DEBUG_INFO(
					   "DEC: wants 0x%08x has 0x%08x = 0x%08x!\n",
					   mem_size, buffer_bytes,
					   ((mem_size) + buffer_bytes));
				mem_size = (IVTV_DEC_MAX_BUF - buffer_bytes);
				IVTV_DEBUG_INFO(
					   "DEC: Changed mem_size to 0x%08x.\n",
					   mem_size);
			}
			bytes_needed = itv->dma_cfg.dec_min_xfer;
			if (mem_size > itv->dma_cfg.dec_max_xfer)
				mem_size = itv->dma_cfg.dec_max_xfer;

			mem_size = ivtv_round_dma(mem_size);

			if (mem_offset < 0x0050fb00 &&
			    (mem_offset + mem_size) > 0x0050fb00) {
				goto clear_dmap;
			}
			if ((mem_offset < 0x0040fb00) ||
			    (mem_offset + mem_size) > 0x0050fb00) {
				IVTV_DEBUG_WARN(
					   "DEC: Decoder wants 0x%08x bytes, "
					   "bad memory location 0x%08x.\n",
					   mem_size, (mem_offset + mem_size));
				goto clear_dmap;
			}
		} else {
                        yuv_num = atomic_read(&itv->yuv_num);
			if (mem_offset == 0) {
				mem_offset = yuv_offset[yuv_num];
				offset = yuv_buf = 
					IVTV_DECODER_OFFSET + mem_offset;
			} else {
				IVTV_DEBUG_WARN(
					"YUV DEC: asks for memoffset: "
					"0x%08x\n", mem_offset);
				yuv_buf = IVTV_DECODER_OFFSET + mem_offset;
				offset = yuv_buf;
			}
			/* YUV has a fixed XFER Size  */
			bytes_needed = max_xfer = mem_size = yuv_frame_size; 
		}

		/* Gather enough buffers */
		down(&stream->mlock);
		bytes_received =
		    gather_free_buffers(itv, type, &free_list, max_xfer);
		up(&stream->mlock);

		/* Did it fail, shouldn't ever */
		if (bytes_received < bytes_needed) {
			IVTV_DEBUG_WARN(
				   "DEC: now %d Dec buffers left, were %d left.\n",
				   bytes_received, bytes_needed);

			goto clear_buf;
		}

		/* Check if too large or small */
		if (type == IVTV_DEC_STREAM_TYPE_MPG) {
			if (bytes_received > itv->dma_cfg.dec_max_xfer) {
				IVTV_DEBUG_WARN(
					   "DEC: got %d Dec buffers, way too many\n",
					   bytes_received);
				bytes_received = itv->dma_cfg.dec_max_xfer;
			} else if (bytes_received < itv->dma_cfg.dec_min_xfer) {
				IVTV_DEBUG_WARN(
					   "DEC: got %d Dec buffers, way too few\n",
					   bytes_received);
				goto clear_buf;
			}
		}
		/* only send size of buffer we recieved */
		mem_size = bytes_received;

		IVTV_DEBUG_DEC(
			   "DEC: want: "
			   "0x%08x  length: 0x%08x  full: 0x%08x  space 0x%08x\n",
			   mem_offset, orig_mem_size, buffer_bytes,
			   (0x00100000 - buffer_bytes));

		/* increment the sequence # */
		sequence = ++stream->seq;

		/* Wait Queue Runs Stat */
		itv->dec_dma_stat.wq_runs++;
		IVTV_DEBUG_DEC(
			   " (%lu) DEC WRQ: intr %d wq_runs %d\n",
			   jiffies,
			   atomic_read(&itv->dec_dma_stat.intr),
			   itv->dec_dma_stat.wq_runs);

		/* Setup DMA xfer in SG Array */
		for (x = 0; !list_empty(&free_list); x++) {
			if (bytes_written >= mem_size) {
				IVTV_DEBUG_WARN(
					   "DEC: Error, No more Buffers!!!.\n");
				break;
			}

			/* YUV UV Offset from Y Buffer */
			if ((type == IVTV_DEC_STREAM_TYPE_YUV)  &&
				!y_done && (bytes_written >= uv_offset)) 
			{
				offset = yuv_buf + IVTV_YUV_BUFFER_UV_OFFSET;
				IVTV_DEBUG_DEC(
					"YUV: decoding Y offset 0x%08x " 
					"UV offset 0x%08x bytes 0x%08x\n", 
					yuv_buf, offset, bytes_written);
				y_done = 1;	
			}

			/* extract a buffer we procured earlier */
			buf = list_entry(free_list.next,
					 struct ivtv_buffer, list);
			list_del_init(&buf->list);

			/* Check if buffer has overflown, do what though? */
			if (buf->buffer.bytesused != buf->buffer.length) {
				IVTV_DEBUG_WARN(
					   "DEC: Error, 0x%08x in Buffer.\n",
					   buf->buffer.bytesused);
			}

			bytes_written += buf->buffer.bytesused;

			/* Decoding State */
			buf->buffer.index = x;
			buf->buffer.sequence = sequence;

			/* Decoder PIO or DMA Mode */
			if (itv->dma_cfg.dec_pio) {
				/* Write using Processor IO */
				memcpy_toio((void *)
					    ((itv->dec_mem + offset)),
					    (void *)buf->buffer.m.userptr,
					    buf->buffer.bytesused);

				/* Incr decoder offset */
				offset += buf->buffer.bytesused;
				sg_size += buf->buffer.bytesused;

				ivtv_buf_fill_nolock(stream,
						     buf->buffer.bytesused,
						     BUF_SUB);

				buf->buffer.bytesused = 0;
				buf->readpos = 0;
				buf->ts = 0;

				/* Don't indicate we are doing DMA */
				stream->SG_handle = 0;

				/* Either free or requeue if min buffers */
				if (stream->buf_total <= stream->buf_min) {
					ivtv_enq_buf_nolock(&stream->free_q,
							    buf);
				} else {
					ivtv_free_buffer(itv, buf, stream);
					buf = NULL;
				}
			} else {
				/* Buffer Addr */
				stream->SGarray[x].src = buf->dma_handle;

				/* Decoder Addr */
				stream->SGarray[x].dst = offset;

				/* Buffer size */
				stream->SGarray[x].size = buf->buffer.bytesused;

				offset += buf->buffer.bytesused;

				buf->ts = jiffies;
				buf->readpos = buf->buffer.bytesused;
				buf->buffer.bytesused = 0;

				/* Check it's valid */
				if (!stream->SGarray[x].src
				    || !stream->SGarray[x].size
				    || !stream->SGarray[x].dst) {
					IVTV_DEBUG_WARN(
						   "DEC: SG Error 0x%08x 0x%08x 0x%08x\n",
						   stream->SGarray[x].src,
						   stream->SGarray[x].size,
						   stream->SGarray[x].dst);
				}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
				pci_dma_sync_single_for_device((struct pci_dev
								*)itv->dev,
							       buf->dma_handle,
							       buf->buffer.
							       length,
							       stream->dma);
#else
				pci_dma_sync_single(itv->dev,
						    buf->dma_handle,
						    buf->buffer.length,
						    stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

				/* Enqueue Buffer to DMA Queue */
				ivtv_enq_buf_nolock(&stream->dma_q, buf);
			}
		}

		/* Somehow really messed up here, never happens */
		if (bytes_received != bytes_written) {
			IVTV_DEBUG_WARN(
				   "DEC: Error, 0x%08x bytes "
				   "received but processed "
				   "0x%08x for DMA\n", bytes_received, x);
			bytes_received = bytes_written;
		}

		/* Decoder PIO or DMA Mode */
		if (!itv->dma_cfg.dec_pio) {
			/* Allocate Buffers SG List */
			stream->SG_length = x;

			IVTV_DEBUG_DMA(
				   "DEC: SGlen = 0x%08x, SG_length = 0x%08x, "
				   "bytes_written = 0x%08x\n",
				   stream->SGlen, stream->SG_length,
				   bytes_written);

			/*Set Last Element Bit */
			stream->SGarray[stream->SG_length - 1].size |=
			    0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
			/* Sync DMA */
			pci_dma_sync_single_for_device((struct pci_dev *)itv->
						       dev, stream->SG_handle,
						       sizeof(struct
							      ivtv_SG_element) *
						       (stream->buf_min /
							PAGE_SIZE),
						       stream->dma);
#else
			/* Sync DMA */
			pci_dma_sync_single(itv->dev, stream->SG_handle,
					    sizeof(struct ivtv_SG_element) *
					    (stream->buf_min / PAGE_SIZE),
					    stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */
		}

		IVTV_DEBUG_DEC(
			   "DEC: sent: "
			   "0x%08x length: 0x%08x  "
			   "fill: 0x%08x space 0x%08x <\n",
			   (mem_offset + bytes_written),
			   bytes_written,
			   (buffer_bytes + bytes_written),
			   (0x00100000 - (buffer_bytes + bytes_written)));

		/* Decoder DMA State */
		itv->dec_dma_stat.last_xfer = bytes_written;
		atomic_set(&itv->dec_dma_stat.type, 0);

		/* Decoder DMA State */
		itv->dec_dma_stat.last_addr = mem_offset;
		itv->dec_dma_stat.bytes_filled = (buffer_bytes + bytes_written);
		itv->dec_dma_stat.bytes_needed = (mem_size - bytes_written);
		itv->dec_dma_stat.total_xfer += bytes_written;
		itv->dec_dma_stat.sg_bytes = stream->buf_fill;

		IVTV_DEBUG_DEC(
			   "DEC: DMA State:\n"
			   " type: 0x%08x "
			   "last_xfer: 0x%08x "
			   "last_addr: 0x%08x\n"
			   " bytes_filled: 0x%08x "
			   "bytes_needed: 0x%08x "
			   "total_xfer: 0x%08x\n"
			   " sg_bytes: 0x%08x\n"
			   " vsync_flag_osd: %d\n"
			   " ts: %lu\n",
			   atomic_read(&itv->dec_dma_stat.type),
			   itv->dec_dma_stat.last_xfer,
			   itv->dec_dma_stat.last_addr,
			   itv->dec_dma_stat.bytes_filled,
			   itv->dec_dma_stat.bytes_needed,
			   itv->dec_dma_stat.total_xfer,
			   itv->dec_dma_stat.sg_bytes,
			   atomic_read(&itv->dec_dma_stat.vsync_flag_osd),
			   itv->dec_dma_stat.ts);

		/* Skip API with PIO mode */
		if (itv->dma_cfg.dec_pio)
			goto pio_skip;

		/* Send DMA */
		down(&itv->DMA_lock);
		if (type == IVTV_DEC_STREAM_TYPE_YUV) {
			if (dma_to_device(itv, stream, 
					stream->SG_handle, 0) != 0) 
			{
				IVTV_DEBUG_WARN(
					"Error YUV DEC DMA\n");
			}
		} else {
			if (dma_to_device(itv, stream, 
					stream->SG_handle, 0) != 0) 
			{
				IVTV_DEBUG_WARN(
					"Error MPG DEC DMA\n");
			}
		}
		up(&itv->DMA_lock);
		/* For some reason must kick the firmware, like PIO mode,
		   I think this tells the firmware we are done and the size
		   of the xfer so it can calculate what we need next.
		   I think we can do this part ourselves but would have to
		   fully calculate xfer info ourselves and not use interrupts 
		 */
		ivtv_vapi(itv, IVTV_API_DEC_DMA_FROM_HOST, 3,
			  0, bytes_written, hw_stream_type);

		/* Dequeue buffers */
		ivtv_TO_DMA_done(itv, stream->type);

		/* Unlock DMA and wakeup waitq */
		unlock_TO_dma(itv, stream->type);

	      pio_skip:
		/* Decoder PIO Mode */
		if (itv->dma_cfg.dec_pio) {
			if (type == IVTV_DEC_STREAM_TYPE_YUV) {
                        	ivtv_write_reg(0x00108080, itv->reg_mem + 0x2898);
                        	ivtv_write_reg(0x01, itv->reg_mem + 0x2800);
			}

			ivtv_vapi(itv, IVTV_API_DEC_DMA_FROM_HOST, 3,
				  0, bytes_written, hw_stream_type);

			clear_bit(IVTV_F_S_DMAP,
				  &itv->streams[stmtype].s_flags);
		}

		IVTV_DEBUG_DMA(
			   "Sched DEC dma: addr: 0x%08llx, "
			   "array_size 0x%08x, type 0x%08x\n",
			   (u64) stream->SG_handle, bytes_written,
			   hw_stream_type);
	} else if (!rc) {
		/* Failed to be needed */
		IVTV_DEBUG_DMA("Sched DEC dma: no data needed\n");

		goto exit_dec_wait;
	}

	/* Exit and don't return */
        clear_bit(IVTV_F_T_DEC_RUNNING, &itv->t_flags);
	return;

	/* Exit Conditions */
      clear_buf:
	/* Requeue if failed and stray buffers not sent */
	down(&stream->mlock);
	while (!list_empty(&free_list)) {
		buf = list_entry(free_list.next, struct ivtv_buffer, list);
		list_del_init(&buf->list);
		ivtv_enq_buf_nolock(&stream->full_q, buf);
	}
	up(&stream->mlock);
      clear_dmap:
        /* Restart DMA */
	unlock_TO_dma(itv, stream->type);
	set_bit(IVTV_F_S_NEEDS_DATA, &stream->s_flags);

	atomic_inc(&itv->dec_dma_stat.intr);
	dec_schedule_work(itv);
      exit_dec_wait:
	IVTV_DEBUG_INFO(
		   "dec_work_queue: Failed at %lu (%lu) jiffies\n",
		   jiffies, (jiffies - then));

	unlock_TO_dma(itv, stream->type);
        clear_bit(IVTV_F_T_DEC_RUNNING, &itv->t_flags);
	return;
}

int ivtv_dec_thread(void *arg)
{
	struct ivtv *itv = (struct ivtv *)arg;

#ifndef LINUX26
	daemonize();
	reparent_to_init();
	strcpy(current->comm, "ivtv-dec");
#else
	daemonize("ivtv-dec");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5)
	current->flags |= PF_NOFREEZE;
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5) */
#endif /* LINUX26 */

#ifndef SCHED_YIELD
	set_user_nice(current, -10);
#else
	current->nice = -10;
#endif /* SCHED_YIELD */

	IVTV_DEBUG_INFO("ivtv_dec_thread: pid = %d, itv = 0x%08lx\n",
		   current->pid, (unsigned long)itv);

	atomic_set(&itv->dec_dma_stat.intr, 0);

	init_waitqueue_head(&itv->dec_thread_w);

	while (atomic_read(&itv->dec_thread_running) > 0) {
		IVTV_DEBUG_INFO(
			   "decoder thread sleeping %d\n", current->pid);

		/* Wait till Ready */
		if (wait_event_interruptible(itv->dec_thread_w,
					     atomic_read(&itv->dec_dma_stat.
							 intr))) {
#ifdef SCHED_NORMAL
                        spin_lock_irq(SIGMASK_LOCK(current));
			flush_signals(current);
                        spin_unlock_irq(SIGMASK_LOCK(current));
#else
			spin_lock_irq(&current->sig->siglock);
			flush_signals(current);
			spin_unlock_irq(&current->sig->siglock);
#endif /* SCHED_NORMAL */
			continue;
		}

		atomic_dec(&itv->dec_dma_stat.intr);
		if (atomic_read(&itv->dec_thread_running) == 0)
			break;

		dec_work_handler(itv);
	}
	itv->dec_thread_pid = 0;
	return 0;
}

int ivtv_enc_thread(void *arg)
{
	struct ivtv *itv = (struct ivtv *)arg;

#ifndef LINUX26
	daemonize();
	reparent_to_init();
	strcpy(current->comm, "ivtv-enc");
#else
	daemonize("ivtv-enc");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5)
	current->flags |= PF_NOFREEZE;
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5) */
#endif /* LINUX26 */

#ifndef SCHED_YIELD
	set_user_nice(current, -10);
#else
	current->nice = -10;
#endif /* SCHED_YIELD */

	IVTV_DEBUG_INFO("ivtv_enc_thread: pid = %d, itv = 0x%08lx\n",
		   current->pid, (unsigned long)itv);

	atomic_set(&itv->enc_dma_stat_intr, 0);

	init_waitqueue_head(&itv->enc_thread_w);

	while (atomic_read(&itv->enc_thread_running) > 0) {
		IVTV_DEBUG_INFO(
			   "encoder thread sleeping %d\n", current->pid);

		/* Wait till Ready */
		if (wait_event_interruptible(itv->enc_thread_w,
					     atomic_read(&itv->
							 enc_dma_stat_intr))) {
#ifdef SCHED_NORMAL
			spin_lock_irq(SIGMASK_LOCK(current));
			flush_signals(current);
			spin_unlock_irq(SIGMASK_LOCK(current));
#else
			spin_lock_irq(&current->sig->siglock);
			flush_signals(current);
			spin_unlock_irq(&current->sig->siglock);
#endif /* SCHED_NORMAL */
			continue;
		}

		atomic_dec(&itv->enc_dma_stat_intr);
		if (atomic_read(&itv->enc_thread_running) == 0)
			break;

		enc_work_handler(itv);
	}
	itv->enc_thread_pid = 0;
	return 0;
}

int ivtv_enc_vbi_thread(void *arg)
{
        struct ivtv *itv = (struct ivtv *)arg;

#ifndef LINUX26
        daemonize();
        reparent_to_init();
        strcpy(current->comm, "ivtv-enc-vbi");
#else
        daemonize("ivtv-enc-vbi");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5)
        current->flags |= PF_NOFREEZE;
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5) */
#endif /* LINUX26 */

#ifndef SCHED_YIELD
        set_user_nice(current, -10);
#else
        current->nice = -10;
#endif /* SCHED_YIELD */

        IVTV_DEBUG_INFO("ivtv_enc_vbi_thread: pid = %d, itv = 0x%08lx\n",
                   current->pid, (unsigned long)itv);

        atomic_set(&itv->enc_vbi_dma_stat_intr, 0);

        init_waitqueue_head(&itv->enc_vbi_thread_w);

        while (atomic_read(&itv->enc_vbi_thread_running) > 0) {
                IVTV_DEBUG_INFO(
                           "encoder thread sleeping %d\n", current->pid);

                /* Wait till Ready */
                if (wait_event_interruptible(itv->enc_vbi_thread_w,
                                             atomic_read(&itv->
                                                         enc_vbi_dma_stat_intr))) {
#ifdef SCHED_NORMAL
                        spin_lock_irq(SIGMASK_LOCK(current));
                        flush_signals(current);
                        spin_unlock_irq(SIGMASK_LOCK(current));
#else
                        spin_lock_irq(&current->sig->siglock);
                        flush_signals(current);
                        spin_unlock_irq(&current->sig->siglock);
#endif /* SCHED_NORMAL */
                        continue;
                }

                atomic_dec(&itv->enc_vbi_dma_stat_intr);
                if (atomic_read(&itv->enc_vbi_thread_running) == 0)
                        break;

                enc_vbi_work_handler(itv);
        }
        itv->enc_vbi_thread_pid = 0;
        return 0;
}

int ivtv_dec_vbi_thread(void *arg)
{
        struct ivtv *itv = (struct ivtv *)arg;

#ifndef LINUX26
        daemonize();
        reparent_to_init();
        strcpy(current->comm, "ivtv-dec-vbi");
#else
        daemonize("ivtv-dec-vbi");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5)
        current->flags |= PF_NOFREEZE;
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5) */
#endif /* LINUX26 */

#ifndef SCHED_YIELD
        set_user_nice(current, -10);
#else
        current->nice = -10;
#endif /* SCHED_YIELD */

        IVTV_DEBUG_INFO("ivtv_dec_vbi_thread: pid = %d, itv = 0x%08lx\n",
                   current->pid, (unsigned long)itv);

        atomic_set(&itv->dec_dma_stat.vbi_intr, 0);

        init_waitqueue_head(&itv->dec_vbi_thread_w);

        while (atomic_read(&itv->dec_vbi_thread_running) > 0) {
                IVTV_DEBUG_INFO(
                           "encoder thread sleeping %d\n", current->pid);

                /* Wait till Ready */
                if (wait_event_interruptible(itv->dec_vbi_thread_w,
                                             atomic_read(&itv->
                                                         dec_dma_stat.vbi_intr))) {
#ifdef SCHED_NORMAL
                        spin_lock_irq(SIGMASK_LOCK(current));
                        flush_signals(current);
                        spin_unlock_irq(SIGMASK_LOCK(current));
#else
                        spin_lock_irq(&current->sig->siglock);
                        flush_signals(current);
                        spin_unlock_irq(&current->sig->siglock);
#endif /* SCHED_NORMAL */
                        continue;
                }

                atomic_dec(&itv->dec_dma_stat.vbi_intr);
                if (atomic_read(&itv->dec_vbi_thread_running) == 0)
                        break;

                dec_vbi_work_handler(itv);
        }
        itv->dec_vbi_thread_pid = 0;
        return 0;
}
