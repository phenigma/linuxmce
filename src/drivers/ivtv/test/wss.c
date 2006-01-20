/*
    Widescreen output mode demonstration utility
    Copyright (C) 2004  Hans Verkuil  <hverkuil@xs4all.nl>

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
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <math.h>
#include <linux/types.h>

#define __user
#include "../utils/videodev2.h"

#include "ivtv.h"

int main(int argc, char **argv)
{
	char *device = "/dev/vbi8";
	struct v4l2_format fmt;
	struct v4l2_sliced_vbi_data *buf;
	int wss;
	int fh;

	if (argc <= 1) {
		fprintf(stderr, "usage: %s 0|1 [vbi device]\n", argv[0]);
		exit(1);
	}
	wss = 8 + (atol(argv[1]) ? 3 : 0);
	if (argc == 3)
		device = argv[2];
	fh = open(device, O_WRONLY);

	if (fh == -1) {
		fprintf(stderr, "cannot open %s\n", device);
		return 1;
	}

	fmt.fmt.sliced.service_set = V4L2_SLICED_WSS_625;
        fmt.type = V4L2_BUF_TYPE_SLICED_VBI_OUTPUT;
	if (ioctl(fh, VIDIOC_S_FMT, &fmt) == -1) {
		fprintf(stderr, "Set VBI mode failed\n");
		exit(1);
	}
	ioctl(fh, VIDIOC_G_FMT, &fmt);
	printf("%08x, %d\n", fmt.fmt.sliced.service_set, fmt.fmt.sliced.io_size);
	buf = malloc(fmt.fmt.sliced.io_size);
	buf[0].id = V4L2_SLICED_WSS_625;
	buf[0].line = 23;
	buf[0].field = 0;
	buf[0].data[0] = wss;
	buf[0].data[1] = 0;
	write(fh, buf, sizeof(buf[0]));
	close(fh);
	return 0;
}
