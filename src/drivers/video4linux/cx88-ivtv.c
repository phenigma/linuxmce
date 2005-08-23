/*
 * $Id: cx88-ivtv.c,v 1.1.2.2 2005/08/16 10:33:37 catalin Exp $
 *
 *  IVTV API emulation for the "blackbird" reference design.
 *
 *    (c) 2005 Catalin Climov <catalin@climov.com>
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
#include <linux/kernel.h>
#include <linux/init.h>

#include "compat.h"
#include "cx88.h"

MODULE_DESCRIPTION("ivtv ioctl emulation module for blackbird TV cards");
MODULE_AUTHOR("Catalin Climov <catalin@climov.com>");
MODULE_LICENSE("GPL");

static unsigned int debug = 0;
module_param(debug,int,0644);
MODULE_PARM_DESC(debug,"enable debug messages [blackbird]");

#define dprintk(level,fmt, arg...)	if (debug >= level) \
	printk(KERN_DEBUG "%s/2-bb: " fmt, dev->core->name , ## arg)

/* blackbird stuff */
enum blackbird_framerate {
	BLACKBIRD_FRAMERATE_NTSC_30, /* NTSC: 30fps */
	BLACKBIRD_FRAMERATE_PAL_25   /* PAL: 25fps */
};
enum blackbird_video_bitrate_type {
	BLACKBIRD_VIDEO_VBR,
	BLACKBIRD_VIDEO_CBR
};
enum blackbird_aspect_ratio {
	BLACKBIRD_ASPECT_RATIO_FORBIDDEN,
	BLACKBIRD_ASPECT_RATIO_1_1_SQUARE,
	BLACKBIRD_ASPECT_RATIO_4_3,
	BLACKBIRD_ASPECT_RATIO_16_9,
	BLACKBIRD_ASPECT_RATIO_221_100,
	BLACKBIRD_ASPECT_RATIO_RESERVED
};
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

static int (*prev_ioctl_hook)(struct inode *inode, struct file *file,
			      unsigned int cmd, void *arg);
static unsigned int (*prev_ioctl_translator)(unsigned int cmd);

/* --- IVTV data structs -------------------------------------------- */

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
#define IVTV_IOC_G_CODEC           _IOR ('@', 48, struct ivtv_ioctl_codec)
#define IVTV_IOC_S_CODEC           _IOW ('@', 49, struct ivtv_ioctl_codec)

/* ------------------------------------------------------------------ */

static unsigned int ivtv_translate_ioctl(unsigned int cmd)
{
#if 0
	printk( KERN_INFO "ivtv_translate_ioctl\n" );
#endif
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
	return prev_ioctl_translator( cmd );
}

static void ivtv_print_codec( struct ivtv_ioctl_codec *codec )
{
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
}

static enum blackbird_stream_type mpeg_stream_types[] = {
	[V4L2_MPEG_SS_1]   = BLACKBIRD_STREAM_MPEG1,
	[V4L2_MPEG_PS_2]   = BLACKBIRD_STREAM_PROGRAM,
	[V4L2_MPEG_TS_2]   = BLACKBIRD_STREAM_TRANSPORT,
	[V4L2_MPEG_PS_DVD] = BLACKBIRD_STREAM_DVD,
};
static enum v4l2_mpeg_streamtype v4l2_stream_types[] = {
	[BLACKBIRD_STREAM_PROGRAM]   = V4L2_MPEG_PS_2,
	[BLACKBIRD_STREAM_TRANSPORT] = V4L2_MPEG_TS_2,
	[BLACKBIRD_STREAM_MPEG1]     = V4L2_MPEG_SS_1,
	[BLACKBIRD_STREAM_DVD]       = V4L2_MPEG_PS_DVD,
};
static enum blackbird_aspect_ratio mpeg_stream_ratios[] = {
	[V4L2_MPEG_ASPECT_SQUARE] = BLACKBIRD_ASPECT_RATIO_1_1_SQUARE,
	[V4L2_MPEG_ASPECT_4_3]    = BLACKBIRD_ASPECT_RATIO_4_3,
	[V4L2_MPEG_ASPECT_16_9]   = BLACKBIRD_ASPECT_RATIO_16_9,
	[V4L2_MPEG_ASPECT_1_221]  = BLACKBIRD_ASPECT_RATIO_221_100,
};
static enum v4l2_mpeg_aspectratio v4l2_aspect_ratios[] = {
	[BLACKBIRD_ASPECT_RATIO_1_1_SQUARE] = V4L2_MPEG_ASPECT_SQUARE,
	[BLACKBIRD_ASPECT_RATIO_4_3]        = V4L2_MPEG_ASPECT_4_3,
	[BLACKBIRD_ASPECT_RATIO_16_9]       = V4L2_MPEG_ASPECT_16_9,
	[BLACKBIRD_ASPECT_RATIO_221_100]    = V4L2_MPEG_ASPECT_1_221,
};
static enum blackbird_video_bitrate_type mpeg_video_bitrates[] = {
	[V4L2_BITRATE_NONE] = BLACKBIRD_VIDEO_CBR,
	[V4L2_BITRATE_CBR]  = BLACKBIRD_VIDEO_CBR,
	[V4L2_BITRATE_VBR]  = BLACKBIRD_VIDEO_VBR,
};
struct bitrate_bits {
	u32 bits; /* layer bits for the best fit */
	u32 rate; /* actual numeric value for the layer best fit */
};
struct bitrate_approximation {
	u32                 target;   /* numeric value of the rate we want */
	struct bitrate_bits layer[2];
};
static struct bitrate_approximation mpeg_audio_bitrates[] = {
	/* target  layer[0].bits           layer[0].rate       layer[1].bits           layer[1].rate */
	{   0, { {                                0,   0, }, {                                0,   0, }, }, },
	{  32, { { BLACKBIRD_AUDIO_BITS_LAYER_1_32 ,  32, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_32 ,  32, }, }, },
	{  48, { { BLACKBIRD_AUDIO_BITS_LAYER_1_64 ,  64, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_48 ,  48, }, }, },
	{  56, { { BLACKBIRD_AUDIO_BITS_LAYER_1_64 ,  64, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_56 ,  56, }, }, },
	{  64, { { BLACKBIRD_AUDIO_BITS_LAYER_1_64 ,  64, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_64 ,  64, }, }, },
	{  80, { { BLACKBIRD_AUDIO_BITS_LAYER_1_96 ,  96, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_80 ,  80, }, }, },
	{  96, { { BLACKBIRD_AUDIO_BITS_LAYER_1_96 ,  96, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_96 ,  96, }, }, },
	{ 112, { { BLACKBIRD_AUDIO_BITS_LAYER_1_128, 128, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_112, 112, }, }, },
	{ 128, { { BLACKBIRD_AUDIO_BITS_LAYER_1_128, 128, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_128, 128, }, }, },
	{ 160, { { BLACKBIRD_AUDIO_BITS_LAYER_1_160, 160, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_160, 160, }, }, },
	{ 192, { { BLACKBIRD_AUDIO_BITS_LAYER_1_192, 192, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_192, 192, }, }, },
	{ 224, { { BLACKBIRD_AUDIO_BITS_LAYER_1_224, 224, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_224, 224, }, }, },
	{ 256, { { BLACKBIRD_AUDIO_BITS_LAYER_1_256, 256, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_256, 256, }, }, },
	{ 288, { { BLACKBIRD_AUDIO_BITS_LAYER_1_288, 288, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_320, 320, }, }, },
	{ 320, { { BLACKBIRD_AUDIO_BITS_LAYER_1_320, 320, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_320, 320, }, }, },
	{ 352, { { BLACKBIRD_AUDIO_BITS_LAYER_1_352, 352, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_384, 384, }, }, },
	{ 384, { { BLACKBIRD_AUDIO_BITS_LAYER_1_384, 384, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_384, 384, }, }, },
	{ 416, { { BLACKBIRD_AUDIO_BITS_LAYER_1_416, 416, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_384, 384, }, }, },
	{ 448, { { BLACKBIRD_AUDIO_BITS_LAYER_1_448, 448, }, { BLACKBIRD_AUDIO_BITS_LAYER_2_384, 384, }, }, },
};
static const int BITRATES_SIZE = ARRAY_SIZE(mpeg_audio_bitrates);

static int ivtv_do_ioctl(struct inode *inode, struct file *file,
                         unsigned int cmd, void *arg)
{
	struct cx8802_fh  *fh                = file->private_data;
	struct cx8802_dev *dev               = fh->dev;
	struct v4l2_mpeg_compression *params = &dev->params;
	struct blackbird_dnr *dnr_params     = &dev->dnr_params;
	/* struct cx88_core  *core = dev->core; */

	if (debug > 1)
		cx88_print_ioctl(dev->core->name,cmd);
#if 0
	printk( KERN_INFO "IVTV IOCTL: 0x%x\n", cmd );
	cx88_print_ioctl(dev->core->name,cmd);
#endif
	dprintk( 1, "IVTV IOCTL: 0x%x\n", cmd );

	switch (cmd) {

	/* --- IVTV emulation layer --------------------------- */
	case IVTV_IOC_S_CODEC:
	{
		struct ivtv_ioctl_codec *codec = arg;
		struct v4l2_mpeg_compression *mpeg;
		struct blackbird_dnr *dnr;
#if 1
		printk( KERN_INFO "IVTV_IOC_S_CODEC\n" );
		ivtv_print_codec( codec );
#endif
		mpeg = kmalloc(sizeof(*mpeg),GFP_KERNEL);
		if( mpeg == NULL )
			return -ENOMEM;
		dnr = kmalloc(sizeof(*dnr),GFP_KERNEL);
		if( dnr == NULL )
		{
			kfree( mpeg );
			return -ENOMEM;
		}
		memcpy(mpeg,params,sizeof(*mpeg));
		memcpy(dnr,dnr_params,sizeof(*dnr));

		mpeg->vi_bframes_count  = codec->bframes;
		mpeg->vi_bitrate.target = codec->bitrate/1000;
		mpeg->vi_bitrate.max    = codec->bitrate_peak/1000;
		mpeg->vi_frames_per_gop = codec->framespergop;
		mpeg->closed_gops       = codec->gop_closure;
		mpeg->pulldown          = codec->pulldown;
		/* TODO: check param bounds */
		mpeg->st_type           = v4l2_stream_types[codec->stream_type];
		mpeg->vi_type           = codec->stream_type == BLACKBIRD_STREAM_MPEG1
						? V4L2_MPEG_VI_1 : V4L2_MPEG_VI_2;
		mpeg->vi_frame_rate     = codec->framerate == BLACKBIRD_FRAMERATE_PAL_25 ? 25 : 30;
		mpeg->vi_aspect_ratio   = v4l2_aspect_ratios[codec->aspect];
		mpeg->vi_bitrate.mode   = codec->bitrate_mode == BLACKBIRD_VIDEO_VBR
						? V4L2_BITRATE_VBR : V4L2_BITRATE_CBR;

		switch( codec->audio_bitmask & 0x3 )
		{
		case BLACKBIRD_AUDIO_BITS_44100HZ:
			mpeg->au_sample_rate = 44100; break;
		case BLACKBIRD_AUDIO_BITS_48000HZ:
			mpeg->au_sample_rate = 48000; break;
		case BLACKBIRD_AUDIO_BITS_32000HZ:
		default:
			mpeg->au_sample_rate = 32000; break;
		}

		switch( codec->audio_bitmask & ( 0x3 << 2 ) )
		{
		case BLACKBIRD_AUDIO_BITS_LAYER_2:
			mpeg->au_type = V4L2_MPEG_AU_2_II; break;
		case BLACKBIRD_AUDIO_BITS_LAYER_1:
		default:
			mpeg->au_type = V4L2_MPEG_AU_2_I; break;
		}

		mpeg->au_bitrate.mode   = mpeg->vi_bitrate.mode;

		int layer = mpeg->au_type;
		int i;
		for(i = 0; i < BITRATES_SIZE; i++)
		{
			if( (codec->audio_bitmask&(0x0F<<4)) == mpeg_audio_bitrates[i].layer[layer].bits )
			{
				mpeg->au_bitrate.target = mpeg_audio_bitrates[i].layer[layer].rate;
				break;
			}
		}
		mpeg->au_bitrate.max    = mpeg->au_bitrate.target;

		dnr->mode     = codec->dnr_mode;
		dnr->spatial  = codec->dnr_spatial;
		dnr->temporal = codec->dnr_temporal;
		dnr->type     = codec->dnr_type;

		blackbird_set_params( dev, mpeg );
		blackbird_set_dnr_params( dev, dnr );

		kfree( dnr );
		kfree( mpeg );
	}
	case IVTV_IOC_G_CODEC:
	{
		struct ivtv_ioctl_codec *codec = arg;

		memset(codec, 0, sizeof( codec ));

		codec->bframes      = params->vi_bframes_count;
		codec->bitrate      = params->vi_bitrate.target * 1000; /* bps */
		codec->bitrate_peak = params->vi_bitrate.max * 1000;
		codec->framespergop = params->vi_frames_per_gop;
		codec->gop_closure  = params->closed_gops;
		codec->pulldown     = params->pulldown;
		codec->dnr_mode     = dnr_params->mode;
		codec->dnr_spatial  = dnr_params->spatial;
		codec->dnr_temporal = dnr_params->temporal;
		codec->dnr_type     = dnr_params->type;
		codec->stream_type  = mpeg_stream_types[params->st_type];
		codec->framerate    = params->vi_frame_rate == 25 ? BLACKBIRD_FRAMERATE_PAL_25 : BLACKBIRD_FRAMERATE_NTSC_30;
		codec->aspect       = mpeg_stream_ratios[params->vi_aspect_ratio];
		codec->bitrate_mode = mpeg_video_bitrates[params->vi_bitrate.mode];

		codec->audio_bitmask = 0;
		switch( params->au_sample_rate )
		{
			case 32000:
				codec->audio_bitmask |= BLACKBIRD_AUDIO_BITS_32000HZ;
				break;
			case 44100:
				codec->audio_bitmask |= BLACKBIRD_AUDIO_BITS_44100HZ;
				break;
			default: /* 48000 */
				codec->audio_bitmask |= BLACKBIRD_AUDIO_BITS_48000HZ;
		}
		codec->audio_bitmask |= params->au_type == V4L2_MPEG_AU_2_I
					? BLACKBIRD_AUDIO_BITS_LAYER_1
					: BLACKBIRD_AUDIO_BITS_LAYER_2;
		if( params->au_bitrate.mode )
		{
			int layer = params->au_type;
			if( params->au_bitrate.target == 0 )
				codec->audio_bitmask |= 0;
			else
			{
				int i;
				for(i = 0; i < BITRATES_SIZE; i++)
				{
					if( params->au_bitrate.target == mpeg_audio_bitrates[i].layer[layer].rate )
					{
						codec->audio_bitmask |= mpeg_audio_bitrates[i].layer[layer].bits;
						break;
					}
				}
			}
		}
		else
			codec->audio_bitmask |= 0;
#if 1
		printk( KERN_INFO "IVTV_IOC_G_CODEC\n" );
		ivtv_print_codec( codec );
#endif
		return 0;
	}

	default:
		if( prev_ioctl_hook )
			return prev_ioctl_hook( inode, file, cmd, arg );
		else
			return -EINVAL;
	}
	return 0;
}

static int __init cx88_ivtv_init(void)
{
	printk(KERN_INFO "ivtv emulation for blackbird version %d.%d.%d loaded\n",
		(CX88_VERSION_CODE >> 16) & 0xff,
		(CX88_VERSION_CODE >>  8) & 0xff,
		CX88_VERSION_CODE & 0xff);
#ifdef SNAPSHOT
	printk(KERN_INFO "cx2388x: snapshot date %04d-%02d-%02d\n",
		SNAPSHOT/10000, (SNAPSHOT/100)%100, SNAPSHOT%100);
#endif
	request_module( "cx88-blackbird" );
	prev_ioctl_translator = cx88_ioctl_translator;
	cx88_ioctl_translator = ivtv_translate_ioctl;
	prev_ioctl_hook = cx88_ioctl_hook;
	cx88_ioctl_hook = ivtv_do_ioctl;
	return 0;
}

static void __exit cx88_ivtv_fini(void)
{
	cx88_ioctl_hook = prev_ioctl_hook;
	cx88_ioctl_translator = prev_ioctl_translator;
}

module_init(cx88_ivtv_init);
module_exit(cx88_ivtv_fini);

/*
 * kate: eol "unix"; indent-width 3; remove-trailing-space on; replace-trailing-space-save on; tab-width 8; replace-tabs off; space-indent off; mixed-indent off
 */
