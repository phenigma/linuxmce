/*
    ioctl system call
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

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

int ivtv_v4l2_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
int ivtv_v4l2_ioctls(struct ivtv *itv, struct ivtv_open_id *id,
	             int streamtype, unsigned int cmd, void *arg);
int ivtv_internal_ioctls(struct ivtv *itv, int streamtype, unsigned int cmd, void *arg);
int ivtv_ivtv_ioctls(struct ivtv *itv, struct ivtv_open_id *id,
		     int streamtype, unsigned int cmd, void *arg);
void ivtv_dec_VSYNC(struct ivtv *itv);
