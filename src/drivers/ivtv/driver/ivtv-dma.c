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
        writel(val,reg);
        readl(reg);
}

u32 ivtv_read_reg(void *reg)
{
        u32 val;

        val = readl(reg);
        return val;
}

/* type is 0=enc, 1=dec ; reg can be 0x00 or 0x04 or 0x10 */
/* if 0x00 then 0x00 is clear, while with 0x04 the opposite is true, 0x03 */
int ivtv_dma_status(struct ivtv *itv, int type, int reg, int sleep) {
        u32 data[IVTV_MBOX_MAX_DATA], result;
        u32 reg_result = 0;
        int ret = 0;

	/* Either check register or use API command */
	if (!sleep) {
        	/* Get DMA status from register */
        	reg_result = ivtv_read_reg((unsigned char *)itv->reg_mem + reg);
	} else {
        	/* API Check DMA Status */
        	ret = ivtv_api(itv, itv->dec_mbox,
                	&itv->dec_msem, IVTV_API_DEC_DMA_STATUS,
                                &result, 0, &data[0]);
        	/* Error with API cmd */
        	if (ret)
                	return -EIO;

        	reg_result = data[0];
	}

        /* Check if complete */
        if (type) { /* 1=Decoder */
                if (!(reg_result & IVTV_DMA_SUCCESS_READ)) {
                        ret |= IVTV_DMA_SUCCESS_READ;
                }
        } else { /* 0=Encoder */
                if (!(reg_result & IVTV_DMA_SUCCESS_WRITE)) {
                        ret |= IVTV_DMA_SUCCESS_WRITE;
                }
        }

        /* Check for errors, gather and return value */
        if (reg_result & IVTV_DMA_ERR_READ) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "DEC DMA Error. Result=0x%08x %lu\n",
                        reg_result, jiffies);
                ret |= IVTV_DMA_ERR_READ;
        }
        if (reg_result & IVTV_DMA_ERR_WRITE) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "ENC DMA Error. Result=0x%08x %lu\n",
                        reg_result, jiffies);
                ret |= IVTV_DMA_ERR_WRITE;
        }
        if (reg_result & IVTV_DMA_ERR_LIST) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                        "Link List DMA Error. Result=0x%08x %lu\n",
                        reg_result, jiffies);
                ret |= IVTV_DMA_ERR_LIST;
        }

        return ret;
}

int ivtv_reset_stream(struct ivtv *itv, int type)
{
        DECLARE_WAITQUEUE(wait, current);
        int rc = 0;
        u32 then;

        then = jiffies;
        add_wait_queue(&itv->streams[type].waitq, &wait);
        do {
                set_current_state(TASK_INTERRUPTIBLE);

                /* check DMA status register */
                if (!test_and_set_bit(IVTV_F_S_DMAP,
                                        &itv->streams[type].s_flags))
                {
                        break;
                }

		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(HZ/100);

                if ((jiffies - then) > HZ) {
                        IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"Stream  %d wait failed.\n",
				type);
                        break;
                }

                if (signal_pending(current))
                        rc = -EINTR;
        } while (!rc);
        set_current_state(TASK_RUNNING);
        remove_wait_queue(&itv->streams[type].waitq, &wait);

        if (rc)
                return -EBUSY;

        IVTV_DEBUG(IVTV_DEBUG_INFO, "Stream %d locked.\n", type);

        return 0;
}

void ivtv_dma_show_status(struct ivtv *itv, int level)
{
        u32 dma_xfer, dma_state, dec_dma_addr, enc_dma_addr, dma_control;

        /* Get DMA Status from card */
        dma_xfer = readl(itv->reg_mem + IVTV_REG_DMAXFER);
        dma_state = readl(itv->reg_mem + IVTV_REG_DMASTATUS);
        dec_dma_addr = readl(itv->reg_mem + IVTV_REG_DECDMAADDR);
        enc_dma_addr = readl(itv->reg_mem + IVTV_REG_ENCDMAADDR);
        dma_control = readl(itv->reg_mem + IVTV_REG_DMACONTROL);

        IVTV_DEBUG(level,
                "DMA Registers State: xfer: 0x%08x, "
                "state: 0x%08x dec_addr: 0x%08x "
                "enc_addr: 0x%08x control: 0x%08x\n",
                dma_xfer, dma_state, dec_dma_addr,
                enc_dma_addr, dma_control);

        IVTV_DEBUG(level,
		"DMA DEC Buffers:\n 0x%08x:0x%08x:0x%08x\n",
		readl(itv->reg_mem + IVTV_REG_DECSG1SRC),	 	
		readl(itv->reg_mem + IVTV_REG_DECSG1DST),	 	
		readl(itv->reg_mem + IVTV_REG_DECSG1LEN));

        IVTV_DEBUG(level,
		"DMA ENC Buffers:\n 0x%08x:0x%08x:0x%08x\n "
		"0x%08x:0x%08x:0x%08x\n " 	
		"0x%08x:0x%08x:0x%08x\n " 	
		"0x%08x:0x%08x:0x%08x\n " 	
		"0x%08x:0x%08x:0x%08x\n " 	
		"0x%08x:0x%08x:0x%08x\n " 	
		"0x%08x:0x%08x:0x%08x\n " 	
		"0x%08x:0x%08x:0x%08x\n",
		readl(itv->reg_mem + IVTV_REG_ENCSG1SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG1DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG1LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG2SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG2DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG2LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG3SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG3DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG3LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG4SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG4DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG4LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG5SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG5DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG5LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG6SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG6DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG6LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG7SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG7DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG7LEN),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG8SRC),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG8DST),	 	
		readl(itv->reg_mem + IVTV_REG_ENCSG8LEN));
	return;
}

void ivtv_dma_err_reset(struct ivtv *itv)
{
        u32 dma_xfer, dma_state, dec_dma_addr, enc_dma_addr, dma_control;

        /* Mask Decoder DMA Error */
        ivtv_set_irq_mask(itv, IVTV_IRQ_DEC_DMA_ERR);

        /* Get DMA Status from card */
        dma_xfer = readl(itv->reg_mem + IVTV_REG_DMAXFER);
        dma_state = readl(itv->reg_mem + IVTV_REG_DMASTATUS);
        dec_dma_addr = readl(itv->reg_mem + IVTV_REG_DECDMAADDR);
        enc_dma_addr = readl(itv->reg_mem + IVTV_REG_ENCDMAADDR);
        dma_control = readl(itv->reg_mem + IVTV_REG_DMACONTROL);

        IVTV_DEBUG(IVTV_DEBUG_ERR,
                "(%lu) Decoder DMA Error 0x%08x\n", 
		jiffies, dma_state);

        IVTV_DEBUG(IVTV_DEBUG_ERR,
                "DMA Registers Before: xfer: 0x%08x, "
                "state: 0x%08x dec_addr: 0x%08x "
                "enc_addr: 0x%08x control: 0x%08x\n",
                dma_xfer, dma_state, dec_dma_addr,
                enc_dma_addr, dma_control);

	ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

        /* Clear Dec DMA Addr */
        IVTV_DEBUG(IVTV_DEBUG_ERR, "Clearing DMA XFER REG\n");
       	writel(0x00000000, itv->reg_mem + IVTV_REG_DMAXFER);

	ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

        /* Get DMA Status from card */
        dma_xfer = readl(itv->reg_mem + IVTV_REG_DMAXFER);
        dma_state = readl(itv->reg_mem + IVTV_REG_DMASTATUS);
        dec_dma_addr = readl(itv->reg_mem + IVTV_REG_DECDMAADDR);
        enc_dma_addr = readl(itv->reg_mem + IVTV_REG_ENCDMAADDR);
        dma_control = readl(itv->reg_mem + IVTV_REG_DMACONTROL);

        IVTV_DEBUG(IVTV_DEBUG_ERR,
                "DMA Registers After: xfer: 0x%08x, "
                "state: 0x%08x dec_addr: 0x%08x "
                "enc_addr: 0x%08x control: 0x%08x\n",
                dma_xfer, dma_state, dec_dma_addr,
                enc_dma_addr, dma_control);

        return;
}

u32 ivtv_round_dma(u32 data) {
        if (data >= (1024*1024))
                data = (1024*1024);
        else if (data >= (512*1024))
                data = (512*1024);
        else if (data >= (256*1024))
                data = (256*1024);
        else if (data >= (128*1024))
                data = (128*1024);
        else if (data >= (64*1024))
                data = (64*1024);
        else if (data >= (32*1024))
                data = (32*1024);

	return data;
}


