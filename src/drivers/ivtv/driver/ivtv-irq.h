/*
    interrupt handling
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


IRQRETURN_T ivtv_irq_handler(int irq, void *dev_id, struct pt_regs *regs);
void ivtv_DMA_tasklet(unsigned long arg);
void ivtv_dec_VSYNC_tasklet(unsigned long arg);
int  ivtv_dec_DMA_done(struct ivtv *itv, struct ivtv_stream *stream);
int ivtv_ignore_DMA_req(struct ivtv *itv, u32 type);
void ivtv_sched_DMA(struct ivtv *itv);
void ivtv_sched_VBI(struct ivtv *itv, int streamtype);
