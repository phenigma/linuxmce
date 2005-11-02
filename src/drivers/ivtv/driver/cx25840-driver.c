/* 
 * cx25840 - driver version 0.0.1
 *
 * Copyright (C) 2004 Ulf Eklund
 *
 * Based on the saa7115 driver and on the first verison of Chris Kennedys
 * cx25840 driver.
 *
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

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/video_decoder.h>
#include <linux/i2c.h>

#define IVTV_INTERNAL
#include "ivtv.h"
#include "decoder.h"
#include "cx25840.h"
#include "compat.h"

#include "cx25840-driver.h"
#include "cx25840-registers.h"
#include "cx25840-audio.h"
#include "cx25840-firmware.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#endif	/* LINUX26 */

MODULE_DESCRIPTION("CX25840 Broadcast audio and video decoder driver");
MODULE_AUTHOR("Ulf Eklund");
MODULE_LICENSE("GPL");

#ifndef I2C_DRIVERID_CX25840
// Using temporary hack for missing I2C driver-ID for CX25840
#define I2C_DRIVERID_CX25840 I2C_DRIVERID_EXP3
#endif	/* I2C_DRIVERID_CX25840 */

int debug = 0;
static int fastfw = 1;
static char *firmware = NULL;

#ifdef LINUX26
module_param(debug, bool, 0644);
module_param(fastfw, bool, 0444);
module_param(firmware, charp, 0444);
#else
MODULE_PARM(debug, "i");
MODULE_PARM(fastfw, "i");
MODULE_PARM(firmware, "s");
#endif	/* LINUX26 */

MODULE_PARM_DESC(debug, "Debugging messages\n\t\t\t0=Off (default), 1=On");
MODULE_PARM_DESC(fastfw,
		 "Load firmware at elevated i2c speed.\n\t\t\t0=100HMz, 1=333MHz (default)");
MODULE_PARM_DESC(firmware, "Firmware image\n\t\t\tDefault: " FWFILE);

static int pvr150_layout[] = {
	CX25840_SVIDEO,
	CX25840_SVIDEO,
	CX25840_COMPOSITE,
	CX25840_COMPOSITE,
	CX25840_COMPOSITE,
	CX25840_COMPOSITE,
	CX25840_TUNER,
	CX25840_TUNER
};

static int pg600_layout[] = {
	CX25840_SVIDEO,
	CX25840_TUNER,
	CX25840_COMPOSITE,
	CX25840_FM,
	CX25840_AUDIOIN,
	-1, -1, -1
};

static int odd_parity(u8 c)
{
	c ^= (c >> 4);
	c ^= (c >> 2);
	c ^= (c >> 1);

	return c & 1;
}

static int decode_vps(u8 * dst, u8 * p)
{
	static const u8 biphase_tbl[] = {
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96,
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2,
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94,
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5,
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1,
		0xc3, 0x4b, 0x43, 0xc3, 0x87, 0x0f, 0x07, 0x87,
		0x83, 0x0b, 0x03, 0x83, 0xc3, 0x4b, 0x43, 0xc3,
		0xc1, 0x49, 0x41, 0xc1, 0x85, 0x0d, 0x05, 0x85,
		0x81, 0x09, 0x01, 0x81, 0xc1, 0x49, 0x41, 0xc1,
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5,
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1,
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4,
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0,
		0xc2, 0x4a, 0x42, 0xc2, 0x86, 0x0e, 0x06, 0x86,
		0x82, 0x0a, 0x02, 0x82, 0xc2, 0x4a, 0x42, 0xc2,
		0xc0, 0x48, 0x40, 0xc0, 0x84, 0x0c, 0x04, 0x84,
		0x80, 0x08, 0x00, 0x80, 0xc0, 0x48, 0x40, 0xc0,
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4,
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96,
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2,
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94,
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0,
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4,
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0,
	};
	u8 c, err = 0;
	int i;

	for (i = 0; i < 2 * 13; i += 2) {
		err |= biphase_tbl[p[i]] | biphase_tbl[p[i + 1]];
		c = (biphase_tbl[p[i + 1]] & 0xf) |
		    ((biphase_tbl[p[i]] & 0xf) << 4);
		dst[i / 2] = c;
	}
	return err & 0xf0;
}

/* ----------------------------------------------------------------------- */

int cx25840_write(struct i2c_client *client, u16 addr, u8 value)
{
	u8 buffer[3];
	buffer[0] = addr >> 8;
	buffer[1] = addr & 0xff;
	buffer[2] = value;
	return i2c_master_send(client, buffer, 3);
}

int cx25840_write4(struct i2c_client *client, u16 addr, u32 value)
{
	u8 buffer[6];
	buffer[0] = addr >> 8;
	buffer[1] = addr & 0xff;
	buffer[2] = value >> 24;
	buffer[3] = (value >> 16) & 0xff;
	buffer[4] = (value >> 8) & 0xff;
	buffer[5] = value & 0xff;
	return i2c_master_send(client, buffer, 6);
}

u8 cx25840_read(struct i2c_client *client, u16 addr)
{
	u8 buffer[2];
	buffer[0] = addr >> 8;
	buffer[1] = addr & 0xff;

	if (i2c_master_send(client, buffer, 2) < 2)
		return 0;

	if (i2c_master_recv(client, buffer, 1) < 1)
		return 0;

	return buffer[0];
}

u32 cx25840_read4(struct i2c_client *client, u16 addr)
{
	u8 buffer[4];
	buffer[0] = addr >> 8;
	buffer[1] = addr & 0xff;

	if (i2c_master_send(client, buffer, 2) < 2)
		return 0;

	if (i2c_master_recv(client, buffer, 4) < 4)
		return 0;

	return (buffer[0] << 24) | (buffer[1] << 16) | 
               (buffer[2] << 8) | buffer[3];
}

int cx25840_and_or(struct i2c_client *client, u16 addr, u8 and_mask, u8 or_value)
{
        return cx25840_write(client, addr,
                        (cx25840_read(client, addr) & and_mask) | or_value);
}

void cx25840_write_setting(struct i2c_client *client, u16 setting, u32 value)
{
	u16 addr = CX25840_REGISTER_ADDRESS(CX25840_SETTING_REGISTER_INDEX(setting));
	int s = CX25840_SETTING_START_BIT(setting);
	int l = CX25840_SETTING_BIT_LENGTH(setting);
	u8 byte, mask;

	while (l > 0) {
		if (s == 0 && l >= 8)
			byte = value;	// just get the low byte
		else {
			mask = (0xff >> (8 - (l > 8 ? 8 : l))) << s;
			byte = cx25840_read(client, addr) & ~mask;
			byte |= (value << s) & mask;
		}
		cx25840_write(client, addr++, byte);

		l -= 8 - s;
		value >>= 8 - s;
		s = 0;
	}
}

u32 cx25840_read_setting(struct i2c_client *client, u16 setting)
{
	u16 addr = CX25840_REGISTER_ADDRESS(CX25840_SETTING_REGISTER_INDEX(setting));
	int s = CX25840_SETTING_START_BIT(setting);
	int l = CX25840_SETTING_BIT_LENGTH(setting);
	int rl = (s + l + 7) / 8, i = 0;
	u32 value = 0;

	while (rl-- > 0)
		value |= cx25840_read(client, addr++) << (i++ * 8);

	return (value >> s) & (0xffffffff >> (32 - l));
}

static void vbi_reg_setup(struct i2c_client *client)
{
	struct cx25840_info *info = i2c_get_clientdata(client);
	int is_pal;
	int is_secam = 0;

	switch (info->state.norm) {
	case VIDEO_MODE_NTSC:
		is_pal = 0;
		break;
	case VIDEO_MODE_PAL:
		is_pal = 1;
		break;
	case VIDEO_MODE_SECAM:
		is_pal = 1;
		is_secam = 1;
		break;
	default:

		switch (cx25840_read(client, 0x40D) & 0xf) { // AFD_FMT_STAT
		case 0x01:	// NTSC-M
		case 0x02:	// NTSC-J
		case 0x03:	// NTSC-4.43
			is_pal = 0;
			break;
		case 0x04:	// PAL-BDGHI
		case 0x05:	// PAL-M
		case 0x06:	// PAL-N
		case 0x07:	// PAL-NC
		case 0x08:	// PAL-60
			is_pal = 1;
			break;
		case 0x0c:	// SECAM
			is_pal = 1;
			is_secam = 1;
			break;
		default:
			is_pal = 1;
			break;
		}
		break;
	}

	if (is_pal) {
		// 8d.
		cx25840_write(client, 0x49F, 0x11); // COMB_PHASE_LIMIT

		cx25840_write(client, 0x470, 0x84);
		cx25840_write(client, 0x471, 0x00);
		cx25840_write(client, 0x472, 0x2d);
		cx25840_write(client, 0x473, 0x5d);

		cx25840_write(client, 0x474, 0x24);
		cx25840_write(client, 0x475, 0x40);
		cx25840_write(client, 0x476, 0x24);
		cx25840_write(client, 0x477, 0x28);

		cx25840_write(client, 0x478, 0x1f);
		cx25840_write(client, 0x479, 0x02);

		if (is_secam) {
			cx25840_write(client, 0x47a, 0x80);
			cx25840_write(client, 0x47b, 0x00);
			cx25840_write(client, 0x47c, 0x5f);
			cx25840_write(client, 0x47d, 0x42);
		} else {
			cx25840_write(client, 0x47a, 0x90);
			cx25840_write(client, 0x47b, 0x20);
			cx25840_write(client, 0x47c, 0x63);
			cx25840_write(client, 0x47d, 0x82);
		}

		cx25840_write(client, 0x47e, 0x0a);
		cx25840_write(client, 0x47f, 0x01);
	} else {
		// 8d.
		cx25840_write(client, 0x49F, 0x14); // COMB_PHASE_LIMIT

		cx25840_write(client, 0x470, 0x7a);
		cx25840_write(client, 0x471, 0x00);
		cx25840_write(client, 0x472, 0x2d);
		cx25840_write(client, 0x473, 0x5b);

		cx25840_write(client, 0x474, 0x1A);
		cx25840_write(client, 0x475, 0x70);
		cx25840_write(client, 0x476, 0x1E);
		cx25840_write(client, 0x477, 0x1E);

		cx25840_write(client, 0x478, 0x1f);
		cx25840_write(client, 0x479, 0x02);
		cx25840_write(client, 0x47a, 0x50);
		cx25840_write(client, 0x47b, 0x66);

		cx25840_write(client, 0x47c, 0x1f);
		cx25840_write(client, 0x47d, 0x7c);
		cx25840_write(client, 0x47e, 0x08);
		cx25840_write(client, 0x47f, 0x00);
	}
}

static inline void init_dll1(struct i2c_client *client)
{
        // This is the Hauppauge sequence used to initialize the Delay Lock
        // Loop 1 (ADC DLL).
	cx25840_write(client, 0x159, 0x23);
	cx25840_write(client, 0x15a, 0x87);
	cx25840_write(client, 0x15b, 0x06);
	cx25840_write(client, 0x159, 0xe1);
	cx25840_write(client, 0x15a, 0x86);
	cx25840_write(client, 0x159, 0xe0);
	cx25840_write(client, 0x159, 0xe1);
	cx25840_write(client, 0x15b, 0x10);
}

static inline void init_dll2(struct i2c_client *client)
{
        // This is the Hauppauge sequence used to initialize the Delay Lock
        // Loop 2 (Audio Decoder DLL).
	cx25840_write(client, 0x15d, 0xe3);
	cx25840_write(client, 0x15e, 0x86);
	cx25840_write(client, 0x15f, 0x06);
	cx25840_write(client, 0x15d, 0xe1);
	cx25840_write(client, 0x15d, 0xe0);
	cx25840_write(client, 0x15d, 0xe1);
}

static void cx25840_initialize(struct i2c_client *client, int loadfw)
{
	CX25840_SET_START_MICROCNTL(0x00);      // 0x803
        // The default of this register should be 4, but I get 0 instead.
        // Set this register to 4 manually.
	cx25840_write(client, 0x000, 0x04);
	// 3.
	init_dll1(client);
	init_dll2(client);
        cx25840_write(client, 0x136, 0x0A);
	// 4.
	CX25840_SET_TUNE_FIL_RST(0x01);         // 0x13C
	CX25840_SET_TUNE_FIL_RST(0x00);         // 0x13C
	// 5.
        if (loadfw)
        	load_audio_firmware(client);
	// 6a.
	CX25840_SET_VID_OUT_EN(0x01);           // 0x115
	CX25840_SET_PIXCLK_OUT_EN(0x01);        // 0x115
	CX25840_SET_SA_SDOUT_OUT_EN(0x01);      // 0x115
	// 6b.
	CX25840_SET_SA_BCLKOUT_OUT_EN(0x01);    // 0x116
	CX25840_SET_SA_WCLKOUT_OUT_EN(0x01);    // 0x116
	CX25840_SET_PLL_CLK_OUT_EN(0x01);       // 0x116
	// 6c.
	CX25840_SET_VID_OUT_SPD(0x01);          // 0x118
	// 7.
	CX25840_SET_VD_SOFT_RST(0x01);          // 0x4A5
	CX25840_SET_VD_SOFT_RST(0x00);          // 0x4A5
	cx25840_write(client, 0x402, 0x00);
	// 8a.
	CX25840_SET_AUTO_SC_LOCK(0x00);         // 0x401
	CX25840_SET_MAN_SC_FAST_LOCK(0x00);     // 0x401
	// 8b.
	CX25840_SET_MAJ_SEL_EN(0x01);           // 0x4A2
	// 8c.
	cx25840_write(client, 0x402, 0x04);     // COMB_NOTCH_MODE
	// 10.
	cx25840_write(client, 0x8d3, 0x1f);
	cx25840_write(client, 0x8e3, 0x03);

	vbi_reg_setup(client);

	CX25840_SET_SA_IN_WS_SEL(0x01);         // 0x914
	CX25840_SET_SA_IN_SONY_MODE(0x01);      // 0x914
	CX25840_SET_SA_OUT_WS_SEL(0x01);        // 0x918
	CX25840_SET_SA_OUT_SONY_MODE(0x01);     // 0x918
	CX25840_SET_SA_OUT_MASTER_MODE(0x01);   // 0x919

	CX25840_SET_AUD_STANDARD(0x0f);         // 0x808
	CX25840_SET_PREF_MODE(0x04);            // 0x809
	cx25840_write(client, 0x8cf, 0x0f);

	CX25840_SET_START_MICROCNTL(0x01);      // 0x803
}

static void cx25840_input_change(struct i2c_client *client)
{
        u8 fmt = cx25840_read(client, 0x40D) & 0xf; // AFD_FMT_STAT

        if (fmt >= 4 && fmt <= 8) {
                // Follow tuner change procedure for PAL
                cx25840_write(client, 0x808, 0xff);
                cx25840_write(client, 0x80B, 0x10);
        }
        else if (fmt == 0xc) {
                // Select autodetect for SECAM
                cx25840_write(client, 0x808, 0xff);
                cx25840_write(client, 0x80B, 0x10);
        }
        else {
                // NTSC
                cx25840_write(client, 0x808, 0xf6);
                cx25840_write(client, 0x80B, 0);
        }
        if (cx25840_read(client, 0x803) & 0x10) {
                // restart audio decoder microcontroller
                cx25840_and_or(client, 0x803, ~0x10, 0);
                cx25840_and_or(client, 0x803, ~0x10, 0x10);
        }
}

static void log_status(struct i2c_client *client)
{
        static const char * const fmt_strs[] = {
                "0x0",
                "NTSC-M",
                "NTSC-J",
                "NTSC-4.43",
                "PAL-BDGHI",
                "PAL-M",
                "PAL-N",
                "PAL-NC",
                "PAL-60",
                "0x9",
                "0xA",
                "0xB",
                "SECAM",
                "0xD",
                "0xE",
                "0xF"
        };

	struct cx25840_info *info = i2c_get_clientdata(client);
        u8 microctrl_vidfmt = cx25840_read(client, 0x80A);
        u8 gen_stat1 = cx25840_read(client, 0x40D);
        u8 download_ctl = cx25840_read(client, 0x803);
        u8 mod_det_stat0 = cx25840_read(client, 0x804);
        u8 mod_det_stat1 = cx25840_read(client, 0x805);
        u8 audio_config = cx25840_read(client, 0x808);
        u8 pref_mode = cx25840_read(client, 0x809);
        u8 afc0 = cx25840_read(client, 0x80B);
        u8 mute_ctl = cx25840_read(client, 0x8D3);
	char *p;

	CX25840_INFO("Video signal:              %spresent\n", (microctrl_vidfmt & 0x10) ? "" : "not ");
	CX25840_INFO("Detected format:           %s (microcontroller: %s)\n",
                        fmt_strs[gen_stat1 & 0xf], fmt_strs[microctrl_vidfmt & 0xf]);

	switch (mod_det_stat0) {
                case 0x00: p = "mono"; break;
                case 0x01: p = "stereo"; break;
                case 0x02: p = "dual"; break;
                case 0x04: p = "tri"; break;
                case 0x10: p = "mono with SAP"; break;
                case 0x11: p = "stereo with SAP"; break;
                case 0x12: p = "dual with SAP"; break;
                case 0x14: p = "tri with SAP"; break;
                case 0xFE: p = "forced mode"; break;
                default:   p = "not defined";
	}
	CX25840_INFO("Detected audio mode:       %s\n", p);

	switch (mod_det_stat1) {
                case 0x00: p = "not defined"; break;
                case 0x01: p = "EIAJ"; break;
                case 0x02: p = "A2-M"; break;
                case 0x03: p = "A2-BG"; break;
                case 0x04: p = "A2-DK1"; break;
                case 0x05: p = "A2-DK2"; break;
                case 0x06: p = "A2-DK3"; break;
                case 0x07: p = "A1 (6.0 MHz FM Mono)"; break;
                case 0x08: p = "AM-L"; break;
                case 0x09: p = "NICAM-BG"; break;
                case 0x0A: p = "NICAM-DK"; break;
                case 0x0B: p = "NICAM-I"; break;
                case 0x0C: p = "NICAM-L"; break;
                case 0x0D: p = "BTSC/EIAJ/A2-M Mono (4.5 MHz FMMono)"; break;
                case 0x0E: p = "IF FM Radio"; break;
                case 0x0F: p = "BTSC"; break;
                case 0x10: p = "high-deviation FM"; break;
                case 0x11: p = "very high-deviation FM"; break;
                case 0xFD: p = "unknown audio standard"; break;
                case 0xFE: p = "forced audio standard"; break;
                case 0xFF: p = "no detected audio standard"; break;
                default:   p = "not defined";
	}
	CX25840_INFO("Detected audio standard:   %s\n", p);
	CX25840_INFO("Audio muted:               %s\n", (mute_ctl & 0x2) ? "yes" : "no");
	CX25840_INFO("Audio microcontroller:     %s\n", (download_ctl & 0x10) ? "running" : "stopped");
	
        switch (audio_config >> 4) {
                case 0x00: p = "undefined"; break;
                case 0x01: p = "BTSC"; break;
                case 0x02: p = "EIAJ"; break;
                case 0x03: p = "A2-M"; break;
                case 0x04: p = "A2-BG"; break;
                case 0x05: p = "A2-DK1"; break;
                case 0x06: p = "A2-DK2"; break;
                case 0x07: p = "A2-DK3"; break;
                case 0x08: p = "A1 (6.0 MHz FM Mono)"; break;
                case 0x09: p = "AM-L"; break;
                case 0x0A: p = "NICAM-BG"; break;
                case 0x0B: p = "NICAM-DK"; break;
                case 0x0C: p = "NICAM-I"; break;
                case 0x0D: p = "NICAM-L"; break;
                case 0x0E: p = "FM radio"; break;
                case 0x0F: p = "automatic detection"; break;
                default:   p = "undefined"; break;
	}
	CX25840_INFO("Configured audio standard: %s\n", p);
	
        if ((audio_config >> 4) < 0xF) {
                switch (audio_config & 0xF) {
                        case 0x00: p = "MONO1 (LANGUAGE A/Mono L+R channel for BTSC, EIAJ, A2)"; break;
                        case 0x01: p = "MONO2 (LANGUAGE B)"; break;
                        case 0x02: p = "MONO3 (STEREO forced MONO)"; break;
                        case 0x03: p = "MONO4 (NICAM ANALOG-Language C/Analog Fallback)"; break;
                        case 0x04: p = "STEREO"; break;
                        case 0x05: p = "DUAL1 (AB)"; break;
                        case 0x06: p = "DUAL2 (AC) (FM)"; break;
                        case 0x07: p = "DUAL3 (BC) (FM)"; break;
                        case 0x08: p = "DUAL4 (AC) (AM)"; break;
                        case 0x09: p = "DUAL5 (BC) (AM)"; break;
                        case 0x0A: p = "SAP"; break;
                        default:   p = "undefined"; break;
                }
	        CX25840_INFO("Configured audio mode:     %s\n", p);
        }
        else {
                switch (audio_config & 0xF) {
                        case 0x00: p = "BG"; break;
                        case 0x01: p = "DK1"; break;
                        case 0x02: p = "DK2"; break;
                        case 0x03: p = "DK3"; break;
                        case 0x04: p = "I"; break;
                        case 0x05: p = "L"; break;
                        case 0x06: p = "BTSC"; break;
                        case 0x07: p = "EIAJ"; break;
                        case 0x08: p = "A2-M"; break;
                        case 0x09: p = "FM Radio"; break;
                        case 0x0F: p = "automatic standard and mode detection"; break;
                        default:   p = "undefined"; break;
                }
	        CX25840_INFO("Configured audio system:   %s\n", p);
        }
        switch (info->state.norm) {
                case VIDEO_MODE_NTSC:  p = "NTSC"; break;
                case VIDEO_MODE_PAL:   p = "PAL"; break;
                case VIDEO_MODE_SECAM: p = "SECAM"; break;
                default:               p = "undefined"; break;
        }
        CX25840_INFO("Specified standard:        %s\n", p);
        if (info->state.input == -1) {
                p = "undefined";
        }
        else {
                switch (info->layout[info->state.input]) {
                        case CX25840_COMPOSITE: p = "composite"; break;
                        case CX25840_SVIDEO:    p = "S-Video"; break;
                        case CX25840_TUNER:     p = "tuner"; break;
                }
        }
        CX25840_INFO("Specified input:           %s\n", p);
        CX25840_INFO("Specified audio input:     %s\n",
                        info->state.audio_input == 0 ? "tuner" : "external");
        switch (info->state.audio) {
                case DECODER_AUDIO_441_KHZ: p = "44.1 kHz"; break;
                case DECODER_AUDIO_48_KHZ:  p = "48 kHz"; break;
                case DECODER_AUDIO_32_KHZ:  p = "32 kHz"; break;
                default:                    p = "undefined"; break;
        }
        CX25840_INFO("Specified audio rate:      %s\n", p);
        switch (pref_mode & 0xf) {
                case 0:  p = "mono/language A"; break;
                case 1:  p = "language B"; break;
                case 2:  p = "language C"; break;
                case 3:  p = "analog fallback"; break;
                case 4:  p = "stereo"; break;
                case 5:  p = "language AC"; break;
                case 6:  p = "language BC"; break;
                case 7:  p = "language AB"; break;
                default: p = "undefined"; break;
        }
        CX25840_INFO("Preferred audio mode:      %s\n", p);
        if ((audio_config & 0xf) == 0xf) {
                switch ((afc0 >> 3) & 0x3) {
                        case 0:  p = "system DK"; break;
                        case 1:  p = "system L"; break;
                        case 2:  p = "autodetect"; break;
                        default: p = "undefined"; break;
                }
                CX25840_INFO("Selected 65 MHz format:    %s\n", p);
                switch (afc0 & 0x7) {
                        case 0:  p = "chroma"; break;
                        case 1:  p = "BTSC"; break;
                        case 2:  p = "EIAJ"; break;
                        case 3:  p = "A2-M"; break;
                        case 4:  p = "autodetect"; break;
                        default: p = "undefined"; break;
                }
                CX25840_INFO("Selected 45 MHz format:    %s\n", p);
        }
}

static void decoder_full_reset(struct i2c_client *client)
{
	CX25840_DEBUG("Reset");
	cx25840_initialize(client, 0);
}

static int cx25840_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct cx25840_info *info = i2c_get_clientdata(client);

	switch (cmd) {
	case 0:
		break;

	// ioctls to allow direct access to the cx25840 registers for testing
	case DECODER_GET_REG:
	{
		struct decoder_reg *reg = arg;

		reg->val = cx25840_read(client, reg->reg & 0x0fff);
		break;
	}
	case DECODER_SET_REG:
	{
		struct decoder_reg *reg = arg;

		cx25840_write(client, reg->reg & 0x0fff, reg->val & 0xff);
		break;
	}

	case DECODER_SET_CARD_TYPE:
	{
		info->cardtype = *(int *)arg;
		if (info->cardtype == 1) {
			info->layout = pg600_layout;
		}
		break;
	}

	case DECODER_LOG_STATUS:
		log_status(client);
		break;

	case DECODER_SET_SIZE:
	{
		// Used video_window because it has height/width and is already defined 
		struct video_window *wind = arg;
		int HSC, VSC;
		int Vsrc, Hsrc;
		int filter, Vlines;
		int is_pal = (info->state.norm != VIDEO_MODE_NTSC);

		Vsrc = cx25840_read_setting(client, VACTIVE_CNT);
		Hsrc = cx25840_read_setting(client, HACTIVE_CNT);

		Vlines = wind->height + (is_pal ? 4 : 7);

		if ((wind->width * 16 < Hsrc) || (Hsrc < wind->width) ||
		    (Vlines * 8 < Vsrc) || (Vsrc < Vlines)) {
			CX25840_ERR("decoder set size: ignoring bad config request Hsrc=%d Vsrc=%d width=%d height=%d\n",
			     Hsrc, Vsrc, wind->width, wind->height);
			return -EINVAL;
		}

		HSC = (Hsrc * (1 << 20)) / wind->width - (1 << 20);
		VSC = (1 << 16) - (Vsrc * (1 << 9) / Vlines - (1 << 9));

		HSC &= 0xffffff;
		VSC &= 0x1fff;

		if (wind->width >= 385)
			filter = 0;
		else if (wind->width > 192)
			filter = 1;
		else if (wind->width > 96)
			filter = 2;
		else
			filter = 3;

		CX25840_DEBUG("decoder set size %dx%d (from %dx%d) -> scale  %ux%u, filter %d\n",
		     wind->width, wind->height, Hsrc, Vsrc, HSC, VSC, filter);

		CX25840_SET_VS_INTRLACE(0x01);
		CX25840_SET_VFILT(filter);
		CX25840_SET_HSCALE(HSC);
		CX25840_SET_VSCALE(VSC);
		break;
	}

	case DECODER_GET_CAPABILITIES:
	{
		struct video_decoder_capability *cap = arg;

		CX25840_DEBUG("decoder get capabilities\n");

		cap->flags = VIDEO_DECODER_PAL |
		    VIDEO_DECODER_NTSC |
		    VIDEO_DECODER_SECAM |
		    VIDEO_DECODER_AUTO | VIDEO_DECODER_CCIR;
		cap->inputs = 8;
		cap->outputs = 1;
		break;
	}

	case DECODER_SET_AUDIO:
	{
		int *iarg = arg;
		int result = 0;

		result = cx25840_set_audio_rate(client, &info->state, *iarg);

		if (result)
			return result;

		break;
	}

	case DECODER_SET_AUDIO_INPUT:
	{
		int *iarg = arg;
		int result = 0;

		result = cx25840_set_audio_input(client, &info->state, *iarg);

		// sample rate settings might be different between inputs
		if (!result)
			result =
			    cx25840_set_audio_rate(client, &info->state, info->state.audio);

		if (result)
			return result;

		break;
	}

	case DECODER_GET_STATUS:
	{
		int *iarg = arg;
		int res;
		res = 0;
		if (cx25840_read(client, 0x80A) & 0x10)  // VIDEO_PRESENT
			res |= DECODER_STATUS_GOOD;
		switch (info->state.norm) {
		case VIDEO_MODE_NTSC:
			res |= DECODER_STATUS_NTSC;
			break;
		case VIDEO_MODE_PAL:
			res |= DECODER_STATUS_PAL;
			break;
		case VIDEO_MODE_SECAM:
			res |= DECODER_STATUS_SECAM;
			break;
		case VIDEO_MODE_AUTO:
		default:
			switch (cx25840_read(client, 0x40D) & 0xF) { // AFD_FMT_STAT
			case 0x01:	// NTSC-M
			case 0x02:	// NTSC-J
			case 0x03:	// NTSC-4.43
				res |= DECODER_STATUS_NTSC;
				break;
			case 0x04:	// PAL-BDGHI
			case 0x05:	// PAL-M
			case 0x06:	// PAL-N
			case 0x07:	// PAL-NC
			case 0x08:	// PAL-60
				res |= DECODER_STATUS_PAL;
				break;
			case 0x0c:	// SECAM
				res |= DECODER_STATUS_SECAM;
				break;
			}
			break;
		}
		// Macrovision check??? if (0x40C != 0) { res |= DECODER_STATUS_COLOR; }
		*iarg = res;
		break;
	}

	case DECODER_SET_NORM:
	{
		int *iarg = arg;
		switch (*iarg) {
		case VIDEO_MODE_NTSC:
			CX25840_DEBUG("decoder set norm NTSC\n");
                        cx25840_write(client, 0x808, 0xf6);
                        cx25840_write(client, 0x80B, 0);
			break;
		case VIDEO_MODE_PAL:
			CX25840_DEBUG("decoder set norm PAL\n");
                        cx25840_write(client, 0x808, 0xff);
                        cx25840_write(client, 0x80B, 0x10);
			break;
		case VIDEO_MODE_SECAM:
			CX25840_DEBUG("decoder set norm SECAM\n");
                        cx25840_write(client, 0x808, 0xff);
                        cx25840_write(client, 0x80B, 0x10);
			break;
		default:
			CX25840_DEBUG("Unknown video mode\n");
			return -EINVAL;
		}
		info->state.norm = *iarg;
		break;
	}

	case DECODER_SET_INPUT:
	{
		int *iarg = arg;

		/* inputs from 0-7 are available */
		if (*iarg < 0 || *iarg > 7) {
			CX25840_ERR("%d is not a valid input!\n", *iarg);
			return -EINVAL;
		}

		CX25840_DEBUG("decoder set input (%d)\n", *iarg);

		switch (info->layout[*iarg]) {
		case CX25840_COMPOSITE:
			CX25840_DEBUG("now setting Composite input\n");
			CX25840_SET_CH_1__SOURCE(info->cardtype == 1 ? 0x00 : 0x02); // 0x103
                        CX25840_SET_INPUT_MODE(0x00);   // 0x401
			CX25840_SET_CH_3__SOURCE(0x00); // 0x103
			CX25840_SET_CH_2__SOURCE(0x00); // 0x103
			break;

		case CX25840_SVIDEO:
			CX25840_DEBUG("now setting S-Video input\n");
			CX25840_SET_CH_1__SOURCE(info->cardtype == 1 ? 0x02 : 0x00); // 0x103
			CX25840_SET_CH_2__SOURCE(info->cardtype == 1 ? 0x00 : 0x01); // 0x103
			CX25840_SET_CH_3__SOURCE(0x00); // 0x103
			CX25840_SET_CH_SEL_ADC2(0x00);   // 0x102
			CX25840_SET_INPUT_MODE(0x01);    // 0x401
			CX25840_SET_VGA_SEL_CH2(0x00);   // 0x104
			CX25840_SET_VGA_SEL_CH3(0x00);   // 0x105
			CX25840_SET_CHROMA_IN_SEL(0x01); // 0x105
			break;

		case CX25840_TUNER:
			CX25840_DEBUG("now setting Tuner input\n");
#if 0		/* Original code, not sure if the new code works
			   for all PG600/PVR550 cards. If the new code
			   works well then this original code can be
			   deleted. */
			CX25840_SET_CH_1__SOURCE(info->cardtype == 1 ? 0x01 : 0x06);
			CX25840_SET_CH_3__SOURCE(0x01);
			CX25840_SET_CH_SEL_ADC2(0x01);
			CX25840_SET_INPUT_MODE(0x00);
#else
			CX25840_SET_CH_1__SOURCE(info->cardtype == 1 ? 0x01 : 0x06); // 0x103
			CX25840_SET_CH_2__SOURCE(info->cardtype == 1 ? 0x01 : 0x00); // 0x103
			CX25840_SET_CH_3__SOURCE(info->cardtype == 1 ? 0x00 : 0x01); // 0x103
			CX25840_SET_CH_SEL_ADC2(info->cardtype == 1 ? 0x00 : 0x01);  // 0x102
			CX25840_SET_INPUT_MODE(0x00);                                // 0x401
#endif
			break;

		case -1:
			CX25840_ERR("%d is not a valid input!\n", *iarg);
			return -EINVAL;
		}

		info->state.input = *iarg;
                cx25840_input_change(client);
		break;
	}

	case DECODER_SET_OUTPUT:
	{
		int *iarg = arg;
		CX25840_DEBUG("decoder set output\n");
		if (*iarg != 0) {
			return -EINVAL;
		}
		break;
	}

	case DECODER_ENABLE_OUTPUT:
	{
		int *iarg = arg;
		int enable = (*iarg != 0);

		if (info->state.enable == enable)
			break;

		CX25840_DEBUG("decoder %s output\n", enable ? "enable" : "disable");

		if (enable) {
			// Re-init Norm
			cx25840_command(client, DECODER_SET_NORM, &info->state.norm);
			// Re-init Input
			cx25840_command(client, DECODER_SET_INPUT, &info->state.input);
			// Re-init Audio Input
			cx25840_command(client, DECODER_SET_AUDIO_INPUT, &info->state.audio_input);
			// Re-init VBI
			cx25840_command(client, DECODER_SET_VBI, &info->state.vbi_mode);
                        cx25840_input_change(client);
		}

		CX25840_SET_VID_OUT_EN(enable);
		CX25840_SET_PLL_CLK_OUT_EN(enable);

		info->state.enable = enable;
		break;
	}

	case DECODER_GET_VBI:
	{
		static u8 lcr2vbi[] = {
			0, IVTV_SLICED_TYPE_TELETEXT_B, 0, 0,	// 1 = Teletext
			IVTV_SLICED_TYPE_WSS_625, 0, IVTV_SLICED_TYPE_CAPTION_525, 0, 0,	// 4 = WSS, 6 = CC
			0, IVTV_SLICED_TYPE_VPS, 0, 0,	// 9 = VPS
			0, 0, 0, 0
		};
		struct decoder_lcr *lcr = (struct decoder_lcr *)arg;
		int i;

		lcr->raw = (cx25840_read(client, 0x404) & 0x10) ? 0 : 1;
		memset(lcr->lcr, 0, sizeof(lcr->lcr));
		if (lcr->raw)
			break;
		for (i = 7; i <= 23; i++) {
			u8 v = cx25840_read(client, 0x424 + i - 7);

			lcr->lcr[i] = lcr2vbi[v >> 4] << 4 | lcr2vbi[v & 0xF];
		}
		break;
	}

	case DECODER_SET_VBI:
	{
		static u8 vbi2lcr[] = {
			0, 1, 0, 0,	// Teletext = 1
			6, 4, 0, 9,	// CC = 6, WSS = 4, VPS = 9
			0, 0, 0, 0,
			0, 0, 0, 0
		};
		struct decoder_lcr *declcr = (struct decoder_lcr *)arg;
		int is_pal = (info->state.norm != VIDEO_MODE_NTSC);
		int vbi_offset = is_pal ? 1 : 0;
		int i, x;
		u8 lcr[24];

		info->state.vbi_mode = *declcr;

		if (declcr->raw) {
			for (i = 0; i <= 23; i++)
				declcr->lcr[i] = 0;
			/* Setup VBI */
			vbi_reg_setup(client);

			/* VBI Offset */
			cx25840_write(client, 0x47F, vbi_offset);
			cx25840_write(client, 0x404, 0x2e);
			break;
		}

		for (x = 0; x <= 23; x++)
			lcr[x] = 0x00;

		/* Setup VBI */
		vbi_reg_setup(client);

		/* Sliced VBI */
		cx25840_write(client, 0x404, 0x36);	/* Ancillery data */
		cx25840_write(client, 0x406, 0x13);
		cx25840_write(client, 0x47F, vbi_offset);

		if (is_pal) {
			for (i = 0; i <= 6; i++)
				declcr->lcr[i] = 0;
		} else {
			for (i = 0; i <= 9; i++)
				declcr->lcr[i] = 0;
			for (i = 22; i <= 23; i++)
				declcr->lcr[i] = 0;
		}
		for (i = 7; i <= 23; i++)
			lcr[i] = vbi2lcr[declcr->lcr[i] >> 4] << 4 |
			    vbi2lcr[declcr->lcr[i] & 0xF];
		for (x = 1, i = 0x424; i <= 0x434; i++, x++) {
			cx25840_write(client, i, lcr[6 + x]);
		}
		cx25840_write(client, 0x43C, 0x16);
		if (is_pal) {
		        cx25840_write(client, 0x474, 0x2a);
                }
                else {
		        cx25840_write(client, 0x474, 0x1a+6);
                }
		break;
	}

	case DECODER_DECODE_VBI_LINE:{
		struct decode_vbi_line *vbi = (struct decode_vbi_line *)arg;
		u8 *p = vbi->p;
		int id1, id2, l, err = 0;

		if (p[0] || p[1] != 0xff || p[2] != 0xff ||
		    (p[3] != 0x55 && p[3] != 0x91)) {
			vbi->line = vbi->type = 0;
			break;
		}
		p += 4;
		id1 = p[-1];
		id2 = p[0] & 0xf;
		l = p[2] & 0x3f;
		l += 5;
		p += 4;
		switch (id2) {
		case 1:
			id2 = V4L2_SLICED_TELETEXT_B;
			break;
		case 6:
			id2 = V4L2_SLICED_CAPTION_525;
			err = !odd_parity(p[0]) || !odd_parity(p[1]);
			break;
		case 9:
			id2 = V4L2_SLICED_VPS;
			if (decode_vps(p, p) != 0) {
				err = 1;
			}
			break;
		case 4:
			id2 = V4L2_SLICED_WSS_625;
			break;
		default:
			id2 = 0;
			err = 1;
			break;
		}
		vbi->type = err ? 0 : id2;
		vbi->line = err ? 0 : l;
		vbi->is_even_field = err ? 0 : (id1 == 0x55);
		vbi->p = p;
		break;
	}

	case DECODER_GET_STATE:
	{
		struct decoder_state *pic = arg;

		pic->bright = info->state.bright;
		pic->contrast = info->state.contrast;
		pic->sat = info->state.sat;
		pic->hue = info->state.hue;
		break;
	}

	case DECODER_SET_STATE:
	{
		struct decoder_state *pic = arg;

		if (pic->bright < 0 || pic->bright > 255) {
			CX25840_ERR("invalid brightness setting %d\n",
				    pic->bright);
			return -EINVAL;
		}

		if (pic->contrast < 0 || pic->contrast > 127) {
			CX25840_ERR("invalid contrast setting %d\n",
				    pic->contrast);
			return -EINVAL;
		}

		if (pic->sat < 0 || pic->sat > 127) {
			CX25840_ERR("invalid saturation setting %d\n",
				    pic->sat);
			return -EINVAL;
		}

		if (pic->hue < -127 || pic->hue > 127) {
			CX25840_ERR("invalid hue setting %d\n", pic->hue);
			return -EINVAL;
		};

		info->state.contrast = pic->contrast;
		info->state.bright = pic->bright;
		info->state.sat = pic->sat;
		info->state.hue = pic->hue;

		CX25840_SET_USAT(info->state.sat * 2);
		CX25840_SET_VSAT(info->state.sat * 2);
		CX25840_SET_BRIGHT((info->state.bright - 128));
		CX25840_SET_CNTRST(info->state.contrast * 2);
		CX25840_SET_HUE(info->state.hue);

		CX25840_DEBUG
		    ("decoder set picture bright=%d contrast=%d saturation=%d hue=%d\n",
		     info->state.bright, info->state.contrast, info->state.sat, info->state.hue);
		break;
	}

	case DECODER_RESET:
		decoder_full_reset(client);
		break;

	case DECODER_GET_VIDEO_STATUS:
	{
		int *status = arg;
		if (cx25840_read_setting(client, VIDEO_PRESENT))
			*status = VIDEO_IS_PRESENT;
		else
			*status = VIDEO_NOT_PRESENT;
		break;
	}

	case VIDIOCGAUDIO:
	{
		/* this shouldn't be called directly from user-space */
		struct video_audio *va = arg;
		cx25840_get_v4l_audio(client, va);
		break;
	}

	case VIDIOCSAUDIO:
	{
		/* this shouldn't be called directly from user-space */
		struct video_audio *va = arg;
		cx25840_set_v4l_audio(client, va);
		break;
	}
        
        case VIDIOCSFREQ:
        {
                cx25840_input_change(client);
                break;
        }

        case VIDIOC_S_TUNER:
        {
		struct v4l2_tuner *vt = arg;

                switch (vt->audmode) {
                        case V4L2_TUNER_MODE_MONO:
                        case V4L2_TUNER_MODE_LANG1:
                                // Force PREF_MODE to MONO
                                cx25840_and_or(client, 0x809, ~0xf, 0);
                                break;
                        case V4L2_TUNER_MODE_STEREO:
                                // Force PREF_MODE to STEREO
                                cx25840_and_or(client, 0x809, ~0xf, 4);
                                break;
                        case V4L2_TUNER_MODE_LANG2:
                                // Force PREF_MODE to LANG2
                                cx25840_and_or(client, 0x809, ~0xf, 1);
                                break;
                }
                break;
        }

        case VIDIOC_G_TUNER:
        {
		struct v4l2_tuner *vt = arg;
                u8 mode = cx25840_read(client, 0x804);
                u8 pref = cx25840_read(client, 0x809) & 0xf;  // PREF_MODE
                int val = 0;
		
                vt->capability |=
		    V4L2_TUNER_CAP_STEREO | V4L2_TUNER_CAP_LANG1 |
		    V4L2_TUNER_CAP_LANG2 | V4L2_TUNER_CAP_SAP;

		/* get rxsubchans and audmode */
		if ((mode & 0xf) == 1)
			val |= V4L2_TUNER_SUB_STEREO;
		else
			val |= V4L2_TUNER_SUB_MONO;
		if (mode == 2 || mode == 4)
			val |= V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_LANG2;
                if (mode & 0x10)
                        val |= V4L2_TUNER_SUB_SAP;
                vt->rxsubchans = val;
                switch (pref) {
                        case 0:
                                vt->audmode = V4L2_TUNER_MODE_MONO;
                                break;
                        case 1:
                        case 2:
                                vt->audmode = V4L2_TUNER_MODE_LANG2;
                                break;
                        case 4:
                        default:
                                vt->audmode = V4L2_TUNER_MODE_STEREO;
                                break;
                }
                break;
        }

	default:
		CX25840_ERR("invalid ioctl %X\n", cmd);
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
#ifndef NEW_I2C
/* pre i2c-2.8.0 */

static void cx25840_inc_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif	/* MODULE */
}

static void cx25840_dec_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif	/* MODULE */
}

#else
/* i2c-2.8.0 and later */
#endif	/* NEW_I2C */

#define I2C_CX25840_ADDRESS  0x88

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */

static unsigned short normal_i2c[] =
    { I2C_CX25840_ADDRESS >> 1, I2C_CLIENT_END };
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif	/* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) */

I2C_CLIENT_INSMOD;

struct i2c_driver i2c_driver_cx25840;

static int cx25840_detect_client(struct i2c_adapter *adapter, int address,
#ifndef LINUX26
				 unsigned short flags,
#endif	/* LINUX26 */
				 int kind)
{
	struct i2c_client *client;
	struct cx25840_info *info;
	u16 device_id;

	/* Check if the adapter supports the needed features 
	 * Not until kernel version 2.6.11 did the bit-algo
	 * correctly report that it would do an I2C-level xfer */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 11)
	if (!adapter->algo->master_xfer)
#else
	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
#endif	/* LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 11) */
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return -ENOMEM;

	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver_cx25840;
	client->flags = I2C_CLIENT_ALLOW_USE;
	snprintf(client->name, sizeof(client->name) - 1, "cx25840");
	
        CX25840_DEBUG("detecting cx25840 client on address 0x%x\n", address << 1);

	device_id = cx25840_read_setting(client, DEVICE_ID);
	/* The high byte of the device ID should be 0x84 if chip is present */
	if ((device_id & 0xFF00) != 0x8400) {
		CX25840_DEBUG("cx25840 not found\n");
		kfree(client);
		return 0;
	}
	CX25840_INFO("cx25%3x-2%x found @ 0x%x (%s)\n",
		      (device_id & 0xfff0) >> 4,
		      (device_id & 0x0f) < 3 ? (device_id & 0x0f) + 1 : 3,
                      address << 1, adapter->name);

	info = kmalloc(sizeof(struct cx25840_info), GFP_KERNEL);
	if (info == NULL) {
		kfree(client);
		return -ENOMEM;
	}

	i2c_set_clientdata(client, info);
	memset(info, 0, sizeof(struct cx25840_info));
	info->state.norm = VIDEO_MODE_NTSC;
	info->state.input = 0;
	info->state.enable = 1;
	info->state.bright = 128;
	info->state.contrast = 64;
	info->state.sat = 64;
	info->state.hue = 0;
	info->state.audio = DECODER_AUDIO_48_KHZ;
	info->state.audio_input = 0;
	info->layout = pvr150_layout;
	info->firmware = firmware;
	info->fastfw = fastfw;

	cx25840_initialize(client, 1);

	i2c_attach_client(client);

	return 0;
}

static int cx25840_attach_adapter(struct i2c_adapter *adapter)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
	if (adapter->id == I2C_HW_B_BT848) {
#else
	if (adapter->id == (I2C_ALGO_BIT | I2C_HW_B_BT848)) {
#endif
		return i2c_probe(adapter, &addr_data, &cx25840_detect_client);
	}
	return 0;
}

static int cx25840_detach_client(struct i2c_client *client)
{
	struct cx25840_state *info = i2c_get_clientdata(client);
	int err;

	err = i2c_detach_client(client);
	if (err) {
		return err;
	}

	kfree(info);
	kfree(client);
	return 0;
}

/* i2c implementation */
struct i2c_driver i2c_driver_cx25840 = {
	.name = "cx25840",

	.id = I2C_DRIVERID_CX25840,
	.flags = I2C_DF_NOTIFY,

	.attach_adapter = cx25840_attach_adapter,
	.detach_client = cx25840_detach_client,
	.command = cx25840_command,
#ifndef NEW_I2C
	/* pre i2c-2.8.0 */
	.inc_use = cx25840_inc_use,
	.dec_use = cx25840_dec_use,
#else
	/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif	/* NEW_I2C */
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif	/* LINUX26 */

static int __init m__init(void)
{
	return i2c_add_driver(&i2c_driver_cx25840);
}

static void __exit m__exit(void)
{
	i2c_del_driver(&i2c_driver_cx25840);
}

module_init(m__init);
module_exit(m__exit);
