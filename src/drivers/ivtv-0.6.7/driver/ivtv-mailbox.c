/*
    mailbox functions
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

#include <stdarg.h>

#include "ivtv-driver.h"
#include "ivtv-reset.h"
#include "ivtv-fileops.h"
#include "ivtv-mailbox.h"

static int ivtv_get_free_mailbox(struct ivtv *itv, int cmd, struct ivtv_mailbox *mbox, int maxnum,
				 int interrupt)
{
	int i = 0, y = 0;
	int retries = 100;
	if (NULL == mbox) {
		IVTV_DEBUG_WARN("Can't get mailbox from NULL\n");
		return -ENODEV;
	}

	/* find free mailbox */
	for (y = 0; y < retries; y++) {
		for (i = 0; i <= maxnum; i++) {
			/* Mailbox is uninitialized, lock mailbox */
			if (readl((unsigned char *)&mbox[i].flags) ==
			    0x00
			    || readl((unsigned char *)&mbox[i].flags) ==
			    0x01) {
                                if (y)
        				IVTV_DEBUG_API(
					   "got new mailbox: %d after %d tries\n",
					   i, y + 1);
				ivtv_write_reg(IVTV_MBOX_DRIVER_BUSY,
					       (unsigned char *)&mbox[i].flags);
				return i;
			}

			if (readl((unsigned char *)&mbox[i].flags) & 4) {
                                if (y)
        				IVTV_DEBUG_API(
					   "got free mailbox: %d after %d tries\n",
					   i, y + 1);
				ivtv_write_reg(IVTV_MBOX_DRIVER_BUSY,
					       (unsigned char *)&mbox[i].flags);
				return i;
			}
			/* Sleep before a retry, if not atomic */
			if (!interrupt)
				ivtv_sleep_timeout(HZ / 100, 0);
		}
		/* Sleep before a retry, if not atomic */
		if (!interrupt)
			ivtv_sleep_timeout(HZ / 100, 0);
	}

	IVTV_DEBUG_WARN(
		   "No Free Mailbox for cmd 0x%08x after %d tries!\n", cmd, y);

	/* Clear all mailboxes */
	for (i = 0; i <= maxnum; i++) {
		IVTV_DEBUG_WARN("Mailbox[%d] 0x%08x flags 0x%08x\n",
			   i, mbox[i].cmd, mbox[i].flags);
		ivtv_write_reg(0x00, (unsigned char *)&mbox[i].flags);
	}
	return -ENODEV;
}

/* This one is for stuff that can't sleep.. irq handlers, etc.. */
int ivtv_api_getresult_nosleep(struct ivtv *itv, struct ivtv_mailbox *mbox, u32 *result,
			       u32 data[])
{
	int count;

	if (NULL == mbox) {
		IVTV_DEBUG_WARN("invalid api mailbox\n");
		return -ENODEV;
	}

	for (count = 0; count < 7; count++) {
		data[count] =
		    readl((unsigned char *)&mbox->data[count]);
	}
	return 0;
}

static int ivtv_api_getresult(struct ivtv *itv, struct ivtv_mailbox *mbox, u32 * result,
			      u32 data[], int api_timeout, int cmd)
{
	u32 readdata;
	int count = 0;

	if (NULL == mbox) {
		IVTV_DEBUG_WARN("invalid api mailbox\n");
		return -ENODEV;
	}

	readdata = readl((unsigned char *)&mbox->flags);

	while (!(readdata & IVTV_MBOX_FIRMWARE_DONE)) {
                if (count)
        		IVTV_DEBUG_API(
			   "result not ready, waiting 10 ms (attempt %d)\n", count + 1);

		if (count++ > api_timeout) {
			IVTV_DEBUG_WARN(
				   "%d ms time out waiting for firmware\n",
				   api_timeout);
			return -EBUSY;
		}

		/* we want to finish this api call and not break for
		   any pending signals. */
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(HZ / 100);

		readdata = readl((unsigned char *)&mbox->flags);
	}

	*result = readl((unsigned char *)&mbox->retval);

	/* Failed, data must be bad */
	if (readl((unsigned char *)&mbox->retval) != 0x00)
		return -EBUSY;

	for (count = 0; count < IVTV_MBOX_MAX_DATA; count++)
		data[count] =
		    readl((unsigned char *)&mbox->data[count]);

	return 0;
}

#define API_ENTRY(x) { (x), #x },

static const struct {
        int cmd;
        const char *name;
} cmd_names[] = {
	/* MPEG decoder API */
	API_ENTRY(CX2341X_DEC_PING_FW)
	API_ENTRY(CX2341X_DEC_START_PLAYBACK)
	API_ENTRY(CX2341X_DEC_STOP_PLAYBACK)
	API_ENTRY(CX2341X_DEC_SET_PLAYBACK_SPEED)
	API_ENTRY(CX2341X_DEC_STEP_VIDEO)
	API_ENTRY(CX2341X_DEC_SET_DMA_BLOCK_SIZE)
	API_ENTRY(CX2341X_DEC_GET_XFER_INFO)
	API_ENTRY(CX2341X_DEC_GET_DMA_STATUS)
	API_ENTRY(CX2341X_DEC_SCHED_DMA_FROM_HOST)
	API_ENTRY(CX2341X_DEC_PAUSE_PLAYBACK)
	API_ENTRY(CX2341X_DEC_HALT_FW)
	API_ENTRY(CX2341X_DEC_SET_STANDARD)
	API_ENTRY(CX2341X_DEC_GET_VERSION)
	API_ENTRY(CX2341X_DEC_SET_STREAM_INPUT)
	API_ENTRY(CX2341X_DEC_GET_TIMING_INFO)
	API_ENTRY(CX2341X_DEC_SET_AUDIO_MODE)
	API_ENTRY(CX2341X_DEC_SET_EVENT_NOTIFICATION)
	API_ENTRY(CX2341X_DEC_SET_DISPLAY_BUFFERS)
	API_ENTRY(CX2341X_DEC_EXTRACT_VBI)
	API_ENTRY(CX2341X_DEC_SET_DECODER_SOURCE)
	API_ENTRY(CX2341X_DEC_SET_AUDIO_OUTPUT)
	API_ENTRY(CX2341X_DEC_SET_AV_DELAY)
	API_ENTRY(CX2341X_DEC_SET_PREBUFFERING)

	/* MPEG encoder API */
	API_ENTRY(CX2341X_ENC_PING_FW)
	API_ENTRY(CX2341X_ENC_START_CAPTURE)
	API_ENTRY(CX2341X_ENC_STOP_CAPTURE)
	API_ENTRY(CX2341X_ENC_SET_AUDIO_ID)
	API_ENTRY(CX2341X_ENC_SET_VIDEO_ID)
	API_ENTRY(CX2341X_ENC_SET_PCR_ID)
	API_ENTRY(CX2341X_ENC_SET_FRAME_RATE)
	API_ENTRY(CX2341X_ENC_SET_FRAME_SIZE)
	API_ENTRY(CX2341X_ENC_SET_BIT_RATE)
	API_ENTRY(CX2341X_ENC_SET_GOP_PROPERTIES)
	API_ENTRY(CX2341X_ENC_SET_ASPECT_RATIO)
	API_ENTRY(CX2341X_ENC_SET_DNR_FILTER_MODE)
	API_ENTRY(CX2341X_ENC_SET_DNR_FILTER_PROPS)
	API_ENTRY(CX2341X_ENC_SET_CORING_LEVELS)
	API_ENTRY(CX2341X_ENC_SET_SPATIAL_FILTER_TYPE)
	API_ENTRY(CX2341X_ENC_SET_3_2_PULLDOWN)
	API_ENTRY(CX2341X_ENC_SET_VBI_LINE)
	API_ENTRY(CX2341X_ENC_SET_STREAM_TYPE)
	API_ENTRY(CX2341X_ENC_SET_OUTPUT_PORT)
	API_ENTRY(CX2341X_ENC_SET_AUDIO_PROPERTIES)
	API_ENTRY(CX2341X_ENC_HALT_FW)
	API_ENTRY(CX2341X_ENC_GET_VERSION)
	API_ENTRY(CX2341X_ENC_SET_GOP_CLOSURE)
	API_ENTRY(CX2341X_ENC_GET_SEQ_END)
	API_ENTRY(CX2341X_ENC_SET_PGM_INDEX_INFO)
	API_ENTRY(CX2341X_ENC_SET_VBI_CONFIG)
	API_ENTRY(CX2341X_ENC_SET_DMA_BLOCK_SIZE)
	API_ENTRY(CX2341X_ENC_GET_PREV_DMA_INFO_MB_10)
	API_ENTRY(CX2341X_ENC_GET_PREV_DMA_INFO_MB_9)
	API_ENTRY(CX2341X_ENC_SCHED_DMA_TO_HOST)
	API_ENTRY(CX2341X_ENC_INITIALIZE_INPUT)
	API_ENTRY(CX2341X_ENC_SET_FRAME_DROP_RATE)
	API_ENTRY(CX2341X_ENC_PAUSE_ENCODER)
	API_ENTRY(CX2341X_ENC_REFRESH_INPUT)
	API_ENTRY(CX2341X_ENC_SET_COPYRIGHT)
	API_ENTRY(CX2341X_ENC_SET_EVENT_NOTIFICATION)
	API_ENTRY(CX2341X_ENC_SET_NUM_VSYNC_LINES)
	API_ENTRY(CX2341X_ENC_SET_PLACEHOLDER)
	API_ENTRY(CX2341X_ENC_MUTE_VIDEO)
	API_ENTRY(CX2341X_ENC_MUTE_AUDIO)
	API_ENTRY(CX2341X_ENC_UNKNOWN)
	API_ENTRY(CX2341X_ENC_MISC)

	/* OSD API */
	API_ENTRY(CX2341X_OSD_GET_FRAMEBUFFER)
	API_ENTRY(CX2341X_OSD_GET_PIXEL_FORMAT)
	API_ENTRY(CX2341X_OSD_SET_PIXEL_FORMAT)
	API_ENTRY(CX2341X_OSD_GET_STATE)
	API_ENTRY(CX2341X_OSD_SET_STATE)
	API_ENTRY(CX2341X_OSD_GET_OSD_COORDS)
	API_ENTRY(CX2341X_OSD_SET_OSD_COORDS)
	API_ENTRY(CX2341X_OSD_GET_SCREEN_COORDS)
	API_ENTRY(CX2341X_OSD_SET_SCREEN_COORDS)
	API_ENTRY(CX2341X_OSD_GET_GLOBAL_ALPHA)
	API_ENTRY(CX2341X_OSD_SET_GLOBAL_ALPHA)
	API_ENTRY(CX2341X_OSD_SET_BLEND_COORDS)
	API_ENTRY(CX2341X_OSD_GET_FLICKER_STATE)
	API_ENTRY(CX2341X_OSD_SET_FLICKER_STATE)
	API_ENTRY(CX2341X_OSD_BLT_COPY)
	API_ENTRY(CX2341X_OSD_BLT_FILL)
	API_ENTRY(CX2341X_OSD_BLT_TEXT)
	API_ENTRY(CX2341X_OSD_SET_FRAMEBUFFER_WINDOW)
	API_ENTRY(CX2341X_OSD_SET_CHROMA_KEY)
	API_ENTRY(CX2341X_OSD_GET_ALPHA_CONTENT_INDEX)
	API_ENTRY(CX2341X_OSD_SET_ALPHA_CONTENT_INDEX)
};

static const char *get_cmd_name(int cmd)
{
        int i;

        for (i = 0; i < sizeof(cmd_names) / sizeof(cmd_names[0]); i++) {
                if (cmd_names[i].cmd == cmd)
                        return cmd_names[i].name;
        }
        return "Unknown";
}

int ivtv_api(struct ivtv *itv, struct ivtv_mailbox *mbox, struct semaphore *sem,
	     int cmd, u32 * result, int args, u32 data[])
{
	int x = 0, gotsem = 0, needsresult = 1;
	int api_timeout = 1000;
	struct ivtv_mailbox *local_box;
	int mbox_num = 1;
	int mbox_max = 1;
	int no_reset = 0;
	int type = 0;		/* 0 = dec, 1 = osd, 2 = enc */
	int i, intr = 0;

	if (NULL == mbox) {
		IVTV_DEBUG_WARN("invalid api mailbox\n");
		return -ENODEV;
	}

	/* Check command type and validity */
	if (cmd <= DECODER_API_OFFSET)
		type = 0;
	else if (cmd <= OSD_API_OFFSET)
		type = 1;
	else if (cmd <= ENCODER_API_OFFSET)
		type = 2;
	else			/* Bad Command */
		return -ENODEV;

	/* encoder/decoder api mailboxes */
	if (type == 2) {
		mbox_max = 2;	/* Encoder, use all three, we need them */
	} else {
		mbox_max = 1;	/* Decoder, 1 for dma, 0 for normal api */
	}

	IVTV_DEBUG_API("API Call: 0x%08x (%s)\n", cmd, get_cmd_name(cmd));

	local_box = &mbox[0];	/* normal commands use mbox 1 */

	/* check args */
	if (args > IVTV_MBOX_MAX_DATA)
		return -EINVAL;

	switch (cmd) {
	case CX2341X_ENC_SCHED_DMA_TO_HOST:
	case CX2341X_DEC_SCHED_DMA_FROM_HOST:
		down(sem);
		gotsem = 1;

		needsresult = 0;
		no_reset = 1;

		mbox_num =
		    ivtv_get_free_mailbox(itv, cmd, &mbox[0], mbox_max, intr);

		/* Valid mailbox? */
		if (mbox_num >= 0 && mbox_num <= 2)
			local_box = &mbox[mbox_num];
		else
			goto ivtv_api_fail;

		/* === Write to the Mailbox === */
		ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
		ivtv_write_reg(IVTV_API_STD_TIMEOUT,
			       (unsigned char *)&local_box->timeout);

		/* Dec DMA Timestamp */
		if (cmd == CX2341X_DEC_SCHED_DMA_FROM_HOST)
			itv->dec_dma_stat.ts = jiffies;

		for (i = 0; i < args; i++) {
			ivtv_write_reg(data[i],
				       (unsigned char *)&local_box->data[i]);
		}

		ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
			       (unsigned char *)&local_box->flags);
		/* === Done with Write to the Mailbox === */

		if (needsresult)
			break;
		goto ivtv_api_done;
		/* Start/Stop capture calls for encoding are sensitive */
	case CX2341X_ENC_START_CAPTURE:
	case CX2341X_DEC_START_PLAYBACK:
	case CX2341X_ENC_STOP_CAPTURE:
	case CX2341X_ENC_SET_EVENT_NOTIFICATION:
	case CX2341X_ENC_PING_FW:
	case CX2341X_DEC_PING_FW:
		down(sem);
		gotsem = 1;

		api_timeout = 1000;
		no_reset = 0;

		/* find free mailbox */
		mbox_num = ivtv_get_free_mailbox(itv, cmd, mbox, mbox_max, intr);
		if (mbox_num >= 0 && mbox_num <= mbox_max)
			local_box = &mbox[mbox_num];
		else
			goto ivtv_api_fail;

		ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
		ivtv_write_reg(IVTV_API_STD_TIMEOUT,
			       (unsigned char *)&local_box->timeout);

		for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
			if (i < args) {
				ivtv_write_reg(data[i],
					       (unsigned char *)&local_box->
					       data[i]);
			} else
				ivtv_write_reg(0x00,
					       (unsigned char *)&local_box->
					       data[i]);
		}

		ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
			       (unsigned char *)&local_box->flags);

		break;
		/* These commands are not stored */
		/* These don't need a result */
	case CX2341X_ENC_UNKNOWN:
	case CX2341X_ENC_MISC:
	case CX2341X_OSD_BLT_COPY:
	case CX2341X_OSD_BLT_FILL:
	case CX2341X_OSD_BLT_TEXT:
	case CX2341X_DEC_SET_DMA_BLOCK_SIZE:
	case CX2341X_ENC_SET_DMA_BLOCK_SIZE:
	case CX2341X_ENC_INITIALIZE_INPUT:
	case CX2341X_ENC_REFRESH_INPUT:
		if (cmd == CX2341X_ENC_REFRESH_INPUT)
			intr = 1;
	case CX2341X_ENC_SET_NUM_VSYNC_LINES:
	case CX2341X_ENC_MUTE_VIDEO:
	case CX2341X_ENC_MUTE_AUDIO:
		needsresult = 0;

	/* These commands need a result and are not stored */
	case CX2341X_ENC_PAUSE_ENCODER:
	case CX2341X_ENC_HALT_FW:
		if (cmd == CX2341X_ENC_HALT_FW) {
			api_timeout = 100;
			no_reset = 1;
		}
	case CX2341X_ENC_GET_VERSION:
	case CX2341X_DEC_GET_VERSION:
	case CX2341X_DEC_HALT_FW:
		if (cmd == CX2341X_DEC_HALT_FW)
			api_timeout = 100;
	case CX2341X_DEC_GET_XFER_INFO:
		if (cmd == CX2341X_DEC_GET_XFER_INFO) {
			api_timeout = 1000;
			no_reset = 1;
		}
	case CX2341X_DEC_STOP_PLAYBACK:
		if (cmd == CX2341X_DEC_STOP_PLAYBACK) {
			api_timeout = 100;
			no_reset = 1;
		}
	case CX2341X_DEC_SET_PLAYBACK_SPEED:
	case CX2341X_DEC_STEP_VIDEO:
	case CX2341X_DEC_PAUSE_PLAYBACK:
	case CX2341X_DEC_GET_TIMING_INFO:
	case CX2341X_DEC_GET_DMA_STATUS:
	case CX2341X_OSD_GET_FRAMEBUFFER:
	case CX2341X_OSD_GET_GLOBAL_ALPHA:
	case CX2341X_OSD_GET_SCREEN_COORDS:
	case CX2341X_OSD_GET_OSD_COORDS:
	case CX2341X_OSD_GET_STATE:
		if (cmd == CX2341X_OSD_GET_STATE) {
			no_reset = 1;
		}
	case CX2341X_OSD_GET_PIXEL_FORMAT:
	case CX2341X_OSD_GET_ALPHA_CONTENT_INDEX:
	case CX2341X_DEC_SET_DECODER_SOURCE:
	case CX2341X_OSD_SET_GLOBAL_ALPHA:
	case CX2341X_OSD_SET_STATE:
		if (cmd == CX2341X_OSD_SET_STATE) {
			api_timeout = 1000;
			no_reset = 1;
		}
	case CX2341X_OSD_SET_FLICKER_STATE:
	case CX2341X_OSD_SET_OSD_COORDS:
	case CX2341X_OSD_SET_SCREEN_COORDS:
	case CX2341X_ENC_SET_VBI_CONFIG:
	case CX2341X_ENC_SET_VBI_LINE:
	case CX2341X_DEC_EXTRACT_VBI:
		if (!intr) {
			down(sem);
			gotsem = 1;
		}

		/* find free mailbox */
		mbox_num = ivtv_get_free_mailbox(itv, cmd, mbox, mbox_max, intr);
		if (mbox_num >= 0 && mbox_num <= mbox_max)
			local_box = &mbox[mbox_num];
		else
			goto ivtv_api_fail;

		ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
		ivtv_write_reg(IVTV_API_STD_TIMEOUT,
			       (unsigned char *)&local_box->timeout);

		for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
			if (i < args) {
				ivtv_write_reg(data[i],
					       (unsigned char *)&local_box->
					       data[i]);
			} else
				ivtv_write_reg(0x00,
					       (unsigned char *)&local_box->
					       data[i]);
		}

		ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
			       (unsigned char *)&local_box->flags);

		break;
		/* These don't need a result */
	case CX2341X_DEC_SET_AUDIO_MODE:
	case CX2341X_DEC_SET_DISPLAY_BUFFERS:
	case CX2341X_DEC_SET_AUDIO_OUTPUT:
	case CX2341X_DEC_SET_AV_DELAY:
	case CX2341X_DEC_SET_PREBUFFERING:
	case CX2341X_DEC_SET_STREAM_INPUT:
	case CX2341X_ENC_SET_PLACEHOLDER:
	case CX2341X_ENC_SET_FRAME_DROP_RATE:
	case CX2341X_ENC_SET_SPATIAL_FILTER_TYPE:
	case CX2341X_ENC_SET_CORING_LEVELS:
	case CX2341X_ENC_SET_DNR_FILTER_PROPS:
	case CX2341X_ENC_SET_DNR_FILTER_MODE:
	case CX2341X_ENC_SET_AUDIO_PROPERTIES:
	case CX2341X_ENC_SET_GOP_CLOSURE:
	case CX2341X_ENC_SET_3_2_PULLDOWN:
	case CX2341X_ENC_SET_GOP_PROPERTIES:
	case CX2341X_ENC_SET_BIT_RATE:
	case CX2341X_ENC_SET_ASPECT_RATIO:
	case CX2341X_ENC_SET_FRAME_SIZE:
	case CX2341X_ENC_SET_FRAME_RATE:
	case CX2341X_ENC_SET_OUTPUT_PORT:
	case CX2341X_ENC_SET_STREAM_TYPE:
	case CX2341X_OSD_SET_PIXEL_FORMAT:
	case CX2341X_OSD_SET_BLEND_COORDS:
	case CX2341X_OSD_SET_FRAMEBUFFER_WINDOW:
	case CX2341X_OSD_SET_CHROMA_KEY:
	case CX2341X_OSD_SET_ALPHA_CONTENT_INDEX:
		needsresult = 0;
	default:		/* Stored Commands */
		if (!intr) {
			down(sem);
			gotsem = 1;
		}

		/* Store command send/return data */
		if (itv->api[cmd].marked &&
		    ((jiffies - itv->api[cmd].jiffies) < MBOX_TIMEOUT)) {
			int stored = 1;
			/* check if same args given within timeout */
			/* if ok, return old args, or run again */
			for (i = 0; i < args; i++) {
				if (itv->api[cmd].s_data[i] == data[i]) {
					/* Data is same as before */
				} else {
					IVTV_DEBUG_API(
						   "cmd: 0x%08x arg %d "
						   "stored as 0x%08x "
						   "but changed to "
						   "0x%08x\n",
						   cmd, i,
						   itv->api[cmd].s_data[i],
						   data[i]);
					stored = 0;	/* different */
					break;
				}
			}

			if (stored) {
				if (i > 0)
					IVTV_DEBUG_API(
					   "cmd: 0x%08x arg %d stored as 0x%08x\n",
                                           cmd, i,
					   itv->api[cmd].s_data[(i - 1)]);
                                else
                                        IVTV_DEBUG_API(
                                                   "cmd: 0x%08x arg %d stored\n",
                                                   cmd, i);
				for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
					data[i] = itv->api[cmd].r_data[i];
				}

				result = 0;
				goto ivtv_api_done;
			}

			itv->api[cmd].marked = 1;	/* dirty mbox */
			itv->api[cmd].jiffies = jiffies;	/* timestamp */

			for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
				if (i < args) {
					itv->api[cmd].s_data[i] = data[i];
				} else {
					itv->api[cmd].s_data[i] = 0;
				}
			}
		} else {	/* Not Stored */
			IVTV_DEBUG_API(
				   "cmd: 0x%08x not stored\n", cmd);
			itv->api[cmd].marked = 1;	/* dirty mbox */
			itv->api[cmd].jiffies = jiffies;	/* timestamp */

			for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
				if (i < args) {
					itv->api[cmd].s_data[i] = data[i];
				} else {
					itv->api[cmd].s_data[i] = 0;
				}
			}
		}

		/* find free mailbox */
		mbox_num = ivtv_get_free_mailbox(itv, cmd, mbox, mbox_max, intr);
		if (mbox_num >= 0 && mbox_num <= mbox_max)
			local_box = &mbox[mbox_num];
		else
			goto ivtv_api_fail;

		ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
		ivtv_write_reg(IVTV_API_STD_TIMEOUT,
			       (unsigned char *)&local_box->timeout);

		for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
			if (i < args) {
				ivtv_write_reg(data[i],
					       (unsigned char *)&local_box->
					       data[i]);
			} else
				ivtv_write_reg(0x00,
					       (unsigned char *)&local_box->
					       data[i]);
		}

		ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
			       (unsigned char *)&local_box->flags);
		break;
	}

	/* Get results if needed */
	if (needsresult) {
		int count;
                u32 before;

		x = ivtv_api_getresult(itv, local_box,
				       result, &data[0], api_timeout, cmd);

		if (x == -EBUSY) {
			IVTV_DEBUG_WARN(
				   "Failed api call 0x%08x with result 0x%08x\n",
				   cmd, x);

			/* make sure cmd is not stored */
			itv->api[cmd].marked = 0;
		} else {
			/* Store results */
			for (count = 0; count < IVTV_MBOX_MAX_DATA; count++)
				itv->api[cmd].r_data[count] = data[count];
		}

                before = readl((unsigned char *)&local_box->flags);

		/* reinit results and flags */
		ivtv_write_reg(0x00, (unsigned char *)&local_box->retval);
		ivtv_write_reg(0x00, (unsigned char *)&local_box->flags);

		IVTV_DEBUG_API(
			   "Releasing mailbox (before 0x%08x, after 0x%08x)\n",
			   before, readl((unsigned char *)&local_box->flags));
	} else {
		x = 0;
	}

      ivtv_api_done:
	if (gotsem) {
		up(sem);
	}

	if (x)
		itv->api[cmd].marked = 0;	/* make sure cmd is not stored */

	return x;
      ivtv_api_fail:
	IVTV_DEBUG_WARN(
		   "Firmware UNRESPONSIVE when trying cmd 0x%08x!!!\n", cmd);
	if (gotsem) {
		up(sem);
	}

	itv->api[cmd].marked = 0;	/* make sure cmd is not stored */

	return -EBUSY;
}

static int ivtv_api_result(struct ivtv *itv, int cmd, int args, u32 * data)
{
	u32 result;

	if (cmd >= 128) {
		return ivtv_api(itv, itv->enc_mbox, &itv->enc_msem,
				cmd, &result, args, data);
	}
	return ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			cmd, &result, args, data);
}

int ivtv_vapi(struct ivtv *itv, int cmd, int args, ...)
{
	u32 data[IVTV_MBOX_MAX_DATA];
	va_list ap;
	int i;

	va_start(ap, args);
	for (i = 0; i < args; i++) {
		data[i] = va_arg(ap, u32);
	}
	va_end(ap);
	return ivtv_api_result(itv, cmd, args, data);
}
