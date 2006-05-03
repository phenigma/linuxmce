/*
#
# ivtv PCM output level/sample.freq diagnostic tool
#
# Copyright (C) 2006 Maksym Veremeyenko (verem@m1stereo.tv)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# http://www.gnu.org/copyleft/gpl.html
#
*/


#include <unistd.h>
#include <features.h>           /* Uses _GNU_SOURCE to define getsubopt in stdlib.h */
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

#define samples_count (48000/10)
#define DB_MAX 50

/*
                  inf    -50  -45  -40  -35  -30  -25  -20  -15  -10   -5     0
    Freq    Level|....|....|....|....|....|....|....|....|....|....|....|....|!
|    inf|  -0.000|01234
*/



union
{
    char bytes[4]; 
    struct
    {
        long left:16;
        long right:16;
    } words;
}  __attribute__((packed)) audio_sample;

void print_usage(void)
{
    printf
    (
      "Usage:\n"
      "\tivtv-pcm-tester <pcm device>\n"
    );
};
    

int main(int argc, char** argv)
{
    int fd;
    int i,c;
    double f, period, current, peak_max = 0.0, peak_min = 65536.0;
    struct timeval t_start, t_stop;
    char line[DB_MAX+1];

    if(argc != 2)
    {
        fprintf(stderr, "Error, incorrect arguments!\n");
        print_usage();
        exit(-1);
    };

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        fprintf
        (
            stderr,
            "Error, unable to open device '%s'\n",
            argv[1]
        );
        exit(-1);
    };

    printf
    (
	"               -50  -45  -40  -35  -30  -25  -20  -15  -10   -5    0\n"
	"    Freq    Level|....|....|....|....|....|....|....|....|....|....|!\n"
    );


                                                                                 
    while(1)
    {
	/* fix time */
	gettimeofday(&t_start, NULL);
    
	/* read samples and find peaks */
	for(i = 0, peak_max = 0.0, peak_min = 65536.0; i<samples_count; i++)
	{
    	    /* read sample */
    	    if(read(fd, &audio_sample, sizeof(audio_sample) ) )
	    {
		current = (double)audio_sample.words.left;
		if (peak_max < current) peak_max = current;
		if (peak_min > current) peak_min = current;
	    }
	    else
	    {
		printf("\n");
		close(fd);
		exit(0);
    	    };
	};

/* printf("current=%lf, peak_min=%lf, peak_max=%lf\n", current, peak_min, peak_max); */

	/* fix time */
	gettimeofday(&t_stop, NULL);
	
	/* calc time */
	period = 
	    (double)(t_stop.tv_sec - t_start.tv_sec)
	    +
	    (1.0/1000000.0)*( (double)(t_stop.tv_usec - t_start.tv_usec) );
	    
	/* calc freq */
	f = 
	    (period == 0.0 ) 
	    ?
	    0
	    :
	    ((double)samples_count) / period;
	
	/* calc level */
        double l = 20.0*log( (peak_max - peak_min)/65536.0)/log(10.0);
	

	/* create line */
	for(i= 0 , c = (long)l; i<DB_MAX ; i++)
	    line[i] = ( (i-DB_MAX) < l )?'=':' ';
	line[DB_MAX] = 0;

    	/* output result */
	printf("|%7.0lf| %7.3lf|%s\r",f,l,line);
	
	fflush(stdout);
	

    };
    
    
};


