/*                                                                                  DMA handling
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

#include "ivtv-driver.h"
#include "ivtv-queue.h"
#include "ivtv-reset.h"
#include "ivtv-mailbox.h"
#include "ivtv-i2c.h"
#include "ivtv-kthreads.h"
#include "ivtv-irq.h"
#include "ivtv-video.h"

void ivtv_write_reg(u32 val, void *reg)
{
	writel(val, reg);
	readl(reg);
}

u32 ivtv_read_reg(void *reg)
{
	u32 val;

	val = readl(reg);
	return val;
}

u32 ivtv_round_dma(u32 data)
{
	if (data >= (64 * 1024))
		data = (64 * 1024);
	else if (data >= (32 * 1024))
		data = (32 * 1024);

	return data;
}
