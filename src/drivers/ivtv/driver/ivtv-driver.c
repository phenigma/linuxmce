/*
    ivtv driver initialization and card probing
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

    Card autodetect:
    Copyright (C) 2004  Hans Verkuil <hverkuil@xs4all.nl>

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

/* Main Driver file for the ivtv project:
 * Driver for the iTVC15 chip.
 * Author: Kevin Thayer (nufan_wfk at yahoo.com)
 * License: GPL
 * http://www.sourceforge.net/projects/ivtv/
 * 
 * -----
 * MPG600/MPG160 support by  T.Adachi <tadachi@tadachi-net.com>
 *                      and Takeru KOMORIYA<komoriya@paken.org>
 *
 * AVerMedia M179 GPIO info by Chris Pinkham <cpinkham@bc2va.org>
 *                using information provided by Jiun-Kuei Jung @ AVerMedia.
 */

#include "ivtv-driver.h"
#include "ivtv-fileops.h"
#include "ivtv-i2c.h"
#include "ivtv-firmware.h"
#include "ivtv-queue.h"
#include "ivtv-irq.h"
#include "ivtv-mailbox.h"
#include "ivtv-reset.h"
#include "ivtv-streams.h"
#include "ivtv-ioctl.h"
#include "ivtv-kthreads.h"
#include "ivtv-cards.h"
#include "ivtv-vbi.h"
#include "ivtv-audio.h"

#ifdef LINUX26
#include <linux/vermagic.h>
#else
#include <linux/version.h>
#include <linux/kmod.h>
#endif

/* Version info*/
#define IVTV_DEFINE_VERSION_INTERNAL(name, major, minor, patchlevel, comment) \
	uint32_t IVTV_VERSION_NUMBER_INTERNAL(name) = \
		((major << 16) | (minor << 8) | (patchlevel)); \
	const char * const IVTV_VERSION_STRING_INTERNAL(name) = \
		#major"."#minor"."#patchlevel;\
	const char * const IVTV_VERSION_COMMENT_INTERNAL(name) = \
		comment;
 
#define IVTV_DEFINE_VERSION(name, major, minor, patchlevel, comment) \
		IVTV_DEFINE_VERSION_INTERNAL(name, major, minor, patchlevel, comment)

IVTV_DEFINE_VERSION(IVTV_VERSION_INFO_NAME,
		IVTV_DRIVER_VERSION_MAJOR,
		IVTV_DRIVER_VERSION_MINOR,
		IVTV_DRIVER_VERSION_PATCHLEVEL,
		IVTV_DRIVER_VERSION_COMMENT);

/* mini header */

/* var to keep track of the number of array elements in use */
int ivtv_cards_active = 0;

/* If you have already X v4l cards, then set this to X. This way
   the device numbers stay matched. Example: you have a WinTV card
   without radio and a PVR-350 with. Normally this would give a
   video1 device together with a radio0 device for the PVR. By
   setting this to 1 you ensure that radio0 is now also radio1. */
int ivtv_first_minor = 0;

/* Master variable for all ivtv info */
struct ivtv ivtv_cards[IVTV_MAX_CARDS];

/* Protects ivtv_cards_active */
spinlock_t ivtv_cards_lock = SPIN_LOCK_UNLOCKED;

/* add your revision and whatnot here */
static struct pci_device_id ivtv_pci_tbl[] __devinitdata = {
		{PCI_VENDOR_ID_ICOMP, PCI_DEVICE_ID_IVTV15,
		 PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
		{PCI_VENDOR_ID_ICOMP, PCI_DEVICE_ID_IVTV16,
		 PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
		{0,}
};


/* Parameter declarations */
static int cardtype[IVTV_MAX_CARDS];
static int tuner[IVTV_MAX_CARDS] = { -1, -1, -1, -1, -1, -1, -1, -1 };
static int radio[IVTV_MAX_CARDS] = { -1, -1, -1, -1, -1, -1, -1, -1 };
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
static int cardtype_c = 1;
static int tuner_c = 1;
static int radio_c = 1;
#endif
#ifdef YUV_FIXUP
static int yuv_fixup = 1;
#endif
#ifdef MODULE
static unsigned int autoload = 1;
#else
static unsigned int autoload = 0;
#endif

static int ivtv_std = 0;

int ivtv_debug = IVTV_DEBUG_ERR;

/* tuner.h tuner type for ivtv card */
int tda9887 = -1; /* SECAM */

int errno;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#ifdef YUV_FIXUP
module_param(yuv_fixup, int, 0644);
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 10)
module_param_array(tuner, int, &tuner_c, 0644);
module_param_array(radio, bool, &radio_c, 0644);
module_param_array(cardtype, int, &cardtype_c, 0644);
#else
module_param_array(tuner, int, tuner_c, 0644);
module_param_array(radio, bool, radio_c, 0644);
module_param_array(cardtype, int, cardtype_c, 0644);
#endif
module_param(tda9887, int, 0644);
module_param(autoload, int, 0644);
module_param(ivtv_debug, int, 0644);
module_param(ivtv_std, int, 0644);
module_param(ivtv_first_minor, int, 0644);
#else
#ifdef YUV_FIXUP
MODULE_PARM(yuv_fixup, "i");
#endif
MODULE_PARM(tuner, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(tda9887, "i");
MODULE_PARM(radio, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(autoload,"i");
MODULE_PARM(cardtype, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(ivtv_debug, "i");
MODULE_PARM(ivtv_std, "i");
MODULE_PARM(ivtv_first_minor, "i");
#endif

#ifdef YUV_FIXUP
MODULE_PARM_DESC(yuv_fixup, "Toggles conversion of Hauppauge Macroblock NV12 to NV12");
#endif

MODULE_PARM_DESC(tuner, "Tuner type selection, see tuner.h for values");
MODULE_PARM_DESC(tda9887, "Enable or disable tda9887 module, 0 = normal, 1+ = pinnacle_id");
MODULE_PARM_DESC(radio, "Enable or disable the radio. Use only if autodetection fails. 0 = disable, 1 = enable");
MODULE_PARM_DESC(autoload, "automatically load i2c modules like tuner.o, default is 1 (yes)");
MODULE_PARM_DESC(cardtype,
		 "Only use this option if your card is not detected properly.\n"
		 "Specify card type:\n"
		 "\t\t1 = WinTV PVR 250\n"
		 "\t\t2 = WinTV PVR 350\n"
	  	 "\t\t3 = AVerMedia M179\n"
		 "\t\t4 = YUAN MPG600/Kuroutoshikou iTVC16-STVLP\n"
	  	 "\t\t5 = YUAN MPG160/Kuroutoshikou iTVC15-STVLP\n"
		 "\t\tDefault: Autodetect" );
MODULE_PARM_DESC(ivtv_debug, "Debug level (bitmask). Default: errors only "
		             "(ivtv_debug = 255 gives full debugging)");
MODULE_PARM_DESC(ivtv_std, "Specify video standard: 1 = NTSC, 2 = PAL, 3 = SECAM, Default: Autodetect");
MODULE_PARM_DESC(ivtv_first_minor, "Set minor assigned to first card");

MODULE_AUTHOR("Kevin Thayer");
MODULE_DESCRIPTION("iTVC15_16 driver");
MODULE_SUPPORTED_DEVICE("iTVC15_16 mpg2 encoder (aka WinTV PVR 250/350)");
MODULE_LICENSE("GPL");
#if defined(LINUX26)
MODULE_VERSION(IVTV_VERSION);
#endif

/* Generic utility functions */
int ivtv_sleep_timeout(int timeout, int intr)
{
	int sleep = timeout;
	int ret = 0;

	do {
		if (intr)
			set_current_state(TASK_INTERRUPTIBLE);
		else
			set_current_state(TASK_UNINTERRUPTIBLE);

		sleep = schedule_timeout(sleep);

		if (intr)
			if ((ret = signal_pending(current)))	
				break;
	} while (sleep);
	return ret;
}


void ivtv_clear_irq_mask(struct ivtv *itv, unsigned long mask)
{
	itv->irqmask &= ~mask;
	writel(itv->irqmask,(itv->reg_mem + IVTV_REG_IRQMASK));

	/* pci posting */
	readl(itv->reg_mem + IVTV_REG_IRQMASK);
}

void ivtv_set_irq_mask(struct ivtv *itv, unsigned long mask)
{
	itv->irqmask |= mask;
	writel(itv->irqmask,(itv->reg_mem + IVTV_REG_IRQMASK));

	/* pci posting */
	readl(itv->reg_mem + IVTV_REG_IRQMASK);
}

void enc_schedule_work(struct ivtv *itv)
{
	wake_up(&itv->enc_thread_w);
}

void dec_schedule_work(struct ivtv *itv)
{
	wake_up(&itv->dec_thread_w);
}

void fw_schedule_work(struct ivtv *itv)
{
#ifdef LINUX26
        queue_work(itv->fw_work_queues, &itv->fw_work_queue);
#else
        ivtv_schedule_work(&itv->fw_work_queue);
#endif
}

#ifdef LINUX26
int ivtv_schedule_work(struct work_struct *work)
{
	return schedule_work(work);
}
#else
int ivtv_schedule_work(struct tq_struct *work)
{
	schedule_task(work);
	return 1;
}
#endif

void ivtv_flush_scheduled_work(struct ivtv *itv)
{
#ifdef LINUX26
	flush_scheduled_work();
	flush_workqueue(itv->vbi_work_queues);
	flush_workqueue(itv->fw_work_queues);
#else
	flush_scheduled_tasks();
#endif
}

/* Release ioremapped memory */
static void ivtv_iounmap(struct ivtv *itv) 
{
	if (itv == NULL)
		return;

	/* Release registers memory */
	if (itv->reg_mem != NULL) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "releasing reg_mem\n");
		iounmap(itv->reg_mem);
		itv->reg_mem = NULL;
	}
	/* Release io memory */
	if (itv->dec_mem != NULL) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "releasing dec_mem\n");
		iounmap(itv->dec_mem);
		itv->dec_mem = NULL;
	}

	/* Release io memory */
	if (itv->enc_mem != NULL) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "releasing enc_mem\n");
		iounmap(itv->enc_mem);
		itv->enc_mem = NULL;
	}
}

/* Hauppauge card? get values from tveeprom */
static int ivtv_read_eeprom(struct ivtv *itv)
{
	u32 eepromdata[5];
	int ret;

	ret = ivtv_hauppauge(itv, 0, eepromdata);

	if (ret) {
		IVTV_KERN_ERR("Error %d reading Hauppauge eeprom.\n", ret);
		IVTV_KERN_ERR("Possible causes: the tveeprom module was not loaded, or\n");
		IVTV_KERN_ERR("the eeprom kernel module was loaded before the tveeprom module.\n");
		return ret;
	}

	IVTV_DEBUG(IVTV_DEBUG_ERR, "Tuner Type %d, "
			"Tuner formats 0x%08x, "
			"Radio: %s, "
			"Model 0x%08x, "
			"Revision 0x%08x\n",
			eepromdata[0], eepromdata[1],
			eepromdata[2] ? "yes" : "no", 
			eepromdata[3], eepromdata[4]);
	if (itv->std == 0) {
		/* autodetect tuner standard */
		if (eepromdata[1] & V4L2_STD_PAL) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "PAL tuner detected\n");
			itv->std |= V4L2_STD_PAL;
			itv->codec.framespergop = 12;	
		}
		if (eepromdata[1] & V4L2_STD_NTSC) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "NTSC tuner detected\n");
			itv->std |= V4L2_STD_NTSC;
			itv->codec.framespergop = 15;	
		}
		if (eepromdata[1] & V4L2_STD_SECAM) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, "SECAM tuner detected\n");
			itv->std |= V4L2_STD_SECAM;
			itv->codec.framespergop = 12;	
		}
	}
	if (itv->options.tuner == -1)
		itv->options.tuner = eepromdata[0];
	IVTV_DEBUG(IVTV_DEBUG_ERR, "Radio %sdetected\n", eepromdata[2] ? "" : "not ");
	if (itv->options.radio == -1)
		itv->options.radio = (eepromdata[2] != 0);
	return ret;
}

static void ivtv_process_options(struct ivtv *itv)
{
	int i, j;

	/* Buffers are now dynamically allocated, could specify limit though */
#ifdef YUV_FIXUP
	itv->options.yuv_fixup = yuv_fixup;
#endif
	itv->options.dec_yuv_buffers = IVTV_DEFAULT_DEC_YUV_BUFFERS;
	itv->options.dec_mpg_buffers = IVTV_DEFAULT_DEC_MPG_BUFFERS;
	itv->options.yuv_buffers = IVTV_DEFAULT_YUV_BUFFERS;
	itv->options.mpg_buffers = IVTV_DEFAULT_MPG_BUFFERS;
	itv->options.vbi_buffers = IVTV_DEFAULT_VBI_BUFFERS;
	itv->options.pcm_buffers = IVTV_DEFAULT_PCM_BUFFERS;
	itv->options.cardtype = cardtype[itv->num];
 	itv->options.tuner = tuner[itv->num];
 	itv->options.radio = radio[itv->num];

	if ((itv->card = ivtv_get_card(itv->options.cardtype - 1))) {
		IVTV_KERN_INFO("User specified %s card\n", itv->card->name);
	} else if (itv->options.cardtype != 0) {
		itv->card = ivtv_get_card(IVTV_CARD_PVR_250);
		IVTV_KERN_ERR("Unknown user specified type, defaulting to %s card\n",
			      itv->card->name);
	} else {
		for (i = 0; (itv->card = ivtv_get_card(i)); i++) {
			for (j = 0; itv->card->pci_list[j].device; j++) {
               			if (itv->dev->device != itv->card->pci_list[j].device)
					continue;
               			if (itv->dev->subsystem_vendor != 
				    	itv->card->pci_list[j].subsystem_vendor)
					continue;
               			if (itv->dev->subsystem_device != 
					itv->card->pci_list[j].subsystem_device)
					continue;
               			IVTV_KERN_INFO("Autodetected %s card\n", itv->card->name);
				goto done;
			}
       		}

done:
       		if (itv->card == NULL) {
			itv->card = ivtv_get_card(IVTV_CARD_PVR_250);
               		IVTV_KERN_ERR("Unknown card: vendor/device: %04x/%04x, subsystem vendor/device: %04x/%04x\n",
			       	itv->dev->vendor, itv->dev->device,
				itv->dev->subsystem_vendor, itv->dev->subsystem_device);
               		IVTV_KERN_ERR("Defaulting to %s card\n", itv->card->name);
			IVTV_KERN_ERR("Please mail the vendor/device and subsystem vendor/device IDs and what kind of\n");
			IVTV_KERN_ERR("card you have to the ivtv-devel mailinglist: http://sourceforge.net/projects/ivtv\n");
			IVTV_KERN_ERR("Prefix your subject line with [UNKNOWN CARD].\n");
		}
	}
	itv->has_itvc15 = (itv->dev->device == PCI_DEVICE_ID_IVTV15);
	IVTV_KERN_INFO("Found an %s based chip\n", itv->has_itvc15 ? "iTVC15" : "iTVC16");
	itv->v4l2_cap = itv->card->v4l2_capabilities;
	
	/* set the standard */
	switch (ivtv_std) {
		case 1:
			itv->std = V4L2_STD_NTSC;
			itv->codec.framespergop = 15;
			break;
		case 2:
			itv->std = V4L2_STD_PAL;
			itv->codec.framespergop = 12;
			break;
		case 3:
			itv->std = V4L2_STD_SECAM;
			itv->codec.framespergop = 12;
			break;
		default:
			break;
	}
}

/* Precondition: the ivtv structure has been memset to 0. Only
   the dev and num fields have been filled in. */
static int __devinit ivtv_init_struct(struct ivtv *itv)
{
	const struct v4l2_input *input;
	int i;

	itv->base_addr = pci_resource_start(itv->dev, 0);
	itv->enc_mem = NULL;
	itv->dec_mem = NULL;
	itv->reg_mem = NULL;

	/* Set FrameBuffer-ID to invalid */
	itv->fb_id = -1;

	init_MUTEX(&itv->enc_msem);
	init_MUTEX(&itv->dec_msem);
	init_MUTEX(&itv->streams_lock);
	init_MUTEX(&itv->i2c_lock);
	init_MUTEX(&itv->DMA_lock);

	itv->DMA_slock = SPIN_LOCK_UNLOCKED;

#ifdef LINUX26
	itv->vbi_work_queues = create_workqueue("ivtv_vbi");
	if (itv->vbi_work_queues == NULL) {
		IVTV_KERN_ERR("Could not create VBI workqueue\n");
		return -1;
	}
	itv->fw_work_queues = create_workqueue("ivtv_fw");
	if (itv->fw_work_queues == NULL) {
		IVTV_KERN_ERR("Could not create FW workqueue\n");
		return -1;
	}
	INIT_WORK(&itv->vbi_work_queue, vbi_work_handler, itv);
	INIT_WORK(&itv->fw_work_queue, fw_work_handler, itv);
#else
	INIT_TQUEUE(&itv->vbi_work_queue, vbi_work_handler, itv);
	INIT_TQUEUE(&itv->fw_work_queue, fw_work_handler, itv);
#endif

	/* start counting open_id at 1 */
	itv->open_id = 1;
        
	/* Initial settings */
        itv->codec.bitrate_mode	= 0;
        itv->codec.bitrate	= 8000000;
        itv->codec.bitrate_peak	= 9600000;
        itv->codec.stream_type	= IVTV_STREAM_PS;
        itv->codec.bframes	= 3;
        itv->codec.gop_closure	= 1;
        itv->codec.dnr_mode	= 0;
        itv->codec.dnr_type	= 0;
        itv->codec.dnr_spatial 	= 0;
        itv->codec.dnr_temporal	= 0;
	itv->codec.aspect	= 2;
	itv->codec.pulldown	= 0;
	/* Freq 48 kHz, Layer II, 384 kbit/s */
	itv->codec.audio_bitmask = 1 | (2 << 2) | (14 << 4);
	
	init_waitqueue_head(&itv->cap_w);
	init_waitqueue_head(&itv->vsync_w);
	
	/* DMA Settings */
	itv->dma_cfg.dec_max_xfer	= IVTV_DMA_MAX_XFER;
	itv->dma_cfg.dec_min_xfer	= IVTV_DMA_MIN_XFER;
	itv->dma_cfg.osd_sg_size	= IVTV_DMA_SG_OSD_SIZE;
	itv->dma_cfg.osd_sg_ent		= IVTV_DMA_SG_OSD_ENT;
	itv->dma_cfg.enc_buf_size	= IVTV_DMA_ENC_BUF_SIZE;
	itv->dma_cfg.enc_yuv_buf_size	= IVTV_DMA_ENC_YUV_BUF_SIZE;
	itv->dma_cfg.enc_pcm_buf_size	= IVTV_DMA_ENC_PCM_BUF_SIZE;
	itv->dma_cfg.dec_buf_size	= IVTV_DMA_DEC_BUF_SIZE;
	itv->dma_cfg.dec_yuv_buf_size	= IVTV_DMA_DEC_YUV_BUF_SIZE;
	itv->dma_cfg.vbi_buf_size	= IVTV_DMA_VBI_BUF_SIZE;
	itv->dma_cfg.dec_vbi_buf_size	= IVTV_DMA_DEC_VBI_BUF_SIZE;
	itv->dma_cfg.max_yuv_buf	= IVTV_MAX_YUV_BUFFERS;
	itv->dma_cfg.max_pcm_buf	= IVTV_MAX_PCM_BUFFERS;
	itv->dma_cfg.max_mpg_buf	= IVTV_MAX_MPG_BUFFERS;
	itv->dma_cfg.max_dec_yuv_buf	= IVTV_MAX_DEC_YUV_BUFFERS;
	itv->dma_cfg.max_dec_mpg_buf	= IVTV_MAX_DEC_MPG_BUFFERS;
	itv->dma_cfg.max_vbi_buf	= IVTV_MAX_VBI_BUFFERS;
	itv->dma_cfg.fw_dec_dma_xfer	= FW_DEC_DMA_XFER;
	itv->dma_cfg.fw_enc_dma_xfer	= FW_ENC_DMA_XFER;
	itv->dma_cfg.fw_enc_dma_type	= FW_ENC_DMA_TYPE;
	itv->dma_cfg.enc_pio		= 0;
	itv->dma_cfg.dec_pio		= IVTV_DEC_PIO;
	itv->dma_cfg.osd_pio		= 0;

	/* Decoder DMA State */
	itv->dec_dma_stat.last_addr	= 0;
	itv->dec_dma_stat.bytes_filled	= 0;
	itv->dec_dma_stat.bytes_needed	= 0;
	itv->dec_dma_stat.last_xfer	= 0;
	itv->dec_dma_stat.total_xfer	= 0;
	itv->dec_dma_stat.sg_bytes	= 0;
	atomic_set(&itv->dec_dma_stat.type, -1);

	/* Is it a VIA or have problems with large DMA xfers */
	itv->via_fix 			= 0;

	/* Encoder Options */
	itv->end_gop			= 0;
	itv->idx_sdf_offset		= 0;
	itv->idx_sdf_num		= 400;
	itv->idx_sdf_mask		= 7;

	/* 0x00EF = saa7114(239) 0x00F0 = saa7115(240) 0x0106 = micro */
	switch (itv->card->type) {
		case IVTV_CARD_PVR_350:
			/* PVR 350s always have radio */
			itv->options.radio = 1;
			break;
		case IVTV_CARD_M179:
			/* saa7114 (see API Documention on ivtv website) */
			itv->digitizer = 0xEF;
			break;
		default:
			/* saa7115 */
			itv->digitizer = 0xF0;
			break;
	}

	itv->dec_options.hide_last_frame= 1;
	itv->dec_options.pts_low	= 0;
	itv->dec_options.pts_hi		= 0;
	itv->dec_options.gop_offset	= 0;
	itv->dec_options.mute_frames	= 1;

	/* Ctrls */
	itv->dec_options.speed.mute	= 1;
	itv->dec_options.speed.aud_mute	= 0;
	itv->dec_options.speed.smooth	= 1;
	itv->dec_options.speed.fr_mask	= 2;
	itv->dec_options.speed.fr_field	= 1;
	itv->dec_options.decbuffers	= 1;
	itv->dec_options.prebuffer	= 1;

	/* OSD */
	itv->global_alpha		= 255;	
	itv->global_alpha_state		= 1;	
	itv->local_alpha_state		= 0;	

	// vbi
	itv->vbi_service_set_in = 0;
	itv->vbi_service_set_out = 0;
	itv->vbi_frame = 0;
	itv->vbi_inserted_frame = 0;
	itv->vbi_passthrough = IVTV_SLICED_WSS_625;

	/* initialize sliced VBI data buffers */
	for (i = 0; i < 36; i++) {
		memset(itv->vbi_sliced_dec_data[i].data, 0xa0,
			       	sizeof(itv->vbi_sliced_dec_data[i].data));
		memset(itv->vbi_sliced_data[i].data, 0xa0,
			       	sizeof(itv->vbi_sliced_data[i].data));
	}

	/* Timestamp */
        memset(&itv->dec_timestamp, 0,
               sizeof(struct ivtv_ioctl_framesync));
	
	init_timer(&itv->vbi_passthrough_timer);
	itv->vbi_passthrough_timer.function = ivtv_set_vbi;
	itv->vbi_passthrough_timer.data = (unsigned long)itv;

	/* Find tuner input */
	for (i = 0; (input = ivtv_get_input(itv, i)); i++) {
		if (input->type == V4L2_INPUT_TYPE_TUNER)
			break;
	}
	if (input == NULL)
		i = 0;
	itv->active_input = i;

	itv->msp34xx_audio_output = 1;  /* SCART1 output for msp34xx based cards */
	itv->audio_input_tv = 0;	/* TV tuner */
	itv->audio_input_radio = 2;	/* Radio tuner */
	itv->audmode_tv = itv->audmode_radio = V4L2_TUNER_MODE_STEREO;

	return 0;
}

static int ivtv_setup_pci(struct ivtv *itv, struct pci_dev *dev, 
	const struct pci_device_id *pci_id)
{
        u16 cmd;
	struct pci_dev *pci_bus;
	unsigned char pci_latency;

        IVTV_DEBUG(IVTV_DEBUG_INFO, "Enabling pci device\n");

        if (pci_enable_device(dev)) {
                IVTV_KERN_ERR("Can't enable device %d!\n", itv->num);
                return -EIO;
        }
        if (pci_set_dma_mask(dev, 0xffffffff)) {
                IVTV_KERN_ERR("No suitable DMA available on card %d.\n", itv->num);
                return -EIO;
        }
	snprintf(itv->name, sizeof(itv->name)-1, "ivtv%d", itv->num);
        if (!request_mem_region(pci_resource_start(dev,0), 
		IVTV_IOREMAP_SIZE, itv->name)) 
	{
                IVTV_KERN_ERR("Cannot request memory region on card %d.\n", 
			itv->num);
                return -EIO;
        }

        /* Check for bus mastering */
        pci_read_config_word(dev, PCI_COMMAND, &cmd);
        if (! (cmd & PCI_COMMAND_MASTER)) {
                IVTV_KERN_ERR("Bus Mastering is not enabled\n");
                return -ENXIO;
        }
        else {
                IVTV_DEBUG(IVTV_DEBUG_INFO, "Bus Mastering Enabled.");
        }

        pci_read_config_byte(dev, PCI_CLASS_REVISION, &itv->card_rev);
        pci_read_config_byte(dev, PCI_LATENCY_TIMER, &pci_latency);

        if (pci_latency < 64) {
                IVTV_KERN_INFO("Unreasonably low latency timer, "
			"setting to 64 (was %d)\n", pci_latency);
                pci_write_config_byte(dev, PCI_LATENCY_TIMER, 64);
                pci_read_config_byte(dev, PCI_LATENCY_TIMER, &pci_latency);
        }

        IVTV_DEBUG(IVTV_DEBUG_INFO, "%d (rev %d) at %02x:%02x.%x, ",
                itv->dev->device, itv->card_rev, dev->bus->number,
                PCI_SLOT(dev->devfn),PCI_FUNC(dev->devfn));
        IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"     irq: %d, latency: %d, memory: 0x%lx\n",
                itv->dev->irq, pci_latency, (unsigned long)itv->base_addr);

        pci_bus = pci_find_slot(0, PCI_DEVFN(0,0));

        if (pci_bus != NULL && pci_bus->vendor == PCI_VENDOR_ID_VIA) {
                IVTV_KERN_INFO("VIA PCI device: 0x%04x vendor: 0x%04x\n",
                                pci_bus->device, pci_bus->vendor);
                itv->via_fix = 1;

                if (pci_bus->device == 0x3189 /* VIA KT400 */)
                        itv->via_fix = 2;
        } else
                IVTV_KERN_INFO("XXX PCI device: 0x%04x vendor: 0x%04x\n",
                                pci_bus->device, pci_bus->vendor);

	return 0;
}

static inline void ivtv_request_module( const char *name )
{
       if ( request_module( name ) != 0 ) {
                IVTV_KERN_ERR( "Failed to load module %s\n", name );
       }
       else {
                IVTV_DEBUG( IVTV_DEBUG_INFO, "Loaded module %s\n", name );
       }
}

static int __devinit ivtv_probe(struct pci_dev *dev, const struct pci_device_id *pci_id)
{
	int retval=0;
	struct ivtv *itv;
	int freq = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Found card #%d\n", ivtv_cards_active);

	spin_lock(&ivtv_cards_lock);

	/* Make sure we've got a place for this card */
	if (ivtv_cards_active == IVTV_MAX_CARDS) {
		IVTV_KERN_ERR("Maximum number of cards detected (%d).\n", 
			ivtv_cards_active);
		spin_unlock(&ivtv_cards_lock);
		return -ENOMEM;
	}

	itv = &ivtv_cards[ivtv_cards_active];
	memset(itv, 0, sizeof(*itv));
	itv->dev = dev;
	itv->num = ivtv_cards_active++;

	spin_unlock(&ivtv_cards_lock);

	ivtv_process_options(itv);
	if (ivtv_init_struct(itv)) {
		retval = -ENOMEM;
		goto err;
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "base addr: 0x%08x\n", itv->base_addr);

	/* PCI Device Setup */
	if ((retval = ivtv_setup_pci(itv, dev, pci_id)) != 0) {
		if (retval == -EIO)
               		goto free_workqueue;
		else if (retval == -ENXIO)
               		goto free_mem;
	}
	/* save itv in the pci struct for later use*/
	pci_set_drvdata(dev, itv);

	/* map io memory */
	IVTV_DEBUG(IVTV_DEBUG_INFO, "attempting ioremap at 0x%08x len 0x%08x\n",
		itv->base_addr + IVTV_ENCODER_OFFSET, IVTV_ENCODER_SIZE);
	itv->enc_mem = ioremap_nocache(itv->base_addr + IVTV_ENCODER_OFFSET,
		IVTV_ENCODER_SIZE);
	if (!itv->enc_mem) {
		IVTV_KERN_ERR("ioremap failed, perhaps increasing ");
		IVTV_KERN_ERR("__VMALLOC_RESERVE in page.h\n");
		IVTV_KERN_ERR("or disabling CONFIG_HIMEM4G ");
		IVTV_KERN_ERR("into the kernel would help\n");
		retval = -ENOMEM;
		goto free_mem;
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "attempting ioremap at 0x%08x len 0x%08x\n",
		itv->base_addr + IVTV_DECODER_OFFSET, IVTV_DECODER_SIZE);
	itv->dec_mem = ioremap_nocache(itv->base_addr + IVTV_DECODER_OFFSET,
		IVTV_DECODER_SIZE);
	if (!itv->dec_mem) {
		IVTV_KERN_ERR("ioremap failed, perhaps increasing ");
		IVTV_KERN_ERR("__VMALLOC_RESERVE in page.h\n");
		IVTV_KERN_ERR("or disabling CONFIG_HIMEM4G ");
		IVTV_KERN_ERR("into the kernel would help\n");
		retval = -ENOMEM;
		goto free_mem;
	}

	/* map registers memory */
	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"attempting ioremap at 0x%08x len 0x%08x\n",
		itv->base_addr + IVTV_REG_OFFSET, IVTV_REG_SIZE);
	itv->reg_mem = 
		ioremap_nocache(itv->base_addr+IVTV_REG_OFFSET,IVTV_REG_SIZE);
	if (!itv->reg_mem) {
		IVTV_KERN_ERR("ioremap failed, perhaps increasing ");
		IVTV_KERN_ERR("__VMALLOC_RESERVE in page.h\n");
		IVTV_KERN_ERR("or disabling CONFIG_HIMEM4G ");
		IVTV_KERN_ERR("into the kernel would help\n");
		retval = -ENOMEM;
		goto free_io;
	}	

	/* active i2c  */
	IVTV_DEBUG(IVTV_DEBUG_INFO, "activating i2c...\n");
	if (init_ivtv_i2c(itv)) {
		IVTV_KERN_ERR("Could not initialize i2c\n");
		goto free_irq;
	}
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, 
		"Active card count: %d.\n", ivtv_cards_active);
	
	switch (itv->card->type) {
		/* Hauppauge models */
		case IVTV_CARD_PVR_250:
		case IVTV_CARD_PVR_350:
			if (autoload)
				ivtv_request_module("tveeprom");
			ivtv_read_eeprom(itv);
			break;
		default:
			break;
	}
		
	if (autoload) {
		ivtv_request_module("tuner");
		ivtv_request_module("saa7115");
		if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
			ivtv_request_module("saa7127");
		}
		if (itv->card->audio_selector == USE_MSP34XX) {
			ivtv_request_module("msp3400");
		}
		if (tda9887 >= 0) {
			ivtv_request_module("tda9887");
		}
	}
	
	if (itv->options.radio == -1)
		itv->options.radio = 0;
	if (itv->options.radio)
		itv->v4l2_cap |= V4L2_CAP_RADIO;

	/* write firmware */
	retval = ivtv_firmware_init(itv);
	if (retval) {
		IVTV_KERN_ERR("Error %d initializing firmware.\n", retval);
		retval = -ENOMEM;
		goto free_i2c;
	}

	/* search for encoder/decoder mailboxes*/
	IVTV_DEBUG(IVTV_DEBUG_INFO, "About to search for mailboxes\n");
	ivtv_find_firmware_mailbox(itv);

	if ((itv->enc_mbox == NULL) && (itv->dec_mbox == NULL)) {
		IVTV_KERN_ERR("Error locating firmware.\n");
		retval = -ENOMEM;
		goto free_i2c;
	}

	/* Try and get firmware versions */
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Getting firmware version..\n");
	retval = ivtv_firmware_versions(itv);
	if (retval) {
		IVTV_KERN_ERR("Error %d getting firmware version!\n", retval);
		goto free_i2c;
	}

	retval = ivtv_streams_setup(itv);
	if (retval) {
		IVTV_KERN_ERR("Error %d setting up streams\n", retval);
		goto free_i2c;
	}

	/* Start Threads */
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Starting Threads\n");
        atomic_set(&itv->enc_thread_running, 1);
        itv->enc_thread_pid = kernel_thread(ivtv_enc_thread, itv,
                                                    CLONE_FS | CLONE_FILES);

        /* Decoder Thread */
        if ((itv->card->type == IVTV_CARD_PVR_350))
        {
                atomic_set(&itv->dec_thread_running, 1);
                itv->dec_thread_pid = kernel_thread(ivtv_dec_thread, itv,
                                                    CLONE_FS | CLONE_FILES);
        }

	IVTV_DEBUG(IVTV_DEBUG_IRQ, "Masking interrupts\n");
	/* clear interrupt mask, effectively disabling interrupts */
	ivtv_set_irq_mask(itv, 0xffffffff);

	/* Register IRQ */
	snprintf(itv->name, sizeof(itv->name)-1, "ivtv%d", itv->num);
	retval = request_irq(itv->dev->irq, ivtv_irq_handler,
			SA_SHIRQ | SA_INTERRUPT, itv->name, (void *)itv);
	if (retval) {
		IVTV_KERN_ERR("Failed to register irq %d\n", retval);
		goto free_i2c;
	}

        /* Default interrupts enabled */
	if ((itv->card->type == IVTV_CARD_PVR_350))
        	ivtv_clear_irq_mask(itv, IVTV_IRQ_MASK_INIT|IVTV_IRQ_DEC_VSYNC);
	else
        	ivtv_clear_irq_mask(itv, IVTV_IRQ_MASK_INIT);

	if (itv->options.tuner > -1) {
		IVTV_KERN_INFO("Setting Tuner %d\n", itv->options.tuner);
		ivtv_tuner(itv, TUNER_SET_TYPE, &(itv->options.tuner) );
	}

	if (itv->std == 0) {
		IVTV_KERN_ERR("Could not detect tuner standard, defaulting to NTSC.\n");
		itv->std = V4L2_STD_NTSC;
	}

	if (itv->std & V4L2_STD_NTSC) {
		/* Why on earth? */
               	freq = 1076; /* ch. 4 67250*16/1000 */
        }

	/* The tuner is fixed to the norm. The other inputs (e.g. S-Video)
	   are not. */
	itv->tuner_std = itv->std;

        /* set the channel */
	if (freq)
               	ivtv_tuner(itv, VIDIOCSFREQ, &freq);

	/* Init new audio input/output */
	ivtv_audio_set_io(itv);

	/* Let the VIDIOC_S_STD ioctl do all the work, keeps the code
	   in one place. */
	itv->std++;  // Force full standard initialization
	ivtv_v4l2_ioctls(itv, NULL, 0, VIDIOC_S_STD, &itv->tuner_std);
	
	IVTV_KERN_INFO("Initialized %s, card #%d\n",
		       	itv->card->name, itv->num);

	return 0;
	
free_irq:
	free_irq(itv->dev->irq, (void *)itv);
free_i2c:
	exit_ivtv_i2c(itv);
free_io:
	ivtv_iounmap(itv);
free_mem:
	release_mem_region(pci_resource_start(itv->dev,0), IVTV_IOREMAP_SIZE);
free_workqueue:
#ifdef LINUX26
	destroy_workqueue(itv->vbi_work_queues);
	destroy_workqueue(itv->fw_work_queues);
#endif
err:
	IVTV_KERN_ERR("Error %d on initialization\n", retval);

	spin_lock(&ivtv_cards_lock);
	ivtv_cards_active--;
	spin_unlock(&ivtv_cards_lock);
	return retval;
}

static void ivtv_remove(struct pci_dev *pci_dev)
{
	struct ivtv *itv = pci_get_drvdata(pci_dev);

	/* Lock firmware reloads */
	itv->fw_reset_counter = 99;
	set_bit(FW_RESET_SHUTDOWN, &itv->r_flags);

	IVTV_DEBUG(IVTV_DEBUG_ERR, "Removing Card #%d.\n", itv->num);

	/* Stop all captures */
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Stopping all streams.\n");
	itv->end_gop = 0;
	if (atomic_read(&itv->capturing) > 0) 
		ivtv_stop_all_captures(itv);

	/* Stop all decoding */
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Stopping decoding.\n");
	if ((itv->card->type == IVTV_CARD_PVR_350)
			&& atomic_read(&itv->decoding) > 0) 
	{
        	ivtv_stop_v4l2_decode_stream(itv, IVTV_DEC_STREAM_TYPE_MPG);
	}

	/* Interrupts */
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Disabling interrupts.\n");
	ivtv_set_irq_mask(itv, 0xffffffff);

        /* Stop all Work Queues */
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Flushing Work Queues.\n"); 
        ivtv_flush_scheduled_work(itv);
#ifdef LINUX26
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Cancel Work Queues.\n"); 
        cancel_delayed_work(&itv->fw_work_queue);
        cancel_delayed_work(&itv->vbi_work_queue);

	IVTV_DEBUG(IVTV_DEBUG_INFO, " Destroy Work Queues.\n"); 
        destroy_workqueue(itv->vbi_work_queues);
        destroy_workqueue(itv->fw_work_queues);
#endif
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Stopping Threads.\n"); 
	/* Stop all threads */
        atomic_set(&itv->enc_thread_running, 0);
	atomic_set(&itv->enc_dma_stat_intr, 1);
	itv->t_flags = 0;	
        wake_up(&itv->enc_thread_w);

	if (itv->enc_thread_pid)
		kill_proc(itv->enc_thread_pid, SIGKILL, 1);

	if (itv->card->type == IVTV_CARD_PVR_350) {
        	atomic_set(&itv->dec_thread_running, 0);
		atomic_set(&itv->dec_dma_stat.intr, 1);
        	wake_up(&itv->dec_thread_w);

		if (itv->dec_thread_pid)
			kill_proc(itv->dec_thread_pid, SIGKILL, 1);

	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, " Stopping Passthrough.\n");
	clear_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags);
	del_timer_sync(&itv->vbi_passthrough_timer);

	IVTV_DEBUG(IVTV_DEBUG_INFO, " Stopping Firmware.\n");
	ivtv_halt_firmware(itv, 3);

	IVTV_DEBUG(IVTV_DEBUG_INFO, " Freeing dma resources.\n");
	IVTV_DEBUG(IVTV_DEBUG_INFO, " Unregistering v4l devices.\n");
	ivtv_streams_cleanup(itv);

	exit_ivtv_i2c(itv);

	IVTV_DEBUG(IVTV_DEBUG_INFO, " Releasing irq.\n");
	free_irq(itv->dev->irq, (void *)itv);

	if (itv->dev) {
	    ivtv_iounmap(itv);
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, " Releasing mem.\n");
	release_mem_region(pci_resource_start(itv->dev,0), IVTV_IOREMAP_SIZE);
	
	IVTV_KERN_INFO("Removed %s, card #%d\n",
		       	itv->card->name, itv->num);
}

/* define a pci_driver for card detection */
static struct pci_driver ivtv_pci_driver = {
	name:     "ivtv-iTVC15_16_mpg2_encoder_card",
	id_table: ivtv_pci_tbl,
	probe:	  ivtv_probe,
	remove:	  ivtv_remove,
};



static int module_start(void)
{
	int i;

	IVTV_DEBUG(IVTV_DEBUG_ERR, "==================== START INIT IVTV ====================\n");
	IVTV_KERN_INFO("version %s (%s) loading\n",
			IVTV_VERSION_STRING(IVTV_VERSION_INFO_NAME),
			IVTV_VERSION_COMMENT(IVTV_VERSION_INFO_NAME));
#ifdef LINUX26
	IVTV_DEBUG(IVTV_DEBUG_ERR, "Linux version: " VERMAGIC_STRING "\n");
#else
	IVTV_DEBUG(IVTV_DEBUG_ERR, "Linux version: " UTS_RELEASE "\n");
#endif
	IVTV_DEBUG(IVTV_DEBUG_ERR, "In case of problems please include the debug info\n");
	IVTV_DEBUG(IVTV_DEBUG_ERR, "between the START INIT IVTV and END INIT IVTV lines when\n");
	IVTV_DEBUG(IVTV_DEBUG_ERR, "mailing the ivtv-devel mailinglist.\n");

	memset(&ivtv_cards[0], 0, IVTV_MAX_CARDS * sizeof(struct ivtv));

	/* Validate parameters */
	if (ivtv_first_minor < 0 || ivtv_first_minor >= IVTV_MAX_CARDS) {
		IVTV_KERN_ERR("ivtv_first_minor must be between 0 and %d. Exiting...\n",
				IVTV_MAX_CARDS - 1);
		return -1;
	}

#ifdef YUV_FIXUP
	if ((yuv_fixup != 0) && (yuv_fixup != 1)) {
		IVTV_KERN_ERR("yuv_fixup must be 0 or 1. Exiting...\n");
		return -1;
	}
#endif
	
	if (ivtv_debug < 0 || ivtv_debug > 255) {
		ivtv_debug = 1;
		IVTV_DEBUG(IVTV_DEBUG_ERR,"debug value must be >= 0 and <= 255!\n");
	}

       	IVTV_DEBUG(IVTV_DEBUG_INFO,
		   "... running on kernel %s\n", UTS_RELEASE);
	IVTV_DEBUG(IVTV_DEBUG_INFO,
		   "Setting some variables to invalid for detection\n");

	for (i = 0; i < IVTV_MAX_CARDS; i++) {
		ivtv_cards[i].num = -1;
		ivtv_cards[i].dev = NULL;
	}

       	IVTV_DEBUG(IVTV_DEBUG_INFO, "Scanning PCI bus..\n");
	if (pci_module_init(&ivtv_pci_driver)) {
		IVTV_KERN_ERR("Error detecting PCI card\n");	
		return -ENODEV;
	}
	IVTV_DEBUG(IVTV_DEBUG_ERR, "====================  END INIT IVTV  ====================\n");
	return 0;
}

static void module_cleanup(void)
{
	pci_unregister_driver(&ivtv_pci_driver);
}

#ifdef LINUX26
EXPORT_SYMBOL(ivtv_set_irq_mask);
EXPORT_SYMBOL(ivtv_cards_active);
EXPORT_SYMBOL(ivtv_cards);
EXPORT_SYMBOL(ivtv_api);
EXPORT_SYMBOL(ivtv_vapi);
EXPORT_SYMBOL(ivtv_clear_irq_mask);
EXPORT_SYMBOL(ivtv_debug);
#endif
module_init(module_start);
module_exit(module_cleanup);
