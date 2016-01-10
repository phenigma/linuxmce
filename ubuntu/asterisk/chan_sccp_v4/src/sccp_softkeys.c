
/*!
 * \file 	sccp_softkeys.c
 * \brief 	SCCP SoftKeys Class
 * \author 	Sergio Chersovani <mlists [at] c-net.it>
 * \note		Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note		This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 *
 * $Date: 2012-02-21 14:37:26 +0000 (Tue, 21 Feb 2012) $
 * $Revision: 3262 $
 */

#include "config.h"
#include "common.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 3262 $")


/* private prototypes */
void sccp_sk_videomode(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
/* done */

/*!
 * \brief Global list of softkeys
 */
struct softKeySetConfigList softKeySetConfig;					/*!< List of SoftKeySets */

#ifdef CS_DYNAMIC_CONFIG

/*!
 * \brief Softkey Pre Reload
 *
 * \deprecated
 * \lock
 * 	- softKeySetConfig
 */
void sccp_softkey_pre_reload(void)
{
// 	sccp_softKeySetConfiguration_t *k;
// 	uint8_t i;
// 	
// 	SCCP_LIST_LOCK(&softKeySetConfig);
// 	while ((k = SCCP_LIST_REMOVE_HEAD(&softKeySetConfig, list))) {
// 		sccp_log(DEBUGCAT_NEWCODE) (VERBOSE_PREFIX_3 "Setting SoftkeySetConfig to Pending Delete=1\n");
// 		for (i = 0; i < (sizeof(SoftKeyModes) / sizeof(softkey_modes)); i++) {
// 			if(k->modes[i].ptr)
// 				sccp_free(k->modes[i].ptr);
// 		}
// 		sccp_free(k);
// 	}
// 	SCCP_LIST_UNLOCK(&softKeySetConfig);
}

/*!
 * \brief Softkey Post Reload
 */
void sccp_softkey_post_reload(void)
{

}
#endif										/* CS_DYNAMIC_CONFIG */

/*!
 * \brief Softkey Function Callback by SKINNY LABEL
 */
static const struct sccp_softkeyMap_cb softkeyCbMap[] = {
	{SKINNY_LBL_NEWCALL, sccp_sk_newcall, FALSE},
	{SKINNY_LBL_REDIAL, sccp_sk_redial, FALSE},
	{SKINNY_LBL_CONFRN, sccp_sk_conference, TRUE},
	{SKINNY_LBL_MEETME, sccp_sk_meetme, TRUE},
	{SKINNY_LBL_JOIN, sccp_sk_join, TRUE},
	{SKINNY_LBL_BARGE, sccp_sk_barge, TRUE},
	{SKINNY_LBL_CBARGE, sccp_sk_cbarge, TRUE},
	{SKINNY_LBL_HOLD, sccp_sk_hold, TRUE},
	{SKINNY_LBL_TRANSFER, sccp_sk_transfer, TRUE},
	{SKINNY_LBL_CFWDALL, sccp_sk_cfwdall, FALSE},
	{SKINNY_LBL_CFWDBUSY, sccp_sk_cfwdbusy, FALSE},
	{SKINNY_LBL_CFWDNOANSWER, sccp_sk_cfwdnoanswer, FALSE},
	{SKINNY_LBL_BACKSPACE, sccp_sk_backspace, TRUE},

	{SKINNY_LBL_ENDCALL, sccp_sk_endcall, TRUE},
	{SKINNY_LBL_RESUME, sccp_sk_resume, TRUE},
	{SKINNY_LBL_ANSWER, sccp_sk_answer, TRUE},
#ifdef CS_SCCP_PARK
	{SKINNY_LBL_PARK, sccp_sk_park, TRUE},
#endif

	{SKINNY_LBL_TRNSFVM, sccp_sk_trnsfvm, TRUE},
	{SKINNY_LBL_IDIVERT, sccp_sk_trnsfvm, TRUE},
	{SKINNY_LBL_DND, sccp_sk_dnd, FALSE},
	{SKINNY_LBL_DIRTRFR, sccp_sk_dirtrfr, TRUE},
	{SKINNY_LBL_SELECT, sccp_sk_select, TRUE},
	{SKINNY_LBL_PRIVATE, sccp_sk_private, TRUE},
#ifdef CS_SCCP_PICKUP
	{SKINNY_LBL_PICKUP, sccp_sk_pickup, FALSE},
	{SKINNY_LBL_GPICKUP, sccp_sk_gpickup, FALSE},
#endif
	{SKINNY_LBL_MONITOR, sccp_feat_monitor, TRUE},
	{SKINNY_LBL_INTRCPT, sccp_sk_resume, TRUE},
	{SKINNY_LBL_CONFLIST, sccp_sk_conflist, TRUE},
	{SKINNY_LBL_DIAL, sccp_sk_dial, TRUE},
	{SKINNY_LBL_VIDEO_MODE, sccp_sk_videomode, TRUE},

};

/*!
 * \brief Get SoftkeyMap by Event
 */
const sccp_softkeyMap_cb_t *sccp_getSoftkeyMap_by_SoftkeyEvent(uint32_t event)
{
	uint8_t i;

	for (i = 0; i < ARRAY_LEN(softkeyCbMap); i++) {
		if (softkeyCbMap[i].event == event) {
			return &softkeyCbMap[i];
		}
	}

	return NULL;
}


/*!
 * \brief Enable or Disable one softkey on a specific softKeySet
 * \param device SCCP Device ()
 * \param softKeySet SoftkeySet \see SoftKeyModes
 * \param softKey softkey e.g. SKINNY_LBL_REDIAL
 * \param enable enabled or disabled
 *
 */
void sccp_softkey_setSoftkeyState(sccp_device_t *device, uint8_t softKeySet, uint8_t softKey, boolean_t enable){
	
	uint8_t i;
	
	/* find softkey */
	for(i = 0; i < device->softKeyConfiguration.modes[softKeySet].count; i++){
		if(device->softKeyConfiguration.modes[softKeySet].ptr[i] == softKey){
			sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: found softkey '%s' at %d\n", DEV_ID_LOG(device), label2str(device->softKeyConfiguration.modes[softKeySet].ptr[i]), i);
			
			if(enable){
				device->softKeyConfiguration.activeMask[softKeySet] |= (1 << i);
			}else{
				device->softKeyConfiguration.activeMask[softKeySet] &= (~(1 << i));
			}
		}
	}

	return;
}

/*!
 * \brief Forces Dialling before timeout
 * \n Usage: \ref sccp_sk_dial
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_dial(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t *c){
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Dial Pressed\n", DEV_ID_LOG(d));
	if (c) { // Handle termination of dialling if in appropriate state.
		/* Only handle this in DIALING state. AFAIK GETDIGITS is used only for call forward and related input functions. (-DD) */
		if ( SCCP_CHANNELSTATE_DIALING == c->state ) {
			sccp_channel_lock(c);
			c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
			sccp_pbx_softswitch_locked(c);
			sccp_channel_unlock(c);
		}
	}
}

/*!
 * \brief Start/Stop VideoMode
 * \n Usage: \ref sccp_sk_videomode
 * \param device SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param channel SCCP Channel
 *
 * \todo Add doxygen entry for sccp_sk_videomode
 * \todo Implement stopping video transmission
 */
void sccp_sk_videomode(sccp_device_t *device, sccp_line_t *l, const uint32_t lineInstance, sccp_channel_t *channel){
#ifdef CS_SCCP_VIDEO
	if (sccp_device_isVideoSupported(device)) {
		sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: We can have video, try to start vrtp\n", DEV_ID_LOG(device));
		if (!channel->rtp.video.rtp && !sccp_rtp_createVideoServer(channel)) {
			sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: can not start vrtp\n", DEV_ID_LOG(device));
		} else {
			sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: vrtp started\n", DEV_ID_LOG(device));
 			sccp_channel_startMultiMediaTransmission(channel);
		}
	}
#endif
}

/*!
 * \brief Redial last Dialed Number by this Device
 * \n Usage: \ref sk_redial
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \lock
 * 	- channel
 */
void sccp_sk_redial(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Redial Pressed\n", DEV_ID_LOG(d));

#ifdef CS_ADV_FEATURES
	if (d->useRedialMenu) {
		sccp_moo_t *r1 = NULL;
		char *data = "<CiscoIPPhoneExecute><ExecuteItem Priority=\"0\"URL=\"Key:Directories\"/><ExecuteItem Priority=\"0\"URL=\"Key:KeyPad3\"/></CiscoIPPhoneExecute>";

		int dummy_len, msgSize, hdr_len, padding;

		dummy_len = strlen(data);
		hdr_len = 40 - 1;
		padding = ((dummy_len + hdr_len) % 4);
		padding = (padding > 0) ? 4 - padding : 0;
		msgSize = hdr_len + dummy_len + padding;

		r1 = sccp_build_packet(UserToDeviceDataVersion1Message, msgSize);
		r1->msg.UserToDeviceDataVersion1Message.lel_callReference = htolel(1);
		r1->msg.UserToDeviceDataVersion1Message.lel_transactionID = htolel(1);
		r1->msg.UserToDeviceDataVersion1Message.lel_sequenceFlag = 0x0002;
		r1->msg.UserToDeviceDataVersion1Message.lel_displayPriority = 0x0002;
		r1->msg.UserToDeviceDataVersion1Message.lel_dataLength = htolel(dummy_len);

		if (dummy_len) {
			char buffer[dummy_len + 2];

			memset(&buffer[0], 0, sizeof(buffer));
			memcpy(&buffer[0], data, dummy_len);

			memcpy(&r1->msg.UserToDeviceDataVersion1Message.data, &buffer[0], sizeof(buffer));
			sccp_dev_send(d, r1);
		}
		return;
	}
#endif

	if (sccp_strlen_zero(d->lastNumber)) {
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: No number to redial\n", d->id);
		return;
	}

	if (c) {
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK) {
			/* we have a offhook channel */
			sccp_channel_lock(c);
			sccp_copy_string(c->dialedNumber, d->lastNumber, sizeof(c->dialedNumber));
			sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: Get ready to redial number %s\n", d->id, d->lastNumber);
			// c->digittimeout = time(0)+1;
			c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
			sccp_pbx_softswitch_locked(c);
			sccp_channel_unlock(c);
		}
		/* here's a KEYMODE error. nothing to do */
		return;
	}
	if (!l)
		l = d->currentLine;
	sccp_channel_newcall(l, d, d->lastNumber, SKINNY_CALLTYPE_OUTBOUND);

}

/*!
 * \brief Initiate a New Call
 * \n Usage: \ref sk_newcall
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_newcall(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	char *adhocNumber = NULL;
	sccp_speed_t *k = NULL;

	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey NewCall Pressed\n", DEV_ID_LOG(d));
	if (!l) {
	  
		/* handle dummy speeddial */
		k = sccp_dev_speed_find_byindex(d, lineInstance, SCCP_BUTTONTYPE_HINT);
		if(k && (strlen(k->ext) > 0) ){
			adhocNumber = k->ext;
		}
	  
		/* use default line if it is set */
		if (d && d->defaultLineInstance > 0) {
			sccp_log((DEBUGCAT_SOFTKEY | DEBUGCAT_LINE)) (VERBOSE_PREFIX_3 "using default line with instance: %u", d->defaultLineInstance);
			l = sccp_line_find_byid(d, d->defaultLineInstance);
		}
	}

	if (!l && d && d->currentLine)
		l = d->currentLine;

	if (!l) {
		sccp_dev_starttone(d, SKINNY_TONE_ZIPZIP, 0, 0, 1);
 		sccp_dev_displayprompt(d, 0, 0, "No line available", 5);
	} else {
		if(!adhocNumber && (strlen(l->adhocNumber) > 0) ){
			adhocNumber = l->adhocNumber;
		}
	  
		if (adhocNumber)
			sccp_channel_newcall(l, d, adhocNumber, SKINNY_CALLTYPE_OUTBOUND);
		else
			sccp_channel_newcall(l, d, NULL, SKINNY_CALLTYPE_OUTBOUND);
	}
	
	if(k)
		sccp_free(k);
}

/*!
 * \brief Hold Call on Current Line
 * \n Usage: \ref sk_hold
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_hold(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Hold Pressed\n", DEV_ID_LOG(d));
	if (!c) {
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: No call to put on hold, check your softkeyset, hold should not be available in this situation.\n", DEV_ID_LOG(d));
		sccp_dev_displayprompt(d, 0, 0, "No call to put on hold.", 5);
		return;
	}
	sccp_channel_lock(c);
	sccp_channel_hold_locked(c);
	sccp_channel_unlock(c);
}

/*!
 * \brief Resume Call on Current Line
 * \n Usage: \ref sk_resume
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_resume(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Resume Pressed\n", DEV_ID_LOG(d));
	if (!c) {
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: No call to resume. Ignoring\n", d->id);
		return;
	}
	sccp_channel_lock(c);
	sccp_channel_resume_locked(d, c, TRUE);
	sccp_channel_unlock(c);
}

/*!
 * \brief Transfer Call on Current Line
 * \n Usage: \ref sk_transfer
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \todo discus Marcello's transfer experiment
 *
 * \lock
 * 	- device->selectedChannels
 * 	- device->buttonconfig
 * 	  - see sccp_line_find_byname_wo()
 * 	  - line->channels
 */
void sccp_sk_transfer(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c){

	sccp_channel_lock(c);
	sccp_channel_transfer_locked(c);
	sccp_channel_unlock(c);
}

/*!
 * \brief End Call on Current Line
 * \n Usage: \ref sk_endcall
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_endcall(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey EndCall Pressed\n", DEV_ID_LOG(d));
	if (!c) {
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: Endcall with no call in progress\n", d->id);
		return;
	}
	sccp_channel_lock(c);
	sccp_channel_endcall_locked(c);
	sccp_channel_unlock(c);
}

/*!
 * \brief Set DND on Current Line if Line is Active otherwise set on Device
 * \n Usage: \ref sk_dnd
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \todo The line param is not used 
 */
void sccp_sk_dnd(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey DND Pressed\n", DEV_ID_LOG(d));

	if (!d->dndFeature.enabled) {
		sccp_dev_displayprompt(d, 0, 0, SKINNY_DISP_DND " " SKINNY_DISP_SERVICE_IS_NOT_ACTIVE, 10);
		return;
	}

	if (!strcasecmp(d->dndFeature.configOptions, "reject")) {
		/* config is set to: dnd=reject */
		if (d->dndFeature.status == SCCP_DNDMODE_OFF)
			d->dndFeature.status = SCCP_DNDMODE_REJECT;
		else
			d->dndFeature.status = SCCP_DNDMODE_OFF;

	} else if (!strcasecmp(d->dndFeature.configOptions, "silent")) {
		/* config is set to: dnd=silent */
		if (d->dndFeature.status == SCCP_DNDMODE_OFF)
			d->dndFeature.status = SCCP_DNDMODE_SILENT;
		else
			d->dndFeature.status = SCCP_DNDMODE_OFF;

	} else {
		/* for all other config us the toggle mode */
		switch (d->dndFeature.status) {
		case SCCP_DNDMODE_OFF:
			d->dndFeature.status = SCCP_DNDMODE_REJECT;
			break;
		case SCCP_DNDMODE_REJECT:
			d->dndFeature.status = SCCP_DNDMODE_SILENT;
			break;
		case SCCP_DNDMODE_SILENT:
			d->dndFeature.status = SCCP_DNDMODE_OFF;
			break;
		default:
			d->dndFeature.status = SCCP_DNDMODE_OFF;
			break;
		}

	}
	
	sccp_feat_changed(d, SCCP_FEATURE_DND);					/* notify the modules the the DND-feature changed state */
	sccp_dev_check_displayprompt(d);					/*! \todo we should use the feature changed event to check displayprompt */
}

/*!
 * \brief BackSpace Last Entered Number
 * \n Usage: \ref sk_backspace
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \lock
 * 	- channel
 * 	  - see sccp_handle_dialtone_nolock()
 */
void sccp_sk_backspace(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Backspace Pressed\n", DEV_ID_LOG(d));
	int len;

	if ((c->state != SCCP_CHANNELSTATE_DIALING) && (c->state != SCCP_CHANNELSTATE_DIGITSFOLL) && (c->state != SCCP_CHANNELSTATE_OFFHOOK)) {
		return;
	}

	sccp_channel_lock(c);

	len = strlen(c->dialedNumber);

	/* we have no number, so nothing to process */
	if (!len) {
		sccp_channel_unlock(c);
		return;
	}

	if (len > 1) {
		c->dialedNumber[len - 1] = '\0';
		/* removing scheduled dial */
		c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
		/* rescheduling dial timeout (one digit) */
		if ((c->scheduler.digittimeout = sccp_sched_add(GLOB(digittimeout) * 1000, sccp_pbx_sched_dial, c)) < 0) {
			sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_1 "SCCP: (sccp_sk_backspace) Unable to reschedule dialing in '%d' s\n", GLOB(digittimeout));
		}
	} else if (len == 1) {
		c->dialedNumber[len - 1] = '\0';
		/* removing scheduled dial */
		c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
		/* rescheduling dial timeout (no digits) */
		if ((c->scheduler.digittimeout = sccp_sched_add(GLOB(firstdigittimeout) * 1000, sccp_pbx_sched_dial, c)) < 0) {
			sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_1 "SCCP: (sccp_sk_backspace) Unable to reschedule dialing in '%d' s\n", GLOB(firstdigittimeout));
		}
	}
	// sccp_log((DEBUGCAT_SOFTKEY))(VERBOSE_PREFIX_3 "%s: backspacing dial number %s\n", c->device->id, c->dialedNumber);
	sccp_handle_dialtone_locked(c);
	sccp_channel_unlock(c);

	sccp_handle_backspace(d, lineInstance, c->callid);
}

/*!
 * \brief Answer Incoming Call
 * \n Usage: \ref sk_answer
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_answer(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Answer Pressed, instance: %d\n", DEV_ID_LOG(d), lineInstance);
	if (c->owner)
		pbx_channel_lock(c->owner);
	sccp_channel_lock(c);
	sccp_channel_answer_locked(d, c);
	sccp_channel_unlock(c);
	if (c->owner)
		pbx_channel_unlock(c->owner);
}

/*!
 * \brief Bridge two selected channels
 * \n Usage: \ref sk_dirtrfr
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \lock
 * 	- line->channels
 * 	  - device->selectedChannels
 * 	- device->selectedChannels
 * 	- device
 */
void sccp_sk_dirtrfr(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Direct Transfer Pressed\n", DEV_ID_LOG(d));

	sccp_selectedchannel_t *x;
	sccp_channel_t *chan1 = NULL, *chan2 = NULL, *tmp = NULL;

	if (!d)
		return;

	if ((sccp_device_selectedchannels_count(d)) != 2) {
		if (SCCP_RWLIST_GETSIZE(l->channels) == 2) {
			sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: Automatically select the two current channels\n", d->id);
			SCCP_LIST_LOCK(&l->channels);
			SCCP_LIST_TRAVERSE(&l->channels, c, list) {
				x = sccp_malloc(sizeof(sccp_selectedchannel_t));
				x->channel = c;
				SCCP_LIST_LOCK(&d->selectedChannels);
				SCCP_LIST_INSERT_HEAD(&d->selectedChannels, x, list);
				SCCP_LIST_UNLOCK(&d->selectedChannels);
			}
			SCCP_LIST_UNLOCK(&l->channels);
		} else if (SCCP_RWLIST_GETSIZE(l->channels) < 2) {
			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_3 "%s: Not enough channels to transfer\n", d->id);
			sccp_dev_displayprompt(d, lineInstance, c->callid, "Not enough calls to trnsf", 5);
//                      sccp_dev_displayprompt(d, 0, 0, SKINNY_DISP_CAN_NOT_COMPLETE_TRANSFER, 5);
			return;
		} else {
			sccp_log(DEBUGCAT_CORE) (VERBOSE_PREFIX_3 "%s: More than 2 channels to transfer, please use softkey select\n", d->id);
			sccp_dev_displayprompt(d, lineInstance, c->callid, "More than 2 calls, use " SKINNY_DISP_SELECT, 5);
//                      sccp_dev_displayprompt(d, 0, 0, SKINNY_DISP_CAN_NOT_COMPLETE_TRANSFER, 5);
			return;
		}
	}

	SCCP_LIST_LOCK(&d->selectedChannels);
	x = SCCP_LIST_FIRST(&d->selectedChannels);
	chan1 = x->channel;
	chan2 = SCCP_LIST_NEXT(x, list)->channel;
	SCCP_LIST_UNLOCK(&d->selectedChannels);

	if (chan1 && chan2) {
		//for using the sccp_channel_transfer_complete function
		//chan2 must be in RINGOUT or CONNECTED state
		sccp_dev_displayprompt(d, 0, 0, SKINNY_DISP_CALL_TRANSFER, 5);
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: (sccp_sk_dirtrfr) First Channel Status (%d), Second Channel Status (%d)\n", DEV_ID_LOG(d), chan1->state, chan2->state);
		if (chan2->state != SCCP_CHANNELSTATE_CONNECTED && chan1->state == SCCP_CHANNELSTATE_CONNECTED) {
			tmp = chan1;
			chan1 = chan2;
			chan2 = tmp;
		} else if (chan1->state == SCCP_CHANNELSTATE_HOLD && chan2->state == SCCP_CHANNELSTATE_HOLD) {
			//resume chan2 if both channels are on hold
			sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: (sccp_sk_dirtrfr) Resuming Second Channel (%d)\n", DEV_ID_LOG(d), chan2->state);
			sccp_channel_lock(chan2);
			sccp_channel_resume_locked(d, chan2, TRUE);
			sccp_channel_unlock(chan2);
			sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: (sccp_sk_dirtrfr) Resumed Second Channel (%d)\n", DEV_ID_LOG(d), chan2->state);
		}
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: (sccp_sk_dirtrfr) First Channel Status (%d), Second Channel Status (%d)\n", DEV_ID_LOG(d), chan1->state, chan2->state);
		sccp_device_lock(d);
		d->transfer_channel = chan1;
		sccp_device_unlock(d);

		sccp_channel_transfer_complete(chan2);
	}
}

/*!
 * \brief Select a Line for further processing by for example DirTrfr
 * \n Usage: \ref sk_select
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \lock
 * 	- device->selectedChannels
 */
void sccp_sk_select(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Select Pressed\n", DEV_ID_LOG(d));
	sccp_selectedchannel_t *x = NULL;
	sccp_moo_t *r1;
	uint8_t numSelectedChannels = 0, status = 0;

	if (!d) {
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "SCCP: (sccp_sk_select) Can't select a channel without a device\n");
		return;
	}
	if (!c) {
		sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: (sccp_sk_select) No channel to be selected\n", DEV_ID_LOG(d));
		return;
	}

	if ((x = sccp_device_find_selectedchannel(d, c))) {
		SCCP_LIST_LOCK(&d->selectedChannels);
		x = SCCP_LIST_REMOVE(&d->selectedChannels, x, list);
		SCCP_LIST_UNLOCK(&d->selectedChannels);
		sccp_free(x);
	} else {
		x = sccp_malloc(sizeof(sccp_selectedchannel_t));
		x->channel = c;
		SCCP_LIST_LOCK(&d->selectedChannels);
		SCCP_LIST_INSERT_HEAD(&d->selectedChannels, x, list);
		SCCP_LIST_UNLOCK(&d->selectedChannels);
		status = 1;
	}

	numSelectedChannels = sccp_device_selectedchannels_count(d);

	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: (sccp_sk_select) '%d' channels selected\n", DEV_ID_LOG(d), numSelectedChannels);

	REQ(r1, CallSelectStatMessage);
	r1->msg.CallSelectStatMessage.lel_status = htolel(status);
	r1->msg.CallSelectStatMessage.lel_lineInstance = htolel(lineInstance);
	r1->msg.CallSelectStatMessage.lel_callReference = htolel(c->callid);
	sccp_dev_send(d, r1);
}

/*!
 * \brief Set Call Forward All on Current Line
 * \n Usage: \ref sk_cfwdall
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_cfwdall(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Call Forward All Pressed\n", DEV_ID_LOG(d));
	if (!l && d) {

		if (d->defaultLineInstance > 0) {
			l = sccp_line_find_byid(d, d->defaultLineInstance);
		}
		if (!l) {
			l = d->currentLine;
		}
		if (!l) {
			l = sccp_line_find_byid(d, 1);
		}
	}

	if (l)
		sccp_feat_handle_callforward(l, d, SCCP_CFWD_ALL);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", DEV_ID_LOG(d), 1);
}

/*!
 * \brief Set Call Forward when Busy on Current Line
 * \n Usage: \ref sk_cfwdbusy
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_cfwdbusy(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Call Forward Busy Pressed\n", DEV_ID_LOG(d));
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}

	if (l)
		sccp_feat_handle_callforward(l, d, SCCP_CFWD_BUSY);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);
}

/*!
 * \brief Set Call Forward when No Answer on Current Line
 * \n Usage: \ref sk_cfwdnoanswer
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_cfwdnoanswer(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Call Forward NoAnswer Pressed\n", DEV_ID_LOG(d));
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}
	if (l)
		sccp_feat_handle_callforward(l, d, SCCP_CFWD_NOANSWER);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);

	/*
	   sccp_log((DEBUGCAT_SOFTKEY))(VERBOSE_PREFIX_3 "### CFwdNoAnswer Softkey pressed - NOT SUPPORTED\n");
	 */
}

/*!
 * \brief Park Call on Current Line
 * \n Usage: \ref sk_park
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_park(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Park Pressed\n", DEV_ID_LOG(d));
#ifdef CS_SCCP_PARK
	sccp_channel_park(c);
#else
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "### Native park was not compiled in\n");
#endif
}

/*!
 * \brief Transfer to VoiceMail on Current Line
 * \n Usage: \ref sk_transfer
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_trnsfvm(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Transfer Voicemail Pressed\n", DEV_ID_LOG(d));
	sccp_feat_idivert(d, l, c);
}

/*!
 * \brief Initiate Private Call on Current Line
 * \n Usage: \ref sk_private
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 *
 * \lock
 * 	- channel
 * 	  - see sccp_dev_displayprompt()
 */
void sccp_sk_private(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Private Pressed\n", DEV_ID_LOG(d));

	if (!d->privacyFeature.enabled) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: private function is not active on this device\n", d->id);
		sccp_dev_displayprompt(d, lineInstance, 0, "PRIVATE function is not active", 5);
		return;
	}

	if (!c) {
		sccp_dev_displayprompt(d, lineInstance, 0, "PRIVATE with no channel active", 5);
		return;
	}

	sccp_channel_lock(c);
	c->privacy = (c->privacy) ? FALSE : TRUE;
//	d->privacyFeature.status = c->privacy;		// should not be activeted on softkey
//	sccp_feat_changed(d, SCCP_FEATURE_PRIVACY);

	if (c->privacy) {
		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_PRIVATE, 0);
	} else {
		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_ENTER_NUMBER, 1);
	}
	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: Private %s on call %d\n", d->id, c->privacy ? "enabled" : "disabled", c->callid);
	sccp_channel_unlock(c);
}

/*!
 * \brief Put Current Line into Conference
 * \n Usage: \ref sk_conference
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 */
void sccp_sk_conference(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Conference Pressed\n", DEV_ID_LOG(d));
	sccp_feat_conference(d, l, lineInstance, c);
}

/*!
 * \brief Show Participant List of Current Conference
 * \n Usage: \ref sk_conflist
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \todo Conferencing option is under development.
 */
void sccp_sk_conflist(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Conflist Pressed\n", DEV_ID_LOG(d));
	sccp_feat_conflist(d, l, lineInstance, c);
}

/*!
 * \brief Join Current Line to Conference
 * \n Usage: \ref sk_join
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 */
void sccp_sk_join(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Join Pressed\n", DEV_ID_LOG(d));
	sccp_feat_join(d, l, lineInstance, c);
}

/*!
 * \brief Barge into Call on the Current Line
 * \n Usage: \ref sk_barge
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_barge(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Barge Pressed\n", DEV_ID_LOG(d));
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}
	if (l)
		sccp_feat_handle_barge(l, lineInstance, d);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);
}

/*!
 * \brief Barge into Call on the Current Line
 * \n Usage: \ref sk_cbarge
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_cbarge(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey cBarge Pressed\n", DEV_ID_LOG(d));
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}
	if (l)
		sccp_feat_handle_cbarge(l, lineInstance, d);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);
}

/*!
 * \brief Put Current Line in to Meetme Conference
 * \n Usage: \ref sk_meetme
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 */
void sccp_sk_meetme(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Meetme Pressed\n", DEV_ID_LOG(d));
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}
	if (l)
		sccp_feat_handle_meetme(l, lineInstance, d);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);
}

/*!
 * \brief Pickup Parked Call
 * \n Usage: \ref sk_pickup
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_pickup(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Pickup Pressed\n", DEV_ID_LOG(d));
#ifndef CS_SCCP_PICKUP
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "### Native EXTENSION PICKUP was not compiled in\n");
#else
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}
	if (l)
		sccp_feat_handle_directpickup(l, lineInstance, d);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);
#endif
}

/*!
 * \brief Pickup Ringing Line from Pickup Group
 * \n Usage: \ref sk_gpickup
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 */
void sccp_sk_gpickup(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: SoftKey Group Pickup Pressed\n", DEV_ID_LOG(d));
#ifndef CS_SCCP_PICKUP
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "### Native GROUP PICKUP was not compiled in\n");
#else
	if (!l && d) {
		l = sccp_line_find_byid(d, 1);
	}
	if (l)
		sccp_feat_grouppickup(l, d);
	else
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line (%d) found\n", d->id, 1);

#endif
}

/*!
 * \brief sets a SoftKey to a specified status (on/off)
 *
 * \param d SCCP Device
 * \param l active line
 * \param lineInstance lineInstance as uint8_t
 * \param c active channel
 * \param keymode int of KEYMODE_*
 * \param softkeyindex index of the SoftKey to set
 * \param status 0 for off otherwise on
 *
 * \todo use SoftKeyLabel instead of softkeyindex
 */
void sccp_sk_set_keystate(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c, unsigned int keymode, unsigned int softkeyindex, unsigned int status)
{
	sccp_moo_t *r;
	uint32_t mask, validKeyMask;
	unsigned i;

	if (!l || !c || !d || !d->session)
		return;

	REQ(r, SelectSoftKeysMessage);
	r->msg.SelectSoftKeysMessage.lel_lineInstance = htolel(lineInstance);
	r->msg.SelectSoftKeysMessage.lel_callReference = htolel(c->callid);
	r->msg.SelectSoftKeysMessage.lel_softKeySetIndex = htolel(keymode);
	//r->msg.SelectSoftKeysMessage.les_validKeyMask = 0xFFFFFFFF; /* htolel(65535); */
	validKeyMask = 0xFFFFFFFF;

	mask = 1;
	for (i = 1; i <= softkeyindex; i++) {
		mask = (mask << 1);
	}

	if (status == 0)							//disable softkey
		mask = ~(validKeyMask & mask);
	else
		mask = validKeyMask | mask;

	r->msg.SelectSoftKeysMessage.les_validKeyMask = htolel(mask);
	sccp_log((DEBUGCAT_SOFTKEY)) (VERBOSE_PREFIX_3 "%s: Send softkeyset to %s(%d) on line %d  and call %d\n", d->id, keymode2str(5), 5, lineInstance, c->callid);
	sccp_dev_send(d, r);
}
