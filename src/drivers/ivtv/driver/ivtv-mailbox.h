/*
    mailbox functions
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

int ivtv_api(struct ivtv *itv, struct ivtv_mailbox *mbox, struct semaphore *sem, int cmd,
		u32 *result, int args, u32 data[]);
int ivtv_api_getresult_nosleep(struct ivtv_mailbox *mbox, u32 *result, u32 data[]);
int ivtv_api_result(struct ivtv *itv, int cmd, int args, u32 *data);
int ivtv_vapi(struct ivtv *itv, int cmd, int args, ...);
