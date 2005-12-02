/*
 *
 *  $Id: pvrusb2-ioread.c,v 1.1 2005/11/14 13:31:24 mchehab Exp $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "pvrusb2-ioread.h"
#include "pvrusb2-debug.h"
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>

#define BUFFER_COUNT 32
#define BUFFER_SIZE PAGE_ALIGN(0x4000)

struct pvr2_ioread {
	struct pvr2_stream *stream;
	char *buffer_storage[BUFFER_COUNT];
	int resid_offs;
	int enabled;
	int stream_running;
	int spigot_open;
	struct semaphore sem;
};

static int pvr2_ioread_init(struct pvr2_ioread *cp)
{
	unsigned int idx;

	cp->stream = 0;
	init_MUTEX(&cp->sem);

	for (idx = 0; idx < BUFFER_COUNT; idx++) {
		cp->buffer_storage[idx] = kmalloc(BUFFER_SIZE,GFP_KERNEL);
		if (!(cp->buffer_storage[idx])) break;
	}

	if (idx < BUFFER_COUNT) {
		// An allocation appears to have failed
		for (idx = 0; idx < BUFFER_COUNT; idx++) {
			if (!(cp->buffer_storage[idx])) continue;
			kfree(cp->buffer_storage[idx]);
		}
		return -ENOMEM;
	}
	return 0;
}

static void pvr2_ioread_done(struct pvr2_ioread *cp)
{
	unsigned int idx;

	pvr2_ioread_setup(cp,0);
	for (idx = 0; idx < BUFFER_COUNT; idx++) {
		if (!(cp->buffer_storage[idx])) continue;
		kfree(cp->buffer_storage[idx]);
	}
}

struct pvr2_ioread *pvr2_ioread_create(void)
{
	struct pvr2_ioread *cp;
	cp = kmalloc(sizeof(*cp),GFP_KERNEL);
	if (!cp) return 0;
	pvr2_trace(PVR2_TRACE_STRUCT,"pvr2_ioread_create id=%p",cp);
	memset(cp,0,sizeof(*cp));
	if (pvr2_ioread_init(cp) < 0) {
		kfree(cp);
		return 0;
	}
	return cp;
}

void pvr2_ioread_destroy(struct pvr2_ioread *cp)
{
	if (!cp) return;
	pvr2_ioread_done(cp);
	pvr2_trace(PVR2_TRACE_STRUCT,"pvr2_ioread_destroy id=%p",cp);
	kfree(cp);
}

static void pvr2_ioread_stop(struct pvr2_ioread *cp)
{
	if (!(cp->enabled)) return;
	pvr2_trace(PVR2_TRACE_START_STOP,
		   "/*---TRACE_READ---*/ pvr2_ioread_stop id=%p",cp);
	pvr2_stream_kill(cp->stream);
	cp->resid_offs = 0;
	cp->enabled = 0;
	cp->stream_running = 0;
	cp->spigot_open = 0;
}

static int pvr2_ioread_start(struct pvr2_ioread *cp)
{
	int stat;
	struct pvr2_buffer *bp;
	if (cp->enabled) return 0;
	if (!(cp->stream)) return 0;
	pvr2_trace(PVR2_TRACE_START_STOP,
		   "/*---TRACE_READ---*/ pvr2_ioread_start id=%p",cp);
	cp->resid_offs = 0;
	while ((bp = pvr2_stream_get_idle_buffer(cp->stream)) != 0) {
		stat = pvr2_buffer_queue(bp);
		if (stat < 0) {
			pvr2_trace(PVR2_TRACE_DATA_FLOW,
				   "/*---TRACE_READ---*/"
				   " pvr2_ioread_start id=%p"
				   " error=%d",
				   cp,stat);
			pvr2_ioread_stop(cp);
			return stat;
		}
	}
	cp->enabled = !0;
	cp->stream_running = 0;
	cp->spigot_open = 0;
	return 0;
}

struct pvr2_stream *pvr2_ioread_get_stream(struct pvr2_ioread *cp)
{
	return cp->stream;
}

int pvr2_ioread_setup(struct pvr2_ioread *cp,struct pvr2_stream *sp)
{
	int ret;
	unsigned int idx;
	struct pvr2_buffer *bp;

	down(&cp->sem); do {
		if (cp->stream) {
			pvr2_trace(PVR2_TRACE_START_STOP,
				   "/*---TRACE_READ---*/"
				   " pvr2_ioread_setup (tear-down) id=%p",cp);
			pvr2_ioread_stop(cp);
			pvr2_stream_kill(cp->stream);
			pvr2_stream_set_buffer_count(cp->stream,0);
			cp->stream = 0;
		}
		if (sp) {
			pvr2_trace(PVR2_TRACE_START_STOP,
				   "/*---TRACE_READ---*/"
				   " pvr2_ioread_setup (setup) id=%p",cp);
			pvr2_stream_kill(sp);
			ret = pvr2_stream_set_buffer_count(sp,BUFFER_COUNT);
			if (ret < 0) return ret;
			for (idx = 0; idx < BUFFER_COUNT; idx++) {
				bp = pvr2_stream_get_buffer(sp,idx);
				pvr2_buffer_set_buffer(bp,
						       cp->buffer_storage[idx],
						       BUFFER_SIZE);
			}
			cp->stream = sp;
		}
	} while (0); up(&cp->sem);

	return 0;
}

int pvr2_ioread_set_enabled(struct pvr2_ioread *cp,int fl)
{
	int ret = 0;
	if ((!fl) == (!(cp->enabled))) return ret;

	down(&cp->sem); do {
		if (fl) {
			ret = pvr2_ioread_start(cp);
		} else {
			pvr2_ioread_stop(cp);
		}
	} while (0); up(&cp->sem);
	return ret;
}

int pvr2_ioread_get_enabled(struct pvr2_ioread *cp)
{
	return cp->enabled != 0;
}

int pvr2_ioread_avail(struct pvr2_ioread *cp)
{
	int ret;
	if (!(cp->enabled)) {
		// Stream is not enabled; so this is an I/O error
		return -EIO;
	}

	ret = 0;
	if (cp->stream_running) {
		if (!pvr2_stream_get_ready_count(cp->stream)) {
			// No data available at all right now.
			ret = -EAGAIN;
		}
	} else {
		if (pvr2_stream_get_ready_count(cp->stream) < BUFFER_COUNT/2) {
			// Haven't buffered up enough yet; try again later
			ret = -EAGAIN;
		}
	}

	if ((!(cp->spigot_open)) != (!(ret == 0))) {
		cp->spigot_open = (ret == 0);
		pvr2_trace(PVR2_TRACE_DATA_FLOW,
			   "/*---TRACE_READ---*/ data is %s",
			   cp->spigot_open ? "available" : "pending");
	}

	return ret;
}

int pvr2_ioread_read(struct pvr2_ioread *cp,void __user *buf,unsigned int cnt)
{
	unsigned int copied_cnt;
	unsigned int bcnt;
	const char *src;
	int stat;
	struct pvr2_buffer *bp;
	int ret = 0;
	unsigned int req_cnt = cnt;

	if (!cnt) {
		pvr2_trace(PVR2_TRACE_TRAP,
			   "/*---TRACE_READ---*/ pvr2_ioread_read id=%p"
			   " ZERO Request? Returning zero.",cp);
		return 0;
	}

	stat = pvr2_ioread_avail(cp);
	if (stat < 0) return stat;

	cp->stream_running = !0;

	down(&cp->sem); do {

		// Suck data out of the buffers and copy to the user
		copied_cnt = 0;
		if (!buf) cnt = 0;
		while (cnt) {
			bp = pvr2_stream_get_ready_buffer(cp->stream);
			if (!bp) break; // Nothing ready; done

			bcnt = pvr2_buffer_get_count(bp);
			if (!bcnt) {
				// Nothing transferred.  Was there an
				// error?
				stat = pvr2_buffer_get_status(bp);
				if (stat < 0) {
					// Streaming error...
					pvr2_trace(PVR2_TRACE_DATA_FLOW,
						   "/*---TRACE_READ---*/"
						   " pvr2_ioread_read id=%p"
						   " buffer_error=%d",
						   cp,stat);
					pvr2_ioread_stop(cp);
					ret = -EIO;
					break;
				}
				src = 0;
			} else {
				// Calculate buffer offset and count
				src = cp->buffer_storage[
					pvr2_buffer_get_id(bp)];
				if (cp->resid_offs > bcnt) {
					cp->resid_offs = bcnt;
				}
				src += cp->resid_offs;
				bcnt -= cp->resid_offs;
			}
			if (bcnt > cnt) {
				// Can't read the entire buffer this time
				// so remember how far in we're going to
				// get so we can resume it later
				bcnt = cnt;
				cp->resid_offs += bcnt;
			} else {
				// We will be able to get the whole thing,
				// so clear the residual offset
				cp->resid_offs = 0;
			}
			if (bcnt) {
				if (copy_to_user(buf,src,bcnt)) {
					// User supplied a bad pointer?
					// Give up - this *will* cause data
					// to be lost.
					ret = -EFAULT;
					break;
				}
				cnt -= bcnt;
				buf += bcnt;
				copied_cnt += bcnt;
			}
			if (cp->resid_offs) break; // If there's a residual
						   // offset, get out
			// Queue the buffer now that we've consumed it.
			stat = pvr2_buffer_queue(bp);
			if (stat < 0) {
				// Streaming error...
				pvr2_trace(PVR2_TRACE_DATA_FLOW,
					   "/*---TRACE_READ---*/"
					   " pvr2_ioread_read id=%p"
					   " queue_error=%d",
					   cp,stat);
				pvr2_ioread_stop(cp);
				ret = stat;
				break;
			}
		}

	} while (0); up(&cp->sem);

	if (!ret) {
		if (copied_cnt) {
			// If anything was copied, return that count
			ret = copied_cnt;
		} else {
			// Nothing copied; suggest to caller that another
			// attempt should be tried again later
			ret = -EAGAIN;
		}
	}

	pvr2_trace(PVR2_TRACE_DATA_FLOW,
		   "/*---TRACE_READ---*/ pvr2_ioread_read"
		   " id=%p request=%d result=%d",
		   cp,req_cnt,ret);
	return ret;
}


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
