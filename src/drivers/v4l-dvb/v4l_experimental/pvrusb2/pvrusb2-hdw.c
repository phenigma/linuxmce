/*
 *
 *  $Id: pvrusb2-hdw.c,v 1.4 2005/11/29 19:49:09 mchehab Exp $
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

#include "compat.h"
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/firmware.h>
#include <asm/semaphore.h>
#include "pvrusb2.h"
#include "pvrusb2-util.h"
#include "pvrusb2-hdw.h"
#include "pvrusb2-i2c.h"
#include "pvrusb2-tuner.h"
#include "pvrusb2-eeprom.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-audio.h"
#include "pvrusb2-video.h"
#include "pvrusb2-encoder.h"
#include "pvrusb2-debug.h"

static struct pvr2_hdw *unit_pointers[PVR_NUM] = {[ 0 ... PVR_NUM-1 ] = 0};
DECLARE_MUTEX(pvr2_unit_sem);

static int hfull = 1;
static int width = 720;
static int initusbreset = 1;
static int procreload = 0;

module_param(initusbreset, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(initusbreset, "Do USB reset device on probe");
module_param(procreload, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(procreload,
                 "Attempt init failure recovery with firmware reload");
module_param(hfull, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(hfull, "full height video ?");
module_param(width, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(width, "video width : 720,480,352");

#define PVR2_CTL_WRITE_ENDPOINT  0x01
#define PVR2_CTL_READ_ENDPOINT   0x81

#define PVR2_GPIO_IN 0x9008
#define PVR2_GPIO_OUT 0x900c
#define PVR2_GPIO_DIR 0x9020

#define trace_firmware(...) pvr2_trace(PVR2_TRACE_FIRMWARE,__VA_ARGS__)

#define PVR2_FIRMWARE_ENDPOINT   0x02

/* size of a firmware chunk */
#define FIRMWARE_CHUNK_SIZE 0x2000

/* Various files we will load with firmware_entry */
#define FIRMWARE1_FILE  "pvrusb2.f1"
#define FIRMWARE2_FILE  "pvrusb2.f2"

typedef int (*pvr2_ctl_set_func)(struct pvr2_hdw *,int ctl_id,int val);
typedef int (*pvr2_ctl_get_func)(struct pvr2_hdw *,int ctl_id);

struct pvr2_ctl_def {
	const char *name;
	pvr2_ctl_set_func set_func;
	pvr2_ctl_get_func get_func;
	int max_value;
	int min_value;
	int skip_init;
	int default_value;
	const char **value_defs_ptr;
	unsigned int value_defs_count;
};


static const char *control_values_srate[] = {
	[PVR2_CVAL_SRATE_48]   = "48KHz",
	[PVR2_CVAL_SRATE_44_1] = "44.1KHz",
};


static const char *control_values_audiobitrate[] = {
	[PVR2_CVAL_AUDIOBITRATE_384] = "384kb/s",
	[PVR2_CVAL_AUDIOBITRATE_320] = "320kb/s",
	[PVR2_CVAL_AUDIOBITRATE_256] = "256kb/s",
	[PVR2_CVAL_AUDIOBITRATE_224] = "224kb/s",
	[PVR2_CVAL_AUDIOBITRATE_192] = "192kb/s",
	[PVR2_CVAL_AUDIOBITRATE_160] = "160kb/s",
	[PVR2_CVAL_AUDIOBITRATE_128] = "128kb/s",
	[PVR2_CVAL_AUDIOBITRATE_112] = "112kb/s",
	[PVR2_CVAL_AUDIOBITRATE_96]  = "96kb/s",
	[PVR2_CVAL_AUDIOBITRATE_80]  = "80kb/s",
	[PVR2_CVAL_AUDIOBITRATE_64]  = "64kb/s",
	[PVR2_CVAL_AUDIOBITRATE_56]  = "56kb/s",
	[PVR2_CVAL_AUDIOBITRATE_48]  = "48kb/s",
	[PVR2_CVAL_AUDIOBITRATE_32]  = "32kb/s",
	[PVR2_CVAL_AUDIOBITRATE_VBR] = "VBR",
};


static const char *control_values_audioemphasis[] = {
	[PVR2_CVAL_AUDIOEMPHASIS_NONE]  = "None",
	[PVR2_CVAL_AUDIOEMPHASIS_50_15] = "50/15us",
	[PVR2_CVAL_AUDIOEMPHASIS_CCITT] = "CCITT J.17",
};


static const char *control_values_videostandard[] = {
	[PVR2_CVAL_VIDEOSTANDARD_NTSC_M]  = "NTSC-M",
	[PVR2_CVAL_VIDEOSTANDARD_SECAM_L] = "SECAM-L",
	[PVR2_CVAL_VIDEOSTANDARD_PAL_BG]  = "PAL-BG",
	[PVR2_CVAL_VIDEOSTANDARD_PAL_I]   = "PAL-I",
	[PVR2_CVAL_VIDEOSTANDARD_PAL_DK]  = "PAL-DK",
	[PVR2_CVAL_VIDEOSTANDARD_PAL_M]  = "PAL-M",
};


static const char *control_values_input[] = {
	[PVR2_CVAL_INPUT_TV]        = "television",  /*xawtv needs this name*/
	[PVR2_CVAL_INPUT_RADIO]     = "radio",
	[PVR2_CVAL_INPUT_SVIDEO]    = "s-video",
	[PVR2_CVAL_INPUT_COMPOSITE] = "composite",
};


static const char *control_values_audiomode[] = {
	[PVR2_CVAL_AUDIOMODE_MONO]   = "Mono",
	[PVR2_CVAL_AUDIOMODE_STEREO] = "Stereo",
	[PVR2_CVAL_AUDIOMODE_SAP]    = "SAP",
	[PVR2_CVAL_AUDIOMODE_LANG1]  = "Lang1",
	[PVR2_CVAL_AUDIOMODE_LANG2]  = "Lang2",
};


static const char *control_values_hsm[] = {
	[PVR2_CVAL_HSM_FAIL] = "Fail",
	[PVR2_CVAL_HSM_HIGH] = "High",
	[PVR2_CVAL_HSM_FULL] = "Full",
};


#define VDEF(x) \
    .value_defs_ptr = x, \
    .value_defs_count = (sizeof(x)/sizeof(x[0]))

static int pvr2_ctl_set_chanprog_id(struct pvr2_hdw *hdw,int ctl_id,int value);
static int pvr2_ctl_get_chanprog_id(struct pvr2_hdw *hdw,int ctl_id);
static int pvr2_ctl_get_signal(struct pvr2_hdw *hdw,int ctl_id);
static int pvr2_ctl_get_streaming(struct pvr2_hdw *hdw,int ctl_id);
static int pvr2_ctl_get_hsm(struct pvr2_hdw *hdw,int ctl_id);
static int pvr2_ctl_get_subsys_mask(struct pvr2_hdw *hdw,int ctl_id);
static int pvr2_ctl_set_subsys_mask(struct pvr2_hdw *hdw,int ctl_id,int val);
static int pvr2_ctl_get_subsys_stream_mask(struct pvr2_hdw *hdw,int ctl_id);
static int pvr2_ctl_set_subsys_stream_mask(struct pvr2_hdw *hdw,int ctl_id,
                                           int val);

static struct pvr2_ctl_def control_defs[PVR2_CID_COUNT] =
{
	[PVR2_CID_BRIGHTNESS] = {
		.name = "Brightness",
		.min_value = 0,
		.max_value = 255,
		.default_value = 128,
	},
	[PVR2_CID_CONTRAST] = {
		.name = "Contrast",
		.min_value = 0,
		.max_value = 127,
		.default_value = 68,
	},
	[PVR2_CID_SATURATION] = {
		.name = "Saturation",
		.min_value = 0,
		.max_value = 127,
		.default_value = 64,
	},
	[PVR2_CID_HUE] = {
		.name = "Hue",
		.min_value = -128,
		.max_value = 127,
		.default_value = 0,
	},
	[PVR2_CID_VOLUME] = {
		.name = "Volume",
		.min_value = 0,
		.max_value = 65535,
		.default_value = 65535,
	},
	[PVR2_CID_BALANCE] = {
		.name = "Balance",
		.min_value = -32768,
		.max_value = 32767,
		.default_value = 0,
	},
	[PVR2_CID_BASS] = {
		.name = "Bass",
		.min_value = -32768,
		.max_value = 32767,
		.default_value = 0,
	},
	[PVR2_CID_TREBLE] = {
		.name = "Treble",
		.min_value = -32768,
		.max_value = 32767,
		.default_value = 0,
	},
	[PVR2_CID_MUTE] = {
		.name = "Mute",
		.min_value = 0,
		.max_value = 1,
		.default_value = 0,
	},
	[PVR2_CID_SRATE] = {
		.name = "Sample rate",
		.min_value = PVR2_CVAL_SRATE_MIN,
		.max_value = PVR2_CVAL_SRATE_MAX,
		.default_value = PVR2_CVAL_SRATE_48,
		VDEF(control_values_srate),
	},
	[PVR2_CID_AUDIOBITRATE] = {
		.name = "Audio Bitrate",
		.min_value = PVR2_CVAL_AUDIOBITRATE_MIN,
		.max_value = PVR2_CVAL_AUDIOBITRATE_MAX,
		.default_value = PVR2_CVAL_AUDIOBITRATE_224,
		VDEF(control_values_audiobitrate),
	},
	[PVR2_CID_AUDIOCRC] = {
		.name = "Audio CRC",
		.min_value = 0,
		.max_value = 1,
		.default_value = 1,
	},
	[PVR2_CID_AUDIOEMPHASIS] = {
		.name = "Audio Emphasis",
		.min_value = PVR2_CVAL_AUDIOEMPHASIS_MIN,
		.max_value = PVR2_CVAL_AUDIOEMPHASIS_MAX,
		.default_value = PVR2_CVAL_AUDIOEMPHASIS_NONE,
		VDEF(control_values_audioemphasis),
	},
	[PVR2_CID_VBR] = {
		.name = "Variable video bitrate",
		.min_value = 0,
		.max_value = 1,
		.default_value = 0,
	},
	[PVR2_CID_AVERAGEVIDEOBITRATE] = {
		.name = "Average video bitrate",
		.min_value = 1,
		.max_value = 20000000,
		.default_value = 6000000,
	},
	[PVR2_CID_PEAKVIDEOBITRATE] = {
		.name = "Peak video bitrate",
		.min_value = 1,
		.max_value = 20000000,
		.default_value = 6000000,
	},
	[PVR2_CID_VIDEOSTANDARD] = {
		.name = "Video Standard",
		.min_value = PVR2_CVAL_VIDEOSTANDARD_MIN,
		.max_value = PVR2_CVAL_VIDEOSTANDARD_MAX,
		.default_value = PVR2_CVAL_VIDEOSTANDARD_NTSC_M,
		VDEF(control_values_videostandard),
	},
	[PVR2_CID_INPUT] = {
		.name = "Video Source",
		.min_value = PVR2_CVAL_INPUT_MIN,
		.max_value = PVR2_CVAL_INPUT_MAX,
		.default_value = PVR2_CVAL_INPUT_TV,
		VDEF(control_values_input),
	},
	[PVR2_CID_AUDIOMODE] = {
		.name = "Audio Mode",
		.min_value = PVR2_CVAL_AUDIOMODE_MIN,
		.max_value = PVR2_CVAL_AUDIOMODE_MAX,
		.default_value = PVR2_CVAL_AUDIOMODE_STEREO,
		VDEF(control_values_audiomode),
	},
	[PVR2_CID_FREQUENCY] = {
		.name = "Tuner Frequency (Hz)",
		.min_value = 55250000L,
		.max_value = 801250000L,
		.default_value = 175250000L,
	},
	[PVR2_CID_HRES] = {
		.name = "Horizontal capture resolution",
		.min_value = 320,
		.max_value = 720,
		.default_value = 720,
	},
	[PVR2_CID_VRES] = {
		.name = "Vertical capture resolution",
		.min_value = 200,
		.max_value = 625,
		.default_value = 480,
	},
	[PVR2_CID_INTERLACE] = {
		.name = "Interlace mode",
		.min_value = 0,
		.max_value = 1,
		.default_value = 0,
	},
	[PVR2_CID_AUDIOLAYER] = {
		.name = "Audio Layer",
		.min_value = 0, /* This is all a wild guess */
		.max_value = 3,
		.default_value = 2, /* Appears to be all that is supported */
	},
	[PVR2_CID_CHANNEL] = {
		.name = "Channel",
		.min_value = 0,
		.max_value = FREQTABLE_SIZE,
		.default_value = 0,
	},
	[PVR2_CID_CHANPROG_ID] = {
		.name = "Channel Program ID",
		.min_value = 0,
		.max_value = FREQTABLE_SIZE,
		.default_value = 0,
	},
	[PVR2_CID_CHANPROG_FREQ] = {
		.name = "Channel Program Frequency",
		.min_value = 55250000L,
		.max_value = 801250000L,
		.skip_init = !0,
		.set_func = pvr2_ctl_set_chanprog_id,
		.get_func = pvr2_ctl_get_chanprog_id,
	},
	[PVR2_CID_SIGNAL_PRESENT] = {
		.name = "Signal Present",
		.min_value = 0,
		.max_value = 1,
		.get_func = pvr2_ctl_get_signal,
	},
	[PVR2_CID_STREAMING_ENABLED] = {
		.name = "Streaming Enabled",
		.min_value = 0,
		.max_value = 1,
		.get_func = pvr2_ctl_get_streaming,
	},
	[PVR2_CID_HSM] = {
		.name = "USB Speed",
		.min_value = PVR2_CVAL_HSM_MIN,
		.max_value = PVR2_CVAL_HSM_MAX,
		.get_func = pvr2_ctl_get_hsm,
		VDEF(control_values_hsm),
	},
	[PVR2_CID_SUBSYS_MASK] = {
		.name = "Subsystem enabled mask",
		.min_value = 0,
		.max_value = 0x7fffffff,
		.skip_init = !0,
		.get_func = pvr2_ctl_get_subsys_mask,
		.set_func = pvr2_ctl_set_subsys_mask,
	},
	[PVR2_CID_SUBSYS_STREAM_MASK] = {
		.name = "Subsystem stream mask",
		.min_value = 0,
		.max_value = 0x7fffffff,
		.skip_init = !0,
		.get_func = pvr2_ctl_get_subsys_stream_mask,
		.set_func = pvr2_ctl_set_subsys_stream_mask,
	},
};

#undef VDEF


const char *pvr2_config_get_name(enum pvr2_config cfg)
{
	switch (cfg) {
	case pvr2_config_empty: return "empty";
	case pvr2_config_mpeg: return "mpeg";
	case pvr2_config_vbi: return "vbi";
	case pvr2_config_radio: return "radio";
	}
	return "<unknown>";
}


struct usb_device *pvr2_hdw_get_dev(struct pvr2_hdw *hdw)
{
	return hdw->usb_dev;
}


unsigned long pvr2_hdw_get_sn(struct pvr2_hdw *hdw)
{
	return hdw->serial_number;
}


struct pvr2_hdw *pvr2_hdw_find(int unit_number)
{
	if (unit_number < 0) return 0;
	if (unit_number >= PVR_NUM) return 0;
	return unit_pointers[unit_number];
}


int pvr2_hdw_get_unit_number(struct pvr2_hdw *hdw)
{
	return hdw->unit_number;
}


/*
 * pvr2_upload_firmware1().
 *
 * Send the 8051 firmware to the device.  After the upload, arrange for
 * device to re-enumerate.
 *
 * NOTE : the pointer to the firmware data given by request_firmware()
 * is not suitable for an usb transaction.
 *
 */
int pvr2_upload_firmware1(struct pvr2_hdw *hdw)
{
	const struct firmware *fw_entry = 0;
	void  *fw_ptr;
	unsigned int pipe;
	int ret;
	u16 address;
	const char *firmware_file = FIRMWARE1_FILE;

	trace_firmware("pvr2_upload_firmware1");

	usb_settoggle(hdw->usb_dev, 0 & 0xf, !(0 & USB_DIR_IN), 0);
	usb_clear_halt(hdw->usb_dev, usb_sndbulkpipe(hdw->usb_dev, 0 & 0x7f));

	pipe = usb_sndctrlpipe(hdw->usb_dev, 0);
	ret = request_firmware(&fw_entry, firmware_file, &hdw->usb_dev->dev);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "request_firmware failed for '%s'", firmware_file);
		return ret;
	}

	if (fw_entry->size != 0x2000){
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,"wrong fx2 firmware size");
		release_firmware(fw_entry);
		return -ENOMEM;
	}

	fw_ptr = kmalloc(0x800, GFP_KERNEL);
	if (fw_ptr == NULL){
		release_firmware(fw_entry);
		return -ENOMEM;
	}

	/* We have to hold the CPU during firmware upload. */
	pvr2_hdw_cpureset_assert(hdw,1);

	/* upload the firmware to address 0000-1fff in 2048 (=0x800) bytes
	   chunk. */

	ret = 0;
	for(address = 0; address < fw_entry->size; address += 0x800) {
		memcpy(fw_ptr, fw_entry->data + address, 0x800);
		ret += usb_control_msg(hdw->usb_dev, pipe, 0xa0, 0x40, address,
				       0, fw_ptr, 0x800, HZ);
	}

	trace_firmware("Upload done, releasing device's CPU");

	/* Now release the CPU.  It will disconnect and reconnect later. */
	pvr2_hdw_cpureset_assert(hdw,0);

	kfree(fw_ptr);
	release_firmware(fw_entry);

	trace_firmware("Upload done (%d bytes sent)",ret);

	/* We should have written 8192 bytes */
	return (ret == 8192) ? 0 : -EIO;
}


/*
 * pvr2_upload_firmware2()
 *
 * This uploads encoder firmware on endpoint 2.
 *
 */

int pvr2_upload_firmware2(struct pvr2_hdw *hdw)
{
	const struct firmware *fw_entry = 0;
	void  *fw_ptr;
	unsigned int pipe, fw_len, fw_done;
	int actual_length;
	int ret = 0;

	trace_firmware("pvr2_upload_firmware2");

	/* First prepare firmware loading */
	ret |= pvr2_hdw_cmd_soft_reset(hdw);
	ret |= pvr2_write_register(hdw, 0x0048, 0xffffffff); /*interrupt mask*/
	ret |= pvr2_hdw_gpio_chg_dir(hdw,0xffffffff,0x00000088); /*gpio dir*/
	ret |= pvr2_hdw_gpio_chg_out(hdw,0xffffffff,0x00000008); /*gpio output state*/
	ret |= pvr2_hdw_cmd_deep_reset(hdw);
	ret |= pvr2_write_register(hdw, 0xa064, 0x00000000); /*APU command*/
	ret |= pvr2_hdw_gpio_chg_dir(hdw,0xffffffff,0x00000408); /*gpio dir*/
	ret |= pvr2_hdw_gpio_chg_out(hdw,0xffffffff,0x00000008); /*gpio output state*/
	ret |= pvr2_write_register(hdw, 0x9058, 0xffffffed); /*VPU ctrl*/
	ret |= pvr2_write_register(hdw, 0x9054, 0xfffffffd); /*reset hw blocks*/
	ret |= pvr2_write_register(hdw, 0x07f8, 0x80000800); /*encoder SDRAM refresh*/
	ret |= pvr2_write_register(hdw, 0x07fc, 0x0000001a); /*encoder SDRAM pre-charge*/
	ret |= pvr2_write_register(hdw, 0x0700, 0x00000000); /*I2C clock*/
	ret |= pvr2_write_register(hdw, 0xaa00, 0x00000000); /*unknown*/
	ret |= pvr2_write_register(hdw, 0xaa04, 0x00057810); /*unknown*/
	ret |= pvr2_write_register(hdw, 0xaa10, 0x00148500); /*unknown*/
	ret |= pvr2_write_register(hdw, 0xaa18, 0x00840000); /*unknown*/
	ret |= pvr2_write_u8(hdw, 0x52, 0);
	ret |= pvr2_write_u16(hdw, 0x0600, 0);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "firmware2 upload prep failed, ret=%d",ret);
		return ret;
	}

	/* Now send firmware */

	ret = request_firmware(&fw_entry, FIRMWARE2_FILE, &hdw->usb_dev->dev);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "request_firmware failed for '%s'", FIRMWARE2_FILE);
		return ret;
	}

	fw_len = fw_entry->size;

	if (fw_len % FIRMWARE_CHUNK_SIZE) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "size of %s must be a multiple of 8192B",
			   FIRMWARE2_FILE);
		release_firmware(fw_entry);
		return -1;
	}

	fw_ptr = kmalloc(FIRMWARE_CHUNK_SIZE, GFP_KERNEL);
	if (fw_ptr == NULL){
		release_firmware(fw_entry);
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "failed to allocate memory for firmware2 upload");
		return -ENOMEM;
	}

	pipe = usb_sndbulkpipe(hdw->usb_dev, PVR2_FIRMWARE_ENDPOINT);

	for (fw_done = 0 ; (fw_done < fw_len) && !ret ;
	     fw_done += FIRMWARE_CHUNK_SIZE ) {
		int i;
		memcpy(fw_ptr, fw_entry->data + fw_done, FIRMWARE_CHUNK_SIZE);
		/* Usbsnoop log  shows that we must swap bytes... */
		for (i = 0; i < FIRMWARE_CHUNK_SIZE/4 ; i++)
			((u32 *)fw_ptr)[i] = ___swab32(((u32 *)fw_ptr)[i]);

		ret |= usb_bulk_msg(hdw->usb_dev, pipe, fw_ptr,
				    FIRMWARE_CHUNK_SIZE,
				    &actual_length, HZ);
		ret |= (actual_length != FIRMWARE_CHUNK_SIZE);
	}

	trace_firmware("upload of %s : %i / %i ",
		       FIRMWARE2_FILE,fw_done,fw_len);

	kfree(fw_ptr);
	release_firmware(fw_entry);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "firmware2 upload transfer failure");
		return ret;
	}

	/* Finish upload */

	ret |= pvr2_write_register(hdw, 0x9054, 0xffffffff); /*reset hw blocks*/
	ret |= pvr2_write_register(hdw, 0x9058, 0xffffffe8); /*VPU ctrl*/
	ret |= pvr2_write_u16(hdw, 0x0600, 0);

	if (ret) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "firmware2 upload post-proc failure");
	} else {
		hdw->subsys_enabled_mask |= PVR2_SUBSYS_ENC_FIRMWARE;
	}
	return ret;
}


#define FIRMWARE_RECOVERY_BITS \
	(PVR2_SUBSYS_ENC_CFG | \
	 PVR2_SUBSYS_ENC_RUN | \
	 PVR2_SUBSYS_ENC_FIRMWARE | \
	 PVR2_SUBSYS_USBSTREAM_RUN)

/*

  This single function is key to pretty much everything.  The pvrusb2
  device can logically be viewed as a series of subsystems which can be
  stopped / started or unconfigured / configured.  To get things streaming,
  one must configure everything and start everything, but there may be
  various reasons over time to deconfigure something or stop something.
  This function handles all of this activity.  Everything EVERYWHERE that
  must affect a subsystem eventually comes here to do the work.

  The current state of all subsystems is represented by a single bit mask,
  known as subsys_enabled_mask.  The bit positions are defined by the
  PVR2_SUBSYS_xxxx macros, with one subsystem per bit position.  At any
  time the set of configured or active subsystems can be queried just by
  looking at that mask.  To change bits in that mask, this function here
  must be called.  The "msk" argument indicates which bit positions to
  change, and the "val" argument defines the new values for the positions
  defined by "msk".

  There is a priority ordering of starting / stopping things, and for
  multiple requested changes, this function implements that ordering.
  (Thus we will act on a request to load encoder firmware before we
  configure the encoder.)  In addition to priority ordering, there is a
  recovery strategy implemented here.  If a particular step fails and we
  detect that failure, this function will clear the affected subsystem bits
  and restart.  Thus we have a means for recovering from a dead encoder:
  Clear all bits that correspond to subsystems that we need to restart /
  reconfigure and start over.

*/
void pvr2_hdw_subsys_bit_chg_no_lock(struct pvr2_hdw *hdw,
				     unsigned long msk,unsigned long val)
{
	unsigned long nmsk;
	unsigned long vmsk;
	int ret;
	unsigned int tryCount = 0;

	if (!hdw->flag_ok) return;

	msk &= PVR2_SUBSYS_ALL;

	for (;;) {
		tryCount++;
		vmsk = hdw->subsys_enabled_mask & PVR2_SUBSYS_ALL;
		nmsk = (vmsk & ~msk) | (val & msk);
		if (!(nmsk ^ vmsk)) break;
		if (tryCount > 4) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "Too many retries when configuring device;"
				   " giving up");
			pvr2_hdw_render_useless(hdw);
			break;
		}
		if (tryCount > 1) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "Retrying device reconfiguration");
		}
		pvr2_trace(PVR2_TRACE_INIT,
			   "subsys mask changing 0x%lx:0x%lx"
			   " from 0x%lx to 0x%lx",
			   msk,val,hdw->subsys_enabled_mask,nmsk);

		vmsk = (nmsk ^ hdw->subsys_enabled_mask) &
			hdw->subsys_enabled_mask;
		if (vmsk) {
			if (vmsk & PVR2_SUBSYS_ENC_RUN) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_encoder_stop");
				ret = pvr2_encoder_stop(hdw);
				if (ret) {
					pvr2_trace(PVR2_TRACE_ERROR_LEGS,
						   "Error recovery initiated");
					hdw->subsys_enabled_mask &=
						~FIRMWARE_RECOVERY_BITS;
					continue;
				}
			}
			if (vmsk & PVR2_SUBSYS_USBSTREAM_RUN) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_hdw_cmd_usbstream(0)");
				pvr2_hdw_cmd_usbstream(hdw,0);
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_RUN) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_disable_output");
				pvr2_decoder_disable_output(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_CFG_ALL) {
				hdw->subsys_enabled_mask &=
					~(vmsk & PVR2_SUBSYS_CFG_ALL);
			}
		}
		vmsk = (nmsk ^ hdw->subsys_enabled_mask) & nmsk;
		if (vmsk) {
			if (vmsk & PVR2_SUBSYS_ENC_FIRMWARE) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_upload_firmware2");
				ret = pvr2_upload_firmware2(hdw);
				if (ret) {
					pvr2_trace(PVR2_TRACE_ERROR_LEGS,
						   "Failure uploading encoder"
						   " firmware");
					pvr2_hdw_render_useless(hdw);
					break;
				}
			}
			if (vmsk & PVR2_SUBSYS_TUNER_CFG_STD) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_tuner_set_standard");
				pvr2_tuner_set_standard(hdw);
			}
			if (vmsk & PVR2_SUBSYS_TUNER_CFG_FREQ) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_tuner_set_freq");
				pvr2_tuner_set_freq(hdw);
			}
			if (vmsk & PVR2_SUBSYS_AUDIO_CFG_STD) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_audio_set_standard");
				pvr2_audio_set_standard(hdw);
			}
			if (vmsk & PVR2_SUBSYS_AUDIO_CFG_MODE) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_audio_set_stereo");
				pvr2_audio_set_stereo(hdw);
			}
			if (vmsk & PVR2_SUBSYS_AUDIO_CFG_VBBTM) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_audio_setvolume");
				pvr2_audio_setvolume(hdw);
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_CFG_NORM) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_set_norm");
				pvr2_decoder_set_norm(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_CFG_INPUT) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_set_input");
				pvr2_decoder_set_input(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_CFG_SIZE) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_set_size");
				pvr2_decoder_set_size(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_CFG_AUDIO) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_set_audio");
				pvr2_decoder_set_audio(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_CFG_BCSH) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_set_bcsh");
				pvr2_decoder_set_bcsh(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_ENC_CFG) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_encoder_configure");
				ret = pvr2_encoder_configure(hdw);
				if (ret) {
					pvr2_trace(PVR2_TRACE_ERROR_LEGS,
						   "Error recovery initiated");
					hdw->subsys_enabled_mask &=
						~FIRMWARE_RECOVERY_BITS;
					continue;
				}
			}
			if (vmsk & PVR2_SUBSYS_DIGITIZER_RUN) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_decoder_enable_output");
				pvr2_decoder_enable_output(hdw->dcp);
			}
			if (vmsk & PVR2_SUBSYS_USBSTREAM_RUN) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_hdw_cmd_usbstream(1)");
				pvr2_hdw_cmd_usbstream(hdw,!0);
			}
			if (vmsk & PVR2_SUBSYS_ENC_RUN) {
				pvr2_trace(PVR2_TRACE_CTL,
					   "/*---TRACE_CTL----*/"
					   " pvr2_encoder_start");
				ret = pvr2_encoder_start(hdw);
				if (ret) {
					pvr2_trace(PVR2_TRACE_ERROR_LEGS,
						   "Error recovery initiated");
					hdw->subsys_enabled_mask &=
						~FIRMWARE_RECOVERY_BITS;
					continue;
				}
			}
		}
	}
}


void pvr2_hdw_subsys_bit_chg(struct pvr2_hdw *hdw,
			     unsigned long msk,unsigned long val)
{
	LOCK_TAKE(hdw->big_lock); do {
		pvr2_hdw_subsys_bit_chg_no_lock(hdw,msk,val);
	} while (0); LOCK_GIVE(hdw->big_lock);
}


void pvr2_hdw_subsys_bit_set(struct pvr2_hdw *hdw,unsigned long msk)
{
	pvr2_hdw_subsys_bit_chg(hdw,msk,msk);
}


void pvr2_hdw_subsys_bit_clr(struct pvr2_hdw *hdw,unsigned long msk)
{
	pvr2_hdw_subsys_bit_chg(hdw,msk,0);
}


unsigned long pvr2_hdw_subsys_get(struct pvr2_hdw *hdw)
{
	return hdw->subsys_enabled_mask;
}


unsigned long pvr2_hdw_subsys_stream_get(struct pvr2_hdw *hdw)
{
	return hdw->subsys_stream_mask;
}


void pvr2_hdw_subsys_stream_bit_chg_no_lock(struct pvr2_hdw *hdw,
                                            unsigned long msk,
                                            unsigned long val)
{
	unsigned long val2;
	msk &= PVR2_SUBSYS_ALL;
	val2 = ((hdw->subsys_stream_mask & ~msk) | (val & msk));
	pvr2_trace(PVR2_TRACE_INIT,
		   "stream mask changing 0x%lx:0x%lx from 0x%lx to 0x%lx",
		   msk,val,hdw->subsys_stream_mask,val2);
	hdw->subsys_stream_mask = val2;
}


void pvr2_hdw_subsys_stream_bit_chg(struct pvr2_hdw *hdw,
                                    unsigned long msk,
                                    unsigned long val)
{
	LOCK_TAKE(hdw->big_lock); do {
		pvr2_hdw_subsys_stream_bit_chg_no_lock(hdw,msk,val);
	} while (0); LOCK_GIVE(hdw->big_lock);
}


static int pvr2_ctl_get_streaming(struct pvr2_hdw *hdw,int ctl_id)
{
	return hdw->flag_streaming_enabled != 0;
}


int pvr2_hdw_set_streaming_no_lock(struct pvr2_hdw *hdw,int enableFl)
{
	if ((!enableFl) == !(hdw->flag_streaming_enabled)) return 0;
	if (enableFl) {
		pvr2_trace(PVR2_TRACE_START_STOP,
			   "/*--TRACE_STREAM--*/ enable");
		pvr2_hdw_subsys_bit_chg_no_lock(hdw,~0,~0);
	} else {
		pvr2_trace(PVR2_TRACE_START_STOP,
			   "/*--TRACE_STREAM--*/ disable");
		pvr2_hdw_subsys_bit_chg_no_lock(hdw,hdw->subsys_stream_mask,0);
	}
	if (!hdw->flag_ok) return -EIO;
	hdw->flag_streaming_enabled = enableFl != 0;
	return 0;
}


int pvr2_hdw_get_streaming(struct pvr2_hdw *hdw)
{
	return hdw->flag_streaming_enabled != 0;
}


int pvr2_hdw_set_streaming(struct pvr2_hdw *hdw,int enable_flag)
{
	int ret;
	LOCK_TAKE(hdw->big_lock); do {
		ret = pvr2_hdw_set_streaming_no_lock(hdw,enable_flag);
	} while (0); LOCK_GIVE(hdw->big_lock);
	return ret;
}


int pvr2_hdw_set_stream_type_no_lock(struct pvr2_hdw *hdw,
                                     enum pvr2_config config)
{
	unsigned long sm = hdw->subsys_enabled_mask;
	if (!hdw->flag_ok) return -EIO;
	pvr2_hdw_subsys_bit_chg_no_lock(hdw,hdw->subsys_stream_mask,0);
	hdw->config = config;
	pvr2_hdw_subsys_bit_chg_no_lock(hdw,~0,sm);
	return 0;
}


int pvr2_hdw_set_stream_type(struct pvr2_hdw *hdw,enum pvr2_config config)
{
	int ret;
	if (!hdw->flag_ok) return -EIO;
	LOCK_TAKE(hdw->big_lock);
	ret = pvr2_hdw_set_stream_type_no_lock(hdw,config);
	LOCK_GIVE(hdw->big_lock);
	return ret;
}


static void pvr2_hdw_setup_low(struct pvr2_hdw *hdw)
{
	unsigned int idx;
	if (hdw->usb_intf->cur_altsetting->desc.bNumEndpoints == 0) {
		if (pvr2_upload_firmware1(hdw) != 0) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "Failure uploading firmware1");
		}
		return;
	}

	if (initusbreset) {
		pvr2_hdw_device_reset(hdw);
	}
	if (!pvr2_hdw_dev_ok(hdw)) return;

	pvr2_i2c_init(hdw);
	if (!pvr2_hdw_dev_ok(hdw)) return;

	if (pvr2_upload_firmware2(hdw)){
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,"device unstable!!");
		pvr2_hdw_render_useless(hdw);
		return;
	}

	for (idx = 0; idx < PVR2_CID_COUNT; idx++) {
		if (control_defs[idx].skip_init) continue;
		pvr2_hdw_set_ctl_value_internal(
			hdw,idx,control_defs[idx].default_value);
	}

	pvr2_reset_ctl_endpoints(hdw);
	if (!pvr2_hdw_dev_ok(hdw)) return;

	hdw->dcp = pvr2_decoder_create(hdw);

	pvr2_eeprom_analyze(hdw);
	if (!pvr2_hdw_dev_ok(hdw)) return;

	if (!pvr2_tuner_get_default_type(hdw)) {
		pvr2_trace(PVR2_TRACE_INIT,
			   "pvr2_hdw_setup: Tuner type overridden to %d",
			   hdw->tuner_type);
	}

	pvr2_tuner_set_type(hdw);
	if (!pvr2_hdw_dev_ok(hdw)) return;

	pvr2_eeprom_set_default_standard(hdw);
	if (!pvr2_hdw_dev_ok(hdw)) return;

	pvr2_hdw_commit_ctl_internal(hdw);
	if (!pvr2_hdw_dev_ok(hdw)) return;

	hdw->vid_stream = pvr2_stream_create();
	if (!pvr2_hdw_dev_ok(hdw)) return;
	pvr2_trace(PVR2_TRACE_INIT,
		   "pvr2_hdw_setup: video stream is %p",hdw->vid_stream);
	if (hdw->vid_stream) {
		pvr2_stream_setup(hdw->vid_stream,hdw->usb_dev,
				  PVR2_VID_ENDPOINT);
	}

	if (!pvr2_hdw_dev_ok(hdw)) return;
	hdw->flag_init_ok = !0;
}


int pvr2_hdw_setup(struct pvr2_hdw *hdw)
{
	pvr2_trace(PVR2_TRACE_INIT,"pvr2_hdw_setup(hdw=%p) begin",hdw);
	LOCK_TAKE(hdw->big_lock); do {
		pvr2_hdw_setup_low(hdw);
		pvr2_trace(PVR2_TRACE_INIT,
			   "pvr2_hdw_setup(hdw=%p) done, ok=%d init_ok=%d",
			   hdw,hdw->flag_ok,hdw->flag_init_ok);
		if (pvr2_hdw_dev_ok(hdw)) {
			if (pvr2_hdw_init_ok(hdw)) {
				pvr2_trace(
					PVR2_TRACE_INFO,
					"Device initialization"
					" completed successfully.");
			} else {
				pvr2_trace(
					PVR2_TRACE_INFO,
					"Device firmware (re)load executed;"
					" it should now reset and reconnect.");
			}
			break;
		}
		if (procreload) {
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"Attempting pvrusb2 recovery by reloading"
				" primary firmware.");
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"If this works, device should disconnect"
				" and reconnect in a sane state.");
			pvr2_upload_firmware1(hdw);
		} else {
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"***WARNING*** pvrusb2 device hardware"
				" appears to be jammed"
				" and I can't clear it.");
			pvr2_trace(
				PVR2_TRACE_ERROR_LEGS,
				"You might need to power cycle"
				" the pvrusb2 device"
				" in order to recover.");
		}
	} while (0); LOCK_GIVE(hdw->big_lock);
	return hdw->flag_init_ok;
}


/* Create and return a structure for interacting with the underlying
   hardware */
struct pvr2_hdw *pvr2_hdw_create(struct usb_interface *intf)
{
	unsigned int idx,cnt1,cnt2;
	struct pvr2_hdw *hdw;
	__u8 ifnum;

	hdw = kmalloc(sizeof(*hdw),GFP_KERNEL);
	pvr2_trace(PVR2_TRACE_INIT,"pvr2_hdw_create: hdw=%p",hdw);
	if (!hdw) goto fail;
	memset(hdw,0,sizeof(*hdw));
	hdw->unit_number = -1;
	hdw->v4l_minor_number = -1;
	hdw->ctl_write_buffer = kmalloc(PVR2_CTL_BUFFSIZE,GFP_KERNEL);
	if (!hdw->ctl_write_buffer) goto fail;
	hdw->ctl_read_buffer = kmalloc(PVR2_CTL_BUFFSIZE,GFP_KERNEL);
	if (!hdw->ctl_read_buffer) goto fail;
	hdw->ctl_write_urb = usb_alloc_urb(0,GFP_KERNEL);
	if (!hdw->ctl_write_urb) goto fail;
	hdw->ctl_read_urb = usb_alloc_urb(0,GFP_KERNEL);
	if (!hdw->ctl_read_urb) goto fail;

	down(&pvr2_unit_sem); do {
		for (idx = 0; idx < PVR_NUM; idx++) {
			if (unit_pointers[idx]) continue;
			hdw->unit_number = idx;
			unit_pointers[idx] = hdw;
			break;
		}
	} while (0); up(&pvr2_unit_sem);

	cnt1 = 0;
	cnt2 = scnprintf(hdw->name+cnt1,sizeof(hdw->name)-cnt1,"pvrusb2");
	cnt1 += cnt2;
	if (hdw->unit_number >= 0) {
		cnt2 = scnprintf(hdw->name+cnt1,sizeof(hdw->name)-cnt1,"_%c",
				 ('a' + hdw->unit_number));
		cnt1 += cnt2;
	}
	if (cnt1 >= sizeof(hdw->name)) cnt1 = sizeof(hdw->name)-1;
	hdw->name[cnt1] = 0;

	pvr2_trace(PVR2_TRACE_INIT,"Driver unit number is %d, name is %s",
		   hdw->unit_number,hdw->name);

	hdw->tuner_type = -1;
	hdw->flag_ok = !0;
	/* Initialize the mask of subsystems that we will shut down when we
	   stop streaming. */
	hdw->subsys_stream_mask = PVR2_SUBSYS_RUN_ALL;
	hdw->subsys_stream_mask |= PVR2_SUBSYS_ENC_CFG;

	pvr2_trace(PVR2_TRACE_INIT,"subsys_stream_mask: 0x%lx",
		   hdw->subsys_stream_mask);

	hdw->usb_intf = intf;
	hdw->usb_dev = interface_to_usbdev(intf);

	ifnum = hdw->usb_intf->cur_altsetting->desc.bInterfaceNumber;
	usb_set_interface(hdw->usb_dev,ifnum,0);

	init_MUTEX(&hdw->ctl_lock_sem);
	init_MUTEX(&hdw->big_lock_sem);


	return hdw;
 fail:
	if (hdw) {
		if (hdw->ctl_read_urb) usb_free_urb(hdw->ctl_read_urb);
		if (hdw->ctl_write_urb) usb_free_urb(hdw->ctl_write_urb);
		if (hdw->ctl_read_buffer) kfree(hdw->ctl_read_buffer);
		if (hdw->ctl_write_buffer) kfree(hdw->ctl_write_buffer);
		kfree(hdw);
	}
	return 0;
}


/* Remove _all_ associations between this driver and the underlying USB
   layer. */
void pvr2_hdw_remove_usb_stuff(struct pvr2_hdw *hdw)
{
	if (hdw->flag_disconnected) return;
	pvr2_trace(PVR2_TRACE_INIT,"pvr2_hdw_remove_usb_stuff: hdw=%p",hdw);
	if (hdw->ctl_read_urb) {
		usb_kill_urb(hdw->ctl_read_urb);
		usb_free_urb(hdw->ctl_read_urb);
		hdw->ctl_read_urb = 0;
	}
	if (hdw->ctl_write_urb) {
		usb_kill_urb(hdw->ctl_write_urb);
		usb_free_urb(hdw->ctl_write_urb);
		hdw->ctl_write_urb = 0;
	}
	if (hdw->ctl_read_buffer) {
		kfree(hdw->ctl_read_buffer);
		hdw->ctl_read_buffer = 0;
	}
	if (hdw->ctl_write_buffer) {
		kfree(hdw->ctl_write_buffer);
		hdw->ctl_write_buffer = 0;
	}
	pvr2_hdw_render_useless_unlocked(hdw);
	hdw->flag_disconnected = !0;
	hdw->usb_dev = 0;
	hdw->usb_intf = 0;
}


/* Destroy hardware interaction structure */
void pvr2_hdw_destroy(struct pvr2_hdw *hdw)
{
	pvr2_trace(PVR2_TRACE_INIT,"pvr2_hdw_destroy: hdw=%p",hdw);
	if (hdw->fw_buffer) {
		kfree(hdw->fw_buffer);
		hdw->fw_buffer = 0;
	}
	if (hdw->vid_stream) {
		pvr2_stream_destroy(hdw->vid_stream);
		hdw->vid_stream = 0;
	}
	pvr2_decoder_destroy(hdw->dcp);
	hdw->dcp = 0;
	pvr2_i2c_done(hdw);
	pvr2_hdw_remove_usb_stuff(hdw);
	down(&pvr2_unit_sem); do {
		if ((hdw->unit_number >= 0) &&
		    (hdw->unit_number < PVR_NUM) &&
		    (unit_pointers[hdw->unit_number] == hdw)) {
			unit_pointers[hdw->unit_number] = 0;
		}
	} while (0); up(&pvr2_unit_sem);
	kfree(hdw);
}


int pvr2_hdw_init_ok(struct pvr2_hdw *hdw)
{
	return hdw->flag_init_ok;
}


int pvr2_hdw_dev_ok(struct pvr2_hdw *hdw)
{
	return (hdw && hdw->flag_ok);
}


/* Called when hardware has been unplugged */
void pvr2_hdw_disconnect(struct pvr2_hdw *hdw)
{
	pvr2_trace(PVR2_TRACE_INIT,"pvr2_hdw_disconnect(hdw=%p)",hdw);
	LOCK_TAKE(hdw->big_lock);
	LOCK_TAKE(hdw->ctl_lock);
	pvr2_hdw_remove_usb_stuff(hdw);
	LOCK_GIVE(hdw->ctl_lock);
	LOCK_GIVE(hdw->big_lock);
}


static int pvr2_ctl_set_chanprog_id(struct pvr2_hdw *hdw,int ctl_id,int value)
{
	/* This is a special case; the value to store is to an array, and
	   the element to select is determined by PVR_CID_CHANPROG_ID. */
	int id = hdw->controls[PVR2_CID_CHANPROG_ID].value;
	if ((id < 1) || (id > FREQTABLE_SIZE)) return 0;
	hdw->freqTable[id-1] = value;
	if (hdw->controls[PVR2_CID_CHANNEL].value == id) {
		/* If the current channel happens to be the slot we just
		   set, then act like the current channel just got changed
		   so we'll update that too. */
		hdw->controls[PVR2_CID_CHANNEL].dirty = !0;
	}
	return 0;
}


static int pvr2_ctl_get_chanprog_id(struct pvr2_hdw *hdw,int ctl_id)
{
	/* This is a special case; the value to return is from an array,
	   and the element to select is determined by
	   PVR_CID_CHANPROG_ID. */
	int id = hdw->controls[PVR2_CID_CHANPROG_ID].value;
	if ((id < 1) || (id > FREQTABLE_SIZE)) return 0;
	return hdw->freqTable[id-1];
}


/* Retrieve current value for a given control */
int pvr2_hdw_get_ctl_value(struct pvr2_hdw *hdw,unsigned int ctl_id)
{
	int ret = 0;

	if (ctl_id >= PVR2_CID_COUNT) return 0;
	LOCK_TAKE(hdw->big_lock); do {
		if (control_defs[ctl_id].get_func) {
			ret = control_defs[ctl_id].get_func(hdw,ctl_id);
			break;
		}
		ret = hdw->controls[ctl_id].value;
	} while(0); LOCK_GIVE(hdw->big_lock);
	return ret;
}


/* Return true if control is writable */
int pvr2_hdw_get_ctl_rw(struct pvr2_hdw *hdw,unsigned int ctl_id)
{
	if (ctl_id >= PVR2_CID_COUNT) return 0;
	if (control_defs[ctl_id].get_func && !control_defs[ctl_id].set_func) {
		return 0;
	}
	return !0;
}


/* Retrieve legal minimum value for a given control */
int pvr2_hdw_get_ctl_min_value(struct pvr2_hdw *hdw,unsigned int ctl_id)
{
	if (ctl_id >= PVR2_CID_COUNT) return 0;
	return control_defs[ctl_id].min_value;
}


/* Retrieve legal maximum value for a given control */
int pvr2_hdw_get_ctl_max_value(struct pvr2_hdw *hdw,unsigned int ctl_id)
{
	if (ctl_id >= PVR2_CID_COUNT) return 0;
	return control_defs[ctl_id].max_value;
}


/* Set current value for given control - normally this is just stored and
   the hardware isn't updated until the commit function is called. */
int pvr2_hdw_set_ctl_value_internal(struct pvr2_hdw *hdw,
                                    unsigned int ctl_id,int value)
{
	if (ctl_id >= PVR2_CID_COUNT) return -EINVAL;
	if (value < control_defs[ctl_id].min_value) return -EINVAL;
	if (value > control_defs[ctl_id].max_value) return -EINVAL;
	if (control_defs[ctl_id].set_func) {
		return control_defs[ctl_id].set_func(hdw,ctl_id,value);
	} else if (control_defs[ctl_id].get_func) {
		/* If there's no "set" function yet there is still a "get"
		   function, then treat this as a read-only value. */
		return -EINVAL;
	}
	hdw->controls[ctl_id].value = value;
	hdw->controls[ctl_id].dirty = !0;
	return 0;
}


/* Set current value for given control - this is just stored; the hardware
   isn't updated until the commit function is called. */
int pvr2_hdw_set_ctl_value(struct pvr2_hdw *hdw,unsigned int ctl_id,int value)
{
	int ret;
	LOCK_TAKE(hdw->big_lock); do {
		ret = pvr2_hdw_set_ctl_value_internal(hdw,ctl_id,value);
	} while (0); LOCK_GIVE(hdw->big_lock);
	return ret;
}


/* Retrieve string name for a given control value (returns a null pointer
   for any invalid combinations). */
const char *pvr2_hdw_get_ctl_value_name(struct pvr2_hdw *hdw,
                                        unsigned int ctl_id,
                                        int value)
{
	struct pvr2_ctl_def *cdef;
	if (ctl_id >= PVR2_CID_COUNT) return 0;
	cdef = control_defs + ctl_id;
	if (! cdef->value_defs_ptr) return 0;
	if (value >= cdef->value_defs_count) return 0;
	return cdef->value_defs_ptr[value];
}


/* Retrieve string name for given control */
const char *pvr2_hdw_get_ctl_name(struct pvr2_hdw *hdw,unsigned int ctl_id)
{
	if (ctl_id >= PVR2_CID_COUNT) return 0;
	return control_defs[ctl_id].name;
}


/* Commit all control changes made up to this point.  Subsystems can be
   indirectly affected by these changes.  For a given set of things being
   committed, we'll clear the affected subsystem bits and then once we're
   done committing everything we'll make a request to restore the subsystem
   state(s) back to their previous value before this function was called.
   Thus we can automatically reconfigure affected pieces of the driver as
   controls are changed. */
int pvr2_hdw_commit_ctl_internal(struct pvr2_hdw *hdw)
{
	unsigned long saved_subsys_mask = hdw->subsys_enabled_mask;
	unsigned long stale_subsys_mask = 0;
	unsigned int idx;
	int value;
	const char *ctl_name;
	const char *ctl_value;
	int commit_flag = 0;

	/* Let's see if the channel changed and we have to update the
	   frequency because of it.  This setup means one can tune the
	   receiver either by just setting the channel (using the frequency
	   table), or by directly programming the frequency.  How do we
	   resolve the obvious conflict here?  The direct frequency takes
	   priority; if directly set then we commit that value and force
	   the channel to zero which is interpreted to mean "none".  If on
	   the other hand we see that the channel has been set and it's a
	   legal value, then we copy that into the frequency this.  The
	   metaphor here is similar to when you tune your digital radio:
	   You an either set a frequency directly or punch up a
	   pre-programmed station.  Either way a frequency is set, and if
	   you do use a preset, then the radio also shows you which preset
	   it is - until you override that by directly entering a new
	   frequency. */
	if (hdw->controls[PVR2_CID_FREQUENCY].dirty) {
		/* Frequency has been directly set, so clear out the
		   channel. */
		hdw->controls[PVR2_CID_CHANNEL].value = 0;
	} else if (hdw->controls[PVR2_CID_CHANNEL].dirty) {
		int id = hdw->controls[PVR2_CID_CHANNEL].value;
		if ((id > 0) && (id <= FREQTABLE_SIZE)) {
			if (hdw->controls[PVR2_CID_FREQUENCY].value !=
			    hdw->freqTable[id-1]) {
				hdw->controls[PVR2_CID_FREQUENCY].value =
					hdw->freqTable[id-1];
				hdw->controls[PVR2_CID_FREQUENCY].dirty = !0;
			}
		}
	}

	for (idx = 0; idx < PVR2_CID_COUNT; idx++) {
		if (!hdw->controls[idx].dirty) continue;
		if (!commit_flag) {
			commit_flag = !0;
		}
		value = hdw->controls[idx].value;
		ctl_name = control_defs[idx].name;
		if (control_defs[idx].value_defs_ptr) {
			if (value < control_defs[idx].value_defs_count) {
				ctl_value =
					control_defs[idx].value_defs_ptr[value];
			} else {
				ctl_value = "<out of range>";
			}
		} else {
			ctl_value = "<integer>";
		}
		pvr2_trace(PVR2_TRACE_CTL,
			   "/*--TRACE_COMMIT--*/ \"%s\" <-- %d (%s)",
			   ctl_name,value,ctl_value);
	}

	if (!commit_flag) {
		/* Nothing has changed */
		return 0;
	}

	/* When video standard changes, reset the hres and vres values -
	   but if the user has pending changes there, then let the changes
	   take priority. */
	if (hdw->controls[PVR2_CID_VIDEOSTANDARD].dirty) {
		if (!hdw->controls[PVR2_CID_VRES].dirty) {
			/* Rewrite the vertical resolution to be
			   appropriate to the video standard that has been
			   selected. */
			int nvres = hdw->controls[PVR2_CID_VRES].value;
			switch (hdw->controls[PVR2_CID_VIDEOSTANDARD].value) {
			case PVR2_CVAL_VIDEOSTANDARD_NTSC_M:
			case PVR2_CVAL_VIDEOSTANDARD_PAL_M:
				nvres = 480;
				break;
			default:
				nvres = 576;
			}
			if (nvres != hdw->controls[PVR2_CID_VRES].value) {
				hdw->controls[PVR2_CID_VRES].value = nvres;
				hdw->controls[PVR2_CID_VRES].dirty = !0;
			}
		}
		if (!hdw->controls[PVR2_CID_INTERLACE].dirty) {
			if (!hdw->controls[PVR2_CID_INTERLACE].value) {
				hdw->controls[PVR2_CID_INTERLACE].value = 0;
				hdw->controls[PVR2_CID_INTERLACE].dirty = !0;
			}
		}
	}

	if (hdw->controls[PVR2_CID_VIDEOSTANDARD].dirty) {
		hdw->controls[PVR2_CID_FREQUENCY].dirty = !0;
		stale_subsys_mask |= (PVR2_SUBSYS_DIGITIZER_CFG_NORM |
				      PVR2_SUBSYS_TUNER_CFG_STD |
				      PVR2_SUBSYS_AUDIO_CFG_STD);
	}

	if (hdw->controls[PVR2_CID_HRES].dirty ||
	    hdw->controls[PVR2_CID_VRES].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_DIGITIZER_CFG_SIZE;
	}

	if (hdw->controls[PVR2_CID_BRIGHTNESS].dirty ||
	    hdw->controls[PVR2_CID_CONTRAST].dirty ||
	    hdw->controls[PVR2_CID_SATURATION].dirty ||
	    hdw->controls[PVR2_CID_HUE].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_DIGITIZER_CFG_BCSH;
	}

	if (hdw->controls[PVR2_CID_SRATE].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_DIGITIZER_CFG_AUDIO;
	}

	if (hdw->controls[PVR2_CID_VIDEOSTANDARD].dirty ||
	    hdw->controls[PVR2_CID_VRES].dirty ||
	    hdw->controls[PVR2_CID_HRES].dirty ||
	    hdw->controls[PVR2_CID_INTERLACE].dirty ||
	    hdw->controls[PVR2_CID_VBR].dirty ||
	    hdw->controls[PVR2_CID_AVERAGEVIDEOBITRATE].dirty ||
	    hdw->controls[PVR2_CID_PEAKVIDEOBITRATE].dirty ||
	    hdw->controls[PVR2_CID_AUDIOBITRATE].dirty ||
	    hdw->controls[PVR2_CID_SRATE].dirty ||
	    hdw->controls[PVR2_CID_AUDIOLAYER].dirty ||
	    hdw->controls[PVR2_CID_AUDIOCRC].dirty ||
	    hdw->controls[PVR2_CID_AUDIOEMPHASIS].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_ENC_CFG;
	}

	if (hdw->controls[PVR2_CID_AUDIOMODE].dirty ||
	    hdw->controls[PVR2_CID_INPUT].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_AUDIO_CFG_MODE;
	}

	if (hdw->controls[PVR2_CID_VOLUME].dirty ||
	    hdw->controls[PVR2_CID_BALANCE].dirty ||
	    hdw->controls[PVR2_CID_BASS].dirty ||
	    hdw->controls[PVR2_CID_TREBLE].dirty ||
	    hdw->controls[PVR2_CID_MUTE].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_AUDIO_CFG_VBBTM;
	}

	if (hdw->controls[PVR2_CID_INPUT].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_DIGITIZER_CFG_INPUT;
	}

	if (hdw->controls[PVR2_CID_FREQUENCY].dirty) {
		stale_subsys_mask |= PVR2_SUBSYS_TUNER_CFG_FREQ;
	}

	if (stale_subsys_mask & (PVR2_SUBSYS_DIGITIZER_CFG_NORM |
				 PVR2_SUBSYS_TUNER_CFG_STD |
				 PVR2_SUBSYS_AUDIO_CFG_STD |
				 PVR2_SUBSYS_DIGITIZER_CFG_SIZE |
				 PVR2_SUBSYS_DIGITIZER_CFG_AUDIO |
				 PVR2_SUBSYS_ENC_CFG)) {
		stale_subsys_mask |= hdw->subsys_stream_mask;
	}

	for (idx = 0; idx < PVR2_CID_COUNT; idx++) {
		hdw->controls[idx].dirty = 0;
	}

	pvr2_hdw_subsys_bit_chg_no_lock(hdw,stale_subsys_mask,0);
	pvr2_hdw_subsys_bit_chg_no_lock(hdw,~0,saved_subsys_mask);

	return 0;
}


int pvr2_hdw_commit_ctl(struct pvr2_hdw *hdw)
{
	LOCK_TAKE(hdw->big_lock); do {
		pvr2_hdw_commit_ctl_internal(hdw);
	} while (0); LOCK_GIVE(hdw->big_lock);
	return 0;
}


/* Find out how many controls there are.  Legal ids are numbered from 1
   through this value. */
unsigned int pvr2_hdw_get_ctl_count(struct pvr2_hdw *hdw)
{
	return PVR2_CID_COUNT;
}


/* Return bit mask indicating signal status */
unsigned int pvr2_hdw_get_signal_status_internal(struct pvr2_hdw *hdw)
{
	unsigned int msk = 0;
	switch (hdw->controls[PVR2_CID_INPUT].value) {
	case PVR2_CVAL_INPUT_TV:
	case PVR2_CVAL_INPUT_RADIO:
		if (pvr2_decoder_is_tuned(hdw->dcp) > 0) {
			msk |= PVR2_SIGNAL_OK;
			if (pvr2_audio_update_status(hdw) == 0) {
				if (hdw->flag_stereo) {
					msk |= PVR2_SIGNAL_STEREO;
				}
				if (hdw->flag_bilingual) {
					msk |= PVR2_SIGNAL_SAP;
				}
			}
		}
		break;
	default:
		msk |= PVR2_SIGNAL_OK | PVR2_SIGNAL_STEREO;
	}
	return msk;
}


static int pvr2_ctl_get_subsys_mask(struct pvr2_hdw *hdw,int ctl_id)
{
	return hdw->subsys_enabled_mask;
}


static int pvr2_ctl_set_subsys_mask(struct pvr2_hdw *hdw,int ctl_id,int val)
{
	pvr2_hdw_subsys_bit_chg_no_lock(hdw,~0,val);
	return 0;
}


static int pvr2_ctl_get_subsys_stream_mask(struct pvr2_hdw *hdw,int ctl_id)
{
	return hdw->subsys_stream_mask;
}


static int pvr2_ctl_set_subsys_stream_mask(struct pvr2_hdw *hdw,int ctl_id,
                                           int val)
{
	pvr2_hdw_subsys_stream_bit_chg_no_lock(hdw,~0,val);
	return 0;
}


static int pvr2_ctl_get_hsm(struct pvr2_hdw *hdw,int ctl_id)
{
	int result = pvr2_hdw_is_hsm(hdw);
	if (result < 0) return PVR2_CVAL_HSM_FAIL;
	if (result) return PVR2_CVAL_HSM_HIGH;
	return PVR2_CVAL_HSM_FULL;
}


int pvr2_hdw_is_hsm(struct pvr2_hdw *hdw)
{
	int result;
	LOCK_TAKE(hdw->ctl_lock); do {
		hdw->cmd_buffer[0] = 0x0b;
		result = pvr2_send_request(hdw,
					   hdw->cmd_buffer,1,
					   hdw->cmd_buffer,1);
		if (result < 0) break;
		result = (hdw->cmd_buffer[0] != 0);
	} while(0); LOCK_GIVE(hdw->ctl_lock);
	return result;
}


static int pvr2_ctl_get_signal(struct pvr2_hdw *hdw,int ctl_id)
{
	return ((pvr2_hdw_get_signal_status_internal(hdw) & PVR2_SIGNAL_OK) ?
		1 : 0);
}


/* Return bit mask indicating signal status */
unsigned int pvr2_hdw_get_signal_status(struct pvr2_hdw *hdw)
{
	unsigned int msk = 0;
	LOCK_TAKE(hdw->big_lock); do {
		msk = pvr2_hdw_get_signal_status_internal(hdw);
	} while (0); LOCK_GIVE(hdw->big_lock);
	return msk;
}


/* Get handle to video output stream */
struct pvr2_stream *pvr2_hdw_get_video_stream(struct pvr2_hdw *hp)
{
	return hp->vid_stream;
}


void pvr2_hdw_cpufw_set_enabled(struct pvr2_hdw *hdw, int enable_flag)
{
	int ret;
	u16 address;
	unsigned int pipe;
	LOCK_TAKE(hdw->big_lock); do {
		if ((hdw->fw_buffer == 0) == !enable_flag) break;

		if (!enable_flag) {
			pvr2_trace(PVR2_TRACE_FIRMWARE,
				   "Cleaning up after CPU firmware fetch");
			kfree(hdw->fw_buffer);
			hdw->fw_buffer = 0;
			hdw->fw_size = 0;
			/* Now release the CPU.  It will disconnect and
			   reconnect later. */
			pvr2_hdw_cpureset_assert(hdw,0);
			break;
		}

		pvr2_trace(PVR2_TRACE_FIRMWARE,
			   "Preparing to suck out CPU firmware");
		hdw->fw_size = 0x2000;
		hdw->fw_buffer = kmalloc(hdw->fw_size,GFP_KERNEL);
		if (!hdw->fw_buffer) {
			hdw->fw_size = 0;
			break;
		}

		memset(hdw->fw_buffer,0,hdw->fw_size);

		/* We have to hold the CPU during firmware upload. */
		pvr2_hdw_cpureset_assert(hdw,1);

		/* download the firmware from address 0000-1fff in 2048
		   (=0x800) bytes chunk. */

		pvr2_trace(PVR2_TRACE_FIRMWARE,"Grabbing CPU firmware");
		pipe = usb_rcvctrlpipe(hdw->usb_dev, 0);
		for(address = 0; address < hdw->fw_size; address += 0x800) {
			ret = usb_control_msg(hdw->usb_dev,pipe,0xa0,0xc0,
					      address,0,
					      hdw->fw_buffer+address,0x800,HZ);
			if (ret < 0) break;
		}

		pvr2_trace(PVR2_TRACE_FIRMWARE,"Done grabbing CPU firmware");

	} while (0); LOCK_GIVE(hdw->big_lock);
}


/* Return true if we're in a mode for retrieval CPU firmware */
int pvr2_hdw_cpufw_get_enabled(struct pvr2_hdw *hdw)
{
	return hdw->fw_buffer != 0;
}


int pvr2_hdw_cpufw_get(struct pvr2_hdw *hdw,unsigned int offs,
                       char *buf,unsigned int cnt)
{
	int ret = -EINVAL;
	LOCK_TAKE(hdw->big_lock); do {
		if (!buf) break;
		if (!cnt) break;

		if (!hdw->fw_buffer) {
			ret = -EIO;
			break;
		}

		if (offs >= hdw->fw_size) {
			pvr2_trace(PVR2_TRACE_FIRMWARE,
				   "Read firmware data offs=%d EOF",
				   offs);
			ret = 0;
			break;
		}

		if (offs + cnt > hdw->fw_size) cnt = hdw->fw_size - offs;

		memcpy(buf,hdw->fw_buffer+offs,cnt);

		pvr2_trace(PVR2_TRACE_FIRMWARE,
			   "Read firmware data offs=%d cnt=%d",
			   offs,cnt);
		ret = cnt;
	} while (0); LOCK_GIVE(hdw->big_lock);

	return ret;
}


int pvr2_hdw_v4l_get_minor_number(struct pvr2_hdw *hdw)
{
	return hdw->v4l_minor_number;
}


/* Store the v4l minor device number */
void pvr2_hdw_v4l_store_minor_number(struct pvr2_hdw *hdw,int v)
{
	hdw->v4l_minor_number = v;
}


void pvr2_reset_ctl_endpoints(struct pvr2_hdw *hdw)
{
	if (!hdw->usb_dev) return;
	usb_settoggle(hdw->usb_dev, PVR2_CTL_WRITE_ENDPOINT & 0xf,
		      !(PVR2_CTL_WRITE_ENDPOINT & USB_DIR_IN), 0);
	usb_settoggle(hdw->usb_dev, PVR2_CTL_READ_ENDPOINT & 0xf,
		      !(PVR2_CTL_READ_ENDPOINT & USB_DIR_IN), 0);
	usb_clear_halt(hdw->usb_dev,
		       usb_rcvbulkpipe(hdw->usb_dev,
				       PVR2_CTL_READ_ENDPOINT & 0x7f));
	usb_clear_halt(hdw->usb_dev,
		       usb_sndbulkpipe(hdw->usb_dev,
				       PVR2_CTL_WRITE_ENDPOINT & 0x7f));
}


static void pvr2_ctl_write_complete(struct urb *urb, struct pt_regs *regs)
{
	struct pvr2_hdw *hdw = urb->context;
	hdw->ctl_write_pend_flag = 0;
	if (hdw->ctl_read_pend_flag) return;
	complete(&hdw->ctl_done);
}


static void pvr2_ctl_read_complete(struct urb *urb, struct pt_regs *regs)
{
	struct pvr2_hdw *hdw = urb->context;
	hdw->ctl_read_pend_flag = 0;
	if (hdw->ctl_write_pend_flag) return;
	complete(&hdw->ctl_done);
}


static void pvr2_ctl_timeout(unsigned long data)
{
	struct pvr2_hdw *hdw = (struct pvr2_hdw *)data;
	if (hdw->ctl_write_pend_flag || hdw->ctl_read_pend_flag) {
		hdw->ctl_timeout_flag = !0;
		if (hdw->ctl_write_pend_flag && hdw->ctl_write_urb) {
			usb_unlink_urb(hdw->ctl_write_urb);
		}
		if (hdw->ctl_read_pend_flag && hdw->ctl_read_urb) {
			usb_unlink_urb(hdw->ctl_read_urb);
		}
	}
}


int pvr2_send_request(struct pvr2_hdw *hdw,
                      void *write_data,unsigned int write_len,
                      void *read_data,unsigned int read_len)
{
	unsigned int idx;
	int status;
	struct timer_list timer;
	if (!hdw->ctl_lock_held) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Attempted to execute control transfer"
			   " without lock!!");
		status = -EINVAL;
		goto done;
	}
	if (!hdw->flag_ok) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Attempted to execute control transfer"
			   " when device not ok");
		return -EIO;
	}
	if (!(hdw->ctl_read_urb && hdw->ctl_write_urb)) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Attempted to execute control transfer"
			   " when USB is disconnected");
		return -EIO;
	}

	/* Ensure that we have sane parameters */
	if (!write_data) write_len = 0;
	if (!read_data) read_len = 0;
	if (write_len > PVR2_CTL_BUFFSIZE) {
		pvr2_trace(
			PVR2_TRACE_ERROR_LEGS,
			"Attempted to execute %d byte"
			" control-write transfer (limit=%d)",
			write_len,PVR2_CTL_BUFFSIZE);
		return -EINVAL;
	}
	if (read_len > PVR2_CTL_BUFFSIZE) {
		pvr2_trace(
			PVR2_TRACE_ERROR_LEGS,
			"Attempted to execute %d byte"
			" control-read transfer (limit=%d)",
			write_len,PVR2_CTL_BUFFSIZE);
		return -EINVAL;
	}
	if ((!write_len) && (!read_len)) {
		pvr2_trace(
			PVR2_TRACE_ERROR_LEGS,
			"Attempted to execute null control transfer?");
		return -EINVAL;
	}

	hdw->cmd_debug_state = 1;
	if (write_len) {
		hdw->cmd_debug_code = ((unsigned char *)write_data)[0];
	} else {
		hdw->cmd_debug_code = 0;
	}
	hdw->cmd_debug_write_len = write_len;
	hdw->cmd_debug_read_len = read_len;

	/* Initialize common stuff */
	init_completion(&hdw->ctl_done);
	hdw->ctl_timeout_flag = 0;
	hdw->ctl_write_pend_flag = 0;
	hdw->ctl_read_pend_flag = 0;
	init_timer(&timer);
	timer.expires = jiffies + HZ*4;
	timer.data = (unsigned long)hdw;
	timer.function = pvr2_ctl_timeout;

	if (write_len) {
		hdw->cmd_debug_state = 2;
		/* Transfer write data to internal buffer */
		for (idx = 0; idx < write_len; idx++) {
			hdw->ctl_write_buffer[idx] =
				((unsigned char *)write_data)[idx];
		}
		/* Initiate a write request */
		usb_fill_bulk_urb(hdw->ctl_write_urb,
				  hdw->usb_dev,
				  usb_sndbulkpipe(hdw->usb_dev,
						  PVR2_CTL_WRITE_ENDPOINT),
				  hdw->ctl_write_buffer,
				  write_len,
				  pvr2_ctl_write_complete,
				  hdw);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
		hdw->ctl_write_urb->transfer_flags |= URB_ASYNC_UNLINK;
#endif
		hdw->ctl_write_urb->actual_length = 0;
		hdw->ctl_write_pend_flag = !0;
		status = usb_submit_urb(hdw->ctl_write_urb,GFP_KERNEL);
		if (status < 0) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "Failed to submit write-control"
				   " URB status=%d",status);
			hdw->ctl_write_pend_flag = 0;
		}
	}

	if (read_len) {
		hdw->cmd_debug_state = 3;
		memset(hdw->ctl_read_buffer,0x43,read_len);
		/* Initiate a read request */
		usb_fill_bulk_urb(hdw->ctl_read_urb,
				  hdw->usb_dev,
				  usb_rcvbulkpipe(hdw->usb_dev,
						  PVR2_CTL_READ_ENDPOINT),
				  hdw->ctl_read_buffer,
				  read_len,
				  pvr2_ctl_read_complete,
				  hdw);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
		hdw->ctl_read_urb->transfer_flags |= URB_ASYNC_UNLINK;
#endif
		hdw->ctl_read_urb->actual_length = 0;
		hdw->ctl_read_pend_flag = !0;
		status = usb_submit_urb(hdw->ctl_read_urb,GFP_KERNEL);
		if (status < 0) {
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "Failed to submit read-control"
				   " URB status=%d",status);
			hdw->ctl_read_pend_flag = 0;
		}
	}

	/* Start timer */
	add_timer(&timer);

	/* Now wait for all I/O to complete */
	hdw->cmd_debug_state = 4;
	while (hdw->ctl_write_pend_flag || hdw->ctl_read_pend_flag) {
		wait_for_completion(&hdw->ctl_done);
	}
	hdw->cmd_debug_state = 5;

	/* Stop timer */
	del_timer_sync(&timer);

	hdw->cmd_debug_state = 6;
	status = 0;

	if (hdw->ctl_timeout_flag) {
		status = -ETIMEDOUT;
		goto done;
	}

	if (write_len) {
		/* Validate results of write request */
		if ((hdw->ctl_write_urb->status != 0) &&
		    (hdw->ctl_write_urb->status != -ENOENT) &&
		    (hdw->ctl_write_urb->status != -ESHUTDOWN) &&
		    (hdw->ctl_write_urb->status != -ECONNRESET)) {
			/* USB subsystem is reporting some kind of failure
			   on the write */
			status = hdw->ctl_write_urb->status;
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "control-write URB failure, status=%d",
				   status);
			goto done;
		}
		if (hdw->ctl_write_urb->actual_length < write_len) {
			/* Failed to write enough data */
			status = -EIO;
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "control-write URB short,"
				   " expected=%d got=%d",
				   write_len,
				   hdw->ctl_write_urb->actual_length);
			goto done;
		}
	}
	if (read_len) {
		/* Validate results of read request */
		if ((hdw->ctl_read_urb->status != 0) &&
		    (hdw->ctl_read_urb->status != -ENOENT) &&
		    (hdw->ctl_read_urb->status != -ESHUTDOWN) &&
		    (hdw->ctl_read_urb->status != -ECONNRESET)) {
			/* USB subsystem is reporting some kind of failure
			   on the read */
			status = hdw->ctl_read_urb->status;
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "control-read URB failure, status=%d",
				   status);
			goto done;
		}
		if (hdw->ctl_read_urb->actual_length < read_len) {
			/* Failed to read enough data */
			status = -EIO;
			pvr2_trace(PVR2_TRACE_ERROR_LEGS,
				   "control-read URB short,"
				   " expected=%d got=%d",
				   read_len,hdw->ctl_read_urb->actual_length);
			goto done;
		}
		/* Transfer retrieved data out from internal buffer */
		for (idx = 0; idx < read_len; idx++) {
			((unsigned char *)read_data)[idx] =
				hdw->ctl_read_buffer[idx];
		}
	}

 done:
	hdw->cmd_debug_state = 0;
	if (status < 0) {
		pvr2_hdw_render_useless_unlocked(hdw);
	}
	return status;
}


int pvr2_write_register(struct pvr2_hdw *hdw, u16 reg, u32 data)
{
	int ret;

	LOCK_TAKE(hdw->ctl_lock);

	hdw->cmd_buffer[0] = 0x04;  /* write register prefix */
	PVR2_DECOMPOSE_LE(hdw->cmd_buffer,1,data);
	hdw->cmd_buffer[5] = 0;
	hdw->cmd_buffer[6] = (reg >> 8) & 0xff;
	hdw->cmd_buffer[7] = reg & 0xff;


	ret = pvr2_send_request(hdw, hdw->cmd_buffer, 8, hdw->cmd_buffer, 0);

	LOCK_GIVE(hdw->ctl_lock);

	return ret;
}


int pvr2_read_register(struct pvr2_hdw *hdw, u16 reg, u32 *data)
{
	int ret = 0;

	LOCK_TAKE(hdw->ctl_lock);

	hdw->cmd_buffer[0] = 0x05;  /* read register prefix */
	hdw->cmd_buffer[1] = 0;
	hdw->cmd_buffer[2] = 0;
	hdw->cmd_buffer[3] = 0;
	hdw->cmd_buffer[4] = 0;
	hdw->cmd_buffer[5] = 0;
	hdw->cmd_buffer[6] = (reg >> 8) & 0xff;
	hdw->cmd_buffer[7] = reg & 0xff;

	ret |= pvr2_send_request(hdw, hdw->cmd_buffer, 8, hdw->cmd_buffer, 4);
	*data = PVR2_COMPOSE_LE(hdw->cmd_buffer,0);

	LOCK_GIVE(hdw->ctl_lock);

	return ret;
}


int pvr2_write_u16(struct pvr2_hdw *hdw, u16 data, int res)
{
	int ret;

	LOCK_TAKE(hdw->ctl_lock);

	hdw->cmd_buffer[0] = (data >> 8) & 0xff;
	hdw->cmd_buffer[1] = data & 0xff;

	ret = pvr2_send_request(hdw, hdw->cmd_buffer, 2, hdw->cmd_buffer, res);

	LOCK_GIVE(hdw->ctl_lock);

	return ret;
}


int pvr2_write_u8(struct pvr2_hdw *hdw, u8 data, int res)
{
	int ret;

	LOCK_TAKE(hdw->ctl_lock);

	hdw->cmd_buffer[0] = data;

	ret = pvr2_send_request(hdw, hdw->cmd_buffer, 1, hdw->cmd_buffer, res);

	LOCK_GIVE(hdw->ctl_lock);

	return ret;
}


void pvr2_hdw_render_useless_unlocked(struct pvr2_hdw *hdw)
{
	if (!hdw->flag_ok) return;
	pvr2_trace(PVR2_TRACE_INIT,"render_useless");
	hdw->flag_ok = 0;
	if (hdw->vid_stream) {
		pvr2_stream_setup(hdw->vid_stream,0,0);
	}
	hdw->flag_streaming_enabled = 0;
	hdw->subsys_enabled_mask = 0;
}


void pvr2_hdw_render_useless(struct pvr2_hdw *hdw)
{
	LOCK_TAKE(hdw->ctl_lock);
	pvr2_hdw_render_useless_unlocked(hdw);
	LOCK_GIVE(hdw->ctl_lock);
}


void pvr2_hdw_device_reset(struct pvr2_hdw *hdw)
{
	int ret;
	pvr2_trace(PVR2_TRACE_INIT,"Performing a device reset...");
	ret = usb_lock_device_for_reset(hdw->usb_dev,0);
	if (ret == 1) {
		ret = usb_reset_device(hdw->usb_dev);
		usb_unlock_device(hdw->usb_dev);
	} else {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "Failed to lock USB device ret=%d",ret);
	}
}


void pvr2_hdw_cpureset_assert(struct pvr2_hdw *hdw,int val)
{
	char da[1];
	unsigned int pipe;
	int ret;

	if (!hdw->usb_dev) return;

	pvr2_trace(PVR2_TRACE_INIT,"cpureset_assert(%d)",val);

	da[0] = val ? 0x01 : 0x00;

	/* Write the CPUCS register on the 8051.  The lsb of the register
	   is the reset bit; a 1 asserts reset while a 0 clears it. */
	pipe = usb_sndctrlpipe(hdw->usb_dev, 0);
	ret = usb_control_msg(hdw->usb_dev,pipe,0xa0,0x40,0xe600,0,da,1,HZ);
	if (ret < 0) {
		pvr2_trace(PVR2_TRACE_ERROR_LEGS,
			   "cpureset_assert(%d) error=%d",val,ret);
		pvr2_hdw_render_useless(hdw);
	}
}


int pvr2_hdw_cmd_deep_reset(struct pvr2_hdw *hdw)
{
	int status;
	LOCK_TAKE(hdw->ctl_lock); do {
		pvr2_trace(PVR2_TRACE_INIT,"Requesting uproc hard reset");
		hdw->flag_ok = !0;
		hdw->cmd_buffer[0] = 0xdd;
		status = pvr2_send_request(hdw,hdw->cmd_buffer,1,0,0);
	} while (0); LOCK_GIVE(hdw->ctl_lock);
	return status;
}


int pvr2_hdw_cmd_soft_reset(struct pvr2_hdw *hdw)
{
	int status;
	LOCK_TAKE(hdw->ctl_lock); do {
		pvr2_trace(PVR2_TRACE_INIT,"Requesting uproc soft reset");
		hdw->cmd_buffer[0] = 0xde;
		status = pvr2_send_request(hdw,hdw->cmd_buffer,1,0,0);
	} while (0); LOCK_GIVE(hdw->ctl_lock);
	return status;
}


int pvr2_hdw_cmd_usbstream(struct pvr2_hdw *hdw,int runFl)
{
	int status;
	LOCK_TAKE(hdw->ctl_lock); do {
		hdw->cmd_buffer[0] = (runFl ? 0x36 : 0x37);
		status = pvr2_send_request(hdw,hdw->cmd_buffer,1,0,0);
	} while (0); LOCK_GIVE(hdw->ctl_lock);
	if (!status) {
		hdw->subsys_enabled_mask =
			((hdw->subsys_enabled_mask &
			  ~PVR2_SUBSYS_USBSTREAM_RUN) |
			 (runFl ? PVR2_SUBSYS_USBSTREAM_RUN : 0));
	}
	return status;
}


void pvr2_hdw_get_debug_info(const struct pvr2_hdw *hdw,
                             struct pvr2_hdw_debug_info *ptr)
{
	ptr->big_lock_held = hdw->big_lock_held;
	ptr->ctl_lock_held = hdw->ctl_lock_held;
	ptr->flag_ok = hdw->flag_ok;
	ptr->flag_disconnected = hdw->flag_disconnected;
	ptr->flag_init_ok = hdw->flag_init_ok;
	ptr->flag_streaming_enabled = hdw->flag_streaming_enabled;
	ptr->subsys_flags = hdw->subsys_enabled_mask;
	ptr->cmd_debug_state = hdw->cmd_debug_state;
	ptr->cmd_code = hdw->cmd_debug_code;
	ptr->cmd_debug_write_len = hdw->cmd_debug_write_len;
	ptr->cmd_debug_read_len = hdw->cmd_debug_read_len;
	ptr->cmd_debug_timeout = hdw->ctl_timeout_flag;
	ptr->cmd_debug_write_pend = hdw->ctl_write_pend_flag;
	ptr->cmd_debug_read_pend = hdw->ctl_read_pend_flag;
	ptr->cmd_debug_rstatus = hdw->ctl_read_urb->status;
	ptr->cmd_debug_wstatus = hdw->ctl_read_urb->status;
}


int pvr2_hdw_gpio_get_dir(struct pvr2_hdw *hdw,u32 *dp)
{
	return pvr2_read_register(hdw,PVR2_GPIO_DIR,dp);
}


int pvr2_hdw_gpio_get_out(struct pvr2_hdw *hdw,u32 *dp)
{
	return pvr2_read_register(hdw,PVR2_GPIO_OUT,dp);
}


int pvr2_hdw_gpio_get_in(struct pvr2_hdw *hdw,u32 *dp)
{
	return pvr2_read_register(hdw,PVR2_GPIO_IN,dp);
}


int pvr2_hdw_gpio_chg_dir(struct pvr2_hdw *hdw,u32 msk,u32 val)
{
	u32 cval,nval;
	int ret;
	if (~msk) {
		ret = pvr2_read_register(hdw,PVR2_GPIO_DIR,&cval);
		if (ret) return ret;
		nval = (cval & ~msk) | (val & msk);
		pvr2_trace(PVR2_TRACE_GPIO,
			   "GPIO direction changing 0x%x:0x%x"
			   " from 0x%x to 0x%x",
			   msk,val,cval,nval);
	} else {
		nval = val;
		pvr2_trace(PVR2_TRACE_GPIO,
			   "GPIO direction changing to 0x%x",nval);
	}
	return pvr2_write_register(hdw,PVR2_GPIO_DIR,nval);
}


int pvr2_hdw_gpio_chg_out(struct pvr2_hdw *hdw,u32 msk,u32 val)
{
	u32 cval,nval;
	int ret;
	if (~msk) {
		ret = pvr2_read_register(hdw,PVR2_GPIO_OUT,&cval);
		if (ret) return ret;
		nval = (cval & ~msk) | (val & msk);
		pvr2_trace(PVR2_TRACE_GPIO,
			   "GPIO output changing 0x%x:0x%x from 0x%x to 0x%x",
			   msk,val,cval,nval);
	} else {
		nval = val;
		pvr2_trace(PVR2_TRACE_GPIO,
			   "GPIO output changing to 0x%x",nval);
	}
	return pvr2_write_register(hdw,PVR2_GPIO_OUT,nval);
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
