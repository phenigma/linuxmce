/*
 *  $Id: pvrusb2-debug.h,v 1.1 2005/11/14 13:31:24 mchehab Exp $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __PVRUSB2_DEBUG_H
#define __PVRUSB2_DEBUG_H

extern int debug;

#define pvr2_trace(msk, fmt, arg...) do {if(msk & debug) printk(KERN_INFO "pvrusb2 " fmt "\n", ##arg); } while (0)

/* These are listed in *rough* order of decreasing usefulness and
   increasing noise level. */
#define PVR2_TRACE_INFO       0x00000001  // Normal messages
#define PVR2_TRACE_ERROR_LEGS 0x00000002  // error messages
#define PVR2_TRACE_TRAP       0x00000004  // Trap & report misbehavior from app
#define PVR2_TRACE_INIT       0x00000008  // misc initialization steps
#define PVR2_TRACE_START_STOP 0x00000010  // Streaming start / stop
#define PVR2_TRACE_CTL        0x00000020  // commit of control changes
#define PVR2_TRACE_DEBUG      0x00000040  // Temporary debug code
#define PVR2_TRACE_EEPROM     0x00000080  // eeprom parsing / report
#define PVR2_TRACE_STRUCT     0x00000100  // internal struct creation
#define PVR2_TRACE_OPEN_CLOSE 0x00000200  // application open / close
#define PVR2_TRACE_CREG       0x00000400  // Main critical region entry / exit
#define PVR2_TRACE_SYSFS      0x00000800  // Sysfs driven I/O
#define PVR2_TRACE_FIRMWARE   0x00001000  // firmware upload actions
#define PVR2_TRACE_TUNER      0x00002000  // tuner operation
#define PVR2_TRACE_I2C        0x00004000  // I2C related stuff
#define PVR2_TRACE_V4LIOCTL   0x00008000  // v4l ioctl details
#define PVR2_TRACE_AUDIO      0x00010000  // audio operation
#define PVR2_TRACE_DECODER    0x00020000  // video capture operation
#define PVR2_TRACE_ENCODER    0x00040000  // mpeg2 encoder operation
#define PVR2_TRACE_BUF_POOL   0x00080000  // Track buffer pool management
#define PVR2_TRACE_BUF_FLOW   0x00100000  // Track buffer flow in system
#define PVR2_TRACE_DATA_FLOW  0x00200000  // Track data flow
#define PVR2_TRACE_DEBUGIFC   0x00400000  // Debug interface actions
#define PVR2_TRACE_GPIO       0x00800000  // GPIO state bit changes
#endif /* __PVRUSB2_HDW_INTERNAL_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
