/*
    radio utility
    Copyright (C) 2004  Hans Verkuil  <hverkuil@xs4all.nl>

    Enhanced with proper options and new functionality:
    Copyright (C) 2004  Brian Jackson

    Added -c option:
    Copyright (C) 2004  Mark Rafn

    Added more options and functionality:
    Copyright (C) 2005  Adam Forsyth  <agforsyth@gmail.com>

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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#define __user
#include <linux/videodev2.h>
#include "ivtv.h"

#define RADIO_DEV "/dev/radio0"
#define AUDIO_IN_DEV "/dev/video24"

#define TRUE 1
#define FALSE 0

struct config {
	int just_tune;
	int passthrough;
	int channelchange;
	char prog_name[255];
	struct v4l2_tuner tuner;
	struct v4l2_frequency freq;
	int fh;
	char radio_dev[PATH_MAX];
	char audio_in[PATH_MAX];
	char play_cmd[PATH_MAX + 50];
	char *play_cmd_tmpl;
	int verbose;
} cfg;

void print_usage(void)
{
	fprintf(stderr, "Usage: %s <options>\n", cfg.prog_name);
	fprintf(stderr, "Possible options are:\n");
	fprintf(stderr,
		"    -d <device>    Radio control device (default: %s)\n",
		RADIO_DEV);
	fprintf(stderr, "    -s             Scan for channels\n");
	fprintf(stderr, "    -a             Scan for frequencies\n");
	fprintf(stderr, 
	    "                   Use if Scan for channels is missing stuff\n");
	fprintf(stderr, 
	    "                   If e.g. 90.9, 91.1, 91.3 all exist use this\n");
	fprintf(stderr, "    -f <freq>      Tune to a specific frequency\n");
	fprintf(stderr,
		"    -j             Just tune (don't try to play the audio)\n");
	fprintf(stderr,
		"                   You'll have to play the audio yourself (see README.radio)\n");
	fprintf(stderr, "    -P             Use passthrough mode of card\n");
	fprintf(stderr, "    -g             Enable channel changing for passthrough mode\n");
	fprintf(stderr, "    -v             Verbose\n");
	fprintf(stderr, "    -h             Display this help message\n");
	fprintf(stderr,
		"    -i <device>    PCM audio capture device (default: %s)\n",
		AUDIO_IN_DEV);
	fprintf(stderr,
		"    -c <command>   Command to play audio.  This will be processed by\n");
	fprintf(stderr,
		"                   the shell, after substituting %%s with the audio device.\n");
	fprintf(stderr, "                   Default: \"%s\"\n",
		cfg.play_cmd_tmpl);
}

static int get_cardnr(int fd)
{
        struct ivtv_driver_info info;
	int res;

	memset(&info, 0, sizeof(info));
	info.size = sizeof(info);
	res = ioctl(fd, IVTV_IOC_G_DRIVER_INFO, &info);
	if (res == 0 && info.size >= IVTV_DRIVER_INFO_V2_SIZE) {
		return info.cardnr;
	}
	return -1;
}

int scan_channels(int allfreqs)
{
	int i;

	cfg.fh = open(cfg.radio_dev, O_RDONLY);
	if (cfg.fh == -1) {
		perror("radio");
		fprintf(stderr, "cannot open %s\n", cfg.radio_dev);
		exit(1);
	}

	cfg.tuner.index = 0;
	if (ioctl(cfg.fh, VIDIOC_G_TUNER, &cfg.tuner) == -1) {
		fprintf(stderr, "ioctl: Failed to set tuner (%s)\n",
			strerror(errno));
		exit(1);
	}
	int isafreq = 0;
	int freqindex = 0;
	int freqstrength;
	int rangehigh;
	int rangelow;
	double freqs[5][2];
	double printedfreq;

	if (allfreqs == 1) {
		rangelow = cfg.tuner.rangelow;
		rangehigh = cfg.tuner.rangehigh;
	} else {
		rangelow = 1392;
		rangehigh = 1728;
	}
	for (i = rangelow; i < rangehigh; i += 1000) {
		cfg.freq.tuner = 0;
		cfg.freq.frequency = i;
		if (ioctl(cfg.fh, VIDIOC_S_FREQUENCY, &cfg.freq) == -1) {
			fprintf(stderr, "ioctl: Failed to set freq (%s)\n",
				strerror(errno));
			exit(1);
		}
		if (ioctl(cfg.fh, VIDIOC_G_TUNER, &cfg.tuner) == -1) {
			fprintf(stderr, "ioctl: Failed to set tuner (%s)\n",
				strerror(errno));
			exit(1);
		}
		//print long freq info if wanted
		printedfreq = cfg.freq.frequency;
		freqstrength = cfg.tuner.signal;
		if (cfg.verbose) {
			printf("%3.2f, %4.0f: %d \n", printedfreq / 16000.0, printedfreq, freqstrength);	
		} 
		if (cfg.tuner.signal > 16384) {
			isafreq++;
			freqindex = isafreq - 1;
			//if there are multiple stations in a row, this may detect the 1st and not the rest.
			if (isafreq < 6) {
				freqs[freqindex][0] = cfg.freq.frequency;
				freqs[freqindex][1] = cfg.tuner.signal;
			}
		} else {
			if (isafreq > 0) {
				switch(isafreq) {
				case 1:
					printedfreq = freqs[0][0];
					freqstrength = freqs[0][1];
				break;
				case 2:
					printedfreq = (freqs[0][0] + freqs[1][0]) / 2;
					freqstrength = (freqs[0][1] + freqs[1][1]) / 2;
				break;
				case 3:
					printedfreq = freqs[1][0];
					freqstrength = freqs[1][1];
				break;
				case 4:
					printedfreq = (freqs[1][0] + freqs[2][0]) / 2;
					freqstrength = (freqs[1][1] + freqs[2][1]) / 2;
				break;
				default:
					printedfreq = freqs[2][0];
					freqstrength = freqs[2][1];
				break;
				}
				printedfreq /= 1000;
				if(printedfreq > 1392 && printedfreq < 1728) {
					if((((int)printedfreq) % 16) == 12) {
						printf("STATION: %3.1f FM\n", (printedfreq / 16.0) - 0.01);				
					} else if((((int)printedfreq) % 16) == 4) {
						printf("STATION: %3.1f FM\n", (printedfreq / 16.0) + 0.01);
					} else {
						printf("STATION: %3.1f FM\n", (printedfreq / 16.0));				
					}
				}
			}	
			isafreq = 0;
		}
	}
	close(cfg.fh);
	return 0;
}

static void cleanup(int signal)
{
	printf("Cleaning up\n");
	if (cfg.passthrough) {
		cfg.passthrough = 0;
		ioctl(cfg.fh, IVTV_IOC_PASSTHROUGH, &cfg.passthrough);
	}
	close(cfg.fh);
	exit(0);
}

int main(int argc, char **argv)
{
	int opt;

	strcpy(cfg.prog_name, argv[0]);
	strcpy(cfg.radio_dev, RADIO_DEV);
	strcpy(cfg.audio_in, AUDIO_IN_DEV);
	cfg.play_cmd_tmpl = "aplay -f dat < %s";
	cfg.just_tune = FALSE;
	cfg.passthrough = FALSE;
	cfg.verbose = 0;

	if (argc < 2) {
		print_usage();
		exit(1);
	}

	while ((opt = getopt(argc, argv, "vghjPsad:f:i:c:")) != -1) {
		switch (opt) {
		case 'j':
			cfg.just_tune = TRUE;
			break;
		case 'P':
			printf("Setting passthrough mode\n");
			cfg.passthrough = TRUE;
			break;
		case 'f':
			cfg.freq.frequency = (__u32)(atof(optarg) * 16000 + .5);
			break;
		case 'd':
			strcpy(cfg.radio_dev, optarg);
			break;
		case 'i':
			strcpy(cfg.audio_in, optarg);
			break;
		case 'c':
			cfg.play_cmd_tmpl = optarg;
			break;
		case 'g':
			cfg.channelchange = TRUE;
			break;
		case 's':
			scan_channels(0);
			exit(0);
		case 'a':
			scan_channels(1);
			exit(0);
		case 'v':
			cfg.verbose = 1;
			break;
		case 'h':
			print_usage();
			exit(0);
		default:
			fprintf(stderr, "Unknown option\n");
			print_usage();
			exit(1);
		}
	}

	cfg.fh = open(cfg.radio_dev, O_RDWR);
	if (cfg.fh == -1) {
		perror("radio");
		fprintf(stderr, "cannot open %s\n", cfg.radio_dev);
		exit(1);
	}

	cfg.freq.tuner = 0;
	if (cfg.freq.frequency) {
		printf("set to freq %3.2f\n", cfg.freq.frequency / 16000.0);
		ioctl(cfg.fh, VIDIOC_S_FREQUENCY, &cfg.freq);
		if (cfg.verbose && ioctl(cfg.fh, VIDIOC_G_TUNER, &cfg.tuner) == 0) {
			printf("signal strength: %d\n", cfg.tuner.signal);
		}
	}

	if (cfg.passthrough) {
		if (ioctl(cfg.fh, IVTV_IOC_PASSTHROUGH, &cfg.passthrough) < 0) {
			fprintf(stderr, "passthrough ioctl failed\n");
			exit(1);
		}
		cfg.just_tune = TRUE;
	}

	signal(SIGPIPE, cleanup);
	signal(SIGHUP, cleanup);
	signal(SIGTERM, cleanup);
	signal(SIGUSR1, cleanup);
	signal(SIGINT, cleanup);

	/* if they want us to just tune, do that and wait, otherwise we
	   play the output with aplay */
	if (cfg.just_tune) {
		//allow in-program channel changing for passthru mode / just tune mode
		if(cfg.channelchange) {
		char newfreqs[8];
		while (1) {
			printf("Enter the frequency to change to: ");
			scanf("%s",newfreqs);
			cfg.freq.tuner = 0;
			cfg.freq.frequency = 16000 * atof(newfreqs);
			printf("set to freq %3.2f\n", cfg.freq.frequency / 16000.0);
			ioctl(cfg.fh, VIDIOC_S_FREQUENCY, &cfg.freq);
		}
		} else {
			pause();
		}
	} else {
		int radio_cardnr = get_cardnr(cfg.fh);
		int audio_cardnr = -1;
		int fd;
		
		fd = open(cfg.audio_in, O_RDONLY);
		if (fd != -1) {
			audio_cardnr = get_cardnr(fd);
			close(fd);
		}
		if (radio_cardnr != audio_cardnr) {
			fprintf(stderr, "%s belongs to a different ivtv driver then %s.\n",
					cfg.audio_in, cfg.radio_dev);
			fprintf(stderr,
				"Run ivtv-detect to discover the correct radio/PCM out combination.\n");
			exit(1);
		}

		snprintf(cfg.play_cmd, sizeof(cfg.play_cmd), cfg.play_cmd_tmpl,
			 cfg.audio_in);
		printf("Running: %s\n", cfg.play_cmd);
		if ((system(cfg.play_cmd)) == -1) {
			fprintf(stderr, "Failed to play audio with aplay\n");
			fprintf(stderr, "Make sure you have it installed\n");
			exit(1);
		}
	}

	cleanup(0);
	return 0;
}
