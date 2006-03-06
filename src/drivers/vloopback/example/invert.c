/*	invert.c
 *
 *	Example program for videoloopback device.
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
#include <linux/videodev.h>


int fmt=VIDEO_PALETTE_RGB24;

int read_img=0;

char *start_capture (int dev, int width, int height)
{
        struct video_capability vid_caps;
	struct video_window vid_win;
	struct video_mbuf vid_buf;
	char *map;

	if (ioctl (dev, VIDIOCGCAP, &vid_caps) == -1) {
		perror ("ioctl (VIDIOCGCAP)");
		return (NULL);
	}
	if (vid_caps.type & VID_TYPE_MONOCHROME) fmt=VIDEO_PALETTE_GREY;
	if (ioctl (dev, VIDIOCGMBUF, &vid_buf) == -1) {
		fprintf(stderr, "no mmap falling back on read\n");
		if (ioctl (dev, VIDIOCGWIN, &vid_win)== -1) {
			perror ("ioctl VIDIOCGWIN");
			return (NULL);
		}
		vid_win.width=width;
		vid_win.height=height;
		if (ioctl (dev, VIDIOCSWIN, &vid_win)== -1) {
			perror ("ioctl VIDIOCSWIN");
			return (NULL);
		}
		read_img=1;
		map=malloc(width*height*3);
		return (map);
	}
	/* If we are going to capture greyscale we need room to blow the image up */
	if (fmt==VIDEO_PALETTE_GREY)
		map=mmap(0, vid_buf.size*3, PROT_READ|PROT_WRITE, MAP_SHARED, dev, 0);
	else
		map=mmap(0, vid_buf.size, PROT_READ|PROT_WRITE, MAP_SHARED, dev, 0);
	
	if ((unsigned char *)-1 == (unsigned char *)map)
		return (NULL);
	return map;
}

int start_pipe (int dev, int width, int height)
{
        struct video_capability vid_caps;
	struct video_window vid_win;
	struct video_picture vid_pic;

	if (ioctl (dev, VIDIOCGCAP, &vid_caps) == -1) {
		perror ("ioctl (VIDIOCGCAP)");
		return (1);
	}
	if (ioctl (dev, VIDIOCGPICT, &vid_pic)== -1) {
		perror ("ioctl VIDIOCGPICT");
		return (1);
	}
	vid_pic.palette=VIDEO_PALETTE_RGB24;
	if (ioctl (dev, VIDIOCSPICT, &vid_pic)== -1) {
		perror ("ioctl VIDIOCSPICT");
		return (1);
	}
	if (ioctl (dev, VIDIOCGWIN, &vid_win)== -1) {
		perror ("ioctl VIDIOCGWIN");
		return (1);
	}
	vid_win.width=width;
	vid_win.height=height;
	if (ioctl (dev, VIDIOCSWIN, &vid_win)== -1) {
		perror ("ioctl VIDIOCSWIN");
		return (1);
	}
	return 0;
}

char *next_capture (int dev, char *map, int width, int height)
{
	int i;
	char *grey, *rgb;
	struct video_mmap vid_mmap;

    	sigset_t    set, old;

	if (read_img) {
		if (fmt==VIDEO_PALETTE_GREY) {
			if (read(dev, map, width*height) != width*height)
				return NULL;
		} else {
			if (read(dev, map, width*height*3) != width*height*3)
				return NULL;
		}
	} else {
		vid_mmap.format=fmt;
		vid_mmap.frame=0;
		vid_mmap.width=width;
		vid_mmap.height=height;
    
        	sigemptyset (&set);	     //BTTV hates signals during IOCTL
	        sigaddset (&set, SIGCHLD);   //block SIGCHLD & SIGALRM
       		sigaddset (&set, SIGALRM);   //for the time of ioctls
        	sigprocmask (SIG_BLOCK, &set, &old);
                    
		if (ioctl(dev, VIDIOCMCAPTURE, &vid_mmap) == -1) {
	        	sigprocmask (SIG_UNBLOCK, &old, NULL);
			return (NULL);
		}
		if (ioctl(dev, VIDIOCSYNC, &vid_mmap) == -1) {
	        	sigprocmask (SIG_UNBLOCK, &old, NULL);
			return (NULL);
		}
		
        	sigprocmask (SIG_UNBLOCK, &old, NULL); //undo the signal blocking
	}
	/* Blow up a grey */
	if (fmt==VIDEO_PALETTE_GREY) {
		i=width*height;
		grey=map+i-1;
		rgb=map+i*3;
		for (; i>=0; i--, grey--) {
			*(rgb--)=*grey;
			*(rgb--)=*grey;
			*(rgb--)=*grey;
		}
	}
	return map;
}

int put_image(int dev, char *image, int width, int height)
{
	if (write(dev, image, width*height*3)!=width*height*3) {
		perror("Error writing image to pipe!");
		return 0;
	}
	return 1;
}

int main (int argc, char **argv)
{
	int i, devin, devout;
	int width;
	int height;
	char *image_out, *image_new;
	
	if (argc != 4) {
		printf("Usage:\n");
		printf("\n");
		printf("invert [input] [output] [widthxheight]\n");
		printf("\n");
		printf("example: invert /dev/video0 /dev/video1 352x288\n");
		printf("\n");
		exit(1);
	}
	sscanf(argv[3], "%dx%d", &width, &height);

	image_out=malloc(width*height*3);

	devin=open (argv[1], O_RDWR);
	if (devin < 0) {
		perror ("Failed to open input video device");
		exit(1);
	}

	devout=open (argv[2], O_RDWR);
	if (devout < 0) {
		perror ("Failed to open output video device");
		exit(1);
	}

	image_new=start_capture (devin, width, height);
	if (!image_new) {
		perror("Capture error");
		exit(1);
	}

	start_pipe(devout, width, height);

	printf("Starting video stream.\n");
	while (next_capture(devin, image_new, width, height)) {
		for (i=width*height*3; i>=0; i--) image_out[i]=-image_new[i];
		if (put_image(devout, image_out, width, height)==0)
			exit(1);
	}
	perror("You bought vaporware!");
	close (devin);
	close (devout);
	free(image_out);
	exit(0);
}
