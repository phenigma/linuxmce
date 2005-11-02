/*
   em2820-cards.c - driver for Empia EM2820/2840 USB video capture devices

   Copyright (C) 2005 Markus Rechberger <mrechberger@gmail.com>
                      Ludovico Cavedon <cavedon@sssup.it>
                      Mauro Carvalho Chehab <mchehab@brturbo.com.br>

   Based on the em2800 driver from Sascha Sommer <saschasommer@freenet.de>

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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/usb.h>
#include <media/tuner.h>
#include "audiochip.h"
#include "tveeprom.h"
#include "msp3400.h"

#include "em2820.h"

enum em2820_board_entry {
#if 0
	EM2800_BOARD_TERRATEC_CINERGY_200,
#endif
	EM2820_BOARD_TERRATEC_CINERGY_250,
	EM2820_BOARD_PINNACLE_USB_2,
	EM2820_BOARD_HAUPPAUGE_WINTV_USB_2,
	EM2820_BOARD_MSI_VOX_USB_2
};

struct em2820_board em2820_boards[] = {
#if 0
	[EM2800_BOARD_TERRATEC_CINERGY_200] = {
		.name         = "Terratec Cinergy 200 USB",
		.vchannels    = 3,
		.norm         = VIDEO_MODE_PAL,
		.tuner_type   = TUNER_LG_PAL_NEW_TAPC,
		.tda9887_conf = TDA9887_PRESENT,
		.has_tuner    = 1,
		.decoder      = EM2820_SAA7113,
		.input          = {{
			.type     = EM2820_VMUX_TELEVISION,
			.vmux     = 2,
			.amux     = 0,
		},{
			.type     = EM2820_VMUX_COMPOSITE1,
			.vmux     = 0,
			.amux     = 1,
		},{
			.type     = EM2820_VMUX_SVIDEO,
			.vmux     = 9,
			.amux     = 1,
		}},
	},
#endif
	[EM2820_BOARD_TERRATEC_CINERGY_250] = {
		.name         = "Terratec Cinergy 250 USB",
		.vchannels    = 3,
		.norm         = VIDEO_MODE_PAL,
		.tuner_type   = TUNER_LG_PAL_NEW_TAPC,
		.tda9887_conf = TDA9887_PRESENT,
		.has_tuner    = 1,
		.decoder      = EM2820_SAA7113,
		.input          = {{
			.type     = EM2820_VMUX_TELEVISION,
			.vmux     = 2,
			.amux     = 0,
		},{
			.type     = EM2820_VMUX_COMPOSITE1,
			.vmux     = 0,
			.amux     = 1,
		},{
			.type     = EM2820_VMUX_SVIDEO,
			.vmux     = 9,
			.amux     = 1,
		}},
	},
	[EM2820_BOARD_PINNACLE_USB_2] = {
		.name         = "Pinnacle PCTV USB 2",
		.vchannels    = 3,
		.norm         = VIDEO_MODE_PAL,
		.tuner_type   = TUNER_LG_PAL_NEW_TAPC,
		.tda9887_conf = TDA9887_PRESENT,
		.has_tuner    = 1,
		.decoder      = EM2820_SAA7113,
		.input          = {{
			.type     = EM2820_VMUX_TELEVISION,
			.vmux     = 2,
			.amux     = 0,
		},{
			.type     = EM2820_VMUX_COMPOSITE1,
			.vmux     = 0,
			.amux     = 1,
		},{
			.type     = EM2820_VMUX_SVIDEO,
			.vmux     = 9,
			.amux     = 1,
		}},
	},
	[EM2820_BOARD_HAUPPAUGE_WINTV_USB_2] = {
		.name         = "Hauppauge WinTV USB 2",
		.vchannels    = 3,
		.norm         = VIDEO_MODE_NTSC,
		.tuner_type   = TUNER_PHILIPS_FM1236_MK3,
		.tda9887_conf = TDA9887_PRESENT|TDA9887_PORT1_ACTIVE|TDA9887_PORT2_ACTIVE,
		.has_tuner    = 1,
		.decoder      = EM2820_TVP5150,
		.has_msp34xx  = 1,
		/*FIXME: S-Video not tested */
		.input          = {{
			.type     = EM2820_VMUX_TELEVISION,
			.vmux     = 0,
			.amux     = 0,
		},{
			.type     = EM2820_VMUX_SVIDEO,
			.vmux     = 2,
			.amux     = 1,
		}},
	},
	[EM2820_BOARD_MSI_VOX_USB_2] = {
		.name		= "MSI VOX USB 2.0",
		.vchannels	= 3,
		.norm		= VIDEO_MODE_PAL,
		.tuner_type	= TUNER_PHILIPS_PAL,
		.tda9887_conf	= TDA9887_PRESENT|TDA9887_PORT1_ACTIVE|TDA9887_PORT2_ACTIVE,
		.has_tuner	= 1,
		.decoder      = EM2820_SAA7114,
		.input          = {{
			.type     = EM2820_VMUX_TELEVISION,
			.vmux     = 2,
			.amux     = 0,
		},{
			.type     = EM2820_VMUX_COMPOSITE1,
			.vmux     = 0,
			.amux     = 1,
		},{
			.type     = EM2820_VMUX_SVIDEO,
			.vmux     = 9,
			.amux     = 1,
		}},
	},
	{ }  /* Terminating entry */
};

/* table of devices that work with this driver */
struct usb_device_id em2820_id_table [] = {
 /* Terratec Cinerhy 200 USB: em2800 nor supported, at the moment */
 /*	{ USB_DEVICE(0xeb1a, 0x2800), .driver_info = EM2800_BOARD_TERRATEC_CINERGY_200 }, */
	{ USB_DEVICE(0x0ccd, 0x0036), .driver_info = EM2820_BOARD_TERRATEC_CINERGY_250 },
	{ USB_DEVICE(0x2304, 0x0208), .driver_info = EM2820_BOARD_PINNACLE_USB_2 },
	{ USB_DEVICE(0x2040, 0x4200), .driver_info = EM2820_BOARD_HAUPPAUGE_WINTV_USB_2 },
	{ USB_DEVICE(0xeb1a, 0x2820), .driver_info = EM2820_BOARD_MSI_VOX_USB_2 },
	{ },
};

void em2820_card_setup(struct em2820 *dev)
{
	/* request some modules */
	if (dev->model == EM2820_BOARD_HAUPPAUGE_WINTV_USB_2) {
		struct tveeprom tv;
#ifdef CONFIG_MODULES
		request_module("tveeprom");
#endif
		/* Call first TVeeprom */

		tveeprom_hauppauge_analog(&dev->i2c_client, &tv, dev->eedata);

		dev->tuner_type= tv.tuner_type;
		if (tv.audio_processor == AUDIO_CHIP_MSP34XX) {
			dev->has_msp34xx=1;
		} else dev->has_msp34xx=0;
#if 0
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x00,0x06,"\x40",1);
		em2820_write_regs_req(dev,0x00,0x15,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x16,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x17,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x18,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x19,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x1a,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x23,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x24,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x13,"\x08",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x27,"\x34",1);
		em2820_write_regs_req(dev,0x00,0x10,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x11,"\x11",1);
		em2820_write_regs_req(dev,0x00,0x28,"\x01",1);
		em2820_write_regs_req(dev,0x00,0x29,"\xaf",1);
		em2820_write_regs_req(dev,0x00,0x2a,"\x01",1);
		em2820_write_regs_req(dev,0x00,0x2b,"\x3b",1);
		em2820_write_regs_req(dev,0x00,0x1c,"\x08",1);
		em2820_write_regs_req(dev,0x00,0x1d,"\x0d",1);
		em2820_write_regs_req(dev,0x00,0x1e,"\xb0",1);
		em2820_write_regs_req(dev,0x00,0x1f,"\x3c",1);
		em2820_write_regs_req(dev,0x00,0x1b,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x1b,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x30,"\x99\x01",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0f\x02",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x27\x20",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x1a\x0c",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x04\xc0",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x28\x06",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x1b\x54",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0d\x47",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x28\x00",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x30\x01",2);
		em2820_write_regs_req(dev,0x02,0xc6,"\x0f\xb0\x8e\x02\xb0",5);
		em2820_write_regs_req(dev,0x02,0x86,"\x00\x14\x10\x44",4);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0e\x35\x03",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x10\x5a\x00",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0d\x1e\x00",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x10\x00\x40\x00\x01",5);
		em2820_write_regs_req(dev,0x02,0xb8,"\x03\x6f",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x00\x00",2);
		em2820_write_regs_req(dev,0x00,0x21,"\x14",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0b\x00",2);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x14,"\x32",1);
		em2820_write_regs_req(dev,0x00,0x25,"\x02",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x88",1);
		em2820_write_regs_req(dev,0x00,0x27,"\x34",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x67",1);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x0a",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x0a",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x00\x80\x00",3);
		em2820_write_regs_req(dev,0x02,0x88,"\x00\x00\x00",3);
		em2820_write_regs_req(dev,0x03,0x88,"\x13\x00\x1f",3);
		em2820_write_regs_req(dev,0x00,0x08,"\xf7",1);
		em2820_write_regs_req(dev,0x02,0x60,"\x0a",1);
		em2820_write_regs_req(dev,0x02,0x60,"\x0b",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x02,0xc6,"\x0f\xb0\x8e\x02\xb0",5);
		em2820_write_regs_req(dev,0x02,0x86,"\x00\x14\x10\x44",4);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x03,0xa0,"\x32",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x36",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x38",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x39",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x3a",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x3b",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x3c",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x3d",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x25",1);
		em2820_write_regs_req(dev,0x00,0x0d,"\x42",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x22,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x03\x6f",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x00\x00",2);
		em2820_write_regs_req(dev,0x00,0x20,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x22,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x02,0xc6,"\x0f\xb0\x8e\x02\xb0",5);
		em2820_write_regs_req(dev,0x02,0x86,"\x00\x14\x10\x44",4);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x00,0x20,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x22,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x25",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x03\x6f",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x00\x00",2);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x00,0x06,"\x40",1);
		em2820_write_regs_req(dev,0x00,0x15,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x16,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x17,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x18,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x19,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x1a,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x23,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x24,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x13,"\x08",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x27,"\x34",1);
		em2820_write_regs_req(dev,0x00,0x10,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x11,"\x11",1);
		em2820_write_regs_req(dev,0x00,0x28,"\x01",1);
		em2820_write_regs_req(dev,0x00,0x29,"\xaf",1);
		em2820_write_regs_req(dev,0x00,0x2a,"\x01",1);
		em2820_write_regs_req(dev,0x00,0x2b,"\x3b",1);
		em2820_write_regs_req(dev,0x00,0x1c,"\x08",1);
		em2820_write_regs_req(dev,0x00,0x1d,"\x0d",1);
		em2820_write_regs_req(dev,0x00,0x1e,"\xb0",1);
		em2820_write_regs_req(dev,0x00,0x1f,"\x3c",1);
		em2820_write_regs_req(dev,0x00,0x1b,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x1b,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x30,"\x99\x01",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0f\x02",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x27\x20",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x1a\x0c",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x04\xc0",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x28\x06",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x1b\x54",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0d\x47",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x28\x00",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x30\x01",2);
		em2820_write_regs_req(dev,0x02,0xc6,"\x0f\xb0\x8e\x02\xb0",5);
		em2820_write_regs_req(dev,0x02,0x86,"\x00\x14\x10\x44",4);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0e\x35\x03",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x10\x5a\x00",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0d\x1e\x00",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x10\x00\x40\x00\x01",5);
		em2820_write_regs_req(dev,0x02,0xb8,"\x03\x6f",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x00\x00",2);
		em2820_write_regs_req(dev,0x00,0x21,"\x14",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0b\x00",2);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x14,"\x32",1);
		em2820_write_regs_req(dev,0x00,0x25,"\x02",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x88",1);
		em2820_write_regs_req(dev,0x00,0x27,"\x34",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x67",1);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x12\x01\x00\x02\x00\x00\x00\x40\x40\x20\x00\x42\x00\x01\x00\x01",16);
		em2820_write_regs_req(dev,0x00,0x0c,"\x09\x02\xf9\x00\x01\x01\x00\x80\xfa\x09\x04\x00\x00\x03\xff\x00",16);
		em2820_write_regs_req(dev,0x00,0x0c,"\x09\x02\xf9\x00\x01\x01\x00\x80\xfa\x09\x04\x00\x00\x03\xff\x00",16);
		em2820_write_regs_req(dev,0x03,0xa0,"\x04",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x14",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x1c",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x18",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x1d",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x19",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x1e",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x1a",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x1f",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x1b",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x22",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x23",1);
		em2820_write_regs_req(dev,0x03,0xa0,"\x0b",1);
		em2820_write_regs_req(dev,0x00,0x06,"\x4c",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x00,0x08,"\xf7",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x02,0xc6,"\x0f\xb0\x8e\x02\xb0",5);
		em2820_write_regs_req(dev,0x02,0x86,"\x00\x14\x10\x44",4);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x02\x20",5);
		em2820_write_regs_req(dev,0x00,0x0f,"\x07",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x00\x00",5);
		em2820_write_regs_req(dev,0x02,0xb8,"\x03\x6f",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x00\x00",2);
		em2820_write_regs_req(dev,0x00,0x08,"\xff",1);
		em2820_write_regs_req(dev,0x00,0x06,"\x40",1);
		em2820_write_regs_req(dev,0x00,0x15,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x16,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x17,"\x20",1);
		em2820_write_regs_req(dev,0x00,0x18,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x19,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x1a,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x23,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x24,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x13,"\x08",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x27,"\x34",1);
		em2820_write_regs_req(dev,0x00,0x10,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x11,"\x11",1);
		em2820_write_regs_req(dev,0x00,0x28,"\x01",1);
		em2820_write_regs_req(dev,0x00,0x29,"\xaf",1);
		em2820_write_regs_req(dev,0x00,0x2a,"\x01",1);
		em2820_write_regs_req(dev,0x00,0x2b,"\x3b",1);
		em2820_write_regs_req(dev,0x00,0x1c,"\x08",1);
		em2820_write_regs_req(dev,0x00,0x1d,"\x0d",1);
		em2820_write_regs_req(dev,0x00,0x1e,"\xb0",1);
		em2820_write_regs_req(dev,0x00,0x1f,"\x3c",1);
		em2820_write_regs_req(dev,0x00,0x1b,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x1b,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x30,"\x99\x01",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0f\x02",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x27\x20",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x1a\x0c",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x04\xc0",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x28\x06",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x1b\x54",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0d\x47",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x28\x00",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x30\x01",2);
		em2820_write_regs_req(dev,0x02,0xc6,"\x0f\xb0\x8e\x02\xb0",5);
		em2820_write_regs_req(dev,0x02,0x86,"\x00\x14\x10\x44",4);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0e\x35\x03",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x10\x5a\x00",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0d\x1e\x00",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x10\x00\x40\x00\x01",5);
		em2820_write_regs_req(dev,0x02,0xb8,"\x03\x6f",2);
		em2820_write_regs_req(dev,0x02,0xb8,"\x00\x00",2);
		em2820_write_regs_req(dev,0x00,0x21,"\x14",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0b\x00",2);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x14,"\x32",1);
		em2820_write_regs_req(dev,0x00,0x25,"\x02",1);
		em2820_write_regs_req(dev,0x00,0x26,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x88",1);
		em2820_write_regs_req(dev,0x00,0x27,"\x34",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x12,"\x67",1);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x21,"\x14",1);
		em2820_write_regs_req(dev,0x00,0x20,"\x10",1);
		em2820_write_regs_req(dev,0x02,0xb8,"\x0b\x00",2);
		em2820_write_regs_req(dev,0x00,0x22,"\x10",1);
		em2820_write_regs_req(dev,0x00,0x14,"\x32",1);
		em2820_write_regs_req(dev,0x00,0x25,"\x02",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x10\x00\x30\x20\x43",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x10\x00\x20\x00\x01",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x0b\x03\x30",5);
		em2820_write_regs_req(dev,0x03,0x88,"\x11\x00\x7e",3);
		em2820_write_regs_req(dev,0x00,0x0f,"\x87",1);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x13\x0f\x60",5);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x08\x03\x30",5);
		em2820_write_regs_req(dev,0x03,0x88,"\x11\x00\x7e",3);
		em2820_write_regs_req(dev,0x02,0x88,"\x12\x00\x00\x73\x00",5);
		em2820_write_regs_req(dev,0x03,0x88,"\x11\x00\x7e",3);
		em2820_write_regs_req(dev,0x03,0x88,"\x11\x00\x7e",3);
		em2820_write_regs_req(dev,0x03,0x88,"\x11\x00\x7e",3);
		em2820_write_regs_req(dev,0x03,0x88,"\x11\x00\x7e",3);
		em2820_write_regs_req(dev,0x02,0x88,"\x10\x00\x20\x00\x20",5);
		em2820_write_regs_req(dev,0x00,0x12,"\x27",1);
		/*
		em2820_write_regs_req(dev,0x00,0x0c,"\x00",1);
		em2820_write_regs_req(dev,0x00,0x0c,"\x12\x01\x00\x02\x00\x00\x00\x40\x40\x20\x00\x42\x00\x01\x00\x01",16);
 		em2820_write_regs_req(dev,0x00,0x0c,"\x09\x02\x6d\x00\x02\x01\x00\x80\xfa",9);
		em2820_write_regs_req(dev,0x00,0x0c,"\x09\x02\x6d\x00\x02\x01\x00\x80\xfa\x09\x04\x01\x00\x00\x01\x01",16);
		*/
#endif
	}
}

EXPORT_SYMBOL(em2820_boards);
EXPORT_SYMBOL(em2820_id_table);

MODULE_DEVICE_TABLE (usb, em2820_id_table);
