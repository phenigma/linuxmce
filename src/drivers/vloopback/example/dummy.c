/*	dummy.c
 *
 *	Example program for using a videoloopback device in zero-copy mode.
 *	Copyright 2000 by Jeroen Vreeken (pe1rxq@amsat.org)
 *	This software is distributed under the GNU public license version 2
 *	See also the file 'COPYING'.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <linux/videodev.h>

/* all seem reasonable, or not? */
#define MAXIOCTL 1024
#define MAXWIDTH 640
#define MAXHEIGHT 480
int width;
int height;
int fmt=VIDEO_PALETTE_RGB24;
char ioctlbuf[MAXIOCTL];
int v4ldev;
char *image_out;
	

int get_frame(void)
{
	int i;
	char colour = 0;

	memset(image_out, 0x128, width*height*3);
	
	for (i=10; i<width-10; i++) {
		image_out[10*width*3+i*3]=colour++;
		image_out[10*width*3+i*3+1]=0;
		image_out[10*width*3+i*3+2]=-colour;
	}
	for (i=10; i<width-10; i++) {
		image_out[(height-10)*width*3+i*3]=colour;
		image_out[(height-10)*width*3+i*3+1]=0;
		image_out[(height-10)*width*3+i*3+2]=-colour++;
	}
	/*
	*/
	usleep(500); /* BIG XXX */
	return 0;
}

char *v4l_create (int dev, int memsize)
{
	char *map;
	
	map=mmap(0, memsize, PROT_READ|PROT_WRITE, MAP_SHARED, dev, 0);
	if ((unsigned char *)-1 == (unsigned char *)map)
		return NULL;
	return map;	
}

int v4l_ioctl(unsigned long int cmd, void *arg)
{
	int i;
	switch (cmd) {
		case VIDIOCGCAP:
		{
			struct video_capability *vidcap=arg;

			sprintf(vidcap->name, "Jeroen's dummy v4l driver");
			vidcap->type= VID_TYPE_CAPTURE;
			vidcap->channels=1;
			vidcap->audios=0;
			vidcap->maxwidth=MAXWIDTH;
			vidcap->maxheight=MAXHEIGHT;
			vidcap->minwidth=20;
			vidcap->minheight=20;
			return 0;
		}
		case VIDIOCGCHAN:
		{
			struct video_channel *vidchan= (struct video_channel *)arg;
			
			printf("VIDIOCGCHAN called\n");
			if (vidchan->channel!=0)
				;//return 1;
			vidchan->channel=0;
			vidchan->flags=0;
			vidchan->tuners=0;
			vidchan->norm=0;
			vidchan->type=VIDEO_TYPE_CAMERA;
			strcpy(vidchan->name, "Loopback");
			
			return 0;
		}
		case VIDIOCSCHAN:
		{
			int *v=arg;
			
			if (v[0]!=0)
				return 1;
			return 0;
		}
		case VIDIOCGTUNER:
		{
			struct video_tuner *v = arg;

			if(v->tuner) {
				printf("VIDIOCGTUNER: Invalid Tuner, was %d\n", v->tuner);
				//return -EINVAL;
			}
			v->tuner=0;
			strcpy(v->name, "Format");
			v->rangelow=0;
			v->rangehigh=0;
			v->flags=0;
			v->mode=VIDEO_MODE_AUTO;
			return 1;
		}
		case VIDIOCGPICT:
		{
			struct video_picture *vidpic=arg;

			vidpic->colour=0x8000;
			vidpic->hue=0x8000;
			vidpic->brightness=0x8000;
			vidpic->contrast=0x8000;
			vidpic->whiteness=0x8000;
			vidpic->depth=0x8000;
			vidpic->palette=fmt;
			return 0;
		}
		case VIDIOCSPICT:
		{
			struct video_picture *vidpic=arg;
			
			if (vidpic->palette!=fmt)
				return 1;
			return 0;
		}
		case VIDIOCGWIN:
		{
			struct video_window *vidwin=arg;

			vidwin->x=0;
			vidwin->y=0;
			vidwin->width=width;
			vidwin->height=height;
			vidwin->chromakey=0;
			vidwin->flags=0;
			vidwin->clipcount=0;
			return 0;
		}
		case VIDIOCSWIN:
		{
			struct video_window *vidwin=arg;
			
			if (vidwin->width > MAXWIDTH ||
			    vidwin->height > MAXHEIGHT )
				return 1;
			if (vidwin->flags)
				return 1;
			width=vidwin->width;
			height=vidwin->height;
			printf("new size: %dx%d\n", width, height);
			return 0;
		}
		case VIDIOCGMBUF:
		{
			struct video_mbuf *vidmbuf=arg;

			vidmbuf->size=width*height*3;
			vidmbuf->frames=1;
			for (i=0; i<vidmbuf->frames; i++)
				vidmbuf->offsets[i]=i*vidmbuf->size;
			return 0;
		}
		case VIDIOCMCAPTURE:
		{
			struct video_mmap *vidmmap=arg;

			//return 0;
			if (vidmmap->height>MAXHEIGHT ||
			    vidmmap->width>MAXWIDTH ||
			    vidmmap->format!=fmt )
				return 1;
			if (vidmmap->height!=height ||
			    vidmmap->width!=width) {
				height=vidmmap->height;
				width=vidmmap->width;
				printf("new size: %dx%d\n", width, height);
			}
			// check if 'vidmmap->frame' is valid
			// initiate capture for 'vidmmap->frame' frames
			return 0;
		}
		case VIDIOCSYNC:
		{
			//struct video_mmap *vidmmap=arg;

			// check if frames are ready.
			// wait until ready.
			get_frame();
			return 0;
		}
		default:
		{
			printf("unknown ioctl: %ld\n", cmd & 0xff);
			return 1;
		}
	}
	return 0;
}

#define VIDIOCSINVALID	_IO('v',BASE_VIDIOCPRIVATE+1)

void sighandler(int signo)
{
	int size, ret;
	unsigned long int cmd;
	struct pollfd ufds;

	if (signo!=SIGIO)
		return;
	ufds.fd=v4ldev;
	ufds.events=POLLIN;
	ufds.revents=0;
	poll(&ufds, 1, 1000);
	if (!ufds.revents & POLLIN) {
		printf("Received signal but got negative on poll?!?!?!?\n");
		return;
	}
	size=read(v4ldev, ioctlbuf, MAXIOCTL);
	if (size >= sizeof(unsigned long int)) {
		memcpy(&cmd, ioctlbuf, sizeof(unsigned long int));
		if (cmd==0) {
			printf("Client closed device\n");
			return;
		}
		ret=v4l_ioctl(cmd, ioctlbuf+sizeof(unsigned long int));
		if (ret) {
			memset(ioctlbuf+sizeof(unsigned long int), MAXIOCTL-sizeof(unsigned long int), 0xff);
			printf("ioctl %lx unsuccesfull, lets issue VIDIOCSINVALID (%x)\n", cmd, VIDIOCSINVALID);
			ioctl(v4ldev, VIDIOCSINVALID);
		} else
			ioctl(v4ldev, cmd, ioctlbuf+sizeof(unsigned long int));
	}
	return;
}

int open_vidpipe(void)
{
	int pipe;
	FILE *vloopbacks;
	char pipepath[255];
	char buffer[255];
	char *loop;
	char *input;
	char *istatus;
	char *output;
	char *ostatus;

	vloopbacks=fopen("/proc/video/vloopback/vloopbacks", "r");
	if (!vloopbacks) {
		perror ("Failed to open '/proc/video/vloopback/vloopbacks");
		return -1;
	}
	/* Read vloopback version */
	fgets(buffer, 255, vloopbacks);
	printf("%s", buffer);
	/* Read explaination line */
	fgets(buffer, 255, vloopbacks);
	while (fgets(buffer, 255, vloopbacks)) {
		if (strlen(buffer)>1) {
			buffer[strlen(buffer)-1]=0;
			loop=strtok(buffer, "\t");
			input=strtok(NULL, "\t");
			istatus=strtok(NULL, "\t");
			output=strtok(NULL, "\t");
			ostatus=strtok(NULL, "\t");
			if (istatus[0]=='-') {
				sprintf(pipepath, "/dev/%s", input);
				pipe=open(pipepath, O_RDWR);
				if (pipe>=0) {
					printf("Input: /dev/%s\n", input);
					printf("Output: /dev/%s\n", output);
					return pipe;
				}
			}
		} 
	}
	return -1;
}

int main (int argc, char **argv)
{
	if (argc != 1) {
		printf("dummy.c\n");
		printf("A example for using a video4linux loopback in zero-copy mode\n");
		printf("Written by Jeroen Vreeken, 2000\n");
		printf("Updated to vloopback API v0.82\n");
		printf("\n");
		exit(1);
	}

	/* Default startup values, nothing special */
	width=352;
	height=288;

	v4ldev=open_vidpipe();
	if (v4ldev < 0) {
		perror ("Failed to open video loopback device");
		exit(1);
	}
	image_out=v4l_create(v4ldev, MAXWIDTH*MAXHEIGHT*3);
	if (!image_out) {
		exit(1);
		perror ("Failed to set device to zero-copy mode");
	}

	signal (SIGIO, sighandler);

	printf("Listening.\n");
	while (1) {
		sleep(1000);
	}

	close (v4ldev);
	free(image_out);
	exit(0);
}
