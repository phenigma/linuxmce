/*
    audio dual language watch thread and decoder thread.

    Dual language watch:
    Copyright (C) 2004  Stephane Zermatten

    Decoder thread:
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
#include "ivtv-queue.h"
#include "ivtv-streams.h"
#include "ivtv-reset.h"
#include "ivtv-mailbox.h"
#include "ivtv-i2c.h"
#include "ivtv-kthreads.h"
#include "ivtv-irq.h"
#include "ivtv-video.h"

#include <linux/smp_lock.h>

static void ivtv_dualwatch_start_kthread(struct ivtv* ivtv);
static void ivtv_dualwatch_stop_kthread(struct ivtv* ivtv);
static void ivtv_dualwatch_init_kthread(struct ivtv *ivtv);
static void ivtv_dualwatch_exit_kthread(struct ivtv *ivtv);
static void ivtv_dualwatch_set_dual(struct ivtv *itv);


/**
 * Change the DUAL flag 
 */
static void ivtv_dualwatch_set_dual(struct ivtv *itv) 
{
        u32 audio_bitmask;
        int goal_stereo_flag;
        int stereo_flag;

        if(!itv->dualwatch.encoding)
                return;

        audio_bitmask = itv->dualwatch.audio_bitmask;
        stereo_flag = audio_bitmask&IVTV_CODEC_AUDIO_MPEG_STEREO_MASK;
        if(itv->dualwatch.enabled) {
                struct v4l2_tuner vt;
                memset(&vt, 0, sizeof(vt));
                ivtv_msp34xx(itv, VIDIOC_G_TUNER, &vt);
                if(vt.rxsubchans&V4L2_TUNER_SUB_LANG2) {
                        goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_DUAL;
                } else {
                        goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO;
                }
        } else {
                if(stereo_flag==IVTV_CODEC_AUDIO_MPEG_STEREO_DUAL) {
                        goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO;
                } else {
                        goal_stereo_flag = stereo_flag;
                }
        }

        if(stereo_flag==IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO
           && ((itv->codec.audio_bitmask&IVTV_CODEC_AUDIO_MPEG_STEREO_MASK ) == IVTV_CODEC_AUDIO_MPEG_STEREO_JOINT)) {
                goal_stereo_flag = IVTV_CODEC_AUDIO_MPEG_STEREO_JOINT;
        }
        
        if(goal_stereo_flag != stereo_flag) {
                u32 new_bitmap = 
                        goal_stereo_flag 
                        | (itv->dualwatch.audio_bitmask & ~IVTV_CODEC_AUDIO_MPEG_STEREO_MASK);

                IVTV_DEBUG( IVTV_DEBUG_API, 
                            "dualwatch: change stereo flag from 0x%x to 0x%x. new audio_bitmask=0x%ux\n",
                            stereo_flag, 
                            goal_stereo_flag,
                            new_bitmap);
                
                if (ivtv_vapi(itv, IVTV_API_ASSIGN_AUDIO_PROPERTIES, 1, new_bitmap) == 0) {
                        itv->dualwatch.audio_bitmask = new_bitmap;
                } else {
                        IVTV_DEBUG( IVTV_DEBUG_API, 
                                    "dualwatch: changing stereo flag failed\n");
                }
        }
}

/* start the thread if necessary */
static void ivtv_dualwatch_update(struct ivtv* itv)
{
        ivtv_dualwatch_set_dual(itv);

        /* start timer, but only if necessary (timer stops automatically) */
        if(itv->dualwatch.encoding 
           && itv->dualwatch.enabled) {
                if(!itv->dualwatch.thread)
                        ivtv_dualwatch_start_kthread(itv);
        } else {
                if(itv->dualwatch.thread)
                        ivtv_dualwatch_stop_kthread(itv);
        }
}
/* enable/disable dualwatch (= enable/disable full stereo (not lang1/lang2 or mono) mode) */
void ivtv_dualwatch_set_enabled(struct ivtv* itv, int enabled)
{
        itv->dualwatch.enabled=enabled;
        ivtv_dualwatch_update(itv);
}

/* start encoding => start the thread if DUALWATCH is enabled */
void ivtv_dualwatch_start_encoding(struct ivtv* itv)
{
        if(itv->dualwatch.encoding)
                return;
        itv->dualwatch.encoding = 1;
        itv->dualwatch.audio_bitmask = itv->codec.audio_bitmask;
        ivtv_dualwatch_update(itv);
}

/* stop encoding => stop the thread if DUALWATCH is enabled */
void ivtv_dualwatch_stop_encoding(struct ivtv* itv)
{
        if(!itv->dualwatch.encoding)
                return;
        if(itv->dualwatch.thread)
                ivtv_dualwatch_stop_kthread(itv);
        itv->dualwatch.encoding = 0; /* after stopping the thread */
}

/* function executed on the thread */
static int ivtv_dualwatch_kthread(void *ptr) {
   struct ivtv* ivtv = (struct ivtv*)ptr;
   struct ivtv_dualwatch *kthread = &ivtv->dualwatch;

   ivtv_dualwatch_init_kthread(ivtv);

   for(;;) {
           interruptible_sleep_on_timeout(&kthread->queue, HZ);
           
           mb();
           
           if (kthread->terminate)
                   break;    
           
           ivtv_dualwatch_set_dual(ivtv);
   }
   ivtv_dualwatch_exit_kthread(ivtv);
   return 0;
}

static void ivtv_dualwatch_start_kthread(struct ivtv* ivtv) {
        /* adapted from example by Martin Frey 
         * http://www.scs.ch/~frey/linux/kernelthreads.html
         */
        struct ivtv_dualwatch *kthread = &ivtv->dualwatch;
        
        if(!kthread->thread) {
                init_MUTEX_LOCKED(&kthread->startstop_sem);
                kernel_thread(ivtv_dualwatch_kthread, ivtv, 0);
                down(&kthread->startstop_sem);
        }
}

static void ivtv_dualwatch_stop_kthread(struct ivtv* ivtv) {
        struct ivtv_dualwatch *kthread = &ivtv->dualwatch;
        
        if (!kthread->thread || kthread->terminate)
                return;
        /* this function needs to be protected with the big
           kernel lock (lock_kernel()). The lock must be
           grabbed before changing the terminate
           flag and released after the down() call. */
        lock_kernel();
        if(kthread->thread) { /* paranoid mode */
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
#endif

        /* lock the kernel. A new kernel thread starts without
           the big kernel lock, regardless of the lock state
           of the creator (the lock level is *not* inheritated)
        */
        lock_kernel();

        /* fill in thread structure */
        kthread->thread = current;
        
        siginitsetinv(&current->blocked, sigmask(SIGKILL)|sigmask(SIGINT)|sigmask(SIGTERM));
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

void fw_work_handler(void *arg) {
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

	if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))) {
		if (st->buf_fill >= (1024*1024) && bytes_needed >= (1024*1024)) 
			bytes_needed = (1024*1024);
		else if (st->buf_fill >= (512*1024) && bytes_needed >= (512*1024)) 
			bytes_needed = (512*1024);
		else if (st->buf_fill >= (256*1024) && bytes_needed >= (256*1024))
			bytes_needed = (256*1024);
		else if (st->buf_fill >= (128*1024) && bytes_needed >= (128*1024))
			bytes_needed = (128*1024);
		else if (st->buf_fill >= (64*1024) && bytes_needed >= (64*1024))
			bytes_needed = (64*1024);
		else if (st->buf_fill >= (32*1024) && bytes_needed >= (32*1024))
			bytes_needed = (32*1024);
	}

        /* gather the needed buffers first, so we don't have to bail
         * in mid-air. put them on a list on the stack */
        for (x = 0; (bytes_received < bytes_needed); x++) {
                buf = ivtv_deq_buf_nolock(&st->full_q, streamtype);
                if (!buf)
                        break;

		bytes_received += buf->buffer.bytesused;

                list_add_tail(&buf->list, free_list);
        }

	IVTV_DEBUG(IVTV_DEBUG_DMA, 
		"DEC: x=%d bytes_needed 0x%08x buf_fill 0x%08x\n",
		x, bytes_needed, st->buf_fill);

        /* damn, failed */
        if (bytes_received < itv->dma_cfg.dec_min_xfer) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "DEC: needed %d bytes for stream %d, "
			"received %d differ by (%d) bytes\n",
                        bytes_needed, streamtype, bytes_received,
                        (bytes_needed - bytes_received));

                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "DEC: free_q: %d elements\n",
                        atomic_read(&st->free_q.elements));
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "DEC: dma_q: %d elements\n",
                        atomic_read(&st->dma_q.elements));
                IVTV_DEBUG(IVTV_DEBUG_ERR,
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

void enc_work_handler(void *arg) {
	struct ivtv *itv = (struct ivtv *)arg;

        /* VIDEO or VBI */
        do {
                if (test_and_clear_bit(IVTV_F_T_ENC_DMA_NEEDED, &itv->t_flags)) {
                        ivtv_sched_DMA(itv);
                }
                if (test_and_clear_bit(IVTV_F_T_ENC_VBI_DMA, &itv->t_flags)) {
                        ivtv_sched_VBI(itv, IVTV_ENC_STREAM_TYPE_VBI);
                }
                if (test_and_clear_bit(IVTV_F_T_DEC_VBI_INSERT, &itv->t_flags)) {
                        ivtv_sched_VBI(itv, IVTV_DEC_STREAM_TYPE_VBI);
                }
        } while (test_bit(IVTV_F_T_DEC_VBI_INSERT, &itv->t_flags) ||
        	 test_bit(IVTV_F_T_ENC_DMA_NEEDED, &itv->t_flags) ||
                 test_bit(IVTV_F_T_ENC_VBI_DMA, &itv->t_flags));

	return;
}

void dec_work_handler(void *arg) {
        struct ivtv *itv = (struct ivtv *)arg;
        int type = IVTV_DEC_STREAM_TYPE_MPG;
        struct ivtv_stream *stream= &itv->streams[type];
	int ret = -1;
	unsigned long then = 0;	
	int rc = 0;
        int x=0, bytes_written=0;
        struct ivtv_buffer *buf;
        u32 data[IVTV_MBOX_MAX_DATA], result;
        u32 mem_offset, mem_size, orig_mem_size, hw_stream_type, buffer_bytes;
        int stmtype = type;
	u32 bytes_needed = 0;
        u32 bytes_received;
        LIST_HEAD(free_list);
	long sequence;
	u32 offset, sg_size = 0;
	int retrys;
        DECLARE_WAITQUEUE(wait, current);
	int redo_dma = 0;
	int max_xfer = itv->dma_cfg.dec_max_xfer;
	unsigned long flags;

	/* YUV or MPG Stream? */
	if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) {
		type = IVTV_DEC_STREAM_TYPE_YUV;
	} else {
		type = IVTV_DEC_STREAM_TYPE_MPG;
	}
       	stream= &itv->streams[type];

        /* Wait for FW Reload */
        if (test_bit(FW_RESET_LOCK, &itv->r_flags)) {
                retrys = 500;

                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "ivtv_write: Sleeping for FW Reset");

                /* Check FW Reset Status  */
                while (test_bit(FW_RESET_LOCK, &itv->r_flags)) {
                        ivtv_sleep_timeout(HZ/100, 0);
                        if (retrys-- == 0)
                                break;
                }
        }

      	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"dec_work_queue: Schedule DMA %lu\n", jiffies);

	ret = ivtv_api_getresult_nosleep(&itv->dec_mbox[9], 
		&result, &data[0]);

      	/* Did xfer status fail? */
       	if (ret) {
               	IVTV_DEBUG(IVTV_DEBUG_ERR,
                       	"DEC 1st DMA Xfer Info failed=0x%08x %lu\n",
               		ret, jiffies);

               	goto exit_dec_wait;
       	}

	data[2] = ivtv_round_dma(data[2]);

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
			(ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) & 0x01) &&
                        ! (ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) & 0x14) &&
			!test_and_set_bit(IVTV_F_S_DMAP, &stream->s_flags)) 
		{
                       		break;
               	}

		if (ivtv_sleep_timeout(HZ/100, 1)) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: User stopped waitq.\n");

                        rc = -EINTR;
			break;
		}

		if (test_bit(IVTV_F_S_STREAMOFF, &stream->s_flags)) {
       			set_current_state(TASK_RUNNING);
       			remove_wait_queue(&stream->waitq, &wait);
			goto clear_dmap;
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
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: start_decode, error starting\n");
                        goto stop_dec;
                }
		goto clear_dmap;
        }

	/* Is the decoder in need of data */
	if (test_and_clear_bit(IVTV_F_S_NEEDS_DATA, &stream->s_flags)) {
		if (!itv->dma_cfg.dec_pio) {
			/* Unmap Last DMA, still a bug if we have to here */
                	if (atomic_read(&stream->dma_q.elements) > 0 &&
				!ivtv_dec_DMA_done(itv, stream)) 
			{
                		IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"DEC: Can't free %d buffers in DMA\n",
                         		atomic_read(&stream->dma_q.elements));
			}
		}

		ret = ivtv_api_getresult_nosleep(&itv->dec_mbox[9], 
			&result, &data[0]);

               	/* Did xfer status fail? */
               	if (ret) {
                       	IVTV_DEBUG(IVTV_DEBUG_ERR,
                               	"DEC 2nd DMA Xfer Info failed=0x%08x %lu\n",
                       		ret, jiffies);

                       	goto clear_dmap;
               	}
		data[2] = ivtv_round_dma(data[2]);

                /* Wait till data input is ready */
		then = jiffies;
                while (ivtv_buf_fill_lock(stream, 0, BUF_USED) < data[2]) {

                	if (test_bit(IVTV_F_S_STREAMOFF, &stream->s_flags)) {
                        	goto clear_dmap;
                	}

			ivtv_sleep_timeout(HZ/10, 1);
			if ((jiffies - then) > (HZ*2)) {
                        	IVTV_DEBUG(IVTV_DEBUG_INFO,
                                "DEC: Only 0x%08x bytes available need %d.\n",
					ivtv_buf_fill_lock(stream, 0, 
					BUF_USED), data[2]);
                        	goto clear_dmap;
			}
                }

                /* Get DMA Xfer Info to know what to send */
		ret = ivtv_api_getresult_nosleep(&itv->dec_mbox[9], 
			&result, &data[0]);

               	/* Did xfer status fail? */
               	if (ret) {
                       	IVTV_DEBUG(IVTV_DEBUG_ERR,
                               	"DEC DMA Xfer Info failed=0x%08x %lu\n",
                       		ret, jiffies);

                       	goto clear_dmap;
               	}
		/* Does it want enough */
		if (data[2] < itv->dma_cfg.dec_min_xfer) {
                       	IVTV_DEBUG(IVTV_DEBUG_INFO,
                        	"DEC: Not enough data needed "
				"by decoder 0x%08x\n",
				data[2]);

			goto clear_dmap;
		}
                IVTV_DEBUG(IVTV_DEBUG_INFO,
                   "DEC: Mailbox 10: 0x%08x 0x%08x 0x%08x 0x%08x\n",
                        data[0],data[1],data[2],data[3]);

                /* Get card mem addr and size from data array */
                hw_stream_type = data[0];
               	mem_offset = data[1];
                mem_size = orig_mem_size = data[2];
                buffer_bytes = data[3]; /* # bytes in card's buffer */

                /* Not ready yet? */
                if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) &&
			buffer_bytes < stream->buf_max &&
                        (stream->buf_max - buffer_bytes) <
                                itv->dma_cfg.dec_min_xfer)
                {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: Wants more data 0x%08x left 0x%08x\n",
                                itv->dec_dma_stat.data[1],
                                (orig_mem_size - bytes_written));

			goto clear_dmap;
		}

		if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) &&
			(mem_offset+itv->dma_cfg.dec_min_xfer) > 0x0050fb00) 
		{
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                             "DEC: Sched Buffer end reached 0x%08x\n",
                             ((mem_offset+itv->dma_cfg.dec_min_xfer)));

                	IVTV_DEBUG(IVTV_DEBUG_ERR,
                   		" DEC: Mailbox 10: "
				"0x%08x 0x%08x 0x%08x 0x%08x\n",
                        	data[0],data[1],data[2],data[3]);

			goto clear_dmap;
		}

                /* Destination Offset */
		if (!itv->dma_cfg.dec_pio)
                	offset = (mem_offset + IVTV_DECODER_OFFSET);
		else
                	offset = mem_offset;

        	/* Invalid DMA Xfer Type */
        	if (hw_stream_type != 0 && hw_stream_type != 2) {
                	IVTV_DEBUG(IVTV_DEBUG_ERR,
                        	"DEC: Decoder Invalid type 0x%08x?\n", 
				hw_stream_type);
                	goto clear_dmap;
        	}

        	/* Check if size request is too big for decoder buffer */
        	if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) &&
			 ((mem_size)+buffer_bytes) > IVTV_DEC_MAX_BUF)
        	{
                	IVTV_DEBUG(IVTV_DEBUG_INFO,
                        	"DEC: wants 0x%08x has 0x%08x = 0x%08x!\n",
                        	mem_size, buffer_bytes, 
				((mem_size)+buffer_bytes));
                	mem_size = (IVTV_DEC_MAX_BUF - buffer_bytes);
                	IVTV_DEBUG(IVTV_DEBUG_INFO,
                        	"DEC: Changed mem_size to 0x%08x.\n", mem_size);
        	}

		if (type == IVTV_DEC_STREAM_TYPE_YUV) {
			offset += IVTV_YUV_BUFFER_OFFSET;
			mem_offset = IVTV_YUV_BUFFER_OFFSET;
		}

                /* Get minimal buffers needed */
		if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))) {
                	bytes_needed = itv->dma_cfg.dec_min_xfer;
			if (mem_size > itv->dma_cfg.dec_max_xfer)
				mem_size = itv->dma_cfg.dec_max_xfer;

			mem_size = ivtv_round_dma(mem_size);
		}

                /* Manually calculate if decoder isn't updating */
                if ((type == IVTV_DEC_STREAM_TYPE_MPG) &&
                        mem_offset < 0x0050fb00 &&
                        (mem_offset + mem_size) > 0x0050fb00)
                {
                        goto clear_dmap;
                }
                if ((type == IVTV_DEC_STREAM_TYPE_MPG) &&
                        ((mem_offset < 0x0040fb00) ||
                        (mem_offset + mem_size) > 0x0050fb00))
                {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: Decoder wants 0x%08x bytes, "
                                "bad memory location 0x%08x.\n",
                                mem_size, (mem_offset + mem_size));

                        goto clear_dmap;
                }

		/* YUV can xfer as much as needed */
		if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))
			max_xfer = mem_size;

		/* Gather enough buffers */
		down(&stream->mlock);
        	bytes_received =
                	gather_free_buffers(itv, 
				type, &free_list, max_xfer);
		up(&stream->mlock);

		/* Did it fail, shouldn't ever */	
		if (bytes_received < bytes_needed) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: now %d Dec buffers left, were %d left.\n",
				bytes_received, bytes_needed);

                        goto clear_buf;
		} 

		if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) &&
			bytes_received > itv->dma_cfg.dec_max_xfer) 
		{
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: got %d Dec buffers, way too many\n",
				bytes_received);
			bytes_received = 
				itv->dma_cfg.dec_max_xfer;
		} else if (!(test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags)) &&
			bytes_received < itv->dma_cfg.dec_min_xfer) 
		{
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: got %d Dec buffers, way too few\n",
				bytes_received);

                        goto clear_buf;
		}

		/* translate buffers back into mem_size */
		mem_size = bytes_received;

        	IVTV_DEBUG(IVTV_DEBUG_DEC,
                	"DEC: want: "
                	"0x%08x  length: 0x%08x  full: 0x%08x  space 0x%08x\n",
                	mem_offset, orig_mem_size, buffer_bytes,
                	(0x00100000-buffer_bytes));

		/* increment the sequence # */
        	sequence = ++stream->seq;

                itv->dec_dma_stat.wq_runs++;
                IVTV_DEBUG(IVTV_DEBUG_DEC, 
			" (%lu) DEC WRQ: intr %d wq_runs %d\n",
			jiffies,
                        atomic_read(&itv->dec_dma_stat.intr),
                        itv->dec_dma_stat.wq_runs);

                /* Send next DMA up to max buffers */
		down(&stream->mlock);
                for (x = 0; !list_empty(&free_list); x++) 
		{
			if (bytes_written >= mem_size) {
				IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        "DEC: Error, No more Buffers!!!.\n");
                                break;
                        }

                	/* extract the buffers we procured earlier */
                	buf = list_entry(free_list.next, 
				struct ivtv_buffer, list);
                	list_del_init(&buf->list);

			/* Check if buffer has overflown, do what though? */
                        if (buf->buffer.bytesused != buf->buffer.length) {
                                IVTV_DEBUG(IVTV_DEBUG_ERR,
                                        "DEC: Error, 0x%08x in Buffer.\n",
                                        buf->buffer.bytesused);
                        }
        
                        bytes_written += buf->buffer.bytesused;

			/* Decoding State */
			buf->buffer.index 	= x;
			buf->buffer.sequence    = sequence;

			/* Decoder DMA Mode */
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
					buf->buffer.bytesused, BUF_SUB);

                        	buf->buffer.bytesused   = 0;
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

				buf->ts 		= jiffies;
                        	buf->readpos            = buf->buffer.bytesused;
                        	buf->buffer.bytesused   = 0;

				/* Check it's valid */
				if (!stream->SGarray[x].src || !stream->SGarray[x].size ||
					!stream->SGarray[x].dst)
				{
                        		IVTV_DEBUG(IVTV_DEBUG_ERR, 
						"DEC: SG Error 0x%08x 0x%08x 0x%08x\n",
						stream->SGarray[x].src,
						stream->SGarray[x].size,
						stream->SGarray[x].dst);
				}
        
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
                       		pci_dma_sync_single_for_device((struct pci_dev*)itv->dev, 
					 buf->dma_handle, buf->buffer.length,
                               			stream->dma);
#else
                       		pci_dma_sync_single(itv->dev, 
					 buf->dma_handle, buf->buffer.length,
                               			stream->dma);
#endif

				/* Enqueue Buffer to DMA Queue */	
                        	ivtv_enq_buf_nolock(&stream->dma_q, buf);
			} 
                }
		up(&stream->mlock);

		if (bytes_received != bytes_written) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "DEC: Error, 0x%08x bytes "
				"received but processed "
				"0x%08x for DMA\n",
                                bytes_received, x);
			bytes_received = bytes_written;
		}

		/* Decoder DMA Mode */
		if (!itv->dma_cfg.dec_pio) {
                	/* Allocate Buffers SG List */
                	stream->SG_length = x;

                	IVTV_DEBUG(IVTV_DEBUG_DMA,
                        	"DEC: SGlen = 0x%08x, SG_length = 0x%08x, "
                        	"bytes_written = 0x%08x\n",
                        	stream->SGlen, stream->SG_length, 
				bytes_written);

                	/*Set Last Element Bit*/
                	stream->SGarray[stream->SG_length-1].size |= 0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
                	/* Sync DMA */
                       	pci_dma_sync_single_for_device((struct pci_dev*)itv->dev, stream->SG_handle,
                               	sizeof(struct ivtv_SG_element) * 
					(stream->buf_max/PAGE_SIZE),
                               		stream->dma);
#else
                	/* Sync DMA */
                       	pci_dma_sync_single(itv->dev, stream->SG_handle,
                               	sizeof(struct ivtv_SG_element) * 
					(stream->buf_max/PAGE_SIZE),
                               		stream->dma);
#endif
		}

		IVTV_DEBUG(IVTV_DEBUG_DEC,
			"DEC: sent: "
			"0x%08x length: 0x%08x  "
			"fill: 0x%08x space 0x%08x <\n",
			(mem_offset+bytes_written),
			bytes_written, 
			(buffer_bytes+bytes_written),
			(0x00100000-(buffer_bytes+bytes_written)));

        	/* Decoder DMA State */
        	itv->dec_dma_stat.last_xfer = bytes_written;
        	atomic_set(&itv->dec_dma_stat.type, 0);

        	/* Decoder DMA State */
        	itv->dec_dma_stat.last_addr     = mem_offset;
        	itv->dec_dma_stat.bytes_filled  = (buffer_bytes+bytes_written);
        	itv->dec_dma_stat.bytes_needed  = (mem_size-bytes_written);
        	itv->dec_dma_stat.total_xfer    += bytes_written;
        	itv->dec_dma_stat.sg_bytes      = stream->buf_fill;

		IVTV_DEBUG(IVTV_DEBUG_DEC,
			"DEC: DMA State:\n"
			" type: 0x%08x "
			"last_xfer: 0x%08x "
			"last_addr: 0x%08x\n"
			" bytes_filled: 0x%08x "
			"bytes_needed: 0x%08x "
			"total_xfer: 0x%08x\n"
			" sg_bytes: 0x%08x\n"
			" vsync_count: %lu\n"
			" ts: %u\n",
			atomic_read(&itv->dec_dma_stat.type),
			itv->dec_dma_stat.last_xfer,
			itv->dec_dma_stat.last_addr,
			itv->dec_dma_stat.bytes_filled,
			itv->dec_dma_stat.bytes_needed,
			itv->dec_dma_stat.total_xfer,
			itv->dec_dma_stat.sg_bytes,
			itv->dec_dma_stat.vsync_count,
			itv->dec_dma_stat.ts);  

		/* Check for YUV */
		if (test_bit(IVTV_F_I_DEC_YUV, &itv->i_flags))
			hw_stream_type = 2;

		/* Skip API with PIO mode */
		if (itv->dma_cfg.dec_pio)
			goto pio_skip;

/* This uses the cards DMA registers instead of the firmware API
   to send the Decoder DMA, I suspect this is safer than how the
   firmware does it, just because the firmware does it so badly :-) */
		down(&itv->DMA_lock);

        	/* wait for DMA complete status */
        	then = jiffies;
        	while (!(ivtv_read_reg(itv->reg_mem + 
				IVTV_REG_DMASTATUS) & 0x00000001))
        	{
                	ivtv_sleep_timeout(HZ/100, 1);
                	if ((jiffies - then) > (HZ*3)) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"DEC: REG_DMASTATUS wait failed\n"); 

				ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        	break;
			}
        	}

redo_dma:
		/* put SG Handle into register 0x08 */
		spin_lock_irqsave(&itv->DMA_slock, flags);
		ivtv_write_reg(stream->SG_handle, 
			itv->reg_mem + IVTV_REG_DECDMAADDR);

		/* Send DMA with register 0x00, using the dec DMA bit */
		ivtv_write_reg(0x01, itv->reg_mem + IVTV_REG_DMAXFER);
		spin_unlock_irqrestore(&itv->DMA_slock, flags);

        	/* wait for DMA to be finished, check interrupt bit */
        	then = jiffies;
        	while (!(ivtv_read_reg(itv->reg_mem + 
				IVTV_REG_DECSG1LEN) & 0x80000000))
        	{
			/* DMA Error */
			if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000014)
				break;

                	ivtv_sleep_timeout(HZ/100, 1);
                	if ((jiffies - then) > HZ) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"DEC: REG_DECSG1LEN wait failed\n"); 

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
				IVTV_REG_DMAXFER) & 0x00000001))
        	{
			/* DMA Error */
			if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000014)
				break;

                	ivtv_sleep_timeout(HZ/100, 1);
                	if ((jiffies - then) > (HZ*3)) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"DEC: REG_DMAXFER wait failed\n"); 

				ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        	break;
			}
        	}

        	/* wait for DMA complete status */
        	then = jiffies;
        	while (!(ivtv_read_reg(itv->reg_mem + 
				IVTV_REG_DMASTATUS) & 0x00000001))
        	{
			/* DMA Error */
			if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000014)
				break;

                	ivtv_sleep_timeout(HZ/100, 1);
                	if ((jiffies - then) > (HZ*3)) {
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"DEC: REG_DMASTATUS2 wait failed\n"); 

				ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        	break;
			}
        	}

		/* DMA Error */
		if ((ivtv_read_reg(itv->reg_mem +
                                IVTV_REG_DMASTATUS) & 0x00000014))
		{
                	IVTV_DEBUG(IVTV_DEBUG_DMA, "DEC: (%d) DMA Error 0x%08x\n",
                        	redo_dma, ivtv_read_reg(itv->reg_mem +IVTV_REG_DMASTATUS));

			/* Reset DMA Error, cancel last DMA? */
			spin_lock_irqsave(&itv->DMA_slock, flags);
			ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
			spin_unlock_irqrestore(&itv->DMA_slock, flags); 
			if (redo_dma < 3) {
				redo_dma++;
				goto redo_dma;
			} else
				IVTV_DEBUG(IVTV_DEBUG_ERR, 
					"DEC: REDO DMA took too many tries.\n"); 
		}

        	/* For some reason must kick the firmware, like PIO mode,
		   I think this tells the firmware we are done and the size
                   of the xfer so it can calculate what we need next.
                   I think we can do this part ourselves but would have to
                   fully calculate xfer info ourselves and not use interrupts 
		*/
        	ivtv_vapi(itv, IVTV_API_DEC_DMA_FROM_HOST, 3,
                	0, bytes_written, hw_stream_type);

		/* Dequeue buffers */
		ivtv_dec_DMA_done(itv, stream);

		/* Unlock DEC DMA */
		up(&itv->DMA_lock);

        	/* Unlock stream */
        	clear_bit(IVTV_F_S_DMAP, &stream->s_flags);

        	/* Wake up next OSD/DEC DMA in wait */
        	wake_up(&stream->waitq);
pio_skip:
		/* Decoder PIO Mode */
		if (itv->dma_cfg.dec_pio) {
        		ivtv_vapi(itv, IVTV_API_DEC_DMA_FROM_HOST, 3,
                		0, bytes_written, hw_stream_type);
        	
			clear_bit(IVTV_F_S_DMAP, 
				&itv->streams[stmtype].s_flags);
		}

        	IVTV_DEBUG(IVTV_DEBUG_INFO,
                   	"Sched DEC dma: addr: 0x%08llx, "
			"array_size 0x%08x, type 0x%08x\n",
			(u64)stream->SG_handle, bytes_written, hw_stream_type);

      		IVTV_DEBUG(IVTV_DEBUG_INFO,
        		"dec_work_queue: DMA Sched at %lu (%lu) jiffies\n", 
			jiffies, (jiffies - then));

	} else if (!rc) {
		/* Failed to be needed */
		IVTV_DEBUG(IVTV_DEBUG_INFO, "Sched DEC dma: no data needed\n");

		goto stop_dec;
	}

	return;
clear_buf:	
		/* Requeue if failed and stray buffers not sent */	
		down(&stream->mlock);
                while (!list_empty(&free_list)) {
                        buf = list_entry(free_list.next,
                                struct ivtv_buffer, list);
                        list_del_init(&buf->list);
                        ivtv_enq_buf_nolock(&stream->full_q, buf);
                }
		up(&stream->mlock);
clear_dmap:	

		/* Stop Decoding */
        	if (test_and_clear_bit(IVTV_F_S_STREAMOFF, &stream->s_flags)) {
			IVTV_DEBUG(IVTV_DEBUG_INFO,
				"dec_work_queue: Stop Decode.\n"); 

                        ivtv_vapi(itv, IVTV_API_DEC_STOP_PLAYBACK, 3,
                        	/*  0 = last frame, 1 = black */
                        	itv->dec_options.hide_last_frame,
                        	itv->dec_options.pts_low, /* when: pts low */
                        	itv->dec_options.pts_hi);  /* when: pts hi */
        	} else {
			ivtv_sleep_timeout(HZ/100, 1);

               		set_bit(IVTV_F_S_NEEDS_DATA, &stream->s_flags);
			clear_bit(IVTV_F_S_DMAP, &stream->s_flags);

			atomic_inc(&itv->enc_dma_stat_intr);
			dec_schedule_work(itv);
		}
exit_dec_wait:	
      	IVTV_DEBUG(IVTV_DEBUG_INFO,
        	"dec_work_queue: Failed at %lu (%lu) jiffies\n", 
		jiffies, (jiffies - then));

	return;
stop_dec:	
	clear_bit(IVTV_F_S_DMAP, &stream->s_flags);

        if (test_bit(IVTV_F_S_STREAMOFF, &stream->s_flags)) {
                goto clear_dmap;
        }

	return;
}


int ivtv_dec_thread(void *arg) {
        struct ivtv *itv = (struct ivtv *)arg;

#ifndef LINUX26
        daemonize();
        reparent_to_init();
        strcpy(current->comm, "ivtv-dec");
#else
        daemonize("ivtv-dec");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5)
        current->flags |= PF_NOFREEZE;
#endif
#endif

#ifndef SCHED_YIELD
        set_user_nice(current, -10);
#else
	current->nice = -10;
#endif

        IVTV_DEBUG(IVTV_DEBUG_ERR, "ivtv_dec_thread: pid = %d, itv = 0x%08lx\n",
             current->pid, (unsigned long)itv);

	atomic_set(&itv->dec_dma_stat.intr, 0);

	init_waitqueue_head(&itv->dec_thread_w);

        while (atomic_read(&itv->dec_thread_running) > 0) {
                IVTV_DEBUG(IVTV_DEBUG_INFO,
                        "decoder thread sleeping %d\n", current->pid);          

                /* Wait till Ready */
                if (wait_event_interruptible(itv->dec_thread_w,
                       atomic_read(&itv->dec_dma_stat.intr)))
                {
#ifndef SCHED_YIELD
                       spin_lock_irq(&current->sighand->siglock);
                       flush_signals(current);
                       spin_unlock_irq(&current->sighand->siglock);
#else
                       spin_lock_irq(&current->sig->siglock);
                       flush_signals(current);
                       spin_unlock_irq(&current->sig->siglock);
#endif
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

int ivtv_enc_thread(void *arg) {
        struct ivtv *itv = (struct ivtv *)arg;

#ifndef LINUX26
        daemonize();
        reparent_to_init();
        strcpy(current->comm, "ivtv-enc");
#else
        daemonize("ivtv-enc");
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 5)
        current->flags |= PF_NOFREEZE;
#endif
#endif

#ifdef SCHED_NORMAL
        set_user_nice(current, -10);
#else
        current->nice = -10;
#endif

        IVTV_DEBUG(IVTV_DEBUG_ERR, "ivtv_enc_thread: pid = %d, itv = 0x%08lx\n",
             current->pid, (unsigned long)itv);

	atomic_set(&itv->enc_dma_stat_intr, 0);

	init_waitqueue_head(&itv->enc_thread_w);

        while (atomic_read(&itv->enc_thread_running) > 0) {
                IVTV_DEBUG(IVTV_DEBUG_INFO,
                        "encoder thread sleeping %d\n", current->pid);

                /* Wait till Ready */
                if (wait_event_interruptible(itv->enc_thread_w,
                       atomic_read(&itv->enc_dma_stat_intr)))
                {
#ifdef SCHED_NORMAL
                       spin_lock_irq(&current->sighand->siglock);
                       flush_signals(current);
                       spin_unlock_irq(&current->sighand->siglock);
#else
                       spin_lock_irq(&current->sig->siglock);
                       flush_signals(current);
                       spin_unlock_irq(&current->sig->siglock);
#endif
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

