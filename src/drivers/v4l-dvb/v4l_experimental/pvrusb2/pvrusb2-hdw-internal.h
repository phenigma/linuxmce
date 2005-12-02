/*
 *
 *  $Id: pvrusb2-hdw-internal.h,v 1.2 2005/11/27 23:01:16 mcisely Exp $
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
#ifndef __PVRUSB2_HDW_INTERNAL_H
#define __PVRUSB2_HDW_INTERNAL_H

/*

  This header sets up all the internal structures and definitions needed to
  track and coordinate the driver's interaction with the hardware.  ONLY
  source files which actually implement part of that whole circus should be
  including this header.  Higher levels, like the external layers to the
  various public APIs (V4L, sysfs, etc) should NOT ever include this
  private, internal header.  This means that pvrusb2-hdw, pvrusb2-encoder,
  etc will include this, but pvrusb2-v4l should not.

*/

#include "compat.h"
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <asm/semaphore.h>
#include "pvrusb2-hdw.h"
#include "pvrusb2-io.h"

#define PVR2_VID_ENDPOINT        0x84
#define PVR2_UNK_ENDPOINT        0x86    /* maybe raw yuv ? */
#define PVR2_VBI_ENDPOINT        0x88

#define PVR2_CTL_BUFFSIZE        64

#define FREQTABLE_SIZE 500

#define LOCK_TAKE(x) do { down(&x##_sem); x##_held = !0; } while (0)
#define LOCK_GIVE(x) do { x##_held = 0; up(&x##_sem); } while (0)

struct pvr2_decoder;

struct pvr2_ctl_state {
	int value;
	int dirty;
};

/* This structure contains all state data directly needed to
   manipulate the hardware (as opposed to complying with a kernel
   interface) */
struct pvr2_hdw {
	/* Underlying USB device handle */
	struct usb_device *usb_dev;
	struct usb_interface *usb_intf;

	/* Video spigot */
	struct pvr2_stream *vid_stream;

	/* Mutex for all hardware state control */
	struct semaphore big_lock_sem;
	int big_lock_held;  /* For debugging */

	char name[32];

	/* I2C stuff */
	struct i2c_adapter i2c_adap;
	struct i2c_algorithm i2c_algo;
	int i2c_linked;
	struct i2c_client *i2c_tuner_client;
	struct i2c_client *i2c_audio_client;
	struct i2c_client *i2c_video_client;
	struct i2c_client *i2c_tveeprom_client;

	/* Frequency table */
	unsigned int freqTable[FREQTABLE_SIZE];

	/* Stuff for handling low level control interaction with device */
	struct semaphore ctl_lock_sem;
	int ctl_lock_held;  /* For debugging */
	struct urb *ctl_write_urb;
	struct urb *ctl_read_urb;
	unsigned char *ctl_write_buffer;
	unsigned char *ctl_read_buffer;
	volatile int ctl_write_pend_flag;
	volatile int ctl_read_pend_flag;
	volatile int ctl_timeout_flag;
	struct completion ctl_done;
	unsigned char cmd_buffer[PVR2_CTL_BUFFSIZE];
	int cmd_debug_state;               // Low level command debugging info
	unsigned char cmd_debug_code;      //
	unsigned int cmd_debug_write_len;  //
	unsigned int cmd_debug_read_len;   //

	int flag_ok;            // device in known good state
	int flag_disconnected;  // flag_ok == 0 due to disconnect
	int flag_init_ok;       // true if structure is fully initialized
	int flag_streaming_enabled; // true if streaming should be on

	// CPU firmware info (used to help find / save firmware data)
	char *fw_buffer;
	unsigned int fw_size;

	// Which subsystem pieces have been enabled / configured
	unsigned long subsys_enabled_mask;

	// Which subsystems are manipulated to enable streaming
	unsigned long subsys_stream_mask;

	/* Tuner / frequency control stuff */
	unsigned int tuner_type;
	unsigned long video_standards;

	/* Context for controller video decoder (see pvrusb2-video.h) */
	struct pvr2_decoder *dcp;

	int unit_number;             /* ID for driver instance */
	unsigned long serial_number; /* ID for hardware itself */

	/* Minor number used by v4l logic (yes, this is a hack, as there should
	   be no v4l junk here).  Probably a better way to do this. */
	int v4l_minor_number;

	enum pvr2_config config;

	/* Information about what audio signal we're hearing */
	int flag_stereo;
	int flag_bilingual;

	/* Every last bit of controllable state */
	struct pvr2_ctl_state controls[PVR2_CID_COUNT];
};

int pvr2_hdw_set_ctl_value_internal(struct pvr2_hdw *hdw,
                                    unsigned int ctl_id,int value);
int pvr2_hdw_commit_ctl_internal(struct pvr2_hdw *hdw);


#endif /* __PVRUSB2_HDW_INTERNAL_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
