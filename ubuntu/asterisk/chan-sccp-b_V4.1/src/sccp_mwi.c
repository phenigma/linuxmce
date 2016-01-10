
/*!
 * \file        sccp_mwi.c
 * \brief       SCCP Message Waiting Indicator Class
 * \author      Marcello Ceschia <marcello.ceschia [at] users.sourceforge.net>
 * \note                This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 *
 * $Date: 2013-04-14 21:38:18 +0200 (Sun, 14 Apr 2013) $
 * $Revision: 4566 $
 */

#include <config.h>
#include "common.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 4566 $")
#ifndef CS_AST_HAS_EVENT
#define SCCP_MWI_CHECK_INTERVAL 30
#endif
void sccp_mwi_checkLine(sccp_line_t * line);
void sccp_mwi_setMWILineStatus(sccp_device_t * d, sccp_line_t * l);
void sccp_mwi_linecreatedEvent(const sccp_event_t * event);
void sccp_mwi_deviceAttachedEvent(const sccp_event_t * event);
void sccp_mwi_addMailboxSubscription(char *mailbox, char *context, sccp_line_t * line);
void sccp_mwi_lineStatusChangedEvent(const sccp_event_t * event);

SCCP_LIST_HEAD (, sccp_mailbox_subscriber_list_t) sccp_mailbox_subscriptions;

/*!
 * start mwi module.
 */
void sccp_mwi_module_start(void)
{
	/* */
	SCCP_LIST_HEAD_INIT(&sccp_mailbox_subscriptions);

	sccp_event_subscribe(SCCP_EVENT_LINE_CREATED, sccp_mwi_linecreatedEvent, TRUE);
	sccp_event_subscribe(SCCP_EVENT_DEVICE_ATTACHED, sccp_mwi_deviceAttachedEvent, TRUE);
	sccp_event_subscribe(SCCP_EVENT_LINESTATUS_CHANGED, sccp_mwi_lineStatusChangedEvent, FALSE);
}

/*!
 * \brief Stop MWI Monitor
 * 
 * \lock
 *      - sccp_mailbox_subscriptions
 */
void sccp_mwi_module_stop()
{
	sccp_mailbox_subscriber_list_t *subscription = NULL;
	sccp_mailboxLine_t *sccp_mailboxLine = NULL;

	SCCP_LIST_LOCK(&sccp_mailbox_subscriptions);
	while ((subscription = SCCP_LIST_REMOVE_HEAD(&sccp_mailbox_subscriptions, list))) {

		/* removing lines */
		SCCP_LIST_LOCK(&subscription->sccp_mailboxLine);
		while ((sccp_mailboxLine = SCCP_LIST_REMOVE_HEAD(&subscription->sccp_mailboxLine, list))) {
			sccp_free(sccp_mailboxLine);
		}
		SCCP_LIST_UNLOCK(&subscription->sccp_mailboxLine);
		SCCP_LIST_HEAD_DESTROY(&subscription->sccp_mailboxLine);

#ifdef CS_AST_HAS_EVENT
		/* unsubscribe asterisk event */
		if (subscription->event_sub) {
			pbx_event_unsubscribe(subscription->event_sub);
		}
#else
		subscription->schedUpdate = SCCP_SCHED_DEL(subscription->schedUpdate);
#endif

		sccp_free(subscription);
	}
	SCCP_LIST_UNLOCK(&sccp_mailbox_subscriptions);
	SCCP_LIST_HEAD_DESTROY(&sccp_mailbox_subscriptions);

	sccp_event_unsubscribe(SCCP_EVENT_LINE_CREATED, sccp_mwi_linecreatedEvent);
	sccp_event_unsubscribe(SCCP_EVENT_DEVICE_ATTACHED, sccp_mwi_deviceAttachedEvent);
	sccp_event_unsubscribe(SCCP_EVENT_LINESTATUS_CHANGED, sccp_mwi_lineStatusChangedEvent);
}

#ifdef CS_AST_HAS_EVENT

/*!
 * \brief Receive MWI Event from Asterisk
 * \param event Asterisk Event
 * \param data Asterisk Data
 * 
 * \warning
 *      - line->devices is not always locked
 * 
 * \lock
 *      - subscription->sccp_mailboxLine
 *        - line
 *          - see sccp_mwi_setMWILineStatus()
 */
void sccp_mwi_event(const struct ast_event *event, void *data)
{
	sccp_mailbox_subscriber_list_t *subscription = data;
	sccp_mailboxLine_t *mailboxLine = NULL;
	sccp_line_t *line = NULL;

	pbx_log(LOG_NOTICE, "Got mwi-event\n");
	if (!subscription || !event)
		return;

	sccp_log(DEBUGCAT_EVENT) (VERBOSE_PREFIX_3 "Got mwi event for %s@%s\n", (subscription->mailbox) ? subscription->mailbox : "NULL", (subscription->context) ? subscription->context : "NULL");

	/* for calculation store previous voicemail counts */
	subscription->previousVoicemailStatistic.newmsgs = subscription->currentVoicemailStatistic.newmsgs;
	subscription->previousVoicemailStatistic.oldmsgs = subscription->currentVoicemailStatistic.oldmsgs;

	subscription->currentVoicemailStatistic.newmsgs = pbx_event_get_ie_uint(event, AST_EVENT_IE_NEWMSGS);
	subscription->currentVoicemailStatistic.oldmsgs = pbx_event_get_ie_uint(event, AST_EVENT_IE_OLDMSGS);

	SCCP_LIST_LOCK(&subscription->sccp_mailboxLine);
	SCCP_LIST_TRAVERSE(&subscription->sccp_mailboxLine, mailboxLine, list) {
		line = sccp_line_retain(mailboxLine->line);
		if (line) {
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_4 "line: %s\n", line->name);
			sccp_linedevices_t *lineDevice = NULL;

			/* update statistics for line  */
			line->voicemailStatistic.oldmsgs -= subscription->previousVoicemailStatistic.oldmsgs;
			line->voicemailStatistic.newmsgs -= subscription->previousVoicemailStatistic.newmsgs;

			line->voicemailStatistic.oldmsgs += subscription->currentVoicemailStatistic.oldmsgs;
			line->voicemailStatistic.newmsgs += subscription->currentVoicemailStatistic.newmsgs;
			/* done */

			/* notify each device on line */
			SCCP_LIST_TRAVERSE(&line->devices, lineDevice, list) {
				if (NULL != lineDevice && NULL != lineDevice->device) {
					sccp_mwi_setMWILineStatus(lineDevice->device, line);
				} else {
					sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_4 "error: null line device.\n");
				}
			}
			sccp_line_release(line);
		}
	}
	SCCP_LIST_UNLOCK(&subscription->sccp_mailboxLine);
}
#else

/*!
 * \brief MWI Progress
 * \param ptr Pointer to Mailbox Subscriber list Entry
 * \note only used for asterisk version without mwi event (scheduled check)
 * 
 * \warning
 *      - line->devices is not always locked
 * 
 * \called_from_asterisk
 * 
 * \lock
 *      - subscription->sccp_mailboxLine
 *        - line
 *          - see sccp_mwi_setMWILineStatus()
 */
int sccp_mwi_checksubscription(const void *ptr)
{
	sccp_mailbox_subscriber_list_t *subscription = (sccp_mailbox_subscriber_list_t *) ptr;
	sccp_line_t *line = NULL;
	sccp_mailboxLine_t *mailboxLine = NULL;

	if (!subscription)
		return -1;

	subscription->previousVoicemailStatistic.newmsgs = subscription->currentVoicemailStatistic.newmsgs;
	subscription->previousVoicemailStatistic.oldmsgs = subscription->currentVoicemailStatistic.oldmsgs;

	char buffer[512];

	sprintf(buffer, "%s@%s", subscription->mailbox, (subscription->context) ? subscription->context : "default");
	sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_4 "SCCP: ckecking mailbox: %s\n", buffer);
	pbx_app_inboxcount(buffer, &subscription->currentVoicemailStatistic.newmsgs, &subscription->currentVoicemailStatistic.oldmsgs);

	/* update devices if something changed */
	if (subscription->previousVoicemailStatistic.newmsgs != subscription->currentVoicemailStatistic.newmsgs) {
		SCCP_LIST_LOCK(&subscription->sccp_mailboxLine);
		SCCP_LIST_TRAVERSE(&subscription->sccp_mailboxLine, mailboxLine, list) {
			line = sccp_line_retain(mailboxLine->line);
			if (line) {
				sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_4 "line: %s\n", line->name);
				sccp_linedevices_t *lineDevice = NULL;

				/* update statistics for line  */
				line->voicemailStatistic.oldmsgs -= subscription->previousVoicemailStatistic.oldmsgs;
				line->voicemailStatistic.newmsgs -= subscription->previousVoicemailStatistic.newmsgs;

				line->voicemailStatistic.oldmsgs += subscription->currentVoicemailStatistic.oldmsgs;
				line->voicemailStatistic.newmsgs += subscription->currentVoicemailStatistic.newmsgs;
				/* done */

				/* notify each device on line */
				SCCP_LIST_TRAVERSE(&line->devices, lineDevice, list) {
					sccp_mwi_setMWILineStatus(lineDevice->device, line);
				}
				sccp_line_release(line);
			}
		}
		SCCP_LIST_UNLOCK(&subscription->sccp_mailboxLine);
	}

	/* reschedule my self */
	if ((subscription->schedUpdate = sccp_sched_add(sched, SCCP_MWI_CHECK_INTERVAL * 1000, sccp_mwi_checksubscription, subscription)) < 0) {
		pbx_log(LOG_ERROR, "Error creating mailbox subscription.\n");
	}
	return 0;
}
#endif

/*!
 * \brief Remove Mailbox Subscription
 * \param mailbox SCCP Mailbox
 * \todo Implement sccp_mwi_unsubscribeMailbox ( \todo Marcello)
 */
void sccp_mwi_unsubscribeMailbox(sccp_mailbox_t ** mailbox)
{

	// \todo implement sccp_mwi_unsubscribeMailbox
	return;
}

/*!
 * \brief Device Attached Event 
 * \param event SCCP Event
 * 
 * \lock
 *      - device
 */
void sccp_mwi_deviceAttachedEvent(const sccp_event_t * event)
{
	if (!event)
		return;

	sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_1 "SCCP: (mwi_deviceAttachedEvent) Get deviceAttachedEvent\n");
	sccp_line_t *line = event->event.deviceAttached.linedevice->line;
	sccp_device_t *device = event->event.deviceAttached.linedevice->device;

	if (!line || !(device = sccp_device_retain(device))) {
		pbx_log(LOG_ERROR, "get deviceAttachedEvent where one parameter is missing. device: %s, line: %s\n", DEV_ID_LOG(device), (line) ? line->name : "null");
		return;
	}

	device->voicemailStatistic.oldmsgs += line->voicemailStatistic.oldmsgs;
	device->voicemailStatistic.newmsgs += line->voicemailStatistic.newmsgs;
	sccp_mwi_setMWILineStatus(device, line);								/* set mwi-line-status */
	device = sccp_device_release(device);
}

/*!
 * \brief Line Status Changed Event 
 * \param event SCCP Event
 * 
 * \lock
 *      - see sccp_mwi_check()
 */
void sccp_mwi_lineStatusChangedEvent(const sccp_event_t * event)
{
	if (!event)
		return;

	sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_1 "SCCP: (mwi_lineStatusChangedEvent) Get lineStatusChangedEvent\n");
	sccp_device_t *device = event->event.lineStatusChanged.device;

	if (!device)
		return;

	sccp_mwi_check(device);
}

/*!
 * \brief Line Created Event 
 * \param event SCCP Event
 * 
 * \warning
 *      - line->mailboxes is not always locked
 */
void sccp_mwi_linecreatedEvent(const sccp_event_t * event)
{
	if (!event)
		return;

	sccp_mailbox_t *mailbox;
	sccp_line_t *line = event->event.lineCreated.line;

	sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_1 "SCCP: (mwi_linecreatedEvent) Get linecreatedEvent\n");

	if (!line) {
		pbx_log(LOG_ERROR, "Get linecreatedEvent, but line not set\n");
		return;
	}

	if (line && (&line->mailboxes) != NULL) {
		SCCP_LIST_TRAVERSE(&line->mailboxes, mailbox, list) {
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_1 "line: '%s' mailbox: %s@%s\n", line->name, mailbox->mailbox, (mailbox->context) ? mailbox->context : "default");
			sccp_mwi_addMailboxSubscription(mailbox->mailbox, (mailbox->context) ? mailbox->context : "default", line);
		}
	}
	return;
}

/*!
 * \brief Add Mailbox Subscription
 * \param mailbox Mailbox as char
 * \param context Mailbox Context
 * \param line SCCP Line
 * 
 * \warning
 *      - subscription->sccp_mailboxLine is not always locked
 * 
 * \lock
 *      - sccp_mailbox_subscriptions
 *      - subscription->sccp_mailboxLine
 */
void sccp_mwi_addMailboxSubscription(char *mailbox, char *context, sccp_line_t * line)
{
	sccp_mailbox_subscriber_list_t *subscription = NULL;
	sccp_mailboxLine_t *mailboxLine = NULL;

	SCCP_LIST_LOCK(&sccp_mailbox_subscriptions);
	SCCP_LIST_TRAVERSE(&sccp_mailbox_subscriptions, subscription, list) {
		if (strlen(mailbox) == strlen(subscription->mailbox)
		    && strlen(context) == strlen(subscription->context)
		    && !strcmp(mailbox, subscription->mailbox)
		    && !strcmp(context, subscription->context)) {
			break;
		}
	}
	SCCP_LIST_UNLOCK(&sccp_mailbox_subscriptions);

	if (!subscription) {
		subscription = sccp_malloc(sizeof(sccp_mailbox_subscriber_list_t));
		if (!subscription) {
			pbx_log(LOG_ERROR, "SCCP: (mwi_addMailboxSubscription) Error allocating memory for sccp_mwi_addMailboxSubscription");
			return;
		}
		memset(subscription, 0, sizeof(sccp_mailbox_subscriber_list_t));

		SCCP_LIST_HEAD_INIT(&subscription->sccp_mailboxLine);

		sccp_copy_string(subscription->mailbox, mailbox, sizeof(subscription->mailbox));
		sccp_copy_string(subscription->context, context, sizeof(subscription->context));
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "SCCP: (mwi_addMailboxSubscription) create subscription for: %s@%s\n", subscription->mailbox, subscription->context);

		SCCP_LIST_LOCK(&sccp_mailbox_subscriptions);
		SCCP_LIST_INSERT_HEAD(&sccp_mailbox_subscriptions, subscription, list);
		SCCP_LIST_UNLOCK(&sccp_mailbox_subscriptions);

		/* get initial value */
		char buffer[512];

		sprintf(buffer, "%s@%s", subscription->mailbox, (subscription->context) ? subscription->context : "default");
		pbx_app_inboxcount(buffer, &subscription->currentVoicemailStatistic.newmsgs, &subscription->currentVoicemailStatistic.oldmsgs);

#ifdef CS_AST_HAS_EVENT
		/* register asterisk event */
		//struct pbx_event_sub *pbx_event_subscribe(enum ast_event_type event_type, ast_event_cb_t cb, char *description, void *userdata, ...);
#if ASTERISK_VERSION_NUMBER >= 10800
		subscription->event_sub = pbx_event_subscribe(AST_EVENT_MWI, sccp_mwi_event, "mailbox subscription", subscription, AST_EVENT_IE_MAILBOX, AST_EVENT_IE_PLTYPE_STR, subscription->mailbox, AST_EVENT_IE_CONTEXT, AST_EVENT_IE_PLTYPE_STR, S_OR(subscription->context, "default"), AST_EVENT_IE_END);
#else
		subscription->event_sub = pbx_event_subscribe(AST_EVENT_MWI, sccp_mwi_event, subscription, AST_EVENT_IE_MAILBOX, AST_EVENT_IE_PLTYPE_STR, subscription->mailbox, AST_EVENT_IE_CONTEXT, AST_EVENT_IE_PLTYPE_STR, S_OR(subscription->context, "default"), AST_EVENT_IE_END);
#endif

#else
		if ((subscription->schedUpdate = sccp_sched_add(sched, SCCP_MWI_CHECK_INTERVAL * 1000, sccp_mwi_checksubscription, subscription)) < 0) {
			pbx_log(LOG_ERROR, "SCCP: (mwi_addMailboxSubscription) Error creating mailbox subscription.\n");
		}
#endif
	}

	/* we already have this subscription */
	SCCP_LIST_TRAVERSE(&subscription->sccp_mailboxLine, mailboxLine, list) {
		if (line == mailboxLine->line)
			break;
	}

	if (!mailboxLine) {
		mailboxLine = sccp_malloc(sizeof(sccp_mailboxLine_t));
		if (!mailboxLine) {
			pbx_log(LOG_ERROR, "SCCP: (mwi_addMailboxSubscription) Error allocating memory for mailboxLine");
			return;
		}
		memset(mailboxLine, 0, sizeof(sccp_mailboxLine_t));

		mailboxLine->line = line;

		line->voicemailStatistic.newmsgs = subscription->currentVoicemailStatistic.newmsgs;
		line->voicemailStatistic.oldmsgs = subscription->currentVoicemailStatistic.oldmsgs;

		SCCP_LIST_LOCK(&subscription->sccp_mailboxLine);
		SCCP_LIST_INSERT_HEAD(&subscription->sccp_mailboxLine, mailboxLine, list);
		SCCP_LIST_UNLOCK(&subscription->sccp_mailboxLine);
	}
}

/*!
 * \brief Check Line for MWI Status
 * \param line SCCP Line
 * 
 * \lock
 *      - line->mailboxes
 */
void sccp_mwi_checkLine(sccp_line_t * line)
{
	sccp_mailbox_t *mailbox = NULL;
	char buffer[512];

	SCCP_LIST_LOCK(&line->mailboxes);
	SCCP_LIST_TRAVERSE(&line->mailboxes, mailbox, list) {
		sprintf(buffer, "%s@%s", mailbox->mailbox, (mailbox->context) ? mailbox->context : "default");
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "SCCP: (mwi_checkLine) Line: %s, Mailbox: %s\n", line->name, buffer);
		if (!sccp_strlen_zero(buffer)) {

#ifdef CS_AST_HAS_NEW_VOICEMAIL
			pbx_app_inboxcount(buffer, &line->voicemailStatistic.newmsgs, &line->voicemailStatistic.oldmsgs);
#else
			if (pbx_app_has_voicemail(buffer)) {
				line->voicemailStatistic.newmsgs = 1;
			}
#endif
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "SCCP: (mwi_checkLine) Line: %s, Mailbox: %s inbox: %d/%d\n", line->name, buffer, line->voicemailStatistic.newmsgs, line->voicemailStatistic.oldmsgs);
		}
	}
	SCCP_LIST_UNLOCK(&line->mailboxes);
}

/*!
 * \brief Set MWI Line Status
 * \param d SCCP Device
 * \param l SCCP Line
 * 
 * \lock
 *      - device
 *        - see sccp_device_find_index_for_line()
 *        - see sccp_dev_send()
 *        - see sccp_mwi_check()
 */
void sccp_mwi_setMWILineStatus(sccp_device_t * d, sccp_line_t * l)
{
	sccp_moo_t *r;
	int instance = 0;
	uint8_t status = 0;
	uint32_t mask;
	uint32_t newState = 0;

	if (!(d = sccp_device_retain(d)))
		return;

	/* when l is defined we are switching on/off the button icon */
	if (l) {
		instance = sccp_device_find_index_for_line(d, l->name);
		status = l->voicemailStatistic.newmsgs ? 1 : 0;
	}

	mask = 1 << instance;

	newState = d->mwilight;
	/* update status */
	if (status) {
		/* activate */
		newState |= mask;
	} else {
		/* deactivate */
		newState &= ~mask;
	}

	/* check if we need to update line status */
	if ((d->mwilight & ~(1 << 0)) != (newState & ~(1 << 0))) {

		d->mwilight = newState;

		REQ(r, SetLampMessage);
		r->msg.SetLampMessage.lel_stimulus = htolel(SKINNY_STIMULUS_VOICEMAIL);
		r->msg.SetLampMessage.lel_stimulusInstance = htolel(instance);
		r->msg.SetLampMessage.lel_lampMode = (d->mwilight & ~(1 << 0)) ? htolel(d->mwilamp) : htolel(SKINNY_LAMP_OFF);

		sccp_dev_send(d, r);
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: (mwi_setMWILineStatus) Turn %s the MWI on line (%s)%d\n", DEV_ID_LOG(d), (mask > 0) ? "ON" : "OFF", (l ? l->name : "unknown"), instance);
	} else {
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: (mwi_setMWILineStatus) Device already knows status %s on line %s (%d)\n", DEV_ID_LOG(d), (newState & ~(1 << 0)) ? "ON" : "OFF", (l ? l->name : "unknown"), instance);
	}

	sccp_mwi_check(d);
	sccp_device_release(d);
}

/*!
 * \brief Check MWI Status for Device
 * \param device SCCP Device
 * \note called by lineStatusChange
 * 
 * \lock
 *      - device->buttonconfig
 *        - see sccp_line_find_byname()
 *        - line->channels
 *      - device
 *        - device->buttonconfig
 *          - see sccp_line_find_byname()
 *        - see sccp_dev_send()
 *        - see sccp_dev_check_displayprompt()
 */
void sccp_mwi_check(sccp_device_t * device)
{
	sccp_buttonconfig_t *config = NULL;

	sccp_line_t *line = NULL;
	sccp_channel_t *c = NULL;

	sccp_moo_t *r = NULL;

	uint8_t status;
	uint32_t mask;

	uint32_t oldmsgs = 0, newmsgs = 0;

	/* check if we have an active channel */
	boolean_t hasActiveChannel = FALSE, hasRinginChannel = FALSE;

	if (!(device = sccp_device_retain(device))) {
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "SCCP: (mwi_check) called with NULL device!\n");
		return;
	}

	/* for each line, check if there is an active call */
	sccp_device_t *tmpDevice = NULL;

	SCCP_LIST_LOCK(&device->buttonconfig);
	SCCP_LIST_TRAVERSE(&device->buttonconfig, config, list) {
		if (config->type == LINE) {
			line = sccp_line_find_byname(config->button.line.name, FALSE);
			if (!line) {
				sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: NULL line retrieved from buttonconfig!\n", DEV_ID_LOG(device));
				continue;
			}
			SCCP_LIST_LOCK(&line->channels);
			SCCP_LIST_TRAVERSE(&line->channels, c, list) {
				tmpDevice = sccp_channel_getDevice_retained(c);
				if (tmpDevice == device) {							// We have a channel belonging to our device (no remote shared line channel)
					if (c->state != SCCP_CHANNELSTATE_ONHOOK && c->state != SCCP_CHANNELSTATE_DOWN) {
						hasActiveChannel = TRUE;
					}
					if (c->state == SCCP_CHANNELSTATE_RINGING) {
						hasRinginChannel = TRUE;
					}
				}
				tmpDevice = tmpDevice ? sccp_device_release(tmpDevice) : NULL;
			}
			/* pre-collect number of voicemails on device to be set later */
			oldmsgs += line->voicemailStatistic.oldmsgs;
			newmsgs += line->voicemailStatistic.newmsgs;
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: (mwi_check) line retrieved from buttonconfig! (%d/%d)\n", DEV_ID_LOG(device), line->voicemailStatistic.newmsgs, line->voicemailStatistic.oldmsgs);
			SCCP_LIST_UNLOCK(&line->channels);
			line = sccp_line_release(line);
		}
	}
	SCCP_LIST_UNLOCK(&device->buttonconfig);

	/* disable mwi light if we have an active channel, but no ringin */
	if (hasActiveChannel && !hasRinginChannel && !device->mwioncall) {
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: we have an active channel, disable mwi light\n", DEV_ID_LOG(device));

		if (device->mwilight & (1 << 0)) {								// Set the MWI light to off only if it is already on.
			device->mwilight &= ~(1 << 0);								/* set mwi light for device to off */

			REQ(r, SetLampMessage);
			r->msg.SetLampMessage.lel_stimulus = htolel(SKINNY_STIMULUS_VOICEMAIL);
			r->msg.SetLampMessage.lel_stimulusInstance = 0;
			r->msg.SetLampMessage.lel_lampMode = htolel(SKINNY_LAMP_OFF);
			sccp_dev_send(device, r);
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: Turn %s the MWI on line (%s) %d\n", DEV_ID_LOG(device), "OFF", "unknown", 0);
		} else {
			sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: MWI already %s on line (%s) %d\n", DEV_ID_LOG(device), "OFF", "unknown", 0);
		}
		device = sccp_device_release(device);
		return;												// <---- This return must be outside the inner if
	}

	/* Note: We must return the function before this point unless we want to turn the MWI on during a call! */
	/*       This is taken care of by the previous block of code. */
	device->voicemailStatistic.newmsgs = oldmsgs;
	device->voicemailStatistic.oldmsgs = newmsgs;

	/* set mwi light */
	mask = device->mwilight & ~(1 << 0);									/* status without mwi light for device (1<<0) */
	status = (mask > 0) ? 1 : 0;

	if ((device->mwilight & (1 << 0)) != status) {								/* status needs update */
		if (status) {
			device->mwilight |= (1 << 0);								/* activate */
		} else {
			device->mwilight &= ~(1 << 0);								/* deactivate */
		}

		REQ(r, SetLampMessage);
		r->msg.SetLampMessage.lel_stimulus = htolel(SKINNY_STIMULUS_VOICEMAIL);
		//r->msg.SetLampMessage.lel_stimulusInstance = 0;
		r->msg.SetLampMessage.lel_lampMode = htolel((device->mwilight) ? device->mwilamp : SKINNY_LAMP_OFF);
		sccp_dev_send(device, r);
		sccp_log(DEBUGCAT_MWI) (VERBOSE_PREFIX_3 "%s: Turn %s the MWI light (newmsgs: %d->%d)\n", DEV_ID_LOG(device), (device->mwilight & (1 << 0)) ? "ON" : "OFF", newmsgs, device->voicemailStatistic.newmsgs);

	}
	/* we should check the display only once, maybe we need a priority stack -MC */
	if (newmsgs > 0) {
		char buffer[StationMaxDisplayTextSize];
		sprintf(buffer, "%s: (%d/%d)", SKINNY_DISP_YOU_HAVE_VOICEMAIL, newmsgs, oldmsgs);
		sccp_device_addMessageToStack(device, SCCP_MESSAGE_PRIORITY_VOICEMAIL, buffer);
	} else {
		sccp_device_clearMessageFromStack(device, SCCP_MESSAGE_PRIORITY_VOICEMAIL);
	}

	device = sccp_device_release(device);
}

/*!
 * \brief Show MWI Subscriptions
 * \param fd Fd as int   
 * \param total Total number of lines as int
 * \param s AMI Session 
 * \param m Message
 * \param argc Argc as int
 * \param argv[] Argv[] as char
 * \return Result as int
 * 
 * \called_from_asterisk
 */
#include <asterisk/cli.h>
int sccp_show_mwi_subscriptions(int fd, int *total, struct mansession *s, const struct message *m, int argc, char *argv[])
{
	sccp_line_t *line = NULL;
	sccp_mailboxLine_t *mailboxLine = NULL;
	char linebuf[30] = "";
	int local_total = 0;

#define CLI_AMI_TABLE_NAME MWI_Subscriptions
#define CLI_AMI_TABLE_PER_ENTRY_NAME Mailbox_Subscriber
#define CLI_AMI_TABLE_LIST_ITER_HEAD &sccp_mailbox_subscriptions
#define CLI_AMI_TABLE_LIST_ITER_TYPE sccp_mailbox_subscriber_list_t
#define CLI_AMI_TABLE_LIST_ITER_VAR subscription
#define CLI_AMI_TABLE_LIST_LOCK SCCP_LIST_LOCK
#define CLI_AMI_TABLE_LIST_ITERATOR SCCP_LIST_TRAVERSE
#define CLI_AMI_TABLE_LIST_UNLOCK SCCP_LIST_UNLOCK
#define CLI_AMI_TABLE_BEFORE_ITERATION 											\
 		SCCP_LIST_TRAVERSE(&subscription->sccp_mailboxLine, mailboxLine, list) {					\
 			line = mailboxLine->line;										\
 			sprintf(linebuf,"%s",line->name);									\
/* 			if (line->name) {										*/	\
/*				ast_join(linebuf, sizeof(linebuf), (const char * const *) line->name);			*/	\
/* 			}												*/	\
 		}

#define CLI_AMI_TABLE_FIELDS 												\
 		CLI_AMI_TABLE_FIELD(Mailbox,			s,	10,	subscription->mailbox)				\
 		CLI_AMI_TABLE_FIELD(LineName,			s,	30,	linebuf)					\
 		CLI_AMI_TABLE_FIELD(Context,			s,	15,	subscription->context)				\
 		CLI_AMI_TABLE_FIELD(New,			d,	3,	subscription->currentVoicemailStatistic.newmsgs)\
 		CLI_AMI_TABLE_FIELD(Old,			d,	3,	subscription->currentVoicemailStatistic.oldmsgs)
#include "sccp_cli_table.h"

	if (s)
		*total = local_total;

	return RESULT_SUCCESS;
}
