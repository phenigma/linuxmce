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

/* Main Driver file for the ivtv project:
 * Driver for the iTVC15 chip.
 * Author: Kevin Thayer (nufan_wfk at yahoo.com)
 * License: GPL
 * http://www.sourceforge.net/projects/ivtv/
 */

#include "ivtv-driver.h"
#include "ivtv-fileops.h"
#include "ivtv-gpio.h"
#include "ivtv-mailbox.h"
#include "ivtv-streams.h"
#include "ivtv-cards.h"
#include "ivtv-firmware.h"

#if defined(LINUX26)
#include <linux/firmware.h>
#endif

static u32 ivtv_firm_search_id[] = {0x12345678,0x34567812,0x56781234,0x78123456};

static int ivtv_check_enc_firmware(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	/* check enc firmware */
        if (ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_ENC_PING_FW,
		     &result, 0, &data[0])) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "Encoder firmware dead!\n");
		/* WARNING this creates a race (if another process is
			in the midst of an api command).. */
		itv->enc_mbox = NULL;
		return -1;
	}
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Encoder OK\n");

	return 0;
}

static int ivtv_check_dec_firmware(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	/* check dec firmware */
        if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_DEC_PING_FW,
		     &result, 0, &data[0])) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "Decoder firmware dead!\n");
		/* WARNING this creates a race (if another process is
			in the midst of an api command).. */
		itv->dec_mbox = NULL;
		return -1;
	}
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Decoder OK\n");

	return 0;
}

int ivtv_check_firmware(struct ivtv *itv)
{
	int enc_error, dec_error;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Checking firmware\n");
	/* fixme need to grab the semaphore to ensure that
	   there are no programs in the mailbox right now */

	/* encoder */
	enc_error = ivtv_check_enc_firmware(itv);

	/* decoder - the function will return 0 if no decoder exists */
	dec_error = ivtv_check_dec_firmware(itv);

	if (enc_error && dec_error)
		return 3;
	if (enc_error)
		return 1;
	if (dec_error)
		return 2;

	return 0;
}

#if defined(LINUX26) && defined(CONFIG_FW_LOADER) && defined(IVTV_FW_LOADER)
static int load_fw_direct(const char *fn, char *mem, struct ivtv *itv)
{
	const struct firmware *fw = NULL;
	struct pci_dev *pdev = itv->dev;
	int retval = -ENOMEM;

	printk(KERN_INFO "ivtv: requesting firmware\n");
	if (request_firmware(&fw, fn, &(pdev->dev)) == 0) {
		if (fw->size <= 0 || fw->size > IVTV_FIRM_IMAGE_SIZE) {
			printk(KERN_INFO "Firmware image too large '%s'\n", fn);
		} else {
			memcpy(mem, fw->data, fw->size);
			retval = fw->size;
			release_firmware(fw);
		}
		printk(KERN_INFO "ivtv: firmware loaded\n");
	} else {
		printk(KERN_WARNING "ivtv: firmware load failed\n");
	}

	return retval;
}
#else
static int load_fw_direct(const char *fn, char *mem, struct ivtv *itv)
{
        kernel_filep filep;
        long l;
        loff_t file_offset=0;

        mm_segment_t fs = get_fs();

        set_fs(get_ds());

        filep = kernel_file_open(fn, 0, 0);
        if(kernel_file_is_err(filep)) {
                printk(KERN_INFO "Unable to open '%s'.\n", fn);
                l = -EINVAL;
                goto failure;
        }
        l = IVTV_FIRM_IMAGE_SIZE;
#ifdef kernel_file_lseek
        /* the 2 means SEEK_END */
        l = kernel_file_lseek(filep, 0L, 2);

        if (l <= 0 || l > IVTV_FIRM_IMAGE_SIZE) {
                printk(KERN_INFO "Firmware image too large '%s'\n", fn);
                l = -ENOMEM;
                goto out;
        }

        kernel_file_lseek(filep, 0L, 0);
#endif
        if (kernel_file_read(filep, mem,l,  &file_offset) != l){
                printk(KERN_INFO "Failed to read '%s'.\n", fn);
                l = -ENOMEM;
        }
#ifdef kernel_file_lseek
out:
#endif
        kernel_file_close(filep);
failure:
        set_fs(fs);

        return (int) l;
}
#endif // LINUX26

static int ivtv_enc_firmware_copy(struct ivtv *itv)
{
        IVTV_DEBUG(IVTV_DEBUG_INFO, "Loading encoder image\n");

        if (load_fw_direct(IVTV_FIRM_ENC_FILENAME,
			   (char *)(itv->enc_mem), itv) !=
			   IVTV_FIRM_IMAGE_SIZE) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "failed loading encoder firmware\n");
		return -3;
	}
	return 0;
}

static int ivtv_dec_firmware_copy(struct ivtv *itv)
{
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Loading decoder image\n");
	if (load_fw_direct(IVTV_FIRM_DEC_FILENAME,
			   (char *)(itv->dec_mem), itv) !=
			   IVTV_FIRM_IMAGE_SIZE) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "failed loading decoder firmware\n");
		return -1;
	}
	return 0;
}

/* stops decoder firmware NICELY */
static int ivtv_stop_firmware_dec(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x = 0, sleep = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping decoder firmware\n");

	if (NULL != itv->dec_mbox) {
		/*halt dec firmware */
		if (itv->has_itvc15) {
        		x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
				     IVTV_API_DEC_HALT_FW, &result, 0, &data[0]);
			if (x) IVTV_DEBUG(IVTV_DEBUG_ERR,
					  "stop_fw error 4. Code %d\n",x);
		
			sleep += 1;
		}
	}
	return sleep;

}

/* stops encoder firmware NICELY */
static int ivtv_stop_firmware_enc(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x = 0, sleep = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping decoder firmware\n");

	if (NULL != itv->enc_mbox) {
		/*halt enc firmware */
	        x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_ENC_HALT_FW,
			&result, 0, &data[0]);
		if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "stop_fw error 3. Code %d\n",x);

		sleep += 1;
	}	
		
	return sleep;
}

void ivtv_halt_firmware(struct ivtv *itv, int mode)
{
	int x = 0;

	/* check that we're not RE-loading firmware */
	/*  a sucessful load will have detected HW  */
	/*  mailboxes. */

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Preparing for firmware halt.\n");
	if (itv->has_itvc15 && (mode == 2 || mode == 3))
		x = ivtv_stop_firmware_dec(itv);
	if (mode == 1 || mode == 3)
		x += ivtv_stop_firmware_enc(itv);

	if (x > 0) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10ms\n");
		ivtv_sleep_timeout(HZ/100, 0);
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping VDM\n");
	writel(IVTV_CMD_VDM_STOP, (IVTV_REG_VDM + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping AO\n");
	writel(IVTV_CMD_AO_STOP, (IVTV_REG_AO + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "pinging (?) APU\n");
	writel(IVTV_CMD_APU_PING, (IVTV_REG_APU + itv->reg_mem));

	/* FIXME need to experiment and see if firmware 
		can be halted separately */
        IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping VPU\n");
        if (!itv->has_itvc15) {
                writel(IVTV_CMD_VPU_STOP16, (IVTV_REG_VPU + itv->reg_mem));
        }
        else {
                writel(IVTV_CMD_VPU_STOP15, (IVTV_REG_VPU + itv->reg_mem));
        }

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Resetting Hw Blocks\n");
	writel(IVTV_CMD_HW_BLOCKS_RST, (IVTV_REG_HW_BLOCKS + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping SPU\n");
	writel(IVTV_CMD_SPU_STOP, (IVTV_REG_SPU + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10ms\n");
	ivtv_sleep_timeout(HZ/100, 0);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "init Encoder SDRAM pre-charge\n");
	writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
		       (IVTV_REG_ENC_SDRAM_PRECHARGE + itv->reg_mem));
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "init Encoder SDRAM refresh to 1us\n");
	writel(IVTV_CMD_SDRAM_REFRESH_INIT,
		       (IVTV_REG_ENC_SDRAM_REFRESH + itv->reg_mem));
	
	if (itv->has_itvc15) {
		IVTV_DEBUG(IVTV_DEBUG_INFO, "init Decoder SDRAM pre-charge\n");
		writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
		       (IVTV_REG_DEC_SDRAM_PRECHARGE + itv->reg_mem));

		IVTV_DEBUG(IVTV_DEBUG_INFO, "init Decoder SDRAM refresh to 1us\n");
		writel(IVTV_CMD_SDRAM_REFRESH_INIT,
		       (IVTV_REG_DEC_SDRAM_REFRESH + itv->reg_mem));
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for %dms (600 recommended)\n",
				   (int)IVTV_SDRAM_SLEEPTIME);
	ivtv_sleep_timeout(IVTV_SDRAM_SLEEPTIME, 0);
}

void ivtv_start_firmware(struct ivtv *itv)
{

	/*I guess this is read-modify-write :)*/
	writel((readl(itv->reg_mem + IVTV_REG_SPU)&IVTV_MASK_SPU_ENABLE),
			(IVTV_REG_SPU+itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_ERR, "Sleeping for 10 ms\n");
	ivtv_sleep_timeout(HZ/10, 0);
	
	if (!itv->has_itvc15) {
		writel((readl(itv->reg_mem + IVTV_REG_VPU)&IVTV_MASK_VPU_ENABLE16),
			(IVTV_REG_VPU+itv->reg_mem));
        } else {
		writel((readl(itv->reg_mem + IVTV_REG_VPU)&IVTV_MASK_VPU_ENABLE15),
			(IVTV_REG_VPU+itv->reg_mem));
        }

	IVTV_DEBUG(IVTV_DEBUG_ERR, "Sleeping for 10 ms\n");
	ivtv_sleep_timeout(HZ/10, 0);
}

int ivtv_find_enc_firmware_mailbox(struct ivtv *itv)
{
	u32 *searchptr, *result;
	int match = 0;

	searchptr = NULL;
	result    = NULL;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Searching for encoder mailbox\n");
	searchptr =(u32 *)(IVTV_FIRM_SEARCH_ENCODER_START + itv->enc_mem);

	while (searchptr < (u32 *)(IVTV_FIRM_SEARCH_ENCODER_END + itv->enc_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr+1; /* pointer aritmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG(IVTV_DEBUG_INFO, "match: 0x%08x at "
					   "0x%08lx. match: %d\n", *result, 
					   (unsigned long)result, match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++; /* pointer aritmetic */
				}
				else 
					match = 0;
			}
		}
		else {
			if((IVTV_DEBUG_INFO)&ivtv_debug) printk(".");

		}
		if ( 4 == match ) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "found encoder mailbox!\n");
			itv->enc_mbox = (struct ivtv_mailbox *) result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->enc_mbox == NULL) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "Encoder mailbox not found\n");
		return -ENODEV;
	}

	return 0;
}

int ivtv_find_dec_firmware_mailbox(struct ivtv *itv)
{
	u32 *searchptr, *result;
	int match = 0;

	searchptr = NULL;
	result    = NULL;

	if (!itv->has_itvc15) {
		return 0;
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Searching for decoder mailbox\n");
	match = 0;
	searchptr = (u32 *)(IVTV_FIRM_SEARCH_DECODER_START + itv->dec_mem);

	while (searchptr < (u32 *)(IVTV_FIRM_SEARCH_DECODER_END + itv->dec_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr+1;  /* pointer aritmetic*/
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG(IVTV_DEBUG_INFO,
					   "match: 0x%08x at 0x%08lx. match: %d\n", 
					   *result, (unsigned long)result, match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++; /* pointer aritmetic */
				}
				else 
					match = 0;
			}
		}
		else {
			if((IVTV_DEBUG_INFO)&ivtv_debug) printk(".");
		}
		if ( 4 == match ) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "found decoder mailbox!\n");
			itv->dec_mbox = (struct ivtv_mailbox *) result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->dec_mbox == NULL) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "Decoder mailbox not found\n");
		return -ENODEV;
	}

	return 0;
}

int ivtv_firmware_versions(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x;

	/* Encoder */	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Getting encoder firmware rev.\n");
	x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_ENC_GETVER,
			&result, 0, &data[0]);
	if (x) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,
			   "error getting Encoder firmware version\n");
		return x;
	}
	IVTV_DEBUG(IVTV_DEBUG_ERR,
		   "Encoder revision: 0x%08x\n", data[0]);

        if (data[0] <= 0x02040011 /*&& itv->via_fix == 1*/) {
                //itv->dma_cfg.enc_buf_size       = 0x00020000; /* 128k */
                //itv->dma_cfg.dec_buf_size       = 0x00020000; /* 128k */
                //itv->dma_cfg.fw_dec_dma_xfer    = 0x00020000; /* 128k */;
                //itv->dma_cfg.fw_enc_dma_xfer    = (128*1024);
                //itv->dma_cfg.fw_enc_dma_type    = 0; /* Use Bytes */
                //itv->dma_cfg.dec_max_xfer       = 0x00020000;
                //itv->dma_cfg.dec_min_xfer       = 0x00020000;;
        } else
                IVTV_DEBUG(IVTV_DEBUG_ERR,
                   "Encoder Firmware is buggy, use version 0x02040011\n");


	if (!itv->has_itvc15)
		return 0;

	/* Decoder */
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Getting decoder firmware rev.\n");
	x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_DEC_GETVER,
		&result, 0, &data[0]);
	if (x) {
		IVTV_DEBUG(IVTV_DEBUG_ERR,
			   "error getting Decoder firmware version\n");
		return x;
	}
	IVTV_DEBUG(IVTV_DEBUG_ERR,
		   "Decoder revision: 0x%08x\n", data[0]);
	return 0;
}

int ivtv_firmware_copy(struct ivtv *itv)
{
        int ret=0;

        ret = ivtv_enc_firmware_copy(itv);
        if (ret)
                return ret;

        if (itv->has_itvc15) {
                ret = ivtv_dec_firmware_copy(itv);
                if (ret)
                        return ret;
        }

        return 0;
}

int ivtv_stop_firmware(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x = 0;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping firmware\n");

	if (atomic_read(&itv->capturing)) {
		x = ivtv_stop_all_captures(itv);
		if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "stop_fw error 1. Code %d\n",x);
	}
		
	/*Stop decoder_playback */
	data[0] = 1; /* 0: render last frame, 1: stop NOW! :) */
	data[1] = 0; /* "low 4 bytes of stop index" */
	data[2] = 0; /* 0: stop immedeately */
        x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_DEC_STOP_PLAYBACK,
		&result, 3, &data[0]);
	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "stop_fw error 2. Code %d\n",x);
		
	/*halt enc firmware */
        x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, IVTV_API_ENC_HALT_FW,
		&result, 0, &data[0]);
	if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "stop_fw error 3. Code %d\n",x);
		
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10ms\n");
	ivtv_sleep_timeout(HZ/100, 0);
	
	/*halt dec firmware */
	if (itv->has_itvc15) {
        	x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_DEC_HALT_FW,
			&result, 0, &data[0]);
		if (x) IVTV_DEBUG(IVTV_DEBUG_ERR, "stop_fw error 4. Code %d\n",x);
		
		IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10ms\n");
		ivtv_sleep_timeout(HZ/100, 0);
	}
	
	return 0;
}

int ivtv_firmware_init(struct ivtv *itv)
{
	int x;

	/* check that we're not RE-loading firmware */
	/*  a sucessful load will have detected HW  */
	/*  mailboxes. */

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping VDM\n");
	writel(IVTV_CMD_VDM_STOP, (IVTV_REG_VDM + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping AO\n");
	writel(IVTV_CMD_AO_STOP, (IVTV_REG_AO + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "pinging (?) APU\n");
	writel(IVTV_CMD_APU_PING, (IVTV_REG_APU + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping VPU\n");
	if (!itv->has_itvc15) {
		writel(IVTV_CMD_VPU_STOP16, (IVTV_REG_VPU + itv->reg_mem));
	}
	else {
		writel(IVTV_CMD_VPU_STOP15, (IVTV_REG_VPU + itv->reg_mem));
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Resetting Hw Blocks\n");
	writel(IVTV_CMD_HW_BLOCKS_RST, (IVTV_REG_HW_BLOCKS + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Stopping SPU\n");
	writel(IVTV_CMD_SPU_STOP, (IVTV_REG_SPU + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10ms\n");
	ivtv_sleep_timeout(HZ/100, 0);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "init Encoder SDRAM pre-charge\n");
	writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
		       (IVTV_REG_ENC_SDRAM_PRECHARGE + itv->reg_mem));
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "init Encoder SDRAM refresh to 1us\n");
	writel(IVTV_CMD_SDRAM_REFRESH_INIT,
		       (IVTV_REG_ENC_SDRAM_REFRESH + itv->reg_mem));
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "init Decoder SDRAM pre-charge\n");
	writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
		       (IVTV_REG_DEC_SDRAM_PRECHARGE + itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "init Decoder SDRAM refresh to 1us\n");
	writel(IVTV_CMD_SDRAM_REFRESH_INIT,
		       (IVTV_REG_DEC_SDRAM_REFRESH + itv->reg_mem));
	
	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for %dms (600 recommended)\n",(int)IVTV_SDRAM_SLEEPTIME);
	ivtv_sleep_timeout(IVTV_SDRAM_SLEEPTIME, 0);

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Card ready for firmware!\n");
	x = ivtv_firmware_copy(itv);
	if (x) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "Error loading firmware %d!\n", x);
		return x;
	}

	/*I guess this is read-modify-write :)*/
	writel((readl(itv->reg_mem + IVTV_REG_SPU)&IVTV_MASK_SPU_ENABLE),
			(IVTV_REG_SPU+itv->reg_mem));

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10 ms\n");
	ivtv_sleep_timeout(HZ/10, 0);
	
	/*I guess this is read-modify-write :)*/
	if (!itv->has_itvc15) {
		writel((readl(itv->reg_mem + IVTV_REG_VPU)&IVTV_MASK_VPU_ENABLE16),
			(IVTV_REG_VPU+itv->reg_mem));
	}
	else {
		writel((readl(itv->reg_mem + IVTV_REG_VPU)&IVTV_MASK_VPU_ENABLE15),
			(IVTV_REG_VPU+itv->reg_mem));
	}

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Sleeping for 10 ms\n");
	ivtv_sleep_timeout(HZ/10, 0);

	/* GPIO init */
	IVTV_DEBUG(IVTV_DEBUG_INFO, "GPIO INIT\n");
	ivtv_set_gpio_audio(itv, IVTV_GPIO_AUDIO_INIT);

	return 0;
}

int ivtv_find_firmware_mailbox(struct ivtv *itv)
{
	u32 *searchptr, *result;
	int match = 0;

	searchptr = NULL;
	result    = NULL;

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Searching for encoder mailbox\n");
	searchptr =(u32 *)(itv->enc_mem);

	while (searchptr < (u32 *)(IVTV_ENCODER_SIZE + itv->enc_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr+1; /* pointer arithmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG(IVTV_DEBUG_INFO, "match: 0x%08x at "
					   "0x%08lx. match: %d\n", *result, 
					   (unsigned long)result, match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++; /* pointer arithmetic */
				}
				else 
					match = 0;
			}
		}
		else {
			IVTV_DEBUG(IVTV_DEBUG_INFO, ".");
		}
		if ( 4 == match ) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "found encoder mailbox!\n");
			itv->enc_mbox = (struct ivtv_mailbox *) result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->enc_mbox == NULL) IVTV_DEBUG(IVTV_DEBUG_ERR, "Encoder mailbox not found\n");

	IVTV_DEBUG(IVTV_DEBUG_INFO, "Searching for decoder mailbox\n");
	match = 0;
	searchptr = (u32 *)(itv->dec_mem);

	while (searchptr < (u32 *)(IVTV_DECODER_SIZE + itv->dec_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr+1;  /* pointer aritmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG(IVTV_DEBUG_INFO, "match: 0x%08x at 0x%08lx. match: %d\n", 
					*result, (unsigned long)result, match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++; /* pointer arithmetic */
				}
				else 
					match = 0;
			}
		}
		else {
			IVTV_DEBUG(IVTV_DEBUG_INFO, ".");
		}
		if ( 4 == match ) {
			IVTV_DEBUG(IVTV_DEBUG_INFO, "found decoder mailbox!\n");
			itv->dec_mbox = (struct ivtv_mailbox *) result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->dec_mbox == 0) IVTV_DEBUG(IVTV_DEBUG_ERR, "Decoder mailbox not found\n");

	return 0;
}

