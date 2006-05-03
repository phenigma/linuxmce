/* 
 * saa717x - Philips SAA717xHL video decoder driver version 0.0.1
 *
 * Copyright (C) 2002 Maxim Yevtyushkin <max@linuxmedialabs.com>
 *                    
 * Based on saa7111 driver by Dave Perks
 * Based on saa7115 driver by Maxim Yevtyushkin
 * 
 * Copyright (C) 1998 Dave Perks <dperks@ibm.net>
 *
 * Slight changes for video timing and attachment output by
 * Wolfgang Scherr <scherr@net4you.net>
 *
 * Changes by Ronald Bultje <rbultje@ronald.bitfreak.net>
 *    - moved over to linux>=2.4.x i2c protocol (1/1/2003)
 *
 * Changes by Kevin Thayer <nufan_wfk at yahoo.com>
 *    - changed to saa7115. (2/17/2003)
 * 
 * Changes by Ohta Kyuma <alpha292@bremen.or.jp>
 *    - Apply to SAA717x,NEC uPD64031,uPD64083. (1/31/2004)
 *
 * Changes by T.Adachi (tadachi@tadachi-net.com)
 *    - support audio, video scaler etc, and ckecked the initialize sequence. 
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
 * NOTE:
 *  I wrote the comment to the place of a register value. But, these are
 *  referenced datasheet of SAA7134. Because we were not able to
 *  obtain the datasheet of saa 7173/4. It seems the functions of almost
 *  registers are the same as SAA7133. But, the correctness of the contents
 *  is not guaranteed.
 *
 *  We implemented the initialization sequences that were based on the
 *  result of the capture of I2C bus on the Windows driver. Therefore, we
 *  are checked only it on the environment of MPG600GR(NTSC) w/uPD64031A+uPD64083.
 *      --tadachi
 */

#include "ivtv-compat.h"
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <media/v4l2-common.h>
#include <media/audiochip.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#include <media/i2c-compat.h>
#endif

MODULE_DESCRIPTION("Philips SAA717x video decoder driver");
MODULE_AUTHOR("Kevin Thayer, K. Ohta");
MODULE_LICENSE("GPL");

#ifndef I2C_DRIVERID_SAA717X
#define I2C_DRIVERID_SAA717X I2C_DRIVERID_EXP3
#endif

static int debug = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(debug, int, 0644);
#else
MODULE_PARM(debug, "i");
#endif
MODULE_PARM_DESC(debug, "Debug level (0-1)");

struct saa717x_state {
	int norm;
	int input;
	int enable;
	int radio;
	int bright;
	int contrast;
	int hue;
	int sat;
	int playback;
	int audio;
	// Add 20041101 K.Ohta
	int tuner_audio_mode;
	int audio_main_mute;
	int audio_main_vol_r;
	int audio_main_vol_l;
	u16 audio_main_bass;
	u16 audio_main_treble;
	u16 audio_main_volume;
	u16 audio_main_balance;
	int input_type;
	int audio_input;
};

/* ----------------------------------------------------------------------- */

static u8 readreg(struct i2c_client *client, unsigned char reg1,unsigned char reg2)
{
        struct i2c_adapter *adap=client->adapter;
        unsigned char mm1[] = {0x01,0x1e};
        unsigned char mm2[] = {0x00};
        struct i2c_msg msgs[2];
        
        msgs[0].flags=0;
        msgs[1].flags=I2C_M_RD;
        msgs[0].addr=msgs[1].addr=client->addr;
        mm1[0]=reg1;
        mm1[1]=reg2;
        msgs[0].len=2; msgs[1].len=1;
        msgs[0].buf=mm1; msgs[1].buf=mm2;
        i2c_transfer(adap, msgs, 2);

        v4l_dbg(2, debug, client,
		"read:  reg 0x%02x%02x=0x%02x\n",
		reg1,reg2,mm2[0]); 
       
        return mm2[0];
}

static u32 readreg3(struct i2c_client *client, unsigned char reg1,unsigned char reg2)
{
        struct i2c_adapter *adap=client->adapter;
        unsigned char mm1[] = {0x01,0x1e};
        unsigned char mm2[] = {0x00,0x00,0x00};
        struct i2c_msg msgs[2];
        
        msgs[0].flags=0;
        msgs[1].flags=I2C_M_RD;
        msgs[0].addr=msgs[1].addr=client->addr;
        mm1[0]=reg1;
        mm1[1]=reg2;
        msgs[0].len=2; msgs[1].len=3;
        msgs[0].buf=mm1; msgs[1].buf=mm2;
        i2c_transfer(adap, msgs, 2);

        v4l_dbg(1, debug, client,
		"read:  reg 0x%02x%02x=0x%02x,0x%02x,0x%02x\n",
		reg1,reg2,mm2[0],mm2[1],mm2[2]); 
       
        return ((((mm2[0]<<8) | mm2[1])<<8) | mm2[2]);
}


// for audio mode
#define TUNER_AUDIO_MONO   0  // LL
#define TUNER_AUDIO_STEREO 1  // LR
#define TUNER_AUDIO_LANG1  2  // LL
#define TUNER_AUDIO_LANG2  3  // RR

#define SAA_7114_NTSC_HOFFSET		(6)
#define SAA_7114_NTSC_VOFFSET           (10)
#define SAA_7114_NTSC_WIDTH             (704)
#define SAA_7114_NTSC_HEIGHT            (480) /* was 250*/

#define SAA_7114_SECAM_HSYNC_START      (-17)
#define SAA_7114_SECAM_HSYNC_STOP       (-32)

#define SAA_7114_SECAM_HOFFSET          (2)
#define SAA_7114_SECAM_VOFFSET          (10)
#define SAA_7114_SECAM_WIDTH            (720)
#define SAA_7114_SECAM_HEIGHT           (300)

#define SAA_7114_PAL_HSYNC_START        (-17)
#define SAA_7114_PAL_HSYNC_STOP         (-32)

#define SAA_7114_PAL_HOFFSET            (2)
#define SAA_7114_PAL_VOFFSET            (10)
#define SAA_7114_PAL_WIDTH              (720)
#define SAA_7114_PAL_HEIGHT             (300)

#define SAA_7114_VERTICAL_CHROMA_OFFSET         0	//0x50504040
#define SAA_7114_VERTICAL_LUMA_OFFSET           0

#define LOBYTE(x) ((unsigned char)((x) & 0xff))
#define HIBYTE(x) ((unsigned char)(((x) >> 8) & 0xff))
#define LOWORD(x) ((unsigned short int)((x) & 0xffff))
#define HIWORD(x) ((unsigned short int)(((x) >> 16) & 0xffff))


/* ----------------------------------------------------------------------- */

static int saa717x_write(struct i2c_client *client, u8 reg1, u8 reg2, u8 value)
{
	struct i2c_adapter *adap=client->adapter;
	unsigned char mm1[3];
	unsigned char mm2[1];
	struct i2c_msg msgs[2];

	msgs[0].flags=0;
	msgs[1].flags=0;
	msgs[0].addr=msgs[1].addr=client->addr;
	mm1[0]=reg1;
	mm1[1]=reg2;
	mm1[2] = value; /* Very Dirty :-( */
	msgs[0].len=3; msgs[1].len=1;
	msgs[0].buf=mm1; msgs[1].buf=mm2;
	i2c_transfer(adap, msgs, 1);

	v4l_dbg(2, debug, client,
		"wrote:  reg 0x%02x%02x=%02x\n",
		reg1,reg2,value); 

	return 0;   
}

static int saa717x_read(struct i2c_client *client, u8 reg1, u8 reg2)
{
	struct i2c_adapter *adap=client->adapter;
	unsigned char mm1[2];
	unsigned char mm2[1];
	struct i2c_msg msgs[2];

	msgs[0].flags=0;
	msgs[1].flags=I2C_M_RD;
	msgs[0].addr=msgs[1].addr=client->addr;
	mm1[0] = reg1;
	mm1[1] = reg2;
	mm2[0] = 0; /* Very Dirty :-( */
	msgs[0].len=2; msgs[1].len=1;
	msgs[0].buf=mm1; msgs[1].buf=mm2;
	i2c_transfer(adap, msgs, 2);
	v4l_dbg(2, debug, client,
		"read:  reg 0x%02x%02x=0x%02x\n",
		reg1,reg2,mm2[0]); 

	return mm2[0];
}

/* ----------------------------------------------------------------------- */

#define I2C_WRITE_DATA_MAX 10

struct saa717x_write_regset_st {
	unsigned char  data_length;
	unsigned char  write_data[I2C_WRITE_DATA_MAX];
};

/***** initialize @ detect_client *****/
static struct saa717x_write_regset_st
reg_init_initialize[] =
{
  /* from linux driver */
  { 3, {0x001, 0x001, 0x008}}, // Increment delay

  { 3, {0x001, 0x003, 0x000}}, // Analog input control 2
  { 3, {0x001, 0x004, 0x090}}, // Analog input control 3
  { 3, {0x001, 0x005, 0x090}}, // Analog input control 4
  { 3, {0x001, 0x006, 0x0eb}}, // Horizontal sync start
  { 3, {0x001, 0x007, 0x0e0}}, // Horizontal sync stop
  { 3, {0x001, 0x009, 0x055}}, // Luminance control

  { 3, {0x001, 0x00f, 0x02a}}, // Chroma gain control
  { 3, {0x001, 0x010, 0x000}}, // Chroma control 2

  { 3, {0x001, 0x014, 0x045}}, // analog/ADC

  { 3, {0x001, 0x018, 0x040}}, // RAW data gain
  { 3, {0x001, 0x019, 0x080}}, // RAW data offset

  { 3, {0x000, 0x044, 0x000}}, // VBI horizontal input window start (L) TASK A
  { 3, {0x000, 0x045, 0x000}}, // VBI horizontal input window start (H) TASK A
  { 3, {0x000, 0x046, 0x0cf}}, // VBI horizontal input window stop (L) TASK A
  { 3, {0x000, 0x047, 0x002}}, // VBI horizontal input window stop (H) TASK A

  { 3, {0x000, 0x049, 0x000}}, // VBI vertical input window start (H) TASK A

  { 3, {0x000, 0x04c, 0x0d0}}, // VBI horizontal output length (L) TASK A
  { 3, {0x000, 0x04d, 0x002}}, // VBI horizontal output length (H) TASK A

  { 3, {0x000, 0x064, 0x080}}, // Lumina brightness TASK A
  { 3, {0x000, 0x065, 0x040}}, // Luminance contrast TASK A
  { 3, {0x000, 0x066, 0x040}}, // Chroma saturation TASK A
                               // 067H: Reserved
  { 3, {0x000, 0x068, 0x000}}, // VBI horizontal scaling increment (L) TASK A
  { 3, {0x000, 0x069, 0x004}}, // VBI horizontal scaling increment (H) TASK A
  { 3, {0x000, 0x06a, 0x000}}, // VBI phase offset TASK A

  { 3, {0x000, 0x06e, 0x000}}, // Horizontal phase offset Luma TASK A
  { 3, {0x000, 0x06f, 0x000}}, // Horizontal phase offset Chroma TASK A

  { 3, {0x000, 0x072, 0x000}}, // Vertical filter mode TASK A

  { 3, {0x000, 0x084, 0x000}}, // VBI horizontal input window start (L) TAKS B
  { 3, {0x000, 0x085, 0x000}}, // VBI horizontal input window start (H) TAKS B
  { 3, {0x000, 0x086, 0x0cf}}, // VBI horizontal input window stop (L) TAKS B
  { 3, {0x000, 0x087, 0x002}}, // VBI horizontal input window stop (H) TAKS B

  { 3, {0x000, 0x089, 0x000}}, // VBI vertical input window start (H) TAKS B

  { 3, {0x000, 0x08c, 0x0d0}}, // VBI horizontal output length (L) TASK B
  { 3, {0x000, 0x08d, 0x002}}, // VBI horizontal output length (H) TASK B

  { 3, {0x000, 0x0a4, 0x080}}, // Lumina brightness TASK B
  { 3, {0x000, 0x0a5, 0x040}}, // Luminance contrast TASK B
  { 3, {0x000, 0x0a6, 0x040}}, // Chroma saturation TASK B
                               // 0A7H reserved
  { 3, {0x000, 0x0a8, 0x000}}, // VBI horizontal scaling increment (L) TASK B
  { 3, {0x000, 0x0a9, 0x004}}, // VBI horizontal scaling increment (H) TASK B
  { 3, {0x000, 0x0aa, 0x000}}, // VBI phase offset TASK B

  { 3, {0x000, 0x0ae, 0x000}}, // Horizontal phase offset Luma TASK B
  { 3, {0x000, 0x0af, 0x000}}, //Horizontal phase offset Chroma TASK B

  { 3, {0x000, 0x0b2, 0x000}}, // Vertical filter mode TASK B

  { 3, {0x000, 0x00c, 0x000}}, // Start point GREEN path
  { 3, {0x000, 0x00d, 0x000}}, // Start point BLUE path
  { 3, {0x000, 0x00e, 0x000}}, // Start point RED path
   
  { 3, {0x000, 0x010, 0x010}}, // GREEN path gamma curve ---
  { 3, {0x000, 0x011, 0x020}},
  { 3, {0x000, 0x012, 0x030}},
  { 3, {0x000, 0x013, 0x040}},
  { 3, {0x000, 0x014, 0x050}},
  { 3, {0x000, 0x015, 0x060}},
  { 3, {0x000, 0x016, 0x070}},
  { 3, {0x000, 0x017, 0x080}},
  { 3, {0x000, 0x018, 0x090}},
  { 3, {0x000, 0x019, 0x0a0}},
  { 3, {0x000, 0x01a, 0x0b0}},
  { 3, {0x000, 0x01b, 0x0c0}},
  { 3, {0x000, 0x01c, 0x0d0}},
  { 3, {0x000, 0x01d, 0x0e0}},
  { 3, {0x000, 0x01e, 0x0f0}},
  { 3, {0x000, 0x01f, 0x0ff}}, // --- GREEN path gamma curve
   
  { 3, {0x000, 0x020, 0x010}}, // BLUE path gamma curve ---
  { 3, {0x000, 0x021, 0x020}},
  { 3, {0x000, 0x022, 0x030}},
  { 3, {0x000, 0x023, 0x040}},
  { 3, {0x000, 0x024, 0x050}},
  { 3, {0x000, 0x025, 0x060}},
  { 3, {0x000, 0x026, 0x070}},
  { 3, {0x000, 0x027, 0x080}},
  { 3, {0x000, 0x028, 0x090}},
  { 3, {0x000, 0x029, 0x0a0}},
  { 3, {0x000, 0x02a, 0x0b0}},
  { 3, {0x000, 0x02b, 0x0c0}},
  { 3, {0x000, 0x02c, 0x0d0}},
  { 3, {0x000, 0x02d, 0x0e0}},
  { 3, {0x000, 0x02e, 0x0f0}},
  { 3, {0x000, 0x02f, 0x0ff}}, // --- BLUE path gamma curve

  { 3, {0x000, 0x030, 0x010}}, // RED path gamma curve ---
  { 3, {0x000, 0x031, 0x020}},
  { 3, {0x000, 0x032, 0x030}},
  { 3, {0x000, 0x033, 0x040}},   
  { 3, {0x000, 0x034, 0x050}},
  { 3, {0x000, 0x035, 0x060}},
  { 3, {0x000, 0x036, 0x070}},
  { 3, {0x000, 0x037, 0x080}},
  { 3, {0x000, 0x038, 0x090}},   
  { 3, {0x000, 0x039, 0x0a0}},
  { 3, {0x000, 0x03a, 0x0b0}},
  { 3, {0x000, 0x03b, 0x0c0}},
  { 3, {0x000, 0x03c, 0x0d0}},
  { 3, {0x000, 0x03d, 0x0e0}},   
  { 3, {0x000, 0x03e, 0x0f0}},
  { 3, {0x000, 0x03f, 0x0ff}}, // --- RED path gamma curve

  { 3, {0x001, 0x009, 0x085}}, // Luminance control 

  /**** from app start ****/
  { 3, {0x005, 0x084, 0x000}}, // AGC gain control
  { 3, {0x005, 0x085, 0x000}}, // Program count
  { 3, {0x005, 0x086, 0x003}}, // Status reset
  { 3, {0x005, 0x088, 0x0ff}}, // Number of audio samples (L)
  { 3, {0x005, 0x089, 0x00f}}, // Number of audio samples (M)
  { 3, {0x005, 0x08a, 0x000}}, // Number of audio samples (H)
  { 3, {0x005, 0x08b, 0x000}}, // Audio select
  { 3, {0x005, 0x08c, 0x010}}, // Audio channel assign1
  { 3, {0x005, 0x08d, 0x032}}, // Audio channel assign2
  { 3, {0x005, 0x08e, 0x054}}, // Audio channel assign3
  { 3, {0x005, 0x08f, 0x023}}, // Audio format
  { 3, {0x005, 0x090, 0x000}}, // SIF control

  { 3, {0x005, 0x095, 0x000}}, // ??
  { 3, {0x005, 0x096, 0x000}}, // ??
  { 3, {0x005, 0x097, 0x000}}, // ??

  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}}, // Digital input crossbar1

  { 5, {0x004, 0x06c, 0x010, 0x0bb, 0x0bb}}, // Digital output selection1-3

  { 5, {0x004, 0x070, 0x010, 0x010, 0x010}}, // Digital output selection4-6

  //  { 5, {0x004, 0x078, 0x0a2, 0x004, 0x000}}, // Sound feature control
  { 5, {0x004, 0x078, 0x000, 0x000, 0x000}}, // Sound feature control

  { 5, {0x004, 0x074, 0x018, 0x000, 0x000}}, // Softmute control

  { 5, {0x004, 0x054, 0x0b9, 0x025, 0x004}}, // Sound Easy programming(reset)
  { 5, {0x004, 0x054, 0x039, 0x025, 0x004}}, // Sound Easy programming(reset)

    /**** common setting( of DVD play, including scaler commands) ****/
  { 3, {0x000, 0x042, 0x003}}, // Data path configuration for VBI (TASK A)

  { 3, {0x000, 0x082, 0x003}}, // Data path configuration for VBI (TASK B)

  { 3, {0x001, 0x008, 0x0f8}}, // Sync control
  { 3, {0x002, 0x0a9, 0x0fd}}, // ???
  { 3, {0x001, 0x002, 0x089}}, // select video input "mode 9"
  { 3, {0x001, 0x011, 0x000}}, // Mode/delay control

  { 3, {0x001, 0x00e, 0x00a}}, // Chroma control 1

  { 3, {0x005, 0x094, 0x002}}, // SIF, analog I/O select

  { 5, {0x004, 0x054, 0x0b9, 0x025, 0x004}}, // Sound 
  { 5, {0x004, 0x054, 0x039, 0x025, 0x004}},
  { 3, {0x001, 0x011, 0x000}},
  { 3, {0x001, 0x00e, 0x00a}},
  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}},
  { 3, {0x003, 0x000, 0x000}},
  { 3, {0x003, 0x001, 0x006}},
  { 3, {0x003, 0x002, 0x000}},
  { 3, {0x003, 0x003, 0x006}},
  { 3, {0x003, 0x008, 0x040}},
  { 3, {0x003, 0x009, 0x000}},
  { 3, {0x003, 0x00a, 0x000}},
  { 3, {0x003, 0x00b, 0x000}},
  { 3, {0x000, 0x000, 0x002}},
  { 3, {0x000, 0x001, 0x000}},
  { 3, {0x000, 0x002, 0x000}},
  { 3, {0x000, 0x003, 0x000}},
  { 3, {0x000, 0x004, 0x033}},
  { 3, {0x000, 0x040, 0x01d}},
  { 3, {0x000, 0x041, 0x001}},
  { 3, {0x000, 0x042, 0x004}},
  { 3, {0x000, 0x043, 0x000}},
  { 3, {0x000, 0x080, 0x01e}},
  { 3, {0x000, 0x081, 0x001}},
  { 3, {0x000, 0x082, 0x004}},
  { 3, {0x000, 0x083, 0x000}},
  { 3, {0x001, 0x090, 0x018}},
  { 3, {0x001, 0x015, 0x000}},
  { 3, {0x001, 0x016, 0x012}},
  { 3, {0x001, 0x017, 0x018}},
  { 3, {0x000, 0x04a, 0x011}},
  { 3, {0x000, 0x08a, 0x011}},
  { 3, {0x000, 0x04b, 0x000}},
  { 3, {0x000, 0x08b, 0x000}},
  { 3, {0x000, 0x048, 0x000}},
  { 3, {0x000, 0x088, 0x000}},
  { 3, {0x000, 0x04e, 0x012}},
  { 3, {0x000, 0x08e, 0x012}},
  { 3, {0x000, 0x058, 0x012}},
  { 3, {0x000, 0x098, 0x012}},
  { 3, {0x000, 0x059, 0x000}},
  { 3, {0x000, 0x099, 0x000}},
  { 3, {0x000, 0x05a, 0x003}},
  { 3, {0x000, 0x09a, 0x003}},
  { 3, {0x000, 0x05b, 0x001}},
  { 3, {0x000, 0x09b, 0x001}},
  { 3, {0x000, 0x054, 0x008}},
  { 3, {0x000, 0x094, 0x008}},
  { 3, {0x000, 0x055, 0x000}},
  { 3, {0x000, 0x095, 0x000}},
  { 3, {0x000, 0x056, 0x0c7}},
  { 3, {0x000, 0x096, 0x0c7}},
  { 3, {0x000, 0x057, 0x002}},
  { 3, {0x000, 0x097, 0x002}},
  { 3, {0x0ff, 0x0ff, 0x0ff}},
  { 3, {0x000, 0x060, 0x001}},
  { 3, {0x000, 0x0a0, 0x001}},
  { 3, {0x000, 0x061, 0x000}},
  { 3, {0x000, 0x0a1, 0x000}},
  { 3, {0x000, 0x062, 0x000}},
  { 3, {0x000, 0x0a2, 0x000}},
  { 3, {0x000, 0x063, 0x000}},
  { 3, {0x000, 0x0a3, 0x000}},
  { 3, {0x000, 0x070, 0x000}},
  { 3, {0x000, 0x0b0, 0x000}},
  { 3, {0x000, 0x071, 0x004}},
  { 3, {0x000, 0x0b1, 0x004}},
  { 3, {0x000, 0x06c, 0x0e9}},
  { 3, {0x000, 0x0ac, 0x0e9}},
  { 3, {0x000, 0x06d, 0x003}},
  { 3, {0x000, 0x0ad, 0x003}},
  { 3, {0x000, 0x05c, 0x0d0}},
  { 3, {0x000, 0x09c, 0x0d0}},
  { 3, {0x000, 0x05d, 0x002}},
  { 3, {0x000, 0x09d, 0x002}},
  { 3, {0x000, 0x05e, 0x0f2}},
  { 3, {0x000, 0x09e, 0x0f2}},
  { 3, {0x000, 0x05f, 0x000}},
  { 3, {0x000, 0x09f, 0x000}},
  { 3, {0x000, 0x074, 0x000}},
  { 3, {0x000, 0x0b4, 0x000}},
  { 3, {0x000, 0x075, 0x000}},
  { 3, {0x000, 0x0b5, 0x000}},
  { 3, {0x000, 0x076, 0x000}},
  { 3, {0x000, 0x0b6, 0x000}},
  { 3, {0x000, 0x077, 0x000}},
  { 3, {0x000, 0x0b7, 0x000}},
  { 3, {0x001, 0x095, 0x008}},
  { 3, {0x0ff, 0x0ff, 0x0ff}},
  { 3, {0x001, 0x008, 0x0f8}},
  { 3, {0x001, 0x011, 0x000}},
  { 3, {0x001, 0x00e, 0x00a}},
  { 3, {0x002, 0x0a9, 0x0fd}},
  { 5, {0x004, 0x064, 0x001, 0x000, 0x000}},
  { 5, {0x004, 0x054, 0x035, 0x021, 0x004}},
  { 3, {0x005, 0x098, 0x0e7}},
  { 3, {0x005, 0x099, 0x07d}},
  { 3, {0x005, 0x09a, 0x018}},
  { 3, {0x005, 0x09c, 0x066}},
  { 3, {0x005, 0x09d, 0x090}},
  { 3, {0x005, 0x09e, 0x001}},
  { 3, {0x005, 0x084, 0x000}},
  { 3, {0x005, 0x085, 0x000}},
  { 3, {0x005, 0x086, 0x003}},
  { 3, {0x005, 0x088, 0x0ff}},
  { 3, {0x005, 0x089, 0x00f}},
  { 3, {0x005, 0x08a, 0x000}},
  { 3, {0x005, 0x08b, 0x000}},
  { 3, {0x005, 0x08c, 0x010}},
  { 3, {0x005, 0x08d, 0x032}},
  { 3, {0x005, 0x08e, 0x054}},
  { 3, {0x005, 0x08f, 0x023}},
  { 3, {0x005, 0x090, 0x000}},
  { 3, {0x005, 0x095, 0x000}},
  { 3, {0x005, 0x096, 0x000}},
  { 3, {0x005, 0x097, 0x000}},
  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x06c, 0x010, 0x0bb, 0x0bb}},
  { 5, {0x004, 0x070, 0x010, 0x010, 0x010}},
  //  { 5, {0x004, 0x078, 0x0a2, 0x004, 0x000}},
  { 5, {0x004, 0x078, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x074, 0x018, 0x000, 0x000}},
  { 5, {0x004, 0x054, 0x035, 0x021, 0x004}},
  { 3, {0x005, 0x098, 0x0e7}},
  { 3, {0x005, 0x099, 0x07d}},
  { 3, {0x005, 0x09a, 0x018}},
  { 3, {0x005, 0x09c, 0x066}},
  { 3, {0x005, 0x09d, 0x090}},
  { 3, {0x005, 0x09e, 0x001}},
  { 3, {0x005, 0x084, 0x000}},
  { 3, {0x005, 0x085, 0x000}},
  { 3, {0x005, 0x086, 0x003}},
  { 3, {0x005, 0x088, 0x0ff}},
  { 3, {0x005, 0x089, 0x00f}},
  { 3, {0x005, 0x08a, 0x000}},
  { 3, {0x005, 0x08b, 0x000}},
  { 3, {0x005, 0x08c, 0x010}},
  { 3, {0x005, 0x08d, 0x032}},
  { 3, {0x005, 0x08e, 0x054}},
  { 3, {0x005, 0x08f, 0x023}},
  { 3, {0x005, 0x090, 0x000}},
  { 3, {0x005, 0x095, 0x000}},
  { 3, {0x005, 0x096, 0x000}},
  { 3, {0x005, 0x097, 0x000}},
  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x06c, 0x010, 0x0bb, 0x0bb}},
  { 5, {0x004, 0x070, 0x010, 0x010, 0x010}},
  //  { 5, {0x004, 0x078, 0x0a2, 0x004, 0x000}},
  { 5, {0x004, 0x078, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x074, 0x018, 0x000, 0x000}},
  { 5, {0x004, 0x054, 0x035, 0x021, 0x004}},
  { 3, {0x005, 0x098, 0x0e7}},
  { 3, {0x005, 0x099, 0x07d}},
  { 3, {0x005, 0x09a, 0x018}},
  { 3, {0x005, 0x09c, 0x066}},
  { 3, {0x005, 0x09d, 0x090}},
  { 3, {0x005, 0x09e, 0x001}},
  { 3, {0x005, 0x084, 0x000}},
  { 3, {0x005, 0x085, 0x000}},
  { 3, {0x005, 0x086, 0x003}},
  { 3, {0x005, 0x088, 0x0ff}},
  { 3, {0x005, 0x089, 0x00f}},
  { 3, {0x005, 0x08a, 0x000}},
  { 3, {0x005, 0x08b, 0x000}},
  { 3, {0x005, 0x08c, 0x010}},
  { 3, {0x005, 0x08d, 0x032}},
  { 3, {0x005, 0x08e, 0x054}},
  { 3, {0x005, 0x08f, 0x023}},
  { 3, {0x005, 0x090, 0x000}},
  { 3, {0x005, 0x095, 0x000}},
  { 3, {0x005, 0x096, 0x000}},
  { 3, {0x005, 0x097, 0x000}},
  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x06c, 0x010, 0x0bb, 0x0bb}},
  { 5, {0x004, 0x070, 0x010, 0x010, 0x010}},
  //  { 5, {0x004, 0x078, 0x0a2, 0x004, 0x000}},
  { 5, {0x004, 0x078, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x074, 0x018, 0x000, 0x000}},
  { 5, {0x004, 0x054, 0x035, 0x021, 0x004}},
  { 3, {0x001, 0x093, 0x000}},
  { 3, {0x003, 0x000, 0x000}},
  { 3, {0x003, 0x001, 0x006}},
  { 3, {0x003, 0x002, 0x000}},
  { 3, {0x003, 0x003, 0x006}},
  { 3, {0x003, 0x008, 0x040}},
  { 3, {0x003, 0x009, 0x000}},
  { 3, {0x003, 0x00a, 0x000}},
  { 3, {0x003, 0x00b, 0x000}},
  { 3, {0x000, 0x000, 0x002}},
  { 3, {0x000, 0x001, 0x000}},
  { 3, {0x000, 0x002, 0x000}},
  { 3, {0x000, 0x003, 0x000}},
  { 3, {0x000, 0x004, 0x033}},
  { 3, {0x000, 0x040, 0x01d}},
  { 3, {0x000, 0x041, 0x001}},
  { 3, {0x000, 0x042, 0x004}},
  { 3, {0x000, 0x043, 0x000}},
  { 3, {0x000, 0x080, 0x01e}},
  { 3, {0x000, 0x081, 0x001}},
  { 3, {0x000, 0x082, 0x004}},
  { 3, {0x000, 0x083, 0x000}},
  { 3, {0x001, 0x090, 0x018}},
  { 3, {0x001, 0x015, 0x000}},
  { 3, {0x001, 0x016, 0x012}},
  { 3, {0x001, 0x017, 0x018}},
  { 3, {0x000, 0x04a, 0x011}},
  { 3, {0x000, 0x08a, 0x011}},
  { 3, {0x000, 0x04b, 0x000}},
  { 3, {0x000, 0x08b, 0x000}},
  { 3, {0x000, 0x048, 0x000}},
  { 3, {0x000, 0x088, 0x000}},
  { 3, {0x000, 0x04e, 0x012}},
  { 3, {0x000, 0x08e, 0x012}},
  { 3, {0x000, 0x058, 0x012}},
  { 3, {0x000, 0x098, 0x012}},
  { 3, {0x000, 0x059, 0x000}},
  { 3, {0x000, 0x099, 0x000}},
  { 3, {0x000, 0x05a, 0x003}},
  { 3, {0x000, 0x09a, 0x003}},
  { 3, {0x000, 0x05b, 0x001}},
  { 3, {0x000, 0x09b, 0x001}},
  { 3, {0x000, 0x054, 0x008}},
  { 3, {0x000, 0x094, 0x008}},
  { 3, {0x000, 0x055, 0x000}},
  { 3, {0x000, 0x095, 0x000}},
  { 3, {0x000, 0x056, 0x0c7}},
  { 3, {0x000, 0x096, 0x0c7}},
  { 3, {0x000, 0x057, 0x002}},
  { 3, {0x000, 0x097, 0x002}},
  { 3, {0x000, 0x060, 0x001}},
  { 3, {0x000, 0x0a0, 0x001}},
  { 3, {0x000, 0x061, 0x000}},
  { 3, {0x000, 0x0a1, 0x000}},
  { 3, {0x000, 0x062, 0x000}},
  { 3, {0x000, 0x0a2, 0x000}},
  { 3, {0x000, 0x063, 0x000}},
  { 3, {0x000, 0x0a3, 0x000}},
  { 3, {0x000, 0x070, 0x000}},
  { 3, {0x000, 0x0b0, 0x000}},
  { 3, {0x000, 0x071, 0x004}},
  { 3, {0x000, 0x0b1, 0x004}},
  { 3, {0x000, 0x06c, 0x0e9}},
  { 3, {0x000, 0x0ac, 0x0e9}},
  { 3, {0x000, 0x06d, 0x003}},
  { 3, {0x000, 0x0ad, 0x003}},
  { 3, {0x000, 0x05c, 0x0d0}},
  { 3, {0x000, 0x09c, 0x0d0}},
  { 3, {0x000, 0x05d, 0x002}},
  { 3, {0x000, 0x09d, 0x002}},
  { 3, {0x000, 0x05e, 0x0f2}},
  { 3, {0x000, 0x09e, 0x0f2}},
  { 3, {0x000, 0x05f, 0x000}},
  { 3, {0x000, 0x09f, 0x000}},
  { 3, {0x000, 0x074, 0x000}},
  { 3, {0x000, 0x0b4, 0x000}},
  { 3, {0x000, 0x075, 0x000}},
  { 3, {0x000, 0x0b5, 0x000}},
  { 3, {0x000, 0x076, 0x000}},
  { 3, {0x000, 0x0b6, 0x000}},
  { 3, {0x000, 0x077, 0x000}},
  { 3, {0x000, 0x0b7, 0x000}},
  { 3, {0x001, 0x095, 0x008}},
  { 3, {0x005, 0x098, 0x0e7}},
  { 3, {0x005, 0x099, 0x07d}},
  { 3, {0x005, 0x09a, 0x018}},
  { 3, {0x005, 0x09c, 0x066}},
  { 3, {0x005, 0x09d, 0x090}},
  { 3, {0x005, 0x09e, 0x001}},
  { 3, {0x005, 0x084, 0x000}},
  { 3, {0x005, 0x085, 0x000}},
  { 3, {0x005, 0x086, 0x003}},
  { 3, {0x005, 0x088, 0x0ff}},
  { 3, {0x005, 0x089, 0x00f}},
  { 3, {0x005, 0x08a, 0x000}},
  { 3, {0x005, 0x08b, 0x000}},
  { 3, {0x005, 0x08c, 0x010}},
  { 3, {0x005, 0x08d, 0x032}},
  { 3, {0x005, 0x08e, 0x054}},
  { 3, {0x005, 0x08f, 0x023}},
  { 3, {0x005, 0x090, 0x000}},
  { 3, {0x005, 0x095, 0x000}},
  { 3, {0x005, 0x096, 0x000}},
  { 3, {0x005, 0x097, 0x000}},
  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x06c, 0x010, 0x0bb, 0x0bb}},
  { 5, {0x004, 0x070, 0x010, 0x010, 0x010}},
  //  { 5, {0x004, 0x078, 0x0a2, 0x004, 0x000}},
  { 5, {0x004, 0x078, 0x000, 0x000, 0x000}},
  { 5, {0x004, 0x074, 0x018, 0x000, 0x000}},
  { 5, {0x004, 0x054, 0x035, 0x021, 0x004}},
  { 3, {0x001, 0x093, 0x0a6}},
  { 3, {0x001, 0x008, 0x0f8}},
  { 3, {0x000, 0x042, 0x003}},
  { 3, {0x000, 0x082, 0x003}},
  { 5, {0x004, 0x054, 0x0b9, 0x025, 0x004}},
  { 5, {0x004, 0x054, 0x039, 0x025, 0x004}},
  { 3, {0x001, 0x093, 0x000}},
  { 3, {0x001, 0x093, 0x0a6}},
  { 5, {0x004, 0x064, 0x000, 0x000, 0x000}},

  { 0, {0x000}}
};

/***** tuner *****/
static struct saa717x_write_regset_st
reg_init_tuner_input[] =
{
  { 3, {0x001, 0x008, 0x0f8}}, // sync control?
  //  { 3, {0x005, 0x094, 0x002}}, // SIF, analog I/O select
  { 3, {0x001, 0x011, 0x000}}, // Mode/delay control
  { 3, {0x001, 0x00e, 0x00a}}, // Chroma control 1
  { 0, {0x000}}
};
/***** composite *****/
static struct saa717x_write_regset_st
reg_init_composite_input[] =
{
  { 3, {0x001, 0x008, 0x0e8}}, // Sync control
  //  { 3, {0x005, 0x094, 0x000}}, // SIF,analog I/O select
  { 3, {0x001, 0x011, 0x000}}, // Mode/delay control
  { 3, {0x001, 0x00e, 0x04a}}, // Chroma control1
  { 0, {0x000}}
};

/***** s-video *****/
static struct saa717x_write_regset_st
reg_init_svideo_input[] =
{
  { 3, {0x001, 0x008, 0x0e8}}, // Sync control
  //  { 3, {0x005, 0x094, 0x000}}, // SIF,analog I/O select
  { 3, {0x001, 0x011, 0x000}}, // Mode/delay control
  { 3, {0x001, 0x00e, 0x04a}}, // Chroma control1
  { 0, {0x000}}
};

#define SAA717X_REG_INIT_INIT 0
#define SAA717X_REG_INIT_TUNER 1
#define SAA717X_REG_INIT_COMPOSITE 2
#define SAA717X_REG_INIT_SVIDEO 3

static struct saa717x_write_regset_st *
reg_init_template[] =
{
  [SAA717X_REG_INIT_INIT] = reg_init_initialize,
  [SAA717X_REG_INIT_TUNER] = reg_init_tuner_input,
  [SAA717X_REG_INIT_COMPOSITE] = reg_init_composite_input,
  [SAA717X_REG_INIT_SVIDEO] = reg_init_svideo_input
};

static struct saa717x_write_regset_st reg_set_audio_template[][3] =
{
  { // for MONO
    // tadachi 6/29 DMA audio output select? 
    // 7-4:DMA2,3-0:DMA1 ch. DMA4,DMA3 DMA2,DMA1
    // 0:MAIN left, 1:MAIN right
    // 2:AUX1 left, 3:AUX1 right
    // 4:AUX2 left, 5:AUX2 right
    // 6:DPL left,  7:DPL  right
    // 8:DPL center, 9:DPL surround
    // A:monitor output, B:digital sience
    {5, {0x04, 0x6C, 0x00, 0xBB, 0xBB}},
 
    // tadachi 6/29 DAC and I2S output select? 
    // 7-4:DAC right ch. 3-0:DAC left ch.
    // I2S1 right,left  I2S2 right,left
    {5, {0x04, 0x70, 0x00, 0x00, 0x00}},
    {0,{0x00}}
  },
  { // for STEREO
    {5, {0x04, 0x6C, 0x10, 0xBB, 0xBB}},
    {5, {0x04, 0x70, 0x10, 0x10, 0x10}},
    {0,{0x00}}
  },
  { // for LANG1
    {5, {0x04, 0x6C, 0x00, 0xBB, 0xBB}},
    {5, {0x04, 0x70, 0x00, 0x00, 0x00}},
    {0,{0x00}}
  },
  { // for LANG2/SAP
    {5, {0x04, 0x6C, 0x11, 0xBB, 0xBB}},
    {5, {0x04, 0x70, 0x11, 0x11, 0x11}},
    {0,{0x00}}
  }
};


/* ===== write regs (sub) ===== */
static int write_regs_sub(struct i2c_client *client,
                          unsigned char  data_length,
                          unsigned char  *data)
{
	int err;
	unsigned char buffer[I2C_WRITE_DATA_MAX];
	unsigned int i;

	for (i = 0; i < data_length; i++) {
		buffer[i] = data[i];
	}

	v4l_dbg(2, debug, client,"wrote addr=%02X, reg=0x%02X%02X, ",
			client->addr,buffer[0],buffer[1]);
	for (i = 2; i < data_length; i++) {
		v4l_dbg(2, debug, client," data=%02X", buffer[i]);
	}
	v4l_dbg(2, debug, client,"\n");

	for (err = 0; err < 3;) {
		if (data_length == i2c_master_send(client, buffer, data_length))
			break;
		err++;
		printk("%s: I/O error #%d\n", client->name,err);
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(HZ/10);
	}
	return 0;
}

/* ===== batch write  ===== */
static int batch_write_regs(struct i2c_client *client,
                     unsigned int write_set)
{
	unsigned int i;

	v4l_dbg(2, debug, client,"writing registers (batch_write_regs) by %d set\n",
			write_set);

	i=0;
	while (reg_init_template[write_set][i].data_length != 0) {
		if (write_regs_sub(client,
			reg_init_template[write_set][i].data_length,
			reg_init_template[write_set][i].write_data)) {
			return -1;
		}
		i++;
	}
	return 0;
}


/* ===== get detected audio flags  (from saa7134 driver) ===== */
static void get_inf_dev_status(struct i2c_client *client,
		      int *dual_flag,
		      int *stereo_flag)
{
	u32 reg_data3;

	static char *stdres[0x20] = {
		[0x00] = "no standard detected",
		[0x01] = "B/G (in progress)",
		[0x02] = "D/K (in progress)",
		[0x03] = "M (in progress)",

		[0x04] = "B/G A2",
		[0x05] = "B/G NICAM",
		[0x06] = "D/K A2 (1)",
		[0x07] = "D/K A2 (2)",
		[0x08] = "D/K A2 (3)",
		[0x09] = "D/K NICAM",
		[0x0a] = "L NICAM",
		[0x0b] = "I NICAM",

		[0x0c] = "M Korea",
		[0x0d] = "M BTSC ",
		[0x0e] = "M EIAJ",

		[0x0f] = "FM radio / IF 10.7 / 50 deemp",
		[0x10] = "FM radio / IF 10.7 / 75 deemp",
		[0x11] = "FM radio / IF sel / 50 deemp",
		[0x12] = "FM radio / IF sel / 75 deemp",

		[0x13 ... 0x1e ] = "unknown",
		[0x1f] = "??? [in progress]",
	};


	*dual_flag = *stereo_flag = 0;

	// (demdec status: 0x528)

	// read current status
	reg_data3 = readreg3(client, 0x05,0x28);

	v4l_dbg(1, debug, client,"tvaudio thread status: 0x%x [%s%s%s]\n",
			reg_data3, stdres[reg_data3 & 0x1f],
			(reg_data3 & 0x000020) ? ",stereo" : "",
			(reg_data3 & 0x000040) ? ",dual"   : "");
	v4l_dbg(1, debug, client,"detailed status: "
			"%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#%s\n",
			(reg_data3 & 0x000080) ? " A2/EIAJ pilot tone "     : "",
			(reg_data3 & 0x000100) ? " A2/EIAJ dual "           : "",
			(reg_data3 & 0x000200) ? " A2/EIAJ stereo "         : "",
			(reg_data3 & 0x000400) ? " A2/EIAJ noise mute "     : "",

			(reg_data3 & 0x000800) ? " BTSC/FM radio pilot "    : "",
			(reg_data3 & 0x001000) ? " SAP carrier "            : "",
			(reg_data3 & 0x002000) ? " BTSC stereo noise mute " : "",
			(reg_data3 & 0x004000) ? " SAP noise mute "         : "",
			(reg_data3 & 0x008000) ? " VDSP "                   : "",

			(reg_data3 & 0x010000) ? " NICST "                  : "",
			(reg_data3 & 0x020000) ? " NICDU "                  : "",
			(reg_data3 & 0x040000) ? " NICAM muted "            : "",
			(reg_data3 & 0x080000) ? " NICAM reserve sound "    : "",

			(reg_data3 & 0x100000) ? " init done "              : "");

	if (((reg_data3 & 0x000020) != 0)||
		((reg_data3 & 0x000200)) != 0) {
		v4l_dbg(1, debug, client,"ST!!!\n");
		*stereo_flag = 1;
	}

	if (((reg_data3 & 0x000040) != 0)||
		((reg_data3 & 0x000100) != 0)) {
		v4l_dbg(1, debug, client,"DUAL!!!\n");
		*dual_flag = 1;
	}
}

/* ===== regs write to set audio mode ===== */
static int set_audio_mode(struct i2c_client *client, int audio_mode)
{
	unsigned int i;

	v4l_dbg(1, debug, client,"writing registers to set audio mode by set %d\n",
			audio_mode);

	i=0;
	while(reg_set_audio_template[audio_mode][i].data_length != 0) {
		if (write_regs_sub(client,
			reg_set_audio_template[audio_mode][i].data_length,
			reg_set_audio_template[audio_mode][i].write_data) != 0) {
			return -1;
		}
		i++;
	}
	return 0;
}

/* ===== write regs to video output level (bright,contrast,hue,sat) ===== */
static int set_video_output_level_regs(struct i2c_client *client,
                   struct saa717x_state *decoder)
{
	char msg[3];

	// brightness ffh(bright)-80h(ITU level)-00h(dark)
	msg[0]=0x01; msg[1]=0x0a;
	msg[2]=decoder->bright;
	write_regs_sub(client,3,msg);

	// contrast 7fh(max:1.984)-44h(ITU)-40h(1.0)-
	//           0h (luminance off) 40:i2c dump
	//           c0h (-1.0 inverse chrominance)
	//           80h (-2.0 inverse chrominance)
	msg[0]=0x01; msg[1]=0x0b;
	msg[2]=decoder->contrast;
	write_regs_sub(client,3,msg);

	// saturation? 7fh(max)-40h(ITU)-0h(color off)
	//             c0h (-1.0 inverse chrominance)
	//             80h (-2.0 inverse chrominance)
	msg[0]=0x01; msg[1]=0x0c;
	msg[2]=decoder->sat;
	write_regs_sub(client,3,msg);

	// color hue(phase) control
	//                  7fh(+178.6)-0h(:0 normal)-80h(-180.0)
	msg[0]=0x01; msg[1]=0x0d;
	msg[2]=decoder->hue;
	write_regs_sub(client,3,msg);

	return 0;
}

/* ===== write regs to set audio volume, bass and treble ===== */
static int set_audio_regs(struct i2c_client *client,
                   struct saa717x_state *decoder)
{
	char msg[5];
	unsigned int work_l, work_r;

	// set SIF analog I/O select
	saa717x_write(client, 0x05, 0x94,decoder->audio_input);
	v4l_dbg(1, debug, client, "set audio input %d\n",
			decoder->audio_input); 

	// normalize ( 65535 to 0 -> 24 to -40 (not -84))
	work_l = (min(65536 - decoder->audio_main_balance, 32768) * decoder->audio_main_volume) / 32768;
	work_r = (min(decoder->audio_main_balance, (u16)32768) * decoder->audio_main_volume) / 32768;
	decoder->audio_main_vol_l = (long)work_l*(24-(-40))/65535-40;
	decoder->audio_main_vol_r = (long)work_r*(24-(-40))/65535-40;

	// set main volume
	// main volume L[7-0],R[7-0],0x00  24=24dB,-83dB, -84(mute)
	//    def:0dB->6dB(MPG600GR)
	msg[0] = 0x04;
       	msg[1] = 0x80;
	// if mute is on, set mute
	if (decoder->audio_main_mute) {
		msg[2] = -84;
		msg[3] = -84;
	} else {
		msg[2] = (char)decoder->audio_main_vol_l;
		msg[3] = (char)decoder->audio_main_vol_r;
	}
	msg[4] = 0;

	v4l_dbg(1, debug, client, "vol_l=%d(0x%02x) vol_r=%d(0x%02x)\n",
			decoder->audio_main_vol_l, msg[2],
			decoder->audio_main_vol_r, msg[3]); 

	write_regs_sub(client,5,msg);

	// bass and treble; go to another function
	// set bass and treble
	msg[0]=0x04; msg[1]=0x88;
	msg[2]=decoder->audio_main_bass;
	msg[3]=decoder->audio_main_treble;
	msg[4]=0;
	write_regs_sub(client,5,msg);

	v4l_dbg(1, debug, client,
			"bass=%d(0x%02x) treble=%d(0x%02x)\n",
			decoder->audio_main_bass, msg[2],
			decoder->audio_main_treble, msg[3]); 
	return 0;
}

/********** scaling staff ***********/
static void set_h_prescale(struct i2c_client *client,
			   int task, int prescale)
{
	static const struct {
		int xpsc;
		int xacl;
		int xc2_1;
		int xdcg;
		int vpfy;
	} vals[] = {
		/* XPSC XACL XC2_1 XDCG VPFY */
		{    1,   0,    0,    0,   0 },
		{    2,   2,    1,    2,   2 },
		{    3,   4,    1,    3,   2 },
		{    4,   8,    1,    4,   2 },
		{    5,   8,    1,    4,   2 },
		{    6,   8,    1,    4,   3 },
		{    7,   8,    1,    4,   3 },
		{    8,  15,    0,    4,   3 },
		{    9,  15,    0,    4,   3 },
		{   10,  16,    1,    5,   3 },
	};
	static const int count = ARRAY_SIZE(vals);
	int i,task_shift;

	task_shift = task*0x40;
	for (i = 0; i < count; i++)
		if (vals[i].xpsc == prescale)
			break;
	if (i == count)
		return;

	// horizonal prescaling
	saa717x_write (client, 0x00,0x60+task_shift, vals[i].xpsc);
	// accumulation length
	saa717x_write (client, 0x00,0x61+task_shift, vals[i].xacl);
	// level control
	saa717x_write (client, 0x00,0x62+task_shift,
			(vals[i].xc2_1 << 3) | (vals[i].xdcg));
	//FIR prefilter control
	saa717x_write (client, 0x00,0x63+task_shift,
			(vals[i].vpfy << 2) | vals[i].vpfy);
}

/********** scaling staff ***********/
static void set_v_scale(struct i2c_client *client, int task, int yscale)
{
	int task_shift;

	task_shift = task*0x40;
	// Vertical scaling ratio (LOW)
	saa717x_write (client, 0x00,0x70+task_shift, yscale &  0xff);
	// Vertical scaling ratio (HI)
	saa717x_write (client, 0x00,0x71+task_shift, yscale >> 8);
}

static int saa717x_set_audio_clock_freq(struct i2c_client *client, u32 freq)
{
	/* not yet implament, so saa717x_cfg_??hz_??_audio is not defined. */
	return 0;
}

static int saa717x_set_v4lctrl(struct i2c_client *client, struct v4l2_control *ctrl)
{
	struct saa717x_state *state = i2c_get_clientdata(client);

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		if (ctrl->value < 0 || ctrl->value > 255) {
			v4l_err(client, "invalid brightness setting %d\n", ctrl->value);
			return -ERANGE;
		}

		state->bright = ctrl->value;
		v4l_dbg(1, debug, client,"bright:%d\n", state->bright);
		saa717x_write(client, 0x01, 0x0a, state->bright);
		break;

	case V4L2_CID_CONTRAST:
		if (ctrl->value < 0 || ctrl->value > 127) {
			v4l_err(client, "invalid contrast setting %d\n", ctrl->value);
			return -ERANGE;
		}

		state->contrast = ctrl->value;
		v4l_dbg(1, debug, client,"contrast:%d\n", state->contrast);
		saa717x_write(client, 0x01, 0x0b, state->contrast);
		break;

	case V4L2_CID_SATURATION:
		if (ctrl->value < 0 || ctrl->value > 127) {
			v4l_err(client, "invalid saturation setting %d\n", ctrl->value);
			return -ERANGE;
		}

		state->sat = ctrl->value;
		v4l_dbg(1, debug, client,"sat:%d\n", state->sat);
		saa717x_write(client, 0x01, 0x0c, state->sat);
		break;

	case V4L2_CID_HUE:
		if (ctrl->value < -127 || ctrl->value > 127) {
			v4l_err(client, "invalid hue setting %d\n", ctrl->value);
			return -ERANGE;
		}

		state->hue = ctrl->value;
		v4l_dbg(1, debug, client,"hue:%d\n", state->hue);
		saa717x_write(client, 0x01, 0x0d, state->hue);
		break;

	case V4L2_CID_AUDIO_MUTE:
		state->audio_main_mute = ctrl->value;
		set_audio_regs(client, state);
		break;

	case V4L2_CID_AUDIO_VOLUME:
		state->audio_main_volume = ctrl->value;
		set_audio_regs(client, state);
		break;

	case V4L2_CID_AUDIO_BALANCE:
		state->audio_main_balance = ctrl->value;
		set_audio_regs(client, state);
		break;

	case V4L2_CID_AUDIO_TREBLE:
		state->audio_main_treble = ctrl->value;
		set_audio_regs(client, state);
		break;

	case V4L2_CID_AUDIO_BASS:
		state->audio_main_bass = ctrl->value;
		set_audio_regs(client, state);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int saa717x_get_v4lctrl(struct i2c_client *client, struct v4l2_control *ctrl)
{
	struct saa717x_state *state = i2c_get_clientdata(client);

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
		ctrl->value = state->bright;
		break;

	case V4L2_CID_CONTRAST:
		ctrl->value = state->contrast;
		break;

	case V4L2_CID_SATURATION:
		ctrl->value = state->sat;
		break;

	case V4L2_CID_HUE:
		ctrl->value = state->hue;
		break;

	case V4L2_CID_AUDIO_MUTE:
		ctrl->value = state->audio_main_mute;
		break;

	case V4L2_CID_AUDIO_VOLUME:
		ctrl->value = state->audio_main_volume;
		break;

	case V4L2_CID_AUDIO_BALANCE:
		ctrl->value = state->audio_main_balance;
		break;

	case V4L2_CID_AUDIO_TREBLE:
		ctrl->value = state->audio_main_treble;
		break;

	case V4L2_CID_AUDIO_BASS:
		ctrl->value = state->audio_main_bass;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static struct v4l2_queryctrl saa717x_qctrl[] = {
	{
		.id            = V4L2_CID_BRIGHTNESS,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Brightness",
		.minimum       = 0,
		.maximum       = 255,
		.step          = 1,
		.default_value = 128,
		.flags         = 0,
	}, {
		.id            = V4L2_CID_CONTRAST,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Contrast",
		.minimum       = 0,
		.maximum       = 255,
		.step          = 1,
		.default_value = 64,
		.flags         = 0,
	}, {
		.id            = V4L2_CID_SATURATION,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Saturation",
		.minimum       = 0,
		.maximum       = 255,
		.step          = 1,
		.default_value = 64,
		.flags         = 0,
	}, {
		.id            = V4L2_CID_HUE,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Hue",
		.minimum       = -128,
		.maximum       = 127,
		.step          = 1,
		.default_value = 0,
		.flags 	       = 0,
	}, {
		.id            = V4L2_CID_AUDIO_VOLUME,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Volume",
		.minimum       = 0,
		.maximum       = 65535,
		.step          = 65535/100,
		.default_value = 58880,
		.flags         = 0,
	}, {
		.id            = V4L2_CID_AUDIO_BALANCE,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Balance",
		.minimum       = 0,
		.maximum       = 65535,
		.step          = 65535/100,
		.default_value = 32768,
		.flags         = 0,
	}, {
		.id            = V4L2_CID_AUDIO_MUTE,
		.type          = V4L2_CTRL_TYPE_BOOLEAN,
		.name          = "Mute",
		.minimum       = 0,
		.maximum       = 1,
		.step          = 1,
		.default_value = 1,
		.flags         = 0,
	}, {
		.id            = V4L2_CID_AUDIO_BASS,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Bass",
		.minimum       = 0,
		.maximum       = 65535,
		.step          = 65535/100,
		.default_value = 32768,
	}, {
		.id            = V4L2_CID_AUDIO_TREBLE,
		.type          = V4L2_CTRL_TYPE_INTEGER,
		.name          = "Treble",
		.minimum       = 0,
		.maximum       = 65535,
		.step          = 65535/100,
		.default_value = 32768,
	},
};

static int saa717x_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct saa717x_state *decoder = i2c_get_clientdata(client);

        v4l_dbg(1, debug, client,"IOCTL: %08x\n",cmd);
   
	switch (cmd) {
	case VIDIOC_INT_AUDIO_CLOCK_FREQ:
		return saa717x_set_audio_clock_freq(client, *(u32 *)arg);

	case VIDIOC_G_CTRL:
		return saa717x_get_v4lctrl(client, (struct v4l2_control *)arg);

	case VIDIOC_S_CTRL:
		return saa717x_set_v4lctrl(client, (struct v4l2_control *)arg);

	case VIDIOC_QUERYCTRL:
	{
		struct v4l2_queryctrl *qc = arg;
		int i;

		for (i = 0; i < ARRAY_SIZE(saa717x_qctrl); i++)
			if (qc->id && qc->id == saa717x_qctrl[i].id) {
				memcpy(qc, &saa717x_qctrl[i], sizeof(*qc));
				return 0;
			}
		return -EINVAL;
	}

#ifdef CONFIG_VIDEO_ADV_DEBUG
	case VIDIOC_INT_G_REGISTER:
	{
		struct v4l2_register *reg = arg;

		if (reg->i2c_id != I2C_DRIVERID_SAA717X)
			return -EINVAL;
		reg->val = readreg(client, (reg->reg >> 8) & 0xff, reg->reg & 0xff);
		break;
	}

	case VIDIOC_INT_S_REGISTER:
	{
		struct v4l2_register *reg = arg;
		u16 addr = reg->reg & 0xffff;
		u8 val = reg->val & 0xff;

		if (reg->i2c_id != I2C_DRIVERID_SAA717X)
			return -EINVAL;
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		saa717x_write(client, addr >> 8, addr & 0xff, val);
		break;
	}
#endif

	case VIDIOC_S_FMT:
	{
		struct v4l2_format *fmt = (struct v4l2_format *)arg;
		struct v4l2_pix_format *pix;
		int prescale, h_scale, v_scale;

		pix = &(fmt->fmt.pix);
		v4l_dbg(1, debug, client, "decoder set size\n");

		// FIXME need better bounds checking here
		if ( (pix->width < 1) || (pix->width > 1440))
			return -EINVAL;
		if ( (pix->height < 1) || (pix->height > 960))
			return -EINVAL;

		// scaling setting
		// NTSC and interlace only ***FIXME!!
		prescale = SAA_7114_NTSC_WIDTH / pix->width;
		if (prescale == 0) prescale = 1;
		h_scale = 1024 * SAA_7114_NTSC_WIDTH / prescale / pix->width;
		v_scale = 512 * 2 /* interlace */ * SAA_7114_NTSC_HEIGHT / pix->height;

		// *** Horizonal prescaling etc ***
		set_h_prescale(client,0,prescale);
		set_h_prescale(client,1,prescale);

		// *** Horizonal scaling increment ***
		// TASK A
		saa717x_write (client, 0x00,0x6C, (u8)(h_scale & 0xFF));
		saa717x_write (client, 0x00,0x6D, (u8)((h_scale >> 8)&0xFF));
		// TASK B
		saa717x_write (client, 0x00,0xAC, (u8)(h_scale & 0xFF));
		saa717x_write (client, 0x00,0xAD, (u8)((h_scale >> 8)&0xFF));

		// *** Vertical prescaling etc ***
		set_v_scale(client,0,v_scale);
		set_v_scale(client,1,v_scale);

		// *** set video output size ***
		// *** video number of pixels at output ***
		// TASK A
		saa717x_write (client, 0x00,0x5C, (u8)(pix->width & 0xFF));
		saa717x_write (client, 0x00,0x5D, (u8)((pix->width >> 8)&0xFF));
		// TASK B
		saa717x_write (client, 0x00,0x9C, (u8)(pix->width & 0xFF));
		saa717x_write (client, 0x00,0x9D, (u8)((pix->width >> 8)&0xFF));

		// *** video number of lines at output ***
		// TASK A
		saa717x_write (client, 0x00,0x5E, (u8)(pix->height & 0xFF));
		saa717x_write (client, 0x00,0x5F, (u8)((pix->height >> 8)&0xFF));
		// TASK B
		saa717x_write (client, 0x00,0x9E, (u8)(pix->height & 0xFF));
		saa717x_write (client, 0x00,0x9F, (u8)((pix->height >> 8)&0xFF));
		break;
	}

	case AUDC_SET_RADIO:
		decoder->radio = 1;
		break;

	case VIDIOC_S_STD:
	{
		u16 *iarg = arg;

		v4l_dbg(1, debug, client, "decoder set norm ");
		v4l_dbg(1, debug, client, "(not yet implementd)\n");

		decoder->radio = 0;
		decoder->norm = *iarg;
		break;
	}

	case VIDIOC_S_INPUT:
	{
		int inp = *(int *)arg;
		int is_tuner = inp & 0x80;  /* tuner input flag */

		inp &= 0x7f;

		v4l_dbg(1, debug, client, "decoder set input (%d)\n", inp);
		/* inputs from 0-9 are available*/
		/* saa717x have mode0-mode9 but mode5 is reserved. */
		if (inp < 0 || inp > 9 || inp == 5) {
			return -EINVAL;
		}

		if (decoder->input != inp) {
			int input_line = inp;
			int input_type = 0;

			decoder->input = input_line;
			v4l_dbg(1, debug, client,  "now setting %s input %d\n",
					input_line >= 6 ? "S-Video" : "Composite",
					input_line);

			/* select mode */
			saa717x_write(client, 0x01, 0x02,
					(saa717x_read(client,0x01,0x02) & 0xf0)|
					input_line);

			/* bypass chrominance trap for modes 6..9 */
			saa717x_write(client, 0x01,0x09,
					(saa717x_read(client,0x01,0x09) & 0x7f)|
					(input_line < 6 ? 0x0 : 0x80));

			if (is_tuner)
				input_type = SAA717X_REG_INIT_TUNER;
			else if (input_line >= 6)
				input_type = SAA717X_REG_INIT_SVIDEO;
			else
				input_type = SAA717X_REG_INIT_COMPOSITE;
			
			decoder->input_type = input_type;
			// change audio_mode
			if (input_type == SAA717X_REG_INIT_TUNER) {
				// tuner
				set_audio_mode(client, decoder->tuner_audio_mode);
			} else {
				// set force toSTEREO mode if Composite or S-Video were choose
				set_audio_mode(client, TUNER_AUDIO_STEREO);
			}
			// change initialize procedure (Composite/S-Video)
			batch_write_regs(client, input_type);
		}
		break;
	}

        case VIDIOC_S_AUDIO:
	{
		struct v4l2_audio *input = arg;

		if (input->index<3) { // FIXME! --tadachi
			decoder->audio_input = input->index;
			v4l_dbg(1, debug, client,
				   "set decoder audio input to %d\n",input->index);
			set_audio_regs(client,decoder);
		} else return -ERANGE;
		break;
	}

        case VIDIOC_G_AUDIO:
	{
		struct v4l2_audio *input = arg;

		memset(input, 0, sizeof(*input));
		input->index = decoder->audio_input;
		break;
	}

	case VIDIOC_STREAMON:
	{
		v4l_dbg(1, debug, client, "decoder enable output\n");
		decoder->enable = 1;
		saa717x_write(client,0x01,0x93,0xa6);
		break;
	}

	case VIDIOC_STREAMOFF:
	{
		v4l_dbg(1, debug, client, "decoder disable output\n");
		decoder->enable = 0;
		saa717x_write(client,0x01,0x93,0x26); // right? FIXME!--tadachi
		break;
	}

	// change audio mode
	case VIDIOC_S_TUNER:
	{
		struct v4l2_tuner *vt=(struct v4l2_tuner *)arg;
		int audio_mode;
		char *mes[4]={
			"MONO","STEREO","LANG1","LANG2/SAP"
		};

		audio_mode = V4L2_TUNER_MODE_STEREO;

		switch (vt->audmode) {
			case V4L2_TUNER_MODE_MONO:
				audio_mode = TUNER_AUDIO_MONO;
				break;
			case V4L2_TUNER_MODE_STEREO:
				audio_mode = TUNER_AUDIO_STEREO;
				break;
			case V4L2_TUNER_MODE_LANG2:// LANG2=SAP in videodev2.h
				audio_mode = TUNER_AUDIO_LANG2;
				break;
			case V4L2_TUNER_MODE_LANG1:
				audio_mode = TUNER_AUDIO_LANG1;
				break;
		}

		v4l_dbg(1, debug, client, "change audio mode to %s\n",
				mes[audio_mode]);
		decoder->tuner_audio_mode = audio_mode;
		// The registers are not changed here.
		// See DECODER_ENABLE_OUTPUT section.
		set_audio_mode(client, decoder->tuner_audio_mode);
		break;
	}

	case VIDIOC_G_TUNER:
	{
		struct v4l2_tuner *vt=(struct v4l2_tuner *)arg;
		int dual_f, stereo_f;

		if (decoder->radio)
			break;
		get_inf_dev_status(client,&dual_f,&stereo_f);

		v4l_dbg(1, debug, client, "DETECT==st:%d dual:%d\n",stereo_f,dual_f);

		/* mono */
		if ((dual_f == 0) && (stereo_f == 0)) {
			vt->rxsubchans = V4L2_TUNER_SUB_MONO;
			v4l_dbg(1, debug, client, "DETECT==MONO\n");
		}

		/* stereo */
		if (stereo_f == 1) {
			if ((vt->audmode == V4L2_TUNER_MODE_STEREO)
					|| (vt->audmode == V4L2_TUNER_MODE_LANG1)) {
				vt->rxsubchans = V4L2_TUNER_SUB_STEREO;
				v4l_dbg(1, debug, client, "DETECT==ST(ST)\n");
			} else {
				vt->rxsubchans = V4L2_TUNER_SUB_MONO;
				v4l_dbg(1, debug, client, "DETECT==ST(MONO)\n");
			}
		}

		/* dual*/
		if (dual_f == 1) {
			if (vt->audmode == V4L2_TUNER_MODE_LANG2) {
				vt->rxsubchans = V4L2_TUNER_SUB_LANG2 | V4L2_TUNER_SUB_MONO;
				v4l_dbg(1, debug, client, "DETECT==DUAL1\n");
			} else {
				vt->rxsubchans = V4L2_TUNER_SUB_LANG1 | V4L2_TUNER_SUB_MONO;
				v4l_dbg(1, debug, client, "DETECT==DUAL2\n");
			}
		}
		break;
	}
	
	case VIDIOC_LOG_STATUS:
		/* not yet implemented */
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */


/* i2c implementation */

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] = { 0x42 >> 1, 0x42 >> 1, I2C_CLIENT_END };
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif	/* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) */

I2C_CLIENT_INSMOD;

struct i2c_driver i2c_driver;


/* ----------------------------------------------------------------------- */
static int saa717x_detect_client (struct i2c_adapter *adapter,
		       int                 address,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		       unsigned short flags, 
#endif
		       int                 kind)
{
	struct i2c_client *client;
	struct saa717x_state *decoder;

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return -ENOMEM;
	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver;
	/*
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
	client->flags = I2C_CLIENT_ALLOW_USE;
#endif*/
	snprintf(client->name, sizeof(client->name) - 1, "saa717x");

	decoder = kmalloc(sizeof(struct saa717x_state), GFP_KERNEL);
	i2c_set_clientdata(client, decoder); 

	if (decoder == NULL) {
		kfree(client);
		return -ENOMEM;
	}
	memset(decoder, 0, sizeof(struct saa717x_state));
	decoder->norm = VIDEO_MODE_NTSC;
	decoder->input = -1;
	decoder->enable = 1;

	// tune these parameters
	decoder->bright = 0x80;
	decoder->contrast = 0x44;
	decoder->sat = 0x40;
	decoder->hue = 0x00;

	// FIXME!!
	decoder->playback = 0;	// initially capture mode used
	decoder->audio = 1; /* DECODER_AUDIO_48_KHZ; */

	decoder->audio_input = 2; //FIXME!!

	decoder->tuner_audio_mode = TUNER_AUDIO_STEREO;
	// set volume, bass and treble
	decoder->audio_main_vol_l = 6;
	decoder->audio_main_vol_r = 6;
	decoder->audio_main_bass = 0;
	decoder->audio_main_treble = 0;
	decoder->audio_main_mute = 0;
	decoder->audio_main_balance = 32768;
	// normalize (24 to -40 (not -84) -> 65535 to 0)
	decoder->audio_main_volume = (decoder->audio_main_vol_r + 41) * 65535/(24-(-40));

	v4l_dbg(1, debug, client, "writing init values\n");

	// FIXME!!
	batch_write_regs(client, SAA717X_REG_INIT_INIT);
	set_video_output_level_regs(client, decoder);
	// set bass,treble to 0db 20041101 K.Ohta
	decoder->audio_main_bass = 0;
	decoder->audio_main_treble = 0;
	set_audio_regs(client, decoder);

	decoder->input_type = 0;

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(2*HZ);
	
	printk("status: (1E) 0x%02x, (1F) 0x%02x\n",
		readreg(client, 0x01,0x1e),
		readreg(client, 0x01,0x1f));

	i2c_attach_client(client);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	MOD_INC_USE_COUNT;
#endif
	return 0;
}

static int
saa717x_attach_adapter (struct i2c_adapter *adapter)
{
	return i2c_probe(adapter, &addr_data, &saa717x_detect_client);
}

static int
saa717x_detach_client (struct i2c_client *client)
{
	struct saa717x *decoder = i2c_get_clientdata(client);
	int err;

	err = i2c_detach_client(client);
	if (err) {
		return err;
	}

	kfree(decoder);
	kfree(client);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	MOD_DEC_USE_COUNT;
#endif
	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
struct i2c_driver i2c_driver = {
	/*
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) && LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
	.owner = THIS_MODULE,
#endif
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
	.name = "saa717x",
	.flags = I2C_DF_NOTIFY,
#else*/
	.driver = {
		.name = "saa717x",
	},
/*#endif*/
	.id = I2C_DRIVERID_SAA717X,
	.attach_adapter = saa717x_attach_adapter,
	.detach_client = saa717x_detach_client,
	.command = saa717x_command,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
EXPORT_NO_SYMBOLS;
#endif

static int __init saa717x_init_module(void)
{
	return i2c_add_driver(&i2c_driver);
}

static void __exit saa717x_exit_module(void)
{
	i2c_del_driver(&i2c_driver);
}

module_init(saa717x_init_module);
module_exit(saa717x_exit_module);
