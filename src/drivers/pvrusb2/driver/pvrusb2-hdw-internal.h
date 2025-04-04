/*
 *
 *  $Id: pvrusb2-hdw-internal.h 2335 2009-10-11 21:33:11Z isely $
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

#include "pvrusb2-options.h"
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/workqueue.h>
#ifndef PVR2_ENABLE_OLD_SEMAPHORES
#include <linux/mutex.h>
#else
#include <asm/semaphore.h>
#endif
#include "pvrusb2-hdw.h"
#include "pvrusb2-io.h"
#ifdef PVR2_ENABLE_V4L2SUBDEV
#include <media/v4l2-device.h>
#endif
#ifdef PVR2_ENABLE_CX25840_ALGO_HACK
#include <linux/i2c-algo-bit.h>
#endif
#ifdef PVR2_CX2341X_HDR_MEDIA
#include <media/cx2341x.h>
#else
#include "cx2341x.h"
#endif
#include "pvrusb2-devattr.h"

#ifndef PVR2_ENABLE_CX2341XMOD

/* Legal values for the SRATE state variable */
#define PVR2_CVAL_SRATE_44_1 0
#define PVR2_CVAL_SRATE_48 1
#define PVR2_CVAL_SRATE_32 2

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

/* Legal values for the STREAMTYPE state variable */
#define PVR2_CVAL_STREAMTYPE_PS 0
#define PVR2_CVAL_STREAMTYPE_TS 1
#define PVR2_CVAL_STREAMTYPE_SS 2
#define PVR2_CVAL_STREAMTYPE_DVD 3
#define PVR2_CVAL_STREAMTYPE_VCD 4
#define PVR2_CVAL_STREAMTYPE_SVCD 5
#define PVR2_CVAL_STREAMTYPE_DVDS1 6
#define PVR2_CVAL_STREAMTYPE_DVDS2 7
#define PVR2_CVAL_STREAMTYPE_PESAV 8
#define PVR2_CVAL_STREAMTYPE_PESV 9
#define PVR2_CVAL_STREAMTYPE_PESA 10

/* Legal values for the ASPECT state variable */
#define PVR2_CVAL_ASPECT_1X1 0
#define PVR2_CVAL_ASPECT_4X3 1
#define PVR2_CVAL_ASPECT_16X9 2
#define PVR2_CVAL_ASPECT_221X1 3

/* Legal values for the VBR state variable */
#define PVR2_CVAL_VBR_CONSTANT 0
#define PVR2_CVAL_VBR_VARIABLE 1

/* Legal values for the AUDIOEMPHASIS state variable */
#define PVR2_CVAL_AUDIOEMPHASIS_NONE 0
#define PVR2_CVAL_AUDIOEMPHASIS_50_15 1
#define PVR2_CVAL_AUDIOEMPHASIS_CCITT 2

/* Legal vales for the AUDIOMODEEXTENSION state variable */
#define PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_4 0
#define PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_8 1
#define PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_12 2
#define PVR2_CVAL_AUDIOMODEEXTENSION_BOUND_16 3

/* Legal vales for the MPEGAUDIOMODE state variable */
#define PVR2_CVAL_MPEGAUDIOMODE_STEREO 0
#define PVR2_CVAL_MPEGAUDIOMODE_JOINT_STEREO 1
#define PVR2_CVAL_MPEGAUDIOMODE_DUAL 2
#define PVR2_CVAL_MPEGAUDIOMODE_MONO 3

#endif /* PVR2_ENABLE_CX2341XMOD */
/* Legal values for PVR2_CID_HSM */
#define PVR2_CVAL_HSM_FAIL 0
#define PVR2_CVAL_HSM_FULL 1
#define PVR2_CVAL_HSM_HIGH 2

#define PVR2_VID_ENDPOINT        0x84
#define PVR2_UNK_ENDPOINT        0x86    /* maybe raw yuv ? */
#define PVR2_VBI_ENDPOINT        0x88

#define PVR2_CTL_BUFFSIZE        64

#define FREQTABLE_SIZE 500

#define LOCK_TAKE(x) do { mutex_lock(&x##_mutex); x##_held = !0; } while (0)
#define LOCK_GIVE(x) do { x##_held = 0; mutex_unlock(&x##_mutex); } while (0)

typedef int (*pvr2_ctlf_is_dirty)(struct pvr2_ctrl *);
typedef void (*pvr2_ctlf_clear_dirty)(struct pvr2_ctrl *);
typedef int (*pvr2_ctlf_check_value)(struct pvr2_ctrl *,int);
typedef int (*pvr2_ctlf_get_value)(struct pvr2_ctrl *,int *);
typedef int (*pvr2_ctlf_set_value)(struct pvr2_ctrl *,int msk,int val);
typedef int (*pvr2_ctlf_val_to_sym)(struct pvr2_ctrl *,int msk,int val,
				    char *,unsigned int,unsigned int *);
typedef int (*pvr2_ctlf_sym_to_val)(struct pvr2_ctrl *,
				    const char *,unsigned int,
				    int *mskp,int *valp);
typedef unsigned int (*pvr2_ctlf_get_v4lflags)(struct pvr2_ctrl *);

/* This structure describes a specific control.  A table of these is set up
   in pvrusb2-hdw.c. */
struct pvr2_ctl_info {
	/* Control's name suitable for use as an identifier */
	const char *name;

	/* Short description of control */
	const char *desc;

	/* Control's implementation */
	pvr2_ctlf_get_value get_value;      /* Get its value */
	pvr2_ctlf_get_value get_def_value;  /* Get its default value */
	pvr2_ctlf_get_value get_min_value;  /* Get minimum allowed value */
	pvr2_ctlf_get_value get_max_value;  /* Get maximum allowed value */
	pvr2_ctlf_set_value set_value;      /* Set its value */
	pvr2_ctlf_check_value check_value;  /* Check that value is valid */
	pvr2_ctlf_val_to_sym val_to_sym;    /* Custom convert value->symbol */
	pvr2_ctlf_sym_to_val sym_to_val;    /* Custom convert symbol->value */
	pvr2_ctlf_is_dirty is_dirty;        /* Return true if dirty */
	pvr2_ctlf_clear_dirty clear_dirty;  /* Clear dirty state */
	pvr2_ctlf_get_v4lflags get_v4lflags;/* Retrieve v4l flags */

	/* Control's type (int, enum, bitmask) */
	enum pvr2_ctl_type type;

	/* Associated V4L control ID, if any */
	int v4l_id;

	/* Associated driver internal ID, if any */
	int internal_id;

	/* Don't implicitly initialize this control's value */
	int skip_init;

	/* Starting value for this control */
	int default_value;

	/* Type-specific control information */
	union {
		struct { /* Integer control */
			long min_value; /* lower limit */
			long max_value; /* upper limit */
		} type_int;
		struct { /* enumerated control */
			unsigned int count;       /* enum value count */
			const char **value_names; /* symbol names */
		} type_enum;
		struct { /* bitmask control */
			unsigned int valid_bits; /* bits in use */
			const char **bit_names;  /* symbol name/bit */
		} type_bitmask;
	} def;
};


/* Same as pvr2_ctl_info, but includes storage for the control description */
#define PVR2_CTLD_INFO_DESC_SIZE 32
struct pvr2_ctld_info {
	struct pvr2_ctl_info info;
	char desc[PVR2_CTLD_INFO_DESC_SIZE];
};

struct pvr2_ctrl {
	const struct pvr2_ctl_info *info;
	struct pvr2_hdw *hdw;
};


#ifdef PVR2_ENABLE_OLD_I2COPS
struct pvr2_decoder_ctrl {
	void *ctxt;
	void (*detach)(void *);
	void (*enable)(void *,int);
	void (*force_reset)(void *);
};

#define PVR2_I2C_PEND_DETECT  0x01  /* Need to detect a client type */
#define PVR2_I2C_PEND_CLIENT  0x02  /* Client needs a specific update */
#define PVR2_I2C_PEND_REFRESH 0x04  /* Client has specific pending bits */
#define PVR2_I2C_PEND_STALE   0x08  /* Broadcast pending bits */

#define PVR2_I2C_PEND_ALL (PVR2_I2C_PEND_DETECT |\
			   PVR2_I2C_PEND_CLIENT |\
			   PVR2_I2C_PEND_REFRESH |\
			   PVR2_I2C_PEND_STALE)
#endif

/* Disposition of firmware1 loading situation */
#define FW1_STATE_UNKNOWN 0
#define FW1_STATE_MISSING 1
#define FW1_STATE_FAILED 2
#define FW1_STATE_RELOAD 3
#define FW1_STATE_OK 4

/* What state the device is in if it is a hybrid */
#define PVR2_PATHWAY_UNKNOWN 0
#define PVR2_PATHWAY_ANALOG 1
#define PVR2_PATHWAY_DIGITAL 2

typedef int (*pvr2_i2c_func)(struct pvr2_hdw *,u8,u8 *,u16,u8 *, u16);
#define PVR2_I2C_FUNC_CNT 128

/* This structure contains all state data directly needed to
   manipulate the hardware (as opposed to complying with a kernel
   interface) */
struct pvr2_hdw {
#ifdef PVR2_ENABLE_CX25840_ALGO_HACK
	// Pad the front of our context structure enough so that cx25840
	// doesn't come along and screw up our state.  A bug in cx25840
	// (kernels 2.6.15 and currently also 2.6.16) causes it to reach
	// through our I2C adapter context pointer and overwrite part of
	// our state.  This is being fixed in V4L but if you're running an
	// old kernel then we still have to contend with it.  We don't use
	// this data _at all_, but by doing this we safely move our stuff
	// out of the way.
	struct i2c_algo_bit_data algo_bit_crud;
#endif
	/* Underlying USB device handle */
	struct usb_device *usb_dev;
	struct usb_interface *usb_intf;

#ifdef PVR2_ENABLE_V4L2SUBDEV
	/* Our handle into the v4l2 sub-device architecture */
	struct v4l2_device v4l2_dev;
#endif
	/* Device description, anything that must adjust behavior based on
	   device specific info will use information held here. */
	const struct pvr2_device_desc *hdw_desc;

	/* Kernel worker thread handling */
	struct workqueue_struct *workqueue;
	struct work_struct workpoll;     /* Update driver state */
#ifdef PVR2_ENABLE_OLD_I2COPS
	struct work_struct worki2csync;  /* Update i2c clients */
#endif

	/* Video spigot */
	struct pvr2_stream *vid_stream;

	/* Mutex for all hardware state control */
#ifndef PVR2_ENABLE_OLD_SEMAPHORES
	struct mutex big_lock_mutex;
#else
	struct semaphore big_lock_mutex;
#endif
	int big_lock_held;  /* For debugging */

	/* This is a simple string which identifies the instance of this
	   driver.  It is unique within the set of existing devices, but
	   there is no attempt to keep the name consistent with the same
	   physical device each time. */
	char name[32];

	/* This is a simple string which identifies the physical device
	   instance itself - if possible.  (If not possible, then it is
	   based on the specific driver instance, similar to name above.)
	   The idea here is that userspace might hopefully be able to use
	   this recognize specific tuners.  It will encode a serial number,
	   if available. */
	char identifier[32];

	/* I2C stuff */
	struct i2c_adapter i2c_adap;
	struct i2c_algorithm i2c_algo;
	pvr2_i2c_func i2c_func[PVR2_I2C_FUNC_CNT];
	int i2c_cx25840_hack_state;
	int i2c_linked;
#ifdef PVR2_ENABLE_OLD_I2COPS
	unsigned int i2c_pend_types;    /* Which types of update are needed */
	unsigned long i2c_pend_mask;    /* Change bits we need to scan */
	unsigned long i2c_stale_mask;   /* Pending broadcast change bits */
	unsigned long i2c_active_mask;  /* All change bits currently in use */
	struct list_head i2c_clients;
#ifndef PVR2_ENABLE_OLD_SEMAPHORES
	struct mutex i2c_list_lock;
#else
	struct semaphore i2c_list_lock;
#endif
#endif

	/* IR related */
	unsigned int ir_scheme_active; /* IR scheme as seen from the outside */

	/* Frequency table */
	unsigned int freqTable[FREQTABLE_SIZE];
	unsigned int freqProgSlot;

	/* Stuff for handling low level control interaction with device */
#ifndef PVR2_ENABLE_OLD_SEMAPHORES
	struct mutex ctl_lock_mutex;
#else
	struct semaphore ctl_lock_mutex;
#endif
	int ctl_lock_held;  /* For debugging */
	struct urb *ctl_write_urb;
	struct urb *ctl_read_urb;
	unsigned char *ctl_write_buffer;
	unsigned char *ctl_read_buffer;
	int ctl_write_pend_flag;
	int ctl_read_pend_flag;
	int ctl_timeout_flag;
	struct completion ctl_done;
	unsigned char cmd_buffer[PVR2_CTL_BUFFSIZE];
	int cmd_debug_state;               // Low level command debugging info
	unsigned char cmd_debug_code;      //
	unsigned int cmd_debug_write_len;  //
	unsigned int cmd_debug_read_len;   //

	/* Bits of state that describe what is going on with various parts
	   of the driver. */
	int state_pathway_ok;         /* Pathway config is ok */
	int state_encoder_ok;         /* Encoder is operational */
	int state_encoder_run;        /* Encoder is running */
	int state_encoder_config;     /* Encoder is configured */
	int state_encoder_waitok;     /* Encoder pre-wait done */
	int state_encoder_runok;      /* Encoder has run for >= .25 sec */
	int state_decoder_run;        /* Decoder is running */
	int state_usbstream_run;      /* FX2 is streaming */
	int state_decoder_quiescent;  /* Decoder idle for > 50msec */
	int state_pipeline_config;    /* Pipeline is configured */
	int state_pipeline_req;       /* Somebody wants to stream */
	int state_pipeline_pause;     /* Pipeline must be paused */
	int state_pipeline_idle;      /* Pipeline not running */

	/* This is the master state of the driver.  It is the combined
	   result of other bits of state.  Examining this will indicate the
	   overall state of the driver.  Values here are one of
	   PVR2_STATE_xxxx */
	unsigned int master_state;

	/* True if device led is currently on */
	int led_on;

	/* True if states must be re-evaluated */
	int state_stale;

	void (*state_func)(void *);
	void *state_data;

	/* Timer for measuring decoder settling time */
	struct timer_list quiescent_timer;

	/* Timer for measuring encoder pre-wait time */
	struct timer_list encoder_wait_timer;

	/* Timer for measuring encoder minimum run time */
	struct timer_list encoder_run_timer;

	/* Place to block while waiting for state changes */
	wait_queue_head_t state_wait_data;


	int force_dirty;        /* consider all controls dirty if true */
	int flag_ok;            /* device in known good state */
	int flag_modulefail;    /* true if at least one module failed to load */
	int flag_disconnected;  /* flag_ok == 0 due to disconnect */
	int flag_init_ok;       /* true if structure is fully initialized */
	int fw1_state;          /* current situation with fw1 */
	int pathway_state;      /* one of PVR2_PATHWAY_xxx */
	int flag_decoder_missed;/* We've noticed missing decoder */
	int flag_tripped;       /* Indicates overall failure to start */

#ifdef PVR2_ENABLE_OLD_I2COPS
	struct pvr2_decoder_ctrl *decoder_ctrl;
#endif
	unsigned int decoder_client_id;

	// CPU firmware info (used to help find / save firmware data)
	char *fw_buffer;
	unsigned int fw_size;
	int fw_cpu_flag; /* True if we are dealing with the CPU */
#ifdef PVR2_ENABLE_OLD_I2COPS

	// True if there is a request to trigger logging of state in each
	// module.
	int log_requested;
#endif

	/* Tuner / frequency control stuff */
	unsigned int tuner_type;
	int tuner_updated;
	unsigned int freqValTelevision;  /* Current freq for tv mode */
	unsigned int freqValRadio;       /* Current freq for radio mode */
	unsigned int freqSlotTelevision; /* Current slot for tv mode */
	unsigned int freqSlotRadio;      /* Current slot for radio mode */
	unsigned int freqSelector;       /* 0=radio 1=television */
	int freqDirty;

	/* Current tuner info - this information is polled from the I2C bus */
	struct v4l2_tuner tuner_signal_info;
	int tuner_signal_stale;

	/* Cropping capability info */
	struct v4l2_cropcap cropcap_info;
	int cropcap_stale;

	/* Video standard handling */
	v4l2_std_id std_mask_eeprom; // Hardware supported selections
	v4l2_std_id std_mask_avail;  // Which standards we may select from
	v4l2_std_id std_mask_cur;    // Currently selected standard(s)
	unsigned int std_enum_cnt;   // # of enumerated standards
	int std_enum_cur;            // selected standard enumeration value
	int std_dirty;               // True if std_mask_cur has changed
	struct pvr2_ctl_info std_info_enum;
	struct pvr2_ctl_info std_info_avail;
	struct pvr2_ctl_info std_info_cur;
	struct v4l2_standard *std_defs;
	const char **std_enum_names;

	// Generated string names, one per actual V4L2 standard
	const char *std_mask_ptrs[32];
	char std_mask_names[32][10];

	int unit_number;             /* ID for driver instance */
	unsigned long serial_number; /* ID for hardware itself */

	char bus_info[32]; /* Bus location info */

	/* Minor numbers used by v4l logic (yes, this is a hack, as there
	   should be no v4l junk here).  Probably a better way to do this. */
	int v4l_minor_number_video;
	int v4l_minor_number_vbi;
	int v4l_minor_number_radio;

	/* Bit mask of PVR2_CVAL_INPUT choices which are valid for the hardware */
	unsigned int input_avail_mask;
	/* Bit mask of PVR2_CVAL_INPUT choices which are currenly allowed */
	unsigned int input_allowed_mask;

	/* Location of eeprom or a negative number if none */
	int eeprom_addr;

	enum pvr2_config active_stream_type;
	enum pvr2_config desired_stream_type;

#ifdef PVR2_ENABLE_CX2341XMOD
	/* Control state needed for cx2341x module */
	struct cx2341x_mpeg_params enc_cur_state;
	struct cx2341x_mpeg_params enc_ctl_state;
	/* True if an encoder attribute has changed */
	int enc_stale;
	/* True if an unsafe encoder attribute has changed */
	int enc_unsafe_stale;
	/* True if enc_cur_state is valid */
	int enc_cur_valid;
#endif

	/* Control state */
#define VCREATE_DATA(lab) int lab##_val; int lab##_dirty
	VCREATE_DATA(brightness);
	VCREATE_DATA(contrast);
	VCREATE_DATA(saturation);
	VCREATE_DATA(hue);
	VCREATE_DATA(volume);
	VCREATE_DATA(balance);
	VCREATE_DATA(bass);
	VCREATE_DATA(treble);
	VCREATE_DATA(mute);
	VCREATE_DATA(cropl);
	VCREATE_DATA(cropt);
	VCREATE_DATA(cropw);
	VCREATE_DATA(croph);
	VCREATE_DATA(input);
	VCREATE_DATA(audiomode);
	VCREATE_DATA(res_hor);
	VCREATE_DATA(res_ver);
	VCREATE_DATA(srate);
#ifdef PVR2_ENABLE_AUTOMODESWITCH
	VCREATE_DATA(automodeswitch);
#endif
#ifndef PVR2_ENABLE_CX2341XMOD
	VCREATE_DATA(audiobitrate);
	VCREATE_DATA(audiomodeextension);
	VCREATE_DATA(mpegaudiomode);
	VCREATE_DATA(audiocrc);
	VCREATE_DATA(audioemphasis);
	VCREATE_DATA(streamtype);
	VCREATE_DATA(aspect);
	VCREATE_DATA(pulldown);
	VCREATE_DATA(vbr);
	VCREATE_DATA(videobitrate);
	VCREATE_DATA(videopeak);
	VCREATE_DATA(audiolayer);
#endif
#undef VCREATE_DATA

#ifdef PVR2_ENABLE_CX2341XMOD
	struct pvr2_ctld_info *mpeg_ctrl_info;
#endif

	struct pvr2_ctrl *controls;
	unsigned int control_cnt;
};

/* This function gets the current frequency */
unsigned long pvr2_hdw_get_cur_freq(struct pvr2_hdw *);
#ifdef PVR2_ENABLE_OLD_I2COPS
void pvr2_hdw_set_decoder(struct pvr2_hdw *,struct pvr2_decoder_ctrl *);
#endif

void pvr2_hdw_status_poll(struct pvr2_hdw *);

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
