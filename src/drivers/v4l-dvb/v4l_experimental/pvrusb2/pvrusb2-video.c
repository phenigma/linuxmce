/*
 *
 *  $Id: pvrusb2-video.c,v 1.2 2005/11/27 23:01:16 mcisely Exp $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *  Copyright (C) 2004 Aurelien Alleaume <slts@free.fr>
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

/*

   This module connects the pvrusb2 driver to the I2C chip level
   driver which handles device video processing.  This interface is
   used internally by the driver; higher level code should only
   interact through the interface provided by pvrusb2-hdw.h.

   Since there are several possible choices of low level drivers, we
   go through a layer of indirection in an attempt to select the right
   choice.  This module implements that indirection.

*/

#include "compat.h"
#include "pvrusb2-video.h"
#include "pvrusb2-debug.h"
#ifndef PVR2_SUPPRESS_SAA711X
#include "pvrusb2-video-v4l.h"
#endif
#ifdef PVR2_ENABLE_SAA7115
#include "pvrusb2-video-ivtv.h"
#endif
#include "pvrusb2-debug.h"
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/slab.h>


struct pvr2_decoder *pvr2_decoder_create(struct pvr2_hdw *hdw)
{
	struct pvr2_decoder *dcp;

#ifndef PVR2_SUPPRESS_SAA711X
	dcp = pvr2_decoder_v4l_create(hdw);
	if (dcp) {
		pvr2_trace(PVR2_TRACE_INIT,"Using v4l video decoder driver");
		return dcp;
	}
#endif

#ifdef PVR2_ENABLE_SAA7115
	dcp = pvr2_decoder_ivtv_create(hdw);
	if (dcp) {
		pvr2_trace(PVR2_TRACE_INIT,"Using ivtv video decoder driver");
		return dcp;
	}
#endif

	pvr2_trace(PVR2_TRACE_ERROR_LEGS,
		   "Failed to select a viable v4l video decoder driver");
	return 0;
}


void pvr2_decoder_destroy(struct pvr2_decoder *dcp)
{
	if (!dcp) return;
	if (dcp->func_table->destroy_func) {
		dcp->func_table->destroy_func(dcp);
	} else {
		kfree(dcp);
	}
}


int pvr2_decoder_set_norm(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->set_norm_func(dcp);
}


int pvr2_decoder_set_input(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->set_input_func(dcp);
}


int pvr2_decoder_set_size(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->set_size_func(dcp);
}


int pvr2_decoder_set_audio(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->set_audio_func(dcp);
}


int pvr2_decoder_set_bcsh(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->set_bcsh_func(dcp);
}


int pvr2_decoder_is_tuned(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->is_tuned_func(dcp);
}


int pvr2_decoder_enable_output(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->enable_func(dcp);
}


int pvr2_decoder_disable_output(struct pvr2_decoder *dcp)
{
	if (!dcp) return -EINVAL;
	return dcp->func_table->disable_func(dcp);
}


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
