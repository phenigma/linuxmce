/*
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo dot com>

    Cleanup and VBI and audio in/out options:
    Copyright (C) 2004  Hans Verkuil <hverkuil@xs4all.nl>

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

#include <unistd.h>
#include <features.h>		/* Uses _GNU_SOURCE to define getsubopt in stdlib.h */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <math.h>
#include <linux/types.h>
#include <sys/klog.h>

#define __user
#include <linux/videodev2.h>

/* Internals copied from media/v4l2-common.h */
struct v4l2_routing {
	__u32 input;
	__u32 output;
};
#define	VIDIOC_INT_S_AUDIO_ROUTING	_IOW ('d', 109, struct v4l2_routing)

struct v4l2_register {
	__u32 i2c_id; 		/* I2C driver ID of the I2C chip. 0 for the I2C adapter. */
	unsigned long reg;
	__u32 val;
};
#define	VIDIOC_INT_S_REGISTER 		_IOW ('d', 100, struct v4l2_register)
#define	VIDIOC_INT_G_REGISTER 		_IOWR('d', 101, struct v4l2_register)
#define VIDIOC_INT_RESET            	_IO  ('d', 102)

#define IVTV_INTERNAL
#include "ivtv.h"

#define __stringify_1(x)	#x
#define __stringify(x)		__stringify_1(x)

#include "ivtv-version.h"

/* GPIO */
#define IVTV_REG_GPIO_IN_OFFSET    (0x9008 + 0x02000000)
#define IVTV_REG_GPIO_OUT_OFFSET   (0x900c + 0x02000000)
#define IVTV_REG_GPIO_DIR_OFFSET   (0x9020 + 0x02000000)

/* Options */
enum Option {
	OptSetDebugLevel = 'D',
	OptSetDevice = 'd',
	OptEndGop = 'E',
	OptGetDebugLevel = 'e',
	OptListRegisters = 'R',
	OptSetRegister = 'r',
	OptReloadFW = 'H',
	OptHelp = 'h',
	OptSetGPIO = 'i',
	OptListGPIO = 'I',
	OptPassThrough = 'K',
	OptFrameSync = 'k',
	OptSetAudioRoute = 'v',
	OptGetVbiPassThrough = 'W',
	OptSetVbiPassThrough = 'w',
	OptLast = 128
};

static char options[OptLast];
static int option_reset_ir = 0;
static int option_version = 0;
static int option_setYuvMode = 0;
static int option_getYuvMode = 0;

/* YUV mode specified */
#define YUVMode 		(1L<<0)
#define YUVThreshold		(1L<<1)

static struct option long_options[] = {
	/* Please keep in alphabetical order of the short option.
	   That makes it easier to see which options are still free. */
	{"set-debug", required_argument, 0, OptSetDebugLevel},
	{"device", required_argument, 0, OptSetDevice},
	{"end-gop", required_argument, 0, OptEndGop},
	{"get-debug", no_argument, 0, OptGetDebugLevel},
	{"list-registers", required_argument, 0, OptListRegisters},
	{"set-register", required_argument, 0, OptSetRegister},
	{"reload", no_argument, 0, OptReloadFW},
	{"help", no_argument, 0, OptHelp},
	{"set-gpio", required_argument, 0, OptSetGPIO},
	{"list-gpio", no_argument, 0, OptListGPIO},
	{"passthrough", required_argument, 0, OptPassThrough},
	{"sync", no_argument, 0, OptFrameSync},
	{"audio-route", required_argument, 0, OptSetAudioRoute},
	{"get-vbi-passthrough", no_argument, 0, OptGetVbiPassThrough},
	{"set-vbi-passthrough", required_argument, 0, OptSetVbiPassThrough},
	{"reset-ir", no_argument, &option_reset_ir, 1},
	{"version", no_argument, &option_version, 1},
	{"get-yuv-mode", no_argument, &option_getYuvMode, 1},
	{"set-yuv-mode", required_argument, &option_setYuvMode, 1},
	{0, 0, 0, 0}
};

static void usage(void)
{
	printf("Usage:\n");
	printf("  -d, --device <dev> use device <dev> instead of /dev/video0\n");
	printf("  -h, --help         display this help message\n");
	printf("  -K, --passthrough <mode>\n");
	printf("                     set passthrough mode: 1 = on, 0 = off [IVTV_IOC_PASSTHROUGH]\n");
	printf("  -W, --get-vbi-passthrough\n");
	printf("                     query the current VBI passthrough mode [IVTV_IOC_G_VBI_PASSTHROUGH]\n");
	printf("  -w, --set-vbi-passthrough=<mode>\n");
	printf("                     set the current VBI passthrough mode to <mode> [IVTV_IOC_S_VBI_PASSTHROUGH]\n");
	printf("                     <mode> is a comma separated list of:\n");
	printf("                     none: no passthrough\n");
	printf("                     cc:   closed caption\n");
	printf("                     wss:  widescreen signal\n");
	printf("                     vps:  vps\n");
	printf("  --get-yuv-mode     display the current yuv mode\n");
	printf("  --set-yuv-mode [param],[param]\n");
        printf("                     set the current yuv mode\n");
	printf("     param:\n");
	printf("       mode        =<#> yuv_mode 0=interlaced,1=progressive,2=auto\n");
	printf("       threshold   =<#> if src_hight <= threshold treat as progressive\n");
	printf("  --reset-ir         reset the infrared receiver [VIDIOC_INT_RESET]\n");
	printf("  --version          shows the version number of this utility.\n");
        printf("                     It should match the driver version.\n");
	printf("\n");
	printf("Expert options:\n");
	printf("  -D, --set-debug <level>\n");
	printf("                     set the module ivtv_debug variable\n");
	printf("  -E, --end-gop      capture last GOP [IVTV_IOC_S_GOP_END]\n");
	printf("  -e, --get-debug    query the module ivtv_debug variable\n");
	printf("  -I, --list-gpio\n");
	printf("                     show GPIO input/direction/output bits\n");
	printf("  -i, --set-gpio [dir=<dir>,]val=<val>\n");
	printf("                     set GPIO direction bits to <dir> and set output to <val>\n");
	printf("  -k, --sync         test vsync's capabilities [IVTV_IOC_FRAMESYNC]\n");
	printf("  -R, --list-registers chip=<chip>,min=<addr>,max=<addr>\n");
	printf("                     dump registers from <min> to <max> [VIDIOC_INT_G_REGISTER]\n");
	printf("                     <chip> is one of: saa711x, saa7127, saa717x, cx2584x, cx2341x\n");
	printf("  -r, --set-register chip=<chip>,reg=<reg>,val=<val>\n");
	printf("                     set register [VIDIOC_INT_S_REGISTER]\n");
	printf("                     <chip> is one of: saa711x, saa7127, saa717x, cx2584x, cx2341x\n");
	printf("  -v, --audio-route=input=<in>,output=<out>\n");
	printf("                     set the audio input/output routing [VIDIOC_INT_S_AUDIO_ROUTING]\n");
	exit(0);
}

static char *pts_to_string(char *str, unsigned long pts)
{
	static char buf[256];
	int hours, minutes, seconds, fracsec;
	float fps;
	int frame;
	char *p = (str) ? str : buf;

	static const int MPEG_CLOCK_FREQ = 90000;
	seconds = pts / MPEG_CLOCK_FREQ;
	fracsec = pts % MPEG_CLOCK_FREQ;

	minutes = seconds / 60;
	seconds = seconds % 60;

	hours = minutes / 60;
	minutes = minutes % 60;

	fps = 30;
	frame = (int)ceilf(((float)fracsec / (float)MPEG_CLOCK_FREQ) * fps);

	snprintf(p, sizeof(buf), "%d:%02d:%02d:%d", hours, minutes, seconds,
		 frame);
	return p;
}

static void print_debug_mask(int mask)
{
#define MASK_OR_NOTHING (mask ? " | " : "")
	if (mask & IVTV_DBGFLG_WARN) {
		mask &= ~IVTV_DBGFLG_WARN;
		printf("IVTV_DBGFLG_WARN%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_INFO) {
		mask &= ~IVTV_DBGFLG_INFO;
		printf("IVTV_DBGFLG_INFO%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_API) {
		mask &= ~IVTV_DBGFLG_API;
		printf("IVTV_DBGFLG_API%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_DMA) {
		mask &= ~IVTV_DBGFLG_DMA;
		printf("IVTV_DBGFLG_DMA%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_IOCTL) {
		mask &= ~IVTV_DBGFLG_IOCTL;
		printf("IVTV_DBGFLG_IOCTL%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_I2C) {
		mask &= ~IVTV_DBGFLG_I2C;
		printf("IVTV_DBGFLG_I2C%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_IRQ) {
		mask &= ~IVTV_DBGFLG_IRQ;
		printf("IVTV_DBGFLG_IRQ%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_DEC) {
		mask &= ~IVTV_DBGFLG_DEC;
		printf("IVTV_DBGFLG_DEC%s", MASK_OR_NOTHING);
	}
	if (mask & IVTV_DBGFLG_YUV) {
		mask &= ~IVTV_DBGFLG_YUV;
		printf("IVTV_DBGFLG_YUV%s", MASK_OR_NOTHING);
	}
	if (mask) {
		printf("0x%08x", mask);
	}
	printf("\n");
}

static int dowrite(const char *buf, const char *fn)
{
	FILE *f = fopen(fn, "w");
	if (f == NULL) {
		printf("failed: %s\n", strerror(errno));
		return errno;
	}
	fprintf(f, buf);
	fclose(f);
	return 0;
}

static char *doread(const char *fn)
{
	static char buf[1000];
	FILE *f = fopen(fn, "r");
	int s;

	if (f == NULL) {
		printf("failed: %s\n", strerror(errno));
		return NULL;
	}
	s = fread(buf, 1, sizeof(buf) - 1, f);
	buf[s] = 0;
	fclose(f);
	return buf;
}

static int doioctl(int fd, int request, void *parm, const char *name)
{
	int retVal;

	printf("ioctl %s ", name);
	retVal = ioctl(fd, request, parm);
	if (retVal < 0)
		printf("failed: %s\n", strerror(errno));
	else
		printf("ok\n");
	
	return retVal;
}
/*
static void printregs(int fd, int start, int length)
{
	struct ivtv_ioctl_register saa7115_reg;
        int i;

	printf("\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
        for (i = start; i < start + length; i++) {
                saa7115_reg.reg = i;
                if (ioctl(fd, IVTV_IOC_G_DECODER_REG, &saa7115_reg) < 0)
                        fprintf(stderr, "ioctl: IVTV_IOC_G_DECODER_REG failed\n");
                else {
                        if ((i & 0xf) == 0)
                                printf("\n%04x: ", i);
                        printf("%02x ", saa7115_reg.val);
                }
        }
        printf("\n");
}*/

/* Copied from <linux/i2c-id.h> */
#define I2C_DRIVERID_SAA711X	73	/* saa711x video encoders	*/
#define I2C_DRIVERID_SAA717X	80	/* saa717x video encoder	*/
#define I2C_DRIVERID_SAA7127	72	/* saa7124 video encoder	*/
#define I2C_DRIVERID_CX25840	71	/* cx2584x video encoder	*/

static int parse_chip(const char *chip)
{
	if (!strcmp(chip, "saa711x"))
		return I2C_DRIVERID_SAA711X;
	if (!strcmp(chip, "saa717x"))
		return I2C_DRIVERID_SAA717X;
	if (!strcmp(chip, "saa7127"))
		return I2C_DRIVERID_SAA7127;
	if (!strcmp(chip, "cx2584x"))
		return I2C_DRIVERID_CX25840;
	if (!strcmp(chip, "cx2341x"))
		return 0;
	return -1;
}

int main(int argc, char **argv)
{
	char *value, *subs;
	int i;
	char *subopts[] = {
#define SUB_REG				0
		"reg",
#define SUB_VAL				1
		"val",
#define SUB_MAX				2
		"max",
#define SUB_MIN				3
		"min",
#define SUB_YUV_MODE			4
		"mode",
#define SUB_YUV_THRESHOLD		5
		"threshold",
#define SUB_DIR				6
		"dir",
#define SUB_INPUT                       7
		"input",
#define SUB_OUTPUT                      8
		"output",
#define SUB_CHIP			9
		"chip",

		NULL
	};

	int fd = -1;

	/* bitfield for OptSetCodec */
	unsigned int set_yuvMode = 0;

	/* command args */
	char ch, *device = strdup("/dev/video0");	/* -d device */
	struct ivtv_ioctl_yuv_interlace yuv_mode = { 0, 0 };
	struct v4l2_register reg;
	unsigned long min_reg = 0x00000000;	/* max registers to list */
	unsigned long max_reg = 0x0200ffff;	/* max registers to list */
	struct v4l2_routing route;	/* audio_route */
	unsigned short gpio_out = 0x0;	/* GPIO output data */
	unsigned short gpio_dir = 0x0;	/* GPIO direction bits */
	int gpio_set_dir = 0;
	int passthrough = 0;
	int vbi_passthrough = 0;
	int debug_level = 0;
	struct ivtv_ioctl_framesync frameinfo;
	int new_debug_level, gdebug_level, end_gop = 0;
	double timestamp;
	char ptsstr[64];
	char scrstr[64];
	char short_options[26 * 2 * 2 + 1];

	if (argc == 1) {
		usage();
		return 0;
	}
	while (1) {
		int option_index = 0;
		int idx = 0;

		for (i = 0; long_options[i].name; i++) {
			if (!isalpha(long_options[i].val))
				continue;
			short_options[idx++] = long_options[i].val;
			if (long_options[i].has_arg == required_argument)
				short_options[idx++] = ':';
		}
		short_options[idx] = 0;
		ch = getopt_long(argc, argv, short_options,
				 long_options, &option_index);
		if (ch == -1)
			break;

		options[(int)ch] = 1;
		switch (ch) {
                case 0:
                    if (option_setYuvMode == 1)
                    {
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_YUV_MODE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <mode>\n");
						usage();
						return 1;

					}
					yuv_mode.interlace_mode = strtol(value, 0L, 0);
					set_yuvMode |= YUVMode;
					break;
				case SUB_YUV_THRESHOLD:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <threshold>\n");
						usage();
						exit(1);
					}
					yuv_mode.threshold = strtol(value, 0L, 0);
					set_yuvMode |= YUVThreshold;
					break;
                                }
                        }
                    }
                    break;
		case OptHelp:
			usage();
			return 0;
		case OptEndGop:{
			end_gop = strtol(optarg, 0L, 0);
			break;
		}
		case OptSetDebugLevel:{
			debug_level = strtol(optarg, 0L, 0);
			break;
		}
		case OptSetDevice:
			device = strdup(optarg);
                        if (device[0] >= '0' && device[0] <= '9' && device[1] == 0) {
                                char dev = device[0];

                                sprintf(device, "/dev/video%c", dev);
                        }
			break;

		case OptSetVbiPassThrough:
			if (strstr(optarg, "cc"))
				vbi_passthrough |= V4L2_SLICED_CAPTION_525;
			if (strstr(optarg, "wss"))
				vbi_passthrough |= V4L2_SLICED_WSS_625;
			if (strstr(optarg, "vps"))
				vbi_passthrough |= V4L2_SLICED_VPS;
			if (strstr(optarg, "none"))
				vbi_passthrough = 0;
			break;

		case OptSetRegister:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_CHIP:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <chip>\n");
						usage();
						exit(1);
					}
					reg.i2c_id = parse_chip(value);
					if (reg.i2c_id < 0) {
						fprintf(stderr,
							"Unknown chip specification\n");
						usage();
						exit(1);
					}
					break;
				case SUB_REG:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <reg>\n");
						usage();
						exit(1);
					}
					reg.reg =
					    (unsigned long)strtoul(value, 0UL,
								   0);
					break;
				case SUB_VAL:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <val>\n");
						usage();
						exit(1);
					}
					reg.val =
					    (unsigned long)strtoul(value, 0UL,
								   0);
					break;
				default:
					fprintf(stderr,
						"Invalid suboptions specified\n");
					usage();
					exit(1);
					break;
				}
			}
			break;
		case OptListRegisters:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_CHIP:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <chip>\n");
						usage();
						exit(1);
					}
					reg.i2c_id = parse_chip(value);
					if (reg.i2c_id < 0) {
						fprintf(stderr,
							"Unknown chip specification\n");
						usage();
						exit(1);
					}
					break;
				case SUB_MIN:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <reg>\n");
						usage();
						exit(1);
					}
					min_reg = (unsigned long)
					    strtol(value, 0L, 0);
					break;
				case SUB_MAX:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <reg>\n");
						usage();
						exit(1);
					}
					max_reg = (unsigned long)
					    strtol(value, 0L, 0);
					break;
				default:
					break;
				}
			}
			break;
		case OptSetAudioRoute:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_INPUT:
					if (value == NULL) {
						printf
						    ("No value given to suboption <input>\n");
						usage();
					}
					route.input = strtol(value, 0L, 0);
					break;
				case SUB_OUTPUT:
					if (value == NULL) {
						printf
						    ("No value given to suboption <output>\n");
						usage();
					}
					route.output = strtol(value, 0L, 0);
					break;
				default:
					printf
					    ("Invalid suboptions specified\n");
					usage();
					break;
				}
			}
			break;
		case OptPassThrough:
			passthrough = strtol(optarg, 0L, 0);
			break;
		case OptSetGPIO:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_DIR:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <dir>\n");
						usage();
						exit(1);
					}
					gpio_dir = strtol(value, 0L, 0);
					gpio_set_dir = 1;
					break;
				case SUB_VAL:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <val>\n");
						usage();
						exit(1);
					}
					gpio_out =
					    (unsigned short)strtol(value, 0L, 0);
					break;
				default:
					fprintf(stderr,
						"Invalid suboptions specified\n");
					usage();
					exit(1);
					break;
				}
			}
			break;
		case ':':
			fprintf(stderr, "Option `%s' requires a value\n",
				argv[optind]);
			usage();
			return 1;
		case '?':
			fprintf(stderr, "Unknown argument `%s'\n",
				argv[optind]);
			usage();
			return 1;
		}
	}
	if (optind < argc) {
		printf("unknown arguments: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
		usage();
		return 1;
	}

	if ((fd = open(device, O_RDWR)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", device,
			strerror(errno));
		exit(1);
	}
	free(device);

	/* Setting Opts */

	if (options[OptSetVbiPassThrough]) {
		if (doioctl(fd, IVTV_IOC_S_VBI_PASSTHROUGH, &vbi_passthrough, 
			"IVTV_IOC_S_VBI_PASSTHROUGH") == 0)
			printf("VBI passthrough mode set to %08x\n",
			       vbi_passthrough);
	}

	if (options[OptGetVbiPassThrough]) {
		if (doioctl(fd, IVTV_IOC_G_VBI_PASSTHROUGH, &vbi_passthrough,
			"IVTV_IOC_G_VBI_PASSTHROUGH") == 0)
			printf("VBI passthrough mode is %08x\n",
			       vbi_passthrough);
	}

	if (options[OptEndGop]) {
		if (doioctl(fd, IVTV_IOC_S_GOP_END, &end_gop,
			"IVTV_IOC_S_GOP_END") == 0)
			printf("End GOP capture set to %d\n", end_gop);
	}

	if (options[OptSetRegister]) {
		if (doioctl(fd, VIDIOC_INT_S_REGISTER, &reg,
			"VIDIOC_INT_S_REGISTER") == 0)
			printf("register 0x%08lx set to 0x%08x\n",
		       		reg.reg, reg.val);
	}

	if (options[OptSetAudioRoute]) {
		doioctl(fd, VIDIOC_INT_S_AUDIO_ROUTING, &route, "VIDIOC_INT_S_AUDIO_ROUTING");
	}

	if (options[OptListRegisters]) {
		int stride = 1;

		if (reg.i2c_id == 0) stride = 4;
		printf("ioctl: VIDIOC_INT_G_REGISTER\n");
		if (stride == 4)
			printf("                00       04       08       0C       10       14       18       1C");
		else
			printf("          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
		for (i = 0; i <= max_reg - min_reg; i += stride) {
			reg.reg = min_reg + (unsigned long)i;
			if (ioctl(fd, VIDIOC_INT_G_REGISTER, &reg) < 0) {
				fprintf(stderr,
					"ioctl: "
					"VIDIOC_INT_G_REGISTER "
					"failed for 0x%08lx\n", reg.reg);
			} else {
				/*if ((i&0xf)==0) 
				   printf("\n%08x: ",i); */
				if (stride > 1 && (i & 0x1f) == 0)
					printf("\n%08x: ", i);
				else if (stride == 1 && (i & 0x0f) == 0)
					printf("\n%08x: ", i);
				printf("%0*x ", 2 * stride, reg.val);
			}
			usleep(1);
		}
		printf("\n");
	}

	if (options[OptFrameSync]) {
		printf("ioctl: IVTV_IOC_FRAMESYNC\n");

		for (;;) {
			if (ioctl(fd, IVTV_IOC_FRAMESYNC, &frameinfo) < 0) {
				fprintf(stderr,
					"ioctl: IVTV_IOC_FRAMESYNC failed\n");
				break;
			} else {
				struct timeval tv;
				gettimeofday(&tv, NULL);
				timestamp =
				    (double)tv.tv_sec +
				    ((double)tv.tv_usec / 1000000.0);

				pts_to_string(ptsstr, frameinfo.pts);
				pts_to_string(scrstr, frameinfo.scr);
				printf
				    ("%10.6f: pts %-20s, scr %-20s, %d frames\n",
				     timestamp, ptsstr, scrstr,
				     frameinfo.frame);
			}
		}
	}

	if (options[OptSetGPIO]) {
		struct v4l2_register reg;

		reg.i2c_id = 0;
		reg.reg = IVTV_REG_GPIO_DIR_OFFSET;
		reg.val = gpio_dir;
		if (gpio_set_dir && doioctl(fd, VIDIOC_INT_S_REGISTER, &reg,
			"VIDIOC_INT_S_REGISTER") == 0)
			printf("GPIO dir set to 0x%04x\n", reg.val);
		reg.reg = IVTV_REG_GPIO_OUT_OFFSET;
		reg.val = gpio_out;
		if (doioctl(fd, VIDIOC_INT_S_REGISTER, &reg,
			"VIDIOC_INT_S_REGISTER") == 0)
			printf("GPIO out set to 0x%04x\n", reg.val);
	}

	if (options[OptListGPIO]) {
		struct v4l2_register reg;

		reg.i2c_id = 0;
		reg.reg = IVTV_REG_GPIO_IN_OFFSET;
		if (ioctl(fd, VIDIOC_INT_G_REGISTER, &reg) == 0) 
			printf("GPIO in:  0x%04x\n", reg.val);
		reg.reg = IVTV_REG_GPIO_DIR_OFFSET;
		if (ioctl(fd, VIDIOC_INT_G_REGISTER, &reg) == 0) 
			printf("GPIO dir: 0x%04x\n", reg.val);
		reg.reg = IVTV_REG_GPIO_OUT_OFFSET;
		if (ioctl(fd, VIDIOC_INT_G_REGISTER, &reg) == 0) 
			printf("GPIO out: 0x%04x\n", reg.val);
	}

	if (options[OptSetDebugLevel]) {
		char buf[20];
		new_debug_level = debug_level;

		sprintf(buf, "%d", debug_level);
		if (dowrite(buf, "/sys/module/ivtv/parameters/ivtv_debug") == 0) {
			printf(" set debug level: ");
			print_debug_mask(new_debug_level);
			printf("\n");
		}
	}

	if (options[OptGetDebugLevel]) {
		char *buf;

		gdebug_level = 0;
		if ((buf = doread("/sys/module/ivtv/parameters/ivtv_debug"))) {
			gdebug_level = atol(buf);
			printf(" debug level: ");
			print_debug_mask(gdebug_level);
			printf("\n");
		}
	}

	if (options[OptPassThrough]) {
		doioctl(fd, IVTV_IOC_PASSTHROUGH, &passthrough, "IVTV_IOC_PASSTHROUGH");
	}

	if (option_reset_ir) {
		doioctl(fd, VIDIOC_INT_RESET, NULL, "VIDIOC_INT_RESET");
	}

	if (option_version) {
                printf("ivtvctl version " IVTV_VERSION "\n");
	}

        if (option_setYuvMode)
        {
            printf("set yuv mode\n");
            struct ivtv_ioctl_yuv_interlace interlace_mode;
            doioctl(fd, IVTV_IOC_G_YUV_INTERLACE, &interlace_mode, "IVTV_IOC_G_YUV_INTERLACE");
            if (set_yuvMode & YUVMode)
                interlace_mode.interlace_mode = yuv_mode.interlace_mode;
            if (set_yuvMode & YUVThreshold)
                interlace_mode.threshold = yuv_mode.threshold;
            doioctl(fd, IVTV_IOC_S_YUV_INTERLACE, &interlace_mode, "IVTV_IOC_S_YUV_INTERLACE");

        }

        if (option_getYuvMode)
        {
            struct ivtv_ioctl_yuv_interlace interlace_mode;
            doioctl(fd, IVTV_IOC_G_YUV_INTERLACE, &interlace_mode, "IVTV_IOC_G_YUV_INTERLACE");
            printf("Current yuv_mode %d ",interlace_mode.interlace_mode);
            switch(interlace_mode.interlace_mode)
            {
            case IVTV_YUV_MODE_INTERLACED:
                printf("interlaced\n");
                break;
            case IVTV_YUV_MODE_PROGRESSIVE:
                printf("progressive\n");
                break;
            case IVTV_YUV_MODE_AUTO:
                printf("auto\n");
                break;
            default:
                printf("undefined\n");
                break;
            }
            printf("\tauto switch threshold %d\n",interlace_mode.threshold);
        }

	close(fd);
	exit(0);
}
