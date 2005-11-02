/*
    Copyright (C) 2003-2004  <author>
	
    Usage info, exit status correction, and new -window option
    Copyright (C) 2005 Adam Forsyth <agforsyth@gmail.com>

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
#include <inttypes.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "videodev2.h"
#include <sys/mman.h>

#include "ivtv.h"

typedef unsigned long long W64;
static inline W64 rdtsc()
{
	W64 t;
	asm volatile ("rdtsc":"=A" (t));
	return t;
}

inline int bit(unsigned int v, unsigned int i)
{
	return (v >> i) & 1;
}

void usage(void) {

printf("Syntax is:\n");
printf("ivtvfbctl /dev/fbX              the framebuffer device for your tuner card\n");
printf("[-help]                         this help\n");
printf("[-on|-off]                      enables / disables alpha layers\n");
printf("[-globalalpha|-noglobalalpha]   enables / disables global alpha\n");
printf("[-localalpha|-nolocalalpha]     enables / disables local alpha\n");
printf("[-flicker|-noflicker]           enables / disables the flicker reduction module\n");
printf("[-alpha<alpha>]                 sets the OSD alpha level\n");
printf("[-window left top width height] sets the screen area to use for video\n");
printf("[-osdoffset x]                  sets the x-offset (from the right) for the OSD\n");
printf("[-osdx x]                       adds x to the x-offset, negative numbers work\n");
printf("[-osdy y]                       sets the y-offset (from the bottom) for the OSD\n");
printf("[-osdstride x]                  sets the pixel stride for the OSD\n");
printf("[-osdlines y]                   sets the height of the OSD\n");
printf("[-bltcopy][-nosleep][-prepdma]\n");


}

int get_state(int fd, struct ivtvfb_ioctl_state_info *state,
	      struct ivtv_osd_coords *osd)
{
	if (ioctl(fd, IVTVFB_IOCTL_GET_STATE, state) < 0 && strerror(errno) != "Success") {
		fprintf(stderr, "IVTVFB_IOCTL_GET_STATE failed (error: %s)\n",
			strerror(errno));
		return -2;
	}

	printf("Framebuffer state:\n");
	printf("  Framebuffer state:       %s\n",
	       (state->status & IVTVFB_STATUS_ENABLED) ? "ON" : "off");
	printf("  Global alpha:            %s\n",
	       (state->
		status & IVTVFB_STATUS_GLOBAL_ALPHA) ? "ENABLED" : "disabled");
	printf("  Local alpha:             %s\n",
	       (state->
		status & IVTVFB_STATUS_LOCAL_ALPHA) ? "ENABLED" : "disabled");
	printf("  Flicker reduction:       %s\n",
	       (state->
		status & IVTVFB_STATUS_FLICKER_REDUCTION) ? "ENABLED" :
	       "disabled");
	printf("  Global alpha value:      %d\n", (int)state->alpha);

	if (ioctl(fd, IVTVFB_IOCTL_GET_ACTIVE_BUFFER, osd) < 0) {
		fprintf(stderr,
			"IVTVFB_IOCTL_GET_ACTIVE_BUFFER failed (error: %s)\n",
			strerror(errno));
		return -2;
	}

	printf("Active buffer:\n");
	printf("  Offset:                  0x%08lx\n", osd->offset);
	printf("  Maximum offset:          0x%08lx\n", osd->max_offset);
	printf("  Horizontal stride:       %d 32-bit words\n",
	       osd->pixel_stride);
	printf("  Vertical lines:          %d lines\n", osd->lines);
	printf("  Coordinates (x, y):      %d, %d\n", osd->x, osd->y);

	printf("\n");

	return 0;
}

//++MTY TODO This is specific to the CPU being tested! Get from /proc/cpuinfo, etc. instead...
#define CPU_HZ (1.6*1000*1000*1000)

int ivtv_frame_loop(int fd, int do_sleep)
{
	struct ivtvfb_ioctl_dma_host_to_ivtv_args args;
	int size = (720 * 480 * 4) /*/2 */ ;
	char *buffer = (char *)mmap(NULL, size,
				    PROT_READ | PROT_WRITE,
				    MAP_PRIVATE | MAP_ANON, 0, 0);
	int rc, i;
	int val = 0;
	int iters = 1024;
	W64 tprev, tnow, tdiff;
	float fps;
	int ret = 0;

	fprintf(stderr, "frameloop: userspace buffer 0x%08lx of %d bytes:\n",
		(unsigned long)buffer, size);
	fflush(stdout);

	fprintf(stderr,
		"Warning, you must change CPU_HZ to be accurate, currently %2.0f:\n",
		CPU_HZ);
	rc = mlock(buffer, size);
	printf("mlock rc = %d\n", rc);

	for (i = 0; i < iters; i++) {
		memset(buffer, val & 0xff, size);
		val++;
		args.source = buffer;
		args.dest_offset = 0;
		args.count = size;
		tprev = rdtsc();
		if ((ret = (ioctl(fd, IVTVFB_IOCTL_PREP_FRAME, &args) < 0  && strerror(errno) != "Success"))) {
			fprintf(stderr,
				"IVTVFB_IOCTL_PREP_DMA failed (error: %s)\n",
				strerror(errno));

			/* If an EAGAIN error, things should be fine still */
			if (ret != -EAGAIN)
				return ret;
		}
		tnow = rdtsc();
		tdiff = (tnow - tprev);
		fps = (float)CPU_HZ / (float)tdiff;
		printf("iter %d: time = %f fps\n", i, fps);
		if (do_sleep)
			usleep(33333);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	char *filename = argv[1];
	int fd, rc, i;
	int do_bltcopy = 0;
	int do_prep_dma = 0;
	int do_sleep = 1;
	int do_osd_change = 0;
	int do_state_change = 0;
	int do_window = 0;

	struct ivtvfb_ioctl_state_info state;
	struct ivtv_osd_coords osd;
	struct ivtvfb_ioctl_blt_copy_args args;
	struct ivtvfb_ioctl_set_window window;

	memset(&osd, 0, sizeof(struct ivtv_osd_coords));
	memset(&state, 0, sizeof(struct ivtvfb_ioctl_state_info));

	if (argc < 2) {
		usage();
		return -1;
	}

	if ((fd = open(filename, O_RDWR)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", filename,
			strerror(errno));
		return -2;
	}

	rc = get_state(fd, &state, &osd);
	if (rc)
		return rc;

	for (i = 2; i < argc; i++) {
		if (!strcmp(argv[i], "-help")) {
			usage();
			return 0;
		} else if (!strcmp(argv[i], "-on")) {
			state.status |= IVTVFB_STATUS_ENABLED;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-off")) {
			state.status &= ~IVTVFB_STATUS_ENABLED;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-globalalpha")) {
			state.status |= IVTVFB_STATUS_GLOBAL_ALPHA;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-noglobalalpha")) {
			state.status &= ~IVTVFB_STATUS_GLOBAL_ALPHA;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-localalpha")) {
			state.status |= IVTVFB_STATUS_LOCAL_ALPHA;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-nolocalalpha")) {
			state.status &= ~IVTVFB_STATUS_LOCAL_ALPHA;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-flicker")) {
			state.status |= IVTVFB_STATUS_FLICKER_REDUCTION;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-noflicker")) {
			state.status &= ~IVTVFB_STATUS_FLICKER_REDUCTION;
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-alpha")) {
			if (++i >= argc) {
				fprintf(stderr,
					"Missing parameter to -alpha\n");
				return -3;
			}
			state.alpha = strtol(argv[i], NULL, 0);
			do_state_change = 1;
		} else if (!strcmp(argv[i], "-osdoffset")) {
			if (++i >= argc) {
				fprintf(stderr,
					"Missing parameter to -osdoffset\n");
				return -3;
			}
			osd.offset = strtol(argv[i], NULL, 0);
			do_osd_change = 1;
		} else if (!strcmp(argv[i], "-osdstride")) {
			if (++i >= argc) {
				fprintf(stderr,
					"Missing parameter to -osdstride\n");
				return -3;
			}
			osd.pixel_stride = strtol(argv[i], NULL, 0);
			do_osd_change = 1;
		} else if (!strcmp(argv[i], "-osdlines")) {
			if (++i >= argc) {
				fprintf(stderr,
					"Missing parameter to -osdline\n");
				return -3;
			}
			osd.lines = strtol(argv[i], NULL, 0);
			do_osd_change = 1;
		} else if (!strcmp(argv[i], "-window")) {
			if (i+4 >= argc) {
				fprintf(stderr,
					"Missing parameter to -window\n");
				return -3;
			}
			int windowints[4];
			int k=++i;
			for( ; i<k+4; i++) {
				windowints[i-k] = strtol(argv[i], NULL, 0);
			}
			window.left = windowints[0];
			window.top = windowints[1];
			window.width = windowints[2];
			window.height = windowints[3];
			do_window = 1;
		} else if (!strcmp(argv[i], "-osdx")) {
			if (++i >= argc) {
				fprintf(stderr, "Missing parameter to -osdx\n");
				return -3;
			}
			osd.x = strtol(argv[i], NULL, 0);
			do_osd_change = 1;
		} else if (!strcmp(argv[i], "-osdy")) {
			if (++i >= argc) {
				fprintf(stderr, "Missing parameter to -osdy\n");
				return -3;
			}
			osd.y = strtol(argv[i], NULL, 0);
			do_osd_change = 1;
		} else if (!strcmp(argv[i], "-bltcopy")) {
			do_bltcopy = 1;
		} else if (!strcmp(argv[i], "-prepdma")) {
			do_prep_dma = 1;
		} else if (!strcmp(argv[i], "-nosleep")) {
			do_sleep = 0;
		} else {
			fprintf(stderr, "Invalid option '%s'\n", argv[i]);
			return -3;
		}
	}

	if (do_bltcopy) {
		printf("ivtvfbctl: Doing BLT copy...\n");
		fflush(stdout);
		args.x = 50;
		args.y = 50;
		args.width = 620;
		args.height = 380;
		args.source_offset = 0;
		args.source_stride = 720;
		if (ioctl(fd, IVTVFB_IOCTL_BLT_COPY, &args) < 0 && errno != 0) {
			fprintf(stderr,
				"%s: IVTVFB_IOCTL_BLT_COPY failed (error: %s)\n",
				filename, strerror(errno));
			return -3;
		}
	} else if (do_prep_dma) {
		ivtv_frame_loop(fd, do_sleep);
	} else if (do_window) {
		printf("ivtvfbctl: Setting window (left: %d top: %d width: %d height: %d)\n",
			window.left, window.top, window.width, window.height);
		fflush(stdout);
		if (ioctl(fd, IVTVFB_IOCTL_SET_WINDOW, &window) < 0 && errno != 0) {
			fprintf(stderr, "left: %d top: %d width: %d height: %d\n", window.left, window.top, window.width, window.height);
			fprintf(stderr,
				"%s: IVTVFB_IOCTL_SET_WINDOW failed (error: %s, %d)\n",
				filename, strerror(errno), errno);
			return -3;
		}
	} else {
		if (do_state_change) {
			if (ioctl(fd, IVTVFB_IOCTL_SET_STATE, &state) < 0 && errno != 0) {
				fprintf(stderr,
					"%s: IVTVFB_IOCTL_SET_STATE failed (error: %s)\n",
					filename, strerror(errno));
				return -3;
			}
		}

		if (do_osd_change) {
			if (ioctl(fd, IVTVFB_IOCTL_SET_ACTIVE_BUFFER, &osd) < 0 && errno != 0) {
				fprintf(stderr,
					"%s: IVTVFB_IOCTL_SET_ACTIVE_BUFFER failed (error: %s)\n",
					filename, strerror(errno));
				return -3;
			}
		}
	}

	rc = get_state(fd, &state, &osd);
	if (rc)
		return rc;

	close(fd);

	return 0;
}
