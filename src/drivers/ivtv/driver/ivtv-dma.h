/*
    DMA handling
    Copyright (C) 2003-2004  Chris Kennedy <ckennedy at kmos org>

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

int ivtv_dma_status(struct ivtv *itv, int type, int reg, int sleep);
int ivtv_reset_stream(struct ivtv *itv, int type);
void ivtv_dma_err_reset(struct ivtv *itv);
void ivtv_dma_show_status(struct ivtv *itv, int level);
u32 ivtv_round_dma(u32 data);
void ivtv_write_reg(u32 val, void *reg);
u32 ivtv_read_reg(void *reg);

