/* 
 * tveeprom - eeprom decoder for tvcard configuration eeproms
 *
 * Data and decoding routines shamelessly borrowed from bttv-cards.c
 * eeprom access routine shamelessly borrowed from bttv-if.c
 * which are:

    Copyright (C) 1996,97,98 Ralph  Metzler (rjkm@thp.uni-koeln.de)
                           & Marcus Metzler (mocm@thp.uni-koeln.de)
    (c) 1999-2001 Gerd Knorr <kraxel@goldbach.in-berlin.de>

 * Adjustments to fit a more general model and all bugs:
 
 	Copyright (C) 2003 John Klar <linpvr at projectplasma.com>

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Turning this on causes this module to compile against the struct
   tveeprom from the kernel, which should make the function's
   prototype match the kernel version and hopefully, finally put an
   end to the problems happening when CONFIG_MODVERSIONS is enabled in
   the kernel. */
#define USE_TVEEPROM_H

#include <linux/version.h>

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/init.h>

#include <linux/slab.h>

#include <linux/mm.h>
#include <linux/pci.h>
#include <linux/signal.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <linux/sched.h>
#include <linux/types.h>

#include "compat.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#else
#include <linux/kmod.h>
#include <linux/init.h>
#endif

#ifdef USE_TVEEPROM_H
#include <linux/i2c.h>
#include <media/tveeprom.h>
#endif

#include <linux/version.h>
#include <asm/uaccess.h>

MODULE_DESCRIPTION("i2c Hauppauge eeprom decoder driver");
MODULE_AUTHOR("John Klar");
MODULE_LICENSE("GPL");

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "tuner.h"

#ifndef I2C_DRIVERID_TVEEPROM
// Using temporary hack for missing I2C driver-ID for tveeprom
#define I2C_DRIVERID_TVEEPROM I2C_DRIVERID_EXP2
#endif

#define   I2C_TVEEPROM        0xA0

static int debug = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(debug, int, 0644);
#else
MODULE_PARM(debug, "i");
#endif
MODULE_PARM_DESC(debug, "Debug level (0-2)");

#define STRM(array,i) (i < sizeof(array)/sizeof(char*) ? array[i] : "unknown")

#define dprintk(num, args...) \
	do { \
		if (debug >= num) \
			printk(KERN_INFO "tveeprom: " args); \
	} while (0)

#define TVEEPROM_KERN_ERR(args...) printk(KERN_ERR "tveeprom: " args);
#define TVEEPROM_KERN_INFO(args...) printk(KERN_INFO "tveeprom: " args);

/* ----------------------------------------------------------------------- */

static unsigned char eeprom_buf[256];

#ifndef USE_TVEEPROM_H
struct tveeprom {
	u32 has_radio;

	u32 tuner_type;
	u32 tuner_formats;

	u32 digitizer;
	u32 digitizer_formats;

	u32 audio_processor;
	u32 decoder_processor;
	/* a_p_fmts? */

	u32 model;
	u32 revision;
	u32 serial_number;
	char rev_str[5];
};
#endif

/* ----------------------------------------------------------------------- */
/* some hauppauge specific stuff                                           */

static struct HAUPPAUGE_TUNER_FMT {
	int id;
	char *name;
} hauppauge_tuner_fmt[] = {
	{
	0x00000000, "unknown1 "}, {
	0x00000000, "unknown2 "}, {
	0x00000007, "PAL(B/G) "}, {
	0x00001000, "NTSC(M) "}, {
	0x00000010, "PAL(I) "}, {
	0x00400000, "SECAM(L/L') "}, {
	0x00000e00, "PAL(D/K) "}, {
	0x03000000, "ATSC Digital "},};

/* This is the full list of possible tuners. Many thanks to Hauppauge for
   supplying this information. Note that many tuners where only used for
   testing and never made it to the outside world. So you will only see
   a subset in actual produced cards. */
static struct HAUPPAUGE_TUNER {
	int id;
	char *name;
} hauppauge_tuner[] = {
	/* 0-9 */
	{ TUNER_ABSENT,        "None" },
	{ TUNER_ABSENT,        "External" },
	{ TUNER_ABSENT,        "Unspecified" },
	{ TUNER_PHILIPS_PAL,   "Philips FI1216" },
	{ TUNER_PHILIPS_SECAM, "Philips FI1216MF" },
	{ TUNER_PHILIPS_NTSC,  "Philips FI1236" },
	{ TUNER_PHILIPS_PAL_I, "Philips FI1246" },
	{ TUNER_PHILIPS_PAL_DK,"Philips FI1256" },
	{ TUNER_PHILIPS_PAL,   "Philips FI1216 MK2" },
	{ TUNER_PHILIPS_SECAM, "Philips FI1216MF MK2" },
	/* 10-19 */
	{ TUNER_PHILIPS_NTSC,  "Philips FI1236 MK2" },
	{ TUNER_PHILIPS_PAL_I, "Philips FI1246 MK2" },
	{ TUNER_PHILIPS_PAL_DK,"Philips FI1256 MK2" },
	{ TUNER_TEMIC_NTSC,    "Temic 4032FY5" },
	{ TUNER_TEMIC_PAL,     "Temic 4002FH5" },
	{ TUNER_TEMIC_PAL_I,   "Temic 4062FY5" },
	{ TUNER_PHILIPS_PAL,   "Philips FR1216 MK2" },
	{ TUNER_PHILIPS_SECAM, "Philips FR1216MF MK2" },
	{ TUNER_PHILIPS_NTSC,  "Philips FR1236 MK2" },
	{ TUNER_PHILIPS_PAL_I, "Philips FR1246 MK2" },
	/* 20-29 */
	{ TUNER_PHILIPS_PAL_DK,"Philips FR1256 MK2" },
	{ TUNER_PHILIPS_PAL,   "Philips FM1216" },
	{ TUNER_PHILIPS_SECAM, "Philips FM1216MF" },
	{ TUNER_PHILIPS_NTSC,  "Philips FM1236" },
	{ TUNER_PHILIPS_PAL_I, "Philips FM1246" },
	{ TUNER_PHILIPS_PAL_DK,"Philips FM1256" },
	{ TUNER_TEMIC_4036FY5_NTSC, "Temic 4036FY5" },
	{ TUNER_ABSENT,        "Samsung TCPN9082D" },
	{ TUNER_ABSENT,        "Samsung TCPM9092P" },
	{ TUNER_TEMIC_4006FH5_PAL, "Temic 4006FH5" },
	/* 30-39 */
	{ TUNER_ABSENT,        "Samsung TCPN9085D" },
	{ TUNER_ABSENT,        "Samsung TCPB9085P" },
	{ TUNER_ABSENT,        "Samsung TCPL9091P" },
	{ TUNER_TEMIC_4039FR5_NTSC, "Temic 4039FR5" },
	{ TUNER_PHILIPS_FQ1216ME,   "Philips FQ1216 ME" },
	{ TUNER_TEMIC_4066FY5_PAL_I, "Temic 4066FY5" },
        { TUNER_PHILIPS_NTSC,        "Philips TD1536" },
        { TUNER_PHILIPS_NTSC,        "Philips TD1536D" },
	{ TUNER_PHILIPS_NTSC,  "Philips FMR1236" }, /* mono radio */
	{ TUNER_ABSENT,        "Philips FI1256MP" },
	/* 40-49 */
	{ TUNER_ABSENT,        "Samsung TCPQ9091P" },
	{ TUNER_TEMIC_4006FN5_MULTI_PAL, "Temic 4006FN5" },
	{ TUNER_TEMIC_4009FR5_PAL, "Temic 4009FR5" },
	{ TUNER_TEMIC_4046FM5,     "Temic 4046FM5" },
	{ TUNER_TEMIC_4009FN5_MULTI_PAL_FM, "Temic 4009FN5" },
	{ TUNER_ABSENT,        "Philips TD1536D FH 44"},
	{ TUNER_LG_NTSC_FM,    "LG TP18NSR01F"},
	{ TUNER_LG_PAL_FM,     "LG TP18PSB01D"},
	{ TUNER_LG_PAL,        "LG TP18PSB11D"},
	{ TUNER_LG_PAL_I_FM,   "LG TAPC-I001D"},
	/* 50-59 */
	{ TUNER_LG_PAL_I,      "LG TAPC-I701D"},
	{ TUNER_ABSENT,        "Temic 4042FI5"},
	{ TUNER_MICROTUNE_4049FM5, "Microtune 4049 FM5"},
	{ TUNER_ABSENT,        "LG TPI8NSR11F"},
	{ TUNER_ABSENT,        "Microtune 4049 FM5 Alt I2C"},
	{ TUNER_PHILIPS_FM1216ME_MK3, "Philips FQ1216ME MK3"},
	{ TUNER_ABSENT,        "Philips FI1236 MK3"},
	{ TUNER_PHILIPS_FM1216ME_MK3, "Philips FM1216 ME MK3"},
	{ TUNER_PHILIPS_FM1236_MK3, "Philips FM1236 MK3"},
	{ TUNER_ABSENT,        "Philips FM1216MP MK3"},
	/* 60-69 */
	{ TUNER_PHILIPS_FM1216ME_MK3, "LG S001D MK3"},
	{ TUNER_ABSENT,        "LG M001D MK3"},
	{ TUNER_ABSENT,        "LG S701D MK3"},
	{ TUNER_ABSENT,        "LG M701D MK3"},
	{ TUNER_ABSENT,        "Temic 4146FM5"},
	{ TUNER_ABSENT,        "Temic 4136FY5"},
	{ TUNER_ABSENT,        "Temic 4106FH5"},
	{ TUNER_ABSENT,        "Philips FQ1216LMP MK3"},
	{ TUNER_LG_NTSC_TAPE,  "LG TAPE H001F MK3"},
	{ TUNER_LG_NTSC_TAPE,  "LG TAPE H701F MK3"},
	/* 70-79 */
	{ TUNER_ABSENT,        "LG TALN H200T"},
	{ TUNER_ABSENT,        "LG TALN H250T"},
	{ TUNER_ABSENT,        "LG TALN M200T"},
	{ TUNER_ABSENT,        "LG TALN Z200T"},
	{ TUNER_ABSENT,        "LG TALN S200T"},
	{ TUNER_ABSENT,        "Thompson DTT7595"},
	{ TUNER_ABSENT,        "Thompson DTT7592"},
	{ TUNER_ABSENT,        "Silicon TDA8275C1 8290"},
	{ TUNER_ABSENT,        "Silicon TDA8275C1 8290 FM"},
	{ TUNER_ABSENT,        "Thompson DTT757"},
	/* 80-89 */
	{ TUNER_ABSENT,        "Philips FQ1216LME MK3"},
	{ TUNER_ABSENT,        "LG TAPC G701D"},
	{ TUNER_LG_NTSC_NEW_TAPC, "LG TAPC H791F"},
	{ TUNER_LG_PAL_NEW_TAPC, "TCL 2002MB 3"},
	{ TUNER_LG_PAL_NEW_TAPC, "TCL 2002MI 3"},
	{ TUNER_TCL_2002N,     "TCL 2002N 6A"},
	{ TUNER_ABSENT,        "Philips FQ1236 MK3"},
	{ TUNER_ABSENT,        "Samsung TCPN 2121P30A"},
	{ TUNER_ABSENT,        "Samsung TCPE 4121P30A"},
	{ TUNER_PHILIPS_FM1216ME_MK3, "TCL MFPE05 2"},
	/* 90-99 */
	{ TUNER_ABSENT,        "LG TALN H202T"},
	{ TUNER_PHILIPS_FQ1216AME_MK4, "Philips FQ1216AME MK4"},
	{ TUNER_PHILIPS_FQ1236A_MK4, "Philips FQ1236A MK4"},
	{ TUNER_ABSENT,        "Philips FQ1286A MK4"},
	{ TUNER_ABSENT,        "Philips FQ1216ME MK5"},
	{ TUNER_ABSENT,        "Philips FQ1236 MK5"},
	{ TUNER_ABSENT,        "Samsung TCPG_6121P30A"},
	{ TUNER_TCL_2002MB,    "TCL 2002MB_3H"},
        { TUNER_ABSENT,        "TCL 2002MI_3H"},
        { TUNER_TCL_2002N,     "TCL 2002N 5H"},
	/* 100-109 */
	{ TUNER_ABSENT,        "Philips FMD1216ME"},
        { TUNER_TEA5767,       "Philips TEA5768HL FM Radio"},
        { TUNER_ABSENT,        "Panasonic ENV57H12D5"},
        { TUNER_PHILIPS_FM1236_MK3, "TCL MFNM05-4"},
        { TUNER_ABSENT,        "TCL MNM05-4"},
        { TUNER_PHILIPS_FM1216ME_MK3, "TCL MPE05-2"},
        { TUNER_ABSENT,        "TCL MQNM05-4"},
        { TUNER_ABSENT,        "LG TAPC-W701D"},
        { TUNER_ABSENT,        "TCL 9886P-WM"},
        { TUNER_ABSENT,        "TCL 1676NM-WM"},
};

static const char *audioIC[] = {
        /* 0-9 */
        "None",
        "TEA6300",
        "TEA6320",
        "TDA9850",
        "MSP3400C",
        "MSP3410D",
        "MSP3415",
        "MSP3430",
        "MSP3438",
        "CS5331",
        /* 10-19 */
        "MSP3435",
        "MSP3440",
        "MSP3445",
        "MSP3411",
        "MSP3416",
        "MSP3425",
        "MSP3451",
        "MSP3418",
        "Type 0x12",
        "OKI7716",
        /* 20-29 */
        "MSP4410",
        "MSP4420",
        "MSP4440",
        "MSP4450",
        "MSP4408",
        "MSP4418",
        "MSP4428",
        "MSP4448",
        "MSP4458",
        "Type 0x1d",
        /* 30-39 */
        "CX880",
        "CX881",
        "CX883",
        "CX882",
        "CX25840",
        "CX25841",
        "CX25842",
        "CX25843",
        "CX23418",
};

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)) || !defined(USE_TVEEPROM_H)
static const char *decoderIC[] = {
        /* 0-9 */
        "Type 0x00",
        "BT815",
        "BT817",
        "BT819",
        "BT815A",
        "BT817A",
        "BT819A",
        "BT827",
        "BT829",
        "BT848",
        /* 10-19 */
        "BT848A",
        "BT849A",
        "BT829A",
        "BT827A",
        "BT878",
        "BT879",
        "BT880",
        "VPX3226E",
        "SAA7114",
        "SAA7115",
        /* 20-29 */
        "CX880",
        "CX881",
        "CX883",
        "SAA7111",
        "SAA7113",
        "CX882",
        "TVP5150A",
        "CX25840",
        "CX25841",
        "CX25842",
        /* 30-39 */
        "CX25843",
        "CX23418",
};
#endif

static int hasRadioTuner(int tunerType)
{
	switch (tunerType) {
	case 18:		//PNPEnv_TUNER_FR1236_MK2:
	case 23:		//PNPEnv_TUNER_FM1236:
	case 38:		//PNPEnv_TUNER_FMR1236:
	case 16:		//PNPEnv_TUNER_FR1216_MK2:
	case 19:		//PNPEnv_TUNER_FR1246_MK2:
	case 21:		//PNPEnv_TUNER_FM1216:
	case 24:		//PNPEnv_TUNER_FM1246:
	case 17:		//PNPEnv_TUNER_FR1216MF_MK2:
	case 22:		//PNPEnv_TUNER_FM1216MF:
	case 20:		//PNPEnv_TUNER_FR1256_MK2:
	case 25:		//PNPEnv_TUNER_FM1256:
	case 33:		//PNPEnv_TUNER_4039FR5:
	case 42:		//PNPEnv_TUNER_4009FR5:
	case 52:		//PNPEnv_TUNER_4049FM5:
	case 54:		//PNPEnv_TUNER_4049FM5_AltI2C:
	case 44:		//PNPEnv_TUNER_4009FN5:
	case 31:		//PNPEnv_TUNER_TCPB9085P:
	case 30:		//PNPEnv_TUNER_TCPN9085D:
	case 46:		//PNPEnv_TUNER_TP18NSR01F:
	case 47:		//PNPEnv_TUNER_TP18PSB01D:
	case 49:		//PNPEnv_TUNER_TAPC_I001D:
	case 60:		//PNPEnv_TUNER_TAPE_S001D_MK3:
	case 57:		//PNPEnv_TUNER_FM1216ME_MK3:
	case 59:		//PNPEnv_TUNER_FM1216MP_MK3:
	case 58:		//PNPEnv_TUNER_FM1236_MK3:
	case 68:		//PNPEnv_TUNER_TAPE_H001F_MK3:
	case 61:		//PNPEnv_TUNER_TAPE_M001D_MK3:
	case 78:		//PNPEnv_TUNER_TDA8275C1_8290_FM:
	case 89:		//PNPEnv_TUNER_TCL_MFPE05_2:
		return 1;
	}
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
void tveeprom_hauppauge_analog(struct i2c_client *c, struct tveeprom *tvee,
                                unsigned char *eeprom_data)
#else
void tveeprom_hauppauge_analog(struct tveeprom *tvee, unsigned char *eeprom_data)
#endif
{
	/* ----------------------------------------------
	 ** The hauppauge eeprom format is tagged
	 **
	 ** if packet[0] == 0x84, then packet[0..1] == length
	 ** else length = packet[0] & 3f;
	 ** if packet[0] & f8 == f8, then EOD and packet[1] == checksum
	 **
	 ** In our (ivtv) case we're interested in the following:
	 ** tuner type: tag [00].05 or [0a].01 (index into hauppauge_tuner)
	 ** tuner fmts: tag [00].04 or [0a].00 (bitmask index into hauppauge_tuner_fmt)
	 ** radio:      tag [00].{last} or [0e].00  (bitmask.  bit2=FM)
	 ** audio proc: tag [02].01 or [05].00 (lower nibble indexes lut?)

	 ** Fun info:
	 ** model:      tag [00].07-08 or [06].00-01
	 ** revision:   tag [00].09-0b or [06].04-06
	 ** serial#:    tag [01].05-07 or [04].04-06

	 ** # of inputs/outputs ???
	 */

	int i, j, k, len, done, beenhere, tag, tuner = 0, t_format = 0;
	char *t_name = NULL, *t_fmt_name = NULL, *fmt_name_str[5];

	tvee->revision = done = len = beenhere = 0;
	for (i = 0; !done && i < 256; i += len) {
		dprintk(2, "processing pos = %02x (%02x, %02x)\n",
			i, eeprom_data[i], eeprom_data[i + 1]);

		if (eeprom_data[i] == 0x84) {
			len = eeprom_data[i + 1] + (eeprom_data[i + 2] << 8);
			i += 3;
		} else if ((eeprom_data[i] & 0xf0) == 0x70) {
			if ((eeprom_data[i] & 0x08)) {
				/* verify checksum! */
				done = 1;
				break;
			}
			len = eeprom_data[i] & 0x07;
			++i;
		} else {
			TVEEPROM_KERN_ERR
			    ("Encountered bad packet header [%02x]. "
			     "Corrupt or not a Hauppauge eeprom.\n",
			     eeprom_data[i]);
			return;
		}

		dprintk(1, "Tag [%02x] + %d bytes:\n", eeprom_data[i], len);
		for(j = 1; j < len; j++) {
			dprintk(1, "   0x%02x\n", eeprom_data[i + j]);
		}

		/* process by tag */
		tag = eeprom_data[i];
		switch (tag) {
		case 0x00:
			tuner = eeprom_data[i + 6];
			t_format = eeprom_data[i + 5];
			tvee->has_radio = eeprom_data[i + len - 1];
			tvee->model =
			    eeprom_data[i + 8] + (eeprom_data[i + 9] << 8);
			tvee->revision = eeprom_data[i + 10] +
			    (eeprom_data[i + 11] << 8) +
			    (eeprom_data[i + 12] << 16);
			break;
		case 0x01:
			tvee->serial_number =
			    eeprom_data[i + 6] +
			    (eeprom_data[i + 7] << 8) +
			    (eeprom_data[i + 8] << 16);
			break;
		case 0x02:
                        /* It seems that either this tag is used, or
                           tag 0x05, but not both. */
                        /* Bit 7 is used for something else, so remove it */
			tvee->audio_processor = eeprom_data[i + 2] & 0x7f;
			break;
		case 0x04:
			tvee->serial_number =
			    eeprom_data[i + 5] +
			    (eeprom_data[i + 6] << 8) +
			    (eeprom_data[i + 7] << 16);
			break;
		case 0x05:
			tvee->audio_processor = eeprom_data[i + 1] & 0x7f;
			break;
		case 0x06:
			tvee->model =
			    eeprom_data[i + 1] + (eeprom_data[i + 2] << 8);
			tvee->revision = eeprom_data[i + 5] +
			    (eeprom_data[i + 6] << 8) +
			    (eeprom_data[i + 7] << 16);
			break;
		case 0x09:
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)) || !defined(USE_TVEEPROM_H)
			tvee->decoder_processor = eeprom_data[i + 1];
#endif
			break;
		case 0x0a:
			if (beenhere == 0) {
				tuner = eeprom_data[i + 2];
				t_format = eeprom_data[i + 1];
				beenhere = 1;
				break;
			} else {
                                TVEEPROM_KERN_INFO("Second (radio) tuner idx %d\n",
                                                   eeprom_data[i + 2]);
				break;
			}
		case 0x0e:
			tvee->has_radio = eeprom_data[i + 1];
			break;
		default:
			dprintk(1, "Not sure what to do with tag [%02x]\n",
				tag);
			/* dump the rest of the packet? */
		}

	}

	if (!done) {
		TVEEPROM_KERN_ERR("Ran out of data!\n");
		return;
	}

	if (tvee->revision != 0) {
		tvee->rev_str[0] = 32 + ((tvee->revision >> 18) & 0x3f);
		tvee->rev_str[1] = 32 + ((tvee->revision >> 12) & 0x3f);
		tvee->rev_str[2] = 32 + ((tvee->revision >> 6) & 0x3f);
		tvee->rev_str[3] = 32 + (tvee->revision & 0x3f);
		tvee->rev_str[4] = 0;
	}

	if (hasRadioTuner(tuner) && !tvee->has_radio) {
		TVEEPROM_KERN_INFO
		    ("The eeprom says no radio is present, but the tuner type %d\n", tuner);
		TVEEPROM_KERN_INFO
		    ("indicates otherwise. I will assume that radio is present.\n");
		tvee->has_radio = 1;
	}

	if (tuner < sizeof(hauppauge_tuner) / sizeof(struct HAUPPAUGE_TUNER)) {
		tvee->tuner_type = hauppauge_tuner[tuner].id;
		t_name = hauppauge_tuner[tuner].name;
	} else {
		t_name = "<unknown>";
	}

	tvee->tuner_formats = 0;
	t_fmt_name = "<none>";
	fmt_name_str[0] = "<none>";
	fmt_name_str[1] = "";
	fmt_name_str[2] = "";
	fmt_name_str[3] = "";
	fmt_name_str[4] = "";
	k = 0;
	for (i = 0; i < 8; i++) {
		if (t_format & (1 << i)) {
			tvee->tuner_formats |= hauppauge_tuner_fmt[i].id;
			/* yuck */
			t_fmt_name = hauppauge_tuner_fmt[i].name;
			fmt_name_str[k++] = hauppauge_tuner_fmt[i].name;
		}
	}
#if 0
	if (t_format <
	    sizeof(hauppauge_tuner_fmt) / sizeof(struct HAUPPAUGE_TUNER_FMT)) {
		tvee->tuner_formats = hauppauge_tuner_fmt[t_format].id;
		t_fmt_name = hauppauge_tuner_fmt[t_format].name;
	} else {
		t_fmt_name = "<unknown>";
	}
#endif

	TVEEPROM_KERN_INFO("ivtv version\n");
	TVEEPROM_KERN_INFO("Hauppauge: model = %d, rev = %s, serial# = %d\n",
			   tvee->model, tvee->rev_str, tvee->serial_number);
	TVEEPROM_KERN_INFO("tuner = %s (idx = %d, type = %d)\n",
			   t_name, tuner, tvee->tuner_type);
	TVEEPROM_KERN_INFO("tuner fmt = %s%s%s%s%s(eeprom = 0x%02x, v4l2 = 0x%08x)\n",
			   fmt_name_str[0],fmt_name_str[1],fmt_name_str[2],fmt_name_str[3],fmt_name_str[4], t_format, tvee->tuner_formats);
	TVEEPROM_KERN_INFO("audio processor = %s (type = %x)\n",
		   STRM(audioIC, tvee->audio_processor),
		   tvee->audio_processor);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)) || !defined(USE_TVEEPROM_H)
	TVEEPROM_KERN_INFO("decoder processor = %s (type = %x)\n",
		   STRM(decoderIC, tvee->decoder_processor),
		   tvee->decoder_processor);
#endif
}

EXPORT_SYMBOL(tveeprom_hauppauge_analog);

/* ----------------------------------------------------------------------- */
/* generic helper functions                                                */

int tveeprom_read(struct i2c_client *c, unsigned char *eedata, int len)
{
	unsigned char buf;
	int err;

	dprintk(1, "%s\n",__FUNCTION__);
	buf = 0;
	if (1 != (err = i2c_master_send(c,&buf,1))) {
		printk(KERN_INFO "tveeprom(%s): Huh, no eeprom present (err=%d)?\n",
		       c->name,err);
		return -1;
	}
	if (len != (err = i2c_master_recv(c,eedata,len))) {
		printk(KERN_WARNING "tveeprom(%s): i2c eeprom read error (err=%d)\n",
		       c->name,err);
		return -1;
	}
	return 0;
}
EXPORT_SYMBOL(tveeprom_read);

int tveeprom_dump(unsigned char *eedata, int len)
{
	int i;

	dprintk(1, "%s\n",__FUNCTION__);
	for (i = 0; i < len; i++) {
		if (0 == (i % 16))
			printk(KERN_INFO "tveeprom: %02x:",i);
		printk(" %02x",eedata[i]);
		if (15 == (i % 16))
			printk("\n");
	}
	return 0;
}
EXPORT_SYMBOL(tveeprom_dump);

/* ----------------------------------------------------------------------- */

/* write I2C */
static int tvee_I2CWrite(struct i2c_client *client,
			 unsigned char b1, unsigned char b2, int both)
{
	unsigned char buffer[2];
	int bytes = both ? 2 : 1;

	buffer[0] = b1;
	buffer[1] = b2;
	if (bytes != i2c_master_send(client, buffer, bytes))
		return -1;
	return 0;
}

static void tvee_readee(struct i2c_client *client, unsigned char *eedata)
{
	int i;

	if (tvee_I2CWrite(client, 0, -1, 0) < 0) {
		TVEEPROM_KERN_ERR("readee error\n");
		return;
	}

	for (i = 0; i < 256; i += 16) {
		if (16 != i2c_master_recv(client, eedata + i, 16)) {
			TVEEPROM_KERN_ERR("readee error\n");
			break;
		}
	}
}

/* ----------------------------------------------------------------------- */

static int
tveeprom_command(struct i2c_client *client, unsigned int cmd, void *arg)
{

	struct tveeprom *eeprom = i2c_get_clientdata(client);
	u32 *eeprom_props = arg;

	switch (cmd) {

	case 0:
		eeprom_props[0] = eeprom->tuner_type;
		eeprom_props[1] = eeprom->tuner_formats;
		eeprom_props[2] = eeprom->model;
		eeprom_props[3] = eeprom->revision;
		eeprom_props[4] = eeprom->has_radio;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
#ifndef NEW_I2C
/* pre i2c-2.8.0 */

static void tveeprom_inc_use(struct i2c_client *client)
{
#ifdef MODULE
/*	MOD_INC_USE_COUNT; */
#endif
}

static void tveeprom_dec_use(struct i2c_client *client)
{
#ifdef MODULE
/*	MOD_DEC_USE_COUNT; */
#endif
}

#else
/* i2c-2.8.0 and later */
#endif

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] = { I2C_TVEEPROM >> 1, I2C_CLIENT_END };
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif

I2C_CLIENT_INSMOD;

struct i2c_driver i2c_driver_tveeprom;

static int tveeprom_detect_client(struct i2c_adapter *adapter, int address,
#ifndef LINUX26
				  unsigned short flags,
#endif
				  int kind)
{
	struct i2c_client *client;
	struct tveeprom *eeprom;

	dprintk(1, "detecting tveeprom client on address 0x%x\n", address << 1);

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return -ENOMEM;
	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver_tveeprom;
	client->flags = I2C_CLIENT_ALLOW_USE;
	snprintf(client->name, sizeof(client->name) - 1, "tveeprom");

	eeprom = kmalloc(sizeof(struct tveeprom), GFP_KERNEL);
	if (eeprom == NULL) {
		kfree(client);
		return -ENOMEM;
	}
	i2c_set_clientdata(client, eeprom);
	memset(eeprom, 0, sizeof(struct tveeprom));
	eeprom->tuner_type = TUNER_ABSENT;

	tvee_readee(client, eeprom_buf);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)
	tveeprom_hauppauge_analog(client, eeprom, eeprom_buf);
#else
	tveeprom_hauppauge_analog(eeprom, eeprom_buf);
#endif

	i2c_attach_client(client);
	return 0;
}

static int tveeprom_attach_adapter(struct i2c_adapter *adapter)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
	if (adapter->id != I2C_HW_B_BT848)
#else
	if (adapter->id != (I2C_ALGO_BIT | I2C_HW_B_BT848))
#endif
		return 0;
	dprintk(1, "starting probe for adapter %s (0x%x)\n",
		adapter->name, adapter->id);
	return i2c_probe(adapter, &addr_data, tveeprom_detect_client);
}

static int tveeprom_detach_client(struct i2c_client *client)
{
	struct tveeprom *eeprom = i2c_get_clientdata(client);
	int err;

	err = i2c_detach_client(client);
	if (err) {
		return err;
	}

	kfree(eeprom);
	kfree(client);
	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
struct i2c_driver i2c_driver_tveeprom = {
	.name = "tveeprom",

	.id = I2C_DRIVERID_TVEEPROM,
	.flags = I2C_DF_NOTIFY,

	.attach_adapter = tveeprom_attach_adapter,
	.detach_client = tveeprom_detach_client,
	.command = tveeprom_command,
#ifndef NEW_I2C
/* pre i2c-2.8.0 */
	.inc_use = tveeprom_inc_use,
	.dec_use = tveeprom_dec_use,
#else
/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif

static int __init tveeprom_init(void)
{
	return i2c_add_driver(&i2c_driver_tveeprom);
}

static void __exit tveeprom_exit(void)
{
	i2c_del_driver(&i2c_driver_tveeprom);
}

module_init(tveeprom_init);
module_exit(tveeprom_exit);
