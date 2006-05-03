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
#include <videodev2.h>

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
	OptListAudioInputs = 'A',
	OptAll = 'a',
	OptGetVbi = 'B',
	OptSetVbi = 'b',
	OptListCodec = 'C',
	OptSetCodec = 'c',
	OptSetDebugLevel = 'D',
	OptSetDevice = 'd',
	OptEndGop = 'E',
	OptGetDebugLevel = 'e',
	OptGetFormat = 'F',
	OptSetFormat = 'f',
	OptListDecRegisters = 'G',
	OptSetDecRegister = 'g',
	OptReloadFW = 'H',
	OptHelp = 'h',
	OptSetGPIO = 'i',
	OptListGPIO = 'I',
	OptListEncRegisters = 'J',
	OptSetEncRegister = 'j',
	OptPassThrough = 'K',
	OptFrameSync = 'k',
	OptGetOutput = 'L',
	OptSetOutput = 'l',
	OptListCapability = 'm',
	OptListOutputs = 'N',
	OptListInputs = 'n',
	OptListITVCRegisters = 'O',
	OptSetITVCRegister = 'o',
	OptGetInput = 'P',
	OptSetInput = 'p',
	OptGetAudioInput = 'Q',
	OptSetAudioInput = 'q',
	OptGetFreq = 'R',
	OptSetFreq = 'r',
	OptStreamOff = 'S',
	OptListStandards = 's',
	OptGetStandard = 'U',
	OptSetStandard = 'u',
	OptGetDriverInfo = 'V',
	OptSetIO = 'v',
	OptGetVbiPassThrough = 'W',
	OptSetVbiPassThrough = 'w',
	OptGetVbiEmbed = 'X',
	OptSetVbiEmbed = 'x',
	OptListCtrls = 'Y',
	OptSetCtrl = 'y',
	OptGetSAPMode = 'Z',
	OptSetSAPMode = 'z',
	OptLast = 128
};

static char options[OptLast];
static int option_reset_ir = 0;
static int option_version = 0;
static int option_setYuvMode = 0;
static int option_getYuvMode = 0;
static int option_log_status = 0;

/* Codec's specified */
#define CAspect			(1L<<1)
#define CBitrateMode		(1L<<2)
#define CBitrate		(1L<<3)
#define CBitratePeak		(1L<<4)
#define CStreamType		(1L<<5)
#define CFreamerate		(1L<<6)
#define CFramesPerGOP		(1L<<7)
#define CBFrames		(1L<<8)
#define CGOPClosure		(1L<<9)
#define CAudio			(1L<<10)
#define CDNRMode		(1L<<11)
#define CDNRType		(1L<<12)
#define CDNRSpatial		(1L<<13)
#define CDNRTemporal		(1L<<14)
#define CPullDown		(1L<<15)

/* YUV mode specified */
#define YUVMode 		(1L<<0)
#define YUVThreshold		(1L<<1)

/* fmts specified */
#define FMTWidth		(1L<<0)
#define FMTHeight		(1L<<1)

static struct option long_options[] = {
	/* Please keep in alphabetical order of the short option.
	   That makes it easier to see which options are still free. */
	{"list-audio-inputs", no_argument, 0, OptListAudioInputs},
	{"all", no_argument, 0, OptAll},
	{"get-vbi", no_argument, 0, OptGetVbi},
	{"set-vbi", required_argument, 0, OptSetVbi},
	{"list-codec-params", no_argument, 0, OptListCodec},
	{"set-codec-params", required_argument, 0, OptSetCodec},
	{"set-debug", required_argument, 0, OptSetDebugLevel},
	{"device", required_argument, 0, OptSetDevice},
	{"end-gop", required_argument, 0, OptEndGop},
	{"get-debug", no_argument, 0, OptGetDebugLevel},
	{"get-format", no_argument, 0, OptGetFormat},
	{"set-format", required_argument, 0, OptSetFormat},
	{"list-dec-registers", no_argument, 0, OptListDecRegisters},
	{"set-dec-registers", required_argument, 0, OptSetDecRegister},
	{"list-itvc-registers", required_argument, 0, OptListITVCRegisters},
	{"set-itvc-registers", required_argument, 0, OptSetITVCRegister},
	{"reload", no_argument, 0, OptReloadFW},
	{"help", no_argument, 0, OptHelp},
	{"set-gpio", required_argument, 0, OptSetGPIO},
	{"list-gpio", no_argument, 0, OptListGPIO},
	{"list-enc-registers", no_argument, 0, OptListEncRegisters},
	{"set-enc-registers", required_argument, 0, OptSetEncRegister},
	{"passthrough", required_argument, 0, OptPassThrough},
	{"sync", no_argument, 0, OptFrameSync},
	{"get-output", no_argument, 0, OptGetOutput},
	{"set-output", required_argument, 0, OptSetOutput},
	{"list-capability", no_argument, 0, OptListCapability},
	{"list-outputs", no_argument, 0, OptListOutputs},
	{"list-inputs", no_argument, 0, OptListInputs},
	{"get-input", no_argument, 0, OptGetInput},
	{"set-input", required_argument, 0, OptSetInput},
	{"get-audio-input", no_argument, 0, OptGetAudioInput},
	{"set-audio-input", required_argument, 0, OptSetAudioInput},
	{"get-freq", no_argument, 0, OptGetFreq},
	{"set-freq", required_argument, 0, OptSetFreq},
	{"streamoff", no_argument, 0, OptStreamOff},
	{"list-standards", no_argument, 0, OptListStandards},
	{"get-standard", no_argument, 0, OptGetStandard},
	{"set-standard", required_argument, 0, OptSetStandard},
	{"info", no_argument, 0, OptGetDriverInfo},
	{"set-io", required_argument, 0, OptSetIO},
	{"get-vbi-passthrough", no_argument, 0, OptGetVbiPassThrough},
	{"set-vbi-passthrough", required_argument, 0, OptSetVbiPassThrough},
	{"get-vbi-embed", no_argument, 0, OptGetVbiEmbed},
	{"set-vbi-embed", required_argument, 0, OptSetVbiEmbed},
	{"list-ctrls", no_argument, 0, OptListCtrls},
	{"set-ctrl", required_argument, 0, OptSetCtrl},
	{"get-sapmode", no_argument, 0, OptGetSAPMode},
	{"set-sapmode", required_argument, 0, OptSetSAPMode},
	{"reset-ir", no_argument, &option_reset_ir, 1},
	{"version", no_argument, &option_version, 1},
	{"get-yuv-mode", no_argument, &option_getYuvMode, 1},
	{"set-yuv-mode", required_argument, &option_setYuvMode, 1},
	{"log-status", no_argument, &option_log_status, 1},
	{0, 0, 0, 0}
};

void usage(void)
{
	printf("Usage:\n");
	printf("  -A, --list-audio-inputs\n");
	printf("                     display audio inputs [VIDIOC_ENUMAUDIO]\n");
	printf("  -a, --all          display all information available\n");
	printf("  -B, --get-vbi      query the current VBI mode [IVTV_G_VBI_MODE]\n");
	printf("  -b, --set-vbi=<mode>\n");
	printf("                     set the current VBI mode to <mode> [IVTV_S_VBI_MODE]\n");
	printf("                     <mode> is a comma separated list of:\n");
	printf("                     raw:      raw VBI mode\n");
	printf("                     teletext: sliced teletext\n");
	printf("                     cc:       sliced closed caption\n");
	printf("                     wss:      sliced widescreen signal\n");
	printf("                     vps:      sliced vps\n");
	printf("  -C, --list-codec-params\n");
	printf("                     display the current codec parameters [IVTV_IOC_G_CODEC]\n");
	printf("  -c, --set-codec-params=[param],[param],...\n");
	printf("                     set the current codec parameters [IVTV_IOC_S_CODEC]\n");
	printf("     param:\n");
	printf("       aspect        =<#> aspect ratio (try 3)\n");
	printf("       audio         =<#> sets audio bitmask (try 0xE9)\n");
	printf("       bframes       =<#> number of B frames\n");
	printf("       bitrate_mode  =<#> bitrate mode (0 = VBR, 1 = CBR)\n");
	printf("       bitrate       =<#> bitrate (DVD max = 9.6Mbps)\n");
	printf("       bitrate_peak  =<#> peak bitrate (must be greater than bitrate)\n");
	printf("       dnr_mode      =<#> see API documentation\n");
	printf("       dnr_spatial   =<#> see API documentation\n");
	printf("       dnr_temporal  =<#> see API documentation\n");
	printf("       dnr_type      =<#> see API documentation\n");
	printf("       framerate     =<#> try 0 (30fps) or 1 (25fps)\n");
	printf("       framespergop  =<#> GOP size\n");
	printf("       gop_closure   =<#> open/closed gop\n");
	printf("       pulldown      =<#> inverse telecine on (1) / off (0)\n");
	printf("       stream_type   =<#> see ivtv.h IVTV_STREAM_* types (try 10 for DVD, or 0 for PS)\n");
	printf("  -d, --device=<dev> use device <dev> instead of /dev/video0\n");
	printf("  -F, --get-format   query the current data format [VIDIOC_G_FMT]\n");
	printf("  -f, --set-format=width=<x>,height=<y>\n");
	printf("                     set the current data format [VIDIOC_S_FMT]\n");
	printf("  -h, --help         display this help message\n");
	printf("  -K, --passthrough=<mode>\n");
	printf("                     set passthrough mode: 1 = on, 0 = off [IVTV_IOC_PASSTHROUGH]\n");
	printf("  -L, --get-output   query the current video output [VIDIOC_G_OUTPUT]\n");
	printf("  -l, --set-output=<num>\n");
	printf("                     set the current video output to <num> [VIDIOC_S_OUTPUT]\n");
	printf("  -m, --list-capability\n");
	printf("                     query device capabilities [VIDIOC_QUERYCAP]\n");
	printf("  -N, --list-outputs display video outputs [VIDIOC_ENUMOUTPUT]\n");
	printf("  -n, --list-inputs  display video inputs [VIDIOC_ENUMINPUT]\n");
	printf("  -P, --get-input    query the current video input [VIDIOC_G_INPUT]\n");
	printf("  -p, --set-input=<num>\n");
	printf("                     set the current video input to <num> [VIDIOC_S_INPUT]\n");
	printf("  -Q, --get-audio-input\n");
	printf("                     query the current audio input [VIDIOC_G_AUDIO]\n");
	printf("  -q, --set-audio-input=<num>\n");
	printf("                     set the current audio input to <num> [VIDIOC_S_AUDIO]\n");
	printf("  -R, --get-freq     query the current frequency [VIDIOC_G_FREQUENCY]\n");
	printf("  -r, --set-freq=<freq>\n");
	printf("                     set the current frequency to <freq> [VIDIOC_S_FREQUENCY]\n");
	printf("  -s, --list-standards\n");
	printf("                     display supported video standards [VIDIOC_ENUMSTD]\n");
	printf("  -U, --get-standard\n");
	printf("                     query the current video standard [VIDIOC_G_STD]\n");
	printf("  -u, --set-standard=<num>\n");
	printf("                     set the current video standard to <num> [VIDIOC_S_STD]\n");
	printf("  -W, --get-vbi-passthrough\n");
	printf("                     query the current VBI passthrough mode [IVTV_IOC_G_VBI_PASSTHROUGH]\n");
	printf("  -w, --set-vbi-passthrough=<mode>\n");
	printf("                     set the current VBI passthrough mode to <mode> [IVTV_IOC_S_VBI_PASSTHROUGH]\n");
	printf("                     <mode> is a comma separated list of:\n");
	printf("                     none: no passthrough\n");
	printf("                     cc:   closed caption\n");
	printf("                     wss:  widescreen signal\n");
	printf("                     vps:  vps\n");
	printf("  -X, --get-vbi-embed\n");
	printf("                     query whether VBI data is embedded in the MPEG stream [IVTV_IOC_G_VBI_EMBED]\n");
	printf("  -x, --set-vbi-embed=<mode>\n");
	printf("                     set whether VBI data is embedded in the MPEG stream [IVTV_IOC_S_VBI_EMBED]\n");
	printf("                     If 1, embed VBI data in the MPEG stream, if 0, don't.\n");
	printf("  -Y, --list-ctrls   display all controls and their values [VIDIOC_QUERYCTRL]\n");
	printf("  -y, --set-ctrl=[ctrl]=<val>\n");
	printf("                     set the control to the value specified [VIDIOC_S_CTRL]\n");
	printf("     ctrl:\n");
	printf("       brightness    =<#> Picture brightness, or more precisely, the black level. [0 - 255]\n");
	printf("       hue           =<#> Hue or color balance. [-128 - 127]\n");
	printf("       saturation    =<#> Picture color saturation or chroma gain. [0 - 127]\n");
	printf("       contrast      =<#> Picture contrast or luma gain. [0 - 127]\n");
	printf("       volume        =<#> Overall audio volume. [0 - 65535]\n");
	printf("       mute          =<#> Mute audio, i.e. set the volume to zero [boolean]\n");
	printf("       balance       =<#> Audio balance. [0 - 65535]\n");
	printf("       bass          =<#> Audio bass. [0 - 65535]\n");
	printf("       treble        =<#> Audio treble. [0 - 65535]\n");
	printf("       loudness      =<#> Audio loudness. [0 - 1]\n");
	printf("  -Z, --get-sapmode  query the current Secondary Audio Program [VIDIOC_G_TUNER]\n");
	printf("  -z, --set-sapmode=<mode>\n");
	printf("                     set the current Secondary Audio Program to <mode> [VIDIOC_S_TUNER]\n");
	printf("                     Possible values: 0 (mono), 1 (stereo), 2 (lang2), 3 (lang1)\n");
	printf("  --get-yuv-mode     display the current yuv mode\n");
	printf("  --set-yuv-mode=[param],[param]\n");
        printf("                     set the current yuv mode\n");
	printf("     param:\n");
	printf("       mode        =<#> yuv_mode 0=interlaced,1=progressive,2=auto\n");
	printf("       threshold   =<#> if src_hight <= threshold treat as progressive\n");
	printf("  --reset-ir         reset the infrared receiver [IVTV_IOC_RESET_IR]\n");
	printf("  --version          shows the version number of this utility.\n");
        printf("                     It should match the driver version.\n");
	printf("\n");
	printf("Expert options:\n");
	printf("  -D, --set-debug=level\n");
	printf("                     set the module ivtv_debug variable [IVTV_IOC_S_DEBUG_LEVEL]\n");
	printf("  -E, --end-gop      capture last GOP [IVTV_IOC_S_GOP_END]\n");
	printf("  -e, --get-debug    query the module ivtv_debug variable [IVTV_IOC_G_DEBUG_LEVEL]\n");
	printf("  -G, --list-dec-registers\n");
	printf("                     dump decoder registers [DECODER_GET_REG]\n");
	printf("  -g, --set-dec-register=reg=<reg>,val=<val>\n");
	printf("                     set decoder registers [DECODER_SET_REG]\n");
	printf("  -H, --reload       forces reload on cards firmware [IVTV_IOC_RELOAD_FW]\n");
	printf("  -I, --list-gpio\n");
	printf("                     show GPIO input/direction/output bits\n");
	printf("  -i, --set-gpio=[dir=<dir>,]val=<val>\n");
	printf("                     set GPIO direction bits to <dir> and set output to <val>\n");
	printf("  -J, --list-enc-registers\n");
	printf("                     dump encoder registers [ENCODER_GET_REG]\n");
	printf("  -j, --set-enc-register=reg=<reg>,val=<val>\n");
	printf("                     set encoder registers [ENCODER_SET_REG]\n");
	printf("  -k, --sync         test vsync's capabilities [IVTV_IOC_FRAMESYNC]\n");
	printf("  -O, --list-itvc-registers=min=<val>,max=<val>\n");
	printf("                     dump ITVC registers [ITVC_GET_REG]\n");
	printf("  -o, --set-itvc-register=reg=<reg>,val=<val>\n");
	printf("                     set ITVC registers [ITVC_SET_REG]\n");
	printf("  -S, --streamoff    send StreamOff API [VIDIOC_STREAMOFF]\n");
	printf("  -V, --info         gets driver information (version) [IVTV_IOC_G_DRIVER_INFO]\n");
	printf("  -v, --set-io=input=<in>,output=<out>\n");
	printf("                     set the MSP34xx input/output mapping [MSP_SET_MATRIX]\n");
	printf("  --log-status       log the board status in the kernel log\n");
	exit(0);
}

int printfmt(struct v4l2_format vfmt)
{
	switch (vfmt.type) {
	case 1:
		printf("\tType   : Video Capture\n");
		printf("\tWidth  : %d\n", vfmt.fmt.pix.width);
		printf("\tHeight : %d\n", vfmt.fmt.pix.height);
		break;
	case 2:
		printf("\tType   : Video Output\n");
		break;
	case 3:
		printf("\tType   : Video Overlay\n");
		break;
	case 4:
		printf("\tType   : VBI Capture\n");
		break;
	case 5:
		printf("\tType   : VBI Output\n");
		break;
	case 0x80:
		printf("\tType   : Private\n");
		break;
	default:
		printf("\tType   : Unknown: %d\n", vfmt.type);
		return -1;
		break;
	}
	return 0;
}

char *pts_to_string(char *str, unsigned long pts)
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
}

int main(int argc, char **argv)
{
	char *value, *subs;
	int i;
	char *subopts[] = {
#define SUB_ASPECT			0
		"aspect",
#define SUB_AUDIO			1
		"audio",
#define SUB_BFRAMES			2
		"bframes",
#define SUB_BITRATE_MODE		3
		"bitrate_mode",
#define SUB_BITRATE			4
		"bitrate",
#define SUB_BITRATE_PEAK		5
		"bitrate_peak",
#define SUB_DNR_MODE			6
		"dnr_mode",
#define SUB_DNR_SPATIAL			7
		"dnr_spatial",
#define SUB_DNR_TEMPORAL		8
		"dnr_temporal",
#define SUB_DNR_TYPE			9
		"dnr_type",
#define SUB_FRAMERATE			10
		"framerate",
#define SUB_FRAMESPERGOP		11
		"framespergop",
#define SUB_FREQ			12
		"freq",
#define SUB_GOP_CLOSURE			13
		"gop_closure",
#define SUB_HEIGHT			14
		"height",
#define SUB_INPUT			15
		"input",
#define SUB_OUTPUT			16
		"output",
#define SUB_PULLDOWN			17
		"pulldown",
#define SUB_STREAM_TYPE			18
		"stream_type",
#define SUB_TUNER			19
		"tuner",
#define SUB_WIDTH			20
		"width",
#define SUB_REG				21
		"reg",
#define SUB_VAL				22
		"val",
#define SUB_BRIGHTNESS			23
		"brightness",
#define SUB_CONTRAST			24
		"contrast",
#define SUB_SATURATION			25
		"saturation",
#define SUB_HUE				26
		"hue",
#define SUB_VOLUME			27
		"volume",
#define SUB_MUTE			28
		"mute",
#define SUB_AUDIO_MODE			29
		"audio_mode",
#define SUB_MAX				30
		"max",
#define SUB_MIN				31
		"min",
#define SUB_YUV_MODE			32
		"mode",
#define SUB_YUV_THRESHOLD		33
		"threshold",
#define SUB_BALANCE			34
		"balance",
#define SUB_BASS			35
		"bass",
#define SUB_TREBLE			36
		"treble",
#define SUB_LOUDNESS			37
		"loudness",
#define SUB_DIR				38
		"dir",
		NULL
	};

	int fd = -1;

	/* bitfield for OptSetCodec */
	unsigned int set_codecs = 0;
	unsigned int set_yuvMode = 0;
	/* bitfield for fmts */
	unsigned int set_fmts = 0;

	int mode = V4L2_TUNER_MODE_STEREO;	/* set audio mode */

	/* command args */
	char ch, *device = strdup("/dev/video0");	/* -d device */
        struct ivtv_driver_info info;
	struct ivtv_ioctl_codec codec;	/* set_codec/list_codec */
	struct ivtv_ioctl_yuv_interlace yuv_mode = { 0, 0 };
	struct v4l2_format vfmt;	/* set_format/get_format */
	struct v4l2_format vbi_fmt;	/* set_format/get_format for sliced VBI */
	struct v4l2_control ctrl;	/* set_ctrl/get_ctrls */
	struct ivtv_ioctl_register saa7115_reg;
	struct ivtv_ioctl_register itvc_reg;
	struct v4l2_capability vcap;	/* list_cap */
	struct v4l2_input vin;	/* list_inputs */
	struct v4l2_output vout;	/* list_outputs */
	struct v4l2_audio vaudio;	/* list audio inputs */
	unsigned long min_reg = 0x00000000;	/* max registers to list */
	unsigned long max_reg = 0x0200ffff;	/* max registers to list */
	int input;		/* set_input/get_input */
	int output;		/* set_output/get_output */
	v4l2_std_id std;	/* get_std/set_std */
	struct v4l2_frequency vf;	/* get_freq/set_freq */
	struct v4l2_standard vs;	/* list_std */
	struct ivtv_msp_matrix mspm;	/* set_io */
	unsigned short gpio_out = 0x0;	/* GPIO output data */
	unsigned short gpio_dir = 0x0;	/* GPIO direction bits */
	int gpio_set_dir = 0;
	int passthrough = 0;
	int vbi_passthrough = 0;
	int debug_level = 0;
	struct ivtv_ioctl_framesync frameinfo;
	int vbi_insert_mpeg;
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
		case OptAll:
			options[OptListCodec] = 1;
			options[OptGetFormat] = 1;
			options[OptListCtrls] = 1;
			options[OptListCapability] = 1;
			options[OptListInputs] = 1;
			options[OptGetInput] = 1;
			options[OptListOutputs] = 1;
			options[OptGetOutput] = 1;
			options[OptListAudioInputs] = 1;
			options[OptGetAudioInput] = 1;
			options[OptListStandards] = 1;
			options[OptGetStandard] = 1;
			options[OptGetFreq] = 1;
			vf.tuner = 0;
			break;

		case OptSetVbi:
			vbi_fmt.fmt.sliced.service_set = 0;
			if (!strstr(optarg, "raw")) {
				if (strstr(optarg, "teletext"))
					vbi_fmt.fmt.sliced.service_set |=
					    V4L2_SLICED_TELETEXT_B;
				if (strstr(optarg, "cc"))
					vbi_fmt.fmt.sliced.service_set |=
					    V4L2_SLICED_CAPTION_525;
				if (strstr(optarg, "wss"))
					vbi_fmt.fmt.sliced.service_set |=
					    V4L2_SLICED_WSS_625;
				if (strstr(optarg, "vps"))
					vbi_fmt.fmt.sliced.service_set |=
                                             V4L2_SLICED_VPS;
			}
			break;

		case OptSetVbiEmbed:
			vbi_insert_mpeg = atol(optarg);
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

		case OptSetCodec:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_ASPECT:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <aspect>\n");
						usage();
						return 1;
					}
					codec.aspect = strtol(value, 0L, 0);
					set_codecs |= CAspect;
					break;
				case SUB_BITRATE_MODE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <bitrate_mode>\n");
						usage();
						exit(1);
					}
					codec.bitrate_mode =
					    strtol(value, 0L, 0);
					set_codecs |= CBitrateMode;
					break;
				case SUB_BITRATE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <bitrate>\n");
						usage();
						return 1;
					}
					codec.bitrate = strtol(value, 0L, 0);
					set_codecs |= CBitrate;
					break;
				case SUB_BITRATE_PEAK:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <bitrate_peak>\n");
						usage();
						return 1;
					}
					codec.bitrate_peak =
					    strtol(value, 0L, 0);
					set_codecs |= CBitratePeak;
					break;
				case SUB_STREAM_TYPE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <stream_type>\n");
						usage();
						return 1;
					}
					codec.stream_type =
					    strtol(value, 0L, 0);
					set_codecs |= CStreamType;
					break;
				case SUB_FRAMERATE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <framerate>\n");
						usage();
						return 1;
					}
					codec.framerate = strtol(value, 0L, 0);
					set_codecs |= CFreamerate;
					break;
				case SUB_FRAMESPERGOP:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <framespergop>\n");
						usage();
						return 1;
					}
					codec.framespergop =
					    strtol(value, 0L, 0);
					set_codecs |= CFramesPerGOP;
					break;
				case SUB_BFRAMES:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <bframes>\n");
						usage();
						exit(1);
					}
					codec.bframes = strtol(value, 0L, 0);
					set_codecs |= CBFrames;
					break;
				case SUB_GOP_CLOSURE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <gop_closure>\n");
						usage();
						exit(1);
					}
					codec.gop_closure =
					    strtol(value, 0L, 0);
					set_codecs |= CGOPClosure;
					break;
				case SUB_AUDIO:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <audio>\n");
						usage();
						exit(1);
					}
					codec.audio_bitmask =
					    strtol(value, 0L, 0);
					set_codecs |= CAudio;
					break;
				case SUB_DNR_MODE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <dnr_mode>\n");
						usage();
						exit(1);
					}
					codec.dnr_mode = strtol(value, 0L, 0);
					set_codecs |= CDNRMode;
					break;
				case SUB_DNR_TYPE:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <dnr_type>\n");
						usage();
						exit(1);
					}
					codec.dnr_type = strtol(value, 0L, 0);
					set_codecs |= CDNRType;
					break;
				case SUB_DNR_SPATIAL:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <dnr_spatial>\n");
						usage();
						exit(1);
					}
					codec.dnr_spatial =
					    strtol(value, 0L, 0);
					set_codecs |= CDNRSpatial;
					break;
				case SUB_DNR_TEMPORAL:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <dnr_temporal>\n");
						usage();
						exit(1);
					}
					codec.dnr_temporal =
					    strtol(value, 0L, 0);
					set_codecs |= CDNRTemporal;
					break;
				case SUB_PULLDOWN:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <pulldown>\n");
						usage();
						exit(1);
					}
					codec.pulldown = strtol(value, 0L, 0);
					set_codecs |= CPullDown;
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
		case OptSetFormat:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_WIDTH:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <width>\n");
						usage();
						exit(1);
					}
					vfmt.fmt.pix.width =
					    strtol(value, 0L, 0);
					set_fmts |= FMTWidth;
					break;
				case SUB_HEIGHT:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <height>\n");
						usage();
						exit(1);
					}
					vfmt.fmt.pix.height =
					    strtol(value, 0L, 0);
					set_fmts |= FMTHeight;
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
		case OptSetITVCRegister:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_REG:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <reg>\n");
						usage();
						exit(1);
					}
					itvc_reg.reg =
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
					itvc_reg.val =
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
		case OptListITVCRegisters:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
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
		case OptSetDecRegister:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_REG:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <reg>\n");
						usage();
						exit(1);
					}
					saa7115_reg.reg = strtol(value, 0L, 0);
					break;
				case SUB_VAL:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <val>\n");
						usage();
						exit(1);
					}
					saa7115_reg.val =
					    (unsigned char)strtol(value, 0L, 0);
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
		case OptSetEncRegister:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_REG:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <reg>\n");
						usage();
						exit(1);
					}
					saa7115_reg.reg = strtol(value, 0L, 0);
					break;
				case SUB_VAL:
					if (value == NULL) {
						fprintf(stderr,
							"No value given to suboption <val>\n");
						usage();
						exit(1);
					}
					saa7115_reg.val =
					    (unsigned char)strtol(value, 0L, 0);
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
		case OptSetInput:
			input = strtol(optarg, 0L, 0);
			break;
		case OptSetOutput:
			output = strtol(optarg, 0L, 0);
			break;
		case OptSetAudioInput:
			vaudio.index = strtol(optarg, 0L, 0);
			break;
		case OptSetFreq:
			vf.frequency = strtol(optarg, 0L, 0);
			break;
		case OptSetStandard:
                        if (!strcmp(optarg, "pal")) {
                                std = V4L2_STD_PAL;
                        } else if (!strcmp(optarg, "ntsc")) {
                                std = V4L2_STD_NTSC;
                        } else if (!strcmp(optarg, "secam")) {
                                std = V4L2_STD_SECAM;
                        } else {
			        std = strtol(optarg, 0L, 0);
                        }
			break;
		case OptGetDriverInfo:{
			break;
		}
		case OptSetIO:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_INPUT:
					if (value == NULL) {
						printf
						    ("No value given to suboption <input>\n");
						usage();
					}
					mspm.input = strtol(value, 0L, 0);
					break;
				case SUB_OUTPUT:
					if (value == NULL) {
						printf
						    ("No value given to suboption <output>\n");
						usage();
					}
					mspm.output = strtol(value, 0L, 0);
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
		case OptSetCtrl:
			subs = optarg;
			while (*subs != '\0') {
				switch (getsubopt(&subs, subopts, &value)) {
				case SUB_BRIGHTNESS:
					ctrl.id = V4L2_CID_BRIGHTNESS;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_CONTRAST:
					ctrl.id = V4L2_CID_CONTRAST;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_SATURATION:
					ctrl.id = V4L2_CID_SATURATION;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_HUE:
					ctrl.id = V4L2_CID_HUE;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_VOLUME:
					ctrl.id = V4L2_CID_AUDIO_VOLUME;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_BALANCE:
					ctrl.id = V4L2_CID_AUDIO_BALANCE;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_BASS:
					ctrl.id = V4L2_CID_AUDIO_BASS;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_TREBLE:
					ctrl.id = V4L2_CID_AUDIO_TREBLE;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_LOUDNESS:
					ctrl.id = V4L2_CID_AUDIO_LOUDNESS;
					ctrl.value = strtol(value, 0L, 0);
					break;
				case SUB_MUTE:
					ctrl.id = V4L2_CID_AUDIO_MUTE;
					ctrl.value = strtol(value, 0L, 0);
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
		case OptSetSAPMode:
			mode = strtol(optarg, 0L, 0);
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

	info.size = sizeof(struct ivtv_driver_info);
	if (ioctl(fd, IVTV_IOC_G_DRIVER_INFO, &info) != 0) {
                fprintf(stderr, "not an ivtv driver device!\n");
        }

	/* Setting Opts */

	if (options[OptSetCodec]) {
		struct ivtv_ioctl_codec in_codec;
		if (ioctl(fd, IVTV_IOC_G_CODEC, &in_codec) < 0)
			fprintf(stderr, "ioctl: IVTV_IOC_G_CODEC failed\n");
		else {
			if (set_codecs & CAspect)
				in_codec.aspect = codec.aspect;
			if (set_codecs & CBitrateMode)
				in_codec.bitrate_mode = codec.bitrate_mode;
			if (set_codecs & CBitrate)
				in_codec.bitrate = codec.bitrate;
			if (set_codecs & CBitratePeak)
				in_codec.bitrate_peak = codec.bitrate_peak;
			if (set_codecs & CStreamType)
				in_codec.stream_type = codec.stream_type;
			if (set_codecs & CFreamerate)
				in_codec.framerate = codec.framerate;
			if (set_codecs & CFramesPerGOP)
				in_codec.framespergop = codec.framespergop;
			if (set_codecs & CBFrames)
				in_codec.bframes = codec.bframes;
			if (set_codecs & CGOPClosure)
				in_codec.gop_closure = codec.gop_closure;
			if (set_codecs & CAudio)
				in_codec.audio_bitmask = codec.audio_bitmask;
			if (set_codecs & CDNRMode)
				in_codec.dnr_mode = codec.dnr_mode;
			if (set_codecs & CDNRType)
				in_codec.dnr_type = codec.dnr_type;
			if (set_codecs & CDNRSpatial)
				in_codec.dnr_spatial = codec.dnr_spatial;
			if (set_codecs & CDNRTemporal)
				in_codec.dnr_temporal = codec.dnr_temporal;
			if (set_codecs & CPullDown)
				in_codec.pulldown = codec.pulldown;
			doioctl(fd, IVTV_IOC_S_CODEC, &in_codec, "IVTV_IOC_S_CODEC");
		}
	}

	if (options[OptSetFormat]) {
		struct v4l2_format in_vfmt;
		printf("ioctl: VIDIOC_S_FMT\n");
		in_vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(fd, VIDIOC_G_FMT, &in_vfmt) < 0)
			fprintf(stderr, "ioctl: VIDIOC_G_FMT failed\n");
		else {
			printf("\tBefore:\n");
			if (printfmt(in_vfmt) != 0)
				fprintf(stderr, "error printing result\n");
			if (set_fmts & FMTWidth)
				in_vfmt.fmt.pix.width = vfmt.fmt.pix.width;
			if (set_fmts & FMTHeight)
				in_vfmt.fmt.pix.height = vfmt.fmt.pix.height;
			if (ioctl(fd, VIDIOC_S_FMT, &in_vfmt) < 0)
				fprintf(stderr, "ioctl: VIDIOC_S_FMT failed\n");
			vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (ioctl(fd, VIDIOC_G_FMT, &vfmt) < 0)
				fprintf(stderr, "ioctl: VIDIOC_G_FMT failed\n");
			else {
				printf("\n\tAfter:\n");
				if (printfmt(vfmt) != 0)
					fprintf(stderr,
						"error printing result\n");
			}
		}
	}

	if (options[OptReloadFW]) {
		doioctl(fd, IVTV_IOC_RELOAD_FW, NULL, "IVTV_IOC_RELOAD_FW");
	}

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

	if (options[OptStreamOff]) {
		doioctl(fd, VIDIOC_STREAMOFF, NULL, "VIDIOC_STREAMOFF");
	}

	if (options[OptEndGop]) {
		if (doioctl(fd, IVTV_IOC_S_GOP_END, &end_gop,
			"IVTV_IOC_S_GOP_END") == 0)
			printf("End GOP capture set to %d\n", end_gop);
	}

	if (options[OptSetITVCRegister]) {
		if (doioctl(fd, IVTV_IOC_S_ITVC_REG, &itvc_reg,
			"IVTV_IOC_S_ITVC_REG") == 0)
			printf("ITVC register 0x%08lx set to 0x%08x\n",
		       		itvc_reg.reg, itvc_reg.val);
	}

	if (options[OptSetDecRegister]) {
		if (doioctl(fd, IVTV_IOC_S_DECODER_REG, &saa7115_reg,
			"IVTV_IOC_S_DECODER_REG") == 0)
			printf("decoder register 0x%04lx set to 0x%02x\n",
		       		saa7115_reg.reg, saa7115_reg.val);
	}

	if (options[OptSetEncRegister]) {
		if (doioctl(fd, IVTV_IOC_S_ENCODER_REG, &saa7115_reg,
			"IVTV_IOC_S_ENCODER_REG") == 0)
			printf("set encoder register 0x%04lx to 0x%02x\n",
		       		saa7115_reg.reg, saa7115_reg.val);
	}

	if (options[OptSetFreq]) {
		vf.tuner = 0;
		if (doioctl(fd, VIDIOC_S_FREQUENCY, &vf, 
			"VIDIOC_S_FREQUENCY") == 0)
			printf("Frequency set to %d\n", vf.frequency);
	}

	if (options[OptSetStandard]) {
                if (std < 16) {
                        vs.index = std;
                        if (ioctl(fd, VIDIOC_ENUMSTD, &vs) >= 0) {
                                std = vs.id;
                        }
                }
		if (doioctl(fd, VIDIOC_S_STD, &std, "VIDIOC_S_STD") == 0)
			printf("Standard set to %08llx\n", (unsigned long long)std);
	}

	if (options[OptGetDriverInfo]) {
                printf(" driver info:\n");
                printf("   version = 0x%08x (%d.%d.%d)\n", info.version,
                       ((info.version >> 16) & 0xff),
                       ((info.version >> 8) & 0xff),
                       ((info.version) & 0xff));
                printf("   comment = %s\n", info.comment);
                printf("   cardnr = %d\n", info.cardnr);
                printf("   hw_flags = %x\n", info.hw_flags);
	}

	if (options[OptSetIO]) {
		if ((mspm.input < 9) & (mspm.output < 3)) {
			doioctl(fd, IVTV_IOC_S_MSP_MATRIX, &mspm, "IVTV_IOC_S_MSP_MATRIX");
		} else
			fprintf(stderr, "Invalid input/output setting %d/%d\n",
				mspm.input, mspm.output);
	}

	if (options[OptSetCtrl]) {
		doioctl(fd, VIDIOC_S_CTRL, &ctrl, "VIDIOC_S_CTRL");
	}

	/* informational opts */

	if (options[OptListCodec]) {
		if (doioctl(fd, IVTV_IOC_G_CODEC, &codec,
			"IVTV_IOC_G_CODEC") == 0) {
			printf("Codec parameters\n");
			printf("aspect      : %d\n", codec.aspect);
			printf("audio       : 0x%04x\n", codec.audio_bitmask);
			printf("bframes     : %d\n", codec.bframes);
			printf("bitrate_mode: %d\n", codec.bitrate_mode);
			printf("bitrate     : %d\n", codec.bitrate);
			printf("bitrate_peak: %d\n", codec.bitrate_peak);
			printf("dnr_mode    : %d\n", codec.dnr_mode);
			printf("dnr_spatial : %d\n", codec.dnr_spatial);
			printf("dnr_temporal: %d\n", codec.dnr_temporal);
			printf("dnr_type    : %d\n", codec.dnr_type);
			printf("framerate   : %d\n", codec.framerate);
			printf("framespergop: %d\n", codec.framespergop);
			printf("gop_closure : %d\n", codec.gop_closure);
			printf("pulldown    : %d\n", codec.pulldown);
			printf("stream_type : %d\n", codec.stream_type);
		}
	}

	if (options[OptGetFormat]) {
		vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (doioctl(fd, VIDIOC_G_FMT, &vfmt, "VIDIOC_G_FMT") == 0)
			if (printfmt(vfmt) != 0)
				fprintf(stderr, "error printing result\n");
	}
	if (options[OptListITVCRegisters]) {
		printf("ioctl: IVTV_IOC_G_ITVC_REG\n");
		printf("                00       04       08       0C       10       14       18       1C");
		for (i = min_reg; i <= max_reg; i += 4) {
			itvc_reg.reg = (unsigned long)i;
			if (ioctl(fd, IVTV_IOC_G_ITVC_REG, &itvc_reg) < 0) {
				fprintf(stderr,
					"ioctl: "
					"IVTV_IOC_G_ITVC_REG "
					"failed for 0x%08lx\n", itvc_reg.reg);
			} else {
				/*if ((i&0xf)==0) 
				   printf("\n%08x: ",i); */
				if ((i & 0x1f) == 0)
					printf("\n%08x: ", i);
				printf("%08x ", itvc_reg.val);
			}
			usleep(1);
		}
		printf("\n");
	}
	if (options[OptListDecRegisters]) {
                if (info.hw_flags & (IVTV_HW_SAA7114|IVTV_HW_SAA7115)) {
		        printf("      SAA711x registers\n");
                        printregs(fd, 0, 0x100);
                }
                else {
		        printf("      cx25840 registers\n");
                        printregs(fd, 0, 2);
                        printregs(fd, 0x100, 0x60);
                        printregs(fd, 0x200, 0x40);
                        printregs(fd, 0x400, 0xc0);
                        printregs(fd, 0x800, 0x1b0);
                }
	}

	if (options[OptListEncRegisters]) {
		printf("ioctl: IVTV_IOC_G_ENCODER_REG\n");
		printf("      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
		for (i = 0; i < 128; i++) {
			saa7115_reg.reg = (unsigned char)i;
			if (ioctl(fd, IVTV_IOC_G_ENCODER_REG, &saa7115_reg) < 0)
				fprintf(stderr,
					"ioctl: IVTV_IOC_G_ENCODER_REG failed\n");
			else {
				if ((i & 0xf) == 0)
					printf("\n%04x: ", i);
				printf("%02x ", saa7115_reg.val);
			}
		}
		printf("\n");
	}

	if (options[OptListCapability]) {
		if (doioctl(fd, VIDIOC_QUERYCAP, &vcap,
			"VIDIOC_QUERYCAP") == 0) {
			printf("\tDriver name   : %s\n", vcap.driver);
			printf("\tCard type     : %s\n", vcap.card);
			printf("\tBus info      : %s\n", vcap.bus_info);
			printf("\tDriver version: %d\n", vcap.version);
			printf("\tCapabilities  : 0x%08X\n", vcap.capabilities);
		}
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

	if (options[OptListInputs]) {
		vin.index = 0;
		printf("ioctl: VIDIOC_ENUMINPUT\n");
		while (ioctl(fd, VIDIOC_ENUMINPUT, &vin) >= 0) {
			if (vin.index)
				printf("\n");
			printf("\tInput   : %d\n", vin.index);
			printf("\tName    : %s\n", vin.name);
			printf("\tType    : 0x%08X\n", vin.type);
			printf("\tAudioset: 0x%08X\n", vin.audioset);
			printf("\tTuner   : 0x%08X\n", vin.tuner);
			printf("\tStandard: 0x%016llX ( ", (unsigned long long)vin.std);
			if (vin.std & 0x000FFF)
				printf("PAL ");	// hack
			if (vin.std & 0x00F000)
				printf("NTSC ");	// hack
			if (vin.std & 0x7F0000)
				printf("SECAM ");	// hack
			printf(")\n");
			printf("\tStatus  : %d\n", vin.status);
			vin.index++;
		}
	}

	if (options[OptSetInput]) {
		if (doioctl(fd, VIDIOC_S_INPUT, &input, "VIDIOC_S_INPUT") == 0)
			printf("Video input set to %d\n", input);
	}

	if (options[OptGetInput]) {
		if (doioctl(fd, VIDIOC_G_INPUT, &input, "VIDIOC_G_INPUT") == 0)
			printf("Video input = %d\n", input);
	}

	if (options[OptListOutputs]) {
		vout.index = 0;
		printf("ioctl: VIDIOC_ENUMOUTPUT\n");
		while (ioctl(fd, VIDIOC_ENUMOUTPUT, &vout) >= 0) {
			if (vout.index)
				printf("\n");
			printf("\tOutput  : %d\n", vout.index);
			printf("\tName    : %s\n", vout.name);
			printf("\tType    : 0x%08X\n", vout.type);
			printf("\tAudioset: 0x%08X\n", vout.audioset);
			printf("\tStandard: 0x%016llX ( ", (unsigned long long)vout.std);
			if (vout.std & 0x000FFF)
				printf("PAL ");	// hack
			if (vout.std & 0x00F000)
				printf("NTSC ");	// hack
			if (vout.std & 0x7F0000)
				printf("SECAM ");	// hack
			printf(")\n");
			vout.index++;
		}
	}

	if (options[OptSetOutput]) {
		if (doioctl(fd, VIDIOC_S_OUTPUT, &output, "VIDIOC_S_OUTPUT") == 0)
			printf("Output set to %d\n", output);
	}

	if (options[OptGetOutput]) {
		if (doioctl(fd, VIDIOC_G_OUTPUT, &output, "VIDIOC_G_OUTPUT") == 0)
			printf("Output = %d\n", output);
	}

	if (options[OptListAudioInputs]) {
		struct v4l2_audio vaudio;	/* list audio inputs */
		vaudio.index = 0;
		printf("ioctl: VIDIOC_ENUMAUDIO\n");
		while (ioctl(fd, VIDIOC_ENUMAUDIO, &vaudio) >= 0) {
			if (vaudio.index)
				printf("\n");
			printf("\tInput   : %d\n", vaudio.index);
			printf("\tName    : %s\n", vaudio.name);
			vaudio.index++;
		}
	}

	if (options[OptSetAudioInput]) {
		if (doioctl(fd, VIDIOC_S_AUDIO, &vaudio, "VIDIOC_S_AUDIO") == 0)
			printf("Audio input set to %d\n", vaudio.index);
	}

	if (options[OptGetAudioInput]) {
		if (doioctl(fd, VIDIOC_G_AUDIO, &vaudio, "VIDIOC_G_AUDIO") == 0)
			printf("Audio input = %d: %s\n", vaudio.index, vaudio.name);
	}

	if (options[OptGetFreq]) {
		vf.tuner = 0;
		if (doioctl(fd, VIDIOC_G_FREQUENCY, &vf, "VIDIOC_G_FREQUENCY") == 0)
			printf("Frequency = %d\n", vf.frequency);
	}

	if (options[OptListStandards]) {
		printf("ioctl: VIDIOC_ENUMSTD\n");
		vs.index = 0;
		while (ioctl(fd, VIDIOC_ENUMSTD, &vs) >= 0) {
			if (vs.index)
				printf("\n");
			printf("\tindex       : %d\n", vs.index);
			printf("\tID          : 0x%016llX\n", (unsigned long long)vs.id);
			printf("\tName        : %s\n", vs.name);
			printf("\tFrame period: %d/%d\n",
			       vs.frameperiod.numerator,
			       vs.frameperiod.denominator);
			printf("\tFrame lines : %d\n", vs.framelines);
			vs.index++;
		}
	}

	if (options[OptGetStandard]) {
		if (doioctl(fd, VIDIOC_G_STD, &std, "VIDIOC_G_STD") == 0)
			printf("Video standard = 0x%08llx\n", (unsigned long long)std);
	}

	if (options[OptGetVbi]) {
                vbi_fmt.type = V4L2_BUF_TYPE_SLICED_VBI_CAPTURE;
		if (doioctl(fd, VIDIOC_G_FMT, &vbi_fmt, 
			"VIDIOC_G_FMT") == 0)
			printf("vbi service_set = %08x\n",
			       vbi_fmt.fmt.sliced.service_set);
	}

	if (options[OptSetVbi]) {
                if (vbi_fmt.fmt.sliced.service_set == 0) {
                        // switch to raw mode
                        vbi_fmt.type = V4L2_BUF_TYPE_VBI_CAPTURE;
		        if (doioctl(fd, VIDIOC_G_FMT, &vbi_fmt, "VIDIOC_G_FMT") == 0 &&
                            doioctl(fd, VIDIOC_S_FMT, &vbi_fmt, "VIDIOC_S_FMT") == 0)
                                printf("vbi set to raw VBI\n");
                } else {
                        vbi_fmt.type = V4L2_BUF_TYPE_SLICED_VBI_CAPTURE;
                        if (doioctl(fd, VIDIOC_S_FMT, &vbi_fmt, 
                                "VIDIOC_S_FMT") == 0)
                                printf("vbi service_set set to %08x\n",
                                       vbi_fmt.fmt.sliced.service_set);
                }
	}

	if (options[OptGetVbiEmbed]) {
		if (doioctl(fd, IVTV_IOC_G_VBI_EMBED, &vbi_insert_mpeg,
			"IVTV_IOC_G_VBI_EMBED") == 0)
			printf("vbi embedded in MPEG stream: %s\n",
			       vbi_insert_mpeg ? "on" : "off");
	}

	if (options[OptSetVbiEmbed]) {
		if (doioctl(fd, IVTV_IOC_S_VBI_EMBED, &vbi_insert_mpeg,
			"IVTV_IOC_S_VBI_EMBED") == 0)
			printf("vbi embedded in MPEG stream set to %d\n",
			       vbi_insert_mpeg);
	}

	if (options[OptListCtrls]) {
		struct v4l2_queryctrl queryctrl;
		int id;

		printf("ioctl: VIDIOC_QUERYCTRL\n");
		for (id = V4L2_CID_BASE; id < V4L2_CID_LASTP1; id++) {
			queryctrl.id = id;
			if (ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl) == 0) {
				if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
					continue;
				ctrl.id = queryctrl.id;
				if (ioctl(fd, VIDIOC_G_CTRL, &ctrl) == 0)
					printf("%s = %d\n", queryctrl.name,
					       ctrl.value);
				else
					printf("error getting ctrl %s\n",
					       queryctrl.name);
			} else if (errno == EINVAL) {
				continue;
			} else {
				perror("VIDIOC_QUERYCTRL");
				exit(1);
			}
		}
	}

	if (options[OptSetGPIO]) {
		struct ivtv_ioctl_register reg;

		reg.reg = IVTV_REG_GPIO_DIR_OFFSET;
		reg.val = gpio_dir;
		if (gpio_set_dir && doioctl(fd, IVTV_IOC_S_ITVC_REG, &reg,
			"IVTV_IOC_S_ITVC_REG") == 0)
			printf("GPIO dir set to 0x%04x\n", reg.val);
		reg.reg = IVTV_REG_GPIO_OUT_OFFSET;
		reg.val = gpio_out;
		if (doioctl(fd, IVTV_IOC_S_ITVC_REG, &reg,
			"IVTV_IOC_S_ITVC_REG") == 0)
			printf("GPIO out set to 0x%04x\n", reg.val);
	}

	if (options[OptListGPIO]) {
		struct ivtv_ioctl_register reg;

		reg.reg = IVTV_REG_GPIO_IN_OFFSET;
		if (ioctl(fd, IVTV_IOC_G_ITVC_REG, &reg) == 0) 
			printf("GPIO in:  0x%04x\n", reg.val);
		reg.reg = IVTV_REG_GPIO_DIR_OFFSET;
		if (ioctl(fd, IVTV_IOC_G_ITVC_REG, &reg) == 0) 
			printf("GPIO dir: 0x%04x\n", reg.val);
		reg.reg = IVTV_REG_GPIO_OUT_OFFSET;
		if (ioctl(fd, IVTV_IOC_G_ITVC_REG, &reg) == 0) 
			printf("GPIO out: 0x%04x\n", reg.val);
	}

	if (options[OptSetDebugLevel]) {
		new_debug_level = debug_level;
		if (doioctl(fd, IVTV_IOC_S_DEBUG_LEVEL, &new_debug_level,
			"IVTV_IOC_S_DEBUG_LEVEL") == 0) {
			printf(" set debug level: ");
			print_debug_mask(new_debug_level);
			printf("\n");
		}
	}

	if (options[OptGetDebugLevel]) {
		gdebug_level = 0;
		if (doioctl(fd, IVTV_IOC_G_DEBUG_LEVEL, &gdebug_level,
			"IVTV_IOC_G_DEBUG_LEVEL") == 0) {
			printf(" debug level: ");
			print_debug_mask(gdebug_level);
			printf("\n");
		}
	}

	if (options[OptGetSAPMode]) {
		struct v4l2_tuner vt;
		memset(&vt, 0, sizeof(struct v4l2_tuner));
		if (doioctl(fd, VIDIOC_G_TUNER, &vt, "VIDIOC_G_TUNER") == 0) {
			if (vt.capability & V4L2_TUNER_CAP_LOW)
				printf(" frequency range: %.1f MHz - %.1f MHz\n",
				     vt.rangelow / 16000.0, vt.rangehigh / 16000.0);
			else
				printf(" frequency range: %.1f MHz - %.1f MHz\n",
				     vt.rangelow / 16.0, vt.rangehigh / 16.0);
			printf(" signal strength: %d%%\n", (int)(vt.signal / 655.35));
			printf(" current audio mode   : %d\n", vt.audmode);
			printf("   (stereo %d, lang1 %d, lang2 %d, mono %d)\n",
		       		V4L2_TUNER_MODE_STEREO,
		       		V4L2_TUNER_MODE_LANG1,
		       		V4L2_TUNER_MODE_LANG2, V4L2_TUNER_MODE_MONO);
			printf(" available subchannels: 0x%x\n", vt.rxsubchans);
			printf("   (stereo 0x%x, lang1 0x%x, lang2 0x%x, mono 0x%x)\n",
		       		V4L2_TUNER_SUB_STEREO,
		       		V4L2_TUNER_SUB_LANG1,
		       		V4L2_TUNER_SUB_LANG2, V4L2_TUNER_SUB_MONO);
		}
	}
	if (options[OptSetSAPMode]) {
		struct v4l2_tuner vt;

		memset(&vt, 0, sizeof(struct v4l2_tuner));
		if (ioctl(fd, VIDIOC_G_TUNER, &vt) < 0) {
			fprintf(stderr, "ioctl: VIDIOC_G_TUNER failed\n");
			exit(1);
		}
		vt.audmode = mode;
		doioctl(fd, VIDIOC_S_TUNER, &vt, "VIDIOC_S_TUNER");
	}

	if (options[OptPassThrough]) {
		doioctl(fd, IVTV_IOC_PASSTHROUGH, &passthrough, "IVTV_IOC_PASSTHROUGH");
	}

	if (option_reset_ir) {
		doioctl(fd, IVTV_IOC_RESET_IR, NULL, "IVTV_IOC_RESET_IR");
	}

	if (option_version) {
                printf("ivtvctl version " IVTV_VERSION "\n");
	}

	if (option_log_status) {
		static char buf[40960];
		int len;

		if (doioctl(fd, VIDIOC_LOG_STATUS, NULL, "VIDIOC_LOG_STATUS") == 0) {
			len = klogctl(3, buf, sizeof(buf) - 1);
			if (len >= 0) {
				char *p = buf;
				char *q;

				buf[len] = 0;
				while ((q = strstr(p, "START STATUS CARD #"))) {
					p = q + 1;
				}
				if (p) {
					while (p > buf && *p != '<') p--;
					q = p;
					while ((q = strstr(q, "<6>"))) {
						memcpy(q, "   ", 3);
					}
					printf("%s", p);
				}
			}
                }
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
