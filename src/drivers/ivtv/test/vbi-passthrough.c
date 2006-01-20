/*
    Sliced vbi passthrough demonstration utility
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
	char *dev_in = "/dev/vbi0";
	char *dev_out = "/dev/vbi8";
	struct v4l2_format fmt;
	struct v4l2_sliced_vbi_data *buf;
	int len;
	int fin;
	int fout;

	if (argc <= 1) {
		fprintf(stderr, "usage: %s [vbi in device] [vbi out device]\n",
			argv[0]);
		exit(1);
	}
	if (argc >= 2)
		dev_in = argv[1];
	if (argc == 3)
		dev_out = argv[2];
	fin = open(dev_in, O_RDONLY);
	fout = open(dev_out, O_WRONLY);

	if (fin == -1) {
		fprintf(stderr, "cannot open %s\n", dev_in);
		return 1;
	}
	if (fout == -1) {
		fprintf(stderr, "cannot open %s\n", dev_out);
		return 1;
	}

	fmt.fmt.sliced.service_set = V4L2_SLICED_VBI_525 | V4L2_SLICED_VBI_625;
	fmt.fmt.sliced.reserved[0] = 0;
	fmt.fmt.sliced.reserved[1] = 0;
        fmt.type = V4L2_BUF_TYPE_SLICED_VBI_OUTPUT;
	ioctl(fout, VIDIOC_S_FMT, &fmt);
	fmt.fmt.sliced.service_set = V4L2_SLICED_VBI_525 | V4L2_SLICED_VBI_625;
        fmt.type = V4L2_BUF_TYPE_SLICED_VBI_CAPTURE;
	ioctl(fin, VIDIOC_S_FMT, &fmt);
	buf = malloc(fmt.fmt.sliced.io_size);
	while ((len = read(fin, buf, fmt.fmt.sliced.io_size)) > 0) {
		if (write(fout, buf, len) != len) {
			break;
		}
	}
	close(fin);
	close(fout);
	return 0;
}
