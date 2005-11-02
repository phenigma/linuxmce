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
#include <linux/unistd.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#define LINUX26 1

#ifndef NEW_I2C
#define NEW_I2C
#endif /* NEW_I2C */

/* missing from module.h in earlier 2.6 kernels */
#ifndef _LINUX_MODULE_PARAMS_H
#include <linux/moduleparam.h>
#endif /* _LINUX_MODULE_PARAMS_H */

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) */

/* syscalls changed in 2.6.8-rc2 so compensate */
#ifdef LINUX26
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
typedef struct file *kernel_filep;
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8) */
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
#endif /* LINUX26 */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 7) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 10)
#include <linux/syscalls.h>
#define open(a, b , c) sys_open(a, b ,c)
#define close(a) sys_close(a)
#define lseek(a, b , c) sys_lseek(a, b ,c)
#define read(a, b , c) sys_read(a, b ,c)
#endif /* LINUX_VERSION_CODE between 2.6.7 - 2.6.10 */

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 7)
typedef struct file *kernel_filep;
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 7) */

/* prevent the videodev2 header from being included */
#define __LINUX_VIDEODEV2_H
#include <linux/videodev.h>

#ifndef HAVE_V4L2
#define HAVE_V4L2 1
 /* Using a non-v4l2-kernel means that the v4l-compat.o module is not
    available, so we have to discard v4l compatibility. We mark that
    with this define. */
#define V4L2_NO_COMPAT
#endif /* HAVE_V4L2 */

/* For now include this version since the sliced VBI API is not yet
   available in any kernel version */
#undef __LINUX_VIDEODEV2_H
#include "../utils/videodev2.h"

#ifndef LINUX26
#define i2c_get_clientdata(client)   ((client)->data)
#define i2c_set_clientdata(client,y) ((client)->data = (y))
#define i2c_get_adapdata(adap)       ((adap)->data);
#define i2c_set_adapdata(adap,y)     ((adap)->data = (y));

#include <linux/slab.h>

extern inline struct video_device *video_device_alloc(void)
{
	struct video_device *vfd;

	vfd = kmalloc(sizeof(*vfd), GFP_KERNEL);
	if (NULL == vfd)
		return NULL;
	memset(vfd, 0, sizeof(*vfd));
	return vfd;
}

extern inline void video_device_release(struct video_device *vfd)
{
	kfree(vfd);
}

#define DEFINE_WAIT(y)			DECLARE_WAITQUEUE(y, current)
#define prepare_to_wait(x, y, z)	set_current_state(z); \
					add_wait_queue(x, y)
#define finish_wait(x, y)		remove_wait_queue(x, y); \
					set_current_state(TASK_RUNNING)

#ifdef INIT_SIGHAND
#define SIGMASK_LOCK(current)    (&(current)->sighand->siglock)
#else
#define SIGMASK_LOCK(current)    (&(current)->sigmask_lock)
#endif /* INIT_SIGHAND */

#define IRQ_NONE
#define IRQ_HANDLED
#define IRQRETURN_T void
#else

#ifdef INIT_SIGHAND
#define SIGMASK_LOCK(current)    (&current->sigmask_lock)
#else
#define SIGMASK_LOCK(current)    (&current->sighand->siglock)
#endif /* INIT_SIGHAND */

#define IRQRETURN_T irqreturn_t
#endif /* LINUX26  */

#ifndef __KERNEL_SYSCALLS__
#define __KERNEL_SYSCALLS__
#endif /* __KERNEL_SYSCALLS__ */

#ifndef I2C_NAME
#define I2C_NAME(s) (s)->name
#endif /* I2C_NAME */

#endif /* IVTV_COMPAT_H */
