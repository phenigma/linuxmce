/* 
 * cx25840 - driver version 0.0.1
 *
 * Copyright (C) 2004 Chris Kennedy
 *
 * Based on saa7115 driver
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

#include <linux/version.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/major.h>

#include <linux/slab.h>

#include <linux/mm.h>
#include <linux/pci.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/slab.h>

#include "compat.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#endif

#include <linux/videodev.h>
#include <linux/version.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/pgtable.h>
#include <asm/page.h>

MODULE_DESCRIPTION("CX25840 video decoder driver");
MODULE_AUTHOR("Chris Kennedy");
MODULE_LICENSE("GPL");

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define IVTV_MAX_CARDS 8

#ifndef I2C_DRIVERID_CX25840
// Using temporary hack for missing I2C driver-ID for CX25840
#define I2C_DRIVERID_CX25840 I2C_DRIVERID_EXP3
#endif

#include <linux/video_decoder.h>

#include "ivtv.h"
#include "cx25840.h"

static int debug = 1;
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug level (0-1)");

static int i2c_enable[IVTV_MAX_CARDS] = { 1, 1, 1, 1, 1, 1, 1, 1 };
MODULE_PARM(i2c_enable, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM_DESC(i2c_enable, "which 150/350 cards to activate the cx25840, conflicts with saa7127 or pvr350");

#define dprintk(num, args...) \
	do { \
		if (debug >= num) \
			printk(KERN_INFO "cx25840: " args); \
	} while (0)

#define SAA7115_KERN_ERR(args...) printk(KERN_ERR "cx25840: " args);
#define SAA7115_KERN_INFO(args...) printk(KERN_INFO "cx25840: " args);

/* ----------------------------------------------------------------------- */

static inline int cx25840_write (struct i2c_client *client, u8 reg, u8 value)
{
        unsigned char buffer[3];

        buffer[0] = 0x01;
        buffer[1] = reg;
        buffer[2] = value;

        return i2c_master_send(client, buffer, 3);
}

static inline int cx25840_write_vid (struct i2c_client *client, u8 reg, u8 value)
{
        unsigned char buffer[3];

        buffer[0] = 0x04;
        buffer[1] = reg;
        buffer[2] = value;

        return i2c_master_send(client, buffer, 3);
}

static inline int cx25840_write_08 (struct i2c_client *client, u8 reg, u8 value)
{
        unsigned char buffer[3];

        buffer[0] = 0x08;
        buffer[1] = reg;
        buffer[2] = value;

        return i2c_master_send(client, buffer, 3);
}

static inline int cx25840_write_09 (struct i2c_client *client, u8 reg, u8 value)
{
        unsigned char buffer[3];

        buffer[0] = 0x09;
        buffer[1] = reg;
        buffer[2] = value;

        return i2c_master_send(client, buffer, 3);
}

static inline int cx25840_write_02 (struct i2c_client *client, u8 reg, u8 value)
{
        unsigned char buffer[3];

        buffer[0] = 0x02;
        buffer[1] = reg;
        buffer[2] = value;

        return i2c_master_send(client, buffer, 3);
}

static inline int cx25840_write_host (struct i2c_client *client, u8 reg, u8 value)
{
        unsigned char buffer[2];

        buffer[0] = reg;
        buffer[1] = value;

        return i2c_master_send(client, buffer, 2);
}

static int writeregs(struct i2c_client *client, const unsigned char *regs)
{
	unsigned char reg, data;

        while (*regs != 0x00) {
                reg =* (regs++);
                data =* (regs++);
                if (cx25840_write(client, reg, data) < 0)
                        return -1;
        }
        return 0;
}

static inline int cx25840_read (struct i2c_client *client, u8 reg)
{
        unsigned char buffer[2];
        unsigned char byte;

        buffer[0] = 0x01;
        buffer[1] = reg;

        i2c_master_send(client, buffer, 2);

        i2c_master_recv(client, &byte, 1);
	return byte;	
}

static inline int cx25840_read_vid (struct i2c_client *client, u8 reg)
{
        unsigned char buffer[2];
        unsigned char byte;

        buffer[0] = 0x04;
        buffer[1] = reg;

        i2c_master_send(client, buffer, 2);

        i2c_master_recv(client, &byte, 1);
        return byte;
}

static inline int cx25840_read_02 (struct i2c_client *client, u8 reg)
{
        unsigned char buffer[2];
        unsigned char byte;

        buffer[0] = 0x02;
        buffer[1] = reg;

        i2c_master_send(client, buffer, 2);

        i2c_master_recv(client, &byte, 1);
        return byte;
}

static inline int cx25840_read_08 (struct i2c_client *client, u8 reg)
{
        unsigned char buffer[2];
        unsigned char byte;

        buffer[0] = 0x08;
        buffer[1] = reg;

        i2c_master_send(client, buffer, 2);

        i2c_master_recv(client, &byte, 1);
        return byte;
}

static inline int cx25840_read_09 (struct i2c_client *client, u8 reg)
{
        unsigned char buffer[2];
        unsigned char byte;

        buffer[0] = 0x09;
        buffer[1] = reg;

        i2c_master_send(client, buffer, 2);

        i2c_master_recv(client, &byte, 1);
        return byte;
}

static inline int cx25840_read_host (struct i2c_client *client, u8 reg)
{
        unsigned char buffer[1];
        unsigned char byte;

        buffer[0] = reg;

        i2c_master_send(client, buffer, 1);

        i2c_master_recv(client, &byte, 1);
        return byte;
}

/* ----------------------------------------------------------------------- */

// If a value differs from the Hauppauge driver values, then the comment start with
// 'was 0xXX' to denote the Hauppauge value. Otherwise the value is identical what the
// Hauppauge driver sets.

static const unsigned char init_cx25840_auto_input[] = {
	0x02, 0x02,
	0x03, 0x46,
	0x18, 0x02, /* pix_clk fast */
	0x00, 0x00
};

static const unsigned char cfg_cx25840_reset_scaler[] = {
	0x15, 0x00, /* Turn off video output */
	0x16, 0x00, /* Turn on pix_clk pin, audio? */
	0x15, 0x8c, /* Turn on video output, uses rsv bit 7?  */
	0x16, 0x07, /* Turn on pix_clk pin, audio? */
	0x00, 0x00
};

/* ============== SAA7715 VIDEO templates =============  */

static const unsigned char cfg_cx25840_NTSC_fullres_x[] = {
	0x00, 0x00
};
static const unsigned char cfg_cx25840_NTSC_fullres_y[] = {
	0x00, 0x00
};

static const unsigned char cfg_cx25840_NTSC_video[] = {
	0x00, 0x00
};
 
static const unsigned char cfg_cx25840_PAL_fullres_x[] = {
	0x00, 0x00
};
static const unsigned char cfg_cx25840_PAL_fullres_y[] = {
	0x00, 0x00
};

static const unsigned char cfg_cx25840_PAL_video[] = {
 	0x00, 0x00
};

/* ============== SAA7715 VIDEO templates (end) =======  */

static const unsigned char cfg_cx25840_vbi_on[] = {
	0x00, 0x00
};

static const unsigned char cfg_cx25840_vbi_off[] = {
	0x00, 0x00
};

static const unsigned char init_cx25840_misc[] = {
	0x00, 0x00 
};

/* ============== SAA7715 AUDIO settings =============  */
static const unsigned char cfg_cx25840_48_audio[] = {
	0x00, 0x00
};

static const unsigned char cfg_cx25840_441_audio[] = {
	0x00, 0x00
};

static const unsigned char cfg_cx25840_32_audio[] = {
	0x00, 0x00
};

static const unsigned char cfg_cx25840_NTSC_48_audio[] = {
	0x28, 0x11,
	0x29, 0x93,
	0x2A, 0xF8,
	0x2C, 0x5F,
	0x2D, 0xFF,
	0x00, 0x00
};

static const unsigned char cfg_cx25840_PAL_48_audio[] = {
	0x28, 0x11,
	0x29, 0x93,
	0x2A, 0xF8,
	0x2C, 0x5F,
	0x2D, 0xFF,
	0x00, 0x00
};

static const unsigned char cfg_cx25840_NTSC_441_audio[] = {
	0x28, 0x05,
	0x29, 0x7E,
	0x2A, 0x38,
	0x2C, 0x1B,
	0x2D, 0x8F,
	0x00, 0x00
};

static const unsigned char cfg_cx25840_PAL_441_audio[] = {
	0x28, 0x05,
	0x29, 0x7E,
	0x2A, 0x38,
	0x2C, 0x1B,
	0x2D, 0x8F,
	0x00, 0x00
};

static const unsigned char cfg_cx25840_NTSC_32_audio[] = {
	0x28, 0x0D,
	0x29, 0x2E,
	0x2A, 0xF8,
	0x2C, 0x2F,
	0x2D, 0xFF,
	0x00, 0x00
};

static const unsigned char cfg_cx25840_PAL_32_audio[] = {
	0x28, 0x0D,
	0x29, 0x2E,
	0x2A, 0xF8,
	0x2C, 0x2F,
	0x2D, 0xFF,
	0x00, 0x00
};

static void cx25840_reset(struct i2c_client *client)
{
        /* put chip to sleep, and wake back up */
        cx25840_write_host(client, 0x00, 0x01);
        cx25840_write_host(client, 0x00, 0x00);

	cx25840_write(client, 0x5A, 0x87);
	cx25840_write(client, 0x5B, 0x06);
	cx25840_write(client, 0x59, 0x21);

	udelay(10); /* Ref says to sleep 10us */

	cx25840_write(client, 0x59, 0xE1);
	cx25840_write(client, 0x59, 0x20);
	cx25840_write(client, 0x59, 0x21);
	cx25840_write(client, 0x5B, 0x10);

}

static void cx25840_audio(struct i2c_client *client)
{
	cx25840_write_08(client, 0x03, 0x00);
	cx25840_write_host(client, 0x00, 0x05);
	cx25840_write_host(client, 0x01, 0x00);
	cx25840_write_host(client, 0x02, 0x00);
	cx25840_write_host(client, 0x03, 0x00);

	cx25840_write_host(client, 0x00, 0x04);
	cx25840_write_host(client, 0x01, 0x00);
	cx25840_write_host(client, 0x02, 0x00);
	cx25840_write_host(client, 0x03, 0x00);

	cx25840_write(client, 0x58, 0x00);
	cx25840_write(client, 0x59, 0x20);
	cx25840_write(client, 0x5A, 0xE7);
	cx25840_write(client, 0x5B, 0x00);

	cx25840_write(client, 0x58, 0x00);
	cx25840_write(client, 0x59, 0x21);
	cx25840_write(client, 0x5A, 0xE7);
	cx25840_write(client, 0x5B, 0x10);

	cx25840_write(client, 0x58, 0x00);
	cx25840_write(client, 0x59, 0x20);
	cx25840_write(client, 0x5A, 0xE7);
	cx25840_write(client, 0x5B, 0x00);

	cx25840_write(client, 0x5C, 0x00);
	cx25840_write(client, 0x5D, 0x20);
	cx25840_write(client, 0x5E, 0xE7);
	cx25840_write(client, 0x5F, 0x00);

	cx25840_write(client, 0x5C, 0x00);
	cx25840_write(client, 0x5D, 0x21);
	cx25840_write(client, 0x5E, 0xE7);
	cx25840_write(client, 0x5F, 0x06);

	cx25840_write(client, 0x3C, 0x01);
	cx25840_write(client, 0x3D, 0x00);
	cx25840_write(client, 0x3E, 0x35);
	cx25840_write(client, 0x3F, 0x00);

	cx25840_write(client, 0x3C, 0x00);
	cx25840_write(client, 0x3D, 0x00);
	cx25840_write(client, 0x3E, 0x35);
	cx25840_write(client, 0x3F, 0x00);

	cx25840_write(client, 0x14, 0x00);
	cx25840_write(client, 0x15, 0x0C);
	cx25840_write(client, 0x16, 0x04);
	cx25840_write(client, 0x17, 0x00);

	cx25840_write_vid(client, 0xA4, 0x00);
	cx25840_write_vid(client, 0xA5, 0x80);
	cx25840_write_vid(client, 0xA6, 0x00);
	cx25840_write_vid(client, 0xA7, 0x00);

	cx25840_write_vid(client, 0xA4, 0x00);
	cx25840_write_vid(client, 0xA5, 0x00);
	cx25840_write_vid(client, 0xA6, 0x00);
	cx25840_write_vid(client, 0xA7, 0x00);

	cx25840_write(client, 0x18, 0x02);
	cx25840_write(client, 0x19, 0x00);
	cx25840_write(client, 0x1A, 0x00);
	cx25840_write(client, 0x1B, 0x00);

	cx25840_write_vid(client, 0x04, 0x3E);
	cx25840_write_vid(client, 0x05, 0x25);
	cx25840_write_vid(client, 0x06, 0x10);
	cx25840_write_vid(client, 0x07, 0x00);

	cx25840_write(client, 0x02, 0x00);
#if 0
	cx25840_write(client, 0x03, 0x11);
#endif
	cx25840_write_vid(client, 0x00, 0x00);
	cx25840_write_vid(client, 0x01, 0xE0);
	cx25840_write_vid(client, 0x02, 0x04);
	cx25840_write_vid(client, 0x03, 0x00);

	cx25840_write_vid(client, 0x9C, 0x14);
	cx25840_write_vid(client, 0x9D, 0x00);
	cx25840_write_vid(client, 0x9E, 0x50);
	cx25840_write_vid(client, 0x9F, 0x14);

	cx25840_write_vid(client, 0x00, 0x20);
	cx25840_write_vid(client, 0x01, 0xE0);
	cx25840_write_vid(client, 0x02, 0x04);
	cx25840_write_vid(client, 0x03, 0x00);

	cx25840_write_vid(client, 0x14, 0xF4);
	cx25840_write_vid(client, 0x15, 0x79);
	cx25840_write_vid(client, 0x22, 0x00);
	cx25840_write_vid(client, 0x20, 0x71);
	cx25840_write_vid(client, 0x21, 0x71);
	cx25840_write_vid(client, 0x22, 0x00);
	cx25840_write_vid(client, 0x20, 0x71);
	cx25840_write_vid(client, 0x21, 0x71);
	cx25840_write_vid(client, 0x14, 0xF4);
	cx25840_write_vid(client, 0x15, 0x79);

	cx25840_write_vid(client, 0x18, 0x00);
	cx25840_write_vid(client, 0x19, 0x00);
	cx25840_write_vid(client, 0x1A, 0x00);
	cx25840_write_vid(client, 0x1B, 0x04);

	cx25840_write_vid(client, 0x1C, 0x00);
	cx25840_write_vid(client, 0x1D, 0x00);
	cx25840_write_vid(client, 0x1E, 0x08);
	cx25840_write_vid(client, 0x1F, 0x00);

	cx25840_write(client, 0x02, 0x00);
#if 0
	cx25840_write(client, 0x03, 0x11);
#endif
	cx25840_write_vid(client, 0x00, 0x20);
	cx25840_write_vid(client, 0x01, 0xE0);
	cx25840_write_vid(client, 0x02, 0x04);
	cx25840_write_vid(client, 0x03, 0x00);

	cx25840_write_08(client, 0x10, 0x01);

	cx25840_write_08(client, 0x08, 0x08);
	cx25840_write_08(client, 0x08, 0xF6);
	cx25840_write_08(client, 0x09, 0x00);
	cx25840_write_08(client, 0x10, 0x01);
	cx25840_write_08(client, 0x1A, 0x00);

	cx25840_write_08(client, 0xD0, 0x70);
	cx25840_write_08(client, 0xD1, 0x38);
	cx25840_write_08(client, 0xD2, 0x06);
	cx25840_write_08(client, 0xD3, 0x1F);

	cx25840_write_08(client, 0x09, 0x04);

	/* ======================================== */

	/* These are from i2c dumps, audio module? */
	cx25840_write_08(client, 0x10, 0x01);
#if 1
	/* Audio master clock */
	cx25840_write(client, 0x27, 0x50);

	/* Video PLL Integer */
	cx25840_write(client, 0x08, 0x0F);
	cx25840_write(client, 0x09, 0x04);
	cx25840_write(client, 0x0A, 0x0A);
	cx25840_write(client, 0x0B, 0x20);

	/* Video PLL Fractional */
	cx25840_write(client, 0x10, 0xE5);
	cx25840_write(client, 0x11, 0xD6);
	cx25840_write(client, 0x12, 0x98);
	cx25840_write(client, 0x13, 0x00);
#endif
	/* Audio? */
	cx25840_write_09(client, 0x00, 0xAA);
	cx25840_write_09(client, 0x01, 0x4F);
	cx25840_write_09(client, 0x02, 0x01);
	cx25840_write_09(client, 0x03, 0x08);
	cx25840_write_09(client, 0x04, 0xAA);
	cx25840_write_09(client, 0x05, 0x4F);
	cx25840_write_09(client, 0x06, 0x01);
	cx25840_write_09(client, 0x07, 0x08);
	cx25840_write_09(client, 0x0C, 0xAA);
	cx25840_write_09(client, 0x0D, 0x4F);
	cx25840_write_09(client, 0x0E, 0x01);
	cx25840_write_09(client, 0x0F, 0x08);

	/* These are from i2c dumps, audio module? */
	cx25840_write_08(client, 0x10, 0x01);
	cx25840_write_08(client, 0x10, 0x00);
	cx25840_write_08(client, 0x10, 0x01);

	/* These are from i2c dumps, audio module? */
	cx25840_write_08(client, 0x08, 0xFF);
	cx25840_write_08(client, 0x09, 0x04);
	cx25840_write_08(client, 0x0A, 0x01);
	cx25840_write_08(client, 0x0B, 0x00);

	/* These are from i2c dumps, audio module? */
	cx25840_write_08(client, 0xD0, 0x70);
	cx25840_write_08(client, 0xD1, 0x38);
	cx25840_write_08(client, 0xD2, 0x06);
	cx25840_write_08(client, 0xD3, 0x1F); 

	cx25840_write_08(client, 0x09, 0x04);
	cx25840_write_08(client, 0x10, 0x01);
#if 1
        /* Audio master clock */
        cx25840_write(client, 0x27, 0x50);

        /* Video PLL Integer */
        cx25840_write(client, 0x08, 0x0F);
        cx25840_write(client, 0x09, 0x04);
        cx25840_write(client, 0x0A, 0x0A);
        cx25840_write(client, 0x0B, 0x20);

        /* Video PLL Fractional */
        cx25840_write(client, 0x10, 0xE5);
        cx25840_write(client, 0x11, 0xD6);
        cx25840_write(client, 0x12, 0x98);
        cx25840_write(client, 0x13, 0x00);
#endif
        /* Audio? */
        cx25840_write_09(client, 0x00, 0xAA);
        cx25840_write_09(client, 0x01, 0x4F);
        cx25840_write_09(client, 0x02, 0x01);
        cx25840_write_09(client, 0x03, 0x08);
        cx25840_write_09(client, 0x04, 0xAA);
        cx25840_write_09(client, 0x05, 0x4F);
        cx25840_write_09(client, 0x06, 0x01);
        cx25840_write_09(client, 0x07, 0x08);
        cx25840_write_09(client, 0x0C, 0xAA);
        cx25840_write_09(client, 0x0D, 0x4F);
        cx25840_write_09(client, 0x0E, 0x01);
        cx25840_write_09(client, 0x0F, 0x08);

        /* These are from i2c dumps, audio module? */
        cx25840_write_08(client, 0x10, 0x01);
        cx25840_write_08(client, 0x10, 0x00);

	cx25840_write_08(client, 0x08, 0xF6);
	cx25840_write_08(client, 0x03, 0x0B);
	cx25840_write_host(client, 0x00, 0x24);

        cx25840_write_08(client, 0x01, 0x00);
        cx25840_write_08(client, 0x00, 0x00);

	/* Firmware upload */
	char *aud_fw_mem  = vmalloc((sizeof(aud_fw_mem))*(AUDIO_FW_SIZE));
	int i;
        unsigned char buffer[2];

        buffer[0] = 0x08;
        buffer[1] = 0x02;

	if (aud_fw_mem != NULL) {
		load_aud_fw(AUDIO_FW_FILE, aud_fw_mem);
        	i2c_master_send(client, buffer, 2);
		for(i=0; i < AUDIO_FW_SIZE; i++) {
        		i2c_master_send(client, &aud_fw_mem[i], 1);
		}
	} else
		printk(KERN_ERR "Error reading cx25840 FW\n");
	vfree(aud_fw_mem); 

#if 1
        cx25840_write_host(client, 0x00, 0x04);
#endif
	cx25840_write_08(client, 0x03, 0x13);
#if 1
        cx25840_write(client, 0x14, 0x00);
        cx25840_write(client, 0x15, 0x8C);
        cx25840_write(client, 0x16, 0x07);
        cx25840_write(client, 0x17, 0x00);
#endif
        cx25840_write_09(client, 0x14, 0xA0);
        cx25840_write_09(client, 0x15, 0x00);
        cx25840_write_09(client, 0x16, 0x00);
        cx25840_write_09(client, 0x17, 0x00);

        cx25840_write_09(client, 0x18, 0xA0);
        cx25840_write_09(client, 0x19, 0x01);
        cx25840_write_09(client, 0x1A, 0x00);
        cx25840_write_09(client, 0x1B, 0x00);
#if 1
        cx25840_write(client, 0x24, 0x87);
        cx25840_write(client, 0x25, 0xB6);
        cx25840_write(client, 0x26, 0x00);
        cx25840_write(client, 0x27, 0x50);
#endif
        /* These are from i2c dumps, audio module? */
        cx25840_write_08(client, 0x08, 0xF6);
        cx25840_write_08(client, 0x09, 0x00);
        cx25840_write_08(client, 0x0A, 0x00);
        cx25840_write_08(client, 0x0B, 0x00);

        cx25840_write_08(client, 0x10, 0x01);
        cx25840_write_08(client, 0x03, 0x03);
        cx25840_write_08(client, 0x03, 0x13);

        /* These are from i2c dumps, audio module? */
        cx25840_write_08(client, 0x08, 0xF6);
        cx25840_write_08(client, 0x09, 0x00);
        cx25840_write_08(client, 0x0A, 0x01);
        cx25840_write_08(client, 0x0B, 0x00);

        cx25840_write_08(client, 0xD0, 0x70);
        cx25840_write_08(client, 0xD1, 0x38);
        cx25840_write_08(client, 0xD2, 0x06);
        cx25840_write_08(client, 0xD3, 0x1F);

        cx25840_write_08(client, 0x09, 0x04);
        cx25840_write_08(client, 0x10, 0x01);
#if 1
        cx25840_write(client, 0x27, 0x50);

        /* These are from i2c dumps, audio module? */
        cx25840_write(client, 0x08, 0x0F);
        cx25840_write(client, 0x09, 0x04);
        cx25840_write(client, 0x0A, 0x0A);
        cx25840_write(client, 0x0B, 0x20);

        cx25840_write(client, 0x10, 0xE5);
        cx25840_write(client, 0x11, 0xD6);
        cx25840_write(client, 0x12, 0x98);
        cx25840_write(client, 0x13, 0x00);
#endif
        /* Audio? */
        cx25840_write_09(client, 0x00, 0xAA);
        cx25840_write_09(client, 0x01, 0x4F);
        cx25840_write_09(client, 0x02, 0x01);
        cx25840_write_09(client, 0x03, 0x08);
        cx25840_write_09(client, 0x04, 0xAA);
        cx25840_write_09(client, 0x05, 0x4F);
        cx25840_write_09(client, 0x06, 0x01);
        cx25840_write_09(client, 0x07, 0x08);
        cx25840_write_09(client, 0x0C, 0xAA);
        cx25840_write_09(client, 0x0D, 0x4F);
        cx25840_write_09(client, 0x0E, 0x01);
        cx25840_write_09(client, 0x0F, 0x08);

        /* These are from i2c dumps, audio module? */
        cx25840_write_08(client, 0x10, 0x00);
        cx25840_write_08(client, 0x10, 0x01);
        cx25840_write_08(client, 0x03, 0x03);
        cx25840_write_08(client, 0x03, 0x13);

        /* These are from i2c dumps, audio module? */
        cx25840_write_08(client, 0x08, 0xF6);
        cx25840_write_08(client, 0x09, 0x00);
        cx25840_write_08(client, 0x0A, 0x01);
        cx25840_write_08(client, 0x0B, 0x00);

        cx25840_write_08(client, 0xD0, 0x70);
        cx25840_write_08(client, 0xD1, 0x38);
        cx25840_write_08(client, 0xD2, 0x06);
        cx25840_write_08(client, 0xD3, 0x1F);

        cx25840_write_08(client, 0x09, 0x04);
        cx25840_write_08(client, 0x10, 0x01);
#if 1
        cx25840_write(client, 0x27, 0x50);

        /* These are from i2c dumps, audio module? */
        cx25840_write(client, 0x08, 0x0F);
        cx25840_write(client, 0x09, 0x04);
        cx25840_write(client, 0x0A, 0x0A);
        cx25840_write(client, 0x0B, 0x20);

        cx25840_write(client, 0x10, 0xE5);
        cx25840_write(client, 0x11, 0xD6);
        cx25840_write(client, 0x12, 0x98);
        cx25840_write(client, 0x13, 0x00);
#endif
        /* Audio? */
        cx25840_write_09(client, 0x00, 0xAA);
        cx25840_write_09(client, 0x01, 0x4F);
        cx25840_write_09(client, 0x02, 0x01);
        cx25840_write_09(client, 0x03, 0x08);
        cx25840_write_09(client, 0x04, 0xAA);
        cx25840_write_09(client, 0x05, 0x4F);
        cx25840_write_09(client, 0x06, 0x01);
        cx25840_write_09(client, 0x07, 0x08);
        cx25840_write_09(client, 0x0C, 0xAA);
        cx25840_write_09(client, 0x0D, 0x4F);
        cx25840_write_09(client, 0x0E, 0x01);
        cx25840_write_09(client, 0x0F, 0x08);

        /* These are from i2c dumps, audio module? */
        cx25840_write_08(client, 0x10, 0x00);
}

/* ============ SAA7715 AUDIO settings (end) ============= */

static int cx25840_command (struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct cx25840_state *state = i2c_get_clientdata(client);

	switch (cmd) {

	case 0:
		break;
	/* ioctls to allow direct access to the cx25840 registers for testing */
	case DECODER_GET_REG:
	{
		struct cx25840_reg *reg = arg;

		reg->val = cx25840_read_08(client, reg->reg);
		break;
	}
	case DECODER_SET_REG:
	{
		struct cx25840_reg *reg = arg;

		cx25840_write_08(client, reg->reg, reg->val);
		break;
	}

	case DECODER_SET_SIZE:
	{
		/* Used video_window because it has height/width and is
		 * already defined */
		struct video_window *wind = arg;
		int HPSC, HFSC;
		int VSCY, Vsrc, FVsrc;
		int width = 0, height = 0;

		dprintk(1, "decoder set size\n");

		/* FIXME need better bounds checking here */
		if ( (wind->width < 1) || (wind->width > 1440))
			return -EINVAL;
		if ( (wind->height < 1) || (wind->height > 960))
			return -EINVAL;

		Vsrc = 480;
		FVsrc = 487;
		if (state->norm != VIDEO_MODE_NTSC) {
			Vsrc = 576;
			FVsrc = 583; /* FIXME, not sure if this is right */
		}

		width = ((720/wind->width)-1)*0x100000;
		height = 65536 - ((FVsrc/wind->width)-1)*512;
			
		if (wind->width != 720) {
			HPSC = (int) (720/wind->width);
			HFSC = (int) ((1024*720)/(HPSC*wind->width));
			dprintk(1, "Hsrc: %d, HPSC: 0x%05x\n", 
				wind->width, HPSC);
			/* width */
			cx25840_write_vid(client, 0x18, (u8)(width & 0xFF));
			cx25840_write_vid(client, 0x19, (u8)((width>>8)&0xFF));
			cx25840_write_vid(client, 0x1A, (u8)((width>>16)&0xFF));

		} else {
			if (state->norm != VIDEO_MODE_NTSC) {
				dprintk(2, "Setting full PAL width\n");
			} else {
				dprintk(2, "Setting full NTSC width\n");
			}
			cx25840_write_vid(client, 0x18, 0x00);
			cx25840_write_vid(client, 0x19, 0x00);
			cx25840_write_vid(client, 0x1A, 0x00);
		}
		
		if (wind->height != Vsrc) {
			VSCY = (int) ((1024*Vsrc)/wind->height);
			dprintk(1, "Vsrc: %d, Vscy: 0x%05x\n", Vsrc, VSCY);
			if (wind->height >= 385)
				cx25840_write_vid(client, 0x1E, (0x00|0x08));
			else if (wind->height >= 193)
                                cx25840_write_vid(client, 0x1E, (0x01|0x08));
                        else 
                                cx25840_write_vid(client, 0x1E, (0x03|0x08));

			/* height */
			cx25840_write_vid(client, 0x1C, (u8)(height & 0xFF));
			cx25840_write_vid(client, 0x1D, (u8)((height>>8)&0xFF));
		} else {
			if (state->norm != VIDEO_MODE_NTSC) {
				dprintk(2, "Setting full PAL height\n");
			} else {
				dprintk(2, "Setting full NTSC height\n");
			}
			/* height */
			cx25840_write_vid(client, 0x1C, 0x00);
			cx25840_write_vid(client, 0x1D, 0x00);
			cx25840_write_vid(client, 0x1E, 0x08);
		}

		break;
	}
	case DECODER_DUMP:
	{
		int i;

		printk(KERN_DEBUG "decoder dump\n");

		for (i = 0; i < 32; i += 16) {
			int j;

			printk(KERN_DEBUG "%s: %03x", client->name, i);
			for (j = 0; j < 16; ++j) {
				printk(" %02x", cx25840_read(client, i + j));
			}
			printk("\n");
		}
		break;
	}

	case DECODER_GET_CAPABILITIES:
	{
		struct video_decoder_capability *cap = arg;

		dprintk(1, "decoder get capabilities\n");

		cap->flags = VIDEO_DECODER_PAL |
			     VIDEO_DECODER_NTSC |
			     VIDEO_DECODER_SECAM |
			     VIDEO_DECODER_AUTO |
			     VIDEO_DECODER_CCIR;
		cap->inputs = 8;
		cap->outputs = 1;
		break;
	}

	case DECODER_SET_AUDIO:
	{
		int *iarg = arg;
		dprintk(1, "set audio: 0x%02x\n", *iarg);
		switch (*iarg) {
			case DECODER_AUDIO_32_KHZ:
				writeregs(client, cfg_cx25840_32_audio);
				if (state->norm == VIDEO_MODE_NTSC) {
					writeregs(client, cfg_cx25840_NTSC_32_audio);
				} else {
					writeregs(client, cfg_cx25840_PAL_32_audio);
				}
				break;
			case DECODER_AUDIO_441_KHZ:
				writeregs(client, cfg_cx25840_441_audio);
				if (state->norm == VIDEO_MODE_NTSC) {
					writeregs(client, cfg_cx25840_NTSC_441_audio);
				} else {
					writeregs(client, cfg_cx25840_PAL_441_audio);
				}
				break;
			case DECODER_AUDIO_48_KHZ:
				writeregs(client, cfg_cx25840_48_audio);
				if (state->norm == VIDEO_MODE_NTSC) {
					writeregs(client, cfg_cx25840_NTSC_48_audio);
				} else {
					writeregs(client, cfg_cx25840_PAL_48_audio);
				}
				break;
			default:
				dprintk(1, "invalid audio setting 0x%02x\n", *iarg);
		}

        	cx25840_write_vid(client, 0xA5, 0x80); /*Assert Video Reset */
        	cx25840_write_vid(client, 0xA5, 0x00); /*DeAssert Video Reset */

		state->audio = *iarg;
		break;
	}
	case DECODER_GET_STATUS:
	{
		int *iarg = arg;
		int status_cp;
		int status;
		int status2;
		int res;

		status_cp = cx25840_read_vid(client, 0x0C); /* CP */
		status = cx25840_read_vid(client, 0x0D); /* Gen */
		status2 = cx25840_read_vid(client, 0x0E); /* Gen2 */

		res = 0;
		if ((status2 & 1)) {
			res |= DECODER_STATUS_GOOD;
		}
		switch (state->norm) {
		case VIDEO_MODE_NTSC:
			res |= DECODER_STATUS_NTSC;
			break;
		case VIDEO_MODE_PAL:
			res |= DECODER_STATUS_PAL;
			break;
		case VIDEO_MODE_SECAM:
			res |= DECODER_STATUS_SECAM;
			break;
		default:
		case VIDEO_MODE_AUTO:
			if ((status & 1)) {
				res |= DECODER_STATUS_NTSC;
			} else {
				res |= DECODER_STATUS_PAL;
			}
			break;
		}
		if (status_cp != 0) {
			res |= DECODER_STATUS_COLOR;
		}
		*iarg = res;
		break;
	}

	case DECODER_SET_NORM:
	{
		int *iarg = arg;

		switch (*iarg) {
		case VIDEO_MODE_NTSC:
			dprintk(1, "decoder set norm NTSC\n");
			writeregs(client, cfg_cx25840_NTSC_video);
			break;

		case VIDEO_MODE_PAL:
			dprintk(1, "decoder set norm PAL\n");
			writeregs(client, cfg_cx25840_PAL_video);
			break;

		case VIDEO_MODE_SECAM:
			dprintk(1, "decoder set norm SECAM\n");
			writeregs(client, cfg_cx25840_PAL_video);
			break;

		default:
			dprintk(1, "Unknown video mode!!!\n");
			return -EINVAL;
		}

		state->norm = *iarg;

		/* switch audio mode too! */
		cx25840_command(client, DECODER_SET_AUDIO, &state->audio);
		break;
	}

	case DECODER_SET_INPUT:
	{
		int *iarg = arg;

		dprintk(1, "decoder set input (%d)\n", *iarg);
		/* inputs from 0-9 are available*/
		if (*iarg < 0 || *iarg > 9) { 
			return -EINVAL;
		}

		if (state->input != *iarg) {
			dprintk(1, "now setting %s input\n",
				*iarg < 0x02 ? "S-Video" : "Composite");
			state->input = *iarg;

			/* Composite or Tuner or S-Video */
			if (state->input >= 2 && state->input == 0x02) 
				state->input |= 0x40; 
			else if (state->input > 2) /* Tuner Input */
				state->input |= 0x40; /* was 0xE0, from datasheet */
			else if (state->input < 2) /* S-Video Input */
				state->input |= 0x10;

			/* input select */
			cx25840_write( client, 0x03, state->input);
		}
        	cx25840_write_vid(client, 0xA5, 0x80); /*Assert Video Reset */
        	cx25840_write_vid(client, 0xA5, 0x00); /*DeAssert Video Reset */
		break;
	}

	case DECODER_SET_OUTPUT:
	{
		int *iarg = arg;

		dprintk(1, "decoder set output\n");

		/* not much choice of outputs */
		if (*iarg != 0) {
			return -EINVAL;
		}
		break;
	}

	case DECODER_ENABLE_OUTPUT:
	{
		int *iarg = arg;
		int enable = (*iarg != 0);

		dprintk(1, "decoder %s output\n", enable ? "enable" : "disable");

		state->playback = !enable;

		if (state->enable != enable) {
			state->enable = enable;

			if (state->enable) {
				cx25840_write(client,0x15,0x8c);
				cx25840_write(client,0x16,0x07);
			} else {
				cx25840_write(client,0x15,0x00);
				cx25840_write(client,0x16,0x00);
			}
		}
        	cx25840_write_vid(client, 0xA5, 0x80); /*Assert Video Reset */
        	cx25840_write_vid(client, 0xA5, 0x00); /*DeAssert Video Reset */
		break;
	}

	case DECODER_GET_WSS:
	{
		int *iarg = arg;
		break; /* FIXME */

		u8 status = 0, wss1 = 0, wss2 = 0;

		//status = cx25840_read(client, 0x6b);
		if (status & 0xC0) {
			*iarg = -1;
			break;
		}
		//wss1 = cx25840_read(client, 0x6c);
		//wss2 = cx25840_read(client, 0x6d);
		*iarg = wss2 << 8 | wss1;
		break;
	}

	case DECODER_GET_CC_ODD:
	{
		int *iarg = arg;
		u8 status = 0, cc1 = 0, cc2 = 0;

		//status = cx25840_read(client, 0x66);
		if (status & 0xC0) {
			*iarg = -1;
			break;
		}
		//cc1 = cx25840_read(client, 0x67);
		//cc2 = cx25840_read(client, 0x68);
		*iarg = cc2 << 8 | cc1;
		break;
	}

	case DECODER_GET_CC_EVEN:
	{
		int *iarg = arg;
		u8 status = 0, cc1 = 0, cc2 = 0;

		//status = cx25840_read(client, 0x66);
		if (status & 0x30) {
			*iarg = -1;
			break;
		}
		//cc1 = cx25840_read(client, 0x69);
		//cc2 = cx25840_read(client, 0x6A);
		*iarg = cc2 << 8 | cc1;
		break;
	}

	case DECODER_SET_VBI:
	{
		int set = *(int *)arg;
		int is_pal = (state->norm != VIDEO_MODE_NTSC);
		int i;
		u8 lcr[22];

		for (i = 2; i <= 23; i++) lcr[i - 2] = 0xFF;
		
		if (set == 0) {
			if (is_pal) for (i = 6; i <= 23; i++) lcr[i - 2] = 0xDD;
			else for (i = 10; i <= 21; i++) lcr[i - 2] = 0xDD;
			for (i = 2; i <= 23; i++) {
				//cx25840_write(client, i - 2 + 0x41, lcr[i - 2]);
			}
			writeregs(client, cfg_cx25840_vbi_on);
			break;
		}

		if (set & IVTV_SLICED_TELETEXT_B) {
			if (is_pal) for (i = 6; i <= 22; i++) lcr[i - 2] = 0x11;
			else SAA7115_KERN_ERR("Teletext not supported for NTSC\n");
		}
		if (set & IVTV_SLICED_CAPTION_625) {
			if (is_pal) lcr[22 - 2] = 0x44;
			else SAA7115_KERN_ERR("PAL Closed Caption not supported for NTSC\n");
		}
		if (set & IVTV_SLICED_CAPTION_525) {
			if (!is_pal) {
				lcr[21 - 2] = 0x44;
			}
			else SAA7115_KERN_ERR("NTSC Closed Caption not supported for PAL\n");
		}
		if (set & IVTV_SLICED_WSS_625) {
			if (is_pal) lcr[23 - 2] = 0x5F;
			else SAA7115_KERN_ERR("WSS not supported for NTSC\n");
		}
		if (set & IVTV_SLICED_VPS) {
			if (is_pal) lcr[16 - 2] = 0x7F;
			else SAA7115_KERN_ERR("VPS not supported for NTSC\n");
		}
		for (i = 2; i <= 23; i++) {
			//cx25840_write(client, i - 2 + 0x41, lcr[i - 2]);
		}
		writeregs(client, cfg_cx25840_vbi_off);
		break;
	}

        case DECODER_GET_PICTURE:
        {
                struct cx25840_state *pic = arg;

                pic->bright = state->bright;
                pic->contrast = state->contrast;
                pic->sat = state->sat;
                pic->hue = state->hue;
                break;
        }

	case DECODER_SET_PICTURE:
	{
		struct cx25840_state *pic = arg;

		if (state->bright != pic->bright) {
			/* We want 0 to 255 */
			if (pic->bright < 0 || pic->bright > 255) {
				SAA7115_KERN_ERR("invalid brightness setting %d", 
					pic->bright);
				return -EINVAL;
			}
			state->bright = pic->bright;
			if (pic->bright > 128) { /* 0 default for chip */
				cx25840_write_vid(client, 
					0x14, (pic->bright-128));
			} else {
				/* FIXME need to adjust luma? */
				cx25840_write_vid(client, 0x14, 0x00);
			}
		}
		if (state->contrast != pic->contrast) {
			/* We want 0 to 127 */
			if (pic->contrast < 0 || pic->contrast > 127) {
				SAA7115_KERN_ERR("invalid contrast setting %d", 
					pic->contrast);
				return -EINVAL;
			}
			state->contrast = pic->contrast;
			cx25840_write_vid(client, 0x15, (pic->contrast*2));
		}
		if (state->sat != pic->sat) {
			/* We want 0 to 127 */
			if (pic->sat < 0 || pic->sat > 127) {
				SAA7115_KERN_ERR("invalid saturation setting %d", 
					pic->sat);
				return -EINVAL;
			}
			state->sat = pic->sat;
			cx25840_write_vid(client, 0x20, (pic->sat*2)); /* U */
			cx25840_write_vid(client, 0x21, (pic->sat*2)); /* V */
		}
		if (state->hue != pic->hue) {
			/* We want -128 to 127 */
			if (pic->hue < -127 || pic->hue > 127) {
				SAA7115_KERN_ERR("invalid hue setting %d", pic->hue);
				return -EINVAL;
			}
			state->hue = pic->hue;
			cx25840_write_vid(client, 0x22, pic->hue);
		}
		dprintk(1,
			"decoder set picture bright=%d contrast=%d "
			"saturation=%d hue=%d\n",
			state->bright, state->contrast,
			state->sat, state->hue);
		dprintk(1,
			"decoder set picture bright=%d contrast=%d "
			"saturation_u=%d saturation_v=%d hue=%d\n",
			cx25840_read_vid(client, 0x14), 
			cx25840_read_vid(client, 0x15),
			cx25840_read_vid(client, 0x20),
			cx25840_read_vid(client, 0x21),
			cx25840_read_vid(client, 0x22));
		break;
	}
	case DECODER_RESET:
	{
		dprintk(1, "decoder RESET ");

        	cx25840_write_vid(client, 0xA5, 0x80); /*Assert Video Reset */
        	cx25840_write_vid(client, 0xA5, 0x00); /*DeAssert Video Reset */
		writeregs(client, cfg_cx25840_reset_scaler);
		break;
	}

	default:
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */


/* i2c implementation */
#ifndef NEW_I2C
  /* pre i2c-2.8.0 */

static void
cx25840_inc_use (struct i2c_client *client)
{
  #ifdef MODULE
	MOD_INC_USE_COUNT;
  #endif
}

static void
cx25840_dec_use (struct i2c_client *client)
{
  #ifdef MODULE
	MOD_DEC_USE_COUNT;
  #endif
}

#else
	/* i2c-2.8.0 and later */
#endif

#define I2C_CX25840_ADDRESS  0x88

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] = { I2C_CX25840_ADDRESS >> 1, I2C_CLIENT_END };
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };

I2C_CLIENT_INSMOD;

static int cx25840_i2c_id = 0;
struct i2c_driver i2c_driver_cx25840;

static int
cx25840_detect_client (struct i2c_adapter *adapter,
		       int                 address,
#ifndef LINUX26                       
		       unsigned short      flags,
#endif                       
		       int                 kind)
{
	struct i2c_client *client;
	struct cx25840_state *state;

	if (i2c_enable[cx25840_i2c_id] == -1) {
		dprintk(1, "client id: cx25840_i2c_id %d, skipped\n",
			cx25840_i2c_id);
		cx25840_i2c_id++;
		return 0;
	}

	dprintk(1, "detecting client on address 0x%x\n",
		address << 1);

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return -ENOMEM;
	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver_cx25840;
	client->flags = I2C_CLIENT_ALLOW_USE;
	client->id = cx25840_i2c_id++;
	snprintf(client->name, sizeof(client->name) - 1, "cx25840[%d]",
		 client->id);

	state = kmalloc(sizeof(struct cx25840_state), GFP_KERNEL);
        i2c_set_clientdata(client, state); 
	if (state == NULL) {
		kfree(client);
		return -ENOMEM;
	}
	memset(state, 0, sizeof(struct cx25840_state));
	state->norm = VIDEO_MODE_NTSC;
	state->input = -1;
	state->enable = 1;
	state->bright = 128;
	state->contrast = 64;
	state->sat = 64;
	state->hue = 0;
	state->playback = 0;	// initially capture mode used
	state->audio = DECODER_AUDIO_48_KHZ;

	dprintk(1, "writing init values\n");

	/* Audio Setup */
	cx25840_audio(client);

	/* put chip to sleep, and wake back up */
	cx25840_reset(client); /* Initialize and Reset Chip*/

	/* Input setup and reset digitizer */
	writeregs(client, init_cx25840_auto_input);
	writeregs(client, cfg_cx25840_reset_scaler);

	i2c_attach_client(client);

	dprintk(1, "status: (0x40C) 0x%02x (0x40D) 0x%02x, (0x40E) 0x%02x\n",
		cx25840_read_vid(client, 0x0C), /* CP */
		cx25840_read_vid(client, 0x0D), /* Gen */
		cx25840_read_vid(client, 0x0E)); /* Gen2 */

	return 0;
}

static int
cx25840_attach_adapter (struct i2c_adapter *adapter)
{
	dprintk(1, "starting probe for adapter %s (0x%x)\n",
		adapter->name, adapter->id);
	return i2c_probe(adapter, &addr_data, &cx25840_detect_client);
	return 0;
}

static int
cx25840_detach_client (struct i2c_client *client)
{
	struct cx25840_state *state = i2c_get_clientdata(client);
	int err;

	err = i2c_detach_client(client);
	if (err) {
		return err;
	}

	kfree(state);
	kfree(client);
	return 0;
}

/* ----------------------------------------------------------------------- */

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
#endif
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif

static int __init
cx25840_init (void)
{
	return i2c_add_driver(&i2c_driver_cx25840);
}

static void __exit
cx25840_exit (void)
{
	i2c_del_driver(&i2c_driver_cx25840);
}

module_init(cx25840_init);
module_exit(cx25840_exit);
