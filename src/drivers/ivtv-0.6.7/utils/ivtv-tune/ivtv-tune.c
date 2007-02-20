/*
   Channel/Frequency changer for V4L2 compatible video encoders
   Copyright © 2005 Tyler Trafford <ttrafford@gmail.com>
   inspired by ptune.pl from James A. Pattie <james@pcxperience.com>

   frequencies.{c,h} are © Nathan Laredo <laredo@broked.net>
   parseconfig.{c,h} are from xawtv v3.94 http://linux.bytesex.org/xawtv/

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>

#define __user
#include <linux/videodev2.h>

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "xawtv_parseconfig.h"
#include "frequencies.h"
#include "cmdline.h"

int setfreq(char *device, int frequency);
int getfreqvalue(char *freqtable, char *channel);
int getxawvalue(char *xawchannel);

int main(int argc, char *argv[])
{
	struct gengetopt_args_info opt;
	struct stat buf;
	FILE *fp = NULL;
	int i, j;
	
	char *homedir = getenv("HOME");
	char *config = (char*)malloc(strlen(homedir) + strlen("/.ivtv-tune") + 1);
	sprintf(config, "%s/.ivtv-tune", homedir);

	// create config file, if it doesn't already exist
	if (stat(config, &buf) != 0)
	{
		fp = fopen(config, "w");
		fprintf(fp, "device /dev/video0\nfreqtable us-cable\n");
		fclose(fp);
	}

	cmdline_parser(argc, argv, &opt);
	cmdline_parser_configfile(config, &opt, 0, 0, 0);
	free(config);

	if (opt.device_given)
	{
		// tune to user-supplied frequency
		if (opt.frequency_given)
		{
			return setfreq(opt.device_arg, (int)(opt.frequency_arg*1000));
		}

		// tune to user-supplied channel
		if (opt.channel_given && opt.freqtable_given)
		{
			i = getfreqvalue(opt.freqtable_arg, opt.channel_arg);

			if (!i)
			{
				printf("Invalid channel for '%s'\n", opt.freqtable_arg);
				return 1;
			}
					
			return setfreq(opt.device_arg, i);
		}

		// tune to channel which exists in ~/.xawtv
		if (opt.xawtv_given)
		{
			i = getxawvalue(opt.xawtv_arg);

			if (i < 0)
			{
				printf("Channel '%s' not defined in .xawtv\n", opt.xawtv_arg);
				return 1;
			}

			return setfreq(opt.device_arg, i);
		}
	}

	// list all channels in current frequency table
	if (opt.list_channels_given && opt.freqtable_given)
	{
		fprintf(stderr, "Channels/Frequencies (MHz) for \'%s\':\n", opt.freqtable_arg);

		for (i = 0; (chanlists[i].name != NULL) && 
				strcmp(chanlists[i].name, opt.freqtable_arg) != 0; i++);

		for (j = 0; j < chanlists[i].count; j++)
		{
			printf("%s\t%.3f\n", chanlists[i].list[j].name,
					chanlists[i].list[j].freq/1000.0);
		}

		return 0;
	} 

	// list all available frequency tables
	if (opt.list_freqtable_given)
	{
		fprintf(stderr, "Frequency Maps:\n");
		
		for (i = 0; chanlists[i].name != NULL; i++)
			printf("%s\n", chanlists[i].name);

		return 0;
	}

	cmdline_parser_print_help();
	return 0;
}

int setfreq(char *device, int frequency)
{
	struct v4l2_frequency vf;
	struct v4l2_tuner vt;
	int fd, result;

	if ((fd = open(device, O_RDWR)) < 0)
	{
		fprintf(stderr, "Failed to open %s\n", device);
		return 1;
	}

	vf.tuner = 0;
	vf.type = V4L2_TUNER_ANALOG_TV;
	vf.frequency = (frequency * 16)/1000;
	result = ioctl(fd, VIDIOC_S_FREQUENCY, &vf);

	vt.index = 0;
	result = ioctl(fd, VIDIOC_G_TUNER, &vt);

	close(fd);

	if (result < 0)
	{
		fprintf(stderr, "ioctl VIDIOC_S_FREQUENCY failed\n");
		return 1;
	}
	
	printf("%s: %.3f MHz", device, frequency/1000.0);
	if (vt.signal)
		printf("  (Signal Detected)");
	printf("\n");

	return 0;
}

int getfreqvalue(char *freqtable, char *channel)
{
	int i, j;

	for (i = 0; (chanlists[i].name != NULL) && 
			strcmp(chanlists[i].name, freqtable) != 0; i++);

	for (j = 0; (j < chanlists[i].count) &&
			strcmp(chanlists[i].list[j].name, channel) != 0; j++);

	if (j == chanlists[i].count)
		return 0;

	return chanlists[i].list[j].freq;
}

int getxawvalue(char *xawchannel)
{
	char *homedir = getenv("HOME");
	char *xawtv = (char*)malloc(strlen(homedir) + strlen("/.xawtv") + 1);
	char *channel, *freqtable;

	sprintf(xawtv, "%s/.xawtv", homedir);
	cfg_parse_file(xawtv);
	free(xawtv);

	channel = cfg_get_str(xawchannel, "channel");
	if (channel) {
		freqtable = cfg_get_str("global", "freqtab");
		return getfreqvalue(freqtable, channel);
	}

	return cfg_get_float(xawchannel, "freq") * 1000;
}
