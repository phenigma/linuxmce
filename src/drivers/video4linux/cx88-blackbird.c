/*
 * $Id: cx88-blackbird.c,v 1.25 2005/02/16 13:11:55 kraxel Exp $
 *
 *  Support for a cx23416 mpeg encoder via cx2388x host port.
 *  "blackbird" reference design.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.8m.com>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *
 *  Includes parts from the ivtv driver( http://ivtv.sourceforge.net/),
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>

#include "cx88.h"

MODULE_DESCRIPTION("driver for cx2388x/cx23416 based mpeg encoder cards");
MODULE_AUTHOR("Jelle Foks <jelle@foks.8m.com>");
MODULE_AUTHOR("Gerd Knorr <kraxel@bytesex.org> [SuSE Labs]");
MODULE_LICENSE("GPL");

static unsigned int mpegbufs = 16;
module_param(mpegbufs,int,0644);
MODULE_PARM_DESC(mpegbufs,"number of mpeg buffers, range 2-32");

static unsigned int debug = 0;
module_param(debug,int,0644);
MODULE_PARM_DESC(debug,"enable debug messages [blackbird]");

#define dprintk(level,fmt, arg...)	if (debug >= level) \
	printk(KERN_DEBUG "%s/2-bb: " fmt, dev->core->name , ## arg)

static LIST_HEAD(cx8802_devlist);

/* ------------------------------------------------------------------- */
/* static data                                                         */

static struct cx88_tvnorm tvnorms[] = {
	{
		.name      = "NTSC-M",
		.id        = V4L2_STD_NTSC_M,
		.cxiformat = VideoFormatNTSC,
		.cxoformat = 0x181f0008,
	},{
		.name      = "NTSC-JP",
		.id        = V4L2_STD_NTSC_M_JP,
		.cxiformat = VideoFormatNTSCJapan,
		.cxoformat = 0x181f0008,
#if 0
	},{
		.name      = "NTSC-4.43",
		.id        = FIXME,
		.cxiformat = VideoFormatNTSC443,
		.cxoformat = 0x181f0008,
#endif
	},{
		.name      = "PAL-BG",
		.id        = V4L2_STD_PAL_BG,
		.cxiformat = VideoFormatPAL,
		.cxoformat = 0x181f0008,
	},{
		.name      = "PAL-DK",
		.id        = V4L2_STD_PAL_DK,
		.cxiformat = VideoFormatPAL,
		.cxoformat = 0x181f0008,
	},{
		.name      = "PAL-I",
		.id        = V4L2_STD_PAL_I,
		.cxiformat = VideoFormatPAL,
		.cxoformat = 0x181f0008,
        },{
		.name      = "PAL-M",
		.id        = V4L2_STD_PAL_M,
		.cxiformat = VideoFormatPALM,
		.cxoformat = 0x1c1f0008,
	},{
		.name      = "PAL-N",
		.id        = V4L2_STD_PAL_N,
		.cxiformat = VideoFormatPALN,
		.cxoformat = 0x1c1f0008,
	},{
		.name      = "PAL-Nc",
		.id        = V4L2_STD_PAL_Nc,
		.cxiformat = VideoFormatPALNC,
		.cxoformat = 0x1c1f0008,
	},{
		.name      = "PAL-60",
		.id        = V4L2_STD_PAL_60,
		.cxiformat = VideoFormatPAL60,
		.cxoformat = 0x181f0008,
	},{
		.name      = "SECAM-L",
		.id        = V4L2_STD_SECAM_L,
		.cxiformat = VideoFormatSECAM,
		.cxoformat = 0x181f0008,
	},{
		.name      = "SECAM-DK",
		.id        = V4L2_STD_SECAM_DK,
		.cxiformat = VideoFormatSECAM,
		.cxoformat = 0x181f0008,
	}
};

/* ------------------------------------------------------------------- */

static const struct v4l2_queryctrl no_ctl = {
	.name  = "42",
	.flags = V4L2_CTRL_FLAG_DISABLED,
};

static struct cx88_ctrl cx8800_ctls[] = {
	/* --- video --- */
	{
		.v = {
			.id            = V4L2_CID_BRIGHTNESS,
			.name          = "Brightness",
			.minimum       = 0x00,
			.maximum       = 0xff,
			.step          = 1,
			.default_value = 0,
			.type          = V4L2_CTRL_TYPE_INTEGER,
		},
		.off                   = 128,
		.reg                   = MO_CONTR_BRIGHT,
		.mask                  = 0x00ff,
		.shift                 = 0,
	},{
		.v = {
			.id            = V4L2_CID_CONTRAST,
			.name          = "Contrast",
			.minimum       = 0,
			.maximum       = 0xff,
			.step          = 1,
			.default_value = 0,
			.type          = V4L2_CTRL_TYPE_INTEGER,
		},
		.reg                   = MO_CONTR_BRIGHT,
		.mask                  = 0xff00,
		.shift                 = 8,
	},{
		.v = {
			.id            = V4L2_CID_HUE,
			.name          = "Hue",
			.minimum       = 0,
			.maximum       = 0xff,
			.step          = 1,
			.default_value = 0,
			.type          = V4L2_CTRL_TYPE_INTEGER,
		},
		.off                   = 0,
		.reg                   = MO_HUE,
		.mask                  = 0x00ff,
		.shift                 = 0,
	},{
		/* strictly, this only describes only U saturation.
		 * V saturation is handled specially through code.
		 */
		.v = {
			.id            = V4L2_CID_SATURATION,
			.name          = "Saturation",
			.minimum       = 0,
			.maximum       = 0xff,
			.step          = 1,
			.default_value = 0,
			.type          = V4L2_CTRL_TYPE_INTEGER,
		},
		.off                   = 0,
		.reg                   = MO_UV_SATURATION,
		.mask                  = 0x00ff,
		.shift                 = 0,
	},{
	/* --- audio --- */
		.v = {
			.id            = V4L2_CID_AUDIO_MUTE,
			.name          = "Mute",
			.minimum       = 0,
			.maximum       = 1,
			.type          = V4L2_CTRL_TYPE_BOOLEAN,
		},
		.reg                   = AUD_VOL_CTL,
		.sreg                  = SHADOW_AUD_VOL_CTL,
		.mask                  = (1 << 6),
		.shift                 = 6,
	},{
		.v = {
			.id            = V4L2_CID_AUDIO_VOLUME,
			.name          = "Volume",
			.minimum       = 0,
			.maximum       = 0x3f,
			.step          = 1,
			.default_value = 0,
			.type          = V4L2_CTRL_TYPE_INTEGER,
		},
		.reg                   = AUD_VOL_CTL,
		.sreg                  = SHADOW_AUD_VOL_CTL,
		.mask                  = 0x3f,
		.shift                 = 0,
	},{
		.v = {
			.id            = V4L2_CID_AUDIO_BALANCE,
			.name          = "Balance",
			.minimum       = 0,
			.maximum       = 0x7f,
			.step          = 1,
			.default_value = 0x40,
			.type          = V4L2_CTRL_TYPE_INTEGER,
		},
		.reg                   = AUD_BAL_CTL,
		.sreg                  = SHADOW_AUD_BAL_CTL,
		.mask                  = 0x7f,
		.shift                 = 0,
	}
};
const int CX8800_CTLS = ARRAY_SIZE(cx8800_ctls);

/* ------------------------------------------------------------------ */

#define BLACKBIRD_FIRM_ENC_FILENAME "blackbird-fw-enc.bin"
#define BLACKBIRD_FIRM_IMAGE_SIZE 256*1024

/* defines below are from ivtv-driver.h */

#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/*Firmware API commands*/
#define IVTV_API_STD_TIMEOUT 0x00010000 /*units??*/

#define IVTV_API_ASSIGN_FRAME_DROP_RATE 0x000000d0
#define IVTV_API_ASSIGN_PLACEHOLDER 0x000000d8
#define IVTV_API_MUTE_VIDEO 0x000000d9
#define IVTV_API_MUTE_AUDIO 0x000000da
#define IVTV_API_INITIALIZE_INPUT 0x000000cd
#define IVTV_API_REFRESH_INPUT 0x000000d3
#define IVTV_API_ASSIGN_NUM_VSYNC_LINES 0x000000d6

#define BLACKBIRD_API_PING               0x80
#define BLACKBIRD_API_BEGIN_CAPTURE      0x81
enum blackbird_capture_type {
	BLACKBIRD_MPEG_CAPTURE,
	BLACKBIRD_RAW_CAPTURE,
	BLACKBIRD_RAW_PASSTHRU_CAPTURE
};
enum blackbird_capture_bits {
	BLACKBIRD_RAW_BITS_NONE             = 0x00,
	BLACKBIRD_RAW_BITS_YUV_CAPTURE      = 0x01,
	BLACKBIRD_RAW_BITS_PCM_CAPTURE      = 0x02,
	BLACKBIRD_RAW_BITS_VBI_CAPTURE      = 0x04,
	BLACKBIRD_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	BLACKBIRD_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
#define BLACKBIRD_API_END_CAPTURE        0x82
enum blackbird_capture_end {
	BLACKBIRD_END_AT_GOP, /* stop at the end of gop, generate irq */
	BLACKBIRD_END_NOW, /* stop immediately, no irq */
};
#define BLACKBIRD_API_SET_AUDIO_ID       0x89
#define BLACKBIRD_API_SET_VIDEO_ID       0x8B
#define BLACKBIRD_API_SET_PCR_ID         0x8D
#define BLACKBIRD_API_SET_FRAMERATE      0x8F
enum blackbird_framerate {
	BLACKBIRD_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	BLACKBIRD_FRAMERATE_PAL_25   /* PAL: 25fps */
};
#define BLACKBIRD_API_SET_RESOLUTION     0x91
#define BLACKBIRD_API_SET_VIDEO_BITRATE  0x95
enum blackbird_video_bitrate_type {
	BLACKBIRD_VIDEO_VBR,
	BLACKBIRD_VIDEO_CBR
};
#define BLACKBIRD_PEAK_RATE_DIVISOR 400
enum blackbird_mux_rate {
	BLACKBIRD_MUX_RATE_DEFAULT,
	 /* dvd mux rate: multiply by 400 to get the actual rate */
	BLACKBIRD_MUX_RATE_DVD = 25200
};
#define BLACKBIRD_API_SET_GOP_STRUCTURE  0x97
#define BLACKBIRD_API_SET_ASPECT_RATIO   0x99
enum blackbird_aspect_ratio {
	BLACKBIRD_ASPECT_RATIO_FORBIDDEN,
	BLACKBIRD_ASPECT_RATIO_1_1_SQUARE,
	BLACKBIRD_ASPECT_RATIO_4_3,
	BLACKBIRD_ASPECT_RATIO_16_9,
	BLACKBIRD_ASPECT_RATIO_221_100,
	BLACKBIRD_ASPECT_RATIO_RESERVED
};
#define BLACKBIRD_API_SET_DNR_MODE       0x9B
enum blackbird_dnr_bits {
	BLACKBIRD_DNR_BITS_MANUAL,
	BLACKBIRD_DNR_BITS_AUTO_SPATIAL,
	BLACKBIRD_DNR_BITS_AUTO_TEMPORAL,
	BLACKBIRD_DNR_BITS_AUTO
};
enum blackbird_median_filter {
	BLACKBIRD_MEDIAN_FILTER_DISABLED,
	BLACKBIRD_MEDIAN_FILTER_HORIZONTAL,
	BLACKBIRD_MEDIAN_FILTER_VERTICAL,
	BLACKBIRD_MEDIAN_FILTER_HV,
	BLACKBIRD_MEDIAN_FILTER_DIAGONAL
};
#define BLACKBIRD_API_SET_MANUAL_DNR     0x9D
#define BLACKBIRD_API_SET_DNR_MEDIAN     0x9F
#define BLACKBIRD_API_SET_SPATIAL_FILTER 0xA1
enum blackbird_spatial_filter_luma {
	BLACKBIRD_SPATIAL_FILTER_LUMA_DISABLED,
	BLACKBIRD_SPATIAL_FILTER_LUMA_1D_HORIZ,
	BLACKBIRD_SPATIAL_FILTER_LUMA_1D_VERT,
	BLACKBIRD_SPATIAL_FILTER_LUMA_2D_HV, /* separable, default */
	BLACKBIRD_SPATIAL_FILTER_LUMA_2D_SYMM /* symmetric non-separable */
};
enum blackbird_spatial_filter_chroma {
	BLACKBIRD_SPATIAL_FILTER_CHROMA_DISABLED,
	BLACKBIRD_SPATIAL_FILTER_CHROMA_1D_HORIZ /* default */
};
#define BLACKBIRD_API_SET_3_2_PULLDOWN   0xB1
enum blackbird_pulldown {
	BLACKBIRD_3_2_PULLDOWN_DISABLED,
	BLACKBIRD_3_2_PULLDOWN_ENABLED
};
#define BLACKBIRD_API_SET_VBI_LINE_NO    0xB7
enum blackbird_vbi_line_bits {
	BLACKBIRD_VBI_LINE_BITS_TOP_FIELD,
	BLACKBIRD_VBI_LINE_BITS_BOT_FIELD = (1 << 31),
	BLACKBIRD_VBI_LINE_BITS_ALL_LINES = 0xFFFFFFFF
};
enum blackbird_vbi_line {
	BLACKBIRD_VBI_LINE_DISABLED,
	BLACKBIRD_VBI_LINE_ENABLED
};
enum blackbird_vbi_slicing {
	BLACKBIRD_VBI_SLICING_NONE,
	BLACKBIRD_VBI_SLICING_CLOSED_CAPTION
};
#define BLACKBIRD_API_SET_STREAM_TYPE    0xB9
enum blackbird_stream_type {
	BLACKBIRD_STREAM_PROGRAM,
	BLACKBIRD_STREAM_TRANSPORT,
	BLACKBIRD_STREAM_MPEG1,
	BLACKBIRD_STREAM_PES_AV,
	BLACKBIRD_STREAM_UNKNOWN4,
	BLACKBIRD_STREAM_PES_VIDEO,
	BLACKBIRD_STREAM_UNKNOWN6,
	BLACKBIRD_STREAM_PES_AUDIO,
	BLACKBIRD_STREAM_UNKNOWN8,
	BLACKBIRD_STREAM_UNKNOWN9, /* audio/pcm ? */
	BLACKBIRD_STREAM_DVD,
	BLACKBIRD_STREAM_VCD,
	BLACKBIRD_STREAM_UNKNOWN12 /* svcd/xvcd ? */
};
#define BLACKBIRD_API_SET_OUTPUT_PORT    0xBB
enum blackbird_stream_port {
	BLACKBIRD_OUTPUT_PORT_MEMORY,
	BLACKBIRD_OUTPUT_PORT_STREAMING,
	BLACKBIRD_OUTPUT_PORT_SERIAL
};
#define BLACKBIRD_API_SET_AUDIO_PARAMS   0xBD
enum blackbird_audio_bits_sample_rate {
	BLACKBIRD_AUDIO_BITS_44100HZ,
	BLACKBIRD_AUDIO_BITS_48000HZ,
	BLACKBIRD_AUDIO_BITS_32000HZ,
	BLACKBIRD_AUDIO_BITS_RESERVED_HZ,
};
enum blackbird_audio_bits_encoding {
	BLACKBIRD_AUDIO_BITS_LAYER_1 = 0x1 << 2,
	BLACKBIRD_AUDIO_BITS_LAYER_2 = 0x2 << 2,
};
enum blackbird_audio_bits_bitrate_layer_1 {
	BLACKBIRD_AUDIO_BITS_LAYER_1_FREE_FORMAT,
	BLACKBIRD_AUDIO_BITS_LAYER_1_32  = 0x01 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_64  = 0x02 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_96  = 0x03 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_128 = 0x04 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_160 = 0x05 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_192 = 0x06 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_224 = 0x07 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_256 = 0x08 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_288 = 0x09 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_320 = 0x0A << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_352 = 0x0B << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_384 = 0x0C << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_416 = 0x0D << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_1_448 = 0x0E << 4,
};
enum blackbird_audio_bits_bitrate_layer_2 {
	BLACKBIRD_AUDIO_BITS_LAYER_2_FREE_FORMAT,
	BLACKBIRD_AUDIO_BITS_LAYER_2_32  = 0x01 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_48  = 0x02 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_56  = 0x03 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_64  = 0x04 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_80  = 0x05 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_96  = 0x06 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_112 = 0x07 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_128 = 0x08 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_160 = 0x09 << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_192 = 0x0A << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_224 = 0x0B << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_256 = 0x0C << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_320 = 0x0D << 4,
	BLACKBIRD_AUDIO_BITS_LAYER_2_384 = 0x0E << 4,
};
enum blackbird_audio_bits_mode {
	BLACKBIRD_AUDIO_BITS_STEREO,
	BLACKBIRD_AUDIO_BITS_JOINT_STEREO = 0x1 << 8,
	BLACKBIRD_AUDIO_BITS_DUAL         = 0x2 << 8,
	BLACKBIRD_AUDIO_BITS_MONO         = 0x3 << 8,
};
enum blackbird_audio_bits_mode_extension {
	BLACKBIRD_AUDIO_BITS_BOUND_4,
	BLACKBIRD_AUDIO_BITS_BOUND_8  = 0x1 << 10,
	BLACKBIRD_AUDIO_BITS_BOUND_12 = 0x2 << 10,
	BLACKBIRD_AUDIO_BITS_BOUND_16 = 0x3 << 10,
};
enum blackbird_audio_bits_emphasis {
	BLACKBIRD_AUDIO_BITS_EMPHASIS_NONE,
	BLACKBIRD_AUDIO_BITS_EMPHASIS_50_15     = 0x1 << 12,
	BLACKBIRD_AUDIO_BITS_EMPHASIS_RESERVED  = 0x2 << 12,
	BLACKBIRD_AUDIO_BITS_EMPHASIS_CCITT_J17 = 0x3 << 12,
};
enum blackbird_audio_bits_crc {
	BLACKBIRD_AUDIO_BITS_CRC_OFF,
	BLACKBIRD_AUDIO_BITS_CRC_ON = 0x1 << 14,
};
enum blackbird_audio_bits_copyright {
	BLACKBIRD_AUDIO_BITS_COPYRIGHT_OFF,
	BLACKBIRD_AUDIO_BITS_COPYRIGHT_ON = 0x1 << 15,
};
enum blackbird_audio_bits_original {
	BLACKBIRD_AUDIO_BITS_COPY,
	BLACKBIRD_AUDIO_BITS_ORIGINAL = 0x1 << 16,
};
#define BLACKBIRD_API_HALT               0xC3
#define BLACKBIRD_API_GET_VERSION        0xC4
#define BLACKBIRD_API_SET_GOP_CLOSURE    0xC5
enum blackbird_gop_closure {
	BLACKBIRD_GOP_CLOSURE_OFF,
	BLACKBIRD_GOP_CLOSURE_ON,
};

/* Registers */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_SPU (0x9050 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_HW_BLOCKS (0x9054 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_VPU (0x9058 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_APU (0xA064 /*| IVTV_REG_OFFSET*/)

/* ------------------------------------------------------------------ */

static void host_setup(struct cx88_core *core)
{
	/* toggle reset of the host */
	cx_write(MO_GPHST_SOFT_RST, 1);
	udelay(100);
	cx_write(MO_GPHST_SOFT_RST, 0);
	udelay(100);

	/* host port setup */
	cx_write(MO_GPHST_WSC, 0x44444444U);
	cx_write(MO_GPHST_XFR, 0);
	cx_write(MO_GPHST_WDTH, 15);
	cx_write(MO_GPHST_HDSHK, 0);
	cx_write(MO_GPHST_MUX16, 0x44448888U);
	cx_write(MO_GPHST_MODE, 0);
}

/* ------------------------------------------------------------------ */

#define P1_MDATA0 0x390000
#define P1_MDATA1 0x390001
#define P1_MDATA2 0x390002
#define P1_MDATA3 0x390003
#define P1_MADDR2 0x390004
#define P1_MADDR1 0x390005
#define P1_MADDR0 0x390006
#define P1_RDATA0 0x390008
#define P1_RDATA1 0x390009
#define P1_RDATA2 0x39000A
#define P1_RDATA3 0x39000B
#define P1_RADDR0 0x39000C
#define P1_RADDR1 0x39000D
#define P1_RRDWR  0x39000E

static int wait_ready_gpio0_bit1(struct cx88_core *core, u32 state)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(1);
	u32 gpio0,need;

	need = state ? 2 : 0;
	for (;;) {
		gpio0 = cx_read(MO_GP0_IO) & 2;
		if (need == gpio0)
			return 0;
		if (time_after(jiffies,timeout))
			return -1;
		udelay(1);
	}
}

static int memory_write(struct cx88_core *core, u32 address, u32 value)
{
	/* Warning: address is dword address (4 bytes) */
	cx_writeb(P1_MDATA0, (unsigned int)value);
	cx_writeb(P1_MDATA1, (unsigned int)(value >> 8));
	cx_writeb(P1_MDATA2, (unsigned int)(value >> 16));
	cx_writeb(P1_MDATA3, (unsigned int)(value >> 24));
	cx_writeb(P1_MADDR2, (unsigned int)(address >> 16) | 0x40);
	cx_writeb(P1_MADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_MADDR0, (unsigned int)address);
	cx_read(P1_MDATA0);
	cx_read(P1_MADDR0);

	return wait_ready_gpio0_bit1(core,1);
}

static int memory_read(struct cx88_core *core, u32 address, u32 *value)
{
        int retval;
	u32 val;

	/* Warning: address is dword address (4 bytes) */
	cx_writeb(P1_MADDR2, (unsigned int)(address >> 16) & ~0xC0);
	cx_writeb(P1_MADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_MADDR0, (unsigned int)address);
	cx_read(P1_MADDR0);

	retval = wait_ready_gpio0_bit1(core,1);

	cx_writeb(P1_MDATA3, 0);
	val     = (unsigned char)cx_read(P1_MDATA3) << 24;
	cx_writeb(P1_MDATA2, 0);
	val    |= (unsigned char)cx_read(P1_MDATA2) << 16;
	cx_writeb(P1_MDATA1, 0);
	val    |= (unsigned char)cx_read(P1_MDATA1) << 8;
	cx_writeb(P1_MDATA0, 0);
	val    |= (unsigned char)cx_read(P1_MDATA0);

	*value  = val;
	return retval;
}

static int register_write(struct cx88_core *core, u32 address, u32 value)
{
	cx_writeb(P1_RDATA0, (unsigned int)value);
	cx_writeb(P1_RDATA1, (unsigned int)(value >> 8));
	cx_writeb(P1_RDATA2, (unsigned int)(value >> 16));
	cx_writeb(P1_RDATA3, (unsigned int)(value >> 24));
	cx_writeb(P1_RADDR0, (unsigned int)address);
	cx_writeb(P1_RADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_RRDWR, 1);
	cx_read(P1_RDATA0);
	cx_read(P1_RADDR0);

	return wait_ready_gpio0_bit1(core,1);
}


static int register_read(struct cx88_core *core, u32 address, u32 *value)
{
	int retval;
	u32 val;

	cx_writeb(P1_RADDR0, (unsigned int)address);
	cx_writeb(P1_RADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_RRDWR, 0);
	cx_read(P1_RADDR0);

	retval  = wait_ready_gpio0_bit1(core,1);
	val     = (unsigned char)cx_read(P1_RDATA0);
	val    |= (unsigned char)cx_read(P1_RDATA1) << 8;
	val    |= (unsigned char)cx_read(P1_RDATA2) << 16;
	val    |= (unsigned char)cx_read(P1_RDATA3) << 24;

	*value  = val;
	return retval;
}

/* ------------------------------------------------------------------ */

/* We don't need to call the API often, so using just one mailbox will probably suffice */
static int blackbird_api_cmd(struct cx8802_dev *dev, u32 command,
			     u32 inputcnt, u32 outputcnt, ...)
{
	unsigned long timeout;
	u32 value, flag, retval;
	int i;
	va_list args;
	va_start(args, outputcnt);

	dprintk(1,"%s: 0x%X\n", __FUNCTION__, command);

	/* this may not be 100% safe if we can't read any memory location
	   without side effects */
	memory_read(dev->core, dev->mailbox - 4, &value);
	if (value != 0x12345678) {
		dprintk(0, "Firmware and/or mailbox pointer not initialized or corrupted\n");
		return -1;
	}

	memory_read(dev->core, dev->mailbox, &flag);
	if (flag) {
		dprintk(0, "ERROR: Mailbox appears to be in use (%x)\n", flag);
		return -1;
	}

	flag |= 1; /* tell 'em we're working on it */
	memory_write(dev->core, dev->mailbox, flag);

	/* write command + args + fill remaining with zeros */
	memory_write(dev->core, dev->mailbox + 1, command); /* command code */
	memory_write(dev->core, dev->mailbox + 3, IVTV_API_STD_TIMEOUT); /* timeout */
	for (i = 0; i < inputcnt ; i++) {
		value = va_arg(args, int);
		memory_write(dev->core, dev->mailbox + 4 + i, value);
		dprintk(1, "API Input %d = %d\n", i, value);
	}
	for (; i < 16 ; i++)
		memory_write(dev->core, dev->mailbox + 4 + i, 0);

	flag |= 3; /* tell 'em we're done writing */
	memory_write(dev->core, dev->mailbox, flag);

	/* wait for firmware to handle the API command */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		memory_read(dev->core, dev->mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies,timeout)) {
			dprintk(0, "ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/* read output values */
	for (i = 0; i < outputcnt ; i++) {
		int *vptr = va_arg(args, int *);
		memory_read(dev->core, dev->mailbox + 4 + i, vptr);
		dprintk(1, "API Output %d = %d\n", i, *vptr);
	}
	va_end(args);

	memory_read(dev->core, dev->mailbox + 2, &retval);
	dprintk(1, "API result = %d\n",retval);

	flag = 0;
	memory_write(dev->core, dev->mailbox, flag);
	return retval;
}


static int blackbird_find_mailbox(struct cx8802_dev *dev)
{
	u32 signature[4]={0x12345678, 0x34567812, 0x56781234, 0x78123456};
	int signaturecnt=0;
	u32 value;
	int i;

	for (i = 0; i < BLACKBIRD_FIRM_IMAGE_SIZE; i++) {
		memory_read(dev->core, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found\n");
			return i+1;
		}
	}
	dprintk(0, "Mailbox signature values not found!\n");
	return -1;
}

static int blackbird_load_firmware(struct cx8802_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 checksum = 0;
	u32 *dataptr;

	retval  = register_write(dev->core, IVTV_REG_VPU, 0xFFFFFFED);
        retval |= register_write(dev->core, IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
        retval |= register_write(dev->core, IVTV_REG_ENC_SDRAM_REFRESH, 0x80000640);
        retval |= register_write(dev->core, IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	msleep(1);
        retval |= register_write(dev->core, IVTV_REG_APU, 0);

	if (retval < 0)
		dprintk(0, "Error with register_write\n");

	retval = request_firmware(&firmware, BLACKBIRD_FIRM_ENC_FILENAME,
				  &dev->pci->dev);
	if (retval != 0) {
		dprintk(0, "ERROR: Hotplug firmware request failed (%s).\n",
			BLACKBIRD_FIRM_ENC_FILENAME);
		dprintk(0, "Please fix your hotplug setup, the board will "
			"not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != BLACKBIRD_FIRM_IMAGE_SIZE) {
		dprintk(0, "ERROR: Firmware size mismatch (have %zd, expected %d)\n",
			firmware->size, BLACKBIRD_FIRM_IMAGE_SIZE);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		dprintk(0, "ERROR: Firmware magic mismatch, wrong file?\n");
		return -1;
	}

	/* transfer to the chip */
	dprintk(1,"Loading firmware ...\n");
	dataptr = (u32*)firmware->data;
	for (i = 0; i < (firmware->size >> 2); i++) {
		value = *dataptr;
		checksum += ~value;
		memory_write(dev->core, i, value);
		dataptr++;
	}

	/* read back to verify with the checksum */
	for (i--; i >= 0; i--) {
		memory_read(dev->core, i, &value);
		checksum -= ~value;
	}
	if (checksum) {
		dprintk(0, "ERROR: Firmware load failed (checksum mismatch).\n");
		return -1;
	}
	release_firmware(firmware);
	dprintk(0, "Firmware upload successful.\n");

        retval |= register_write(dev->core, IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
        retval |= register_read(dev->core, IVTV_REG_SPU, &value);
        retval |= register_write(dev->core, IVTV_REG_SPU, value & 0xFFFFFFFE);
	msleep(1);

	retval |= register_read(dev->core, IVTV_REG_VPU, &value);
        retval |= register_write(dev->core, IVTV_REG_VPU, value & 0xFFFFFFE8);

	if (retval < 0)
		dprintk(0, "Error with register_write\n");
	return 0;
}

/**
 Settings used by the windows tv app for PVR2000:
=================================================================================================================
Profile | Codec | Resolution | CBR/VBR | Video Qlty   | V. Bitrate | Frmrate | Audio Codec | A. Bitrate | A. Mode
-----------------------------------------------------------------------------------------------------------------
MPEG-1  | MPEG1 | 352x288PAL | (CBR)   | 1000:Optimal | 2000 Kbps  | 25fps   | MPG1 Layer2 | 224kbps    | Stereo
MPEG-2  | MPEG2 | 720x576PAL | VBR     | 600 :Good    | 4000 Kbps  | 25fps   | MPG1 Layer2 | 224kbps    | Stereo
VCD     | MPEG1 | 352x288PAL | (CBR)   | 1000:Optimal | 1150 Kbps  | 25fps   | MPG1 Layer2 | 224kbps    | Stereo
DVD     | MPEG2 | 720x576PAL | VBR     | 600 :Good    | 6000 Kbps  | 25fps   | MPG1 Layer2 | 224kbps    | Stereo
DB* DVD | MPEG2 | 720x576PAL | CBR     | 600 :Good    | 6000 Kbps  | 25fps   | MPG1 Layer2 | 224kbps    | Stereo
=================================================================================================================
*DB: "DirectBurn"
*/
static void blackbird_codec_settings(struct cx8802_dev *dev)
{
	int bitrate_mode = 1;
	int bitrate = 7500000;
	int bitrate_peak = 7500000;
#if 1
	bitrate_mode = BLACKBIRD_VIDEO_VBR;
	bitrate = 3500*1024;
	bitrate_peak = 4000*1024;
#endif

	/* assign stream type */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_STREAM_TYPE, 1, 0, BLACKBIRD_STREAM_PROGRAM);

	/* assign output port */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_OUTPUT_PORT, 1, 0, BLACKBIRD_OUTPUT_PORT_STREAMING); /* Host */

	/* assign framerate */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_FRAMERATE, 1, 0, BLACKBIRD_FRAMERATE_PAL_25);

	/* assign frame size */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_RESOLUTION, 2, 0,
			  dev->height, dev->width);

	/* assign aspect ratio */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_ASPECT_RATIO, 1, 0, BLACKBIRD_ASPECT_RATIO_4_3);

	/* assign bitrates */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_VIDEO_BITRATE, 5, 0,
			 bitrate_mode,         /* mode */
			 bitrate,              /* bps */
			 bitrate_peak / 400,   /* peak/400 */
			 BLACKBIRD_MUX_RATE_DEFAULT /*, 0x70*/);             /* encoding buffer, ckennedy */

	/* assign gop properties */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_GOP_STRUCTURE, 2, 0, 15, 3);

	/* assign 3 2 pulldown */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_3_2_PULLDOWN, 1, 0, BLACKBIRD_3_2_PULLDOWN_DISABLED);

	/* note: it's not necessary to set the samplerate, the mpeg encoder seems to autodetect/adjust */
	//blackbird_api_cmd(dev, IVTV_API_ASSIGN_AUDIO_PROPERTIES, 1, 0, (2<<2) | (8<<4));
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_AUDIO_PARAMS, 1, 0,
			BLACKBIRD_AUDIO_BITS_44100HZ |
			BLACKBIRD_AUDIO_BITS_LAYER_2 |
			BLACKBIRD_AUDIO_BITS_LAYER_2_224 |
			BLACKBIRD_AUDIO_BITS_STEREO |
			/*BLACKBIRD_AUDIO_BITS_BOUND_4 |*/
			BLACKBIRD_AUDIO_BITS_EMPHASIS_NONE |
			BLACKBIRD_AUDIO_BITS_CRC_OFF |
			BLACKBIRD_AUDIO_BITS_COPYRIGHT_OFF |
			BLACKBIRD_AUDIO_BITS_COPY
		);

	/* assign gop closure */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_GOP_CLOSURE, 1, 0, BLACKBIRD_GOP_CLOSURE_OFF);

	/* assign audio properties */
	//blackbird_api_cmd(dev, IVTV_API_ASSIGN_AUDIO_PROPERTIES, 1, 0, 0 | (2 << 2) | (14 << 4));

	/* assign dnr filter mode */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_DNR_MODE, 2, 0,
			BLACKBIRD_DNR_BITS_MANUAL,
			BLACKBIRD_MEDIAN_FILTER_DISABLED
		);

	/* assign dnr filter props*/
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_MANUAL_DNR, 2, 0, 0, 0);

	/* assign coring levels (luma_h, luma_l, chroma_h, chroma_l) */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_DNR_MEDIAN, 4, 0, 0, 255, 0, 255);

	/* assign spatial filter type: luma_t: horiz_only, chroma_t: horiz_only */
	blackbird_api_cmd(dev, BLACKBIRD_API_SET_SPATIAL_FILTER, 2, 0,
			BLACKBIRD_SPATIAL_FILTER_LUMA_1D_HORIZ,
			BLACKBIRD_SPATIAL_FILTER_CHROMA_1D_HORIZ
		);

	/* assign frame drop rate */
	//blackbird_api_cmd(dev, IVTV_API_ASSIGN_FRAME_DROP_RATE, 1, 0, 0);
}

static int blackbird_initialize_codec(struct cx8802_dev *dev)
{
	struct cx88_core *core = dev->core;
	int version;
	int retval;

	dprintk(1,"Initialize codec\n");
	retval = blackbird_api_cmd(dev, BLACKBIRD_API_PING, 0, 0); /* ping */
	if (retval < 0) {
		/* ping was not successful, reset and upload firmware */
		cx_write(MO_SRST_IO, 0); /* SYS_RSTO=0 */
		msleep(1);
		cx_write(MO_SRST_IO, 1); /* SYS_RSTO=1 */
		msleep(1);
		retval = blackbird_load_firmware(dev);
		if (retval < 0)
			return retval;

		dev->mailbox = blackbird_find_mailbox(dev);
		if (dev->mailbox < 0)
			return -1;

		retval = blackbird_api_cmd(dev, BLACKBIRD_API_PING, 0, 0); /* ping */
		if (retval < 0) {
			dprintk(0, "ERROR: Firmware ping failed!\n");
			return -1;
		}

		retval = blackbird_api_cmd(dev, BLACKBIRD_API_GET_VERSION, 0, 1, &version);
		if (retval < 0) {
			dprintk(0, "ERROR: Firmware get encoder version failed!\n");
			return -1;
		}
		dprintk(0, "Firmware version is 0x%08x\n", version);
	}
	msleep(1);

	cx_write(MO_PINMUX_IO, 0x88); /* 656-8bit IO and enable MPEG parallel IO */
	cx_clear(MO_INPUT_FORMAT, 0x100); /* chroma subcarrier lock to normal? */
	cx_write(MO_VBOS_CONTROL, 0x84A00); /* no 656 mode, 8-bit pixels, disable VBI */
	cx_clear(MO_OUTPUT_FORMAT, 0x0008); /* Normal Y-limits to let the mpeg encoder sync */

#if 0 /* FIXME */
	set_scale(dev, 720, 480, V4L2_FIELD_INTERLACED);
#endif
	blackbird_codec_settings(dev);
	msleep(1);

	//blackbird_api_cmd(dev, IVTV_API_ASSIGN_NUM_VSYNC_LINES, 4, 0, 0xef, 0xef);
	blackbird_api_cmd(dev, IVTV_API_ASSIGN_NUM_VSYNC_LINES, 4, 0, 0xf0, 0xf0);
	//blackbird_api_cmd(dev, IVTV_API_ASSIGN_NUM_VSYNC_LINES, 4, 0, 0x180, 0x180);
	blackbird_api_cmd(dev, IVTV_API_ASSIGN_PLACEHOLDER, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	blackbird_api_cmd(dev, IVTV_API_INITIALIZE_INPUT, 0, 0); /* initialize the video input */

	msleep(1);

	blackbird_api_cmd(dev, IVTV_API_MUTE_VIDEO, 1, 0, 0);
	msleep(1);
	blackbird_api_cmd(dev, IVTV_API_MUTE_AUDIO, 1, 0, 0);
	msleep(1);

	//blackbird_api_cmd(dev, BLACKBIRD_API_BEGIN_CAPTURE, 2, 0, 0, 0x13); /* start capturing to the host interface */
	blackbird_api_cmd(dev, BLACKBIRD_API_BEGIN_CAPTURE, 2, 0,
			BLACKBIRD_MPEG_CAPTURE,
			BLACKBIRD_RAW_BITS_NONE
		); /* start capturing to the host interface */
	msleep(10);

	blackbird_api_cmd(dev, IVTV_API_REFRESH_INPUT, 0,0);
	return 0;
}

/* ------------------------------------------------------------------ */

static int bb_buf_setup(struct videobuf_queue *q,
			unsigned int *count, unsigned int *size)
{
	struct cx8802_fh *fh = q->priv_data;

	fh->dev->ts_packet_size  = 512;
	fh->dev->ts_packet_count = 100;

	*size = fh->dev->ts_packet_size * fh->dev->ts_packet_count;
	if (0 == *count)
		*count = mpegbufs;
	if (*count < 2)
		*count = 2;
	if (*count > 32)
		*count = 32;
	return 0;
}

static int
bb_buf_prepare(struct videobuf_queue *q, struct videobuf_buffer *vb,
	       enum v4l2_field field)
{
	struct cx8802_fh *fh = q->priv_data;
	return cx8802_buf_prepare(fh->dev, (struct cx88_buffer*)vb);
}

static void
bb_buf_queue(struct videobuf_queue *q, struct videobuf_buffer *vb)
{
	struct cx8802_fh *fh = q->priv_data;
	cx8802_buf_queue(fh->dev, (struct cx88_buffer*)vb);
}

static void
bb_buf_release(struct videobuf_queue *q, struct videobuf_buffer *vb)
{
	struct cx8802_fh *fh = q->priv_data;
	cx88_free_buffer(fh->dev->pci, (struct cx88_buffer*)vb);
}

static struct videobuf_queue_ops blackbird_qops = {
	.buf_setup    = bb_buf_setup,
	.buf_prepare  = bb_buf_prepare,
	.buf_queue    = bb_buf_queue,
	.buf_release  = bb_buf_release,
};

/* ------------------------------------------------------------------ */

static int video_mux(struct cx8802_dev *dev, unsigned int input)
{
	struct cx88_core *core = dev->core;

	dprintk(1,"video_mux: %d [vmux=%d,gpio=0x%x,0x%x,0x%x,0x%x]\n",
		input, INPUT(input)->vmux,
		INPUT(input)->gpio0,INPUT(input)->gpio1,
		INPUT(input)->gpio2,INPUT(input)->gpio3);
	dev->core->input = input;
	cx_andor(MO_INPUT_FORMAT, 0x03 << 14, INPUT(input)->vmux << 14);
	cx_write(MO_GP3_IO, INPUT(input)->gpio3);
	cx_write(MO_GP0_IO, INPUT(input)->gpio0);
	cx_write(MO_GP1_IO, INPUT(input)->gpio1);
	cx_write(MO_GP2_IO, INPUT(input)->gpio2);

	switch (INPUT(input)->type) {
	case CX88_VMUX_SVIDEO:
		cx_set(MO_AFECFG_IO,    0x00000001);
		cx_set(MO_INPUT_FORMAT, 0x00010010);
		cx_set(MO_FILTER_EVEN,  0x00002020);
		cx_set(MO_FILTER_ODD,   0x00002020);
		break;
	default:
		cx_clear(MO_AFECFG_IO,    0x00000001);
		cx_clear(MO_INPUT_FORMAT, 0x00010010);
		cx_clear(MO_FILTER_EVEN,  0x00002020);
		cx_clear(MO_FILTER_ODD,   0x00002020);
		break;
	}
	return 0;
}

/* ------------------------------------------------------------------ */

static int get_control(struct cx8802_dev *dev, struct v4l2_control *ctl)
{
	struct cx88_core *core = dev->core;
	struct cx88_ctrl *c = NULL;
	u32 value;
	int i;

	for (i = 0; i < CX8800_CTLS; i++)
		if (cx8800_ctls[i].v.id == ctl->id)
			c = &cx8800_ctls[i];
	if (NULL == c)
		return -EINVAL;

	value = c->sreg ? cx_sread(c->sreg) : cx_read(c->reg);
	switch (ctl->id) {
	case V4L2_CID_AUDIO_BALANCE:
		ctl->value = (value & 0x40) ? (value & 0x3f) : (0x40 - (value & 0x3f));
		break;
	case V4L2_CID_AUDIO_VOLUME:
		ctl->value = 0x3f - (value & 0x3f);
		break;
	default:
		ctl->value = ((value + (c->off << c->shift)) & c->mask) >> c->shift;
		break;
	}
	return 0;
}

static int set_control(struct cx8802_dev *dev, struct v4l2_control *ctl)
{
	struct cx88_core *core = dev->core;
	struct cx88_ctrl *c = NULL;
        u32 v_sat_value;
	u32 value;
	int i;

	for (i = 0; i < CX8800_CTLS; i++)
		if (cx8800_ctls[i].v.id == ctl->id)
			c = &cx8800_ctls[i];
	if (NULL == c)
		return -EINVAL;

	switch (ctl->id) {
	case V4L2_CID_HUE:
		dprintk( 0, "HUE: 0x%02x\n", ctl->value );
		ctl->value = 0x0;
		break;
	case V4L2_CID_CONTRAST:
		dprintk( 0, "CONTRAST: 0x%02x\n", ctl->value );
		break;
	case V4L2_CID_BRIGHTNESS:
		dprintk( 0, "BRIGHTNESS: 0x%02x\n", ctl->value );
		break;
	}

	if (ctl->value < c->v.minimum)
	{
		dprintk( 0, "set_control: 0x%x < 0x%x\n", ctl->value, c->v.minimum );
		return -ERANGE;
	}
	if (ctl->value > c->v.maximum)
	{
		dprintk( 0, "set_control: 0x%x > 0x%x\n", ctl->value, c->v.maximum );
		/* clamp the value */
		ctl->value = c->v.maximum;
#if 0
		return -ERANGE;
#endif
	}
	switch (ctl->id) {
	case V4L2_CID_AUDIO_BALANCE:
		value = (ctl->value < 0x40) ? (0x40 - ctl->value) : ctl->value;
		break;
	case V4L2_CID_AUDIO_VOLUME:
		value = 0x3f - (ctl->value & 0x3f);
		break;
	case V4L2_CID_SATURATION:
		/* special v_sat handling */
		v_sat_value = ctl->value - (0x7f - 0x5a);
		if (v_sat_value > 0xff)
			v_sat_value = 0xff;
		if (v_sat_value < 0x00)
			v_sat_value = 0x00;
		cx_andor(MO_UV_SATURATION, 0xff00, v_sat_value << 8);
		/* fall through to default route for u_sat */
	default:
		value = ((ctl->value - c->off) << c->shift) & c->mask;
		break;
	}
	dprintk(1,"set_control id=0x%X reg=0x%x val=0x%x%s\n",
		ctl->id, c->reg, value, c->sreg ? " [shadowed]" : "");
	if (c->sreg) {
		cx_sandor(c->sreg, c->reg, c->mask, value);
	} else {
		cx_andor(c->reg, c->mask, value);
	}
	return 0;
}

static void init_controls(struct cx8802_dev *dev)
{
	static struct v4l2_control mute = {
		.id    = V4L2_CID_AUDIO_MUTE,
		.value = 1,
	};
	static struct v4l2_control volume = {
		.id    = V4L2_CID_AUDIO_VOLUME,
		.value = 0x3f,
	};
	static struct v4l2_control hue = {
		.id    = V4L2_CID_HUE,
		.value = 0x0,
	};
	static struct v4l2_control contrast = {
		.id    = V4L2_CID_CONTRAST,
		.value = 0x40,
	};
	static struct v4l2_control brightness = {
		.id    = V4L2_CID_BRIGHTNESS,
		.value = 0x99,
	};

	set_control(dev,&mute);
	set_control(dev,&volume);
	set_control(dev,&hue);
	set_control(dev,&contrast);
	set_control(dev,&brightness);
}

/* --- IVTV emulation stuff ----------------------------------------- */

/* For use with IVTV ioctl's */
struct ivtv_ioctl_codec {
        uint32_t aspect;
        uint32_t audio_bitmask;
        uint32_t bframes;
        uint32_t bitrate_mode;
        uint32_t bitrate;
        uint32_t bitrate_peak;
        uint32_t dnr_mode;
        uint32_t dnr_spatial;
        uint32_t dnr_temporal;
        uint32_t dnr_type;
        uint32_t framerate; /* read only, ignored on write */
        uint32_t framespergop;  /* read only, ignored on write */
        uint32_t gop_closure;
        uint32_t pulldown;
        uint32_t stream_type;
};

struct ivtv_sliced_vbi_format {
    unsigned long service_set;  /* one or more of the IVTV_SLICED_ defines */
    unsigned long packet_size;  /* the size in bytes of the ivtv_sliced_data packet */
    unsigned long io_size;      /* maximum number of bytes passed by one read() call */
    unsigned long reserved;
};

#define IVTV_IOC_G_CODEC           _IOR ('@', 48, struct ivtv_ioctl_codec)
#define IVTV_IOC_S_CODEC           _IOW ('@', 49, struct ivtv_ioctl_codec)
#define IVTV_IOC_S_VBI_MODE        _IOWR('@', 35, struct ivtv_sliced_vbi_format)
#define IVTV_IOC_G_VBI_MODE        _IOR ('@', 36, struct ivtv_sliced_vbi_format)
#define IVTV_IOC_S_VBI_EMBED       _IOW ('@', 54, int)

/* ------------------------------------------------------------------ */

static int mpeg_do_ioctl(struct inode *inode, struct file *file,
			 unsigned int cmd, void *arg)
{
	struct cx8802_fh  *fh  = file->private_data;
	struct cx8802_dev *dev = fh->dev;
	struct cx88_core  *core = dev->core;

	int err;

	if (debug > 1)
		cx88_print_ioctl(dev->core->name,cmd);
#if 1
	cx88_print_ioctl(dev->core->name,cmd);
	printk( KERN_INFO "IOCTL: 0x%x\n", cmd );
	dprintk( 0, "IOCTL: 0x%x\n", cmd );
#endif

	switch (cmd) {

	/* --- capabilities ------------------------------------------ */
	case VIDIOC_QUERYCAP:
	{
		struct v4l2_capability *cap = arg;

		memset(cap,0,sizeof(*cap));
		strcpy(cap->driver, "cx88_blackbird");
		strlcpy(cap->card, cx88_boards[core->board].name,
			sizeof(cap->card));
		sprintf(cap->bus_info,"PCI:%s",pci_name(dev->pci));
		cap->version = CX88_VERSION_CODE;
		cap->capabilities =
			V4L2_CAP_VIDEO_CAPTURE |
			V4L2_CAP_READWRITE     |
#if 0
			V4L2_CAP_STREAMING     |
			V4L2_CAP_VBI_CAPTURE   |
			V4L2_CAP_VIDEO_OVERLAY |
#endif
			0;
		if (UNSET != core->tuner_type)
			cap->capabilities |= V4L2_CAP_TUNER;

		return 0;
	}

	/* ---------- tv norms ---------- */
	case VIDIOC_ENUMSTD:
	{
		struct v4l2_standard *e = arg;
		unsigned int i;

		i = e->index;
		if (i >= ARRAY_SIZE(tvnorms))
			return -EINVAL;
		err = v4l2_video_std_construct(e, tvnorms[e->index].id,
					       tvnorms[e->index].name);
		e->index = i;
		if (err < 0)
			return err;
		return 0;
	}
	case VIDIOC_G_STD:
	{
		v4l2_std_id *id = arg;

		*id = core->tvnorm->id;
		return 0;
	}
	case VIDIOC_S_STD:
	{
		v4l2_std_id *id = arg;
		unsigned int i;

		for(i = 0; i < ARRAY_SIZE(tvnorms); i++)
			if (*id & tvnorms[i].id)
				break;
		if (i == ARRAY_SIZE(tvnorms))
			return -EINVAL;

		down(&dev->lock);
		cx88_set_tvnorm(dev->core,&tvnorms[i]);
		up(&dev->lock);
		return 0;
	}

	/* ------ input switching ---------- */
	case VIDIOC_ENUMINPUT:
	{
		static const char *iname[] = {
			[ CX88_VMUX_COMPOSITE1 ] = "Composite1",
			[ CX88_VMUX_COMPOSITE2 ] = "Composite2",
			[ CX88_VMUX_COMPOSITE3 ] = "Composite3",
			[ CX88_VMUX_COMPOSITE4 ] = "Composite4",
			[ CX88_VMUX_SVIDEO     ] = "S-Video",
			[ CX88_VMUX_TELEVISION ] = "Television",
			[ CX88_VMUX_CABLE      ] = "Cable TV",
			[ CX88_VMUX_DVB        ] = "DVB",
			[ CX88_VMUX_DEBUG      ] = "for debug only",
		};
		struct v4l2_input *i = arg;
		unsigned int n;

		n = i->index;
		if (n >= 4)
			return -EINVAL;
		if (0 == INPUT(n)->type)
			return -EINVAL;
		memset(i,0,sizeof(*i));
		i->index = n;
		i->type  = V4L2_INPUT_TYPE_CAMERA;
		strcpy(i->name,iname[INPUT(n)->type]);
		if ((CX88_VMUX_TELEVISION == INPUT(n)->type) ||
		    (CX88_VMUX_CABLE      == INPUT(n)->type))
			i->type = V4L2_INPUT_TYPE_TUNER;
		for (n = 0; n < ARRAY_SIZE(tvnorms); n++)
			i->std |= tvnorms[n].id;
		return 0;
	}
	case VIDIOC_G_INPUT:
	{
		unsigned int *i = arg;

		*i = dev->core->input;
		return 0;
	}
	case VIDIOC_S_INPUT:
	{
		unsigned int *i = arg;

		if (*i >= 4)
			return -EINVAL;
		down(&dev->lock);
		cx88_newstation(core);
		video_mux(dev,*i);
		up(&dev->lock);
		return 0;
	}

	/* --- capture ioctls ---------------------------------------- */
	case VIDIOC_ENUM_FMT:
	{
		struct v4l2_fmtdesc *f = arg;
		int index;

		index = f->index;
		if (index != 0)
			return -EINVAL;

		memset(f,0,sizeof(*f));
		f->index = index;
		strlcpy(f->description, "MPEG-2 TS", sizeof(f->description));
		f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		f->pixelformat = V4L2_PIX_FMT_MPEG;
		return 0;
	}
	case VIDIOC_G_FMT:
	case VIDIOC_S_FMT:
	case VIDIOC_TRY_FMT:
	{
		/* FIXME -- quick'n'dirty for exactly one size ... */
		struct v4l2_format *f = arg;

		memset(f,0,sizeof(*f));
		f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		f->fmt.pix.width        = dev->width;
		f->fmt.pix.height       = dev->height;
		f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
		f->fmt.pix.field        = V4L2_FIELD_NONE;
		f->fmt.pix.bytesperline = 0;
		f->fmt.pix.sizeimage    = 1024 * 512 /* FIXME: BUFFER_SIZE */;
		f->fmt.pix.colorspace   = 0;
		return 0;
	}

	/* --- controls ---------------------------------------------- */
	case VIDIOC_QUERYCTRL:
	{
		struct v4l2_queryctrl *c = arg;
		int i;

		if (c->id <  V4L2_CID_BASE ||
		    c->id >= V4L2_CID_LASTP1)
			return -EINVAL;
		for (i = 0; i < CX8800_CTLS; i++)
			if (cx8800_ctls[i].v.id == c->id)
				break;
		if (i == CX8800_CTLS) {
			*c = no_ctl;
			return 0;
		}
		*c = cx8800_ctls[i].v;
		return 0;
	}
	case VIDIOC_G_CTRL:
		return get_control(dev,arg);
	case VIDIOC_S_CTRL:
		return set_control(dev,arg);

	/* --- tuner ioctls ------------------------------------------ */
	case VIDIOC_G_TUNER:
	{
		struct v4l2_tuner *t = arg;
		u32 reg;

		if (UNSET == core->tuner_type)
			return -EINVAL;
		if (0 != t->index)
			return -EINVAL;

		memset(t,0,sizeof(*t));
		strcpy(t->name, "Television");
		t->type       = V4L2_TUNER_ANALOG_TV;
		t->capability = V4L2_TUNER_CAP_NORM;
		t->rangehigh  = 0xffffffffUL;

		cx88_get_stereo(core ,t);
		reg = cx_read(MO_DEVICE_STATUS);
                t->signal = (reg & (1<<5)) ? 0xffff : 0x0000;
		return 0;
	}
	case VIDIOC_S_TUNER:
	{
		struct v4l2_tuner *t = arg;

		if (UNSET == core->tuner_type)
			return -EINVAL;
		if (0 != t->index)
			return -EINVAL;
		cx88_set_stereo(core, t->audmode, 1);
		return 0;
	}
	case VIDIOC_G_FREQUENCY:
	{
		struct v4l2_frequency *f = arg;

		if (UNSET == core->tuner_type)
			return -EINVAL;
		if (f->tuner != 0)
			return -EINVAL;
		memset(f,0,sizeof(*f));
		f->type = V4L2_TUNER_ANALOG_TV;
		f->frequency = dev->freq;
		return 0;
	}
	case VIDIOC_S_FREQUENCY:
	{
		struct v4l2_frequency *f = arg;

		if (UNSET == core->tuner_type)
			return -EINVAL;
		if (f->tuner != 0)
			return -EINVAL;
		down(&dev->lock);
		dev->freq = f->frequency;
		cx88_newstation(core);
#ifdef V4L2_I2C_CLIENTS
		cx88_call_i2c_clients(dev->core,VIDIOC_S_FREQUENCY,f);
#else
		cx88_call_i2c_clients(dev->core,VIDIOCSFREQ,&dev->freq);
#endif
		up(&dev->lock);
		return 0;
	}

	/* --- streaming capture ------------------------------------- */
	case VIDIOC_REQBUFS:
		return videobuf_reqbufs(&fh->mpegq, arg);

	case VIDIOC_QUERYBUF:
		return videobuf_querybuf(&fh->mpegq, arg);

	case VIDIOC_QBUF:
		return videobuf_qbuf(&fh->mpegq, arg);

	case VIDIOC_DQBUF:
		return videobuf_dqbuf(&fh->mpegq, arg,
				      file->f_flags & O_NONBLOCK);

	case VIDIOC_STREAMON:
		return videobuf_streamon(&fh->mpegq);

	case VIDIOC_STREAMOFF:
		return videobuf_streamoff(&fh->mpegq);

	/* --- IVTV emulation layer --------------------------- */
	case IVTV_IOC_S_CODEC:
		printk( KERN_INFO "IVTV_IOC_S_CODEC\n" );
	case IVTV_IOC_G_CODEC:
	{
		struct ivtv_ioctl_codec *codec = arg;
#if 0
		printk( KERN_INFO "IVTV_IOC_G/S_CODEC\n" );
		printk( KERN_INFO "CODEC: aspect: %d\n", codec->aspect );
		printk( KERN_INFO "CODEC: audio : %d\n", codec->audio_bitmask );
		printk( KERN_INFO "CODEC: bfrms : %d\n", codec->bframes );
		printk( KERN_INFO "CODEC: br_mod: %d\n", codec->bitrate_mode );
		printk( KERN_INFO "CODEC: btrate: %d\n", codec->bitrate );
		printk( KERN_INFO "CODEC: btr_pk: %d\n", codec->bitrate_peak );
		printk( KERN_INFO "CODEC: dnr_md: %d\n", codec->dnr_mode );
		printk( KERN_INFO "CODEC: dnr_sp: %d\n", codec->dnr_spatial );
		printk( KERN_INFO "CODEC: dnr_tp: %d\n", codec->dnr_temporal );
		printk( KERN_INFO "CODEC: dnr_ty: %d\n", codec->dnr_type );
		printk( KERN_INFO "CODEC: framer: %d\n", codec->framerate );
		printk( KERN_INFO "CODEC: grmgop: %d\n", codec->framespergop );
		printk( KERN_INFO "CODEC: gop_cl: %d\n", codec->gop_closure );
		printk( KERN_INFO "CODEC: pulldn: %d\n", codec->pulldown );
		printk( KERN_INFO "CODEC: strtyp: %d\n\n", codec->stream_type );
#endif

		codec->aspect = 2; /* 4:3 */
		codec->audio_bitmask = (2 << 2) | (14 << 4); /* layer II | 384kbps */
		codec->bframes = 2;
		codec->bitrate_mode = 1; /* cbr */
		codec->bitrate = 7500000; /* bps */
		codec->bitrate_peak = 7500000; /* peak */
		codec->dnr_mode = 0; /* spatial=manual | temporal=manual */
		codec->dnr_spatial = 0;
		codec->dnr_temporal = 0;
		codec->dnr_type = 0; /* disabled */
		codec->framerate = 30; /* 30fps */
		codec->framespergop = 15;
		codec->gop_closure = 0; /* open */
		codec->pulldown = 0; /* enabled */
		codec->stream_type = 0; /* program stream */
#if 0
		printk( KERN_INFO "CODEC: aspect: %d\n", codec->aspect );
		printk( KERN_INFO "CODEC: audio : %d\n", codec->audio_bitmask );
		printk( KERN_INFO "CODEC: bfrms : %d\n", codec->bframes );
		printk( KERN_INFO "CODEC: br_mod: %d\n", codec->bitrate_mode );
		printk( KERN_INFO "CODEC: btrate: %d\n", codec->bitrate );
		printk( KERN_INFO "CODEC: btr_pk: %d\n", codec->bitrate_peak );
		printk( KERN_INFO "CODEC: dnr_md: %d\n", codec->dnr_mode );
		printk( KERN_INFO "CODEC: dnr_sp: %d\n", codec->dnr_spatial );
		printk( KERN_INFO "CODEC: dnr_tp: %d\n", codec->dnr_temporal );
		printk( KERN_INFO "CODEC: dnr_ty: %d\n", codec->dnr_type );
		printk( KERN_INFO "CODEC: framer: %d\n", codec->framerate );
		printk( KERN_INFO "CODEC: grmgop: %d\n", codec->framespergop );
		printk( KERN_INFO "CODEC: gop_cl: %d\n", codec->gop_closure );
		printk( KERN_INFO "CODEC: pulldn: %d\n", codec->pulldown );
		printk( KERN_INFO "CODEC: strtyp: %d\n", codec->stream_type );
#endif
		return 0;
	}
	case IVTV_IOC_S_VBI_MODE:
	{
		struct ivtv_sliced_vbi_format *fmt = arg;
		printk( KERN_INFO "IVTV_IOC_S_VBI_MODE: ss: %ld, ps: %ld, is: %ld\n",
				fmt->service_set, fmt->packet_size, fmt->io_size );
#if 0
		fmt->service_set = 0;  /* one or more of the IVTV_SLICED_ defines */
    		fmt->packet_size = 0;  /* the size in bytes of the ivtv_sliced_data packet */
		fmt->io_size = 0;      /* maximum number of bytes passed by one read() call */
#endif
		return 0;
	}
	case IVTV_IOC_G_VBI_MODE:
	{
		struct ivtv_sliced_vbi_format *fmt = arg;

		fmt->service_set = 1;  /* one or more of the IVTV_SLICED_ defines */
    		fmt->packet_size = 0;  /* the size in bytes of the ivtv_sliced_data packet */
		fmt->io_size = 0;      /* maximum number of bytes passed by one read() call */
		return 0;
	}
	case IVTV_IOC_S_VBI_EMBED:
	{
		int *embed = arg;
		printk( KERN_INFO "IVTV_IOC_S_VBI_EMBED: %d\n", *embed );
		return 0;
	}

	default:
	{
		int err;
		err = v4l_compat_translate_ioctl(inode,file,cmd,arg,
						  mpeg_do_ioctl);
		if( err < 0 )
		{
			printk( KERN_INFO "Unknown IOCTL: 0x%x\n", cmd );
			cx88_print_ioctl(dev->core->name,cmd);
		}
		return err;
		/*return -EINVAL;*/
	}

	}
	return 0;
}

static int mpeg_ioctl(struct inode *inode, struct file *file,
		       unsigned int cmd, unsigned long arg)
{
	switch( cmd )
	{
		case 0xFFEE7703: cmd = IVTV_IOC_G_CODEC; break;
		case 0xFFEE7704: cmd = IVTV_IOC_S_CODEC; break;
		case 0xFFEE7781: /*cmd = IVTV_IOC_PLAY; break;*/
		case 0xFFEE7782: /*cmd = IVTV_IOC_PAUSE; break;*/
		case 0xFFEE7783: /*cmd = IVTV_IOC_FRAMESYNC; break;*/
		case 0xFFEE7784: /*cmd = IVTV_IOC_GET_TIMING; break;*/
		case 0xFFEE7785: /*cmd = IVTV_IOC_S_SLOW_FAST; break;*/
		case 0xFFEE7786: /*cmd = IVTV_IOC_S_START_DECODE; break;*/
		case 0xFFEE7787: /*cmd = IVTV_IOC_S_STOP_DECODE; break;*/
		case 0xFFEE7789: /*cmd = IVTV_IOC_GET_FB; break;*/
			printk( KERN_INFO "IVTV: 0x%x\n", cmd );
	}

	int rez;
	rez = video_usercopy(inode, file, cmd, arg, mpeg_do_ioctl);
	if( rez < 0 )
	{
		printk( KERN_INFO "_______: 0x%x ---> %d\n", cmd, rez );
		cx88_print_ioctl( "_______", cmd );
	}
	return rez;
}

static int mpeg_open(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	struct cx8802_dev *h,*dev = NULL;
	struct cx8802_fh *fh;
	struct list_head *list;

	list_for_each(list,&cx8802_devlist) {
		h = list_entry(list, struct cx8802_dev, devlist);
		if (h->mpeg_dev->minor == minor)
			dev = h;
	}
	if (NULL == dev)
		return -ENODEV;

	if (blackbird_initialize_codec(dev) < 0)
		return -EINVAL;
	dprintk(1,"open minor=%d\n",minor);

	/* allocate + initialize per filehandle data */
	fh = kmalloc(sizeof(*fh),GFP_KERNEL);
	if (NULL == fh)
		return -ENOMEM;
	memset(fh,0,sizeof(*fh));
	file->private_data = fh;
	fh->dev      = dev;

	/* FIXME: locking against other video device */
	cx88_set_scale(dev->core, dev->width, dev->height,
		       V4L2_FIELD_INTERLACED);

	videobuf_queue_init(&fh->mpegq, &blackbird_qops,
			    dev->pci, &dev->slock,
			    V4L2_BUF_TYPE_VIDEO_CAPTURE,
			    V4L2_FIELD_TOP,
			    sizeof(struct cx88_buffer),
			    fh);
	return 0;
}

static int mpeg_release(struct inode *inode, struct file *file)
{
	struct cx8802_fh  *fh  = file->private_data;

	//blackbird_api_cmd(fh->dev, BLACKBIRD_API_END_CAPTURE, 3, 0, BLACKBIRD_END_NOW, 0, 0x13);
	blackbird_api_cmd(fh->dev, BLACKBIRD_API_END_CAPTURE, 3, 0,
			BLACKBIRD_END_NOW,
			BLACKBIRD_MPEG_CAPTURE,
			BLACKBIRD_RAW_BITS_NONE
		);

	/* stop mpeg capture */
	if (fh->mpegq.streaming)
		videobuf_streamoff(&fh->mpegq);
	if (fh->mpegq.reading)
		videobuf_read_stop(&fh->mpegq);

	videobuf_mmap_free(&fh->mpegq);
	file->private_data = NULL;
	kfree(fh);
	return 0;
}

static ssize_t
mpeg_read(struct file *file, char __user *data, size_t count, loff_t *ppos)
{
	struct cx8802_fh *fh = file->private_data;
	ssize_t ret;

	/*file->f_flags |= O_NONBLOCK;*/
	ret = videobuf_read_stream(&fh->mpegq, data, count, ppos, 0,
				    file->f_flags & O_NONBLOCK );

#if 1
	/* why is read() choking with mythbackend while it works just fine
	   with cat, dd and mplayer ? */
	if( ret < 0 )
		printk( KERN_INFO "mpeg_read: %d, count: %d, non-blocking: %d\n", ret, count, file->f_flags & O_NONBLOCK );
	if( ret != count && !( file->f_flags & O_NONBLOCK ) )
		printk( KERN_INFO "mpeg_read: %d, count: %d, non-blocking: %d\n", ret, count, file->f_flags & O_NONBLOCK );
#endif
	return ret;
}

static unsigned int
mpeg_poll(struct file *file, struct poll_table_struct *wait)
{
	struct cx8802_fh *fh = file->private_data;

	return videobuf_poll_stream(file, &fh->mpegq, wait);
}

static int
mpeg_mmap(struct file *file, struct vm_area_struct * vma)
{
	struct cx8802_fh *fh = file->private_data;

	return videobuf_mmap_mapper(&fh->mpegq, vma);
}

static struct file_operations mpeg_fops =
{
	.owner	       = THIS_MODULE,
	.open	       = mpeg_open,
	.release       = mpeg_release,
	.read	       = mpeg_read,
	.poll          = mpeg_poll,
	.mmap	       = mpeg_mmap,
	.ioctl	       = mpeg_ioctl,
	.llseek        = no_llseek,
};

static struct video_device cx8802_mpeg_template =
{
	.name          = "cx8802",
	.type          = VID_TYPE_CAPTURE|VID_TYPE_TUNER|VID_TYPE_SCALES|VID_TYPE_MPEG_ENCODER,
	.hardware      = 0,
	.fops          = &mpeg_fops,
	.minor         = -1,
};

/* ------------------------------------------------------------------ */

static void blackbird_unregister_video(struct cx8802_dev *dev)
{
	if (dev->mpeg_dev) {
		if (-1 != dev->mpeg_dev->minor)
			video_unregister_device(dev->mpeg_dev);
		else
			video_device_release(dev->mpeg_dev);
		dev->mpeg_dev = NULL;
	}
}

static int blackbird_register_video(struct cx8802_dev *dev)
{
	int err;

	dev->mpeg_dev = cx88_vdev_init(dev->core,dev->pci,
				       &cx8802_mpeg_template,"mpeg");
	err = video_register_device(dev->mpeg_dev,VFL_TYPE_GRABBER, -1);
	if (err < 0) {
		printk(KERN_INFO "%s/2: can't register mpeg device\n",
		       dev->core->name);
		return err;
	}
	printk(KERN_INFO "%s/2: registered device video%d [mpeg]\n",
	       dev->core->name,dev->mpeg_dev->minor & 0x1f);
	return 0;
}

/* ----------------------------------------------------------- */

static int __devinit blackbird_probe(struct pci_dev *pci_dev,
				     const struct pci_device_id *pci_id)
{
	struct cx8802_dev *dev;
	struct cx88_core  *core;
	int err;

	/* general setup */
	core = cx88_core_get(pci_dev);
	if (NULL == core)
		return -EINVAL;

	err = -ENODEV;
	if (!cx88_boards[core->board].blackbird)
		goto fail_core;

	err = -ENOMEM;
	dev = kmalloc(sizeof(*dev),GFP_KERNEL);
	if (NULL == dev)
		goto fail_core;
	memset(dev,0,sizeof(*dev));
	dev->pci = pci_dev;
	dev->core = core;
	dev->width = 720;
	dev->height = 480;

	err = cx8802_init_common(dev);
	if (0 != err)
		goto fail_free;

	/* blackbird stuff */
	printk("%s/2: cx23416 based mpeg encoder (blackbird reference design)\n",
	       core->name);
	host_setup(dev->core);

	list_add_tail(&dev->devlist,&cx8802_devlist);
	blackbird_register_video(dev);

	/* initial device configuration: needed ? */
#if 1
	down(&dev->lock);
	init_controls(dev);
	cx88_set_tvnorm(dev->core,tvnorms);
	video_mux(dev,0);
	up(&dev->lock);
#endif

	return 0;

 fail_free:
	kfree(dev);
 fail_core:
	cx88_core_put(core,pci_dev);
	return err;
}

static void __devexit blackbird_remove(struct pci_dev *pci_dev)
{
        struct cx8802_dev *dev = pci_get_drvdata(pci_dev);

	/* blackbird */
	blackbird_unregister_video(dev);
	list_del(&dev->devlist);

	/* common */
	cx8802_fini_common(dev);
	cx88_core_put(dev->core,dev->pci);
	kfree(dev);
}

static struct pci_device_id cx8802_pci_tbl[] = {
	{
		.vendor       = 0x14f1,
		.device       = 0x8802,
                .subvendor    = PCI_ANY_ID,
                .subdevice    = PCI_ANY_ID,
	},{
		/* --- end of list --- */
	}
};
MODULE_DEVICE_TABLE(pci, cx8802_pci_tbl);

static struct pci_driver blackbird_pci_driver = {
        .name     = "cx88-blackbird",
        .id_table = cx8802_pci_tbl,
        .probe    = blackbird_probe,
        .remove   = __devexit_p(blackbird_remove),
	.suspend  = cx8802_suspend_common,
	.resume   = cx8802_resume_common,
};

static int blackbird_init(void)
{
	printk(KERN_INFO "cx2388x blackbird driver version %d.%d.%d loaded\n",
	       (CX88_VERSION_CODE >> 16) & 0xff,
	       (CX88_VERSION_CODE >>  8) & 0xff,
	       CX88_VERSION_CODE & 0xff);
#ifdef SNAPSHOT
	printk(KERN_INFO "cx2388x: snapshot date %04d-%02d-%02d\n",
	       SNAPSHOT/10000, (SNAPSHOT/100)%100, SNAPSHOT%100);
#endif
	return pci_module_init(&blackbird_pci_driver);
}

static void blackbird_fini(void)
{
	pci_unregister_driver(&blackbird_pci_driver);
}

module_init(blackbird_init);
module_exit(blackbird_fini);

/* ----------------------------------------------------------- */
/*
 * Local variables:
 * c-basic-offset: 8
 * End:
 */

