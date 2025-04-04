#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <linux/init.h>

#include "compat.h"
#include "tuner.h"
#include <media/audiochip.h>

/* Addresses to scan */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c[] = { I2C_CLIENT_END };
static unsigned short normal_i2c_range[] = { 0x60, 0x6f, I2C_CLIENT_END };
#else
static unsigned short normal_i2c[] = {
 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
I2C_CLIENT_END
};
#endif

I2C_CLIENT_INSMOD;

#define UNSET (-1U)

/* insmod options */
static unsigned int debug = 0;
static unsigned int type = UNSET;
static unsigned int addr = 0;
static unsigned int tv_range[2] = { 44, 958 };
static unsigned int radio_range[2] = { 65, 108 };
static unsigned int tv_antenna = 1;
static unsigned int radio_antenna = 0;
static unsigned int no_autodetect = 0;
static char pal[] = "-";

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
MODULE_PARM(no_autodetect, "i");
MODULE_PARM(debug, "i");
MODULE_PARM(type, "i");
MODULE_PARM(addr, "i");
MODULE_PARM(tv_range, "2i");
MODULE_PARM(radio_range, "2i");
MODULE_PARM(tv_antenna, "i");
MODULE_PARM(radio_antenna, "i");
MODULE_PARM(pal, "s");
#else
module_param(no_autodetect, int, 0444);
module_param(debug, int, 0644);
module_param(type, int, 0444);
module_param(addr, int, 0444);
module_param(tv_antenna, int, 0444);
module_param(radio_antenna, int, 0444);
module_param_string(pal, pal, sizeof(pal), 0644);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 10)
module_param_array(tv_range, int, NULL, 0644);
module_param_array(radio_range, int, NULL, 0644);
#else
static unsigned int dummy = 0;
module_param_array(tv_range, int, dummy, 0644);
module_param_array(radio_range, int, dummy, 0644);
#endif /* LINUX VERSION CODE >= 2.6.10 */
#endif /* LINUX_VERSION_CODE < 2.6.0 */

MODULE_PARM_DESC(debug, "Debug level (0-1)");
MODULE_PARM_DESC(pal, "Type of PAL signal (b,g,i,d,k)");
MODULE_PARM_DESC(no_autodetect, "Disable TEA5767 autodetection (0-1)");
MODULE_PARM_DESC(addr, "Override i2c address to scan");
MODULE_PARM_DESC(tv_range, \
	"start,end permitted frequency range for TV, in MHz");
MODULE_PARM_DESC(radio_range, \
	"start,end permitted frequency range for radio, in MHz");

#define optimize_vco 1

MODULE_DESCRIPTION("device driver for various TV and TV+FM radio tuners");
MODULE_AUTHOR("Ralph Metzler, Gerd Knorr, Gunther Mayer");
MODULE_LICENSE("GPL");

static int this_adap;
#define dprintk     if (debug) printk

struct tuner {
	unsigned int type;	/* chip type */
	unsigned int freq;	/* keep track of the current settings */
	v4l2_std_id std;
	int using_v4l2;

	unsigned int radio;
	unsigned int input;

	// only for MT2032
	unsigned int xogc;
	unsigned int radio_if2;

	void (*tv_freq) (struct i2c_client * c, unsigned int freq);
	void (*radio_freq) (struct i2c_client * c, unsigned int freq);
        struct i2c_client *i2c;
};

static struct i2c_driver driver;
static struct i2c_client client_template;

/* ---------------------------------------------------------------------- */

/* tv standard selection for Temic 4046 FM5
   this value takes the low bits of control byte 2
   from datasheet Rev.01, Feb.00 
     standard     BG      I       L       L2      D
     picture IF   38.9    38.9    38.9    33.95   38.9
     sound 1      33.4    32.9    32.4    40.45   32.4
     sound 2      33.16   
     NICAM        33.05   32.348  33.05           33.05
 */
#define TEMIC_SET_PAL_I         0x05
#define TEMIC_SET_PAL_DK        0x09
#define TEMIC_SET_PAL_L         0x0a	// SECAM ?
#define TEMIC_SET_PAL_L2        0x0b	// change IF !
#define TEMIC_SET_PAL_BG        0x0c

/* tv tuner system standard selection for Philips FQ1216ME
   this value takes the low bits of control byte 2
   from datasheet "1999 Nov 16" (supersedes "1999 Mar 23")
     standard 		BG	DK	I	L	L`
     picture carrier	38.90	38.90	38.90	38.90	33.95
     colour		34.47	34.47	34.47	34.47	38.38
     sound 1		33.40	32.40	32.90	32.40	40.45
     sound 2		33.16	-	-	-	-
     NICAM		33.05	33.05	32.35	33.05	39.80
 */
#define PHILIPS_SET_PAL_I	0x01	/* Bit 2 always zero ! */
#define PHILIPS_SET_PAL_BGDK	0x09
#define PHILIPS_SET_PAL_L2	0x0a
#define PHILIPS_SET_PAL_L	0x0b

/* system switching for Philips FI1216MF MK2
   from datasheet "1996 Jul 09",
    standard         BG     L      L'
    picture carrier  38.90  38.90  33.95
    colour	     34.47  34.37  38.38
    sound 1          33.40  32.40  40.45
    sound 2          33.16  -      -
    NICAM            33.05  33.05  39.80
 */
#define PHILIPS_MF_SET_BG	0x01	/* Bit 2 must be zero, Bit 3 is system output */
#define PHILIPS_MF_SET_PAL_L	0x03	// France
#define PHILIPS_MF_SET_PAL_L2	0x02	// L'

/* ---------------------------------------------------------------------- */

struct tunertype {
	char *name;
	unsigned char Vendor;
	unsigned char Type;

	unsigned short thresh1;	/*  band switch VHF_LO <=> VHF_HI  */
	unsigned short thresh2;	/*  band switch VHF_HI <=> UHF     */
	unsigned char VHF_L;	/*  band switch byte, low band     */
	unsigned char VHF_H;	/*  band switch byte, mid band     */
	unsigned char UHF;	/*  band switch byte, high band    */
	unsigned char config;	/*  config byte, aka control byte  */
	unsigned short IFPCoff;	/*  Intermediate Frequency Picture Carrier:
				   622.4 = 16*38.90 MHz PAL, 
				   732   = 16*45.75 NTSCi, 
				   940   = 58.75 NTSC-Japan */
};

/*
 *	The floats in the tuner struct are computed at compile time
 *	by gcc and cast back to integers. Thus we don't violate the
 *	"no float in kernel" rule.
 */
static struct tunertype tuners[] = {
        /* 0 - 9 */
	{ "Temic PAL (4002 FH5)", TEMIC, PAL,
	  16 * 140.25, 16 * 463.25, 0x02, 0x04, 0x01, 0x8e, 623},
	{ "Philips PAL_I (FI1246 and compatibles)", Philips, PAL_I,
	  16 * 140.25, 16 * 463.25, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Philips NTSC (FI1236,FM1236 and compatibles)", Philips, NTSC,
	  16 * 157.25, 16 * 451.25, 0xA0, 0x90, 0x30, 0x8e, 732},
	{ "Philips (SECAM+PAL_BG) (FI1216MF, FM1216MF, FR1216MF)", Philips, SECAM,
	  16 * 168.25, 16 * 447.25, 0xA7, 0x97, 0x37, 0x8e, 623},
	{ "NoTuner", NoTuner, NOTUNER,
	  0, 0, 0x00, 0x00, 0x00, 0x00, 0x00},
	{ "Philips PAL_BG (FI1216 and compatibles)", Philips, PAL,
	  16 * 168.25, 16 * 447.25, 0xA0, 0x90, 0x30, 0x8e, 623},
	{ "Temic NTSC (4032 FY5)", TEMIC, NTSC,
	  16 * 157.25, 16 * 463.25, 0x02, 0x04, 0x01, 0x8e, 732},
	{ "Temic PAL_I (4062 FY5)", TEMIC, PAL_I,
	  16 * 170.00, 16 * 450.00, 0x02, 0x04, 0x01, 0x8e, 623},
	{ "Temic NTSC (4036 FY5)", TEMIC, NTSC,
	  16 * 157.25, 16 * 463.25, 0xa0, 0x90, 0x30, 0x8e, 732},
	{ "Alps HSBH1", TEMIC, NTSC,
	  16 * 137.25, 16 * 385.25, 0x01, 0x02, 0x08, 0x8e, 732},

        /* 10 - 19 */
	{ "Alps TSBE1", TEMIC, PAL,
	  16 * 137.25, 16 * 385.25, 0x01, 0x02, 0x08, 0x8e, 732},
	{ "Alps TSBB5", Alps, PAL_I,	/* tested (UK UHF) with Modulartech MM205 */
	  16 * 133.25, 16 * 351.25, 0x01, 0x02, 0x08, 0x8e, 632},
	{ "Alps TSBE5", Alps, PAL,	/* untested - data sheet guess. Only IF differs. */
	  16 * 133.25, 16 * 351.25, 0x01, 0x02, 0x08, 0x8e, 622},
	{ "Alps TSBC5", Alps, PAL,	/* untested - data sheet guess. Only IF differs. */
	  16 * 133.25, 16 * 351.25, 0x01, 0x02, 0x08, 0x8e, 608},
	{ "Temic PAL_BG (4006FH5)", TEMIC, PAL,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Alps TSCH6", Alps, NTSC,
	  16 * 137.25, 16 * 385.25, 0x14, 0x12, 0x11, 0x8e, 732},
	{ "Temic PAL_DK (4016 FY5)", TEMIC, PAL,
	  16 * 168.25, 16 * 456.25, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Philips NTSC_M (MK2)", Philips, NTSC,
	  16 * 160.00, 16 * 454.00, 0xa0, 0x90, 0x30, 0x8e, 732},
	{ "Temic PAL_I (4066 FY5)", TEMIC, PAL_I,
	  16 * 169.00, 16 * 454.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Temic PAL* auto (4006 FN5)", TEMIC, PAL,
	  16 * 169.00, 16 * 454.00, 0xa0, 0x90, 0x30, 0x8e, 623},

        /* 20 - 29 */
	{ "Temic PAL_BG (4009 FR5) or PAL_I (4069 FR5)", TEMIC, PAL,
	  16 * 141.00, 16 * 464.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Temic NTSC (4039 FR5)", TEMIC, NTSC,
	  16 * 158.00, 16 * 453.00, 0xa0, 0x90, 0x30, 0x8e, 732},
	{ "Temic PAL/SECAM multi (4046 FM5)", TEMIC, PAL,
	  16 * 169.00, 16 * 454.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Philips PAL_DK (FI1256 and compatibles)", Philips, PAL,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Philips PAL/SECAM multi (FQ1216ME)", Philips, PAL,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0xce, 623},
	{ "LG PAL_I+FM (TAPC-I001D)", LGINNOTEK, PAL_I,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "LG PAL_I (TAPC-I701D)", LGINNOTEK, PAL_I,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "LG NTSC+FM (TPI8NSR01F)", LGINNOTEK, NTSC,
	  16 * 210.00, 16 * 497.00, 0xa0, 0x90, 0x30, 0x8e, 732},
	{ "LG PAL_BG+FM (TPI8PSB01D)", LGINNOTEK, PAL,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "LG PAL_BG (TPI8PSB11D)", LGINNOTEK, PAL,
	  16 * 170.00, 16 * 450.00, 0xa0, 0x90, 0x30, 0x8e, 623},
        
        /* 30 - 39 */
	{ "Temic PAL* auto + FM (4009 FN5)", TEMIC, PAL,
	  16 * 141.00, 16 * 464.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "SHARP NTSC_JP (2U5JF5540)", SHARP, NTSC,	/* 940=16*58.75 NTSC@Japan */
	  16 * 137.25, 16 * 317.25, 0x01, 0x02, 0x08, 0x8e, 732},	// Corrected to NTSC=732 (was:940)
	{ "Samsung PAL TCPM9091PD27", Samsung, PAL,	/* from sourceforge v3tv */
	  16 * 169, 16 * 464, 0xA0, 0x90, 0x30, 0x8e, 623},
	{ "MT20xx universal", Microtune, PAL | NTSC,
	  0, 0, 0, 0, 0, 0, 0},
	{ "Temic PAL_BG (4106 FH5)", TEMIC, PAL,
	  16 * 141.00, 16 * 464.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Temic PAL_DK/SECAM_L (4012 FY5)", TEMIC, PAL,
	  16 * 140.25, 16 * 463.25, 0x02, 0x04, 0x01, 0x8e, 623},
	{ "Temic NTSC (4136 FY5)", TEMIC, NTSC,
	  16 * 158.00, 16 * 453.00, 0xa0, 0x90, 0x30, 0x8e, 732},
	{ "LG PAL (newer TAPC series)", LGINNOTEK, PAL,
	  16 * 170.00, 16 * 450.00, 0x01, 0x02, 0x08, 0xce, 623},
	{ "Philips PAL/SECAM multi (FM1216ME MK3)", Philips, PAL,
	  16 * 158.00, 16 * 442.00, 0x01, 0x02, 0x04, 0x8e, 623},
	{ "LG NTSC (newer TAPC series)", LGINNOTEK, NTSC,
	  16 * 170.00, 16 * 450.00, 0x01, 0x02, 0x08, 0x8e, 732},
        
        /* 40 - 49 */
	{ "HITACHI V7-J180AT", HITACHI, NTSC,
	  16 * 170.00, 16 * 450.00, 0x01, 0x02, 0x00, 0x8e, 940},
	{ "Philips PAL_MK (FI1216 MK)", Philips, PAL,
	  16 * 140.25, 16 * 463.25, 0x01, 0xc2, 0xcf, 0x8e, 623},
	{ "Philips 1236D ATSC/NTSC daul in", Philips, ATSC,
	  16 * 157.25, 16 * 454.00, 0xa0, 0x90, 0x30, 0x8e, 732},
	{ "Philips NTSC MK3 (FM1236MK3 or FM1236/F)", Philips, NTSC,
	  16 * 160.00, 16 * 442.00, 0x01, 0x02, 0x04, 0x8e, 732},
	{ "Philips 4 in 1 (ATI TV Wonder Pro/Conexant)", Philips, NTSC,
	  16 * 160.00, 16 * 442.00, 0x01, 0x02, 0x04, 0x8e, 732},
	{ "Microtune 4049 FM5", Microtune, PAL,
	  16 * 141.00, 16 * 464.00, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Panasonic VP27s/ENGE4324D", Panasonic, NTSC,
	  16 * 160.00, 16 * 454.00, 0x01, 0x02, 0x08, 0xce, 940},
	{ "LG NTSC (TAPE series)", LGINNOTEK, NTSC,
	  16 * 160.00, 16 * 442.00, 0x01, 0x02, 0x04, 0x8e, 732},
	{ "Tenna TNF 8831 BGFF)", Philips, PAL,
	  16 * 161.25, 16 * 463.25, 0xa0, 0x90, 0x30, 0x8e, 623},
	{ "Microtune 4042 FI5 ATSC/NTSC dual in", Microtune, NTSC,
	  16 * 162.00, 16 * 457.00, 0xa2, 0x94, 0x31, 0x8e, 732},
        
        /* 50 - 59 */
	{ "TCL 2002N", TCL, NTSC,
	  16 * 172.00, 16 * 448.00, 0x01, 0x02, 0x08, 0x8e, 732},
	{ "Philips PAL/SECAM_D (FM 1256 I-H3)", Philips, PAL,
	  16 * 160.00, 16 * 442.00, 0x01, 0x02, 0x04, 0x8e, 623},
	{ "Thomson DDT 7610 (ATSC/NTSC)", THOMSON, ATSC,
	  16 * 157.25, 16 * 454.00, 0x39, 0x3a, 0x3c, 0x8e, 732},
	{ "Philips FQ1286", Philips, NTSC,
	  16 * 160.00, 16 * 454.00, 0x41, 0x42, 0x04, 0x8e, 940},
	{ "tda8290+75", Philips, PAL | NTSC,
	  /* see tda8290.c for details */ },
	{ "TCL 2002MB", TCL, PAL,
	  16 * 170.00, 16 * 450.00, 0x01, 0x02, 0x08, 0xce, 623},
	{ "Philips PAL/SECAM multi (FQ1216AME MK4)", Philips, PAL,
	  16 * 160.00, 16 * 442.00, 0x01, 0x02, 0x04, 0xce, 623},
	{ "Philips FQ1236A MK4", Philips, NTSC,
	  16 * 160.00, 16 * 442.00, 0x01, 0x02, 0x04, 0x8e, 732},
	{ "Ymec TVision TVF-8531MF/8831MF/8731MF", Philips, NTSC,
	  16*160.00,16*454.00,0xa0,0x90,0x30,0x8e,732},
	{ "Ymec TVision TVF-5533MF", Philips, NTSC,
	  16*160.00,16*454.00,0x01,0x02,0x04,0x8e,732},

        /* 60 - 67 */
	{ "Thomson DDT 7611 (ATSC/NTSC)", THOMSON, ATSC,
	  16*157.25,16*454.00,0x39,0x3a,0x3c,0x8e,732},
	{ "Tena TNF9533-D/IF/TNF9533-B/DF", Philips, PAL,
          16*160.25,16*464.25,0x01,0x02,0x04,0x8e,623},
	{ "Philips TEA5767HN FM Radio", Philips, RADIO,
          /* see tea5767.c for details */},
	{ "Philips FMD1216ME MK3 Hybrid Tuner", Philips, PAL,
	  16*160.00,16*442.00,0x51,0x52,0x54,0x86,623 },
	{ "LG TDVS-H062F/TUA6034", LGINNOTEK, NTSC,
	  16*160.00,16*455.00,0x01,0x02,0x04,0x8e,732},
	{ "Ymec TVF66T5-B/DFF", Philips, PAL,
          16*160.25,16*464.25,0x01,0x02,0x08,0x8e,623},
 	{ "LG NTSC (TALN mini series)", LGINNOTEK, NTSC,
	  16*150.00,16*425.00,0x01,0x02,0x08,0x8e,732 },
        { "Philips TD1316 Hybrid Tuner", Philips, PAL,
          16*160.00,16*442.00,0xa1,0xa2,0xa4,0xc8,623 },
};

#define TUNERS ARRAY_SIZE(tuners)

/* ---------------------------------------------------------------------- */

static int tea5767_signal(struct i2c_client *c);
static int tea5767_stereo(struct i2c_client *c);

static int tuner_getstatus(struct i2c_client *c)
{
	unsigned char byte;

	struct tuner *t = i2c_get_clientdata(c);

	if (t->type == TUNER_MT2032)
		return 0;

	if (t->type == TUNER_TEA5767) {
		return tea5767_signal(c) | tea5767_stereo(c);
        }

	if (1 != i2c_master_recv(c, &byte, 1))
		return 0;
	return byte;
}

#define TUNER_POR       0x80
#define TUNER_FL        0x40
#define TUNER_MODE      0x38
#define TUNER_AFC       0x07

#define TUNER_STEREO    0x10	/* radio mode */
#define TUNER_SIGNAL    0x07	/* radio mode */

static int tuner_signal(struct i2c_client *c)
{
	return (tuner_getstatus(c) & TUNER_SIGNAL) << 13;
}

static int tuner_stereo(struct i2c_client *c)
{
	return (tuner_getstatus(c) & TUNER_STEREO);
}

#if 0				/* unused */
static int tuner_islocked(struct i2c_client *c)
{
	return (tuner_getstatus(c) & TUNER_FL);
}

static int tuner_afcstatus(struct i2c_client *c)
{
	return (tuner_getstatus(c) & TUNER_AFC) - 2;
}

static int tuner_mode(struct i2c_client *c)
{
	return (tuner_getstatus(c) & TUNER_MODE) >> 3;
}
#endif

/* ---------------------------------------------------------------------- */

#define MT2032 0x04
#define MT2030 0x06
#define MT2040 0x07
#define MT2050 0x42

static char *microtune_part[] = {
	[MT2030] = "MT2030",
	[MT2032] = "MT2032",
	[MT2040] = "MT2040",
	[MT2050] = "MT2050",
};

// IsSpurInBand()?
static int mt2032_spurcheck(int f1, int f2, int spectrum_from, int spectrum_to)
{
	int n1 = 1, n2, f;

	f1 = f1 / 1000;		//scale to kHz to avoid 32bit overflows
	f2 = f2 / 1000;
	spectrum_from /= 1000;
	spectrum_to /= 1000;

	dprintk("spurcheck f1=%d f2=%d  from=%d to=%d\n", f1, f2, spectrum_from,
		spectrum_to);

	do {
		n2 = -n1;
		f = n1 * (f1 - f2);
		do {
			n2--;
			f = f - f2;
			dprintk(" spurtest n1=%d n2=%d ftest=%d\n", n1, n2, f);

			if ((f > spectrum_from) && (f < spectrum_to))
				printk("mt2032 spurcheck triggered: %d\n", n1);
		} while ((f > (f2 - spectrum_to)) || (n2 > -5));
		n1++;
	} while (n1 < 5);

	return 1;
}

static int mt2032_compute_freq(unsigned int rfin, unsigned int if1, unsigned int if2, unsigned int spectrum_from, unsigned int spectrum_to, unsigned char *buf, int *ret_sel, unsigned int xogc)	//all in Hz
{
	unsigned int fref, lo1, lo1n, lo1a, s, sel, lo1freq, desired_lo1,
	    desired_lo2, lo2, lo2n, lo2a, lo2num, lo2freq;

	fref = 5250 * 1000;	//5.25MHz
	desired_lo1 = rfin + if1;

	lo1 = (2 * (desired_lo1 / 1000) + (fref / 1000)) / (2 * fref / 1000);
	lo1n = lo1 / 8;
	lo1a = lo1 - (lo1n * 8);

	s = rfin / 1000 / 1000 + 1090;

	if (optimize_vco) {
		if (s > 1890)
			sel = 0;
		else if (s > 1720)
			sel = 1;
		else if (s > 1530)
			sel = 2;
		else if (s > 1370)
			sel = 3;
		else
			sel = 4;	// >1090
	} else {
		if (s > 1790)
			sel = 0;	// <1958
		else if (s > 1617)
			sel = 1;
		else if (s > 1449)
			sel = 2;
		else if (s > 1291)
			sel = 3;
		else
			sel = 4;	// >1090
	}
	*ret_sel = sel;

	lo1freq = (lo1a + 8 * lo1n) * fref;

	dprintk("mt2032: rfin=%d lo1=%d lo1n=%d lo1a=%d sel=%d, lo1freq=%d\n",
		rfin, lo1, lo1n, lo1a, sel, lo1freq);

	desired_lo2 = lo1freq - rfin - if2;
	lo2 = (desired_lo2) / fref;
	lo2n = lo2 / 8;
	lo2a = lo2 - (lo2n * 8);
	lo2num = ((desired_lo2 / 1000) % (fref / 1000)) * 3780 / (fref / 1000);	//scale to fit in 32bit arith
	lo2freq =
	    (lo2a + 8 * lo2n) * fref + lo2num * (fref / 1000) / 3780 * 1000;

	dprintk("mt2032: rfin=%d lo2=%d lo2n=%d lo2a=%d num=%d lo2freq=%d\n",
		rfin, lo2, lo2n, lo2a, lo2num, lo2freq);

	if (lo1a < 0 || lo1a > 7 || lo1n < 17 || lo1n > 48 || lo2a < 0
	    || lo2a > 7 || lo2n < 17 || lo2n > 30) {
		printk
		    ("mt2032: frequency parameters out of range: %d %d %d %d\n",
		     lo1a, lo1n, lo2a, lo2n);
		return (-1);
	}

	mt2032_spurcheck(lo1freq, desired_lo2, spectrum_from, spectrum_to);
	// should recalculate lo1 (one step up/down)

	// set up MT2032 register map for transfer over i2c
	buf[0] = lo1n - 1;
	buf[1] = lo1a | (sel << 4);
	buf[2] = 0x86;		// LOGC
	buf[3] = 0x0f;		//reserved
	buf[4] = 0x1f;
	buf[5] = (lo2n - 1) | (lo2a << 5);
	if (rfin > 400 * 1000 * 1000)
		buf[6] = 0xe4;
	else
		buf[6] = 0xf4;	// set PKEN per rev 1.2 
	buf[7] = 8 + xogc;
	buf[8] = 0xc3;		//reserved
	buf[9] = 0x4e;		//reserved
	buf[10] = 0xec;		//reserved
	buf[11] = (lo2num & 0xff);
	buf[12] = (lo2num >> 8) | 0x80;	// Lo2RST

	return 0;
}

static int mt2032_check_lo_lock(struct i2c_client *c)
{
	int try, lock = 0;
	unsigned char buf[2];
	for (try = 0; try < 10; try++) {
		buf[0] = 0x0e;
		i2c_master_send(c, buf, 1);
		i2c_master_recv(c, buf, 1);
		dprintk("mt2032 Reg.E=0x%02x\n", buf[0]);
		lock = buf[0] & 0x06;

		if (lock == 6)
			break;

		dprintk("mt2032: pll wait 1ms for lock (0x%2x)\n", buf[0]);
		udelay(1000);
	}
	return lock;
}

static int mt2032_optimize_vco(struct i2c_client *c, int sel, int lock)
{
	unsigned char buf[2];
	int tad1;

	buf[0] = 0x0f;
	i2c_master_send(c, buf, 1);
	i2c_master_recv(c, buf, 1);
	dprintk("mt2032 Reg.F=0x%02x\n", buf[0]);
	tad1 = buf[0] & 0x07;

	if (tad1 == 0)
		return lock;
	if (tad1 == 1)
		return lock;

	if (tad1 == 2) {
		if (sel == 0)
			return lock;
		else
			sel--;
	} else {
		if (sel < 4)
			sel++;
		else
			return lock;
	}

	dprintk("mt2032 optimize_vco: sel=%d\n", sel);

	buf[0] = 0x0f;
	buf[1] = sel;
	i2c_master_send(c, buf, 2);
	lock = mt2032_check_lo_lock(c);
	return lock;
}

static void mt2032_set_if_freq(struct i2c_client *c, unsigned int rfin,
			       unsigned int if1, unsigned int if2,
			       unsigned int from, unsigned int to)
{
	unsigned char buf[21];
	int lint_try, ret, sel, lock = 0;
	struct tuner *t = i2c_get_clientdata(c);

	dprintk("mt2032_set_if_freq rfin=%d if1=%d if2=%d from=%d to=%d\n",
		rfin, if1, if2, from, to);

	buf[0] = 0;
	ret = i2c_master_send(c, buf, 1);
	i2c_master_recv(c, buf, 21);

	buf[0] = 0;
	ret =
	    mt2032_compute_freq(rfin, if1, if2, from, to, &buf[1], &sel,
				t->xogc);
	if (ret < 0)
		return;

	// send only the relevant registers per Rev. 1.2
	buf[0] = 0;
	ret = i2c_master_send(c, buf, 4);
	buf[5] = 5;
	ret = i2c_master_send(c, buf + 5, 4);
	buf[11] = 11;
	ret = i2c_master_send(c, buf + 11, 3);
	if (ret != 3)
		printk("mt2032_set_if_freq failed with %d\n", ret);

	// wait for PLLs to lock (per manual), retry LINT if not.
	for (lint_try = 0; lint_try < 2; lint_try++) {
		lock = mt2032_check_lo_lock(c);

		if (optimize_vco)
			lock = mt2032_optimize_vco(c, sel, lock);
		if (lock == 6)
			break;

		printk("mt2032: re-init PLLs by LINT\n");
		buf[0] = 7;
		buf[1] = 0x80 + 8 + t->xogc;	// set LINT to re-init PLLs
		i2c_master_send(c, buf, 2);
		mdelay(10);
		buf[1] = 8 + t->xogc;
		i2c_master_send(c, buf, 2);
	}

	if (lock != 6)
		printk("MT2032 Fatal Error: PLLs didn't lock.\n");

	buf[0] = 2;
	buf[1] = 0x20;		// LOGC for optimal phase noise
	ret = i2c_master_send(c, buf, 2);
	if (ret != 2)
		printk("mt2032_set_if_freq2 failed with %d\n", ret);
}

static void mt2032_set_tv_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);
	int if2, from, to;

	// signal bandwidth and picture carrier
	if (t->std & V4L2_STD_525_60) {
		// NTSC
		from = 40750 * 1000;
		to = 46750 * 1000;
		if2 = 45750 * 1000;
	} else {
		// PAL
		from = 32900 * 1000;
		to = 39900 * 1000;
		if2 = 38900 * 1000;
	}

	mt2032_set_if_freq(c, freq * 62500 /* freq*1000*1000/16 */ ,
			   1090 * 1000 * 1000, if2, from, to);
}

static void mt2032_set_radio_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);
	int if2 = t->radio_if2;

	// per Manual for FM tuning: first if center freq. 1085 MHz
	mt2032_set_if_freq(c, freq * 62500 /* freq*1000*1000/16 */ ,
			   1085 * 1000 * 1000, if2, if2, if2);
}

// Initalization as described in "MT203x Programming Procedures", Rev 1.2, Feb.2001
static int mt2032_init(struct i2c_client *c)
{
	struct tuner *t = i2c_get_clientdata(c);
	unsigned char buf[21];
	int ret, xogc, xok = 0;

	// Initialize Registers per spec.
	buf[1] = 2;		// Index to register 2
	buf[2] = 0xff;
	buf[3] = 0x0f;
	buf[4] = 0x1f;
	ret = i2c_master_send(c, buf + 1, 4);

	buf[5] = 6;		// Index register 6
	buf[6] = 0xe4;
	buf[7] = 0x8f;
	buf[8] = 0xc3;
	buf[9] = 0x4e;
	buf[10] = 0xec;
	ret = i2c_master_send(c, buf + 5, 6);

	buf[12] = 13;		// Index register 13
	buf[13] = 0x32;
	ret = i2c_master_send(c, buf + 12, 2);

	// Adjust XOGC (register 7), wait for XOK
	xogc = 7;
	do {
		dprintk("mt2032: xogc = 0x%02x\n", xogc & 0x07);
		mdelay(10);
		buf[0] = 0x0e;
		i2c_master_send(c, buf, 1);
		i2c_master_recv(c, buf, 1);
		xok = buf[0] & 0x01;
		dprintk("mt2032: xok = 0x%02x\n", xok);
		if (xok == 1)
			break;

		xogc--;
		dprintk("mt2032: xogc = 0x%02x\n", xogc & 0x07);
		if (xogc == 3) {
			xogc = 4;	// min. 4 per spec
			break;
		}
		buf[0] = 0x07;
		buf[1] = 0x88 + xogc;
		ret = i2c_master_send(c, buf, 2);
		if (ret != 2)
			printk("mt2032_init failed with %d\n", ret);
	} while (xok != 1);
	t->xogc = xogc;

	t->tv_freq = mt2032_set_tv_freq;
	t->radio_freq = mt2032_set_radio_freq;
	return (1);
}

static void mt2050_set_antenna(struct i2c_client *c, unsigned char antenna)
{
	unsigned char buf[2];
	int ret;

	buf[0] = 6;
	buf[1] = antenna ? 0x11 : 0x10;
	ret = i2c_master_send(c, buf, 2);
	dprintk("mt2050: enabled antenna connector %d\n", antenna);
}

static void mt2050_set_if_freq(struct i2c_client *c, unsigned int freq,
			       unsigned int if2)
{
	unsigned int if1 = 1218 * 1000 * 1000;
	unsigned int f_lo1, f_lo2, lo1, lo2, f_lo1_modulo, f_lo2_modulo, num1,
	    num2, div1a, div1b, div2a, div2b;
	int ret;
	unsigned char buf[6];

	dprintk("mt2050_set_if_freq freq=%d\n", freq);

	f_lo1 = freq + if1;
	f_lo1 = (f_lo1 / 1000000) * 1000000;

	f_lo2 = f_lo1 - freq - if2;
	f_lo2 = (f_lo2 / 50000) * 50000;

	lo1 = f_lo1 / 4000000;
	lo2 = f_lo2 / 4000000;

	f_lo1_modulo = f_lo1 - (lo1 * 4000000);
	f_lo2_modulo = f_lo2 - (lo2 * 4000000);

	num1 = 4 * f_lo1_modulo / 4000000;
	num2 = 4096 * (f_lo2_modulo / 1000) / 4000;

	// todo spurchecks

	div1a = (lo1 / 12) - 1;
	div1b = lo1 - (div1a + 1) * 12;

	div2a = (lo2 / 8) - 1;
	div2b = lo2 - (div2a + 1) * 8;

	dprintk("lo1 lo2 = %d %d\n", lo1, lo2);
	dprintk("num1 num2 div1a div1b div2a div2b= %x %x %x %x %x %x\n", num1,
		num2, div1a, div1b, div2a, div2b);

	buf[0] = 1;
	buf[1] = 4 * div1b + num1;
	if (freq < 275 * 1000 * 1000)
		buf[1] = buf[1] | 0x80;

	buf[2] = div1a;
	buf[3] = 32 * div2b + num2 / 256;
	buf[4] = num2 - (num2 / 256) * 256;
	buf[5] = div2a;
	if (num2 != 0)
		buf[5] = buf[5] | 0x40;

	if (debug) {
		int i;
		printk("bufs is: ");
		for (i = 0; i < 6; i++)
			printk("%x ", buf[i]);
		printk("\n");
	}

	ret = i2c_master_send(c, buf, 6);
	if (ret != 6)
		printk("mt2050_set_if_freq failed with %d\n", ret);
}

static void mt2050_set_tv_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);
	unsigned int if2;

	if (t->std & V4L2_STD_525_60) {
		// NTSC
		if2 = 45750 * 1000;
	} else {
		// PAL
		if2 = 38900 * 1000;
	}
	mt2050_set_if_freq(c, freq * 62500, if2);
	mt2050_set_antenna(c, tv_antenna);
}

static void mt2050_set_radio_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);
	int if2 = t->radio_if2;

	mt2050_set_if_freq(c, freq * 62500, if2);
	mt2050_set_antenna(c, radio_antenna);
}

static int mt2050_init(struct i2c_client *c)
{
	struct tuner *t = i2c_get_clientdata(c);
	unsigned char buf[2];
	int ret;

	buf[0] = 6;
	buf[1] = 0x10;
	ret = i2c_master_send(c, buf, 2);	//  power

	buf[0] = 0x0f;
	buf[1] = 0x0f;
	ret = i2c_master_send(c, buf, 2);	// m1lo

	buf[0] = 0x0d;
	ret = i2c_master_send(c, buf, 1);
	i2c_master_recv(c, buf, 1);

	dprintk("mt2050: sro is %x\n", buf[0]);
	t->tv_freq = mt2050_set_tv_freq;
	t->radio_freq = mt2050_set_radio_freq;
	return 0;
}

static int microtune_init(struct i2c_client *c)
{
	struct tuner *t = i2c_get_clientdata(c);
	char *name;
	unsigned char buf[21];
	int company_code;

	memset(buf, 0, sizeof(buf));
	t->tv_freq = NULL;
	t->radio_freq = NULL;
	name = "unknown";

	i2c_master_send(c, buf, 1);
	i2c_master_recv(c, buf, 21);
	if (debug) {
		int i;
		printk(KERN_DEBUG "tuner: MT2032 hexdump:\n");
		for (i = 0; i < 21; i++) {
			printk(" %02x", buf[i]);
			if (((i + 1) % 8) == 0)
				printk(" ");
			if (((i + 1) % 16) == 0)
				printk("\n ");
		}
		printk("\n ");
	}
	company_code = buf[0x11] << 8 | buf[0x12];
	printk("tuner: microtune: companycode=%04x part=%02x rev=%02x\n",
	       company_code, buf[0x13], buf[0x14]);

#if 0
	/* seems to cause more problems than it solves ... */
	switch (company_code) {
	case 0x30bf:
	case 0x3cbf:
	case 0x3dbf:
	case 0x4d54:
	case 0x8e81:
	case 0x8e91:
		/* ok (?) */
		break;
	default:
		printk("tuner: microtune: unknown companycode\n");
		return 0;
	}
#endif

	if (buf[0x13] < ARRAY_SIZE(microtune_part) &&
	    NULL != microtune_part[buf[0x13]])
		name = microtune_part[buf[0x13]];
	switch (buf[0x13]) {
	case MT2032:
		mt2032_init(c);
		break;
	case MT2050:
		mt2050_init(c);
		break;
	default:
		printk
		    ("tuner: microtune %s found, not (yet?) supported, sorry :-/\n",
		     name);
		return 0;
	}
	printk("tuner: microtune %s found, OK\n", name);
	return 0;
}

/* ---------------------------------------------------------------------- */


#define PREFIX "TEA5767 "

/******************************
 * Write mode register values *
 ******************************/

/* First register */
#define TEA5767_MUTE		0x80	/* Mutes output */
#define TEA5767_SEARCH		0x40	/* Activates station search */
/* Bits 0-5 for divider MSB */

/* Second register */
/* Bits 0-7 for divider LSB */

/* Third register */

/* Station search from botton to up */
#define TEA5767_SEARCH_UP	0x80

/* Searches with ADC output = 10 */
#define TEA5767_SRCH_HIGH_LVL	0x60

/* Searches with ADC output = 10 */
#define TEA5767_SRCH_MID_LVL	0x40

/* Searches with ADC output = 5 */
#define TEA5767_SRCH_LOW_LVL	0x20

/* if on, div=4*(Frf+Fif)/Fref otherwise, div=4*(Frf-Fif)/Freq) */
#define TEA5767_HIGH_LO_INJECT	0x10

/* Disable stereo */
#define TEA5767_MONO		0x08

/* Disable right channel and turns to mono */
#define TEA5767_MUTE_RIGHT	0x04

/* Disable left channel and turns to mono */
#define TEA5767_MUTE_LEFT	0x02

#define TEA5767_PORT1_HIGH	0x01

/* Forth register */
#define TEA5767_PORT2_HIGH	0x80
/* Chips stops working. Only I2C bus remains on */
#define TEA5767_STDBY		0x40

/* Japan freq (76-108 MHz. If disabled, 87.5-108 MHz */
#define TEA5767_JAPAN_BAND	0x20

/* Unselected means 32.768 KHz freq as reference. Otherwise Xtal at 13 MHz */
#define TEA5767_XTAL_32768	0x10

/* Cuts weak signals */
#define TEA5767_SOFT_MUTE	0x08

/* Activates high cut control */
#define TEA5767_HIGH_CUT_CTRL	0x04

/* Activates stereo noise control */
#define TEA5767_ST_NOISE_CTL	0x02

/* If activate PORT 1 indicates SEARCH or else it is used as PORT1 */
#define TEA5767_SRCH_IND	0x01

/* Fiveth register */

/* By activating, it will use Xtal at 13 MHz as reference for divider */
#define TEA5767_PLLREF_ENABLE	0x80

/* By activating, deemphasis=50, or else, deemphasis of 50us */
#define TEA5767_DEEMPH_75	0X40

/*****************************
 * Read mode register values *
 *****************************/

/* First register */
#define TEA5767_READY_FLAG_MASK	0x80
#define TEA5767_BAND_LIMIT_MASK	0X40
/* Bits 0-5 for divider MSB after search or preset */

/* Second register */
/* Bits 0-7 for divider LSB after search or preset */

/* Third register */
#define TEA5767_STEREO_MASK	0x80
#define TEA5767_IF_CNTR_MASK	0x7f

/* Four register */
#define TEA5767_ADC_LEVEL_MASK	0xf0

/* should be 0 */
#define TEA5767_CHIP_ID_MASK	0x0f

/* Fiveth register */
/* Reserved for future extensions */
#define TEA5767_RESERVED_MASK	0xff

enum tea5767_xtal_freq {
        TEA5767_LOW_LO_32768    = 0,
        TEA5767_HIGH_LO_32768   = 1,
        TEA5767_LOW_LO_13MHz    = 2,
        TEA5767_HIGH_LO_13MHz   = 3,
};


/*****************************************************************************/

static void tea5767_set_tv_freq(struct i2c_client *c, unsigned int freq)
{
	//struct tuner *t = i2c_get_clientdata(c);

	printk("This tuner doesn't support TV freq.\n");
}

static void tea5767_status_dump(unsigned char *buffer)
{
	unsigned int div, frq;

	if (TEA5767_READY_FLAG_MASK & buffer[0])
		printk(PREFIX "Ready Flag ON\n");
	else
		printk(PREFIX "Ready Flag OFF\n");

	if (TEA5767_BAND_LIMIT_MASK & buffer[0])
		printk(PREFIX "Tuner at band limit\n");
	else
		printk(PREFIX "Tuner not at band limit\n");

	div = ((buffer[0] & 0x3f) << 8) | buffer[1];

	switch (TEA5767_HIGH_LO_32768) {
	case TEA5767_HIGH_LO_13MHz:
		frq = (div * 50000 - 700000 - 225000) / 4;	/* Freq in KHz */
		break;
	case TEA5767_LOW_LO_13MHz:
		frq = (div * 50000 + 700000 + 225000) / 4;	/* Freq in KHz */
		break;
	case TEA5767_LOW_LO_32768:
		frq = (div * 32768 + 700000 + 225000) / 4;	/* Freq in KHz */
		break;
	case TEA5767_HIGH_LO_32768:
	default:
		frq = (div * 32768 - 700000 - 225000) / 4;	/* Freq in KHz */
		break;
	}
	buffer[0] = (div >> 8) & 0x3f;
	buffer[1] = div & 0xff;

	printk(PREFIX "Frequency %d.%03d KHz (divider = 0x%04x)\n",
	       frq / 1000, frq % 1000, div);

	if (TEA5767_STEREO_MASK & buffer[2])
		printk(PREFIX "Stereo\n");
	else
		printk(PREFIX "Mono\n");

	printk(PREFIX "IF Counter = %d\n", buffer[2] & TEA5767_IF_CNTR_MASK);

	printk(PREFIX "ADC Level = %d\n",
	       (buffer[3] & TEA5767_ADC_LEVEL_MASK) >> 4);

	printk(PREFIX "Chip ID = %d\n", (buffer[3] & TEA5767_CHIP_ID_MASK));

	printk(PREFIX "Reserved = 0x%02x\n",
	       (buffer[4] & TEA5767_RESERVED_MASK));
}

/* Freq should be specifyed at 62.5 Hz */
static void tea5767_set_radio_freq(struct i2c_client *c, unsigned int frq)
{
	//struct tuner *t = i2c_get_clientdata(c);
	unsigned char buffer[5];
	unsigned div;
	int rc;

        frq *= 1000;
	dprintk (PREFIX "radio freq = %d.%03d MHz\n", frq/16000,(frq/16)%1000);

	/* Rounds freq to next decimal value - for 62.5 KHz step */
	/* frq = 20*(frq/16)+radio_frq[frq%16]; */

	buffer[2] = TEA5767_PORT1_HIGH;
	buffer[3] = TEA5767_PORT2_HIGH | TEA5767_HIGH_CUT_CTRL |
		    TEA5767_ST_NOISE_CTL | TEA5767_JAPAN_BAND;
	buffer[4] = 0;

        /*
	if (t->mode == T_STANDBY) {
		dprintk("TEA5767 set to standby mode\n");
		buffer[3] |= TEA5767_STDBY;
	}
        */

        /*
	if (t->audmode == V4L2_TUNER_MODE_MONO) {
		dprintk("TEA5767 set to mono\n");
		buffer[2] |= TEA5767_MONO;
	} else {
		dprintk("TEA5767 set to stereo\n");
	}
        */

	/* Should be replaced */
	switch (TEA5767_HIGH_LO_32768) {
	case TEA5767_HIGH_LO_13MHz:
		dprintk ("TEA5767 radio HIGH LO inject xtal @ 13 MHz\n");
		buffer[2] |= TEA5767_HIGH_LO_INJECT;
		buffer[4] |= TEA5767_PLLREF_ENABLE;
		div = (frq * 4000 / 16 + 700000 + 225000 + 25000) / 50000;
		break;
	case TEA5767_LOW_LO_13MHz:
		dprintk ("TEA5767 radio LOW LO inject xtal @ 13 MHz\n");

		buffer[4] |= TEA5767_PLLREF_ENABLE;
		div = (frq * 4000 / 16 - 700000 - 225000 + 25000) / 50000;
		break;
	case TEA5767_LOW_LO_32768:
		dprintk ("TEA5767 radio LOW LO inject xtal @ 32,768 MHz\n");
		buffer[3] |= TEA5767_XTAL_32768;
		/* const 700=4000*175 Khz - to adjust freq to right value */
		div = ((frq * 4000 / 16 - 700000 - 225000) + 16384) >> 15;
		break;
	case TEA5767_HIGH_LO_32768:
	default:
		dprintk ("TEA5767 radio HIGH LO inject xtal @ 32,768 MHz\n");

		buffer[2] |= TEA5767_HIGH_LO_INJECT;
		buffer[3] |= TEA5767_XTAL_32768;
		div = ((frq * (4000 / 16) + 700000 + 225000) + 16384) >> 15;
		break;
	}
	buffer[0] = (div >> 8) & 0x3f;
	buffer[1] = div & 0xff;

	if (5 != (rc = i2c_master_send(c, buffer, 5)))
		printk("i2c i/o error: rc == %d (should be 5)\n", rc);

	if (debug) {
		if (5 != (rc = i2c_master_recv(c, buffer, 5)))
			printk("i2c i/o error: rc == %d (should be 5)\n", rc);
		else
			tea5767_status_dump(buffer);
	}
}

static int tea5767_signal(struct i2c_client *c)
{
	unsigned char buffer[5];
	int rc;
	//struct tuner *t = i2c_get_clientdata(c);

	memset(buffer, 0, sizeof(buffer));
	if (5 != (rc = i2c_master_recv(c, buffer, 5)))
		printk("i2c i/o error: rc == %d (should be 5)\n", rc);

	return ((buffer[3] & TEA5767_ADC_LEVEL_MASK) << (13 - 4));
}

static int tea5767_stereo(struct i2c_client *c)
{
	unsigned char buffer[5];
	int rc;
	//struct tuner *t = i2c_get_clientdata(c);

	memset(buffer, 0, sizeof(buffer));
	if (5 != (rc = i2c_master_recv(c, buffer, 5)))
		printk("i2c i/o error: rc == %d (should be 5)\n", rc);

	rc = buffer[2] & TEA5767_STEREO_MASK;

	dprintk("TEA5767 radio ST GET = %02x\n", rc);

	return ((buffer[2] & TEA5767_STEREO_MASK) ? V4L2_TUNER_SUB_STEREO : 0);
}

static int tea5767_autodetection(struct i2c_client *c)
{
	unsigned char buffer[7] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	int rc;
	//struct tuner *t = i2c_get_clientdata(c);
#if 0 /* Needed if uncomment I2C send code below */
	int div;
#endif

	if ((rc = i2c_master_recv(c, buffer, 7))< 5) {
		printk("It is not a TEA5767. Received %i bytes.\n", rc);
		return EINVAL;
	}

	/* If all bytes are the same then it's a TV tuner and not a tea5767 */
	if (buffer[0] == buffer[1] && buffer[0] == buffer[2] &&
	    buffer[0] == buffer[3] && buffer[0] == buffer[4]) {
		printk("All bytes are equal. It is not a TEA5767\n");
		return EINVAL;
	}

	/*  Status bytes:
	 *  Byte 4: bit 3:1 : CI (Chip Identification) == 0
	 *          bit 0   : internally set to 0
	 *  Byte 5: bit 7:0 : == 0
	 */
	if (((buffer[3] & 0x0f) != 0x00) || (buffer[4] != 0x00)) {
		printk("Chip ID is not zero. It is not a TEA5767\n");
		return EINVAL;
	}

	/* It seems that tea5767 returns 0xff after the 5th byte */
	if ((buffer[5] != 0xff) || (buffer[6] != 0xff)) {
		printk("Returned more than 5 bytes. It is not a TEA5767\n");
		return EINVAL;
	}

#if 0 /*Sometimes, this code doesn't work */
	/* Sets tuner at some freq (87.5 MHz) and see if it is ok */
	div = ((87500 * 4000 + 700000 + 225000) + 16768) >> 15;
	buffer[0] = ((div >> 8) & 0x3f) | TEA5767_MUTE;
	buffer[1] = div & 0xff;
	buffer[2] = TEA5767_PORT1_HIGH;
	buffer[3] = TEA5767_PORT2_HIGH | TEA5767_HIGH_CUT_CTRL |
		    TEA5767_ST_NOISE_CTL;
	buffer[4] = 0;

	if (5 != (rc = i2c_master_send(c, buffer, 5)))
		printk("i2c i/o error: rc == %d (should be 5)\n", rc);

	msleep(15);

	if (5 != (rc = i2c_master_recv(c, buffer, 5))) {
		printk("It is not a TEA5767. Received %i bytes.\n", rc);
		return EINVAL;
	}

	/* Initial freq for 32.768KHz clock */
	if ((buffer[1] != (div & 0xff) ) || ((buffer[0] & 0x3f) != ((div >> 8) & 0x3f))) {
		printk("It is not a TEA5767. div=%d, Return: %02x %02x %02x %02x %02x\n",
				div,buffer[0],buffer[1],buffer[2],buffer[3],buffer[4]);
		tea5767_status_dump(buffer);
		return EINVAL;
	}
#endif
        printk("TEA5767 detected.\n");
	return 0;
}

static int tea5767_tuner_init(struct i2c_client *c)
{
	struct tuner *t = i2c_get_clientdata(c);

#if 0 /* By removing autodetection allows forcing TEA chip */
	if (tea5767_autodetection(c) == EINVAL)
		return EINVAL;
#endif
	printk("type set to %d (%s)\n", t->type, "Philips TEA5767HN FM Radio");
	strlcpy(c->name, "tea5767", sizeof(c->name));

	t->tv_freq = tea5767_set_tv_freq;
	t->radio_freq = tea5767_set_radio_freq;

	return (0);
}

/* ---------------------------------------------------------------------- */

static void default_set_tv_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);
	u8 config;
	u16 div;
	struct tunertype *tun;
	unsigned char buffer[4];
	int rc;

	tun = &tuners[t->type];
	if (freq < tun->thresh1)
		config = tun->VHF_L;
	else if (freq < tun->thresh2)
		config = tun->VHF_H;
	else
		config = tun->UHF;

	/* tv norm specific stuff for multi-norm tuners */
	switch (t->type) {
	case TUNER_PHILIPS_SECAM:	// FI1216MF
		/* 0x01 -> ??? no change ??? */
		/* 0x02 -> PAL BDGHI / SECAM L */
		/* 0x04 -> ??? PAL others / SECAM others ??? */
		config &= ~0x02;
		if (t->std & V4L2_STD_SECAM)
			config |= 0x02;
		break;

	case TUNER_TEMIC_4046FM5:
		config &= ~0x0f;

		if (t->std & V4L2_STD_PAL_BG) {
			config |= TEMIC_SET_PAL_BG;

		} else if (t->std & V4L2_STD_PAL_I) {
			config |= TEMIC_SET_PAL_I;

		} else if (t->std & V4L2_STD_PAL_DK) {
			config |= TEMIC_SET_PAL_DK;

		} else if (t->std & V4L2_STD_SECAM_L) {
			config |= TEMIC_SET_PAL_L;

		}
		break;

	case TUNER_PHILIPS_FQ1216ME:
		config &= ~0x0f;

		if (t->std & (V4L2_STD_PAL_BG | V4L2_STD_PAL_DK)) {
			config |= PHILIPS_SET_PAL_BGDK;

		} else if (t->std & V4L2_STD_PAL_I) {
			config |= PHILIPS_SET_PAL_I;

		} else if (t->std & V4L2_STD_SECAM_L) {
			config |= PHILIPS_SET_PAL_L;

		}
		break;

	case TUNER_PHILIPS_ATSC:
		/* 0x00 -> ATSC antenna input 1 */
		/* 0x01 -> ATSC antenna input 2 */
		/* 0x02 -> NTSC antenna input 1 */
		/* 0x03 -> NTSC antenna input 2 */
		config &= ~0x03;
		if (t->std & V4L2_STD_ATSC)
			config |= 2;
		/* FIXME: input */
		break;
	}

	/*
	 * Philips FI1216MK2 remark from specification :
	 * for channel selection involving band switching, and to ensure
	 * smooth tuning to the desired channel without causing
	 * unnecessary charge pump action, it is recommended to consider
	 * the difference between wanted channel frequency and the
	 * current channel frequency.  Unnecessary charge pump action
	 * will result in very low tuning voltage which may drive the
	 * oscillator to extreme conditions.
	 *
	 * Progfou: specification says to send config data before
	 * frequency in case (wanted frequency < current frequency).
	 */

	div = freq + tun->IFPCoff;
	if ((t->type == TUNER_PHILIPS_SECAM ||
	     t->type == TUNER_PHILIPS_FM1216ME_MK3) && freq < t->freq) {
		buffer[0] = tun->config;
		buffer[1] = config;
		buffer[2] = (div >> 8) & 0x7f;
		buffer[3] = div & 0xff;
	} else {
		buffer[0] = (div >> 8) & 0x7f;
		buffer[1] = div & 0xff;
		buffer[2] = tun->config;
		buffer[3] = config;
	}
	dprintk("tuner: tv 0x%02x 0x%02x 0x%02x 0x%02x\n",
		buffer[0], buffer[1], buffer[2], buffer[3]);

	if (4 != (rc = i2c_master_send(c, buffer, 4)))
		printk("tuner: i2c i/o error: rc == %d (should be 4)\n", rc);

}

static void default_set_radio_freq(struct i2c_client *c, unsigned int freq)
{
	struct tunertype *tun;
	struct tuner *t = i2c_get_clientdata(c);
	unsigned char buffer[4];
	unsigned div;
	int rc;

	tun = &tuners[t->type];
	switch (t->type) {
	case TUNER_PHILIPS_FM1236_MK3:
	case TUNER_LG_NTSC_TAPE:
   		div = ((freq * 62500) + 10700000) / 50000;
   		buffer[0] = (div >> 8) & 0x7f;
   		buffer[1] = div & 0xff;
   		buffer[2] = 0x80;
		buffer[3] = 0x19;
		break;
	case TUNER_PHILIPS_FM1216ME_MK3:
   		div = ((freq * 62500) + 10700000) / 50000;
		if (freq < t->freq) {
	   		buffer[0] = 0x88;
			buffer[1] = 0x19;
   			buffer[2] = (div >> 8) & 0x7f;
	   		buffer[3] = div & 0xff;
		} else {
   			buffer[0] = (div >> 8) & 0x7f;
	   		buffer[1] = div & 0xff;
	   		buffer[2] = 0x88;
			buffer[3] = 0x19;
		}
		break;
        case TUNER_MICROTUNE_4049FM5:
		div = freq + (int)(16 * 33.3);
		buffer[0] = (div >> 8) & 0x7f;
		buffer[1] = div & 0xff;
		buffer[2] = tun->config;
		buffer[3] = 0xa4;
		break;
	default:
		div = freq + (int)(16 * 10.7);
		buffer[0] = (div >> 8) & 0x7f;
		buffer[1] = div & 0xff;
		buffer[2] = tun->config;
		buffer[3] = 0xa4;
		break;
	}

	dprintk("tuner: radio 0x%02x 0x%02x 0x%02x 0x%02x\n",
		buffer[0], buffer[1], buffer[2], buffer[3]);

	if (4 != (rc = i2c_master_send(c, buffer, 4)))
		printk("tuner: i2c i/o error: rc == %d (should be 4)\n", rc);
}

/* ---------------------------------------------------------------------- */

// Set tuner frequency,  freq in Units of 62.5kHz = 1/16MHz
static void set_tv_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);

	if (t->type == UNSET) {
		printk("tuner: tuner type not set\n");
		return;
	}
	if (NULL == t->tv_freq) {
		printk("tuner: Huh? tv_set is NULL?\n");
		return;
	}
	if (freq < tv_range[0] * 16 || freq > tv_range[1] * 16) {
		/* FIXME: better do that chip-specific, but
		   right now we don't have that in the config
		   struct and this way is still better than no
		   check at all */
		printk("tuner: TV freq (%d.%02d) out of range (%d-%d)\n",
		       freq / 16, freq % 16 * 100 / 16, tv_range[0],
		       tv_range[1]);
		return;
	}
	t->tv_freq(c, freq);
}

static void set_radio_freq(struct i2c_client *c, unsigned int freq)
{
	struct tuner *t = i2c_get_clientdata(c);

	if (t->type == UNSET) {
		printk("tuner: tuner type not set\n");
		return;
	}
	if (NULL == t->radio_freq) {
		printk("tuner: no radio tuning for this one, sorry.\n");
		return;
	}
	if (freq < radio_range[0] * 16 || freq > radio_range[1] * 16) {
		printk("tuner: radio freq (%d.%02d) out of range (%d-%d)\n",
		       freq / 16, freq % 16 * 100 / 16,
		       radio_range[0], radio_range[1]);
		return;
	}
	t->radio_freq(c, freq);
}

static void set_freq(struct i2c_client *c, unsigned long freq)
{
	struct tuner *t = i2c_get_clientdata(c);

	if (t->radio) {
		dprintk("tuner: radio freq set to %lu.%02lu\n",
			freq / 16, freq % 16 * 100 / 16);
		set_radio_freq(c, freq);
	} else {
		dprintk("tuner: tv freq set to %lu.%02lu\n",
			freq / 16, freq % 16 * 100 / 16);
		set_tv_freq(c, freq);
	}
	t->freq = freq;
}

static void set_type(struct i2c_client *c, unsigned int type, char *source)
{
	struct tuner *t = i2c_get_clientdata(c);

	if (t->type != UNSET) {
		if (t->type != type)
			printk("tuner: type already set to %d, "
			       "ignoring request for %d\n", t->type, type);
		return;
	}
	if (type >= TUNERS) {
                printk("tuner: type too large %d >= %d\n", type, (int)TUNERS);
		return;
        }

	t->type = type;
	printk("tuner: type set to %d (%s) by %s\n",
	       t->type, tuners[t->type].name, source);
	strlcpy(c->name, tuners[t->type].name, sizeof(c->name));

	switch (t->type) {
	case TUNER_MT2032:
		microtune_init(c);
		break;
	case TUNER_TEA5767:
		if (tea5767_tuner_init(c) == EINVAL) {
			t->type = TUNER_ABSENT;
                        return;
                }
                break;
	default:
		t->tv_freq = default_set_tv_freq;
		t->radio_freq = default_set_radio_freq;
		break;
	}
}

static int tuner_fixup_std(struct tuner *t)
{
	if ((t->std & V4L2_STD_PAL) == V4L2_STD_PAL) {
		/* get more precise norm info from insmod option */
		switch (pal[0]) {
		case 'b':
		case 'B':
		case 'g':
		case 'G':
			dprintk("insmod fixup: PAL => PAL-BG\n");
			t->std = V4L2_STD_PAL_BG;
			break;
		case 'i':
		case 'I':
			dprintk("insmod fixup: PAL => PAL-I\n");
			t->std = V4L2_STD_PAL_I;
			break;
		case 'd':
		case 'D':
		case 'k':
		case 'K':
			dprintk("insmod fixup: PAL => PAL-DK\n");
			t->std = V4L2_STD_PAL_DK;
			break;
		case '-':
			/* default parameter, do nothing */
			break;
		default:
			printk("tuner: pal= argument not recognised\n");
			break;
		}
	}
	return 0;
}

/* ---------------------------------------------------------------------- */

static int tuner_attach(struct i2c_adapter *adap, int addr, int kind)
{
	struct tuner *t;
	struct i2c_client *client;

	if (this_adap > 1)
		return -1;
	this_adap++;

	client_template.adapter = adap;
	client_template.addr = addr;

	printk("tuner (ivtv): chip found at addr 0x%x i2c-bus %s\n",
	       addr << 1, adap->name);

	if (NULL == (client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL)))
		return -ENOMEM;
	memcpy(client, &client_template, sizeof(struct i2c_client));
	t = kmalloc(sizeof(struct tuner), GFP_KERNEL);
	if (NULL == t) {
		kfree(client);
		return -ENOMEM;
	}
	memset(t, 0, sizeof(struct tuner));
        t->i2c = client;
	i2c_set_clientdata(client, t);
	t->type = UNSET;
	t->radio_if2 = 10700 * 1000;	// 10.7MHz - FM radio

	/* TEA5767 autodetection code - only for addr = 0xc0 */
	if (!no_autodetect) {
		if (addr == 0x60) {
			if (tea5767_autodetection(t->i2c) != EINVAL) {
				t->type = UNSET;
				t->freq = 87.5 * 16; /* Sets freq to FM range */

				i2c_attach_client (t->i2c);
				set_type(t->i2c,TUNER_TEA5767, "autodetect");
				return 0;
			}
		}
	}
	i2c_attach_client(client);
	if (type < TUNERS) {
		set_type(client, type, "insmod option");
		printk
		    ("tuner: The type=<n> insmod option will go away soon.\n");
		printk("tuner: Please use the tuner=<n> option provided by\n");
		printk
		    ("tuner: tv aard core driver (bttv, saa7134, ...) instead.\n");
	}
	return 0;
}

static int tuner_probe(struct i2c_adapter *adap)
{
	if (0 != addr) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
		normal_i2c_range[0] = addr;
		normal_i2c_range[1] = addr;
#else
		normal_i2c[0] = addr;
		normal_i2c[1] = I2C_CLIENT_END;
#endif
	}
	this_adap = 0;

#ifdef I2C_CLASS_TV_ANALOG
	if (adap->class & I2C_CLASS_TV_ANALOG)
		return i2c_probe(adap, &addr_data, tuner_attach);
#else
	switch (adap->id) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
	case I2C_HW_SMBUS_VOODOO3:
	case I2C_HW_B_BT848:
	case I2C_HW_B_RIVA:
#else
	case I2C_ALGO_BIT | I2C_HW_SMBUS_VOODOO3:
	case I2C_ALGO_BIT | I2C_HW_B_BT848:
	case I2C_ALGO_BIT | I2C_HW_B_RIVA:
#endif
	case I2C_ALGO_SAA7134:
	case I2C_ALGO_SAA7146:
		return i2c_probe(adap, &addr_data, tuner_attach);
		break;
	}
#endif
	return 0;
}

static int tuner_detach(struct i2c_client *client)
{
	struct tuner *t = i2c_get_clientdata(client);

	i2c_detach_client(client);
	kfree(t);
	kfree(client);
	return 0;
}

#define SWITCH_V4L2	if (!t->using_v4l2 && debug) \
		          printk("tuner: switching to v4l2\n"); \
	                  t->using_v4l2 = 1;
#define CHECK_V4L2	if (t->using_v4l2) { if (debug) \
			  printk("tuner: ignore v4l1 call\n"); \
		          return 0; }

static int tuner_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct tuner *t = i2c_get_clientdata(client);
	unsigned int *iarg = (int *)arg;

	switch (cmd) {

		/* --- configuration --- */
	case TUNER_SET_TYPE_ADDR:  /* 2.6.13 API */
                set_type(client, ((struct tuner_setup *)arg)->type, client->adapter->name);
                break;
	case TUNER_SET_TYPE:  /* 2.6.12 and lower API */
		set_type(client, *iarg, client->adapter->name);
		break;
	case AUDC_SET_RADIO:
		if (!t->radio) {
			set_tv_freq(client, 400 * 16);
			t->radio = 1;
		}
		break;
	case AUDC_CONFIG_PINNACLE:
		switch (*iarg) {
		case 2:
			dprintk("tuner: pinnacle pal\n");
			t->radio_if2 = 33300 * 1000;
			break;
		case 3:
			dprintk("tuner: pinnacle ntsc\n");
			t->radio_if2 = 41300 * 1000;
			break;
		}
		break;

		/* --- v4l ioctls --- */
		/* take care: bttv does userspace copying, we'll get a
		   kernel pointer here... */
	case VIDIOCSCHAN:
	{
		static const v4l2_std_id map[] = {
			[VIDEO_MODE_PAL] = V4L2_STD_PAL,
			[VIDEO_MODE_NTSC] = V4L2_STD_NTSC_M,
			[VIDEO_MODE_SECAM] = V4L2_STD_SECAM,
			[4 /* bttv */ ] = V4L2_STD_PAL_M,
			[5 /* bttv */ ] = V4L2_STD_PAL_N,
			[6 /* bttv */ ] = V4L2_STD_NTSC_M_JP,
		};
		struct video_channel *vc = arg;

		CHECK_V4L2;
		t->radio = 0;
		if (vc->norm < ARRAY_SIZE(map))
			t->std = map[vc->norm];
		tuner_fixup_std(t);
		if (t->freq)
			set_tv_freq(client, t->freq);
		return 0;
	}
	case VIDIOCSFREQ:
	{
		unsigned long v = *(unsigned long *)arg;

		CHECK_V4L2;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
		if (t->radio) {
                        v /= 1000;
                }
#endif
		set_freq(client, v);
		return 0;
	}
	case VIDIOCGTUNER:
	{
		struct video_tuner *vt = arg;

		CHECK_V4L2;
		if (t->radio)
			vt->signal = tuner_signal(client);
		return 0;
	}
	case VIDIOCGAUDIO:
	{
		struct video_audio *va = arg;

		CHECK_V4L2;
		if (t->radio)
			va->mode =
			    (tuner_stereo(client) ? VIDEO_SOUND_STEREO :
			     VIDEO_SOUND_MONO);
		return 0;
	}

	case VIDIOC_S_STD:
	{
		v4l2_std_id *id = arg;

		SWITCH_V4L2;
		t->radio = 0;
		t->std = *id;
		tuner_fixup_std(t);
		if (t->freq)
			set_freq(client, t->freq);
		break;
	}
	case VIDIOC_S_FREQUENCY:
	{
		struct v4l2_frequency *f = arg;

		SWITCH_V4L2;
		if (V4L2_TUNER_ANALOG_TV == f->type) {
			t->radio = 0;
		}
		if (V4L2_TUNER_RADIO == f->type) {
			if (!t->radio) {
				set_tv_freq(client, 400 * 16);
				t->radio = 1;
			}
		}
		t->freq = f->frequency;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 13)
		if (V4L2_TUNER_RADIO == f->type) {
                        t->freq /= 1000;
                }
#endif
		set_freq(client, t->freq);
		break;
	}
	case VIDIOC_G_TUNER:
	{
		struct v4l2_tuner *tuner = arg;

		SWITCH_V4L2;
		if (t->radio)
			tuner->signal = tuner_signal(client);
		break;
	}
	default:
		/* nothing */
		break;
	}

	return 0;
}

/* ----------------------------------------------------------------------- */

static struct i2c_driver driver = {
	.owner = THIS_MODULE,
	.name = "i2c TV tuner driver",
	.id = I2C_DRIVERID_TUNER,
	.flags = I2C_DF_NOTIFY,
	.attach_adapter = tuner_probe,
	.detach_client = tuner_detach,
	.command = tuner_command,
};
static struct i2c_client client_template = {
	.name = "(tuner unset)",
	.flags = I2C_CLIENT_ALLOW_USE,
	.driver = &driver,
};

static int tuner_init_module(void)
{
	i2c_add_driver(&driver);
	return 0;
}

static void tuner_cleanup_module(void)
{
	i2c_del_driver(&driver);
}

module_init(tuner_init_module);
module_exit(tuner_cleanup_module);

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
