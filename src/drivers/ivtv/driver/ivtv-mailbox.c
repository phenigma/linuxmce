/*
    mailbox functions
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

#include <stdarg.h>

#include "ivtv-driver.h"
#include "ivtv-dma.h"
#include "ivtv-reset.h"
#include "ivtv-fileops.h"
#include "ivtv-mailbox.h"

static int ivtv_get_free_mailbox(int cmd, struct ivtv_mailbox *mbox, int maxnum, int interrupt)
{
	int i = 0, y = 0;
	int retries = 100;
	if (NULL == mbox) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "Can't get mailbox from NULL\n");
		return -ENODEV;
	}

	/* find free mailbox */
	for (y = 0; y < retries; y++) {
		for (i = 0; i <= maxnum; i++) {
			/* Mailbox is uninitialized, lock mailbox */
			if (ivtv_read_reg((unsigned char *)&mbox[i].flags) == 0x00 ||
				ivtv_read_reg((unsigned char *)&mbox[i].flags) == 0x01) 
			{
				IVTV_DEBUG(IVTV_DEBUG_API, 
					"got new mailbox: %d after %d tries\n",
					i, y);
				ivtv_write_reg(IVTV_MBOX_DRIVER_BUSY, 
					(unsigned char *)&mbox[i].flags);
				ivtv_read_reg((unsigned char *)&mbox[i].flags);
				return i;
			}

			if (ivtv_read_reg((unsigned char *)&mbox[i].flags) &4) {
				IVTV_DEBUG(IVTV_DEBUG_API, 
					"got free mailbox: %d after %d tries\n",
					i, y);
				ivtv_write_reg(IVTV_MBOX_DRIVER_BUSY, 
					(unsigned char *)&mbox[i].flags);
				ivtv_read_reg((unsigned char *)&mbox[i].flags);
				return i;
			}
			/* Sleep before a retry, if not atomic */
			if (!interrupt)
				ivtv_sleep_timeout(HZ/100, 0);
		}
		/* Sleep before a retry, if not atomic */
		if (!interrupt)
			ivtv_sleep_timeout(HZ/100, 0);
	}

	IVTV_DEBUG(IVTV_DEBUG_ERR, 
		"No Free Mailbox for cmd 0x%08x after %d tries!\n", cmd, y);

	/* Clear all mailboxes */
	for (i = 0; i <= maxnum; i++) {
  		IVTV_DEBUG(IVTV_DEBUG_ERR, "Mailbox[%d] 0x%08x flags 0x%08x\n", 
  			i, mbox[i].cmd, mbox[i].flags);
		ivtv_write_reg(0x00, (unsigned char *)&mbox[i].flags);
		ivtv_read_reg((unsigned char *)&mbox[i].flags);
	}
	return -ENODEV;
}

/* This one is for stuff that can't sleep.. irq handlers, etc.. */
int ivtv_api_getresult_nosleep(struct ivtv_mailbox *mbox, u32 *result, u32 data[])
{
	u32 readdata;
	int count = 0;

	if (NULL == mbox) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "invalid api mailbox\n");
		return -ENODEV;
	}

	readdata = ivtv_read_reg((unsigned char *)&mbox->flags);

	*result = ivtv_read_reg((unsigned char *)&mbox->retval);

	/* Failed, data must be bad */
	if (ivtv_read_reg((unsigned char *)&mbox->retval) != 0x00)
		return -EBUSY;

	for ( count=0; count < IVTV_MBOX_MAX_DATA; count++)
		data[count] = ivtv_read_reg((unsigned char *)&mbox->data[count]);

	return 0;
}

static int ivtv_api_getresult(struct ivtv_mailbox *mbox, u32 *result, u32 data[],
			 int api_timeout, int intr, int cmd)
{
	u32 readdata;
	int count = 0;

	if (NULL == mbox) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "invalid api mailbox\n");
		return -ENODEV;
	}

	readdata = ivtv_read_reg((unsigned char *)&mbox->flags);

	while (!(readdata & IVTV_MBOX_FIRMWARE_DONE)) {
		IVTV_DEBUG(IVTV_DEBUG_API,
			   "[%d]result not ready, waiting 10 ms\n",
			   count);

		if (count++ > api_timeout) {
			IVTV_DEBUG(IVTV_DEBUG_ERR,
				   "%d ms time out waiting for firmware\n",
				   api_timeout);
			return -EBUSY;
		}

		/* we want to finish this api call and not break for
	   		any pending signals. */
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(HZ / 100);

		readdata = ivtv_read_reg((unsigned char *)&mbox->flags);
	}

	*result = ivtv_read_reg((unsigned char *)&mbox->retval);

	/* Failed, data must be bad */
	if (ivtv_read_reg((unsigned char *)&mbox->retval) != 0x00)
		return -EBUSY;

	for ( count=0; count < IVTV_MBOX_MAX_DATA; count++)
		data[count] = ivtv_read_reg((unsigned char *)&mbox->data[count]);

	return 0;
}

int ivtv_api(struct ivtv *itv, struct ivtv_mailbox *mbox, struct semaphore *sem, int cmd,
		u32 *result, int args, u32 data[]) {
	int x = 0, gotsem = 0, needsresult = 1;
	int api_timeout = 1000;
	struct ivtv_mailbox *local_box;
	int mbox_num = 1; int mbox_max = 1;
	int no_reset = 0;
   	int type = 0; /* 0 = dec, 1 = osd, 2 = enc */
	int i, intr = 1;

	if (NULL == mbox) {
		IVTV_DEBUG(IVTV_DEBUG_ERR, "invalid api mailbox\n");
		return -ENODEV;
	}

	/* Check command type and validity */
	if (cmd <= DECODER_API_OFFSET)
		type = 0;
	else if (cmd <= OSD_API_OFFSET)
		type = 1;
	else if (cmd <= ENCODER_API_OFFSET)
		type = 2;	
	else /* Bad Command */
		return -ENODEV;

	/* encoder/decoder api mailboxes */
	if (type == 2 ) {
		mbox_max = 2; /* Encoder, use all three, we need them */
	} else {
		mbox_max = 1; /* Decoder, 1 for dma, 0 for normal api */
	}

	IVTV_DEBUG(IVTV_DEBUG_API, "API Call: 0x%08x\n", cmd);

	local_box = &mbox[0]; /* normal commands use mbox 1 */

	/* check args */
	if (args > IVTV_MBOX_MAX_DATA) return -EINVAL;
	
	switch (cmd) {
		case IVTV_API_SCHED_DMA_TO_HOST:
		case IVTV_API_DEC_DMA_FROM_HOST:
		case IVTV_API_OSD_DMA_FROM_HOST:
			needsresult = 0;
			no_reset = 1;

			mbox_num = 1;

			/* Decoder and OSD are outside interrupt time */
			if (cmd == IVTV_API_OSD_DMA_FROM_HOST ||
				cmd == IVTV_API_DEC_DMA_FROM_HOST) 
			{
				intr = 0;
				mbox_max = 0;
				down(sem);
				gotsem	= 1;
				/* We don't wait for these anymore */
				if (cmd == IVTV_API_OSD_DMA_FROM_HOST) {
					api_timeout = 
						((data[1]*PAGE_SIZE) / OSD_API_TIMEOUT);
					if (api_timeout > 1000) 
						api_timeout = 1000;
					else if (api_timeout < 100) 
						api_timeout = 100;

					if (atomic_read(&itv->decoding) > 0)
						needsresult = OSD_API_WAIT;
				} else {
					api_timeout = 
						(data[1] / DEC_API_TIMEOUT);
					if (api_timeout > 2000)
						api_timeout = 2000;
					else if (api_timeout < 1000)
						api_timeout = 1000;

					needsresult = DEC_API_WAIT;
				}
				no_reset = 1;

				/* Change OSD back to correct API cmd */
				cmd = IVTV_API_DEC_DMA_FROM_HOST;

				mbox_num = 
					ivtv_get_free_mailbox(cmd, 
						&mbox[0], mbox_max, 0);
			} else {
				mbox_num = 
					ivtv_get_free_mailbox(cmd, 
						&mbox[0], mbox_max, 1);
			}

			/* Valid mailbox? */
			if (mbox_num >= 0 && mbox_num <=2)
				local_box = &mbox[mbox_num];
			else
				goto ivtv_api_fail;

			/* === Write to the Mailbox === */
			ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
			ivtv_write_reg(IVTV_API_STD_TIMEOUT, 
				(unsigned char *)&local_box->timeout);
			ivtv_write_reg(0x00, 
				(unsigned char *)&local_box->retval);

			/* Dec DMA Timestamp */
			if (cmd == IVTV_API_DEC_DMA_FROM_HOST) 
				itv->dec_dma_stat.ts = jiffies;

			for (i = 0; i < args; i++) {
				ivtv_write_reg(data[i], 
					(unsigned char *)&local_box->data[i]);
			}

                        ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
                                 (unsigned char *)&local_box->flags);
			ivtv_read_reg((unsigned char *)&local_box->flags);
			/* === Done with Write to the Mailbox === */

			if (needsresult)
				break;
			goto ivtv_api_done;
		/* Start/Stop capture calls for encoding are sensitive */
		case IVTV_API_BEGIN_CAPTURE:
		case IVTV_API_DEC_START_PLAYBACK:
		case IVTV_API_END_CAPTURE:
		case IVTV_API_EVENT_NOTIFICATION:
		case IVTV_API_ENC_PING_FW:
		case IVTV_API_DEC_PING_FW:
			down(sem);
			gotsem	= 1;
			intr = 0;

			api_timeout = 1000;
			no_reset = 0;

			/* find free mailbox */
			mbox_num = ivtv_get_free_mailbox(cmd, mbox, mbox_max, 0);
			if (mbox_num >= 0 && mbox_num <=mbox_max)
				local_box = &mbox[mbox_num];
			else
				goto ivtv_api_fail;

			ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
			ivtv_write_reg(IVTV_API_STD_TIMEOUT, 
				(unsigned char *)&local_box->timeout);
                        ivtv_write_reg(0x00, 
				(unsigned char *)&local_box->retval);

			for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
				if (i < args) {
					ivtv_write_reg(data[i], 
						(unsigned char *)&local_box->data[i]);
				} else {
					ivtv_write_reg(0, 
						(unsigned char *)&local_box->data[i]);
				}
			}

                        ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
                                 (unsigned char *)&local_box->flags);
			ivtv_read_reg((unsigned char *)&local_box->flags);

			break;
		/* These commands are not stored */
		/* These don't need a result */
		case IVTV_API_FB_BLT_COPY:
		case IVTV_API_FB_BLT_FILL:
		case IVTV_API_FB_BLT_TEXT:
		case IVTV_API_DEC_DMA_BLOCKSIZE:
		case IVTV_API_ASSIGN_DMA_BLOCKLEN:
		case IVTV_API_INITIALIZE_INPUT:
		case IVTV_API_REFRESH_INPUT:
		case IVTV_API_ASSIGN_NUM_VSYNC_LINES:	
		case IVTV_API_MUTE_VIDEO:
		case IVTV_API_MUTE_AUDIO:
			needsresult = 0;
		case IVTV_API_PAUSE_ENCODER:
		case IVTV_API_ENC_HALT_FW:
			if (cmd == IVTV_API_ENC_HALT_FW) {
				api_timeout = 100;
				no_reset = 1;
			}
		case IVTV_API_ENC_GETVER:
		case IVTV_API_DEC_GETVER:
		case IVTV_API_DEC_HALT_FW:
			if (cmd == IVTV_API_DEC_HALT_FW)
				api_timeout = 100;
		case IVTV_API_DEC_XFER_INFO:
			if (cmd == IVTV_API_DEC_XFER_INFO) {
				api_timeout = 1000;
				no_reset = 1;
			}
		case IVTV_API_DEC_STOP_PLAYBACK:
			if (cmd == IVTV_API_DEC_STOP_PLAYBACK) {
				api_timeout = 100;
				no_reset = 1;
			}
		case IVTV_API_DEC_PLAYBACK_SPEED:
		case IVTV_API_DEC_STEP_VIDEO:
		case IVTV_API_DEC_PAUSE_PLAYBACK:
		case IVTV_API_DEC_TIMING_INFO:
		case IVTV_API_DEC_DMA_STATUS:
		case IVTV_API_FB_GET_FRAMEBUFFER:
		case IVTV_API_FB_GET_GLOBAL_ALPHA:
		case IVTV_API_FB_GET_SCREEN_COORDS:
		case IVTV_API_FB_GET_OSD_COORDS:
		case IVTV_API_FB_GET_STATE:
			if (cmd == IVTV_API_FB_GET_STATE) {
				no_reset = 1;
			}
		case IVTV_API_FB_GET_PIXEL_FORMAT:
		case IVTV_API_FB_GET_ALPHA_CONTENT_INDEX:
		case IVTV_API_DEC_DECODE_SOURCE:
                case IVTV_API_FB_SET_GLOBAL_ALPHA:
                case IVTV_API_FB_SET_STATE:
			if (cmd == IVTV_API_FB_SET_STATE) {
				api_timeout = 1000;
				no_reset = 1;
			}
                case IVTV_API_FB_SET_FLICKER_STATE:
                case IVTV_API_FB_SET_OSD_COORDS:
                case IVTV_API_FB_SET_SCREEN_COORDS:
                        down(sem);
                        gotsem	= 1;

			/* find free mailbox */
			mbox_num = ivtv_get_free_mailbox(cmd, mbox, mbox_max, 0);
			if (mbox_num >= 0 && mbox_num <=mbox_max)
				local_box = &mbox[mbox_num];
			else
				goto ivtv_api_fail;

			ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
			ivtv_write_reg(IVTV_API_STD_TIMEOUT, 
				(unsigned char *)&local_box->timeout);
                        ivtv_write_reg(0x00, 
				(unsigned char *)&local_box->retval);

			for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
				if (i < args) {
					ivtv_write_reg(data[i], 
						(unsigned char *)&local_box->data[i]);
				} else {
					ivtv_write_reg(0, 
						(unsigned char *)&local_box->data[i]);
				}
			}

                        ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
                                 (unsigned char *)&local_box->flags);
			ivtv_read_reg((unsigned char *)&local_box->flags);

			break;
		/* These don't need a result */
                case IVTV_API_DEC_SELECT_AUDIO:
                case IVTV_API_DEC_DISPLAY_BUFFERS:
                case IVTV_API_DEC_AUDIO_OUTPUT:
                case IVTV_API_DEC_SET_AV_DELAY:
                case IVTV_API_DEC_BUFFER:
                case IVTV_API_DEC_STREAM_INPUT:
                case IVTV_API_ASSIGN_PLACEHOLDER:
                case IVTV_API_ASSIGN_FRAME_DROP_RATE:
                case IVTV_API_ASSIGN_SPATIAL_FILTER_TYPE:
                case IVTV_API_ASSIGN_CORING_LEVELS:
                case IVTV_API_ASSIGN_DNR_FILTER_PROPS:
                case IVTV_API_ASSIGN_DNR_FILTER_MODE:
                case IVTV_API_ASSIGN_AUDIO_PROPERTIES:
                case IVTV_API_ASSIGN_GOP_CLOSURE:
                case IVTV_API_ASSIGN_3_2_PULLDOWN:
                case IVTV_API_ASSIGN_GOP_PROPERTIES:
                case IVTV_API_ASSIGN_BITRATES:
                case IVTV_API_ASSIGN_ASPECT_RATIO:
                case IVTV_API_ASSIGN_FRAME_SIZE:
                case IVTV_API_ASSIGN_FRAMERATE:
                case IVTV_API_ASSIGN_OUTPUT_PORT:
                case IVTV_API_ASSIGN_STREAM_TYPE:
                case IVTV_API_FB_SET_PIXEL_FORMAT:
                case IVTV_API_FB_SET_BLEND_COORDS:
                case IVTV_API_FB_SET_FRAMEBUFFER_WINDOW:
                case IVTV_API_FB_SET_CHROMA_KEY:
                case IVTV_API_FB_SET_ALPHA_CONTENT_INDEX:
			needsresult = 0;
		default: /* Stored Commands */
                        down(sem);
                        gotsem	= 1;

		        /* Store command send/return data */
		        if (itv->api[cmd].marked && 
				((jiffies - itv->api[cmd].jiffies) < 
				MBOX_TIMEOUT)) 
			{
		                int stored = 1;
		                /* check if same args given within timeout */
		                /* if ok, return old args, or run again */
		                for (i = 0; i < args; i++) {
		                        if (itv->api[cmd].s_data[i] == data[i])
					{
		                                /* Data is same as before */
		                        } else {
						IVTV_DEBUG(IVTV_DEBUG_API, 
							"cmd: 0x%08x arg %d "
							"stored as 0x%08x "
							"but changed to "
							"0x%08x\n", 
							cmd, i, 
							itv->api[cmd].s_data[i],
							data[i]);
		                                stored = 0; /* different */
		                                break;
		                        }
		                }
		
		                if (stored) {
					IVTV_DEBUG(IVTV_DEBUG_API, 
						"cmd: 0x%08x arg %d stored\n",
						cmd, i);
					if (i > 0)
						 IVTV_DEBUG(IVTV_DEBUG_API,
							"as 0x%08x\n", 
							itv->api[cmd].s_data[(i-1)]);
		                        for (i=0; i < IVTV_MBOX_MAX_DATA; i++) {
		                               	data[i] = 
							itv->api[cmd].r_data[i];
					}
		
		                        result = 0;
					goto ivtv_api_done;
		                }
		
		                itv->api[cmd].marked = 1; /* dirty mbox */
		                itv->api[cmd].jiffies = jiffies; /* timestamp */

                        	for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
                                	if (i < args) {
		                        	itv->api[cmd].s_data[i] = 
							data[i];
                                	} else {
		                        	itv->api[cmd].s_data[i] = 0;
                                	}
                        	}
		        } else { /* Not Stored */
				IVTV_DEBUG(IVTV_DEBUG_API, 
					"cmd: 0x%08x not stored\n", cmd);
		                itv->api[cmd].marked = 1; /* dirty mbox */
		                itv->api[cmd].jiffies = jiffies; /* timestamp */

                        	for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
                                	if (i < args) {
		                        	itv->api[cmd].s_data[i] = 
							data[i];
                                	} else {
		                        	itv->api[cmd].s_data[i] = 0;
                                	}
                        	}
		        }

			/* find free mailbox */
			mbox_num = ivtv_get_free_mailbox(cmd, mbox, mbox_max, 0);
			if (mbox_num >= 0 && mbox_num <=mbox_max)
				local_box = &mbox[mbox_num];
			else
				goto ivtv_api_fail;

			ivtv_write_reg(cmd, (unsigned char *)&local_box->cmd);
			ivtv_write_reg(IVTV_API_STD_TIMEOUT, 
				(unsigned char *)&local_box->timeout);
                        ivtv_write_reg(0x00, 
				(unsigned char *)&local_box->retval);

			for (i = 0; i < IVTV_MBOX_MAX_DATA; i++) {
				if (i < args) {
					ivtv_write_reg(data[i], 
						(unsigned char *)&local_box->data[i]);
				} else {
					ivtv_write_reg(0, 
						(unsigned char *)&local_box->data[i]);
				}
			}

			ivtv_write_reg((IVTV_MBOX_DRIVER_DONE | IVTV_MBOX_DRIVER_BUSY),
				(unsigned char *)&local_box->flags);
			ivtv_read_reg((unsigned char *)&local_box->flags);
			break;
	}

	/* Get results if needed */
	if (needsresult) {
		int count;
		x = ivtv_api_getresult(local_box, 
			result, &data[0], api_timeout, intr, 
			cmd);

		if(x == -EBUSY) {
			IVTV_DEBUG(IVTV_DEBUG_ERR, 
				"Failed api call 0x%08x with result 0x%08x\n", 
				cmd, x);

			/* make sure cmd is not stored */
        		itv->api[cmd].marked = 0; 
		} else {
			/* Store results */
	        	for (count=0; count < IVTV_MBOX_MAX_DATA; count++)
                		itv->api[cmd].r_data[count] = data[count];
		}

		IVTV_DEBUG(IVTV_DEBUG_API, 
			"Releasing mailbox (before 0x%08x, ",
		   	ivtv_read_reg((unsigned char *)&local_box->flags));

		/* reinit results and flags */
                ivtv_write_reg(0x00, (unsigned char *)&local_box->retval);
		ivtv_write_reg(0x00, (unsigned char *)&local_box->flags);

		IVTV_DEBUG(IVTV_DEBUG_API, "after 0x%08x)\n", 
			ivtv_read_reg((unsigned char *)&local_box->flags));
	} else {
		x = 0;
	}

ivtv_api_done:
	if (gotsem) {
        	up(sem);
	}

	if (x)
        	itv->api[cmd].marked = 0; /* make sure cmd is not stored */

	return x;
ivtv_api_fail:
	IVTV_DEBUG(IVTV_DEBUG_ERR,
		"Firmware UNRESPONSIVE when trying cmd 0x%08x!!!\n", cmd);
	if (gotsem) {
        	up(sem);
	}

       	itv->api[cmd].marked = 0; /* make sure cmd is not stored */

	return -EBUSY;
}

int ivtv_api_result(struct ivtv *itv, int cmd, int args, u32 *data)
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
