/*
 *
 *  $Id: pvrusb2-video.h,v 1.2 2005/11/27 23:01:16 mcisely Exp $
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

#ifndef __PVRUSB2_VIDEO_H
#define __PVRUSB2_VIDEO_H

/*

   This module connects the pvrusb2 driver to the I2C chip level
   driver which handles device video processing.  This interface is
   used internally by the driver; higher level code should only
   interact through the interface provided by pvrusb2-hdw.h.

*/

struct pvr2_hdw;
struct pvr2_decoder;

typedef int (*pvr2_decoder_func)(struct pvr2_decoder *);

struct pvr2_decoder_func_table {
	pvr2_decoder_func destroy_func;
	pvr2_decoder_func set_norm_func;
	pvr2_decoder_func set_input_func;
	pvr2_decoder_func set_size_func;
	pvr2_decoder_func set_audio_func;
	pvr2_decoder_func set_bcsh_func;
	pvr2_decoder_func is_tuned_func;
	pvr2_decoder_func enable_func;
	pvr2_decoder_func disable_func;
};

struct pvr2_decoder {
	struct pvr2_hdw *hdw;
	const struct pvr2_decoder_func_table *func_table;
};

struct pvr2_decoder *pvr2_decoder_create(struct pvr2_hdw *);
void pvr2_decoder_destroy(struct pvr2_decoder *);
int pvr2_decoder_set_norm(struct pvr2_decoder *);
int pvr2_decoder_set_input(struct pvr2_decoder *);
int pvr2_decoder_set_size(struct pvr2_decoder *);
int pvr2_decoder_set_audio(struct pvr2_decoder *);
int pvr2_decoder_set_bcsh(struct pvr2_decoder *);
int pvr2_decoder_is_tuned(struct pvr2_decoder *);
int pvr2_decoder_enable_output(struct pvr2_decoder *);
int pvr2_decoder_disable_output(struct pvr2_decoder *);


#endif /* __PVRUSB2_VIDEO_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
