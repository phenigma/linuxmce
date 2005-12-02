/*
 *
 *  $Id: pvrusb2-hdw.h,v 1.2 2005/11/29 14:10:44 mchehab Exp $
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
#ifndef __PVRUSB2_HDW_H
#define __PVRUSB2_HDW_H

#include <linux/usb.h>
#include "pvrusb2-io.h"

/* Definition of state variables that we can inspect & change.  Numbers are
   assigned from zero counting up with no gaps. */
#define PVR2_CID_BRIGHTNESS 0
#define PVR2_CID_CONTRAST 1
#define PVR2_CID_SATURATION 2
#define PVR2_CID_HUE 3
#define PVR2_CID_VOLUME 4
#define PVR2_CID_BALANCE 5
#define PVR2_CID_BASS 6
#define PVR2_CID_TREBLE 7
#define PVR2_CID_MUTE 8
#define PVR2_CID_SRATE 9
#define PVR2_CID_AUDIOBITRATE 10
#define PVR2_CID_AUDIOCRC 11
#define PVR2_CID_AUDIOEMPHASIS 12
#define PVR2_CID_VBR 13
#define PVR2_CID_AVERAGEVIDEOBITRATE 14
#define PVR2_CID_PEAKVIDEOBITRATE 15
#define PVR2_CID_VIDEOSTANDARD 16
#define PVR2_CID_INPUT 17
#define PVR2_CID_AUDIOMODE 18
#define PVR2_CID_FREQUENCY 19 // Units of Hz
#define PVR2_CID_HRES 20
#define PVR2_CID_VRES 21
#define PVR2_CID_INTERLACE 22
#define PVR2_CID_AUDIOLAYER 23
#define PVR2_CID_CHANNEL 24
#define PVR2_CID_CHANPROG_ID 25
#define PVR2_CID_CHANPROG_FREQ 26
#define PVR2_CID_SIGNAL_PRESENT 27
#define PVR2_CID_STREAMING_ENABLED 28
#define PVR2_CID_HSM 29
#define PVR2_CID_SUBSYS_MASK 30
#define PVR2_CID_SUBSYS_STREAM_MASK 31

/* Number of state variables */
#define PVR2_CID_COUNT 32

/* Legal values for the SRATE state variable */
#define PVR2_CVAL_SRATE_48 0
#define PVR2_CVAL_SRATE_44_1 1
#define PVR2_CVAL_SRATE_MIN PVR2_CVAL_SRATE_48
#define PVR2_CVAL_SRATE_MAX PVR2_CVAL_SRATE_44_1

/* Legal values for the AUDIOBITRATE state variable */
#define PVR2_CVAL_AUDIOBITRATE_384 0
#define PVR2_CVAL_AUDIOBITRATE_320 1
#define PVR2_CVAL_AUDIOBITRATE_256 2
#define PVR2_CVAL_AUDIOBITRATE_224 3
#define PVR2_CVAL_AUDIOBITRATE_192 4
#define PVR2_CVAL_AUDIOBITRATE_160 5
#define PVR2_CVAL_AUDIOBITRATE_128 6
#define PVR2_CVAL_AUDIOBITRATE_112 7
#define PVR2_CVAL_AUDIOBITRATE_96 8
#define PVR2_CVAL_AUDIOBITRATE_80 9
#define PVR2_CVAL_AUDIOBITRATE_64 10
#define PVR2_CVAL_AUDIOBITRATE_56 11
#define PVR2_CVAL_AUDIOBITRATE_48 12
#define PVR2_CVAL_AUDIOBITRATE_32 13
#define PVR2_CVAL_AUDIOBITRATE_VBR 14
#define PVR2_CVAL_AUDIOBITRATE_MIN PVR2_CVAL_AUDIOBITRATE_384
#define PVR2_CVAL_AUDIOBITRATE_MAX PVR2_CVAL_AUDIOBITRATE_VBR

/* Legal values for the AUDIOEMPHASIS state variable */
#define PVR2_CVAL_AUDIOEMPHASIS_NONE 0
#define PVR2_CVAL_AUDIOEMPHASIS_50_15 1
#define PVR2_CVAL_AUDIOEMPHASIS_CCITT 2
#define PVR2_CVAL_AUDIOEMPHASIS_MIN PVR2_CVAL_AUDIOEMPHASIS_NONE
#define PVR2_CVAL_AUDIOEMPHASIS_MAX PVR2_CVAL_AUDIOEMPHASIS_CCITT

/* Legal values for the VIDEOSTANDARD state variable */
#define PVR2_CVAL_VIDEOSTANDARD_NTSC_M 0
#define PVR2_CVAL_VIDEOSTANDARD_SECAM_L 1
#define PVR2_CVAL_VIDEOSTANDARD_PAL_BG 2
#define PVR2_CVAL_VIDEOSTANDARD_PAL_I 3
#define PVR2_CVAL_VIDEOSTANDARD_PAL_DK 4
#define PVR2_CVAL_VIDEOSTANDARD_PAL_M 5

#define PVR2_CVAL_VIDEOSTANDARD_MIN PVR2_CVAL_VIDEOSTANDARD_NTSC_M
#define PVR2_CVAL_VIDEOSTANDARD_MAX PVR2_CVAL_VIDEOSTANDARD_PAL_M

/* Legal values for the INPUT state variable */
#define PVR2_CVAL_INPUT_TV 0
#define PVR2_CVAL_INPUT_SVIDEO 1
#define PVR2_CVAL_INPUT_COMPOSITE 2
#define PVR2_CVAL_INPUT_RADIO 3
#define PVR2_CVAL_INPUT_MIN PVR2_CVAL_INPUT_TV
#define PVR2_CVAL_INPUT_MAX PVR2_CVAL_INPUT_RADIO

/* Legal values for the AUDIOMODE state variable */
#define PVR2_CVAL_AUDIOMODE_MONO 0
#define PVR2_CVAL_AUDIOMODE_STEREO 1
#define PVR2_CVAL_AUDIOMODE_SAP 2
#define PVR2_CVAL_AUDIOMODE_LANG1 3
#define PVR2_CVAL_AUDIOMODE_LANG2 4
#define PVR2_CVAL_AUDIOMODE_MIN PVR2_CVAL_AUDIOMODE_MONO
#define PVR2_CVAL_AUDIOMODE_MAX PVR2_CVAL_AUDIOMODE_LANG2

/* Values that pvr2_hdw_get_signal_status() returns */
#define PVR2_SIGNAL_OK     0x0001
#define PVR2_SIGNAL_STEREO 0x0002
#define PVR2_SIGNAL_SAP    0x0004

/* Legal values for PVR2_CID_HSM */
#define PVR2_CVAL_HSM_FAIL 0
#define PVR2_CVAL_HSM_FULL 1
#define PVR2_CVAL_HSM_HIGH 2
#define PVR2_CVAL_HSM_MIN PVR2_CVAL_HSM_FAIL
#define PVR2_CVAL_HSM_MAX PVR2_CVAL_HSM_HIGH

/* Subsystem definitions - these are various pieces that can be
   independently stopped / started.  Usually you don't want to mess with
   this directly (let the driver handle things itself), but it is useful
   for debugging. */
#define PVR2_SUBSYS_ENC_FIRMWARE        0x00000001
#define PVR2_SUBSYS_TUNER_CFG_STD       0x00000002
#define PVR2_SUBSYS_TUNER_CFG_FREQ      0x00000004
#define PVR2_SUBSYS_AUDIO_CFG_VBBTM     0x00000008
#define PVR2_SUBSYS_AUDIO_CFG_STD       0x00000010
#define PVR2_SUBSYS_AUDIO_CFG_MODE      0x00000020
#define PVR2_SUBSYS_DIGITIZER_CFG_NORM  0x00000040
#define PVR2_SUBSYS_DIGITIZER_CFG_INPUT 0x00000080
#define PVR2_SUBSYS_DIGITIZER_CFG_SIZE  0x00000100
#define PVR2_SUBSYS_DIGITIZER_CFG_AUDIO 0x00000200
#define PVR2_SUBSYS_DIGITIZER_CFG_BCSH  0x00000400
#define PVR2_SUBSYS_ENC_CFG             0x00000800
#define PVR2_SUBSYS_DIGITIZER_RUN       0x00001000
#define PVR2_SUBSYS_USBSTREAM_RUN       0x00002000
#define PVR2_SUBSYS_ENC_RUN             0x00004000

#define PVR2_SUBSYS_TUNER_CFG_ALL ( \
	PVR2_SUBSYS_TUNER_CFG_STD | \
	PVR2_SUBSYS_TUNER_CFG_FREQ )
#define PVR2_SUBSYS_AUDIO_CFG_ALL ( \
	PVR2_SUBSYS_AUDIO_CFG_MODE | \
	PVR2_SUBSYS_AUDIO_CFG_STD | \
	PVR2_SUBSYS_AUDIO_CFG_VBBTM )
#define PVR2_SUBSYS_DIGITIZER_CFG_ALL ( \
	PVR2_SUBSYS_DIGITIZER_CFG_NORM | \
	PVR2_SUBSYS_DIGITIZER_CFG_INPUT | \
	PVR2_SUBSYS_DIGITIZER_CFG_SIZE | \
	PVR2_SUBSYS_DIGITIZER_CFG_AUDIO | \
	PVR2_SUBSYS_DIGITIZER_CFG_BCSH )
#define PVR2_SUBSYS_CFG_ALL ( \
        PVR2_SUBSYS_ENC_FIRMWARE | \
	PVR2_SUBSYS_TUNER_CFG_ALL | \
	PVR2_SUBSYS_AUDIO_CFG_ALL | \
	PVR2_SUBSYS_DIGITIZER_CFG_ALL | \
	PVR2_SUBSYS_ENC_CFG )
#define PVR2_SUBSYS_RUN_ALL ( \
	PVR2_SUBSYS_DIGITIZER_RUN | \
	PVR2_SUBSYS_USBSTREAM_RUN | \
	PVR2_SUBSYS_ENC_RUN )
#define PVR2_SUBSYS_ALL ( \
	PVR2_SUBSYS_CFG_ALL | \
	PVR2_SUBSYS_RUN_ALL )

enum pvr2_config {
	pvr2_config_empty,
	pvr2_config_mpeg,
	pvr2_config_vbi,
	pvr2_config_radio,
};

const char *pvr2_config_get_name(enum pvr2_config);

struct pvr2_hdw;

/* Create and return a structure for interacting with the underlying
   hardware */
struct pvr2_hdw *pvr2_hdw_create(struct usb_interface *intf);

/* Get pointer to structure given unit number */
struct pvr2_hdw *pvr2_hdw_find(int unit_number);

/* Destroy hardware interaction structure */
void pvr2_hdw_destroy(struct pvr2_hdw *);

/* Set up the structure and attempt to put the device into a usable state.
   This can be a time-consuming operation, which is why it is not done
   internally as part of the create() step.  Return value is exactly the
   same as pvr2_hdw_init_ok(). */
int pvr2_hdw_setup(struct pvr2_hdw *);

/* Initialization succeeded */
int pvr2_hdw_init_ok(struct pvr2_hdw *);

/* Return true if in the ready (normal) state */
int pvr2_hdw_dev_ok(struct pvr2_hdw *);

/* Return small integer number [1..N] for logical instance number of this
   device.  This is useful for indexing array-valued module parameters. */
int pvr2_hdw_get_unit_number(struct pvr2_hdw *);

/* Get pointer to underlying USB device */
struct usb_device *pvr2_hdw_get_dev(struct pvr2_hdw *);

/* Retrieve serial number of device */
unsigned long pvr2_hdw_get_sn(struct pvr2_hdw *);

/* Called when hardware has been unplugged */
void pvr2_hdw_disconnect(struct pvr2_hdw *);

/* Retrieve current value for a given control */
int pvr2_hdw_get_ctl_value(struct pvr2_hdw *,unsigned int ctl_id);

/* Return true if control is writable */
int pvr2_hdw_get_ctl_rw(struct pvr2_hdw *,unsigned int ctl_id);

/* Retrieve legal minimum value for a given control */
int pvr2_hdw_get_ctl_min_value(struct pvr2_hdw *,unsigned int ctl_id);

/* Retrieve legal maximum value for a given control */
int pvr2_hdw_get_ctl_max_value(struct pvr2_hdw *,unsigned int ctl_id);

/* Set current value for given control - this is just stored; the hardware
   isn't updated until the commit function is called. */
int pvr2_hdw_set_ctl_value(struct pvr2_hdw *,unsigned int ctl_id,int value);

/* Retrieve string name for given control */
const char *pvr2_hdw_get_ctl_name(struct pvr2_hdw *,unsigned int ctl_id);

/* Retrieve string name for a given control value (returns a null pointer
   for any invalid combinations). */
const char *pvr2_hdw_get_ctl_value_name(struct pvr2_hdw *,
                                        unsigned int ctl_id,int value);

/* Commit all control changes made up to this point */
int pvr2_hdw_commit_ctl(struct pvr2_hdw *);

/* Find out how many controls there are.  Legal ids are numbered from 0
   through this value - 1. */
unsigned int pvr2_hdw_get_ctl_count(struct pvr2_hdw *);

/* Return PVR2_SIGNAL_XXXX bit mask indicating signal status */
unsigned int pvr2_hdw_get_signal_status(struct pvr2_hdw *);

/* Query device and see if it thinks it is on a high-speed USB link */
int pvr2_hdw_is_hsm(struct pvr2_hdw *);

/* Turn streaming on/off */
int pvr2_hdw_set_streaming(struct pvr2_hdw *,int);

/* Find out if streaming is on */
int pvr2_hdw_get_streaming(struct pvr2_hdw *);

/* Configure the type of stream to generate */
int pvr2_hdw_set_stream_type(struct pvr2_hdw *, enum pvr2_config);

/* Get handle to video output stream */
struct pvr2_stream *pvr2_hdw_get_video_stream(struct pvr2_hdw *);

/* Enable / disable various pieces of hardware.  Items to change are
   identified by bit positions within msk, and new state for each item is
   identified by corresponding bit positions within val. */
void pvr2_hdw_subsys_bit_chg(struct pvr2_hdw *hdw,
                             unsigned long msk,unsigned long val);

/* Shortcut for pvr2_hdw_subsys_bit_chg(hdw,msk,msk) */
void pvr2_hdw_subsys_bit_set(struct pvr2_hdw *hdw,unsigned long msk);

/* Shortcut for pvr2_hdw_subsys_bit_chg(hdw,msk,0) */
void pvr2_hdw_subsys_bit_clr(struct pvr2_hdw *hdw,unsigned long msk);

/* Retrieve mask indicating which pieces of hardware are currently enabled
   / configured. */
unsigned long pvr2_hdw_subsys_get(struct pvr2_hdw *);

/* Adjust mask of what get shut down when streaming is stopped.  This is a
   debugging aid. */
void pvr2_hdw_subsys_stream_bit_chg(struct pvr2_hdw *hdw,
                                    unsigned long msk,unsigned long val);

/* Retrieve mask indicating which pieces of hardware are disabled when
   streaming is turned off. */
unsigned long pvr2_hdw_subsys_stream_get(struct pvr2_hdw *);


/* Enable / disable retrieval of CPU firmware.  This must be enabled before
   pvr2_hdw_cpufw_get() will function.  Note that doing this may prevent
   the device from running (and leaving this mode may imply a device
   reset). */
void pvr2_hdw_cpufw_set_enabled(struct pvr2_hdw *, int enable_flag);

/* Return true if we're in a mode for retrieval CPU firmware */
int pvr2_hdw_cpufw_get_enabled(struct pvr2_hdw *);

/* Retrieve a piece of the CPU's firmware at the given offset.  Return
   value is the number of bytes retrieved or zero if we're past the end or
   an error otherwise (e.g. if firmware retrieval is not enabled). */
int pvr2_hdw_cpufw_get(struct pvr2_hdw *,unsigned int offs,
                       char *buf,unsigned int cnt);

/* Retrieve previously stored v4l minor device number */
int pvr2_hdw_v4l_get_minor_number(struct pvr2_hdw *);

/* Store the v4l minor device number */
void pvr2_hdw_v4l_store_minor_number(struct pvr2_hdw *,int);


/* The following entry points are all lower level things you normally don't
   want to worry about. */

/* Attempt to recover from a USB foul-up (in practice I find that if you
   have to do this, then it's already too late). */
void pvr2_reset_ctl_endpoints(struct pvr2_hdw *hdw);

/* Issue a command and get a response from the device.  LOTS of higher
   level stuff is built on this. */
int pvr2_send_request(struct pvr2_hdw *, void *, unsigned int,
		      void *, unsigned int);

/* Slightly higher level device communication functions. */
int pvr2_write_register(struct pvr2_hdw *, u16, u32);
int pvr2_read_register(struct pvr2_hdw *, u16, u32 *);
int pvr2_write_u16(struct pvr2_hdw *, u16, int);
int pvr2_write_u8(struct pvr2_hdw *, u8, int);

/* Call if for any reason we can't talk to the hardware anymore - this will
   cause the driver to stop flailing on the device. */
void pvr2_hdw_render_useless(struct pvr2_hdw *);
void pvr2_hdw_render_useless_unlocked(struct pvr2_hdw *);

/* Set / clear 8051's reset bit */
void pvr2_hdw_cpureset_assert(struct pvr2_hdw *,int);

/* Execute a USB-commanded device reset */
void pvr2_hdw_device_reset(struct pvr2_hdw *);

/* Execute hard reset command (after this point it's likely that the
   encoder will have to be reconfigured).  This also clears the "useless"
   state. */
int pvr2_hdw_cmd_deep_reset(struct pvr2_hdw *);

/* Execute simple reset command */
int pvr2_hdw_cmd_soft_reset(struct pvr2_hdw *);

/* Stop / start video stream transport */
int pvr2_hdw_cmd_usbstream(struct pvr2_hdw *hdw,int runFl);

/* Direct manipulation of GPIO bits */
int pvr2_hdw_gpio_get_dir(struct pvr2_hdw *hdw,u32 *);
int pvr2_hdw_gpio_get_out(struct pvr2_hdw *hdw,u32 *);
int pvr2_hdw_gpio_get_in(struct pvr2_hdw *hdw,u32 *);
int pvr2_hdw_gpio_chg_dir(struct pvr2_hdw *hdw,u32 msk,u32 val);
int pvr2_hdw_gpio_chg_out(struct pvr2_hdw *hdw,u32 msk,u32 val);

/* This data structure is specifically for the next function... */
struct pvr2_hdw_debug_info {
	int big_lock_held;
	int ctl_lock_held;
	int flag_ok;
	int flag_disconnected;
	int flag_init_ok;
	int flag_streaming_enabled;
	unsigned long subsys_flags;
	int cmd_debug_state;
	int cmd_debug_write_len;
	int cmd_debug_read_len;
	int cmd_debug_write_pend;
	int cmd_debug_read_pend;
	int cmd_debug_timeout;
	int cmd_debug_rstatus;
	int cmd_debug_wstatus;
	unsigned char cmd_code;
};

/* Non-intrusively retrieve internal state info - this is useful for
   diagnosing lockups.  Note that this operation is completed without any
   kind of locking and so it is not atomic and may yield inconsistent
   results.  This is *purely* a debugging aid. */
void pvr2_hdw_get_debug_info(const struct pvr2_hdw *hdw,
                             struct pvr2_hdw_debug_info *);

/* Cause encoder firmware to be uploaded into the device.  This is normally
   done autonomously, but the interface is exported here because it is also
   a debugging aid. */
int pvr2_upload_firmware2(struct pvr2_hdw *hdw);


#endif /* __PVRUSB2_HDW_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
