/*
    2.4/2.6 kernel compatibility layer
    Copyright (C) 2004  Steven Fuerst & Rob Caelers

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

#ifndef IVTV_COMPAT_H
#define IVTV_COMPAT_H

#include <linux/kernel.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#define LINUX26 1
#ifndef NEW_I2C
#define NEW_I2C
#endif
#endif

// syscalls changed in 2.6.8-rc2 so compensate
#ifdef LINUX26
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
typedef struct file * kernel_filep;
#endif
#define kernel_file_open(a, b, c) filp_open(a, b, c)

#define kernel_file_read(a, b, c, d) vfs_read(a, b, c, d)
// #define kernel_file_lseek(a, b, c) lseek(a, b, c) // DONT Do this for now
#define kernel_file_close(a) filp_close(a, NULL)
#define kernel_file_is_err(a) (IS_ERR(a))

#else
typedef int kernel_filep;

#define kernel_file_open(a, b, c) open(a, b, c)
#define kernel_file_read(a, b, c, d) read(a, b, c)
#define kernel_file_lseek(a, b, c) lseek(a, b, c)
#define kernel_file_close(a) close(a)
#define kernel_file_is_err(a) (a == -1)
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 7) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 10)
#include <linux/syscalls.h>
#define open(a, b , c) sys_open(a, b ,c)
#define close(a) sys_close(a)
#define lseek(a, b , c) sys_lseek(a, b ,c)
#define read(a, b , c) sys_read(a, b ,c)
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 7)
typedef struct file * kernel_filep;
#endif

/* Mmm, 2.6 doesn't know about this one... */
#ifndef V4L2_CAP_RADIO
#define V4L2_CAP_RADIO          0x00040000  /* is a radio device */
#endif

#ifndef LINUX26
#define i2c_get_clientdata(client)   ((client)->data)
#define i2c_set_clientdata(client,y) ((client)->data = (y))
#define i2c_get_adapdata(adap)       ((adap)->data);
#define i2c_set_adapdata(adap,y)     ((adap)->data = (y));

#include <linux/slab.h>
#include <linux/videodev.h>

extern inline struct video_device *video_device_alloc(void)
{
	struct video_device *vfd;

	vfd = kmalloc(sizeof(*vfd),GFP_KERNEL);
	if (NULL == vfd)
		return NULL;
	memset(vfd,0,sizeof(*vfd));
	return vfd;
}

extern inline void video_device_release(struct video_device *vfd)
{
	kfree(vfd);
}

#define IRQ_NONE
#define IRQ_HANDLED
#define IRQRETURN_T void
#else
#define IRQRETURN_T irqreturn_t
#endif

#ifndef __KERNEL_SYSCALLS__
#define __KERNEL_SYSCALLS__
#endif

#endif
