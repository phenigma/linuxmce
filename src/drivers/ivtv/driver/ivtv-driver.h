/*
    ivtv driver internal defines and structures
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>
    Copyright (C) 2004  Chris Kennedy <ckennedy at kmos.org>

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

#ifndef IVTV_DRIVER_H
#define IVTV_DRIVER_H

/* Internal header for ivtv project:
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
#ifndef MODULE
 #define MODULE
#endif

#include "compat.h"

#include <linux/module.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#include <linux/moduleparam.h>
#endif
#include <linux/init.h>
#include <linux/config.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/video_decoder.h>
#include <linux/video_encoder.h>
#include <linux/videodev.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/list.h>
#include <linux/unistd.h>
#include <linux/byteorder/swab.h>
#include <linux/pagemap.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>
#include <asm/system.h>

#define IVTV_INTERNAL
#include "ivtv.h"

#include "saa7115.h"

/* If you don't want to patch to v4l2, grab a copy of
 * videodev2.h and put it in the same dir as this file */ 
#ifndef HAVE_V4L2
 #define HAVE_V4L2 1 
 /* Using a non-v4l2-kernel means that the v4l-compat.o module is not
    available, so we have to discard v4l compatibility. We mark that
    with this define. */
 #define V4L2_NO_COMPAT
 #include "videodev2.h"
#endif

/* Some older videodev2.h headers do not have these ioctls defined. */
#ifndef VIDIOC_ENUMAUDIO
#define VIDIOC_ENUMAUDIO       _IOWR ('V', 65, struct v4l2_audio)
#endif
#ifndef VIDIOC_ENUMAUDOUT
#define VIDIOC_ENUMAUDOUT      _IOWR ('V', 66, struct v4l2_audioout)
#endif

#include "tuner.h"
#ifdef LINUX26
#include <linux/workqueue.h>
#else
#include <linux/tqueue.h>
#endif

#ifdef CONFIG_LIRC_I2C
#  error "This driver is not compatible with the LIRC I2C kernel configuration option."
#endif

#ifndef CONFIG_PCI
#  error "This driver requires kernel PCI support."
#endif

#define IVTV_ENCODER_OFFSET	0x00000000
#define IVTV_ENCODER_SIZE	0x00800000/* Last half isn't needed 0x01000000*/

#define IVTV_DECODER_OFFSET	0x01000000
#define IVTV_DECODER_SIZE	0x00800000/* Last half isn't needed 0x01000000*/

#define IVTV_REG_OFFSET 	0x02000000
#define IVTV_REG_SIZE		0x00010000

#define IVTV_ENCDEC_SIZE	IVTV_REG_OFFSET
#define IVTV_IOREMAP_SIZE	(IVTV_REG_OFFSET + IVTV_REG_SIZE)

/* Buffers on hardware offsets */
#define IVTV_YUV_BUFFER_OFFSET    0x00240400 /* second buffer? */
#define IVTV_YUV_BUFFER_OFFSET_2  0x00029000 /* FIXME this needs work */
#define IVTV_YUV_BUFFER_OFFSET_3  0x002d8200 /* third buffer? */
#define IVTV_YUV_BUFFER_UV_OFFSET 0x10e00 /* only NTSC? */

/* General */
#define IVTV_DRIVER_NAME "ivtv"
#define IVTV_DRIVER_VERSION_MAJOR 0
#define IVTV_DRIVER_VERSION_MINOR 2
#define IVTV_DRIVER_VERSION_PATCHLEVEL 0
#define IVTV_DRIVER_VERSION_COMMENT "rc3f" 
#define IVTV_VERSION __stringify(IVTV_DRIVER_VERSION_MAJOR) "." __stringify(IVTV_DRIVER_VERSION_MINOR) "." __stringify(IVTV_DRIVER_VERSION_PATCHLEVEL) "-" IVTV_DRIVER_VERSION_COMMENT 
#define IVTV_DRIVER_VERSION KERNEL_VERSION(IVTV_DRIVER_VERSION_MAJOR,IVTV_DRIVER_VERSION_MINOR,IVTV_DRIVER_VERSION_PATCHLEVEL)

#define IVTV_MAX_CARDS 8

#define IVTV_CARD_PVR_250 0 /* wintv pvr 250 */
#define IVTV_CARD_PVR_350 1 /* encoder, decoder, tv-out */
#define IVTV_CARD_M179    2 /* AVerMedia M179 (encoder only) */
#define IVTV_CARD_MPG600  3 /* Kuroutoshikou ITVC16-STVLP/YUAN MPG600, encoder only */
#define IVTV_CARD_MPG160  4 /* Kuroutoshikou ITVC15-STVLP/YUAN MPG160
                               iTVC15 based, but does not have tv-out */

#define NO_AUDIO    0  /* the card can't handle audio */
#define USE_MSP34XX 1  /* switch audio by MSP34XX */
#define USE_GPIO    2  /* switch audio by GPIO */

#define IVTV_AUDIO_32000 0x2  /* 32 kHz audio */
#define IVTV_AUDIO_44100 0x0  /* 44.1 kHz audio */
#define IVTV_AUDIO_48000 0x1  /* 48 kHz audio */

#define IVTV_V4L2_DEC_MPG_OFFSET 16 /* offset from 0 to register decoder mpg v4l2 minors on */
#define IVTV_V4L2_ENC_PCM_OFFSET 24 /* offset from 0 to register pcm v4l2 minors on */
#define IVTV_V4L2_ENC_YUV_OFFSET 32 /* offset from 0 to register yuv v4l2 minors on */
#define IVTV_V4L2_DEC_YUV_OFFSET 48 /* offset from 0 to register decoder yuv v4l2 minors on */
#define IVTV_V4L2_DEC_VBI_OFFSET  4 /* offset from 0 to register decoder vbi input v4l2 minors on */
#define IVTV_V4L2_DEC_VOUT_OFFSET 8 /* offset from 0 to register vbi output v4l2 minors on */

#define IVTV_ENC_STREAM_TYPE_MPG 0
#define IVTV_ENC_STREAM_TYPE_YUV 1
#define IVTV_ENC_STREAM_TYPE_VBI 2
#define IVTV_ENC_STREAM_TYPE_PCM 3
#define IVTV_ENC_STREAM_TYPE_RAD 4
#define IVTV_DEC_STREAM_TYPE_MPG 5
#define IVTV_DEC_STREAM_TYPE_VBI 6
#define IVTV_DEC_STREAM_TYPE_VOUT 7
#define IVTV_DEC_STREAM_TYPE_YUV 8

#define IVTV_ENC_MEM_START 0x00000000
#define IVTV_DEC_MEM_START 0x01000000
#define PCI_VENDOR_ID_ICOMP  0x4444
#define PCI_DEVICE_ID_IVTV15 0x0803
#define IVTV_PCI_ID_HAUPPAUGE 0x0070 /* subsystem vendor id */
#define IVTV_PCI_ID_AVERMEDIA 0x1461
#define IVTV_PCI_ID_YUAN 0x12ab
#define PCI_DEVICE_ID_IVTV16 0x0016
#define IVTV_MBOX_MAX_BOXES 20
#define IVTV_MBOX_API_BOXES 6
#define IVTV_MBOX_DMA_START 6
#define IVTV_MBOX_DMA_END 8
#define IVTV_MBOX_MAX_DATA 16
#define IVTV_MBOX_DMA 9
#define IVTV_MBOX_FIELD_DISPLAYED 8
#define IVTV_MBOX_SIZE 80

/* API Command timeout for DMA xfers, divide xfer/timeout, wait for api cmd? */
#define OSD_API_TIMEOUT 500 /* Divide xfer size by this */
#define DEC_API_TIMEOUT 500 /* Divide xfer size by this */
#define OSD_API_WAIT	0
#define DEC_API_WAIT	0

/* Decoder Buffer hardware size on Chip */
#define IVTV_DEC_MAX_BUF        0x00100000 /* max bytes in decoder buffer */
#define IVTV_DEC_MIN_BUF        0x00010000 /* min bytes in dec buffer */

/* ======================================================================== */
/* ========================== START USER SETTABLE DMA VARIABLES =========== */
/* ======================================================================== */

/* DMA Buffers Sizes */
#define IVTV_DMA_ENC_BUF_SIZE     0x00004000
#define IVTV_DMA_ENC_YUV_BUF_SIZE 0x00019500
#define IVTV_DMA_ENC_PCM_BUF_SIZE 0x00001200
#define IVTV_DMA_DEC_BUF_SIZE     0x00010000
#define IVTV_DMA_DEC_YUV_BUF_SIZE 0x00019500
#define IVTV_DMA_DEC_BUF_SIZE     0x00010000

/* These two values should not be changed. VBI data does not currently
   use DMA, so these buffers just have to be large enough to store one
   frame of raw or sliced VBI data */
#define IVTV_DMA_VBI_BUF_SIZE     (51 * 1024) /* 1443 * 36 aligned at 1024 bytes */
#define IVTV_DMA_DEC_VBI_BUF_SIZE 0x00000800  /* 2048 is sufficient to store the
                                                 sliced VBI data of one frame. */

/* Decoder DMA Maximum Transfer per interrupt */
#define IVTV_DMA_MAX_XFER	65536 /* max data sent per interrupt */
#define IVTV_DMA_MIN_XFER	IVTV_DEC_MIN_BUF
		/* min data sent per interrupt */

/* Align all Xfers to this amount */
#define IVTV_DMA_OSD_ALIGN_XFER 65536 /* Give DMA in this size packet */

#define IVTV_DMA_SG_OSD_SIZE	2883584 /* max dma OSD sg list */
#define IVTV_DMA_SG_OSD_ENT	(2883584/PAGE_SIZE) /* sg entities */

/* Decoder DMA or PIO, 1=PIO, 0=DMA */
#define IVTV_DEC_PIO		0 /* This sometimes times out, seems to  kill
                                     encoding sometimes */

/* What we tell the firmware to expect or return, 131072 is best so far */
#define FW_DEC_DMA_XFER 65536	/* 524288, 262144, 131072, 65536 */
#define FW_ENC_DMA_XFER	1  /* 524288, 262144, 131072 */
#define FW_ENC_DMA_TYPE	1  /* 1-XX = frame based, 0 = bytes (see above) */

/* DMA Buffers, Default allocated */
#define IVTV_DEFAULT_MPG_BUFFERS (int)(4 * ((1024 * 1024)))
#define IVTV_DEFAULT_YUV_BUFFERS 0
#define IVTV_DEFAULT_VBI_BUFFERS (int)(2 * ((1024 * 1024)))
#define IVTV_DEFAULT_PCM_BUFFERS (int)(2 * ((1024 * 1024)))
#define IVTV_DEFAULT_DEC_MPG_BUFFERS (int)(1 * ((1024 * 1024)))
#define IVTV_DEFAULT_DEC_YUV_BUFFERS 0
#define IVTV_DEFAULT_DEC_VBI_BUFFERS (int)(256 * (1024))

/* DMA Buffers Limits */
#define IVTV_MAX_MPG_BUFFERS (int)(16 * ((1024 * 1024)))
#define IVTV_MAX_YUV_BUFFERS (int)(32 * ((1024 * 1024)))
#define IVTV_MAX_VBI_BUFFERS (int)(2 * ((1024 * 1024)))
#define IVTV_MAX_PCM_BUFFERS (int)(32 * ((1024 * 1024)))
#define IVTV_MAX_DEC_MPG_BUFFERS (int)(1 * ((1024 * 1024)))
#define IVTV_MAX_DEC_YUV_BUFFERS (int)(3 * ((1024 * 1024)))
#define IVTV_MAX_DEC_VBI_BUFFERS (int)(1 * ((1024 * 1024)))

/* ======================================================================== */
/* ========================== END USER SETTABLE DMA VARIABLES ============= */
/* ======================================================================== */

/* Decoder DMA Settings */
struct ivtv_dma_settings {
	/* Driver DMA xfer sizes */
	int	dec_max_xfer;
	int	dec_min_xfer;
	
	/* SG Array Sizes */
	int	osd_sg_size;
	int	osd_sg_ent;

	/* Buffer sizes */
	int 	enc_buf_size;
	int 	enc_yuv_buf_size;
	int 	enc_pcm_buf_size;
	int 	dec_buf_size;
	int 	dec_yuv_buf_size;
	int 	vbi_buf_size;
	int 	dec_vbi_buf_size;

	/* Max growable amount of buffers */
	int	max_yuv_buf;
	int	max_pcm_buf;
	int	max_mpg_buf;
	int 	max_dec_yuv_buf;
	int 	max_dec_mpg_buf;
	int	max_vbi_buf;

	/* Chip DMA Xfer Settings */
        int 	fw_dec_dma_xfer;
        int 	fw_enc_dma_xfer;
        int 	fw_enc_dma_type;

	/* Processor IO */
	int	enc_pio;
	int	dec_pio;
	int	osd_pio;
};

/* Decoder DMA state structure */
struct ivtv_dec_dma {
        u32 		last_addr;
        u32 		bytes_filled;
        u32 		bytes_needed;
        u32 		last_xfer;
        u32 		total_xfer;
        u32 		sg_bytes;
	atomic_t	type; /* 0=mpg, 1=osd, 2=yuv */ 
	u32 		data[IVTV_MBOX_MAX_DATA];
	u32 		speed_data[IVTV_MBOX_MAX_DATA];
	unsigned long 	vsync_count;
	u32 		ts;
	u32 		wq_runs;
	atomic_t	intr;
};

/* Decoder Status Register */
#define IVTV_DMA_ERR_LIST 0x00000010
#define IVTV_DMA_ERR_WRITE 0x00000008
#define IVTV_DMA_ERR_READ 0x00000004
#define IVTV_DMA_SUCCESS_WRITE 0x00000002
#define IVTV_DMA_SUCCESS_READ 0x00000001
#define IVTV_DMA_READ_ERR (IVTV_DMA_ERR_LIST | IVTV_DMA_ERR_READ)
#define IVTV_DMA_WRITE_ERR (IVTV_DMA_ERR_LIST | IVTV_DMA_ERR_WRITE)
#define IVTV_DMA_ERR (IVTV_DMA_ERR_LIST | IVTV_DMA_ERR_WRITE | IVTV_DMA_ERR_READ)

#define ITVC_GET_REG		0x1
#define ITVC_SET_REG		0x2

/* Encoder Timeouts */
#define ENC_DMA_TIMEOUT (HZ*5)	 /* Encoding max timeout for data */

/* video related */
#define IVTV_MAX_INPUTS 9

/* Decoder Registers in Decoder Memory Space */
#define IVTV_REG_DEC_SIZE       0x00003a50 /*| IVTV_DECODER_OFFSET */
#define IVTV_REG_DEC_READY      0x00003a54 /*| IVTV_DECODER_OFFSET */
#define IVTV_REG_DEC_UNKNOWN1   0x00003a58 /*| IVTV_DECODER_OFFSET */
#define IVTV_REG_DEC_POINTER    0x00003a5c /*| IVTV_DECODER_OFFSET */
#define IVTV_REG_DEC_NEEDED     0x00003a60 /*| IVTV_DECODER_OFFSET */
#define IVTV_REG_DEC_UNKNOWN2   0x00003a64 /*| IVTV_DECODER_OFFSET */
#define IVTV_REG_DEC_FILL       0x00003a68 /*| IVTV_DECODER_OFFSET */

/* DMA Registers */
#define IVTV_REG_DMAXFER 	(0x0000 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DMASTATUS 	(0x0004 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DECDMAADDR 	(0x0008 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCDMAADDR 	(0x000c /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DMACONTROL 	(0x0010 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DMABITS 	(0x001c /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_IRQSTATUS 	(0x0040 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_IRQMASK 	(0x0048 /*| IVTV_REG_OFFSET*/)

/* SG Buffers */
#define IVTV_REG_ENCSG1SRC 	(0x0080 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG1DST 	(0x0084 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG1LEN 	(0x0088 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG2SRC 	(0x008c /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG2DST 	(0x0090 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG2LEN 	(0x0094 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG3SRC 	(0x0098 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG3DST 	(0x009c /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG3LEN 	(0x00a0 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG4SRC 	(0x00a4 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG4DST 	(0x00a8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG4LEN 	(0x00ac /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG5SRC 	(0x00b0 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG5DST 	(0x00b4 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG5LEN 	(0x00b8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG6SRC 	(0x00bc /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG6DST 	(0x00c0 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG6LEN 	(0x00c4 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG7SRC 	(0x00c8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG7DST 	(0x00cc /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG7LEN 	(0x00d0 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG8SRC 	(0x00d4 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG8DST 	(0x00d8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENCSG8LEN 	(0x00dc /*| IVTV_REG_OFFSET*/)

#define IVTV_REG_DECSG1SRC 	(0x00e0 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DECSG1DST 	(0x00e4 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DECSG1LEN 	(0x00e8 /*| IVTV_REG_OFFSET*/)

/* Setup? Registers */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DEC_SDRAM_REFRESH (0x08F8 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_DEC_SDRAM_PRECHARGE (0x08FC /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_VDM (0x2800 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_AO (0x2D00 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_BYTEFLUSH (0x2D24 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_SPU (0x9050 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_HW_BLOCKS (0x9054 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_VPU (0x9058 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_APU (0xA064 /*| IVTV_REG_OFFSET*/)

/* IRQ Masks */
#define IVTV_IRQ_MASK_INIT 0x00040000 /*IVTV_IRQ_DEC_DMA_ERR, ???, ??? */

#define IVTV_IRQ_MASK_CAPTURE 0xE0000000 /* IVTV_IRQ_ENC_START_CAP,
					    IVTV_IRQ_ENC_EOS,
					    IVTV_IRQ_ENC_VBI_CAP */

#define IVTV_IRQ_MASK_DECODE 0x00480000 /* IVTV_IRQ_DEC_VSYNC, 
					   IVTV_IRQ_DEC_VBI_RE_INSERT, 
					   IVTV_IRQ_DEC_DATA_REQ */

#define IVTV_IRQ_ENC_START_CAP		(0x1 << 31)
#define IVTV_IRQ_ENC_EOS		(0x1 << 30)
#define IVTV_IRQ_ENC_VBI_CAP		(0x1 << 29)
#define IVTV_IRQ_ENC_VIM_RST		(0x1 << 28)
#define IVTV_IRQ_ENC_DMA_COMPLETE	(0x1 << 27)
#define IVTV_IRQ_DEC_DMA_26		(0x1 << 26)
#define IVTV_IRQ_DEC_COPY_PROTECT	(0x1 << 25)
#define IVTV_IRQ_DEC_AUD_MODE_CHG	(0x1 << 24)
#define IVTV_IRQ_DEC_DMA_23		(0x1 << 23)
#define IVTV_IRQ_DEC_DATA_REQ		(0x1 << 22)
#define IVTV_IRQ_DEC_IFRAME_DONE	(0x1 << 21)
#define IVTV_IRQ_DEC_DMA_COMPLETE	(0x1 << 20)
#define IVTV_IRQ_DEC_VBI_RE_INSERT	(0x1 << 19)
#define IVTV_IRQ_DEC_DMA_ERR		(0x1 << 18)
#define IVTV_IRQ_DEC_DMA_ERR2		(0x1 << 17)
#define IVTV_IRQ_DEC_DMA_ERR3		(0x1 << 16)
#define IVTV_IRQ_DEC_DMA_15		(0x1 << 15)
#define IVTV_IRQ_DEC_DMA_14		(0x1 << 14)
#define IVTV_IRQ_DEC_DMA_13		(0x1 << 13)
#define IVTV_IRQ_DEC_DMA_12		(0x1 << 12)
#define IVTV_IRQ_DEC_DMA_11		(0x1 << 11)
#define IVTV_IRQ_DEC_VSYNC		(0x1 << 10)
#define IVTV_IRQ_DEC_DMA_9		(0x1 << 9)
#define IVTV_IRQ_DEC_DMA_8		(0x1 << 8)
#define IVTV_IRQ_DEC_DMA_7		(0x1 << 7)
#define IVTV_IRQ_DEC_DMA_6		(0x1 << 6)
#define IVTV_IRQ_DEC_DMA_5		(0x1 << 5)
#define IVTV_IRQ_DEC_DMA_4		(0x1 << 4)
#define IVTV_IRQ_DEC_DMA_3		(0x1 << 3)
#define IVTV_IRQ_DEC_DMA_2		(0x1 << 2)
#define IVTV_IRQ_DEC_DMA_1		(0x1 << 1)
#define IVTV_IRQ_DEC_DMA_0		(0x1 << 0)

/* commands */
#define IVTV_MASK_SPU_ENABLE 0xFFFFFFFE
#define IVTV_MASK_VPU_ENABLE15 0xFFFFFFF6
#define IVTV_MASK_VPU_ENABLE16 0xFFFFFFFB
#define IVTV_CMD_VDM_STOP 0x00000000
#define IVTV_CMD_AO_STOP 0x00000005
#define IVTV_CMD_APU_PING 0x00000000
#define IVTV_CMD_VPU_STOP15 0xFFFFFFFE
#define IVTV_CMD_VPU_STOP16 0xFFFFFFEE
#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF
#define IVTV_CMD_SPU_STOP 0x00000001
#define IVTV_CMD_SDRAM_PRECHARGE_INIT 0x0000001A
#define IVTV_CMD_SDRAM_REFRESH_INIT 0x80000640
#define IVTV_SDRAM_SLEEPTIME (60 * HZ / 100)	/* 600ms */

/*Used for locating the firmware mailboxes*/
#if defined(LINUX26) && defined(CONFIG_FW_LOADER) && defined(IVTV_FW_LOADER)
#define IVTV_FIRM_ENC_FILENAME "ivtv-fw-enc.bin" 
#define IVTV_FIRM_DEC_FILENAME "ivtv-fw-dec.bin"
#else
#define IVTV_FIRM_ENC_FILENAME "/lib/modules/ivtv-fw-enc.bin" 
#define IVTV_FIRM_DEC_FILENAME "/lib/modules/ivtv-fw-dec.bin"
#endif
#define IVTV_FIRM_IMAGE_SIZE 256*1024
#define IVTV_FIRM_SEARCH_ENCODER_START 0x00000000
#define IVTV_FIRM_SEARCH_DECODER_START 0x00000000
#define IVTV_FIRM_SEARCH_ENCODER_END (IVTV_ENCODER_OFFSET + IVTV_ENCODER_SIZE - 1)
#define IVTV_FIRM_SEARCH_DECODER_END (IVTV_DECODER_OFFSET + IVTV_DECODER_SIZE - 1)
#define IVTV_FIRM_SEARCH_STEP (0x00000100 / sizeof(u32))
 
/* Firmware mailbox flags*/
#define IVTV_MBOX_FIRMWARE_DONE 0x00000004
#define IVTV_MBOX_DRIVER_DONE 0x00000002
#define IVTV_MBOX_DRIVER_BUSY 0x00000001
#define IVTV_MBOX_FREE 0x00000000

#define IVTV_API_STD_TIMEOUT 0x02000000 /*units??*/

/* fw reset commands */
#define IVTV_CMD_QUICK_RESET 0  /* stop then start firmware, don't re-upload */
#define IVTV_CMD_SOFT_RESET  1  /* quick + find mboxes */
#define IVTV_CMD_FULL_RESET  2  /* full stop/upload/start/find mboxes */

/*Firmware API commands*/
#define IVTV_API_ENC_PING_FW 0x00000080
#define IVTV_API_DEC_PING_FW 0x00000000
#define IVTV_API_ENC_GETVER 0x000000C4
#define IVTV_API_DEC_GETVER 0x00000011
#define IVTV_API_ENC_HALT_FW 0x000000C3
#define IVTV_API_DEC_HALT_FW 0x0000000E
#define IVTV_API_DEC_START_PLAYBACK 0x00000001
#define IVTV_API_DEC_STOP_PLAYBACK 0x00000002
#define IVTV_API_DEC_PLAYBACK_SPEED 0x00000003
#define IVTV_API_DEC_STEP_VIDEO 0x00000005
#define IVTV_API_DEC_PAUSE_PLAYBACK 0x0000000d
#define IVTV_API_DEC_DMA_BLOCKSIZE 0x00000008
#define IVTV_API_DEC_DMA_FROM_HOST 0x00000000b
#define IVTV_API_OSD_DMA_FROM_HOST 0x000000ff
#define IVTV_API_DEC_DISP_STANDARD 0x00000010
#define IVTV_API_DEC_STREAM_INPUT 0x00000014
#define IVTV_API_DEC_TIMING_INFO 0x00000015
#define IVTV_API_DEC_SELECT_AUDIO 0x00000016
#define IVTV_API_DEC_EVENT_NOTIFICATION 0x00000017
#define IVTV_API_DEC_DISPLAY_BUFFERS 0x00000018
#define IVTV_API_DEC_EXTRACT_VBI 0x00000019
#define IVTV_API_DEC_DECODE_SOURCE 0x0000001a
#define IVTV_API_DEC_AUDIO_OUTPUT 0x0000001b
#define IVTV_API_DEC_SET_AV_DELAY 0x0000001c
#define IVTV_API_DEC_BUFFER 0x0000001e
#define IVTV_API_DEC_DMA_STATUS 0x0000000a
#define IVTV_API_DEC_XFER_INFO 0x00000009
#define IVTV_API_ASSIGN_DMA_BLOCKLEN 0x000000c9
#define IVTV_API_ASSIGN_PGM_INDEX_INFO 0x000000c7
#define IVTV_API_ASSIGN_STREAM_TYPE 0x000000b9
#define IVTV_API_ASSIGN_OUTPUT_PORT 0x000000bb
#define IVTV_API_ASSIGN_FRAMERATE 0x0000008f
#define IVTV_API_ASSIGN_FRAME_SIZE 0x00000091
#define IVTV_API_ASSIGN_ASPECT_RATIO 0x00000099
#define IVTV_API_ASSIGN_BITRATES 0x00000095
#define IVTV_API_ASSIGN_GOP_PROPERTIES 0x00000097
#define IVTV_API_ASSIGN_3_2_PULLDOWN 0x000000b1
#define IVTV_API_ASSIGN_GOP_CLOSURE 0x000000c5
#define IVTV_API_ASSIGN_AUDIO_PROPERTIES 0x000000bd
#define IVTV_API_ASSIGN_DNR_FILTER_MODE 0x0000009b
#define IVTV_API_ASSIGN_DNR_FILTER_PROPS 0x0000009d
#define IVTV_API_ASSIGN_CORING_LEVELS 0x0000009f
#define IVTV_API_ASSIGN_SPATIAL_FILTER_TYPE 0x000000a1
#define IVTV_API_ASSIGN_FRAME_DROP_RATE 0x000000d0
#define IVTV_API_ASSIGN_PLACEHOLDER 0x000000d8
#define IVTV_API_MUTE_VIDEO 0x000000d9
#define IVTV_API_MUTE_AUDIO 0x000000da
#define IVTV_API_CONFIG_VBI 0x000000c8
#define IVTV_API_SELECT_VBI_LINE 0x000000b7
#define IVTV_API_INITIALIZE_INPUT 0x000000cd
#define IVTV_API_REFRESH_INPUT 0x000000d3
#define IVTV_API_ASSIGN_NUM_VSYNC_LINES 0x000000d6
#define IVTV_API_BEGIN_CAPTURE 0x00000081
#define IVTV_API_PAUSE_ENCODER 0x000000d2
#define IVTV_API_EVENT_NOTIFICATION 0x000000d5
#define IVTV_API_END_CAPTURE 0x00000082
#define IVTV_API_SCHED_DMA_TO_HOST 0x000000cc
#define IVTV_API_FB_GET_FRAMEBUFFER 0x00000041
#define IVTV_API_FB_GET_PIXEL_FORMAT 0x00000042
#define IVTV_API_FB_SET_PIXEL_FORMAT 0x00000043
#define IVTV_API_FB_GET_STATE 0x00000044
#define IVTV_API_FB_SET_STATE 0x00000045
#define IVTV_API_FB_GET_OSD_COORDS 0x00000046
#define IVTV_API_FB_SET_OSD_COORDS 0x00000047
#define IVTV_API_FB_GET_SCREEN_COORDS 0x00000048
#define IVTV_API_FB_SET_SCREEN_COORDS 0x00000049
#define IVTV_API_FB_GET_GLOBAL_ALPHA 0x0000004a
#define IVTV_API_FB_SET_GLOBAL_ALPHA 0x0000004b
#define IVTV_API_FB_SET_BLEND_COORDS 0x0000004c
// 0x4d unknown
// 0x4e unknown
#define IVTV_API_FB_GET_FLICKER_STATE 0x0000004f
#define IVTV_API_FB_SET_FLICKER_STATE 0x00000050
// 0x51 unknown
#define IVTV_API_FB_BLT_COPY 0x00000052
#define IVTV_API_FB_BLT_FILL 0x00000053
#define IVTV_API_FB_BLT_TEXT 0x00000054
// 0x55 unknown
#define IVTV_API_FB_SET_FRAMEBUFFER_WINDOW 0x00000056
// 0x57 - 0x5f unknown
#define IVTV_API_FB_SET_CHROMA_KEY 0x00000060
#define IVTV_API_FB_GET_ALPHA_CONTENT_INDEX 0x00000061
#define IVTV_API_FB_SET_ALPHA_CONTENT_INDEX 0x00000062

/* i2c stuff */
#define I2C_CLIENTS_MAX 16
#define I2C_TIMING (0x7<<4)
#define IVTV_REG_I2C_SETSCL_OFFSET (0x7000 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_I2C_SETSDA_OFFSET (0x7004 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_I2C_GETSCL_OFFSET (0x7008 /*| IVTV_REG_OFFSET*/)
#define IVTV_REG_I2C_GETSDA_OFFSET (0x700c /*| IVTV_REG_OFFSET*/)

/* debugging */
#define IVTV_DEBUG(x,args...) if((x) & ivtv_debug) printk("ivtv: " args);
#define IVTV_DEBUG_OSD(x,args...) if((x) & ivtv_debug) printk("ivtv-osd: " args);

/* Standard kernel messages */
#define IVTV_KERN_ERR(args...) printk(KERN_ERR "ivtv: " args);
#define IVTV_OSD_KERN_ERR(args...) printk(KERN_ERR "ivtv-osd: " args);
#define IVTV_KERN_INFO(args...) printk(KERN_INFO "ivtv: " args);
#define IVTV_OSD_KERN_INFO(args...) printk(KERN_INFO "ivtv-osd: " args);

#define VBI_RAW_SIZE 		1443
#define VBI_SLICED_SIZE 	51

/* Values for IVTV_API_DEC_PLAYBACK_SPEED mpeg_frame_type_mask parameter: */
#define MPEG_FRAME_TYPE_IFRAME 1
#define MPEG_FRAME_TYPE_IFRAME_PFRAME 3
#define MPEG_FRAME_TYPE_ALL 7

extern int ivtv_debug;

struct ivtv_options {
	int yuv_fixup;		/* Should we re-work YUV to a standard format? */
	int yuv_buffers;	/* How many yuv buffers to allocate? */
	int pcm_buffers;	/* How many pcm buffers to allocate? */
	int mpg_buffers;	/* how many mpg buffers to allocate? */
	int vbi_buffers;	/* how many vbi buffers to allocate? */
	int dec_mpg_buffers;	/* how many decoder mpg buffers to allocate? */
	int dec_yuv_buffers;	/* How many decoder yuv buffers to allocate? */
	int cardtype;		/* force card type on load */
 	int tuner;		/* set tuner on load */
 	int radio;		/* enable/disable radio */
};

struct ivtv_dec_options {
	int hide_last_frame;	/* 0 = display last frame on stop_decode
				 * 1 = display black */
	u32 pts_low;		/* low bits PTS to stop playback at */
	u32 pts_hi;		/* hi bits  PTS to stop playback at */
	int gop_offset;		/* on start-playback, skip this
				 *  # of frames in the GOP */
	int mute_frames;	/* # of audio frames to mute on playback start */
	int decbuffers;		/* 0 = 6 buffers, 1 = 9 buffers */
	int prebuffer;		/* 0 = no prebuffer, 1 = enabled, see docs */
	struct ivtv_speed	speed;
};

/* ivtv-specific mailbox template */
struct ivtv_mailbox {
	u32 flags;
	u32 cmd;
	u32 retval;
	u32 timeout;
	u32 data[IVTV_MBOX_MAX_DATA];
};

/* per buffer bit flags */
#define IVTV_F_B_NEED_BUF_SWAP  0  /* bit 0 for swap needed */

/* per-stream, s_flags */
#define IVTV_F_S_DMAP		0
#define IVTV_F_S_OVERFLOW	1
#define IVTV_F_S_NO_READ	2  /* this stream won't be read from */
#define IVTV_F_S_NO_DMA		3  /* this stream doesn't use DMA */
#define IVTV_F_S_NEEDS_DATA	4	
#define IVTV_F_S_DECODING	5  /* this stream is decoding */
#define IVTV_F_S_CAPTURING	6  /* this stream is capturing */
#define IVTV_F_S_IN_USE 	7  /* this stream is in use, no one else can
				      try to use it. */
#define IVTV_F_S_STREAMOFF	8 /* signal end of stream EOS */ 
#define IVTV_F_S_RESET		9
#define IVTV_F_S_DMA_DONE	10

/* per-ivtv, i_flags */
#define IVTV_F_I_SPEED_CHANGE	0
#define IVTV_F_I_EOS		1
#define IVTV_F_I_RADIO_USER	2
#define IVTV_F_I_PASSTHROUGH	3
#define IVTV_F_I_DIG_PAUSE	4
#define IVTV_F_I_DIG_RST	5
#define IVTV_F_I_MUTE_PAUSE	6
#define IVTV_F_I_DEC_YUV	7

/* dma-tasklet, dma-thread, t_flags */
/* tasklets */
#define IVTV_F_T_DEC_VSYNC	0
#define IVTV_F_T_DEC_VBI_INSERT	1
#define IVTV_F_T_ENC_DMA_NEEDED 2
#define IVTV_F_T_ENC_DMA_DONE 	3
#define IVTV_F_T_ENC_VBI_DMA 	4
#define IVTV_F_T_DEC_SCHED	5
#define IVTV_F_T_DEC_DONE	6

/* firmware-reset, r_flags */
#define FW_RESET_NEEDED        	0
#define OSD_RESET_NEEDED       	1
#define DEC_DMA_ERR		3
#define FW_SOFT_RESET_NEEDED  	4
#define FW_RESET_LOCK	  	5
#define FW_RESET_SHUTDOWN  	6

/* user DMA , u_flags */
#define DMA_IN_USE        	0

/* Scatter-Gather array element, used in DMA transfers */
struct ivtv_SG_element {
	u32 src;
	u32 dst;
	u32 size;
};

struct ivtv_buffer {
	size_t			readpos;
	dma_addr_t		dma_handle;
	struct v4l2_buffer	buffer;
        struct list_head        list;
	unsigned long		ts;
	unsigned long   	b_flags;
};

struct ivtv_buffer_list {
	atomic_t		elements;
        struct list_head        list;
};

struct ivtv_user_dma {
        int 			page_count;
	int 			offset;
        struct page 		**map;

	unsigned long		u_flags; /* status flags */

        /* Base Dev SG Array for iTVC15/16 */
        struct ivtv_SG_element  *SGarray;
        dma_addr_t 		SG_handle;
        int                     SG_length;

        /* SG List of Buffers */
        struct scatterlist      *SGlist;
        int                     SGlen;

	unsigned long 		sg_size;

	wait_queue_head_t       waitq;
};

struct ivtv_stream {
	long			id;
	long			seq;
	int			ubytes; /* bytes written back to 
					   user this frame */
	unsigned long		s_flags; /* status flags, see above */
	int 			dma;  /* can be PCI_DMA_TODEVICE, 
					 PCI_DMA_FROMDEVICE or
					 PCI_DMA_NONE */
	unsigned long		trans_id;
	int 			subtype;
	int 			type;
	int 			dmatype;
	u64 			pts; /* last seen Program Time Stamp (PTS) */
	wait_queue_head_t	waitq;
	struct video_device    *v4l2dev;

	/* Buffer Stats */ 
	atomic_t		allocated_buffers;
	int 			buffers;
	u32 			buf_min;
	u32 			buf_max;
	int 			bufsize;
	u32			buf_total;
	u32			buf_fill;

	/* Buffer Queues */
	struct ivtv_buffer_list free_q; /* unused buffers */
	struct ivtv_buffer_list full_q; /* filled buffers */
	struct ivtv_buffer_list dma_q;  /* awaiting dma to fill them */
					/* only updated in interrupt time! */
	struct ivtv_buffer_list io_q;

	/* Base Dev SG Array for iTVC15/16 */
        struct ivtv_SG_element *SGarray;
        dma_addr_t 		SG_handle;
        int 			SG_length;

	/* SG List of Buffers */
	struct scatterlist	*SGlist;
	int			SGlen;

	/* User based DMA for OSD */
	struct ivtv_user_dma    udma;

	/* Locking */
        spinlock_t 		slock;
        struct semaphore 	mlock;

	/* Redo DMA? */
	atomic_t 		redo_dma;
	atomic_t 		stolen_bufs;

};

struct ivtv_open_id {
        int                     open_id;
        int                     type;
        struct ivtv             *itv;
};

/* dualwatch thread and flags */
/* audio_bitmask: 
 * bits 0-15 follow iso11172, mostly, see http://ivtv.sourceforge.net/firmware-api.html. 
 */
#define IVTV_CODEC_AUDIO_MPEG_STEREO_MASK        (0x03 << 8)
#define IVTV_CODEC_AUDIO_MPEG_STEREO_STEREO      (0x00 << 8)
#define IVTV_CODEC_AUDIO_MPEG_STEREO_JOINT       (0x01 << 8)
#define IVTV_CODEC_AUDIO_MPEG_STEREO_DUAL        (0x02 << 8)
#define IVTV_CODEC_AUDIO_MPEG_STEREO_MONO        (0x03 << 8)

struct ivtv_dualwatch {
        int enabled; /* 1=stereo mode enabled */
        int encoding; /* 1=ivtv currently encoding */
        u32 audio_bitmask; /* current real value of audio_bitmask */

        /* control thread */
        struct task_struct *thread;
        struct semaphore startstop_sem;
        wait_queue_head_t queue;
        int terminate;
};

/* Save API commands in structure */
#define DECODER_API_OFFSET 63  /* if < 63 then decoder */
#define OSD_API_OFFSET	   127 /* if < 127 then osd */
#define ENCODER_API_OFFSET 255 /* else encoder */

#define MBOX_TIMEOUT	(HZ*1800) /* jiffies */

struct api_cmd {
        int marked;  /* is this used */
        u64 jiffies; /* last command issued */

        u32 s_data[IVTV_MBOX_MAX_DATA]; /* send api data */
        u32 r_data[IVTV_MBOX_MAX_DATA]; /* returned api data */
};

/* forward declaration of struct defined in ivtv-cards.h */
struct ivtv_card;

/* Struct to hold info about ivtv cards */
struct ivtv {
	const struct ivtv_card *card;	/* card information */
	int has_itvc15; 	/* 1 if it is an iTVC15 based card, 0 for iTVC16 */
	u32 v4l2_cap;  		/* V4L2 capabilities of card */

	struct pci_dev *dev;
	struct ivtv_options	options;
	struct ivtv_dec_options	dec_options;
	int num;                /* invalidate during init! */
	char name[8];           /* name for printk and interupts ala bttv */
	int first_read;		/* used to clean up stream */
	unsigned long i_flags;
	atomic_t capturing;  	/* count number of active capture streams */
	atomic_t decoding;	/* count number of active decoding streams */
	atomic_t dec_dma_needed;

       	/* Semaphore to ensure stream changes do not happen concurrently. To be
           precise: while this lock is held, no other process can start/stop,
	   pause/resume or change codecs/formats for any stream.
	 */
	struct semaphore streams_lock;
	struct semaphore i2c_lock;
	struct semaphore DMA_lock;
	spinlock_t       DMA_slock;

	int open_id;      /* incremented each time an open occurs, used as unique ID.
			     starts at 1, so 0 can be used as uninitialized value
			     in the stream->id. */

	unsigned long trans_id; 

	u32 enc_fw_ver, dec_fw_ver, base_addr; /*is base_addr needed? */
	u32 irqmask;
	
	/* VBI data */
        u32 vbi_dec_start, vbi_dec_size;
        u32 vbi_enc_start, vbi_enc_size;
	int vbi_index;
	int vbi_total_frames;
	unsigned long vbi_frame;
	int vbi_dec_mode;
	int vbi_dec_io_size;
	int vbi_passthrough;
	u8  vbi_cc_data[4];
	u8  vbi_cc_mode;
	u8  vbi_cc_no_update;
	u8  vbi_vps[5];
	u8  vbi_vps_found;
	int vbi_wss;
	u8  vbi_wss_found;
	u8  vbi_wss_no_update;
	u32 vbi_service_set_in;
	u32 vbi_service_set_out;
	int vbi_insert_mpeg;

        /* Buffer for the maximum of 2 * 18 * packet_size sliced VBI lines.
	   One for /dev/vbi0 and one for /dev/vbi4 */
        struct {
                u32 id;
                u32 line;
                u8 data[44];
        } vbi_sliced_data[36], vbi_sliced_dec_data[36];
	/* Buffer for VBI data inserted into MPEG stream.
	   The first byte is a dummy byte that's never used.
	   The next 16 bytes contain the MPEG header for the VBI data,
	   the remainder is the actual VBI data.
	   The max size accepted by the MPEG VBI reinsertion turns out
	   to be 1552 bytes, which happens to be 4 + (1 + 42) * (2 * 18) bytes,
	   where 4 is a four byte header, 42 is the max sliced VBI payload, 1 is
	   a single line header byte and 2 * 18 is the number of VBI lines per frame.

	   However, it seems that the data must be 1K aligned, so we have to
	   pad the data until the 1 or 2 K boundary. */
	u8 vbi_sliced_mpeg_data[20][2049];
	int vbi_sliced_mpeg_offset[20];
	int vbi_sliced_mpeg_size[20];
	int vbi_search_ba;
	unsigned long vbi_inserted_frame;
	struct timer_list vbi_passthrough_timer;

       	struct ivtv_mailbox *enc_mbox, *dec_mbox;
	struct semaphore enc_msem;
	struct semaphore dec_msem;

	unsigned char card_rev, *enc_mem, *dec_mem, *reg_mem;

	u32 	idx_sdf_offset;
	u32 	idx_sdf_num;
	u32	idx_sdf_mask;

	wait_queue_head_t cap_w, vsync_w;

	/* Decoder */
	struct ivtv_ioctl_framesync dec_timestamp;

	/* Framebuffer DMA support */
	int	fb_id;

	/* i2c */
	struct i2c_adapter         i2c_adap;
	struct i2c_algo_bit_data   i2c_algo;
	struct i2c_client          i2c_client;
	int                        i2c_state;
	struct i2c_client         *i2c_clients[I2C_CLIENTS_MAX];

	/* v4l2 and User settings*/

        /* codec settings */
        struct ivtv_ioctl_codec codec;
	u32 			audmode_tv;
	u32 			audmode_radio;
	u32 			freq_tv;
	u32 			freq_radio;
	u32			audio_input_tv;
	u32			audio_input_radio;
	u32			msp34xx_audio_output;
	u32 			active_input;
	u32 			active_output;
	v4l2_std_id 		std;
	v4l2_std_id 		tuner_std;  /* The norm of the tuner (fixed) */
	u32 			width, height;
	u32 			vbi_start[2], vbi_count;
	int			streamcount; /* Number of elements in streams */
	struct ivtv_stream	*streams;

        /* dualwatch */
        struct ivtv_dualwatch   dualwatch;

	/* Firmware Reset */
	int fw_reset_counter;
	unsigned long r_flags;

	/* Tasklet Flags */
	unsigned long t_flags;

	int     end_gop;

	/* Digitizer type */
	int 	digitizer; /* 0x00EF = saa7114 0x00FO = saa7115 0x0106 = mic*/

	/* alpha value for osd firmware resets */
	int global_alpha; 
	int global_alpha_state; 
	int local_alpha_state; 

        /* API Commands */
        struct api_cmd          api[256];
	
	int 			via_fix;

	atomic_t		streams_setup;

#ifdef LINUX26
	struct workqueue_struct *vbi_work_queues;
	struct workqueue_struct *fw_work_queues;
	struct work_struct      vbi_work_queue;
	struct work_struct      fw_work_queue;
#else
	struct tq_struct        vbi_work_queue;
	struct tq_struct        fw_work_queue;
#endif

	struct ivtv_dec_dma		dec_dma_stat;
	struct ivtv_dma_settings	dma_cfg;

	/* Decoder Thread */
	wait_queue_head_t       dec_thread_w;
        pid_t                   dec_thread_pid;
        atomic_t                dec_thread_running;

	/* Encoder Thread */
	wait_queue_head_t       enc_thread_w;
        pid_t                   enc_thread_pid;
        atomic_t                enc_thread_running;

	atomic_t		enc_dma_stat_intr;
};

/* Globals */
extern struct ivtv ivtv_cards[];
extern int ivtv_cards_active;
extern int ivtv_first_minor;
extern spinlock_t ivtv_cards_lock;
extern int tda9887; /*SECAM*/

/*==============Prototypes==================*/

void dec_schedule_work(struct ivtv *itv);
void enc_schedule_work(struct ivtv *itv);
void fw_schedule_work(struct ivtv *itv);

#ifdef LINUX26
int ivtv_schedule_work(struct work_struct *work);
#else
int ivtv_schedule_work(struct tq_struct *work);
#endif
void ivtv_flush_scheduled_work(struct ivtv *itv);

/* Hardware/IRQ */
void ivtv_set_irq_mask(struct ivtv *itv, unsigned long mask);
void ivtv_clear_irq_mask(struct ivtv *itv, unsigned long mask);

/* Return non-zero if a signal is pending */
int ivtv_sleep_timeout(int timeout, int intr);


/* debug stuff, to get the locking right */
#ifndef WARN_ON
#define WARN_ON(condition) do { \
	if (unlikely((condition)!=0)) { \
		printk("Badness in %s at %s:%d\n", __FUNCTION__, __FILE__, __LINE__); \
		dump_stack(); \
	} \
} while (0)
#endif

#define IVTV_ASSERT(x)	WARN_ON(!(x))

static inline int ivtv_sem_count(struct semaphore *sem)
{
	return atomic_read(&sem->count);
}

static inline int std2norm(v4l2_std_id std)
{
	return (std & V4L2_STD_PAL) ? VIDEO_MODE_PAL :
		((std & V4L2_STD_SECAM) ? VIDEO_MODE_SECAM : VIDEO_MODE_NTSC);
}

#endif
