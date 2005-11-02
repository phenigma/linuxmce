/*
    Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org

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
#include <features.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>

#define IVTV_INTERNAL
#include "ivtv.h"

/* defined in compile args */
#define VIDEO_DEVICE "/dev/video"

void usage(void);
static inline int ivtv_api_fwapi(int ivtvfd, struct ivtv_ioctl_event args);

static inline int ivtv_api_fwapi(int ivtvfd, struct ivtv_ioctl_event args)
{
	int x;
	/* Send event API command */
	if ((x = ioctl(ivtvfd, IVTV_IOC_EVENT_SETUP, &args)))
		fprintf(stderr, "ioctl: IVTV_IOC_EVENT_SETUP failed\n");
	else {
		fprintf(stderr, "returned:\n");
		fprintf(stderr, " data1: 0x%08x\n", args.api.data[0]);
		fprintf(stderr, " data2: 0x%08x\n", args.api.data[1]);
		fprintf(stderr, " data3: 0x%08x\n", args.api.data[2]);
		fprintf(stderr, " data4: 0x%08x\n", args.api.data[3]);
		fprintf(stderr, " data5: 0x%08x\n", args.api.data[4]);
		fprintf(stderr, " data6: 0x%08x\n", args.api.data[5]);
	}
	return x;
}

int main(int argc, char *argv[])
{
	int video_port = 0;
	char *video_device = NULL;
	int fdin = -1;
	int mbox_put = 0;
	uint32_t cmd = 0, mode = 0;
	uint32_t data1 = 0, data2 = 0, data3 = 0, data4 = 0;
	int i, j, x, strsize;
	struct ivtv_ioctl_event args;

	video_device = malloc(24 + 1);
	if (video_device == NULL)
		exit(1);
	snprintf(video_device, 24 + 1, "%s%d", VIDEO_DEVICE, video_port);

  /*******************/
	/* Check Arguments */
  /*******************/
	if (argc > 1) {
		for (i = 1; argv[i] && (i <= argc); i++) {
			if ((strncmp(argv[i], "-", 1) == 0
			     && (strlen(argv[i]) == 1))) {
				exit(1);
			} else if ((strncmp(argv[i], "-", 1) == 0)
				   && (argv[i][1] == '\0'))
				continue;
			else if (strncmp(argv[i], "-input", 6) == 0) {
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
				video_port = (int)atoi(var);
				snprintf(video_device, 24 + 1, "%s%d",
					 VIDEO_DEVICE, video_port);
				i++;
				continue;
			} else if (strncmp(argv[i], "-data1", 6) == 0) {
				/* Data1 */
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
				data1 = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-data2", 6) == 0) {
				/* Data2 */
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
				data2 = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-data3", 6) == 0) {
				/* Data3 */
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
				data3 = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-data4", 6) == 0) {
				/* Data4 */
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
				data4 = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-mbox_put", 9) == 0) {
				/* Input Mailbox */
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
				mbox_put = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-cmd", 4) == 0) {
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
				cmd = (int)atoi(var);
				i++;
				continue;
			} else if (strncmp(argv[i], "-enc", 4) == 0) {
				mode = 1;
			} else if (strncmp(argv[i], "-dec", 4) == 0) {
				/* Mode */
				mode = 0;
			} else if (strncmp(argv[i], "-reload", 7) == 0) {
				/* Open Video Input */
				if ((fdin = open(video_device, O_RDONLY)) < 0) {
					fprintf(stderr,
						"Failed to open %s: %s\n",
						video_device, strerror(errno));
					exit(0);
				}
				if (ioctl(fdin, IVTV_IOC_RELOAD_FW, 0) < 0)
					fprintf(stderr, "ioctl: failed\n");

				close(fdin);

				exit(0);
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
				//seconds = (int) atoi(argv[i]);
			}
		}
	} else {
		usage();
		exit(1);
	}

	snprintf(video_device, 24 + 1, "%s%d", VIDEO_DEVICE, video_port);

	/* Open Video Input */
	if ((fdin = open(video_device, O_RDONLY)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", video_device,
			strerror(errno));
		exit(0);
	}

	for (x = 0; x < IVTV_MBOX_MAX_DATA; x++) {
		args.api.data[x] = 0;
	}

	args.type = mode;
	args.mbox = mbox_put;
	args.api.cmd = cmd;
	args.api.data[0] = data1;
	args.api.data[1] = data2;
	args.api.data[2] = data3;
	args.api.data[3] = data4;
	args.api.args = 4;

	fprintf(stderr,
		"sending: 0x%08x 1. 0x%08x 2. 0x%08x 3. 0x%08x 4. 0x%08x\n",
		cmd, args.api.data[0], args.api.data[1], args.api.data[2],
		args.api.data[3]);

	x = ivtv_api_fwapi(fdin, args);

	close(fdin);

	return x;
}

void usage(void)
{
	fprintf(stderr,
		"Usage: fwapi [-enc|-dec] -mbox_put N -cmd DDD -data1 DDD -data2 DDD -data3 DDD -data4 DDD\n");
	return;
}
