/*
    Copyright (C) 2005  Jelte van der Hoek.
   
    Most stuff was shamelessly nicked from ivtvctl.c:
   
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo dot com>
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

#define IVTV_INTERNAL
#include "ivtv.h"

#include "cx25840-registers.h"
#include "cx25840-settings.h"

/* Options */
enum Option {
	OptSetDevice = 'd',
	OptHelp = 'h',
	OptListDecRegisters = 'l',
	OptSetDecRegister = 's',
	OptLast = 128
};

static char options[OptLast];

static struct option long_options[] = {
	/* Please keep in alphabetical order of the short option.
	   That makes it easier to see which options are still free. */
	{"device", required_argument, 0, OptSetDevice},
	{"help", no_argument, 0, OptHelp},
	{"list-dec-registers", no_argument, 0, OptListDecRegisters},
	{"set-dec-registers", no_argument, 0, OptSetDecRegister},
	{0, 0, 0, 0}
};

void usage(void)
{
	printf("Usage:\n");
	printf("  -h, --help         display this help message\n");
	printf("  -d, --device=<dev> use device <dev> instead of /dev/video0\n");
	printf("  -l, --list-dec-registers\n");
	printf("                     dump CX25840 settings to stdout\n");
	printf("  -s, --set-dec-register\n");
	printf("                     read CX25840 settings from stdin\n");
	exit(0);
}

__u8 cx25840_read(int fd, __u16 addr)
{
	struct ivtv_saa71xx_reg reg;
	reg.reg = addr;

	if (ioctl(fd, IVTV_IOC_G_SAA7115_REG, &reg) < 0) {
		fprintf(stderr, "ioctl IVTV_IOC_G_SAA7115_REG failed\n");
		exit(1);
	}

	return reg.val;
}

void cx25840_write(int fd, __u16 addr, __u8 value)
{
	struct ivtv_saa71xx_reg reg;
	reg.reg = addr;
	reg.val = value;

	if (ioctl(fd, IVTV_IOC_S_SAA7115_REG, &reg) < 0) {
		fprintf(stderr, "ioctl IVTV_IOC_S_SAA7115_REG failed\n");
		exit(1);
	}
}

void write_setting(int fd, __u16 setting, __u32 value)
{
	__u16 addr = CX25840_REGISTER_ADDRESS(CX25840_SETTING_REGISTER_INDEX(setting));
	int s = CX25840_SETTING_START_BIT(setting);
	int l = CX25840_SETTING_BIT_LENGTH(setting);
	__u8 byte, mask;

	while (l > 0) {
		if (s == 0 && l >= 8)
			byte = value;	// just get the low byte
		else {
			mask = (0xff >> (8 - (l > 8 ? 8 : l))) << s;
			byte = cx25840_read(fd, addr) & ~mask;
			byte |= (value << s) & mask;
		}
		cx25840_write(fd, addr++, byte);

		l -= 8 - s;
		value >>= 8 - s;
		s = 0;
	}
}

__u32 read_setting(int fd, __u16 setting)
{
	__u16 addr = CX25840_REGISTER_ADDRESS(CX25840_SETTING_REGISTER_INDEX(setting));
	int s = CX25840_SETTING_START_BIT(setting);
	int l = CX25840_SETTING_BIT_LENGTH(setting);
	int rl = (s + l + 7) / 8, i = 0;
	__u32 value = 0;

	while (rl-- > 0)
		value |= cx25840_read(fd, addr++) << (i++ * 8);

	return (value >> s) & (0xffffffff >> (32 - l));
}

void list_dec_registers(int fd)
{
	uint32_t index, value;

	for (index = 0; index < number_of_settings; index++) {
		value = read_setting(fd, index);

		value_map *ap = CX25840_SETTING_MAPPING(index);
		char *name = "\0";
		while (ap != NULL && ap->name != NULL) {
			if (ap->value == value) {
				name = ap->name;
				break;
			}
			ap++;
		}

		if (*name) {
			printf("%s=%d (0x%X) \"%s\"\n",
			       CX25840_SETTING_NAME(index), value, value, name);
		} else {
			printf("%s=%d (0x%X)\n",
			       CX25840_SETTING_NAME(index), value, value);
		}
	}
}

void set_dec_registers(int fd)
{
	FILE *fp = stdin;
	char buf[512];
	char *name, *eq;
	uint32_t index;

	while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
		name = buf;

		// skip whitespace
		while (*name == '\t' || *name == ' ') {
			name++;
		}

		// ignore empty lines
		if (*name == '\0' || *name == '\n' || *name == '/') {
			continue;
		}

		// truncate the "C" macro syntax
		if (strncmp(name, "CX25840_SET_", 12) == 0) {
			// truncate the first characters
			name += 12;
		}

		// find equals as well as '('
		eq = strpbrk(name, "=(");
		if (eq == NULL) {
			fprintf(stderr,
				"Syntax error: use ['CX25840_SET_']<setting>('('|'=')<value>: Ignoring %s\n",
				buf);
			continue;
		}
		*eq++ = '\0';

		for (index = 0; index < number_of_settings; index++) {
			if (strcmp(name, CX25840_SETTING_NAME(index)) == 0)
				break;
		}

		if (index >= number_of_settings) {
			continue;
		}

		write_setting(fd, index, strtol(eq, &eq, 0));
	}
}

int main(int argc, char **argv)
{
	char *device = "/dev/video0";

	int fd = -1;
	int i;
	char short_options[26 * 2 + 1];
	char ch;

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
		case OptHelp:
			usage();
			return 0;

		case OptSetDevice:
			device = strdup(optarg);
			break;
		}
	}

	// open the device
	if ((fd = open(device, O_RDWR)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", device,
			strerror(errno));
		exit(1);
	}

	if (options[OptListDecRegisters]) {
		list_dec_registers(fd);
	}

	if (options[OptSetDecRegister]) {
		set_dec_registers(fd);
	}
	// close the device
	close(fd);

	return 0;
}
