/*
    audio dual language watch thread and decoder thread.
    Copyright (C) 2004  <name of author>
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


void ivtv_dualwatch_start_encoding(struct ivtv* itv);
void ivtv_dualwatch_stop_encoding(struct ivtv* itv);
void ivtv_dualwatch_set_enabled(struct ivtv* itv, int enabled);
void enc_work_handler(void *data);
void dec_work_handler(void *data);
void fw_work_handler(void *data);

int ivtv_dec_thread(void *arg);
int ivtv_enc_thread(void *arg);
