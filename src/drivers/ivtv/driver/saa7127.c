/*
 * saa7127 - Philips SAA7127 video encoder driver version 0.2
 *
 * Copyright (C) 2003 Roy Bulter <rbulter@hetnet.nl>
 *
 * Based on SAA7126 video encoder driver by Gillem & Andreas Oberritter
 *
 * Copyright (C) 2000-2001 Gillem <htoa@gmx.net>
 * Copyright (C) 2002 Andreas Oberritter <obi@saftware.de>
 *
 * Based on Stadis 4:2:2 MPEG-2 Decoder Driver by Nathan Laredo
 *
 * Copyright (C) 1999 Nathan Laredo <laredo@gnu.org>
 *
 * This driver is designed for the Hauppauge 250/350 Linux driver
 * designed by the Ivytv Project (ivtv.sf.net) 
 *  
 * Copyright (C) 2003 Kevin Thayer <nufan_wfk@yahoo.com>
 *
 * VBI additions:
 * Copyright (C) 2004 Hans Verkuil <hverkuil@xs4all.nl>
 *
 * Dual output support:
 * Copyright (C) 2004 Eric Varsanyi
 *
 * NTSC Tuning and 7.5 IRE Setup
 * Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org
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

/*
 * Revision History
 *
 *
 * Revision : 0.1 (09-05-2003)
 * Change   : First Version
 *
 * Revision : 0.2 (21-05-2003)
 * Change   : solved compiler error on line 785(800) 
 *            reg61h variable was not set in saa7127_set_norm function
 *
 * Revision : 0.3 (21-07-2003) Matt T. Yourst <yourst@yourst.com>
 * Change   : Update configuration tables to make NTSC appear correctly;
 *            Enable alternative outputs (s-video, composite, RGB, etc.)
 */

#include <linux/version.h>

#include <linux/module.h>
#include <linux/delay.h>
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
#include <asm/segment.h>
#include <linux/types.h>

#include "compat.h"
#ifndef LINUX26
#include <linux/wrapper.h>
#endif

#include <linux/version.h>
#include <asm/uaccess.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/videodev.h>
#include <linux/video_encoder.h>

#include "saa7127.h"

#ifndef I2C_DRIVERID_SAA7127
  // Using temporary hack for missing I2C driver-ID for saa7127
  #define I2C_DRIVERID_SAA7127 I2C_DRIVERID_EXP2
#endif

/*
 **********************************************************************
 *  Debug Macro's
 *
 *
 **********************************************************************
 */

#define dprintk(num, args...) \
	do { \
		if (debug >= num) \
			printk(KERN_INFO "saa7127: " args); \
	} while (0)

#define SAA7127_KERN_ERR(args...) printk(KERN_ERR "saa7127: " args);
#define SAA7127_KERN_INFO(args...) printk(KERN_INFO "saa7127: " args);

static int debug = 1;
static int test_image = 0;

/*
 **********************************************************************
 *  
 *  Array's with configuration parameters for the SAA7127
 *
 **********************************************************************
 */

struct i2c_reg_value 
{
  unsigned char reg;
  unsigned char value;
};

struct i2c_reg_value saa7129_init_config_extra[] =
{
  {SAA7127_REG_OUTPUT_PORT_CONTROL,             0x38},
  {SAA7127_REG_VTRIG,                           0xfa},
};

struct i2c_reg_value saa7127_init_config_common[] = 
{
  {SAA7127_REG_WIDESCREEN_CONFIG,               0x0d},
  {SAA7127_REG_WIDESCREEN_ENABLE,               0x00},
  {SAA7127_REG_COPYGEN_0,                       0x77},
  {SAA7127_REG_COPYGEN_1,                       0x41},
  {SAA7127_REG_COPYGEN_2,                       0x00}, // (Macrovision enable/disable)
  {SAA7127_REG_OUTPUT_PORT_CONTROL,             0x9e},
  {SAA7127_REG_GAIN_LUMINANCE_RGB,              0x00},
  {SAA7127_REG_GAIN_COLORDIFF_RGB,              0x00},
  {SAA7127_REG_INPUT_PORT_CONTROL_1,            0x80}, // (for color bars)
  {SAA7127_REG_LINE_21_ODD_0,                   0x77},
  {SAA7127_REG_LINE_21_ODD_1,                   0x41},
  {SAA7127_REG_LINE_21_EVEN_0,                  0x88},
  {SAA7127_REG_LINE_21_EVEN_1,                  0x41},
  {SAA7127_REG_RCV_PORT_CONTROL,                0x12},
  {SAA7127_REG_VTRIG,                           0xf9},
  {SAA7127_REG_HTRIG_HI,                        0x00},
  {SAA7127_REG_RCV2_OUTPUT_START,               0x41},
  {SAA7127_REG_RCV2_OUTPUT_END,                 0xc3},
  {SAA7127_REG_RCV2_OUTPUT_MSBS,                0x00},
  {SAA7127_REG_TTX_REQUEST_H_START,             0x3e},
  {SAA7127_REG_TTX_REQUEST_H_DELAY_LENGTH,      0xb8},
  {SAA7127_REG_CSYNC_ADVANCE_VSYNC_SHIFT,       0x03},
  {SAA7127_REG_TTX_ODD_REQ_VERT_START,          0x15},
  {SAA7127_REG_TTX_ODD_REQ_VERT_END,            0x16},
  {SAA7127_REG_TTX_EVEN_REQ_VERT_START,         0x15},
  {SAA7127_REG_TTX_EVEN_REQ_VERT_END,           0x16},
  {SAA7127_REG_FIRST_ACTIVE,                    0x1a},
  {SAA7127_REG_LAST_ACTIVE,                     0x01},
  {SAA7127_REG_MSB_VERTICAL,                    0xc0},
  {SAA7127_REG_DISABLE_TTX_LINE_LO_0,           0x00},
  {SAA7127_REG_DISABLE_TTX_LINE_LO_1,           0x00},
  {0, 0}
};

#define SAA7127_NTSC_DAC_CONTROL 0x15
struct i2c_reg_value saa7127_init_config_ntsc[] = 
{
  {SAA7127_REG_BURST_START,                     0x19},
  {SAA7127_REG_BURST_END,                       0x1d},
  {SAA7127_REG_CHROMA_PHASE,                    0xA3},
  {SAA7127_REG_GAINU,                           0x98},
  {SAA7127_REG_GAINV,                           0xd3},
  {SAA7127_REG_BLACK_LEVEL,                     0x39},
  {SAA7127_REG_BLANKING_LEVEL,                  0x2e},
  {SAA7127_REG_VBI_BLANKING,                    0x2e},
  {SAA7127_REG_DAC_CONTROL,                     0x15},
  {SAA7127_REG_BURST_AMP,                       0x4d},
  {SAA7127_REG_SUBC3,                           0x1f},
  {SAA7127_REG_SUBC2,                           0x7c},
  {SAA7127_REG_SUBC1,                           0xf0},
  {SAA7127_REG_SUBC0,                           0x21},
  {SAA7127_REG_MULTI,                           0x90},
  {SAA7127_REG_CLOSED_CAPTION,                  0x11},
  {0, 0}
};

#define SAA7127_PAL_DAC_CONTROL 0x02
struct i2c_reg_value saa7127_init_config_pal[] = 
{
  {SAA7127_REG_BURST_START,                     0x21},
  {SAA7127_REG_BURST_END,                       0x1d},
  {SAA7127_REG_CHROMA_PHASE,                    0x3f},
  {SAA7127_REG_GAINU,                           0x7d},
  {SAA7127_REG_GAINV,                           0xaf},
  {SAA7127_REG_BLACK_LEVEL,                     0x33},
  {SAA7127_REG_BLANKING_LEVEL,                  0x35},
  {SAA7127_REG_VBI_BLANKING,                    0x35},
  {SAA7127_REG_DAC_CONTROL,                     0x02},
  {SAA7127_REG_BURST_AMP,                       0x2f},
  {SAA7127_REG_SUBC3,                           0xcb}, 
  {SAA7127_REG_SUBC2,                           0x8a},
  {SAA7127_REG_SUBC1,                           0x09},
  {SAA7127_REG_SUBC0,                           0x2a},
  {SAA7127_REG_MULTI,                           0xa0},
  {SAA7127_REG_CLOSED_CAPTION,                  0x00},
  {0, 0}
};

/*
 **********************************************************************
 *  
 *  Encoder Struct, holds the configuration state of the encoder
 *
 **********************************************************************
 */

struct saa7127
{
	enum saa7127_video_norm       norm;
	enum saa7127_input_type       input_type;
	enum saa7127_output_type      output_type;
	enum saa7127_enable_type      enable;
	enum saa7127_wss_enable_type  wss_enable;
	enum saa7127_wss_mode_type    wss_mode;
	enum saa7127_cc_enable_type   cc_enable;
	u32 cc_data;
	enum saa7127_vps_enable_type  vps_enable;
	u8 vps_data[5];
	u8 reg_2d;
	u8 reg_3a;
	u8 reg_61;
};

/* ----------------------------------------------------------------------- */

static int saa7127_read (struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}


/* ----------------------------------------------------------------------- */


static int saa7127_writereg(struct i2c_client *client, u8 reg, u8 val)
{
	int i;

	for (i = 0; i < 3; i++) {
		if (i2c_smbus_write_byte_data(client, reg, val) == 0)
			return 0;
	}
	SAA7127_KERN_ERR("I2C Write Problem\n");
	return -1;
}


/* ----------------------------------------------------------------------- */

static int saa7127_write_inittab(struct i2c_client *client, const struct i2c_reg_value *regs)
{
	while (regs->reg != 0) {
		saa7127_writereg(client, regs->reg, regs->value);
		regs++;
	}
	return 0;
}


/* ----------------------------------------------------------------------- */


static int saa7127_set_vps(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	dprintk(1, "Turn VPS Signal %s\n", encoder->vps_enable ? "on" : "off");
	saa7127_writereg(client, 0x54, (encoder->vps_enable << 7));
	return 0;
}


/* ----------------------------------------------------------------------- */


static int saa7127_set_vps_data(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	dprintk(1, "Set VPS data %02x %02x %02x %02x %02x\n",
			encoder->vps_data[0], encoder->vps_data[1],
			encoder->vps_data[2], encoder->vps_data[3],
			encoder->vps_data[4]);
	saa7127_writereg(client, 0x55, encoder->vps_data[0]);
	saa7127_writereg(client, 0x56, encoder->vps_data[1]);
	saa7127_writereg(client, 0x57, encoder->vps_data[2]);
	saa7127_writereg(client, 0x58, encoder->vps_data[3]);
	saa7127_writereg(client, 0x59, encoder->vps_data[4]);
	return 0;
}


/* ----------------------------------------------------------------------- */

/* ----------------------------------------------------------------------- */


static int saa7127_set_cc_data(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);
	int line = 0x11;

	dprintk(2, "CC data %x: %08x\n", encoder->cc_enable, encoder->cc_data);
	if (encoder->norm == SAA7127_VIDEO_NORM_PAL) {
		line = 0x15;
	}
	saa7127_writereg(client, SAA7127_REG_CLOSED_CAPTION, (encoder->cc_enable << 6) | line);
	saa7127_writereg(client, SAA7127_REG_LINE_21_ODD_0, (encoder->cc_data & 0xff));
	saa7127_writereg(client, SAA7127_REG_LINE_21_ODD_1, ((encoder->cc_data >> 8) & 0xff));
	saa7127_writereg(client, SAA7127_REG_LINE_21_EVEN_0, ((encoder->cc_data >> 16) & 0xff));
	saa7127_writereg(client, SAA7127_REG_LINE_21_EVEN_1, ((encoder->cc_data >> 24) & 0xff));
	return 0;
}


/* ----------------------------------------------------------------------- */


static int saa7127_set_wss(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	dprintk(1, "Turn WSS %s\n", encoder->wss_enable ? "on" : "off");
	switch (encoder->wss_enable) {
		case SAA7127_WSS_DISABLE:
			saa7127_writereg(client, 0x27, 0x00);
			break;
		case SAA7127_WSS_ENABLE:
			saa7127_writereg(client, 0x27, 0x80);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}



/* ----------------------------------------------------------------------- */


static int saa7127_set_wss_mode(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	switch (encoder->wss_mode) {
		case SAA7127_WSS_MODE_4_3_FULL_FORMAT:
			dprintk(1, "Widescreen Mode 4:3 Full Format\n");
			saa7127_writereg(client, 0x26, 0x08);
			break;
		case SAA7127_WSS_MODE_BOX_14_9_C:
			dprintk(1, "Widescreen Mode Box 14:9 Center\n");
			saa7127_writereg(client, 0x26, 0x01);
			break;
		case SAA7127_WSS_MODE_BOX_14_9_TOP:
			dprintk(1, "Widescreen Mode Box 14:9 Top\n");
			saa7127_writereg(client, 0x26, 0x02);
			break;
		case SAA7127_WSS_MODE_BOX_16_9_C:
			dprintk(1, "Widescreen Mode Box 16:9 Center\n");
			saa7127_writereg(client, 0x26, 0x0b);
			break;
		case SAA7127_WSS_MODE_BOX_16_9_TOP:
			dprintk(1, "Widescreen Mode Box 16:9 Top\n");
			saa7127_writereg(client, 0x26, 0x04);
			break;
		case SAA7127_WSS_MODE_SMALL_BOX_16_9_C:
			dprintk(1, "Widescreen Mode Small Box 16:9 Center\n");
			saa7127_writereg(client, 0x26, 0x0d);
			break;
		case SAA7127_WSS_MODE_4_3_14_9_FULL_FORMAT:
			dprintk(1, "Widescreen Mode 14:9 Full Format\n");
			saa7127_writereg(client, 0x26, 0x0e);
			break;
		case SAA7127_WSS_MODE_16_9_ANAMORPHIC:
			dprintk(1, "Widescreen Mode 16:9 Full Format\n");
			saa7127_writereg(client, 0x26, 0x07);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}



/* ----------------------------------------------------------------------- */


static int saa7127_set_enable (struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	switch (encoder->enable)
	{
		case SAA7127_DISABLE:
			dprintk(1, "Disable Video Output\n");
			saa7127_writereg(client, 0x2d, (encoder->reg_2d & 0xf0));
			saa7127_writereg(client, 0x61, (encoder->reg_61 | 0xc0));
			break;
		case SAA7127_ENABLE:
			dprintk(1, "Enable Video Output\n");
			saa7127_writereg(client, 0x2d, encoder->reg_2d);
			saa7127_writereg(client, 0x61, encoder->reg_61);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}


/* ----------------------------------------------------------------------- */

static int saa7127_set_norm(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);
	const struct i2c_reg_value *inittab;

	switch (encoder->norm) {
		case SAA7127_VIDEO_NORM_NTSC:
			dprintk(1, "Selecting NTSC video Standard\n");
			inittab = saa7127_init_config_ntsc;
			encoder->reg_61 = SAA7127_NTSC_DAC_CONTROL;
			break;
		case SAA7127_VIDEO_NORM_PAL:
			dprintk(1, "Selecting PAL video Standard\n");
			inittab = saa7127_init_config_pal;
			encoder->reg_61 = SAA7127_PAL_DAC_CONTROL;
			break;
		default:
			return -EINVAL;
	}

	/* Write Table */
	saa7127_write_inittab(client, inittab);
	return 0;
}

/* ----------------------------------------------------------------------- */

static int saa7127_set_output_type(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	encoder->reg_3a = 0x13;   // by default swithch YUV to RGB-matrix on

	switch (encoder->output_type)
	{
		case SAA7127_OUTPUT_TYPE_RGB:
			dprintk(1, "Selecting RGB Output Type\n");
			encoder->reg_2d = 0x0f; // RGB + CVBS (for sync)
			break;
		case SAA7127_OUTPUT_TYPE_COMPOSITE:
			dprintk(1, "Selecting Composite Output Type\n");
			encoder->reg_2d = 0x08; // 00001000 CVBS only, RGB DAC's off (high impedance mode) !!!
			break;
		case SAA7127_OUTPUT_TYPE_SVIDEO:
			dprintk(1, "Selecting S-Video Output Type\n");      
			encoder->reg_2d = 0xff; // 11111111  croma -> R, luma -> CVBS + G + B
			break;
		case SAA7127_OUTPUT_TYPE_YUV_V:
			dprintk(1, "Selecting YUV V Output Type\n");        
			encoder->reg_2d = 0x4f; // reg 2D = 01001111, all DAC's on, RGB + VBS
			encoder->reg_3a = 0x0b; // reg 3A = 00001011, bypass RGB-matrix
			break;
		case SAA7127_OUTPUT_TYPE_YUV_C:
			dprintk(1, "Selecting YUV C Output Type\n");
			encoder->reg_2d = 0x0f; // reg 2D = 00001111, all DAC's on, RGB + CVBS
			encoder->reg_3a = 0x0b; // reg 3A = 00001011, bypass RGB-matrix
			break;
		case SAA7127_OUTPUT_TYPE_BOTH:
			dprintk(1, "Selecting S-Video+Composite\n");
			encoder->reg_2d = 0xbf;
			break;
		default:
			return -EINVAL;
	}

	/* Configure Encoder */

	saa7127_writereg(client, 0x2d, encoder->reg_2d);
	saa7127_writereg(client, 0x3a, (encoder->input_type == SAA7127_INPUT_TYPE_TEST_IMAGE) ? 0x80 : encoder->reg_3a);
	return 0;
}


static int saa7127_set_input_type(struct i2c_client *client)
{
	struct saa7127 *encoder = (struct saa7127 *) i2c_get_clientdata(client);

	switch (encoder->input_type)
	{
		case SAA7127_INPUT_TYPE_NORMAL:         /* avia */
			dprintk(1, "Selecting Normal Encoder Input\n");          
			saa7127_writereg(client, 0x3a, encoder->reg_3a);
			break;
		case SAA7127_INPUT_TYPE_TEST_IMAGE:     /* color bar */
			dprintk(1, "Selecting Colour Bar generator\n");
			saa7127_writereg(client, 0x3a, 0x80);
			break;
		default:
			return -EINVAL;
	}
	return 0;
}


/* ----------------------------------------------------------------------- */


static int saa7127_command (struct i2c_client *client,
                            unsigned int       cmd,
                            void              *arg)
{
	struct saa7127 *encoder = i2c_get_clientdata(client);
	int *iarg = arg;
	struct video_encoder_capability *cap = arg;

	switch (cmd) {
	case ENCODER_GET_CAPABILITIES:
		dprintk(1, "Asking Encoder Capabilities\n");
		cap->flags = VIDEO_ENCODER_PAL | VIDEO_ENCODER_NTSC;
		cap->inputs = 1;
		cap->outputs = 1;
		break;

	case ENCODER_SET_NORM:
		dprintk(1, "Setting Encoder Video Standard\n");
		switch (*iarg) {
		case VIDEO_MODE_NTSC:
			dprintk(1, "Set NTSC Video Mode\n");    
			encoder->norm = SAA7127_VIDEO_NORM_NTSC;
			break;
		case VIDEO_MODE_PAL:
		case VIDEO_MODE_SECAM:
			dprintk(1, "Set PAL Video Mode\n");        
			encoder->norm = SAA7127_VIDEO_NORM_PAL;
			break;
		default:
			return -EINVAL;
		}
		saa7127_set_norm(client);
		break;

	case ENCODER_SET_INPUT:
		dprintk(1, "Setting Encoder Input\n");
		switch (*iarg) {
		case SAA7127_INPUT_NORMAL:    /* encoder input selected */
			dprintk(1, "Select Normal input\n");        
			encoder->input_type = SAA7127_INPUT_TYPE_NORMAL;
			break;
		case SAA7127_INPUT_TESTIMAGE: /* Internal colourbars selected */
			dprintk(1, "Select ColourBar Generator\n");              
			encoder->input_type = SAA7127_INPUT_TYPE_TEST_IMAGE;
			break;
		default:
			return -EINVAL;
		}
		saa7127_set_input_type (client);
		break;

	case ENCODER_SET_OUTPUT:
		dprintk(1, "Setting Encoder Output\n");
		encoder->output_type = *iarg;
		saa7127_set_output_type(client);
		break;

	case ENCODER_ENABLE_OUTPUT:
		dprintk(1, "Turn on/off Output\n");    
		switch (*iarg) {     
		case SAA7127_VIDEO_ENABLE:
			dprintk(1,  "Turn on Video Output\n");
			encoder->enable = SAA7127_ENABLE;
			break;
		case SAA7127_VIDEO_DISABLE:
			dprintk(1, "Turn off Video Output\n");      
			encoder->enable = SAA7127_DISABLE;
			break;
		default:
			return -EINVAL;
		}
		saa7127_set_enable(client);
		break;

	case ENCODER_ENABLE_WSS: {
		int enable = *iarg ? SAA7127_WSS_ENABLE : SAA7127_WSS_DISABLE;

		dprintk(2, "Turn WSS output %s\n", *iarg ? "on" : "off");   
		if (enable == encoder->wss_enable) break;
		encoder->wss_enable = enable;
		saa7127_set_wss(client);
		break;
	}

	case ENCODER_SET_WSS_MODE:
		dprintk(2, "Set WSS Mode\n");   
		if (encoder->wss_mode == *iarg) break;
		encoder->wss_mode = *iarg;
		saa7127_set_wss_mode(client);
		break;

	case ENCODER_ENABLE_CC:
		dprintk(2, "Turn CC output %s\n", *iarg ? "on" : "off");   
		encoder->cc_enable = *iarg;
		break;

	case ENCODER_SET_CC_DATA:
		dprintk(2, "Set CC Data\n");   
		encoder->cc_data = *iarg;
		saa7127_set_cc_data(client);
		break;

	case ENCODER_ENABLE_VPS:
		dprintk(2, "Turn VPS output %s\n", *iarg ? "on" : "off");   
		if (encoder->vps_enable == *iarg) break;
		encoder->vps_enable = *iarg;
		saa7127_set_vps(client);
		break;

	case ENCODER_SET_VPS_DATA:
		dprintk(1, "Set VPS Data\n");   
		if (!memcmp(encoder->vps_data, ((struct saa7127_vps_data *)arg)->data, 5)) {
			break;
		}
		memcpy(encoder->vps_data, ((struct saa7127_vps_data *)arg)->data, 5);
		saa7127_set_vps_data(client);
		break;

		/* ioctls to allow direct access to the saa7127 registers for testing */
	case ENCODER_GET_REG: {
			struct saa7127_reg *reg = arg;

			reg->val = saa7127_read(client, reg->reg);
			break;
		}
	case ENCODER_SET_REG: {
			struct saa7127_reg *reg = arg;

			saa7127_writereg(client, reg->reg, reg->val);
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

static void saa7127_inc_use (struct i2c_client *client)
{
  #ifdef MODULE
    MOD_INC_USE_COUNT;
  #endif
}


/* ----------------------------------------------------------------------- */

static void saa7127_dec_use (struct i2c_client *client)
{
  #ifdef MODULE
    MOD_DEC_USE_COUNT;
  #endif
}

#else
    /* ver >= i2c 2.8.0 */
#endif


/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */

static unsigned short normal_i2c[]        = { I2C_SAA7127_ADDRESS >> 1,I2C_CLIENT_END };
static unsigned short normal_i2c_range[]  = { I2C_CLIENT_END };

I2C_CLIENT_INSMOD;

static int saa7127_i2c_id = 0;
struct i2c_driver i2c_driver_saa7127;


/* ----------------------------------------------------------------------- */



static int saa7127_detect_client (struct i2c_adapter *adapter,
                                  int                 address,
#ifndef LINUX26
                                  unsigned short      flags,
#endif
                                  int                 kind)
{
	struct i2c_client *client;
	struct saa7127 *encoder;
	int read_result = 0;

	dprintk(2,"detecting saa7127 client on address 0x%x\n", address << 1);

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return (0);

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return (-ENOMEM);

	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver_saa7127;
	client->flags = I2C_CLIENT_ALLOW_USE;
	client->id = saa7127_i2c_id++;
	snprintf(client->name, sizeof(client->name) - 1, "saa7127[%d]", client->id);

	encoder = kmalloc(sizeof(struct saa7127), GFP_KERNEL);

	if (encoder == NULL) {
		kfree(client);
		return (-ENOMEM);
	}

	i2c_set_clientdata(client, encoder);
	memset(encoder, 0, sizeof(struct saa7127));

	/* Initialize default values */
	encoder->output_type  = SAA7127_OUTPUT_TYPE_BOTH;
	encoder->wss_enable   = SAA7127_WSS_DISABLE;
	encoder->wss_mode     = SAA7127_WSS_MODE_4_3_FULL_FORMAT;

	/* Look if the pal module parameter is set */

	/* Select NTSC Video Standard, default */  
	encoder->norm         = SAA7127_VIDEO_NORM_NTSC;

	/* Enable the Encoder */
	encoder->enable = SAA7127_ENABLE;

	/* The Encoder is does have internal Colourbar generator */
	/* This can be used for debugging, configuration values for the encoder */

	if (test_image == 1) {
		/* Select ColourBar Generator */
		encoder->input_type = SAA7127_INPUT_TYPE_TEST_IMAGE;
	} else {
		/* Select normal input */
		encoder->input_type = SAA7127_INPUT_TYPE_NORMAL;
	}

	dprintk(2, "Writing init values\n");

	/* Configure Encoder */

	dprintk(2, "Configuring encoder\n");
	saa7127_write_inittab(client, saa7127_init_config_common);
	saa7127_set_norm(client);
	saa7127_set_output_type (client);
	saa7127_set_wss (client);
	saa7127_set_wss_mode (client);
	saa7127_set_input_type (client);
	saa7127_set_enable (client);

	/* Detect if it's an saa7129 */
	read_result = saa7127_read(client, SAA7129_REG_FADE_KEY_COL2);
	saa7127_writereg(client, SAA7129_REG_FADE_KEY_COL2, 0xAA);
	if (saa7127_read(client, SAA7129_REG_FADE_KEY_COL2) == 0xAA) {
		saa7127_writereg(client, 
			SAA7129_REG_FADE_KEY_COL2, read_result);	
		saa7127_write_inittab(client, saa7129_init_config_extra);
	}

	read_result = saa7127_read(client, 0x4F);
	dprintk(2, "Read status register (00h): 0x%02x\n", read_result);
		
	i2c_attach_client(client);

	return 0;
}


/* ----------------------------------------------------------------------- */


static int saa7127_attach_adapter (struct i2c_adapter *adapter)
{
	dprintk(2, "starting probe for adapter %s (0x%x)\n", adapter->name, adapter->id);
	return i2c_probe(adapter, &addr_data, &saa7127_detect_client);
}


/* ----------------------------------------------------------------------- */


static int saa7127_detach_client (struct i2c_client *client)
{
  struct saa7127 *encoder = i2c_get_clientdata(client);
  int err;
  
  /* Turn off TV output */

  encoder->enable       = SAA7127_DISABLE;

  /* Make sure this turns off output */
  saa7127_writereg(client, 0x2d, (encoder->reg_2d & 0xf0));
  saa7127_writereg(client, 0x61, (encoder->reg_61 | 0xc0));

  err = i2c_detach_client(client);

  if (err)
  {
    return (err);
  }

  kfree(encoder);
  kfree(client);
  return (0);
}

/* ----------------------------------------------------------------------- */


struct i2c_driver i2c_driver_saa7127 =
{
  .name           = "saa7127",
  .id             = I2C_DRIVERID_SAA7127,
  .flags          = I2C_DF_NOTIFY,
  .attach_adapter = saa7127_attach_adapter,
  .detach_client  = saa7127_detach_client,
  .command        = saa7127_command,
#ifndef NEW_I2C
  .inc_use        = saa7127_inc_use,
  .dec_use        = saa7127_dec_use,
#else
    /* ver >= i2c 2.8.0 */
   .owner          = THIS_MODULE,
#endif
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif

/* ----------------------------------------------------------------------- */


static int __init saa7127_init (void)
{
  SAA7127_KERN_INFO("video encoder driver version V %s loaded\n", SAA7127_DRIVER_VERSION);
  return (i2c_add_driver(&i2c_driver_saa7127));
}



/* ----------------------------------------------------------------------- */


static void __exit saa7127_exit (void)
{
  SAA7127_KERN_INFO("video encoder driver unloaded\n");
  i2c_del_driver(&i2c_driver_saa7127);
}



/* ----------------------------------------------------------------------- */


module_init(saa7127_init);
module_exit(saa7127_exit);


MODULE_DESCRIPTION("Philips SAA7127 video encoder driver");
MODULE_AUTHOR("Roy Bulter");
MODULE_LICENSE("GPL");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(debug, int, 0644);
module_param(test_image, int, 0644);
#else
MODULE_PARM(debug, "i");
MODULE_PARM(test_image, "i");
#endif
MODULE_PARM_DESC(debug, "debug level (0-2) ");
MODULE_PARM_DESC(test_image, "test_image (0-1) ");

