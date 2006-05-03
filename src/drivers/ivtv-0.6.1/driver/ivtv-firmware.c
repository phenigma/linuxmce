/*
    ivtv firmware functions.
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>
    Copyright (C) 2004  Chris Kennedy <c@groovy.org>

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
#include "ivtv-fileops.h"
#include "ivtv-gpio.h"
#include "ivtv-mailbox.h"
#include "ivtv-streams.h"
#include "ivtv-cards.h"
#include "ivtv-firmware.h"

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0)
#define FWDEV(x) &((x)->dev->dev)
#else
#define FWDEV(x) (x)->name
#endif

#include <linux/firmware.h>

static u32 ivtv_firm_search_id[] =
    { 0x12345678, 0x34567812, 0x56781234, 0x78123456 };

static int ivtv_check_enc_firmware(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	/* check enc firmware */
	if (ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, CX2341X_ENC_PING_FW,
		     &result, 0, &data[0])) {
		IVTV_DEBUG_WARN("Encoder firmware dead!\n");
		/* WARNING this creates a race (if another process is
		   in the midst of an api command).. */
		itv->enc_mbox = NULL;
		return -1;
	}
	IVTV_DEBUG_INFO("Encoder OK\n");

	return 0;
}

static int ivtv_check_dec_firmware(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	/* check dec firmware */
	if (ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_DEC_PING_FW,
		     &result, 0, &data[0])) {
		IVTV_DEBUG_WARN("Decoder firmware dead!\n");
		/* WARNING this creates a race (if another process is
		   in the midst of an api command).. */
		itv->dec_mbox = NULL;
		return -1;
	}
	IVTV_DEBUG_INFO("Decoder OK\n");

	return 0;
}

int ivtv_check_firmware(struct ivtv *itv)
{
	int enc_error, dec_error;

	IVTV_DEBUG_INFO("Checking firmware\n");
	/* fixme need to grab the semaphore to ensure that
	   there are no programs in the mailbox right now */

	/* encoder */
	enc_error = ivtv_check_enc_firmware(itv);

	/* decoder */
	if (itv->has_cx23415)
		dec_error = ivtv_check_dec_firmware(itv);
	else
		dec_error = 0;

	if (enc_error && dec_error)
		return 3;
	if (enc_error)
		return 1;
	if (dec_error)
		return 2;

	return 0;
}

static int load_fw_direct(const char *fn, char *mem, struct ivtv *itv, long size)
{
	const struct firmware *fw = NULL;
	int retval = -ENOMEM;

	if (request_firmware(&fw, fn, FWDEV(itv)) == 0) {
		int i;
		u32 *dst = (u32 *)mem;
		const u32 *src = (const u32 *)fw->data;

		if (fw->size >= size) {
			retval = size;
		} else {
			retval = fw->size;
		}
		for (i = 0; i < retval; i += 4) {
			writel(*src, dst);
			dst++;
			src++;
		}
		release_firmware(fw);
		IVTV_INFO("loaded %s firmware (%d bytes)\n", fn, retval);
	} else {
		IVTV_INFO("unable to open firmware %s\n", fn);
		IVTV_INFO("did you put the firmware in the hotplug firmware directory?\n");
	}

	return retval;
}

static int ivtv_enc_firmware_copy(struct ivtv *itv)
{
	IVTV_DEBUG_INFO("Loading encoder image\n");

	if (load_fw_direct(ivtv_efw ? ivtv_efw : CX2341X_FIRM_ENC_FILENAME,
			   (char *)(itv->enc_mem), itv, IVTV_FIRM_IMAGE_SIZE) !=
	    IVTV_FIRM_IMAGE_SIZE) {
		IVTV_DEBUG_WARN("failed loading encoder firmware\n");
		return -3;
	}
	return 0;
}

static int ivtv_dec_firmware_copy(struct ivtv *itv)
{
	IVTV_DEBUG_INFO("Loading decoder image\n");
	if (load_fw_direct(ivtv_dfw ? ivtv_dfw : CX2341X_FIRM_DEC_FILENAME,
			   (char *)(itv->dec_mem), itv,IVTV_FIRM_IMAGE_SIZE) !=
	    IVTV_FIRM_IMAGE_SIZE) {
		IVTV_DEBUG_WARN("failed loading decoder firmware\n");
		return -1;
	}
	return 0;
}

/* stops decoder firmware NICELY */
static int ivtv_stop_firmware_dec(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x = 0, sleep = 0;

	IVTV_DEBUG_INFO("Stopping decoder firmware\n");

	if (NULL != itv->dec_mbox) {
		/*halt dec firmware */
		if (itv->has_cx23415) {
			x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
				     CX2341X_DEC_HALT_FW, &result, 0,
				     &data[0]);
			if (x)
				IVTV_DEBUG_WARN(
					   "stop_fw error 4. Code %d\n", x);

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

	IVTV_DEBUG_INFO("Stopping decoder firmware\n");

	if (NULL != itv->enc_mbox) {
		/*halt enc firmware */
		x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem,
			     CX2341X_ENC_HALT_FW, &result, 0, &data[0]);
		if (x)
			IVTV_DEBUG_WARN("stop_fw error 3. Code %d\n",
				   x);

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

	IVTV_DEBUG_INFO("Preparing for firmware halt.\n");
	if (itv->has_cx23415 && (mode == 2 || mode == 3))
		x = ivtv_stop_firmware_dec(itv);
	if (mode == 1 || mode == 3)
		x += ivtv_stop_firmware_enc(itv);

	if (x > 0) {
		IVTV_DEBUG_INFO("Sleeping for 10ms\n");
		ivtv_sleep_timeout(HZ / 100, 0);
	}

	IVTV_DEBUG_INFO("Stopping VDM\n");
	writel(IVTV_CMD_VDM_STOP, (IVTV_REG_VDM + itv->reg_mem));

	IVTV_DEBUG_INFO("Stopping AO\n");
	writel(IVTV_CMD_AO_STOP, (IVTV_REG_AO + itv->reg_mem));

	IVTV_DEBUG_INFO("pinging (?) APU\n");
	writel(IVTV_CMD_APU_PING, (IVTV_REG_APU + itv->reg_mem));

	/* FIXME need to experiment and see if firmware 
	   can be halted separately */
	IVTV_DEBUG_INFO("Stopping VPU\n");
	if (!itv->has_cx23415) {
		writel(IVTV_CMD_VPU_STOP16, (IVTV_REG_VPU + itv->reg_mem));
	} else {
		writel(IVTV_CMD_VPU_STOP15, (IVTV_REG_VPU + itv->reg_mem));
	}

	IVTV_DEBUG_INFO("Resetting Hw Blocks\n");
	writel(IVTV_CMD_HW_BLOCKS_RST, (IVTV_REG_HW_BLOCKS + itv->reg_mem));

	IVTV_DEBUG_INFO("Stopping SPU\n");
	writel(IVTV_CMD_SPU_STOP, (IVTV_REG_SPU + itv->reg_mem));

	IVTV_DEBUG_INFO("Sleeping for 10ms\n");
	ivtv_sleep_timeout(HZ / 100, 0);

	IVTV_DEBUG_INFO("init Encoder SDRAM pre-charge\n");
	writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
	       (IVTV_REG_ENC_SDRAM_PRECHARGE + itv->reg_mem));

	IVTV_DEBUG_INFO("init Encoder SDRAM refresh to 1us\n");
	writel(IVTV_CMD_SDRAM_REFRESH_INIT,
	       (IVTV_REG_ENC_SDRAM_REFRESH + itv->reg_mem));

	if (itv->has_cx23415) {
		IVTV_DEBUG_INFO("init Decoder SDRAM pre-charge\n");
		writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
		       (IVTV_REG_DEC_SDRAM_PRECHARGE + itv->reg_mem));

		IVTV_DEBUG_INFO(
			   "init Decoder SDRAM refresh to 1us\n");
		writel(IVTV_CMD_SDRAM_REFRESH_INIT,
		       (IVTV_REG_DEC_SDRAM_REFRESH + itv->reg_mem));
	}

	IVTV_DEBUG_INFO("Sleeping for %dms (600 recommended)\n",
		   (int)IVTV_SDRAM_SLEEPTIME);
	ivtv_sleep_timeout(IVTV_SDRAM_SLEEPTIME, 0);
}

void ivtv_start_firmware(struct ivtv *itv)
{
	/* I guess this is read-modify-write :) */
	writel((readl(itv->reg_mem + IVTV_REG_SPU) & IVTV_MASK_SPU_ENABLE),
	       (IVTV_REG_SPU + itv->reg_mem));

	IVTV_DEBUG_WARN("Sleeping for 100 ms\n");
	ivtv_sleep_timeout(HZ / 10, 0);

	if (!itv->has_cx23415) {
		writel((readl(itv->reg_mem + IVTV_REG_VPU) &
			IVTV_MASK_VPU_ENABLE16), (IVTV_REG_VPU + itv->reg_mem));
	} else {
		writel((readl(itv->reg_mem + IVTV_REG_VPU) &
			IVTV_MASK_VPU_ENABLE15), (IVTV_REG_VPU + itv->reg_mem));
	}

	IVTV_DEBUG_WARN("Sleeping for 100 ms\n");
	ivtv_sleep_timeout(HZ / 10, 0);
}

int ivtv_find_enc_firmware_mailbox(struct ivtv *itv)
{
	u32 *searchptr, *result;
	int match = 0;

	searchptr = NULL;
	result = NULL;

	IVTV_DEBUG_INFO("Searching for encoder mailbox\n");
	searchptr = (u32 *) (IVTV_FIRM_SEARCH_ENCODER_START + itv->enc_mem);

	while (searchptr <
	       (u32 *) (IVTV_FIRM_SEARCH_ENCODER_END + itv->enc_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr + 1;	/* pointer arithmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG_INFO("match: 0x%08x at "
					   "0x%08lx. match: %d\n", *result,
					   (unsigned long)result, match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++;	/* pointer arithmetic */
				} else
					match = 0;
			}
		} else {
			if ((IVTV_DBGFLG_INFO) & ivtv_debug)
				printk(".");

		}
		if (4 == match) {
			IVTV_DEBUG_INFO("found encoder mailbox!\n");
			itv->enc_mbox = (struct ivtv_mailbox *)result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->enc_mbox == NULL) {
		IVTV_DEBUG_WARN("Encoder mailbox not found\n");
		return -ENODEV;
	}

	return 0;
}

int ivtv_find_dec_firmware_mailbox(struct ivtv *itv)
{
	u32 *searchptr, *result;
	int match = 0;

	searchptr = NULL;
	result = NULL;

	if (!itv->has_cx23415) {
		return 0;
	}

	IVTV_DEBUG_INFO("Searching for decoder mailbox\n");
	match = 0;
	searchptr = (u32 *) (IVTV_FIRM_SEARCH_DECODER_START + itv->dec_mem);

	while (searchptr <
	       (u32 *) (IVTV_FIRM_SEARCH_DECODER_END + itv->dec_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr + 1;	/* pointer arithmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG_INFO(
					   "match: 0x%08x at 0x%08lx. match: %d\n",
					   *result, (unsigned long)result,
					   match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++;	/* pointer arithmetic */
				} else
					match = 0;
			}
		} else {
			if ((IVTV_DBGFLG_INFO) & ivtv_debug)
				printk(".");
		}
		if (4 == match) {
			IVTV_DEBUG_INFO("found decoder mailbox!\n");
			itv->dec_mbox = (struct ivtv_mailbox *)result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->dec_mbox == NULL) {
		IVTV_DEBUG_WARN("Decoder mailbox not found\n");
		return -ENODEV;
	}

	return 0;
}

int ivtv_firmware_versions(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int x;

	/* Encoder */
	IVTV_DEBUG_INFO("Getting encoder firmware rev.\n");
	x = ivtv_api(itv, itv->enc_mbox, &itv->enc_msem, CX2341X_ENC_GET_VERSION,
		     &result, 0, &data[0]);
	if (x) {
		IVTV_ERR("error getting Encoder firmware version\n");
		return x;
	}
	IVTV_INFO("Encoder revision: 0x%08x\n", data[0]);

	if (data[0] != 0x02040011 && data[0] != 0x02040024 && data[0] != 0x02050032)
		IVTV_DEBUG_WARN("Encoder Firmware can be buggy, use version 0x02040011, 0x02040024 or 0x02050032.\n");

	if (!itv->has_cx23415)
		return 0;

	/* Decoder */
	IVTV_DEBUG_INFO("Getting decoder firmware rev.\n");
	x = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_DEC_GET_VERSION,
		     &result, 0, &data[0]);
	if (x) {
		IVTV_ERR("error getting Decoder firmware version\n");
		return x;
	}
	IVTV_INFO("Decoder revision: 0x%08x\n", data[0]);
	return 0;
}

int ivtv_firmware_copy(struct ivtv *itv)
{
	int ret = 0;

	ret = ivtv_enc_firmware_copy(itv);
	if (ret)
		return ret;

	if (itv->has_cx23415) {
		ret = ivtv_dec_firmware_copy(itv);
		if (ret)
			return ret;
	}

	return 0;
}

int ivtv_firmware_init(struct ivtv *itv)
{
	int x;

	/* check that we're not RE-loading firmware */
	/*  a sucessful load will have detected HW  */
	/*  mailboxes. */

	IVTV_DEBUG_INFO("Stopping VDM\n");
	writel(IVTV_CMD_VDM_STOP, (IVTV_REG_VDM + itv->reg_mem));

	IVTV_DEBUG_INFO("Stopping AO\n");
	writel(IVTV_CMD_AO_STOP, (IVTV_REG_AO + itv->reg_mem));

	IVTV_DEBUG_INFO("pinging (?) APU\n");
	writel(IVTV_CMD_APU_PING, (IVTV_REG_APU + itv->reg_mem));

	IVTV_DEBUG_INFO("Stopping VPU\n");
	if (!itv->has_cx23415) {
		writel(IVTV_CMD_VPU_STOP16, (IVTV_REG_VPU + itv->reg_mem));
	} else {
		writel(IVTV_CMD_VPU_STOP15, (IVTV_REG_VPU + itv->reg_mem));
	}

	IVTV_DEBUG_INFO("Resetting Hw Blocks\n");
	writel(IVTV_CMD_HW_BLOCKS_RST, (IVTV_REG_HW_BLOCKS + itv->reg_mem));

	IVTV_DEBUG_INFO("Stopping SPU\n");
	writel(IVTV_CMD_SPU_STOP, (IVTV_REG_SPU + itv->reg_mem));

	IVTV_DEBUG_INFO("Sleeping for 10ms\n");
	ivtv_sleep_timeout(HZ / 100, 0);

	IVTV_DEBUG_INFO("init Encoder SDRAM pre-charge\n");
	writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
	       (IVTV_REG_ENC_SDRAM_PRECHARGE + itv->reg_mem));

	IVTV_DEBUG_INFO("init Encoder SDRAM refresh to 1us\n");
	writel(IVTV_CMD_SDRAM_REFRESH_INIT,
	       (IVTV_REG_ENC_SDRAM_REFRESH + itv->reg_mem));

	IVTV_DEBUG_INFO("init Decoder SDRAM pre-charge\n");
	writel(IVTV_CMD_SDRAM_PRECHARGE_INIT,
	       (IVTV_REG_DEC_SDRAM_PRECHARGE + itv->reg_mem));

	IVTV_DEBUG_INFO("init Decoder SDRAM refresh to 1us\n");
	writel(IVTV_CMD_SDRAM_REFRESH_INIT,
	       (IVTV_REG_DEC_SDRAM_REFRESH + itv->reg_mem));

	IVTV_DEBUG_INFO("Sleeping for %dms (600 recommended)\n",
		   (int)IVTV_SDRAM_SLEEPTIME);
	ivtv_sleep_timeout(IVTV_SDRAM_SLEEPTIME, 0);

	IVTV_DEBUG_INFO("Card ready for firmware!\n");
	x = ivtv_firmware_copy(itv);
	if (x) {
		IVTV_DEBUG_WARN("Error loading firmware %d!\n", x);
		return x;
	}

	/*I guess this is read-modify-write :) */
	writel((readl(itv->reg_mem + IVTV_REG_SPU) & IVTV_MASK_SPU_ENABLE),
	       (IVTV_REG_SPU + itv->reg_mem));

	IVTV_DEBUG_INFO("Sleeping for 100 ms\n");
	ivtv_sleep_timeout(HZ / 10, 0);

	/*I guess this is read-modify-write :) */
	if (!itv->has_cx23415) {
		writel((readl(itv->reg_mem + IVTV_REG_VPU) &
			IVTV_MASK_VPU_ENABLE16), (IVTV_REG_VPU + itv->reg_mem));
	} else {
		writel((readl(itv->reg_mem + IVTV_REG_VPU) &
			IVTV_MASK_VPU_ENABLE15), (IVTV_REG_VPU + itv->reg_mem));
	}

	IVTV_DEBUG_INFO("Sleeping for 100 ms\n");
	ivtv_sleep_timeout(HZ / 10, 0);
	return 0;
}

int ivtv_find_firmware_mailbox(struct ivtv *itv)
{
	u32 *searchptr, *result;
	int match = 0;

	searchptr = NULL;
	result = NULL;

	IVTV_DEBUG_INFO("Searching for encoder mailbox\n");
	searchptr = (u32 *) (itv->enc_mem);

	while (searchptr < (u32 *) (IVTV_ENCODER_SIZE + itv->enc_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr + 1;	/* pointer arithmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG_INFO("match: 0x%08x at "
					   "0x%08lx. match: %d\n", *result,
					   (unsigned long)result, match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++;	/* pointer arithmetic */
				} else
					match = 0;
			}
		}
		if (4 == match) {
			IVTV_DEBUG_INFO("found encoder mailbox!\n");
			itv->enc_mbox = (struct ivtv_mailbox *)result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->enc_mbox == NULL)
		IVTV_DEBUG_WARN("Encoder mailbox not found\n");

	IVTV_DEBUG_INFO("Searching for decoder mailbox\n");
	match = 0;
	searchptr = (u32 *) (itv->dec_mem);

	while (searchptr < (u32 *) (IVTV_DECODER_SIZE + itv->dec_mem)) {
		if (ivtv_firm_search_id[match] == readl(searchptr)) {
			result = searchptr + 1;	/* pointer arithmetic */
			match++;
			while ((match > 0) && (match < 4)) {
				IVTV_DEBUG_INFO(
					   "match: 0x%08x at 0x%08lx. match: %d\n",
					   *result, (unsigned long)result,
					   match);
				if (ivtv_firm_search_id[match] == readl(result)) {
					match++;
					result++;	/* pointer arithmetic */
				} else
					match = 0;
			}
		}
		if (4 == match) {
			IVTV_DEBUG_INFO("found decoder mailbox!\n");
			itv->dec_mbox = (struct ivtv_mailbox *)result;
			break;
		}
		searchptr += IVTV_FIRM_SEARCH_STEP;
	}
	if (itv->dec_mbox == 0)
		IVTV_DEBUG_WARN("Decoder mailbox not found\n");

	return 0;
}


void
ivtv_init_mpeg_decoder(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	long readbytes;
	unsigned char *mem_offset;
	data[0] = 0;
	data[1] = itv->width;	/* YUV source width */
	data[2] = itv->height;
	data[3] = itv->codec.audio_bitmask & 0xffff;	/* Audio settings to use,
							   bitmap. see docs. */
	if ( ivtv_api(itv, itv->dec_mbox,
                      &itv->dec_msem,
                      CX2341X_DEC_SET_DECODER_SOURCE, &result, 4, &data[0]) != 0)
	{
		IVTV_ERR("ivtv_init_mpeg_decoder failed to set decoder source\n");
		return;
	}

	if (ivtv_vapi(itv, CX2341X_DEC_START_PLAYBACK, 2,
                      itv->dec_options.gop_offset,
                      itv->dec_options.mute_frames) != 0)
	{
		IVTV_ERR("ivtv_init_mpeg_decoder failed to start playback\n");
		return;
	}
	ivtv_api_getresult_nosleep(itv, &itv->dec_mbox[9], &result, &data[0]);
	mem_offset = itv->dec_mem+ data[1];

	if ((readbytes = load_fw_direct(IVTV_DECODE_INIT_MPEG_FILENAME, (char *)mem_offset, itv,data[2])) <= 0)
	{
		IVTV_INFO("failed to read mpeg decoder initialisation file %s\n",IVTV_DECODE_INIT_MPEG_FILENAME);
	} else
	{
		ivtv_vapi(itv, CX2341X_DEC_SCHED_DMA_FROM_HOST, 3,
                          0, readbytes, 0);
		ivtv_sleep_timeout(HZ/10,0);
	}
	ivtv_vapi(itv, CX2341X_DEC_STOP_PLAYBACK, 4,
                  0,
                  itv->dec_options.pts_low,
                  itv->dec_options.pts_hi, 1);
}
