/* cx25840ctl - Set and list registers on a CX25840 chip via i2c-dev
 * 
 * Copyright (C) 2005  Jelte van der Hoek.
 *
 * Extensively modified by Tyler Trafford <ttrafford@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "i2c-dev.h"
#include "cmdline.h"
#include "cx25840-registers.h"

static inline int cx25840_write(int fd, __u16 addr, __u8 value);
static inline __u8 cx25840_read(int fd, __u16 addr);
static inline void write_setting(int fd, __u16 setting, __u32 value);
static inline __u32 read_setting(int fd, __u16 setting);
static inline void set_registers(int fd);
static inline void list_registers(FILE *output, int fd);

int main(int argc, char **argv)
{
	struct gengetopt_args_info opt;
	char device[20] = "\0";
	char sysfile[50] = "\0";
	char name[10] = "\0";
	int address = 0x44;
	int fd = -1;
	FILE *fp, *output;
	char *pager = "less";

	cmdline_parser(argc, argv, &opt);

	if (opt.inputs_num == 0)
	{
		cmdline_parser_print_help();
		return 0;
	}

	sprintf(device, "/dev/i2c-%s", opt.inputs[0]);

	// open the device
	if ((fd = open(device, O_RDWR)) < 0)
	{
		fprintf(stderr, "Failed to open %s: %s\n", device,
			strerror(errno));
		exit(1);
	}

	sprintf(sysfile, "/sys/bus/i2c/devices/%s-%04x/name", opt.inputs[0], address);
	if ((fp = fopen(sysfile, "r")) != NULL)
	{
		fgets(name, sizeof(name), fp);
		fclose(fp);
	}

	if (strncmp(name, "cx25840", 7) != 0)
	{
		fprintf(stderr, "i2c-%s @ 0x%x is not a cx25840\n", opt.inputs[0], address);
		close(fd);
		exit(1);
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, address) < 0)
	{
		fprintf(stderr, "Error: Could not set address to 0x%x: %s\n",
			address, strerror(errno));
		exit(1);
	}

	if (opt.list_registers_given)
	{
		if (!(pager = getenv("PAGER")))
			pager = "less";

		if (!(output = popen(pager, "w")))
			list_registers(stdout, fd);
		else
		{
			list_registers(output, fd);
			pclose(output);
		}
	}

	if (opt.set_registers_given)
		set_registers(fd);

	// close the device
	close(fd);

	return 0;
}

static inline int cx25840_write(int fd, __u16 addr, __u8 value)
{
	__u8 buffer[3];
	buffer[0] = addr >> 8;
	buffer[1] = addr & 0xff;
	buffer[2] = value;
	return write(fd, buffer, 3);
}

static inline __u8 cx25840_read(int fd, __u16 addr)
{
	__u8 buffer[2];
	buffer[0] = addr >> 8;
	buffer[1] = addr & 0xff;

	if (write(fd, buffer, 2) < 2)
		return 0;

	if (read(fd, buffer, 1) < 1)
		return 0;

	return buffer[0];
}

static inline void write_setting(int fd, __u16 setting, __u32 value)
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

static inline __u32 read_setting(int fd, __u16 setting)
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

static inline void list_registers(FILE *output, int fd)
{
	__u32 index, value;

	for (index = 0; index < number_of_settings; index++)
	{
		value_map *ap = CX25840_SETTING_MAPPING(index);
		char *name = "\0";

		value = read_setting(fd, index);
		while (ap != NULL && ap->name != NULL)
		{
			if (ap->value == value)
			{
				name = ap->name;
				break;
			}
			ap++;
		}

		if (*name)
		{
			fprintf(output, "%s=%d (0x%X) \"%s\"\n",
			        CX25840_SETTING_NAME(index), value, value, name);
		}
		else
		{
			fprintf(output, "%s=%d (0x%X)\n",
			        CX25840_SETTING_NAME(index), value, value);
		}
	}
}

static inline void set_registers(int fd)
{
	FILE *fp = stdin;
	char buf[512];
	char *name, *eq;
	__u32 index;

	while (fgets(buf, sizeof(buf) - 1, fp) != NULL)
	{
		name = buf;

		// skip whitespace
		while (*name == '\t' || *name == ' ')
			name++;

		// ignore empty lines
		if (*name == '\0' || *name == '\n' || *name == '/')
			continue;

		// truncate the "C" macro syntax
		if (strncmp(name, "CX25840_SET_", 12) == 0)
			name += 12;

		// find equals as well as '('
		eq = strpbrk(name, "=(");
		if (eq == NULL)
		{
			fprintf(stderr,
				"Syntax error: use ['CX25840_SET_']<setting>('('|'=')<value>: Ignoring %s\n",
				buf);
			continue;
		}
		*eq++ = '\0';

		for (index = 0; index < number_of_settings; index++)
		{
			if (strcmp(name, CX25840_SETTING_NAME(index)) == 0)
				break;
		}

		if (index >= number_of_settings)
			continue;

		write_setting(fd, index, strtol(eq, &eq, 0));
	}
}
