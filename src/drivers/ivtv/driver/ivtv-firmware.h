/*
    ivtv firmware functions.
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>
    Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org

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

int ivtv_firmware_init(struct ivtv *itv);
int ivtv_find_firmware_mailbox(struct ivtv *itv);
int ivtv_firmware_versions(struct ivtv *itv);
int ivtv_stop_firmware(struct ivtv *itv);
void ivtv_halt_firmware(struct ivtv *itv, int mode);
void ivtv_start_firmware(struct ivtv *itv);
int ivtv_check_firmware(struct ivtv *itv);
int ivtv_find_enc_firmware_mailbox(struct ivtv *itv);
int ivtv_find_dec_firmware_mailbox(struct ivtv *itv);
int ivtv_firmware_copy(struct ivtv *itv);
