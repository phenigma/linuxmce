/*
    Copyright (C) 2004  Chris Kennedy <c@groovy.org>

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

#include <time.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>

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
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <linux/types.h>

#define __user
#include <linux/videodev2.h>

#include "ivtv.h"

#include "ivtv-functions.h"
#include "encoder.h"

/* defined in compile args */
/* #define VIDEO_PORT 0 */

void cleanup(int);
void __cleanup(void);
void stop_encoder(int);
void index_t(void);
void *index_thread(void *);
int mindex(char *output_file, char *index_file, char *bstatus);
int streamfd(int fdout, int fdin, int count);
int writeall(int fd, char *buf, int count);
int chann(int fd, int chan_num);
int chanf(int fd, int frequency);
void usage(void);

int splice(unsigned char *, int, int);
#define SPLICE_START 1
#define SPLICE_END   2
#define GOP_COUNTER  3
#define GOP_COUNTER_START  4

#define VIDEO_DEVICE "/dev/video"

#define START_BYTES 0
#define START_GOP 0

/* Encoder Settings */
#define VIDEO_INPUT	  0

#define BRIGHTNESS      128
#define CONTRAST        64
#define SATURATION      64
#define HUE             0

#define ASPECT  2
#define BMODE   0
#define BRATE   8000000
#define BPEAK   12000000
#define STREAM  0
#define AUDIO   201		/* 0xc9(201) = 256k 48000khz, 0xe9(233) = 384k 48000khz */
#define DNRMODE 3
#define DNRTYPE 3
#define DNRSPAT 0
#define DNRTEMP 0
#define PDOWN   0
#define FRATE   0
#define FPGOP   15
#define BFRAMES 3
#define GOPCL   1

int settings[18] = { ASPECT, BMODE, BRATE, BPEAK, STREAM, AUDIO,
	DNRMODE, DNRTYPE, DNRSPAT, DNRTEMP, PDOWN,
	BRIGHTNESS, CONTRAST, SATURATION, HUE, VIDEO_INPUT,
	FRATE, FPGOP
};

#define CAP_LAST_GOP 1
#define USE_STOPCAP

#define ELOCK "/tmp/en_lock."
#define ELOCKLINK "/tmp/en_lock_l."
#define BSTATUS "/tmp/"

//#define MAX_INPUT_LINE 1572864
//#define MAX_INPUT_LINE 256000
//#define MAX_INPUT_LINE 16384
#define MAX_INPUT_LINE 32768
#define SHOW_LINE_COUNT 0
#define VERBOSE 1
#define DEBUG 0

/* GOP Splicer/Timer Variables */
#define VID_START     0x000001e0
#define VID_END       0x000001ef
#define SEQ_START     0x000001b3
#define SEQ_END       0x000001b7
#define GOP_START     0x000001b8
#define PICTURE_START 0x00000100
#define SLICE_MIN     0x00000101
#define SLICE_MAX     0x000001af

char *lock_link = NULL;
int fdout = -1, fdin = -1, fd = -1;
int video_port = VIDEO_PORT;
char *video_device = NULL, *elock = NULL;
/* GOP Splicer/Timer Variables */
int startpos = 0;
int laststartpos = 0;
int gopset = 0;
int gopcount = 0;
int firstgoppos = 0;
int framesRead = 0;
int keyframedist = 0;
int lastKey = 0;
unsigned char prvpkt[3];
int start_write = 1;
int stop_gop = 0;
int end_stream = 0;
unsigned long total_bytes = 0;
unsigned long bytes;

/* Threading stuff */
pthread_t index_thread_tid;

int chan_num = 0;
int chan_freq = 0;
char *output_file = NULL;
char *index_file = NULL;
char *bstatus = NULL;

int main(int argc, char *argv[])
{
	unsigned char *line = NULL;
	int strsize, i, j;
	int seconds = 0;
	FILE *lck = NULL;
	pid_t pid;
	time_t now;
	int launch_thread = 0;
	int setup_only = 0;
	int stdout_stream = 0;

	/* GOP Splicer/Timer Variables */
	prvpkt[0] = '\0';
	prvpkt[2] = '\0';
	prvpkt[3] = '\0';
	startpos = 0;
	laststartpos = 0;
	gopset = 0;
	gopcount = 0;
	firstgoppos = 0;
	framesRead = 0;
	keyframedist = 0;
	lastKey = 0;

	/* get current UNIX time and PID */
	time(&now);
	pid = getpid();

	/* Allocate memory for Strings */
	lock_link = malloc(25 + 7);
	if (lock_link == NULL)
		exit(1);
	snprintf(lock_link, 25 + 7, "%s%d", ELOCKLINK, pid);

	signal(SIGPIPE, cleanup);
	signal(SIGHUP, cleanup);
	signal(SIGTERM, cleanup);
	signal(SIGUSR1, cleanup);
	signal(SIGINT, cleanup);
	signal(SIGALRM, stop_encoder);
	atexit(__cleanup);

  /*******************/
	/* Check Arguments */
  /*******************/
	if (argc > 1) {
		for (i = 1; argv[i] && (i <= argc); i++) {
			if ((strncmp(argv[i], "-", 1) == 0
			     && (strlen(argv[i]) == 1))) {
				if (i == (argc - 1)) {
					stdout_stream = 1;
					fdout = 1;
					output_file = "stdout";
				} else
					exit(1);
			} else if ((strncmp(argv[i], "-", 1) == 0)
				   && (argv[i][1] == '\0'))
				continue;
			else if (strncmp(argv[i], "-input", 3) == 0) {
				/* Input */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Input: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[15] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-hue", 3) == 0) {
				/* Hue */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Hue: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[14] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-saturation", 10) == 0) {
				/* Saturation */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Saturation: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[13] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-contrast", 8) == 0) {
				/* Contrast */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Contrast: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[12] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-brightness", 10) == 0) {
				/* Brightness */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Brightness: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[11] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-pulldown", 8) == 0) {
				/* Pulldown */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Pulldown: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[10] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-dnrtemp", 7) == 0) {
				/* DNR Temporal */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"DNR Temporal: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[9] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-dnrspat", 7) == 0) {
				/* DNR Spatial */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"DNR Spatial: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[8] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-dnrtype", 7) == 0) {
				/* DNR Type */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"DNR Type: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[7] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-dnrmode", 7) == 0) {
				/* DNR Mode */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"DNR Mode: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[6] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-audio", 5) == 0) {
				/* Audio Bitmask */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Audio Bitmask: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[5] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-stream", 6) == 0) {
				/* Stream Type */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Stream Type: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[4] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-bpeak", 5) == 0) {
				/* Bitrate Peak */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Bitrate: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[3] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-brate", 5) == 0) {
				/* Bitrate */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Bitrate: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[2] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-bmode", 5) == 0) {
				/* Bitrate Mode */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Bitrate Mode: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[1] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-aspect", 6) == 0) {
				/* Aspect Ratio */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Aspect Ratio: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[0] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-frate", 6) == 0) {
				/* Frame Rate */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Frame Rate: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[16] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-fpgop", 6) == 0) {
				/* Frames per GOP */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Frames per GOP: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				settings[17] = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-vport", 6) == 0) {
				/* Video Device */
				char *var = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Video Device: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				var = malloc(strsize + 1);
				if (var == NULL)
					continue;
				strncpy(var, argv[i + 1], strsize + 1);
				video_port = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-c", 2) == 0) {
				/* Tune Channel */
				char *station = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Tune Channel: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				station = malloc(strsize + 1);
				if (station == NULL)
					continue;
				strncpy(station, argv[i + 1], strsize + 1);
				chan_num = (int)atoi(station);
				i++;
				continue;
			} else if (strncmp(argv[i], "-f", 2) == 0) {
				/* Tune Frequency */
				char *freqstr = NULL;
				for (j = 0; argv[i + 1][j] != '\0'; j++) {
					if (isdigit(argv[i + 1][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Tune Frequency: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				strsize = strlen(argv[i + 1]);
				freqstr = malloc(strsize + 1);
				if (freqstr == NULL)
					continue;
				strncpy(freqstr, argv[i + 1], strsize + 1);
				chan_freq = (int)atoi(freqstr);
				i++;
				continue;
			} else if (strncmp(argv[i], "/", 1) == 0
				   && !stdout_stream) {
				/* File location */
				strsize = strlen(argv[i]);
				output_file = malloc(strsize + 1);
				index_file = malloc(strsize + 6 + 1);
				if (output_file == NULL || index_file == NULL)
					continue;
				strncpy(output_file, argv[i], strsize + 1);
				snprintf(index_file, (strsize + 6 + 1),
					 "%s.index", argv[i]);
				continue;
			} else if (strncmp(argv[i], "-s", 2) == 0) {
				/* Setup Encoder */
				setup_only = 1;
				continue;
			} else {
				/* Recording Time */
				for (j = 0; argv[i][j] != '\0'; j++) {
					if (isdigit(argv[i][j]) == 0) {
						/* Bad Argument Given */
						fprintf(stderr,
							"Rec Time: ERROR: bad option! %s\n",
							argv[i]);
						usage();
						exit(1);
					}
				}
				seconds = (int)atoi(argv[i]);
			}
		}
	} else {
		usage();
		exit(1);
	}

	elock = malloc(23 + 7);
	if (elock == NULL)
		exit(1);
	snprintf(elock, 23 + 7, "%s%d", ELOCK, video_port);

	video_device = malloc(24 + 1);
	if (video_device == NULL)
		exit(1);
	snprintf(video_device, 24 + 1, "%s%d", VIDEO_DEVICE, video_port);

	bstatus = malloc(29 + 1);
	if (bstatus == NULL)
		exit(1);
	snprintf(bstatus, 29 + 1, "%s%d.status", BSTATUS, video_port);

	/* Printout What we will do */
	if (VERBOSE && setup_only == 0)
		fprintf(stderr, "\n(%d) Encoding for %d Seconds to %s\n",
			video_port, seconds, output_file);

	/* Lock Video Input */
	lck = fopen(lock_link, "w");
	if (lck == NULL)
		return 1;

	fprintf(lck, "%d", pid);

	if (link(lock_link, elock) != 0) {
		fclose(lck);
		unlink(lock_link);
		return 1;
	} else
		fclose(lck);

	/* Setup Environment */
	umask(022);

	/* Allocate input line */
	line = malloc(MAX_INPUT_LINE + 1);
	if (line == NULL)
		exit(1);

	/* Open Video Input */
	while ((fdin = open(video_device, O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", video_device,
			strerror(errno));
		sleep(1);
	}

	/* Tune to a TV Frequency */
	if (chan_freq > 0) {
		if (VERBOSE)
			fprintf(stderr, "(%d) Tuning to Frequency %d\n",
				video_port, chan_freq);
		chanf(fdin, chan_freq);
	}

	/* Tune to a TV Channel */
	else if (chan_num > 0) {
		if (VERBOSE)
			fprintf(stderr, "(%d) Tuning to Channel %d\n",
				video_port, chan_num);
		chann(fdin, chan_num);
	}

	if (setup_only == 1) {
		if (VERBOSE)
			fprintf(stderr, "Setting up Encoder %d\n", video_port);
		setup_encoder(fdin, settings);
		ivtv_api_enc_endgop(fdin, CAP_LAST_GOP);
		cleanup(0);
	}

	/* setup video device */
	setup_encoder(fdin, settings);

	/* End Encoding at GOP Ending Call 0=StopNOW, 1=GOPwait */
	ivtv_api_enc_endgop(fdin, CAP_LAST_GOP);

	/* Open Mpeg Output */
	if (!stdout_stream) {
		if ((fdout = open(output_file, O_CREAT | O_WRONLY)) < 0) {
			fprintf(stderr, "Failed to open %s: %s\n", output_file,
				strerror(errno));
			exit(1);
		}
		chmod(output_file, 0644);
	}

	/* Time the Recording */
#if 0
	t_stop.it_interval.tv_sec = 0;
	t_stop.it_interval.tv_usec = 1;
	t_stop.it_value.tv_sec = 0;
	t_stop.it_value.tv_usec = (seconds * 1000000);

	setitimer(ITIMER_REAL, &t_stop, 0);
#endif

	alarm(seconds);

	while (start_write) {
		if (stop_gop) {
			stop_gop = 0;
			ivtv_api_enc_stop(fdin);
			end_stream = 1;
			signal(SIGALRM, SIG_IGN);
		}
		bytes = streamfd(fdout, fdin, MAX_INPUT_LINE);
		if (!bytes && end_stream)
			break;

		if (bytes > 0)
			total_bytes = total_bytes + bytes;
		else
			fprintf(stderr, "(%d) %lu received from encoder!!!\n",
				video_port, bytes);

		if (SHOW_LINE_COUNT)
			fprintf(stderr, "(%d) Wrote %lu bytes total: %lu\n",
				video_port, bytes, total_bytes);

		if (launch_thread == 0 && !stdout_stream) {
			launch_thread = 1;
			index_t();

		}
	}
	__cleanup();
	exit(0);
}

void cleanup(int signal)
{
	__cleanup();
	exit(0);
}

void __cleanup(void)
{
	if (fdin > 0)
		close(fdin);
	if (fdout > 0)
		close(fdout);
	unlink(lock_link);
	unlink(elock);
}

void stop_encoder(int signal)
{
#ifdef USE_STOPCAP
	fprintf(stderr, "Stop signal for Encoding received, bytes %lu\n",
		bytes);
	stop_gop = 1;
#else
	start_write = 0;
	cleanup(signal);
#endif
}

/* Code borrowed from MythTV */
int splice(unsigned char *buf, int len, int mode)
{
	unsigned char *bufptr = buf;
	unsigned int state = 0xFFFFFFFF, v = 0;
	int prvcount = -1;
	int count = 0;
	int lastgop = 0;
	int lastpic = 0;

	while (bufptr < buf + len) {
		if (++prvcount < 3)
			v = prvpkt[prvcount];
		else {
			v = *bufptr++;
			count++;
		}

		if (state == 0x000001) {
			state = ((state << 8) | v) & 0xFFFFFF;
			if (state >= SLICE_MIN && state <= SLICE_MAX) {
				continue;
			}

			if (state >= VID_START && state <= VID_END) {
				laststartpos = (bufptr - buf) + startpos - 4;
				continue;
			}

			switch (state) {
			case SEQ_START:
			{
				if (DEBUG)
					fprintf(stderr,
						"SeqHeader mode %d lastgop %d bytes %d\n",
						mode, lastgop, count);
				/*if(count >= 4 && mode == SPLICE_START) {
				   if(count >= 4)  
				   return count - 4;
				   } */
				break;
			}
			case SEQ_END:
			{
				if (DEBUG)
					fprintf(stderr,
						"SeqEnd mode %d lastgop %d bytes %d\n",
						mode, lastgop, count);
				if (count >= 1 && mode == SPLICE_END) {
					if (count >= 1)
						return count - 4;
				}
				break;
			}
			case GOP_START:
			{
				gopcount++;
				lastgop = count;
				if (DEBUG)
					fprintf(stderr,
						"GOPcount %d mode %d lastgop %d bytes %d\n",
						gopcount, mode, lastgop, count);
				if (mode == SPLICE_START) {
					if (count >= 1)
						return count - 1;
				} else if (mode == GOP_COUNTER_START) {
					if (count >= 1 && gopcount >= START_GOP)
						return count - 1;
				}
				break;
			}
			case PICTURE_START:
			{
				framesRead++;
				lastpic = count;
				if (DEBUG)
					fprintf(stderr,
						"Picture Start frames %d bytes %d\n",
						framesRead, count);
				break;
			}
			default:
				break;
			}
			continue;
		}
		state = ((state << 8) | v) & 0xFFFFFF;
	}
	memcpy(prvpkt, buf + len - 3, 3);
	if (mode == GOP_COUNTER)
		return gopcount - 1;
	else if (mode == SPLICE_END) {
		if (lastgop > 0)
			return lastgop;
		else
			return -1;
	} else
		return count;
}

int writeall(int fd, char *buf, int count)
{
	int origcount = count;

	/* Total Bytes Decoded */
	total_bytes = total_bytes + count;

	while (count > 0) {
		int n = write(fd, buf, count);
		if (n < 0)
			return n;
		count -= n;
		buf += n;
	}

	return origcount;
}

int streamfd(int fdout, int fdin, int count)
{
	static const int bufsize = 262144;
	char buf[bufsize];

	int remaining = count;
	count = 0;

	while (remaining > 0) {
		int k;
		int n = (remaining < bufsize) ? remaining : bufsize;
		n = read(fdin, buf, n);
		if (n <= 0)
			return n;
		k = writeall(fdout, buf, n);
		if (k < 0)
			return k;
		assert(k == n);
		remaining -= n;
		count += n;
	}

	return count;
}

void index_t(void)
{
	pthread_create(&index_thread_tid, NULL, index_thread,
		       (void *)(long)fdout);
	pthread_detach(index_thread_tid);

	return;
}

/* Index Thread */
void *index_thread(void *arg)
{
	mindex(output_file, index_file, bstatus);
	if (VERBOSE)
		fprintf(stderr, "(%d) index_loop stopped on byte %lu\n",
			video_port, total_bytes);
	return 0;
}

void usage(void)
{
	fprintf(stderr,
		"Usage: encoder [-s] [-c chann|-f freq] [seconds] [mpegfile]\n\
[-vport N]\tVideo Device Port: 0-n\n\
[-aspect N]\tAspect Ratio: 1-4\n\
[-bmode N]\tBitrate Mode: 0=var 1=const\n\
[-brate N]\tBitrate: 1000000-15000000\n\
[-bpeak N]\tPeak Bitrate: 1000000-16000000\n\
[-stream N]\tStream Type: 0-14\n\
[-frate N]\tFrame Rate: 0=30fps or 1=25fps\n\
[-fpgop N]\tGOP size\n\
[-audio N]\tAudio Bitmask in Decimal: 0-255 bitmask\n\
[-dnrmode N]\tDNR Mode: 0-3\n\
[-dnrtype N]\tDNR Type: 0-3\n\
[-dnrspat N]\tDNR Spatial: 0-16\n\
[-dnrtemp N]\tDNR Tempporal: 0-32\n\
[-pulldown N]\tPulldown: 0-1\n\
[-brightness N]\t0-255\n\
[-contrast N]\t0-127\n\
[-saturation N]\t0-127\n\
[-hue N]\t-128-128\n\
[-input N]\tInput Port: 0-8\n");

}
