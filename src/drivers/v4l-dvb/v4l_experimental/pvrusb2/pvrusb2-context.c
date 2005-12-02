/*
 *  $Id: pvrusb2-context.c,v 1.1 2005/11/14 13:31:24 mchehab Exp $
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

#include "pvrusb2-context.h"
#include "pvrusb2-io.h"
#include "pvrusb2-hdw.h"
#include "pvrusb2-debug.h"
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <asm/semaphore.h>


static void pvr2_context_destroy(struct pvr2_context *mp)
{
	if (mp->hdw) pvr2_hdw_destroy(mp->hdw);
	pvr2_trace(PVR2_TRACE_STRUCT,"Destroying pvr_main id=%p",mp);
	flush_workqueue(mp->workqueue);
	destroy_workqueue(mp->workqueue);
	kfree(mp);
}


static void pvr2_context_setup(struct pvr2_context *mp)
{
	pvr2_context_enter(mp); do {
		(mp->kthread_ref_count)--;
		if (!pvr2_hdw_dev_ok(mp->hdw)) break;
		pvr2_hdw_setup(mp->hdw);
		if (!pvr2_hdw_dev_ok(mp->hdw)) break;
		if (!pvr2_hdw_init_ok(mp->hdw)) break;
		mp->video_stream.stream = pvr2_hdw_get_video_stream(mp->hdw);
		if (mp->setup_func) {
			mp->setup_func(mp);
		}
	} while (0); pvr2_context_exit(mp);
}


struct pvr2_context *pvr2_context_create(
	struct usb_interface *intf,void (*setup_func)(struct pvr2_context *))
{
	struct pvr2_context *mp = 0;
	mp = kmalloc(sizeof(*mp),GFP_KERNEL);
	if (!mp) goto done;
	memset(mp,0,sizeof(*mp));
	pvr2_trace(PVR2_TRACE_STRUCT,"Creating pvr_main id=%p",mp);
	mp->setup_func = setup_func;
	init_MUTEX(&mp->sem);
	mp->hdw = pvr2_hdw_create(intf);
	if (!mp->hdw) {
		pvr2_context_destroy(mp);
		mp = 0;
		goto done;
	}

	mp->workqueue = create_singlethread_workqueue("pvrusb2");
	(mp->kthread_ref_count)++;
	INIT_WORK(&mp->workinit,(void (*)(void*))pvr2_context_setup,mp);
	queue_work(mp->workqueue,&mp->workinit);
 done:
	return mp;
}


void pvr2_context_enter(struct pvr2_context *mp)
{
	down(&mp->sem);
	pvr2_trace(PVR2_TRACE_CREG,"pvr2_context_enter(id=%p)",mp);
}


void pvr2_context_exit(struct pvr2_context *mp)
{
	int destroy_flag = 0;
	if (!(mp->mc_first ||
	      (!mp->disconnect_flag) ||
	      mp->kthread_ref_count)) {
		destroy_flag = !0;
	}
	pvr2_trace(PVR2_TRACE_CREG,"pvr2_context_exit(id=%p) outside",mp);
	up(&mp->sem);
	if (destroy_flag) pvr2_context_destroy(mp);
}


static void pvr2_context_run_checks(struct pvr2_context *mp)
{
	struct pvr2_channel *ch1,*ch2;
	for (ch1 = mp->mc_first; ch1; ch1 = ch2) {
		ch2 = ch1->mc_next;
		if (ch1->check_func) {
			ch1->check_func(ch1);
		}
	}
}


void pvr2_context_disconnect(struct pvr2_context *mp)
{
	pvr2_context_enter(mp); do {
		pvr2_hdw_disconnect(mp->hdw);
		mp->disconnect_flag = !0;
		pvr2_context_run_checks(mp);
	} while (0); pvr2_context_exit(mp);
}


void pvr2_channel_init(struct pvr2_channel *cp,struct pvr2_context *mp)
{
	cp->hdw = mp->hdw;
	cp->mc_head = mp;
	cp->mc_next = 0;
	cp->mc_prev = mp->mc_last;
	if (mp->mc_last) {
		mp->mc_last->mc_next = cp;
	} else {
		mp->mc_first = cp;
	}
	mp->mc_last = cp;
}


static void pvr2_channel_disclaim_stream(struct pvr2_channel *cp)
{
	if (!cp->stream) return;
	pvr2_stream_kill(cp->stream->stream);
	cp->stream->user = 0;
	cp->stream = 0;
}


void pvr2_channel_done(struct pvr2_channel *cp)
{
	struct pvr2_context *mp = cp->mc_head;
	pvr2_channel_disclaim_stream(cp);
	if (cp->mc_next) {
		cp->mc_next->mc_prev = cp->mc_prev;
	} else {
		mp->mc_last = cp->mc_prev;
	}
	if (cp->mc_prev) {
		cp->mc_prev->mc_next = cp->mc_next;
	} else {
		mp->mc_first = cp->mc_next;
	}
	cp->hdw = 0;
}


int pvr2_channel_claim_stream(struct pvr2_channel *cp,
                              struct pvr2_context_stream *sp)
{
	int code = 0;
	pvr2_context_enter(cp->mc_head); do {
		if (sp == cp->stream) break;
		if (sp->user) {
			code = -EBUSY;
			break;
		}
		pvr2_channel_disclaim_stream(cp);
		if (!sp) break;
		sp->user = cp;
		cp->stream = sp;
	} while (0); pvr2_context_exit(cp->mc_head);
	return code;
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
