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

#ifndef __KERNEL_SYSCALLS__
#define __KERNEL_SYSCALLS__
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)

/* missing from module.h in earlier 2.6 kernels */
#ifndef _LINUX_MODULE_PARAMS_H
#include <linux/moduleparam.h>
#endif

#ifdef INIT_SIGHAND
#define SIGMASK_LOCK(current)    (&current->sigmask_lock)
#else
#define SIGMASK_LOCK(current)    (&current->sighand->siglock)
#endif

#define IRQRETURN_T irqreturn_t

#else

#include <linux/videodev.h>
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
#endif

#define IRQ_NONE
#define IRQ_HANDLED
#define IRQRETURN_T void

#endif

#endif
