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
#include "ivtv-version.h"
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
#include "ivtv-dma.h"
#include "cx25840.h"
#include "ivtv-gpio.h"

#ifdef LINUX26
#include <linux/vermagic.h>
#else
#include <linux/version.h>
#include <linux/kmod.h>
#endif /* LINUX26 */

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
struct ivtv *ivtv_cards[IVTV_MAX_CARDS];

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

MODULE_DEVICE_TABLE(pci,ivtv_pci_tbl);

u32 yuv_offset[4] = {IVTV_YUV_BUFFER_OFFSET,
                     IVTV_YUV_BUFFER_OFFSET_1,
                     IVTV_YUV_BUFFER_OFFSET_2,
                     IVTV_YUV_BUFFER_OFFSET_3};

/* Parameter declarations */
static int cardtype[IVTV_MAX_CARDS];
static int tuner[IVTV_MAX_CARDS] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
static int radio[IVTV_MAX_CARDS] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
/* tuner.h tuner type for ivtv card */
static int tda9887[IVTV_MAX_CARDS] = { -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2 };


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
static int cardtype_c = 1;
static int tuner_c = 1;
static int radio_c = 1;
static int tda9887_c = 1;
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) */
static int yuv_fixup = 0;
#ifdef MODULE
static unsigned int autoload = 1;
#else
static unsigned int autoload = 0;
#endif /* MODULE */

static int ivtv_std = 0;

/* Buffers */
static int mpg_buffers = IVTV_DEFAULT_MPG_BUFFERS;
static int yuv_buffers = IVTV_DEFAULT_YUV_BUFFERS;
static int vbi_buffers = IVTV_DEFAULT_VBI_BUFFERS;
static int pcm_buffers = IVTV_DEFAULT_PCM_BUFFERS;
static int dec_mpg_buffers = IVTV_DEFAULT_DEC_MPG_BUFFERS;
static int dec_yuv_buffers = IVTV_DEFAULT_DEC_YUV_BUFFERS;
static int dec_vbi_buffers = IVTV_DEFAULT_DEC_VBI_BUFFERS;
static int dec_osd_buffers = IVTV_DEFAULT_DEC_OSD_BUFFERS;

static int max_mpg_buffers = IVTV_MAX_MPG_BUFFERS;
static int max_yuv_buffers = IVTV_MAX_YUV_BUFFERS;
static int max_vbi_buffers = IVTV_MAX_VBI_BUFFERS;
static int max_pcm_buffers = IVTV_MAX_PCM_BUFFERS;
static int max_dec_mpg_buffers = IVTV_MAX_DEC_MPG_BUFFERS;
static int max_dec_yuv_buffers = IVTV_MAX_DEC_YUV_BUFFERS;
static int max_dec_vbi_buffers = IVTV_MAX_DEC_VBI_BUFFERS;
static int max_dec_osd_buffers = IVTV_MAX_DEC_OSD_BUFFERS;

static int ivtv_dynbuf = 0;
static int ivtv_yuv_mode = 0;
static int ivtv_yuv_threshold=480;


char *ivtv_efw = NULL;
char *ivtv_dfw = NULL;

int ivtv_debug = IVTV_DBGFLG_WARN;

int errno;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(yuv_fixup, int, 0644);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 10)
module_param_array(tuner, int, &tuner_c, 0644);
module_param_array(radio, bool, &radio_c, 0644);
module_param_array(cardtype, int, &cardtype_c, 0644);
module_param_array(tda9887, int, &tda9887_c, 0644);
#else
module_param_array(tuner, int, tuner_c, 0644);
module_param_array(radio, bool, radio_c, 0644);
module_param_array(cardtype, int, cardtype_c, 0644);
module_param_array(tda9887, int, tda9887_c, 0644);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 10) */
module_param(autoload, int, 0644);
module_param(ivtv_debug, int, 0644);
module_param(ivtv_std, int, 0644);
module_param(ivtv_dynbuf, int, 0644);
module_param(ivtv_yuv_mode, int, 0644);
module_param(ivtv_yuv_threshold, int, 0644);
module_param(ivtv_efw, charp, 0644);
module_param(ivtv_dfw, charp, 0644);
module_param(ivtv_first_minor, int, 0644);

module_param(mpg_buffers, int, 0644);
module_param(yuv_buffers, int, 0644);
module_param(vbi_buffers, int, 0644);
module_param(pcm_buffers, int, 0644);
module_param(dec_mpg_buffers, int, 0644);
module_param(dec_yuv_buffers, int, 0644);
module_param(dec_vbi_buffers, int, 0644);
module_param(dec_osd_buffers, int, 0644);

module_param(max_mpg_buffers, int, 0644);
module_param(max_yuv_buffers, int, 0644);
module_param(max_vbi_buffers, int, 0644);
module_param(max_pcm_buffers, int, 0644);
module_param(max_dec_mpg_buffers, int, 0644);
module_param(max_dec_yuv_buffers, int, 0644);
module_param(max_dec_vbi_buffers, int, 0644);
module_param(max_dec_osd_buffers, int, 0644);
#else
MODULE_PARM(yuv_fixup, "i");
MODULE_PARM(tuner, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(tda9887, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(radio, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(autoload, "i");
MODULE_PARM(cardtype, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM(ivtv_debug, "i");
MODULE_PARM(ivtv_std, "i");
MODULE_PARM(ivtv_dynbuf, "i");
MODULE_PARM(ivtv_yuv_mode, "i");
MODULE_PARM(ivtv_yuv_threshold, "i");
MODULE_PARM(ivtv_efw, "s");
MODULE_PARM(ivtv_dfw, "s");
MODULE_PARM(ivtv_first_minor, "i");

MODULE_PARM(mpg_buffers, "i");
MODULE_PARM(yuv_buffers, "i");
MODULE_PARM(vbi_buffers, "i");
MODULE_PARM(pcm_buffers, "i");
MODULE_PARM(dec_mpg_buffers, "i");
MODULE_PARM(dec_yuv_buffers, "i");
MODULE_PARM(dec_vbi_buffers, "i");
MODULE_PARM(dec_osd_buffers, "i");

MODULE_PARM(max_mpg_buffers, "i");
MODULE_PARM(max_yuv_buffers, "i");
MODULE_PARM(max_vbi_buffers, "i");
MODULE_PARM(max_pcm_buffers, "i");
MODULE_PARM(max_dec_mpg_buffers, "i");
MODULE_PARM(max_dec_yuv_buffers, "i");
MODULE_PARM(max_dec_vbi_buffers, "i");
MODULE_PARM(max_dec_osd_buffers, "i");

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) */

MODULE_PARM_DESC(yuv_fixup,
		 "Toggles conversion of Hauppauge\n"
		 "\t\t\tMacroblock NV12 to NV12");

MODULE_PARM_DESC(tuner, "Tuner type selection,\n"
			"\t\t\tsee tuner.h for values");
MODULE_PARM_DESC(tda9887,
		 "Configure or disable tda9887 module. Use only if autodetection\n"
		 "\t\t\tfails. -1 = disable, 0 = normal, 1 = no QSS");
MODULE_PARM_DESC(radio,
		 "Enable or disable the radio. Use only if autodetection\n"
		 "\t\t\tfails. 0 = disable, 1 = enable");
MODULE_PARM_DESC(autoload,
		 "automatically load i2c modules like tuner.o,\n"
		 "\t\t\tdefault is 1 (yes)");
MODULE_PARM_DESC(cardtype,
		 "Only use this option if your card is not detected properly.\n"
		 "\t\tSpecify card type:\n"
                 "\t\t\t1 = WinTV PVR 250\n"
		 "\t\t\t2 = WinTV PVR 350\n"
                 "\t\t\t3 = AVerMedia M179\n"
		 "\t\t\t4 = YUAN MPG600/Kuroutoshikou iTVC16-STVLP\n"
		 "\t\t\t5 = YUAN MPG160/Kuroutoshikou iTVC15-STVLP\n"
		 "\t\t\t6 = WinTV PVR-150 or PVR-500\n"
		 "\t\t\t7 = YUAN PG600/DIAMONDMM PVR-550 (CX Falcon 2)\n"
		 "\t\t\t8 = Adaptec AVC-2410\n"
		 "\t\t\t9 = Adaptec AVC-2010\n"
		 "\t\t\tDefault: Autodetect");
MODULE_PARM_DESC(ivtv_debug,
		 "Debug level (bitmask). Default: errors only\n"
		 "\t\t\t(ivtv_debug = 511 gives full debugging)");
MODULE_PARM_DESC(ivtv_std,
		 "Specify video standard: 1 = NTSC, 2 = PAL, 3 = SECAM,\n"
		 "\t\t\tDefault: Autodetect");
MODULE_PARM_DESC(ivtv_dynbuf,
		 "Specify using Dynamic Buffers: 0 = No, 1 = Yes,\n"
		 "\t\t\tDefault: No");
MODULE_PARM_DESC(ivtv_yuv_mode,
		 "Specify the yuv playback mode:\n"
                 "\t\t\t0 = interlaced\n\t\t\t1 = progressive\n\t\t\t2 = auto\n"
		 "\t\t\tDefault: 0 (interlaced)");
MODULE_PARM_DESC(ivtv_yuv_threshold,
		 "If ivtv_yuv_mode is 2 (auto) then playback content as\n\t\tprogressive if src height <= ivtv_yuvthreshold\n"
		 "\t\t\tDefault: 480");;
MODULE_PARM_DESC(ivtv_efw,
		 "Encoder firmware image\n"
		 "\t\t\tDefault: " IVTV_FIRM_ENC_FILENAME);
MODULE_PARM_DESC(ivtv_dfw,
		 "Decoder firmware image\n"
		 "\t\t\tDefault: " IVTV_FIRM_DEC_FILENAME);
MODULE_PARM_DESC(mpg_buffers,
		 "Enc MPG Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_MPG_BUFFERS_CNT);
MODULE_PARM_DESC(max_mpg_buffers,
		 "Max Enc MPG Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_MPG_BUFFERS_CNT);
MODULE_PARM_DESC(yuv_buffers,
		 "Enc YUV Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_YUV_BUFFERS_CNT);
MODULE_PARM_DESC(max_yuv_buffers,
		 "Max Enc YUV Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_YUV_BUFFERS_CNT);
MODULE_PARM_DESC(vbi_buffers,
		 "Enc VBI Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_VBI_BUFFERS_CNT);
MODULE_PARM_DESC(max_vbi_buffers,
		 "Max Enc VBI Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_VBI_BUFFERS_CNT);
MODULE_PARM_DESC(pcm_buffers,
		 "Enc PCM buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_PCM_BUFFERS_CNT);
MODULE_PARM_DESC(max_pcm_buffers,
		 "Max Enc PCM Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_PCM_BUFFERS_CNT);
MODULE_PARM_DESC(dec_mpg_buffers,
		 "Dec MPG buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_DEC_MPG_BUFFERS_CNT);
MODULE_PARM_DESC(max_dec_mpg_buffers,
		 "Max Dec MPG Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_DEC_MPG_BUFFERS_CNT);
MODULE_PARM_DESC(dec_yuv_buffers,
		 "Dec YUV buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_DEC_YUV_BUFFERS_CNT);
MODULE_PARM_DESC(max_dec_yuv_buffers,
		 "Max Dec YUV Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_DEC_YUV_BUFFERS_CNT);
MODULE_PARM_DESC(dec_vbi_buffers,
		 "Dec VBI buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_DEC_VBI_BUFFERS_CNT);
MODULE_PARM_DESC(max_dec_vbi_buffers,
		 "Max Dec VBI Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_DEC_VBI_BUFFERS_CNT);
MODULE_PARM_DESC(dec_osd_buffers,
		 "Dec OSD buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_DEFAULT_DEC_OSD_BUFFERS_CNT);
MODULE_PARM_DESC(max_dec_osd_buffers,
		 "Max Dec OSD Buffers (in megs)\n"
		 "\t\t\tDefault: " IVTV_MAX_DEC_OSD_BUFFERS_CNT);

MODULE_PARM_DESC(ivtv_first_minor, "Set minor assigned to first card");

MODULE_AUTHOR("Kevin Thayer");
MODULE_DESCRIPTION("iTVC15_16 driver");
MODULE_SUPPORTED_DEVICE
    ("iTVC15_16 mpg2 encoder (WinTV PVR-150/250/350/500,\n"
		"\t\t\t Yuan MPG series and similar)");
MODULE_LICENSE("GPL");

#if defined(LINUX26)
MODULE_VERSION(IVTV_VERSION);
#endif /* defined(LINUX26) */

void ivtv_clear_irq_mask(struct ivtv *itv, unsigned long mask)
{
	itv->irqmask &= ~mask;
	writel(itv->irqmask, (itv->reg_mem + IVTV_REG_IRQMASK));

	/* pci posting */
	readl(itv->reg_mem + IVTV_REG_IRQMASK);
}

void ivtv_set_irq_mask(struct ivtv *itv, unsigned long mask)
{
	itv->irqmask |= mask;
	writel(itv->irqmask, (itv->reg_mem + IVTV_REG_IRQMASK));

	/* pci posting */
	readl(itv->reg_mem + IVTV_REG_IRQMASK);
}

void enc_schedule_work(struct ivtv *itv)
{
	wake_up(&itv->enc_thread_w);
}

void dec_vbi_schedule_work(struct ivtv *itv)
{
        wake_up(&itv->dec_vbi_thread_w);
}

void enc_vbi_schedule_work(struct ivtv *itv)
{
        wake_up(&itv->enc_vbi_thread_w);
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
#endif /* LINUX26 */
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
#endif /* LINUX26 */

void ivtv_flush_scheduled_work(struct ivtv *itv)
{
#ifdef LINUX26
	flush_scheduled_work();
	flush_workqueue(itv->vbi_work_queues);
	flush_workqueue(itv->fw_work_queues);
#else
	flush_scheduled_tasks();
#endif /* LINUX26 */
}

/* Release ioremapped memory */
static void ivtv_iounmap(struct ivtv *itv)
{
	if (itv == NULL)
		return;

	/* Release registers memory */
	if (itv->reg_mem != NULL) {
		IVTV_DEBUG_INFO("releasing reg_mem\n");
		iounmap(itv->reg_mem);
		itv->reg_mem = NULL;
	}
	/* Release io memory */
	if (itv->dec_mem != NULL) {
		IVTV_DEBUG_INFO("releasing dec_mem\n");
		iounmap(itv->dec_mem);
		itv->dec_mem = NULL;
	}

	/* Release io memory */
	if (itv->enc_mem != NULL) {
		IVTV_DEBUG_INFO("releasing enc_mem\n");
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
		IVTV_ERR("Error %d reading Hauppauge eeprom.\n", ret);
		IVTV_ERR
		    ("Possible causes: the tveeprom module was not loaded, or\n");
		IVTV_ERR
		    ("the eeprom kernel module was loaded before the tveeprom module.\n");
		return ret;
	}

        /* if the model is a 23xxx (PVR500), and this is the 'PVR150' on
           that card (subsystem device ID has bit 4 set), then the eeprom
           reported incorrectly that a radio is present. A radio is only
           available for the first 'PVR150'. 
         
           Many thanks to Hauppauge for providing this data. */
        if (eepromdata[2] / 1000 == 23) {
                int is_first = (itv->dev->subsystem_device & 0x0010) == 0;

                IVTV_INFO("This is the %s unit of a PVR500\n",
                                is_first ? "first" : "second");
                itv->card_name = is_first ? "WinTV PVR 500 (unit #1)" :
                                            "WinTV PVR 500 (unit #2)";
                if (!is_first) {
                        IVTV_INFO("Correcting tveeprom data: no radio present on second unit\n");
                        eepromdata[4] = 0;
                }
        }

#if 0
        // not yet...
        switch (eepromdata[2]) {
                // There is one confirmed case where this card
                // uses the PCI subsystem IDs of the PVR250, thus
                // being misidentified. Correct this based on the
                // more reliable eeprom model number.
                case 26034:
                        itv->card = ivtv_get_card(IVTV_CARD_PVR_150);
                        break;
                default:
                        break;
        }
#endif

	if (itv->options.tuner == -1)
		itv->options.tuner = eepromdata[0];
	if (itv->options.radio == -1)
		itv->options.radio = (eepromdata[4] != 0);
	if (itv->std != 0) {
                // user specified tuner standard
                return ret;
        }

        /* autodetect tuner standard */
        if (eepromdata[1] & V4L2_STD_PAL) {
                IVTV_DEBUG_INFO("PAL tuner detected\n");
                itv->std |= V4L2_STD_PAL;
                itv->codec.framespergop = 12;
                ivtv_std = 2;
        } else if (eepromdata[1] & V4L2_STD_NTSC) {
                IVTV_DEBUG_INFO("NTSC tuner detected\n");
                itv->std |= V4L2_STD_NTSC;
                itv->codec.framespergop = 15;
                ivtv_std = 1;
        } else if (eepromdata[1] & V4L2_STD_SECAM) {
                IVTV_DEBUG_INFO("SECAM tuner detected\n");
                itv->std |= V4L2_STD_SECAM;
                itv->codec.framespergop = 12;
                ivtv_std = 3;
        } else if (ivtv_std == 0) {
                IVTV_INFO("No tuner detected, default to NTSC\n");
                itv->std |= V4L2_STD_NTSC;
                itv->codec.framespergop = 15;
                ivtv_std = 1;
        } else {
                IVTV_INFO("No tuner detected, and std forced to %d\n", 
                                ivtv_std);
        }
	return ret;
}

static void ivtv_process_options(struct ivtv *itv)
{
        const char *chipname;
	int i, j;

	/* Buffers are now dynamically allocated, could specify limit though */
	itv->options.yuv_fixup = yuv_fixup;
	itv->options.mpg_buffers = mpg_buffers;
	itv->options.max_mpg_buffers = max_mpg_buffers;
	itv->options.yuv_buffers = yuv_buffers;
	itv->options.max_yuv_buffers = max_yuv_buffers;
	itv->options.vbi_buffers = vbi_buffers;
	itv->options.max_vbi_buffers = max_vbi_buffers;
	itv->options.pcm_buffers = pcm_buffers;
	itv->options.max_pcm_buffers = max_pcm_buffers;
	itv->options.dec_mpg_buffers = dec_mpg_buffers;
	itv->options.max_dec_mpg_buffers = max_dec_mpg_buffers;
	itv->options.dec_yuv_buffers = dec_yuv_buffers;
	itv->options.max_dec_yuv_buffers = max_dec_yuv_buffers;
	itv->options.dec_osd_buffers = dec_osd_buffers;
	itv->options.max_dec_osd_buffers = max_dec_osd_buffers;
	itv->options.dec_vbi_buffers = dec_vbi_buffers;
	itv->options.max_dec_vbi_buffers = max_dec_vbi_buffers;
	itv->options.cardtype = cardtype[itv->num];
	itv->options.tuner = tuner[itv->num];
	itv->options.radio = radio[itv->num];
	itv->options.tda9887 = tda9887[itv->num];
	itv->options.dynbuf = ivtv_dynbuf;

        itv->has_itvc15 = (itv->dev->device == PCI_DEVICE_ID_IVTV15);
        chipname = itv->has_itvc15 ? "iTVC15" : "iTVC16";
	if ((itv->card = ivtv_get_card(itv->options.cardtype - 1))) {
		IVTV_INFO("User specified %s card (detected %s based chip)\n",
                                itv->card->name, chipname);
	} else {
                if (itv->options.cardtype != 0) {
		        IVTV_ERR("Unknown user specified type, trying to autodetect card\n");
                }
		for (i = 0; (itv->card = ivtv_get_card(i)); i++) {
			for (j = 0; itv->card->pci_list[j].device; j++) {
				if (itv->dev->device !=
				    itv->card->pci_list[j].device)
					continue;
				if (itv->dev->subsystem_vendor !=
				    itv->card->pci_list[j].subsystem_vendor)
					continue;
				if (itv->dev->subsystem_device !=
				    itv->card->pci_list[j].subsystem_device)
					continue;
				IVTV_INFO("Autodetected %s card (%s based)\n",
                                                itv->card->name, chipname);
				goto done;
			}
		}

	      done:
		if (itv->card == NULL) {
			itv->card = ivtv_get_card(IVTV_CARD_PVR_250);
			IVTV_ERR("Unknown card: vendor/device: %04x/%04x\n",
			     itv->dev->vendor, itv->dev->device);
                        IVTV_ERR("              subsystem vendor/device: %04x/%04x\n",
			     itv->dev->subsystem_vendor, itv->dev->subsystem_device);
                        IVTV_ERR("              %s based\n", chipname);
			IVTV_ERR("Defaulting to %s card\n", itv->card->name);
			IVTV_ERR("Please mail the vendor/device and subsystem vendor/device IDs and what kind of\n");
			IVTV_ERR("card you have to the ivtv-devel mailinglist: http://sourceforge.net/projects/ivtv\n");
			IVTV_ERR("Prefix your subject line with [UNKNOWN CARD].\n");
		}
	}
	itv->v4l2_cap = itv->card->v4l2_capabilities;
        itv->card_name = itv->card->name;

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
   the dev and num fields have been filled in.
   No assumptions on the card type may be made here (see ivtv_init_struct2
   for that).
 */
static int __devinit ivtv_init_struct1(struct ivtv *itv)
{
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
	init_MUTEX(&itv->vbi_dec_lock);

	itv->DMA_slock = SPIN_LOCK_UNLOCKED;

#ifdef LINUX26
	itv->vbi_work_queues = create_workqueue("ivtv_vbi");
	if (itv->vbi_work_queues == NULL) {
		IVTV_ERR("Could not create VBI workqueue\n");
		return -1;
	}
	itv->fw_work_queues = create_workqueue("ivtv_fw");
	if (itv->fw_work_queues == NULL) {
		IVTV_ERR("Could not create FW workqueue\n");
		return -1;
	}
	INIT_WORK(&itv->vbi_work_queue, vbi_work_handler, itv);
	INIT_WORK(&itv->fw_work_queue, fw_work_handler, itv);
#else
	INIT_TQUEUE(&itv->vbi_work_queue, vbi_work_handler, itv);
	INIT_TQUEUE(&itv->fw_work_queue, fw_work_handler, itv);
#endif /* LINUX26 */

	/* start counting open_id at 1 */
	itv->open_id = 1;

	/* Initial settings */
	itv->codec.bitrate_mode = 0;
	itv->codec.bitrate = 8000000;
	itv->codec.bitrate_peak = 9600000;
	itv->codec.stream_type = IVTV_STREAM_DVD_S2;
	itv->codec.bframes = 3;
	itv->codec.gop_closure = 1;
	itv->codec.dnr_mode = 0;
	itv->codec.dnr_type = 0;
	itv->codec.dnr_spatial = 0;
	itv->codec.dnr_temporal = 8;
	itv->codec.aspect = 2;
	itv->codec.pulldown = 0;
	/* Freq 48 kHz, Layer II, 384 kbit/s */
	itv->codec.audio_bitmask = 1 | (2 << 2) | (14 << 4);

	init_waitqueue_head(&itv->cap_w);
	init_waitqueue_head(&itv->vsync_w_fs);
	init_waitqueue_head(&itv->vsync_w_osd);
	init_waitqueue_head(&itv->vsync_w_vbi);
	init_waitqueue_head(&itv->vsync_w_yuv);

	/* DMA Settings */
	itv->dma_cfg.dec_max_xfer = IVTV_DMA_MAX_XFER;
	itv->dma_cfg.dec_min_xfer = IVTV_DMA_MIN_XFER;
	itv->dma_cfg.osd_sg_size = IVTV_DMA_SG_OSD_SIZE;
	itv->dma_cfg.osd_sg_ent = IVTV_DMA_SG_OSD_ENT;
	itv->dma_cfg.enc_buf_size = IVTV_DMA_ENC_BUF_SIZE;
        itv->dma_cfg.enc_yuv_buf_size = IVTV_DMA_ENC_YUV_BUF_SIZE;

	itv->dma_cfg.enc_pcm_buf_size = IVTV_DMA_ENC_PCM_BUF_SIZE;
	itv->dma_cfg.dec_buf_size = IVTV_DMA_DEC_BUF_SIZE;
        itv->dma_cfg.dec_yuv_buf_size = IVTV_DMA_DEC_YUV_BUF_SIZE;

	itv->dma_cfg.dec_vbi_buf_size = IVTV_DMA_DEC_VBI_BUF_SIZE;

	itv->dma_cfg.max_mpg_buf = max_mpg_buffers;
	itv->dma_cfg.max_yuv_buf = max_yuv_buffers;
	itv->dma_cfg.max_vbi_buf = max_vbi_buffers;
	itv->dma_cfg.max_pcm_buf = max_pcm_buffers;
	itv->dma_cfg.max_dec_mpg_buf = max_dec_mpg_buffers;
	itv->dma_cfg.max_dec_yuv_buf = max_dec_yuv_buffers;

	itv->dma_cfg.fw_dec_dma_xfer = FW_DEC_DMA_XFER;
	itv->dma_cfg.fw_enc_dma_xfer = FW_ENC_DMA_XFER;
	itv->dma_cfg.fw_enc_dma_type = FW_ENC_DMA_TYPE;
	itv->dma_cfg.vbi_pio = IVTV_VBI_PIO;
	itv->dma_cfg.enc_pio = IVTV_ENC_PIO;
	itv->dma_cfg.dec_pio = IVTV_DEC_PIO;
	itv->dma_cfg.osd_pio = 0;

	/* Decoder DMA State */
	itv->dec_dma_stat.last_addr = 0;
	itv->dec_dma_stat.bytes_filled = 0;
	itv->dec_dma_stat.bytes_needed = 0;
	itv->dec_dma_stat.last_xfer = 0;
	itv->dec_dma_stat.total_xfer = 0;
	itv->dec_dma_stat.sg_bytes = 0;
	atomic_set(&itv->dec_dma_stat.type, -1);

	/* Encoder Options */
	itv->end_gop = 0;
	itv->idx_sdf_offset = 0;
	itv->idx_sdf_num = 400;
	itv->idx_sdf_mask = 7;

	itv->dec_vbi_pkt = 0;

	itv->dec_options.hide_last_frame = 1;
	itv->dec_options.pts_low = 0;
	itv->dec_options.pts_hi = 0;
	itv->dec_options.gop_offset = 0;
	itv->dec_options.mute_frames = 1;

	/* Ctrls */
	itv->dec_options.speed.mute = 1;
	itv->dec_options.speed.aud_mute = 0;
	itv->dec_options.speed.smooth = 1;
	itv->dec_options.speed.fr_mask = 2;
	itv->dec_options.speed.fr_field = 1;
	itv->dec_options.decbuffers = 1;
	itv->dec_options.prebuffer = 1;
	itv->dec_options.no_stop = 0;
	itv->dec_options.fast_stop = 1;

	/* OSD */
	itv->global_alpha = 255;
	itv->global_alpha_state = 1;
	itv->local_alpha_state = 0;

	// vbi
	itv->vbi_in.raw = 1;
	itv->vbi_service_set_out = 0;
	itv->vbi_frame = 0;
	itv->vbi_inserted_frame = 0;
	itv->vbi_passthrough = 0;
	itv->vbi_insert_mpeg = 0;

	/* Timestamp */
	memset(&itv->dec_timestamp, 0, sizeof(struct ivtv_ioctl_framesync));

	init_timer(&itv->vbi_passthrough_timer);
	itv->vbi_passthrough_timer.function = ivtv_set_vbi;
	itv->vbi_passthrough_timer.data = (unsigned long)itv;

	itv->msp34xx_audio_output = 1;	/* SCART1 output for msp34xx based cards */
	itv->audio_input_tv = 0;	/* TV tuner */
	itv->audmode_tv = itv->audmode_radio = V4L2_TUNER_MODE_STEREO;
 	// yuv
        atomic_set(&itv->yuv_info.next_dma_frame,-1);
        atomic_set(&itv->yuv_info.next_fill_frame,0);
 	itv->yuv_info.lace_mode = ivtv_yuv_mode;
 	itv->yuv_info.lace_threshold = ivtv_yuv_threshold;

        itv->lastVsyncFrame = 0;
	return 0;
}

// Second initialization part. Here the card type has been
// autodetected.
static void __devinit ivtv_init_struct2(struct ivtv *itv)
{
	const struct v4l2_input *input;
        int i;

	itv->audio_input_radio = 2;	/* Radio tuner */

        /* 0x00EF = saa7114(239) 0x00F0 = saa7115(240) 0x0106 = micro */
        switch (itv->card->type) {
        case IVTV_CARD_PVR_350:
                /* PVR 350s always have radio */
                itv->options.radio = 1;
                itv->digitizer = 0xF1;
                break;
        case IVTV_CARD_MPG160:
        case IVTV_CARD_MPG600:
        case IVTV_CARD_M179:
                /* saa7114 (see API Documention on ivtv website) */
                itv->digitizer = 0xEF;
                break;
        case IVTV_CARD_PVR_250:
                itv->digitizer = 0xF1;
                break;
        case IVTV_CARD_PG600:
        case IVTV_CARD_PVR_150:
                itv->digitizer = 0x140;
		itv->audio_input_radio = 4;	/* Radio tuner */
                break;
        default:
                /* saa7115 */
                itv->digitizer = 0xF1;
                break;
        }

        if (itv->card->type == IVTV_CARD_PVR_150 ||
            itv->card->type == IVTV_CARD_PG600) {
	        itv->vbi_sliced_size = 288;  // multiple of 16, real size = 284
        } else {
	        itv->vbi_sliced_size = 64;   // multiple of 16, real size = 52
        }

	/* Find tuner input */
	for (i = 0; (input = ivtv_get_input(itv, i)); i++) {
		if (input->type == V4L2_INPUT_TYPE_TUNER)
			break;
	}
	if (input == NULL)
		i = 0;
	itv->active_input = i;
}

static int ivtv_setup_pci(struct ivtv *itv, struct pci_dev *dev,
			  const struct pci_device_id *pci_id)
{
	u16 cmd;
	unsigned char pci_latency;

	IVTV_DEBUG_INFO("Enabling pci device\n");

	if (pci_enable_device(dev)) {
		IVTV_ERR("Can't enable device %d!\n", itv->num);
		return -EIO;
	}
	if (pci_set_dma_mask(dev, 0xffffffff)) {
		IVTV_ERR("No suitable DMA available on card %d.\n",
			      itv->num);
		return -EIO;
	}
	snprintf(itv->name, sizeof(itv->name) - 1, "ivtv%d", itv->num);
	if (!request_mem_region(pci_resource_start(dev, 0),
				IVTV_IOREMAP_SIZE, itv->name)) {
		IVTV_ERR("Cannot request memory region on card %d.\n",
			      itv->num);
		return -EIO;
	}

	/* Check for bus mastering */
	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	if (!(cmd & PCI_COMMAND_MASTER)) {
		IVTV_DEBUG_INFO("Attempting to enable Bus Mastering\n");
		pci_set_master(dev);
		pci_read_config_word(dev, PCI_COMMAND, &cmd);
		if (!(cmd & PCI_COMMAND_MASTER)) {
			IVTV_ERR("Bus Mastering is not enabled\n");
			return -ENXIO;
		}
	} 
	IVTV_DEBUG_INFO("Bus Mastering Enabled.\n");

	pci_read_config_byte(dev, PCI_CLASS_REVISION, &itv->card_rev);
	pci_read_config_byte(dev, PCI_LATENCY_TIMER, &pci_latency);

	if (pci_latency < 64) {
		IVTV_INFO("Unreasonably low latency timer, "
			       "setting to 64 (was %d)\n", pci_latency);
		pci_write_config_byte(dev, PCI_LATENCY_TIMER, 64);
		pci_read_config_byte(dev, PCI_LATENCY_TIMER, &pci_latency);
	}
	pci_write_config_dword(dev, 0x40, 0xffff);

	IVTV_DEBUG_INFO("%d (rev %d) at %02x:%02x.%x, "
		   "irq: %d, latency: %d, memory: 0x%lx\n",
		   itv->dev->device, itv->card_rev, dev->bus->number,
		   PCI_SLOT(dev->devfn), PCI_FUNC(dev->devfn),
		   itv->dev->irq, pci_latency, (unsigned long)itv->base_addr);

	return 0;
}

static inline void ivtv_request_module(struct ivtv *itv, const char *name)
{
	if (request_module(name) != 0) {
		IVTV_ERR("Failed to load module %s\n", name);
	} else {
		IVTV_DEBUG_INFO("Loaded module %s\n", name);
	}
}

static int __devinit ivtv_probe(struct pci_dev *dev,
				const struct pci_device_id *pci_id)
{
	int retval = 0;
	struct ivtv *itv;
        struct v4l2_frequency vf;
	int norm;

	spin_lock(&ivtv_cards_lock);

	/* Make sure we've got a place for this card */
	if (ivtv_cards_active == IVTV_MAX_CARDS) {
		printk(KERN_ERR "ivtv:  Maximum number of cards detected (%d).\n",
			      ivtv_cards_active);
		spin_unlock(&ivtv_cards_lock);
		return -ENOMEM;
	}

	itv = kmalloc(sizeof(struct ivtv), GFP_ATOMIC);
	if (itv == 0) {
		spin_unlock(&ivtv_cards_lock);
		return -ENOMEM;
        }
	ivtv_cards[ivtv_cards_active] = itv;
	memset(itv, 0, sizeof(*itv));
	itv->dev = dev;
	itv->num = ivtv_cards_active++;
        if (itv->num) {
                printk(KERN_INFO "ivtv:  ======================  NEXT CARD  ======================\n");
        }

	spin_unlock(&ivtv_cards_lock);

	ivtv_process_options(itv);
	if (ivtv_init_struct1(itv)) {
		retval = -ENOMEM;
		goto err;
	}

	init_waitqueue_head(&itv->w_intr_wq);
	init_waitqueue_head(&itv->r_intr_wq);

	IVTV_DEBUG_INFO("base addr: 0x%08x\n", itv->base_addr);

	/* PCI Device Setup */
	if ((retval = ivtv_setup_pci(itv, dev, pci_id)) != 0) {
		if (retval == -EIO)
			goto free_workqueue;
		else if (retval == -ENXIO)
			goto free_mem;
	}
	/* save itv in the pci struct for later use */
	pci_set_drvdata(dev, itv);

	/* map io memory */
	IVTV_DEBUG_INFO("attempting ioremap at 0x%08x len 0x%08x\n",
		   itv->base_addr + IVTV_ENCODER_OFFSET, IVTV_ENCODER_SIZE);
	itv->enc_mem = ioremap_nocache(itv->base_addr + IVTV_ENCODER_OFFSET,
				       IVTV_ENCODER_SIZE);
	if (!itv->enc_mem) {
		IVTV_ERR("ioremap failed, perhaps increasing "
		         "__VMALLOC_RESERVE in page.h\n");
		IVTV_ERR("or disabling CONFIG_HIMEM4G "
		         "into the kernel would help\n");
		retval = -ENOMEM;
		goto free_mem;
	}

	IVTV_DEBUG_INFO("attempting ioremap at 0x%08x len 0x%08x\n",
		   itv->base_addr + IVTV_DECODER_OFFSET, IVTV_DECODER_SIZE);
	itv->dec_mem = ioremap_nocache(itv->base_addr + IVTV_DECODER_OFFSET,
				       IVTV_DECODER_SIZE);
	if (!itv->dec_mem) {
		IVTV_ERR("ioremap failed, perhaps increasing "
		         "__VMALLOC_RESERVE in page.h\n");
		IVTV_ERR("or disabling CONFIG_HIMEM4G "
		         "into the kernel would help\n");
		retval = -ENOMEM;
		goto free_mem;
	}

	/* map registers memory */
	IVTV_DEBUG_INFO(
		   "attempting ioremap at 0x%08x len 0x%08x\n",
		   itv->base_addr + IVTV_REG_OFFSET, IVTV_REG_SIZE);
	itv->reg_mem =
	    ioremap_nocache(itv->base_addr + IVTV_REG_OFFSET, IVTV_REG_SIZE);
	if (!itv->reg_mem) {
		IVTV_ERR("ioremap failed, perhaps increasing "
		         "__VMALLOC_RESERVE in page.h\n");
		IVTV_ERR("or disabling CONFIG_HIMEM4G "
		         "into the kernel would help\n");
		retval = -ENOMEM;
		goto free_io;
	}

	/* active i2c  */
	IVTV_DEBUG_INFO("activating i2c...\n");
	if (init_ivtv_i2c(itv)) {
		IVTV_ERR("Could not initialize i2c\n");
		goto free_irq;
	}

	IVTV_DEBUG_INFO(
		   "Active card count: %d.\n", ivtv_cards_active);

	switch (itv->card->type) {
        case IVTV_CARD_M179:
                if (itv->options.tuner == -1) {
                        /* As far as I know all M179 cards use this tuner */
                        itv->options.tuner = TUNER_PHILIPS_NTSC;
                }
                break;

        case IVTV_CARD_AVC2410:
                if (itv->options.tuner == -1) {
			// Setup tuner for this card based on the selected
			// standard. This card has no eeprom and in fact the
			// Windows driver relies on the country/region setting
			// of the user.
			if (itv->std <= 0 || itv->std == V4L2_STD_NTSC) {
				// This is a good guess, but I'm not totally
				// sure this is the correct tuner.
				itv->options.tuner = TUNER_PHILIPS_FM1236_MK3;
			} else {
				// This has been verified.
				itv->options.tuner = TUNER_PHILIPS_FM1216ME_MK3;
			}
                }
                break;

	/* Hauppauge models */
	case IVTV_CARD_PVR_150:
	case IVTV_CARD_PVR_250:
	case IVTV_CARD_PVR_350:
#ifndef CONFIG_VIDEO_TVEEPROM
		if (autoload)
			ivtv_request_module(itv, "tveeprom");
#endif /* CONFIG_VIDEO_TVEEPROM */
                itv->hw_flags |= IVTV_HW_TVEEPROM;
                // Based on the model number the cardtype may be changed.
                // The PCI IDs are not always reliable.
		ivtv_read_eeprom(itv);
		break;
	default:
		break;
	}

        // The card is now fully identified, continue with card-specific
        // initialization.
	ivtv_init_struct2(itv);

	if (autoload) {
#ifndef CONFIG_VIDEO_TUNER
		ivtv_request_module(itv, "tuner");
#endif /* CONFIG_VIDEO_TUNER */
		if (itv->card->type == IVTV_CARD_PVR_150 ||
		    itv->card->type == IVTV_CARD_PG600) {
                        int cardtype = (itv->card->type == IVTV_CARD_PG600);

			ivtv_request_module(itv, "cx25840");
		        itv->card->video_dec_func(itv, DECODER_SET_CARD_TYPE, &cardtype);
                        itv->has_cx25840 = 1;
                        itv->hw_flags |= IVTV_HW_CX25840;
                        itv->vbi_raw_decoder_line_size = 1444;
                        itv->vbi_raw_decoder_sav_odd_field = 0x20;
                        itv->vbi_raw_decoder_sav_even_field = 0x60;
                        itv->vbi_sliced_decoder_line_size = 284;
                        itv->vbi_sliced_decoder_sav_odd_field = 0xB0;
                        itv->vbi_sliced_decoder_sav_even_field = 0xF0;
			if (itv->card->type == IVTV_CARD_PVR_150) {
				ivtv_request_module(itv, "wm8775");
                                itv->hw_flags |= IVTV_HW_WM8775;
			}
		} else {
			ivtv_request_module(itv, "saa7115");
                        itv->has_saa7115 = 1;
                        itv->hw_flags |= IVTV_HW_SAA7115;
                        itv->vbi_raw_decoder_line_size = 1443;
                        itv->vbi_raw_decoder_sav_odd_field = 0x25;
                        itv->vbi_raw_decoder_sav_even_field = 0x62;
                        itv->vbi_sliced_decoder_line_size = 51;
                        itv->vbi_sliced_decoder_sav_odd_field = 0xAB;
                        itv->vbi_sliced_decoder_sav_even_field = 0xEC;
                }
		if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) {
			ivtv_request_module(itv, "saa7127");
                        itv->hw_flags |= IVTV_HW_SAA7127;
		}
		if (itv->card->audio_selector == USE_MSP34XX) {
#ifndef CONFIG_VIDEO_BT848
			ivtv_request_module(itv, "msp3400");
#endif /* CONFIG_VIDEO_TUNER */
                        itv->hw_flags |= IVTV_HW_MSP34XX;
		}
                if (itv->card->audio_selector == USE_CS53L32A ||
                    itv->card->type == IVTV_CARD_AVC2410) {
			ivtv_request_module(itv, "cs53l32a");
                        itv->hw_flags |= IVTV_HW_CS53132A;
		}
                // -2 == autodetect, -1 == no tda9887
		if (itv->options.tda9887 != -1) {
#ifndef CONFIG_VIDEO_TUNER
			ivtv_request_module(itv, "tda9887");
#endif /* CONFIG_VIDEO_TUNER */
		}
                /* when tda9887 is loaded and detects the chip it will
                   call attach_inform() in ivtv-i2c.c which will set the
                   itv->has_tda9887 variable to 1. */
                if (itv->has_tda9887) {
                        itv->hw_flags |= IVTV_HW_TDA9887;
                }
                if (itv->options.tda9887 == -2 && itv->has_tda9887)
                        itv->options.tda9887 = 0;
	}

	if (itv->std <= 0) {
		IVTV_ERR("Could not detect tuner standard, defaulting to NTSC.\n");
		itv->std = V4L2_STD_NTSC;
		ivtv_std = 1;
	}

        if (itv->std & V4L2_STD_525_60) {
                itv->is_60hz = 1;
                if (itv->has_cx25840) {
                        // NTSC does not yet have a working sliced VBI
                        // implementation for the cx25840.
                        itv->v4l2_cap &= ~V4L2_CAP_SLICED_VBI_CAPTURE;
                }
        } else {
                itv->is_50hz = 1;
        }

        /* Setup YUV Encoding/Decoding Buffers */
        if (itv->is_60hz) {
                itv->dma_cfg.enc_yuv_buf_size = IVTV_DMA_ENC_YUV_BUF_SIZE;
                itv->dma_cfg.dec_yuv_buf_size = IVTV_DMA_DEC_YUV_BUF_SIZE;
        } else {
                itv->dma_cfg.enc_yuv_buf_size = 0x19500;
                itv->dma_cfg.dec_yuv_buf_size = 0x19500;
        }
	itv->dma_cfg.enc_yuv_buf_size /= 8;
	itv->dma_cfg.dec_yuv_buf_size /= 2;

        /* Setup VBI Raw Size. Should be big enough to hold PAL.
           It is possible to switch between PAL and NTSC, so we need to
           take the largest size here. */
        // 1456 is multiple of 16, real size = 1444
        itv->vbi_raw_size = 1456;
        // We use a buffer size of 1/2 of the total size needed for a
        // frame. This is actually very useful, since we now receive
        // a field at a time and that makes 'compressing' the raw data
        // down to size by stripping off the SAV codes a lot easier.
        if (itv->is_60hz) {
                // Note: having two different buffer sizes prevents standard
                // switching on the fly. We need to find a better solution...
                itv->dma_cfg.vbi_buf_size = itv->vbi_raw_size * 24 / 2;
        } else {
                itv->dma_cfg.vbi_buf_size = itv->vbi_raw_size * 36 / 2;
        }

        if (itv->i2c_radio_tuner_addr) {
                IVTV_INFO("Detected a TEA5767 radio tuner. Enabling radio support.\n");
                itv->options.radio = 1;
        }
	if (itv->options.radio == -1)
		itv->options.radio = 0;
	if (itv->options.radio)
		itv->v4l2_cap |= V4L2_CAP_RADIO;

	/* write firmware */
	retval = ivtv_firmware_init(itv);
	if (retval) {
		IVTV_ERR("Error %d initializing firmware.\n", retval);
		retval = -ENOMEM;
		goto free_i2c;
	}

	/* search for encoder/decoder mailboxes */
	IVTV_DEBUG_INFO("About to search for mailboxes\n");
	ivtv_find_firmware_mailbox(itv);

	if ((itv->enc_mbox == NULL) && (itv->dec_mbox == NULL)) {
		IVTV_ERR("Error locating firmware.\n");
		retval = -ENOMEM;
		goto free_i2c;
	}

	/* Try and get firmware versions */
	IVTV_DEBUG_INFO("Getting firmware version..\n");
	retval = ivtv_firmware_versions(itv);
	if (retval) {
		IVTV_ERR("Error %d getting firmware version!\n", retval);
		goto free_i2c;
	}
        /* Always use frame based mode. Experiments have demonstrated that byte
           stream based mode results in dropped frames and corruption. Not often,
           but occasionally. Many thanks go to Leonard Orb who spent a lot of
           effort and time trying to trace the cause of the drop outs. */
	itv->dma_cfg.enc_buf_size = 0x8000; /* 32k */
	itv->dma_cfg.fw_enc_dma_xfer = 1;   /* frame based */
	itv->dma_cfg.fw_enc_dma_type = 1;

	retval = ivtv_streams_setup(itv);
	if (retval) {
		IVTV_ERR("Error %d setting up streams\n", retval);
		goto free_i2c;
	}

	/* Start Threads */
	IVTV_DEBUG_INFO("Starting Threads\n");
	atomic_set(&itv->enc_thread_running, 1);
	itv->enc_thread_pid = kernel_thread(ivtv_enc_thread, itv,
					    CLONE_FS | CLONE_FILES);
	atomic_set(&itv->enc_vbi_thread_running, 1);
	itv->enc_vbi_thread_pid = kernel_thread(ivtv_enc_vbi_thread, itv,
					    CLONE_FS | CLONE_FILES);

	/* Decoder Thread */
	if ((itv->card->type == IVTV_CARD_PVR_350)) {
            	ivtv_init_mpeg_decoder(itv);
		atomic_set(&itv->dec_thread_running, 1);
		itv->dec_thread_pid = kernel_thread(ivtv_dec_thread, itv,
						    CLONE_FS | CLONE_FILES);
		atomic_set(&itv->dec_vbi_thread_running, 1);
		itv->dec_vbi_thread_pid = kernel_thread(ivtv_dec_vbi_thread, itv,
						    CLONE_FS | CLONE_FILES);
	}

	IVTV_DEBUG_IRQ("Masking interrupts\n");
	/* clear interrupt mask, effectively disabling interrupts */
	ivtv_set_irq_mask(itv, 0xffffffff);

	/* Register IRQ */
	snprintf(itv->name, sizeof(itv->name) - 1, "ivtv%d", itv->num);
	retval = request_irq(itv->dev->irq, ivtv_irq_handler,
			     SA_SHIRQ | SA_INTERRUPT, itv->name, (void *)itv);
	if (retval) {
		IVTV_ERR("Failed to register irq %d\n", retval);
		goto free_i2c;
	}

	/* On an itvc16 this seems to be able to enable DMA to the chip? */
	if (!itv->has_itvc15)
		ivtv_write_reg(0x03, itv->reg_mem + 0x10);

	/* Default interrupts enabled. For the PVR350 this includes the
           decoder VSYNC interrupt, which is always on. It is not only used
           during decoding but also by the OSD.
           Some old PVR250 cards had an iTVC15, so testing for that is too
           general. Instead test if the card has video output capability. */
	if (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT) 
		ivtv_clear_irq_mask(itv,
				    IVTV_IRQ_MASK_INIT | IVTV_IRQ_DEC_VSYNC);
	else
		ivtv_clear_irq_mask(itv, IVTV_IRQ_MASK_INIT);

	if (itv->options.tuner > -1) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
                {
                        struct tuner_setup setup;

                        setup.addr = ADDR_UNSET;
                        setup.type = itv->options.tuner;
                        setup.mode_mask = T_ANALOG_TV;
                        if (itv->options.radio && itv->i2c_radio_tuner_addr == 0)
                                setup.mode_mask |= T_RADIO;
        		ivtv_tv_tuner(itv, TUNER_SET_TYPE_ADDR, &setup);
                }
#else
		ivtv_tv_tuner(itv, TUNER_SET_TYPE, &(itv->options.tuner));
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13) */
	}

        vf.tuner = 0;
        vf.frequency = 6400; /* the tuner 'baseline' frequency */
	if (itv->std & V4L2_STD_NTSC) {
		/* Why on earth? */
		vf.frequency = 1076;	/* ch. 4 67250*16/1000 */
	}

	/* The tuner is fixed to the norm. The other inputs (e.g. S-Video)
	   are not. */
	itv->tuner_std = itv->std;

	norm = std2norm(itv->std);

        /* Digitizer */
	itv->card->video_dec_func(itv, DECODER_SET_NORM, &norm);

	/* Change Input to active, 150 needs this, seems right */
	itv->card->video_dec_func(itv, DECODER_SET_INPUT, &itv->active_input);

	/* Init new audio input/output */
	ivtv_audio_set_io(itv);

	/* Let the VIDIOC_S_STD ioctl do all the work, keeps the code
	   in one place. */
	itv->std++;		// Force full standard initialization
	ivtv_v4l2_ioctls(itv, NULL, 0, VIDIOC_S_STD, &itv->tuner_std);
        if (itv->options.tuner == TUNER_PHILIPS_FM1216ME_MK3) {
                int cfg = TDA9887_PORT1_ACTIVE|TDA9887_PORT2_ACTIVE;
                ivtv_tda9887(itv, TDA9887_SET_CONFIG, &cfg);
        }
	ivtv_v4l2_ioctls(itv, NULL, 0, VIDIOC_S_FREQUENCY, &vf);

	IVTV_INFO("Initialized %s, card #%d\n", itv->card_name, itv->num);

	return 0;

      free_irq:
	free_irq(itv->dev->irq, (void *)itv);
      free_i2c:
	exit_ivtv_i2c(itv);
      free_io:
	ivtv_iounmap(itv);
      free_mem:
	release_mem_region(pci_resource_start(itv->dev, 0), IVTV_IOREMAP_SIZE);
      free_workqueue:
#ifdef LINUX26
	destroy_workqueue(itv->vbi_work_queues);
	destroy_workqueue(itv->fw_work_queues);
#endif /* LINUX26 */
      err:
	IVTV_ERR("Error %d on initialization\n", retval);

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

	IVTV_DEBUG_INFO("Removing Card #%d.\n", itv->num);

	/* Stop all captures */
	IVTV_DEBUG_INFO(" Stopping all streams.\n");
	itv->end_gop = 0;
	if (atomic_read(&itv->capturing) > 0)
		ivtv_stop_all_captures(itv);

	/* Stop all decoding */
	IVTV_DEBUG_INFO(" Stopping decoding.\n");
	if ((itv->card->type == IVTV_CARD_PVR_350)
	    && atomic_read(&itv->decoding) > 0) {
		ivtv_stop_v4l2_decode_stream(itv, IVTV_DEC_STREAM_TYPE_MPG);
	}

	/* Interrupts */
	IVTV_DEBUG_INFO(" Disabling interrupts.\n");
	ivtv_set_irq_mask(itv, 0xffffffff);

	/* Stop all Work Queues */
	IVTV_DEBUG_INFO(" Flushing Work Queues.\n");
	ivtv_flush_scheduled_work(itv);
#ifdef LINUX26
	IVTV_DEBUG_INFO(" Cancel Work Queues.\n");
	cancel_delayed_work(&itv->fw_work_queue);
	cancel_delayed_work(&itv->vbi_work_queue);

	IVTV_DEBUG_INFO(" Destroy Work Queues.\n");
	destroy_workqueue(itv->vbi_work_queues);
	destroy_workqueue(itv->fw_work_queues);
#endif /* LINUX26 */
	IVTV_DEBUG_INFO(" Stopping Threads.\n");
	/* Stop all threads */
	atomic_set(&itv->enc_vbi_thread_running, 0);
	atomic_set(&itv->enc_vbi_dma_stat_intr, 1);

	atomic_set(&itv->enc_thread_running, 0);
	atomic_set(&itv->enc_dma_stat_intr, 1);
	itv->t_flags = 0;

	wake_up(&itv->enc_vbi_thread_w);
	wake_up(&itv->enc_thread_w);

	if (itv->enc_thread_pid)
		kill_proc(itv->enc_thread_pid, SIGKILL, 1);

	if (itv->enc_vbi_thread_pid)
		kill_proc(itv->enc_vbi_thread_pid, SIGKILL, 1);

	if (itv->card->type == IVTV_CARD_PVR_350) {
		atomic_set(&itv->dec_vbi_thread_running, 0);
		atomic_set(&itv->dec_dma_stat.vbi_intr, 1);
		wake_up(&itv->dec_vbi_thread_w);

		atomic_set(&itv->dec_thread_running, 0);
		atomic_set(&itv->dec_dma_stat.intr, 1);
		wake_up(&itv->dec_thread_w);

		if (itv->dec_thread_pid)
			kill_proc(itv->dec_thread_pid, SIGKILL, 1);

		if (itv->dec_vbi_thread_pid)
			kill_proc(itv->dec_vbi_thread_pid, SIGKILL, 1);

	}

	IVTV_DEBUG_INFO(" Stopping Passthrough.\n");
	clear_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags);
	del_timer_sync(&itv->vbi_passthrough_timer);

	IVTV_DEBUG_INFO(" Stopping Firmware.\n");
	ivtv_halt_firmware(itv, 3);

	IVTV_DEBUG_INFO(" Freeing dma resources.\n");
	IVTV_DEBUG_INFO(" Unregistering v4l devices.\n");
	ivtv_streams_cleanup(itv);

	exit_ivtv_i2c(itv);

	IVTV_DEBUG_INFO(" Releasing irq.\n");
	free_irq(itv->dev->irq, (void *)itv);

	if (itv->dev) {
		ivtv_iounmap(itv);
	}

	IVTV_DEBUG_INFO(" Releasing mem.\n");
	release_mem_region(pci_resource_start(itv->dev, 0), IVTV_IOREMAP_SIZE);

	pci_disable_device(itv->dev);

	IVTV_INFO("Removed %s, card #%d\n", itv->card_name, itv->num);
}

/* define a pci_driver for card detection */
static struct pci_driver ivtv_pci_driver = {
      name:"ivtv-iTVC15_16_mpg2_encoder_card",
      id_table:ivtv_pci_tbl,
      probe:ivtv_probe,
      remove:ivtv_remove,
};

static int module_start(void)
{
	printk(KERN_INFO "ivtv:  ==================== START INIT IVTV ====================\n");
	printk(KERN_INFO "ivtv:  version %s %s loading\n",
		       IVTV_VERSION_STRING(IVTV_VERSION_INFO_NAME),
		       IVTV_VERSION_COMMENT(IVTV_VERSION_INFO_NAME));
#ifdef LINUX26
	printk(KERN_INFO "ivtv:  Linux version: " VERMAGIC_STRING "\n");
#else
	printk(KERN_INFO "ivtv:  Linux version: " UTS_RELEASE "\n");
#endif /* LINUX26 */
	printk(KERN_INFO "ivtv:  In case of problems please include the debug info\n");
	printk(KERN_INFO "ivtv:  between the START INIT IVTV and END INIT IVTV lines when\n");
	printk(KERN_INFO "ivtv:  mailing the ivtv-devel mailinglist.\n");

	memset(ivtv_cards, 0, sizeof(ivtv_cards));

	/* Validate parameters */
	if (ivtv_first_minor < 0 || ivtv_first_minor >= IVTV_MAX_CARDS) {
		printk(KERN_ERR "ivtv:  ivtv_first_minor must be between 0 and %d. Exiting...\n",
		     IVTV_MAX_CARDS - 1);
		return -1;
	}
	if ((yuv_fixup != 0) && (yuv_fixup != 1)) {
		printk(KERN_ERR "ivtv:  yuv_fixup must be 0 or 1. Exiting...\n");
		return -1;
	}

	if (ivtv_debug < 0 || ivtv_debug > 511) {
		ivtv_debug = 1;
		printk(KERN_INFO "ivtv:  debug value must be >= 0 and <= 511!\n");
	}

	if (pci_module_init(&ivtv_pci_driver)) {
		printk(KERN_ERR "ivtv:  Error detecting PCI card\n");
		return -ENODEV;
	}
	printk(KERN_INFO "ivtv:  ====================  END INIT IVTV  ====================\n");
	return 0;
}

static void module_cleanup(void)
{
        int i, j;

        for (i = 0; i < ivtv_cards_active; i++) {
                for (j = 0; j < IVTV_VBI_FRAMES; j++) {
                        kfree(ivtv_cards[i]->vbi_sliced_mpeg_data[j]);
                }
                kfree(ivtv_cards[i]);
        }
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
EXPORT_SYMBOL(ivtv_reset_ir_gpio);
#endif /* LINUX26 */
module_init(module_start);
module_exit(module_cleanup);
