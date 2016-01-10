
/*!
 * \file        sccp_features.c
 * \brief       SCCP Features Class
 * \author      Federico Santulli <fsantulli [at] users.sourceforge.net >
 * \note                This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 * \since       2009-01-16
 *
 * $Date: 2013-04-25 21:39:21 +0200 (Thu, 25 Apr 2013) $
 * $Revision: 4582 $
 */

/*!
 * \remarks     Purpose:        SCCP Features
 *              When to use:    Only methods directly related to handling phone features should be stored in this source file.
 *                              Phone Features are Capabilities of the phone, like:
 *                                - CallForwarding
 *                                - Dialing
 *                                - Changing to Do Not Disturb(DND) Status
 *              Relationships:  These Features are called by FeatureButtons. Features can in turn call on Actions.
 *
 */

#include <config.h>
#include "common.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 4582 $")

/*!
 * \brief Handle Call Forwarding
 * \param l SCCP Line
 * \param device SCCP Device
 * \param type CallForward Type (NONE, ALL, BUSY, NOANSWER) as SCCP_CFWD_*
 * \return SCCP Channel
 *
 * \callgraph
 * \callergraph
 *
 * \lock
 *      - channel
 *        - see sccp_channel_set_active()
 *        - see sccp_indicate_nolock()
 */
void sccp_feat_handle_callforward(sccp_line_t * l, sccp_device_t * device, uint8_t type)
{
	sccp_channel_t *c = NULL;
	sccp_linedevices_t *linedevice = NULL;

	if (!l) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line is not specified!\n");
		return;
	}

	if (!device) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if device is not specified!\n");
		return;
	}

	linedevice = sccp_linedevice_find(device, l);
	if (!linedevice) {
		pbx_log(LOG_ERROR, "%s: Device does not have line configured \n", DEV_ID_LOG(device));
		return;
	}

	/* if call forward is active and you asked about the same call forward maybe you would disable */
	if ((linedevice->cfwdAll.enabled && type == SCCP_CFWD_ALL)
	    || (linedevice->cfwdBusy.enabled && type == SCCP_CFWD_BUSY)) {

		sccp_line_cfwd(l, device, SCCP_CFWD_NONE, NULL);
		goto EXIT;
	} else {
		if (type == SCCP_CFWD_NOANSWER) {
			sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "### CFwdNoAnswer NOT SUPPORTED\n");
			sccp_dev_displayprompt(device, 0, 0, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
			goto EXIT;
		}
	}

	/* look if we have a call  */
	if ((c = sccp_channel_get_active(device))) {
		if (c->ss_action == SCCP_SS_GETFORWARDEXTEN) {
			// we have a channel, checking if
			if (c->state == SCCP_CHANNELSTATE_RINGOUT || c->state == SCCP_CHANNELSTATE_CONNECTED || c->state == SCCP_CHANNELSTATE_PROCEED || c->state == SCCP_CHANNELSTATE_BUSY || c->state == SCCP_CHANNELSTATE_CONGESTION) {
				if (c->calltype == SKINNY_CALLTYPE_OUTBOUND) {
					pbx_log(LOG_ERROR, "%s: 1\n", DEV_ID_LOG(device));
					// if we have an outbound call, we can set callforward to dialed number -FS
					if (c->dialedNumber && !sccp_strlen_zero(c->dialedNumber)) {		// checking if we have a number !
						pbx_log(LOG_ERROR, "%s: 2\n", DEV_ID_LOG(device));
						sccp_line_cfwd(l, device, type, c->dialedNumber);
						// we are on call, so no tone has been played until now :)
						//sccp_dev_starttone(device, SKINNY_TONE_ZIPZIP, instance, 0, 0);
						sccp_channel_endcall(c);
						goto EXIT;
					}
				} else if (c->owner && ast_bridged_channel(c->owner)) {				// check if we have an ast channel to get callerid from
					// if we have an incoming or forwarded call, let's get number from callerid :) -FS
					char *number = NULL;

					pbx_log(LOG_ERROR, "%s: 3\n", DEV_ID_LOG(device));

					if (PBX(get_callerid_name))
						PBX(get_callerid_number) (c, &number);
					if (number) {
						sccp_line_cfwd(l, device, type, number);
						pbx_log(LOG_ERROR, "%s: 4\n", DEV_ID_LOG(device));
						// we are on call, so no tone has been played until now :)
						sccp_dev_starttone(device, SKINNY_TONE_ZIPZIP, linedevice->lineInstance, 0, 0);
						sccp_channel_endcall(c);
						sccp_free(number);
						goto EXIT;
					}
					// if we where here it's cause there is no number in callerid,, so put call on hold and ask for a call forward number :) -FS
					if (!sccp_channel_hold(c)) {
						// if can't hold  it means there is no active call, so return as we're already waiting a number to dial
						sccp_dev_displayprompt(device, 0, 0, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
						goto EXIT;
					}
				}
			} else if (c->state == SCCP_CHANNELSTATE_OFFHOOK && sccp_strlen_zero(c->dialedNumber)) {
				pbx_log(LOG_ERROR, "%s: 5\n", DEV_ID_LOG(device));
				// we are dialing but without entering a number :D -FS
				sccp_dev_stoptone(device, linedevice->lineInstance, (c && c->callid) ? c->callid : 0);
				// changing SS_DIALING mode to SS_GETFORWARDEXTEN
				c->ss_action = SCCP_SS_GETFORWARDEXTEN;						/* Simpleswitch will catch a number to be dialed */
				c->ss_data = type;								/* this should be found in thread */
				// changing channelstate to GETDIGITS
				sccp_indicate(device, c, SCCP_CHANNELSTATE_GETDIGITS);
				goto EXIT;
			} else {
				// we cannot allocate a channel, or ask an extension to pickup.
				sccp_dev_displayprompt(device, 0, 0, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
				goto EXIT;
			}
		} else {
			/* see case for channel */
		}
	}

	if (!c) {
		// if we where here there is no call in progress, so we should allocate a channel.
		c = sccp_channel_allocate(l, device);
		if (!c) {
			pbx_log(LOG_ERROR, "%s: Can't allocate SCCP channel for line %s\n", DEV_ID_LOG(device), l->name);
			goto EXIT;
		}
		sccp_channel_set_active(device, c);

		if (!sccp_pbx_channel_allocate(c, NULL)) {
			pbx_log(LOG_WARNING, "%s: (handle_callforward) Unable to allocate a new channel for line %s\n", DEV_ID_LOG(device), l->name);
			sccp_indicate(device, c, SCCP_CHANNELSTATE_CONGESTION);					// implicitly retained device by sccp_action
			goto EXIT;
		}

	} else {
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK) {

			/** we just opened a channel for cfwd, switch ss_action = SCCP_SS_GETFORWARDEXTEN */
			c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(device, linedevice->lineInstance, c->callid);

		} else {
			// other call in progress, put on hold
			int ret = sccp_channel_hold(c);

			if (!ret) {
				pbx_log(LOG_ERROR, "%s: Active call '%d' could not be put on hold\n", DEV_ID_LOG(device), c->callid);
				goto EXIT;
			}
		}
	}

	c->ss_action = SCCP_SS_GETFORWARDEXTEN;									/* Simpleswitch will catch a number to be dialed */
	c->ss_data = type;											/* this should be found in thread */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;
	sccp_indicate(device, c, SCCP_CHANNELSTATE_GETDIGITS);
	sccp_dev_displayprompt(device, linedevice->lineInstance, c->callid, "Enter number to cfwd", 0);

	PBX(set_callstate) (c, AST_STATE_OFFHOOK);

	if (device->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel(c);
	}

EXIT:
	linedevice = linedevice ? sccp_linedevice_release(linedevice) : NULL;
	c = c ? sccp_channel_release(c) : NULL;
}

#ifdef CS_SCCP_PICKUP

/*!
 * \brief Handle Direct Pickup of Line
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 *
 * \lock
 *      - channel
 *        - see sccp_channel_set_active()
 *        - see sccp_indicate_nolock()
 */
void sccp_feat_handle_directed_pickup(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || strlen(d->id) < 3) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETPICKUPEXTEN;							/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;										/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			c = sccp_channel_release(c);
			return;
		} else {
			/* there is an active call, let's put it on hold first */
			if (!sccp_channel_hold(c)) {
				c = sccp_channel_release(c);
				return;
			}
		}
		c = sccp_channel_release(c);
	}

	c = sccp_channel_allocate(l, d);

	if (!c) {
		pbx_log(LOG_ERROR, "%s: (handle_directed_pickup) Can't allocate SCCP channel for line %s\n", d->id, l->name);
		return;
	}

	c->ss_action = SCCP_SS_GETPICKUPEXTEN;									/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;												/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate(c, NULL)) {
		pbx_log(LOG_WARNING, "%s: (handle_directed_pickup) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_CONGESTION);
		c = sccp_channel_release(c);
		return;
	}

	PBX(set_callstate) (c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel(c);
	}

	c = sccp_channel_release(c);
}
#endif

#ifdef CS_SCCP_PICKUP

/*!
 * \brief Handle Direct Pickup of Extension
 * \param c *locked* SCCP Channel
 * \param exten Extension as char
 * \return Success as int
 *
 * \lock
 *      - asterisk channel
 *        - device
 */
int sccp_feat_directed_pickup(sccp_channel_t * c, char *exten)
{
	int res = -1;
#if CS_AST_DO_PICKUP
	PBX_CHANNEL_TYPE *target = NULL;		/* potential pickup target */
	PBX_CHANNEL_TYPE *original = NULL;		/* destination */
	PBX_CHANNEL_TYPE *tmp = NULL;
	const char *ringermode = NULL;

	sccp_device_t *d;
	char *context;
	char *name = NULL;
	char *number = NULL;
	sccp_channel_t *tmpChannel;

        if (!PBX(findPickupChannelByExtenLocked)) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) findPickupChannelByExtenLocked not implemented for this asterisk version\n");
		return -1;
        }

	if (sccp_strlen_zero(exten)) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) zero exten\n");
		return -1;
	}

	if (!c || !c->owner) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) no channel\n");
		return -1;
	}

	original = c->owner;

	if (!c->line || !(d = sccp_channel_getDevice_retained(c))) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) no device\n");
		return -1;
	}

	/* copying extension into our buffer */
        if ((context = strchr(exten, '@'))) {
               *context++ = '\0';
	} else {
                if (!sccp_strlen_zero(d->directed_pickup_context)) {
                        context = (char *) strdup(d->directed_pickup_context);
                } else {
                        context = (char *) pbx_channel_context(original);
                }
        }
        target = PBX(findPickupChannelByExtenLocked)(original, exten, context);
        if (target) {
                tmp = (CS_AST_BRIDGED_CHANNEL(target) ? CS_AST_BRIDGED_CHANNEL(target) : target);

                /* fixup callinfo */
                tmpChannel = CS_AST_CHANNEL_PVT(target);
                if (tmpChannel) {
                        if (PBX(get_callerid_name))
                                PBX(get_callerid_name) (tmpChannel, &name);

                        if (PBX(get_callerid_number))
                                PBX(get_callerid_number) (tmpChannel, &number);
                }
                pbx_log(LOG_NOTICE, "SCCP: (directed_pickup) %s callerid is ('%s'-'%s')\n", pbx_channel_name(tmp), name ? name : "", number ? number : "");
                tmp = NULL;
                res = 0;
                if (d->directed_pickup_modeanswer) {
                        if ((res = ast_answer(c->owner))) {						// \todo: remove res in this line: Although the value stored to 'res' is used in the enclosing expression, the value is never actually read from 'res'
                                sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) Unable to answer '%s'\n", PBX(getChannelName) (c));
                                res = -1;
                        } else if ((res = PBX(queue_control) (c->owner, AST_CONTROL_ANSWER))) {
                                sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) Unable to queue answer on '%s'\n", PBX(getChannelName) (c));
                                res = -1;
                        }
                }

                if (res == 0) {
                        SCCP_SCHED_DEL(c->scheduler.digittimeout);
                        c->calltype = SKINNY_CALLTYPE_INBOUND;
                        c->state = SCCP_CHANNELSTATE_PROCEED;
                        c->answered_elsewhere = TRUE;
                        
                        res = ast_do_pickup(c->owner, target);
                        if (!res) {
                                /* directed pickup succeeded */
                                sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: (directed_pickup) pickup succeeded on callid: %d\n", DEV_ID_LOG(d), c->callid);
                                sccp_rtp_stop(c);										/* stop previous audio */
                                pbx_channel_set_hangupcause(original, AST_CAUSE_ANSWERED_ELSEWHERE);
                                pbx_hangup(original);										/* hangup masqueraded zombie channel */

                                pbx_channel_set_hangupcause(c->owner, AST_CAUSE_NORMAL_CLEARING);				/* reset picked up channel */
                                sccp_channel_setDevice(c, d);
                                sccp_channel_set_active(d, c);
                                sccp_channel_updateChannelCapability(c);
                                if (d->directed_pickup_modeanswer) {
                                        sccp_indicate(d, c, SCCP_CHANNELSTATE_CONNECTED);
                                } else {
                                        uint8_t instance;
                                        instance = sccp_device_find_index_for_line(d, c->line->name);
                                        sccp_dev_stoptone(d, instance, c->callid);
                                        sccp_dev_set_speaker(d, SKINNY_STATIONSPEAKER_OFF);
                                        sccp_channel_set_active(d, NULL);
                                        c->ringermode = SKINNY_STATION_OUTSIDERING;			// default ring
                                        ringermode = pbx_builtin_getvar_helper(c->owner, "ALERT_INFO");
                                        if (ringermode && !sccp_strlen_zero(ringermode)) {
                                                sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: Found ALERT_INFO=%s\n", ringermode);
                                                if (strcasecmp(ringermode, "inside") == 0)
                                                        c->ringermode = SKINNY_STATION_INSIDERING;
                                                else if (strcasecmp(ringermode, "feature") == 0)
                                                        c->ringermode = SKINNY_STATION_FEATURERING;
                                                else if (strcasecmp(ringermode, "silent") == 0)
                                                        c->ringermode = SKINNY_STATION_SILENTRING;
                                                else if (strcasecmp(ringermode, "urgent") == 0)
                                                        c->ringermode = SKINNY_STATION_URGENTRING;
                                        }
                                        sccp_indicate(d, c, SCCP_CHANNELSTATE_RINGING);
                                }
                        }
                } else {
                	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) Giving Up\n");
                }
                target = pbx_channel_unref(target);
                pbx_channel_unlock(target);
        } else {
                pbx_channel_unlock(target);
	}
	if (!res) {
	        sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) Exten '%s@'%s' Picked up Succesfully\n", exten, context);
        } else {
	        sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (directed_pickup) Failed to pickup up Exten '%s@'%s'\n", exten, context);
        }
	d = sccp_device_release(d);
#endif	
	return res;
}

/*!
 * \brief Handle Group Pickup Feature
 * \param l SCCP Line
 * \param d SCCP Device
 * \return Success as int
 * 
 * \todo backport from trunk
 *
 * \lock
 *      - asterisk channel
 *        - channel
 *          - see sccp_indicate_nolock()
 *          - see sccp_device_find_index_for_line()
 *          - see sccp_dev_stoptone()
 *          - see sccp_dev_set_speaker()
 *          - device
 *          - see sccp_indicate_nolock()
 *
 * \todo Fix callerid setting before calling ast_pickup_call
 */
int sccp_feat_grouppickup(sccp_line_t * l, sccp_device_t * d)
{
	int res = 0;
	PBX_CHANNEL_TYPE *dest = NULL;
	sccp_channel_t *c;

	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: (grouppickup) no line or device\n");
		return -1;
	}
	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) starting grouppickup\n", DEV_ID_LOG(d));

	if (!l->pickupgroup) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) pickupgroup not configured in sccp.conf\n", d->id);
		return -1;
	}

	if (!PBX(feature_pickup)) {
		pbx_log(LOG_ERROR, "%s: (grouppickup) GPickup feature not implemented\n", d->id);
	}

	/* create channel for pickup */
	if ((c = sccp_channel_find_bystate_on_line(l, SCCP_CHANNELSTATE_OFFHOOK)) && !pbx_test_flag(pbx_channel_flags(c->owner), AST_FLAG_ZOMBIE)) {
        	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) Already offhook, reusing callid %d\n", d->id, c->callid);
		dest = c->owner;
	} else {
		/* emulate a new call so we end up in the same state as when a new call has been started */
        	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) Starting new channel\n", d->id);
		c = sccp_channel_newcall(l, d, NULL, SKINNY_CALLTYPE_OUTBOUND, NULL);
		dest = c->owner;
	}
	/* prepare for call pickup */
	SCCP_SCHED_DEL(c->scheduler.digittimeout);
	c->calltype = SKINNY_CALLTYPE_INBOUND;
	c->state = SCCP_CHANNELSTATE_PROCEED;
	c->answered_elsewhere = TRUE;

	res = pbx_pickup_call(dest);										/* Should be replaced with a PBX(feature_pickup) call, which could give information back why the pickup failed (elsewhere or no match) */
	if (!res) {
		/* gpickup succeeded */
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) pickup succeeded on callid: %d\n", DEV_ID_LOG(d), c->callid);
		sccp_rtp_stop(c);										/* stop previous audio */
		pbx_channel_set_hangupcause(dest, AST_CAUSE_ANSWERED_ELSEWHERE);
		pbx_hangup(dest);										/* hangup masqueraded zombie channel */

		pbx_channel_set_hangupcause(c->owner, AST_CAUSE_NORMAL_CLEARING);
		sccp_channel_setDevice(c, d);
		sccp_channel_set_active(d, c);
		sccp_channel_updateChannelCapability(c);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_CONNECTED);						/* connect calls - reinstate audio */
	} else {
		/* call pickup failed, restore previous situation */
		c->answered_elsewhere = FALSE;

		/* send gpickup failure */
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) pickup failed (someone else picked it up already or not in the correct callgroup)\n", DEV_ID_LOG(d));
		int instance = sccp_device_find_index_for_line(d, l->name);

		sccp_dev_set_message(d, "Pickup Failed", 5, FALSE, TRUE);					/* use messageStack */
		sccp_dev_starttone(d, SKINNY_TONE_BEEPBONK, instance, c->callid, 2);
		sleep(1);											/* give a little time for tone to get delivered, before hanging up */

		/* hangup */
		pbx_channel_set_hangupcause(c->owner, AST_CAUSE_CALL_REJECTED);
		pbx_hangup(c->owner);
	}
	c = sccp_channel_release(c);
	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: (grouppickup) finished (%d)\n", DEV_ID_LOG(d), res);
	return res;
}
#endif														// CS_SCCP_PICKUP

/*!
 * \brief Update Caller ID
 * \param c SCCP Channel
 *
 * \callgraph
 * \callergraph
 */
void sccp_feat_updatecid(sccp_channel_t * c)
{
	char *name = NULL, *number = NULL;

	if (!c || !c->owner)
		return;

	if (!c->calltype == SKINNY_CALLTYPE_OUTBOUND) {
		if (!ast_bridged_channel(c->owner)) {
			return;
		}
	}

	if (PBX(get_callerid_name))
		PBX(get_callerid_name) (c, &name);

	if (PBX(get_callerid_number))
		PBX(get_callerid_number) (c, &number);

	sccp_channel_set_callingparty(c, name, number);

	if (name)
		sccp_free(name);
	if (number)
		sccp_free(number);
}

/*!
 * \brief Handle VoiceMail
 * \param d SCCP Device
 * \param lineInstance LineInstance as uint8_t
 *
 * \lock
 *      - channel
 *        - see sccp_dev_displayprompt()
 */
void sccp_feat_voicemail(sccp_device_t * d, uint8_t lineInstance)
{

	sccp_channel_t *c;
	sccp_line_t *l;

	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: Voicemail Button pressed on line (%d)\n", d->id, lineInstance);

	if ((c = sccp_channel_get_active(d))) {
		if (!c->line || sccp_strlen_zero(c->line->vmnum)) {
			sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No voicemail number configured on line %d\n", d->id, lineInstance);
			c = sccp_channel_release(c);
			return;
		}
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK || c->state == SCCP_CHANNELSTATE_DIALING) {
			sccp_copy_string(c->dialedNumber, c->line->vmnum, sizeof(c->dialedNumber));
			c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
			sccp_pbx_softswitch(c);
			c = sccp_channel_release(c);
			return;
		}

		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
		c = sccp_channel_release(c);
		return;
	}

	if (!lineInstance) {
		if (d->defaultLineInstance)
			lineInstance = d->defaultLineInstance;
		else
			lineInstance = 1;
	}

	l = sccp_line_find_byid(d, lineInstance);

	if (!l) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line with instance %d found.\n", d->id, lineInstance);

		//TODO workaround to solve the voicemail button issue with old hint style and speeddials before first line -MC
		if (d->defaultLineInstance) {
			l = sccp_line_find_byid(d, d->defaultLineInstance);
		}
	}
	if (!l) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No line with defaultLineInstance %d found. Not Dialing Voicemail Extension.\n", d->id, d->defaultLineInstance);
		return;
	}

	if (!sccp_strlen_zero(l->vmnum)) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: Dialing voicemail %s\n", d->id, l->vmnum);
		c = sccp_channel_newcall(l, d, l->vmnum, SKINNY_CALLTYPE_OUTBOUND, NULL);

	} else {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: No voicemail number configured on line %d\n", d->id, lineInstance);
	}
	c = c ? sccp_channel_release(c) : NULL;
	l = sccp_line_release(l);
}

/*!
 * \brief Handle Divert/Transfer Call to VoiceMail
 * \param d SCCP Device
 * \param l SCCP Line
 * \param c SCCP Channel
 */
void sccp_feat_idivert(sccp_device_t * d, sccp_line_t * l, sccp_channel_t * c)
{
	int instance;

	if (!l) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM pressed but no line found\n", d->id);
		sccp_dev_displayprompt(d, 0, 0, "No line found to transfer", 5);
		return;
	}
	if (!l->trnsfvm) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM pressed but not configured in sccp.conf\n", d->id);
		return;
	}
	if (!c || !c->owner) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM with no channel active\n", d->id);
		sccp_dev_displayprompt(d, 0, 0, "TRANSVM with no channel active", 5);
		return;
	}

	if (c->state != SCCP_CHANNELSTATE_RINGING && c->state != SCCP_CHANNELSTATE_CALLWAITING) {
		sccp_log((DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: TRANSVM pressed in no ringing state\n", d->id);
		return;
	}

	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "%s: TRANSVM to %s\n", d->id, l->trnsfvm);
	PBX(setChannelCallForward) (c, l->trnsfvm);
	instance = sccp_device_find_index_for_line(d, l->name);
	sccp_device_sendcallstate(d, instance, c->callid, SKINNY_CALLSTATE_PROCEED, SKINNY_CALLPRIORITY_LOW, SKINNY_CALLINFO_VISIBILITY_DEFAULT);	/* send connected, so it is not listed as missed call */
	pbx_setstate(c->owner, AST_STATE_BUSY);
	PBX(queue_control) (c->owner, AST_CONTROL_BUSY);
}

/*!
 * \brief Handle 3-Way Phone Based Conferencing on a Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 *
 * \lock
 *      - channel
 *        - see sccp_indicate_nolock()
 *      - channel
 *        - see sccp_channel_set_active()
 *        - see sccp_indicate_nolock()
 *        - see sccp_pbx_channel_allocate()
 *        - see sccp_channel_openreceivechannel()
 */
void sccp_feat_handle_conference(sccp_device_t * d, sccp_line_t * l, uint8_t lineInstance, sccp_channel_t * c)
{
#ifdef CS_SCCP_CONFERENCE
	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return;
	}

	if (!d->allow_conference) {
		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
		pbx_log(LOG_NOTICE, "%s: conference not enabled\n", DEV_ID_LOG(d));
		return;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active(d))) {
		if (!sccp_channel_hold(c)) {
                       	sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_TEMP_FAIL, 5);
			c = sccp_channel_release(c);
			return;
		}
		c = sccp_channel_release(c);
	}

	sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: Allocating new channel for conference\n");
	if ((c = sccp_channel_allocate(l, d))) {
		c->ss_action = SCCP_SS_GETCONFERENCEROOM;							/* Simpleswitch will catch a number to be dialed */
		c->ss_data = 0;											/* not needed here */
		c->calltype = SKINNY_CALLTYPE_OUTBOUND;

		sccp_channel_set_active(d, c);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);

		/* ok the number exist. allocate the asterisk channel */
		if (!sccp_pbx_channel_allocate(c, NULL)) {
			pbx_log(LOG_WARNING, "%s: (sccp_feat_handle_conference) Unable to allocate a new channel for line %s\n", d->id, l->name);
			sccp_indicate(d, c, SCCP_CHANNELSTATE_CONGESTION);
			c = sccp_channel_release(c);
			return;
		}

		PBX(set_callstate) (c, AST_STATE_OFFHOOK);
		/* removing scheduled dial */
		c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);

		sccp_pbx_softswitch(c);

		c = sccp_channel_release(c);
	} else {
		pbx_log(LOG_ERROR, "%s: (sccp_feat_handle_conference) Can't allocate SCCP channel for line %s\n", DEV_ID_LOG(d), l->name);
		return;
	}
#endif
}

/*!
 * \brief Handle Conference
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \return Success as int
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 *
 * \lock
 *      - device->selectedChannels
 *        - see sccp_conference_addParticipant()
 *      - device->buttonconfig
 *        - see sccp_line_find_byname()
 *        - line->channels
 *          - see sccp_conference_addParticipant()
 */
void sccp_feat_conference_start(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c)
{
#ifdef CS_SCCP_CONFERENCE
	sccp_channel_t *channel = NULL;
	sccp_selectedchannel_t *selectedChannel = NULL;
	boolean_t selectedFound = FALSE;

	if (!(d = sccp_device_retain(d)) || !c) {
		return;
	}

	uint8_t num = sccp_device_numberOfChannels(d);
	int instance = sccp_device_find_index_for_line(d, l->name);

	sccp_log((DEBUGCAT_CONFERENCE | DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: sccp_device_numberOfChannels %d.\n", DEV_ID_LOG(d), num);

	if (d->conference /* && num > 3 */ ) {
		/* if we have selected channels, add this to conference */

		SCCP_LIST_LOCK(&d->selectedChannels);
		SCCP_LIST_TRAVERSE(&d->selectedChannels, selectedChannel, list) {
			selectedFound = TRUE;

			if (NULL != selectedChannel->channel && selectedChannel->channel != c) {
				if (channel != d->active_channel) {
					sccp_conference_addParticipatingChannel(d->conference, c, CS_AST_BRIDGED_CHANNEL(c->owner));
				}
			}
		}
		SCCP_LIST_UNLOCK(&d->selectedChannels);

		/* If no calls were selected, add all calls to the conference, across all lines. */
		if (FALSE == selectedFound) {
			// all channels on this phone
			sccp_line_t *line = NULL;
			uint8_t i = 0;

			for (i = 0; i < StationMaxButtonTemplateSize; i++) {
				if (d->buttonTemplate[i].type == SKINNY_BUTTONTYPE_LINE && d->buttonTemplate[i].ptr) {
					if ((line = sccp_line_retain(d->buttonTemplate[i].ptr))) {
						SCCP_LIST_LOCK(&line->channels);
						SCCP_LIST_TRAVERSE(&line->channels, channel, list) {
							if (channel != d->active_channel) {
								sccp_log((DEBUGCAT_CONFERENCE | DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: sccp conference: channel %s, state: %s.\n", DEV_ID_LOG(d), pbx_channel_name(CS_AST_BRIDGED_CHANNEL(channel->owner)), channelstate2str(channel->state));
								sccp_conference_addParticipatingChannel(d->conference, c, CS_AST_BRIDGED_CHANNEL(channel->owner));
							}
						}
						SCCP_LIST_UNLOCK(&line->channels);
						line = sccp_line_release(line);
					}
				}

			}
		}
		sccp_conference_start(d->conference);
	} else {
		sccp_dev_displayprompt(d, instance, c->callid, "Error creating conf", 5);
		pbx_log(LOG_NOTICE, "%s: conference could not be created\n", DEV_ID_LOG(d));
	}
	d = d ? sccp_device_release(d) : NULL;
#else
	sccp_log((DEBUGCAT_CONFERENCE | DEBUGCAT_FEATURE)) (VERBOSE_PREFIX_3 "%s: conference not enabled\n", DEV_ID_LOG(d));
	sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
#endif
}

/*!
 * \brief Handle Join a Conference
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 */
void sccp_feat_join(sccp_device_t * d, sccp_line_t * l, uint8_t lineInstance, sccp_channel_t * c)
{
#if CS_SCCP_CONFERENCE
	sccp_channel_t *channel = NULL;

	if (!c)
		return;

	if ((d = sccp_device_retain(d))) {
		if (!d->allow_conference) {
			sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
			pbx_log(LOG_NOTICE, "%s: conference not enabled\n", DEV_ID_LOG(d));
			sccp_dev_displayprompt(d, lineInstance, c->callid, "conference not allowed", 5);
		} else if (!d->conference) {
			pbx_log(LOG_NOTICE, "%s: There is currently no active conference on this device. Start Conference First.\n", DEV_ID_LOG(d));
			sccp_dev_displayprompt(d, lineInstance, c->callid, "No Conference to Join", 5);
		} else if (!d->active_channel) {
			pbx_log(LOG_NOTICE, "%s: No active channel on device to join to the conference.\n", DEV_ID_LOG(d));
			sccp_dev_displayprompt(d, lineInstance, c->callid, "No Active Channel", 5);
		} else if (d->active_channel->conference) {
			pbx_log(LOG_NOTICE, "%s: Channel is already part of a conference.\n", DEV_ID_LOG(d));
			sccp_dev_displayprompt(d, lineInstance, c->callid, "Already in Conference", 5);
		} else {
			channel = d->active_channel;
			pbx_log(LOG_NOTICE, "%s: Joining new participant to conference %d.\n", DEV_ID_LOG(d), d->conference->id);
			sccp_channel_hold(channel);
			sccp_conference_addParticipatingChannel(d->conference, channel, CS_AST_BRIDGED_CHANNEL(channel->owner));

			sccp_channel_t *mod_chan = NULL;

			SCCP_LIST_LOCK(&l->channels);
			SCCP_LIST_TRAVERSE(&l->channels, mod_chan, list) {
				if (d->conference == mod_chan->conference) {
					sccp_channel_resume(d, mod_chan, FALSE);
                                	sccp_feat_conflist(d, mod_chan->line, 0, mod_chan);
					break;
				}
			}
			SCCP_LIST_UNLOCK(&l->channels);
			sccp_conference_update(d->conference);
		}
		d = sccp_device_release(d);
	}
#else
	pbx_log(LOG_NOTICE, "%s: conference not enabled\n", DEV_ID_LOG(d));
	sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
#endif
}

/*!
 * \brief Handle Conference List
 * \param d SCCP Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param c SCCP Channel
 * \return Success as int
 */
void sccp_feat_conflist(sccp_device_t * d, sccp_line_t * l, uint8_t lineInstance, sccp_channel_t * c)
{
	if (d) {
#ifdef CS_SCCP_CONFERENCE
		if (!d->allow_conference) {
			sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
			pbx_log(LOG_NOTICE, "%s: conference not enabled\n", DEV_ID_LOG(d));
			return;
		}
		sccp_conference_show_list(c->conference, c);
#else
		sccp_dev_displayprompt(d, lineInstance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
#endif
	}
}

/*!
 * \brief Handle 3-Way Phone Based Conferencing on a Device
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 *
 * \lock
 *      - channel
 *        - see sccp_indicate_nolock()
 *      - channel
 *        - see sccp_channel_set_active()
 *        - see sccp_indicate_nolock()
 *        - see sccp_pbx_channel_allocate()
 *        - see sccp_channel_openreceivechannel()
 */
void sccp_feat_handle_meetme(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETMEETMEROOM;							/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;										/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			c = sccp_channel_release(c);
			return;
			/* there is an active call, let's put it on hold first */
		} else if (!sccp_channel_hold(c)) {
			c = sccp_channel_release(c);
			return;
		}
		c = sccp_channel_release(c);
	}

	c = sccp_channel_allocate(l, d);

	if (!c) {
		pbx_log(LOG_ERROR, "%s: (handle_meetme) Can't allocate SCCP channel for line %s\n", DEV_ID_LOG(d), l->name);
		return;
	}

	c->ss_action = SCCP_SS_GETMEETMEROOM;									/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;												/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate(c, NULL)) {
		pbx_log(LOG_WARNING, "%s: (handle_meetme) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_CONGESTION);
		c = sccp_channel_release(c);
		return;
	}

	PBX(set_callstate) (c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel(c);
	}

	/* removing scheduled dial */
	c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);

	if (!(c->scheduler.digittimeout = sccp_sched_add(GLOB(firstdigittimeout) * 1000, sccp_pbx_sched_dial, c))) {
		sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: Unable to schedule dialing in '%d' ms\n", GLOB(firstdigittimeout));
	}

	c = sccp_channel_release(c);
}

/*!
 * \brief Meetme Application Config Structure
 */
struct meetmeAppConfig {
	char *appName;
	char *defaultMeetmeOption;
} meetmeApps[] = {
	{
	"MeetMe", "qd"}, {
	"ConfBridge", "Mac"}, {
	"Konference", "MTV"}
};

/*!
 * \brief a Meetme Application Thread
 * \param data Data
 * \author Federico Santulli
 *
 * \lock
 *      - channel
 *        - see sccp_indicate_nolock()
 *        - see sccp_channel_set_calledparty()
 *        - see sccp_channel_setSkinnyCallstate()
 *        - see sccp_channel_send_callinfo()
 *        - see sccp_indicate_nolock()
 */
static void *sccp_feat_meetme_thread(void *data)
{
	sccp_channel_t *c = data;
	sccp_device_t *d = NULL;

	struct meetmeAppConfig *app = NULL;

	char ext[SCCP_MAX_EXTENSION];
	char context[SCCP_MAX_CONTEXT];

	char meetmeopts[SCCP_MAX_CONTEXT];

#if ASTERISK_VERSION_NUMBER >= 10600
#define SCCP_CONF_SPACER ','
#endif

#if ASTERISK_VERSION_NUMBER >= 10400 && ASTERISK_VERSION_NUMBER < 10600
#define SCCP_CONF_SPACER '|'
#endif

#if ASTERISK_VERSION_NUMBER >= 10400
	unsigned int eid = pbx_random();
#else
	unsigned int eid = random();

#define SCCP_CONF_SPACER '|'
#endif
	if (!c || !(d = sccp_channel_getDevice_retained(c))) {
		pbx_log(LOG_NOTICE, "SCCP: no channel or device provided for meetme feature. exiting\n");
		return NULL;
	}

	/* searching for meetme app */
	uint32_t i;

	for (i = 0; i < sizeof(meetmeApps) / sizeof(struct meetmeAppConfig); i++) {
		if (pbx_findapp(meetmeApps[i].appName)) {
			app = &(meetmeApps[i]);
			sccp_log((DEBUGCAT_CORE)) (VERBOSE_PREFIX_3 "SCCP: using '%s' for meetme\n", meetmeApps[i].appName);
			break;
		}
	}
	/* finish searching for meetme app */

	if (!app) {												// \todo: remove res in this line: Although the value stored to 'res' is used in the enclosing expression, the value is never actually read from 'res'
		pbx_log(LOG_WARNING, "SCCP: No MeetMe application available!\n");
		c = sccp_channel_retain(c);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_DIALING);
		sccp_channel_set_calledparty(c, SKINNY_DISP_CONFERENCE, c->dialedNumber);
		sccp_channel_setSkinnyCallstate(c, SKINNY_CALLSTATE_PROCEED);
		sccp_channel_send_callinfo(d, c);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_INVALIDCONFERENCE);
		c = sccp_channel_release(c);
		d = sccp_device_release(d);
		return NULL;
	}
	// SKINNY_DISP_CAN_NOT_COMPLETE_CONFERENCE
	if (c && c->owner) {
		if (!pbx_channel_context(c->owner) || sccp_strlen_zero(pbx_channel_context(c->owner))) {
			d = sccp_device_release(d);
			return NULL;
		}
		/* replaced by meetmeopts in global, device, line */
		//              snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, (c->line->meetmeopts&& !sccp_strlen_zero(c->line->meetmeopts)) ? c->line->meetmeopts : "qd");
		if (!sccp_strlen_zero(c->line->meetmeopts)) {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, c->line->meetmeopts);
		} else if (!sccp_strlen_zero(d->meetmeopts)) {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, d->meetmeopts);
		} else if (!sccp_strlen_zero(GLOB(meetmeopts))) {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, GLOB(meetmeopts));
		} else {
			snprintf(meetmeopts, sizeof(meetmeopts), "%s%c%s", c->dialedNumber, SCCP_CONF_SPACER, app->defaultMeetmeOption);
		}

		sccp_copy_string(context, pbx_channel_context(c->owner), sizeof(context));

		snprintf(ext, sizeof(ext), "sccp_meetme_temp_conference_%ud", eid);

		if (!pbx_exists_extension(NULL, context, ext, 1, NULL)) {
			pbx_add_extension(context, 1, ext, 1, NULL, NULL, app->appName, meetmeopts, NULL, "sccp_feat_meetme_thread");
			pbx_log(LOG_WARNING, "SCCP: create extension exten => %s,%d,%s(%s)\n", ext, 1, app->appName, meetmeopts);
		}
		//              sccp_copy_string(c->owner->exten, ext, sizeof(c->owner->exten));
		PBX(setChannelExten) (c, ext);

		c = sccp_channel_retain(c);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_DIALING);
		sccp_channel_set_calledparty(c, SKINNY_DISP_CONFERENCE, c->dialedNumber);
		sccp_channel_setSkinnyCallstate(c, SKINNY_CALLSTATE_PROCEED);
		sccp_channel_send_callinfo(d, c);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_CONNECTEDCONFERENCE);

		if (pbx_pbx_run(c->owner)) {
			sccp_indicate(d, c, SCCP_CHANNELSTATE_INVALIDCONFERENCE);
			pbx_log(LOG_WARNING, "SCCP: SCCP_CHANNELSTATE_INVALIDCONFERENCE\n");
		}
		c = sccp_channel_release(c);
		ast_context_remove_extension(context, ext, 1, NULL);
	}

	d = sccp_device_release(d);
	return NULL;
}

/*!
 * \brief Start a Meetme Application Thread
 * \param c SCCP Channel
 * \author Federico Santulli
 */
void sccp_feat_meetme_start(sccp_channel_t * c)
{
#if CS_EXPERIMENTAL
	sccp_threadpool_add_work(GLOB(general_threadpool), (void *) sccp_feat_meetme_thread, (void *) c);
#else
	pthread_attr_t attr;
	pthread_t t;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pbx_pthread_create(&t, &attr, sccp_feat_meetme_thread, c) < 0) {
		pbx_log(LOG_WARNING, "SCCP: Cannot create a MeetMe thread (%s).\n", strerror(errno));
	}
	pthread_attr_destroy(&attr);
#endif
}

/*!
 * \brief Handle Barging into a Call
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 *
 * \lock
 *      - channel
 *        - see sccp_indicate_nolock()
 */
void sccp_feat_handle_barge(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || !d->id || sccp_strlen_zero(d->id)) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETBARGEEXTEN;							/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;										/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			c = sccp_channel_release(c);
			return;
		} else if (!sccp_channel_hold(c)) {
			/* there is an active call, let's put it on hold first */
			c = sccp_channel_release(c);
			return;
		}
		c = sccp_channel_release(c);
	}

	c = sccp_channel_allocate(l, d);

	if (!c) {
		pbx_log(LOG_ERROR, "%s: (handle_barge) Can't allocate SCCP channel for line %s\n", d->id, l->name);
		return;
	}

	c->ss_action = SCCP_SS_GETBARGEEXTEN;									/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;												/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate(c, NULL)) {
		pbx_log(LOG_WARNING, "%s: (handle_barge) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_CONGESTION);
		c = sccp_channel_release(c);
		return;
	}

	PBX(set_callstate) (c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel(c);
	}
	c = sccp_channel_release(c);
}

/*!
 * \brief Barging into a Call Feature
 * \param c SCCP Channel
 * \param exten Extention as char
 * \return Success as int
 */
int sccp_feat_barge(sccp_channel_t * c, char *exten)
{
	/* sorry but this is private code -FS */
	sccp_device_t *d = NULL;

	if (!c || !(d = sccp_channel_getDevice_retained(c)))
		return -1;

	uint8_t instance = sccp_device_find_index_for_line(d, c->line->name);

	sccp_dev_displayprompt(d, instance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
	d = sccp_device_release(d);
	return 1;
}

/*!
 * \brief Handle Barging into a Conference
 * \param l SCCP Line
 * \param lineInstance lineInstance as uint8_t
 * \param d SCCP Device
 * \return SCCP Channel
 * \todo Conferencing option needs to be build and implemented
 *       Using and External Conference Application Instead of Meetme makes it possible to use app_Conference, app_MeetMe, app_Konference and/or others
 *
 * \lock
 *      - channel
 *        - see sccp_indicate_nolock()
 *      - channel
 *        - see sccp_channel_set_active()
 *        - see sccp_indicate_nolock()
 */
void sccp_feat_handle_cbarge(sccp_line_t * l, uint8_t lineInstance, sccp_device_t * d)
{
	sccp_channel_t *c;

	if (!l || !d || strlen(d->id) < 3) {
		pbx_log(LOG_ERROR, "SCCP: Can't allocate SCCP channel if line or device are not defined!\n");
		return;
	}

	/* look if we have a call */
	if ((c = sccp_channel_get_active(d))) {
		// we have a channel, checking if
		if (c->state == SCCP_CHANNELSTATE_OFFHOOK && (!c->dialedNumber || (c->dialedNumber && sccp_strlen_zero(c->dialedNumber)))) {
			// we are dialing but without entering a number :D -FS
			sccp_dev_stoptone(d, lineInstance, (c && c->callid) ? c->callid : 0);
			// changing SS_DIALING mode to SS_GETFORWARDEXTEN
			c->ss_action = SCCP_SS_GETCBARGEROOM;							/* Simpleswitch will catch a number to be dialed */
			c->ss_data = 0;										/* this should be found in thread */
			// changing channelstate to GETDIGITS
			sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);
			c = sccp_channel_release(c);
			return;
		} else {
			/* there is an active call, let's put it on hold first */
			if (!sccp_channel_hold(c)) {
				c = sccp_channel_release(c);
				return;
			}
		}
		c = sccp_channel_release(c);
	}

	c = sccp_channel_allocate(l, d);

	if (!c) {
		pbx_log(LOG_ERROR, "%s: (handle_cbarge) Can't allocate SCCP channel for line %s\n", d->id, l->name);
		return;
	}

	c->ss_action = SCCP_SS_GETCBARGEROOM;									/* Simpleswitch will catch a number to be dialed */
	c->ss_data = 0;												/* not needed here */

	c->calltype = SKINNY_CALLTYPE_OUTBOUND;

	sccp_channel_set_active(d, c);
	sccp_indicate(d, c, SCCP_CHANNELSTATE_GETDIGITS);

	/* ok the number exist. allocate the asterisk channel */
	if (!sccp_pbx_channel_allocate(c, NULL)) {
		pbx_log(LOG_WARNING, "%s: (handle_cbarge) Unable to allocate a new channel for line %s\n", d->id, l->name);
		sccp_indicate(d, c, SCCP_CHANNELSTATE_CONGESTION);
		c = sccp_channel_release(c);
		return;
	}

	PBX(set_callstate) (c, AST_STATE_OFFHOOK);

	if (d->earlyrtp == SCCP_CHANNELSTATE_OFFHOOK && !c->rtp.audio.rtp) {
		sccp_channel_openreceivechannel(c);
	}

	c = sccp_channel_release(c);
}

/*!
 * \brief Barging into a Conference Feature
 * \param c SCCP Channel
 * \param conferencenum Conference Number as char
 * \return Success as int
 */
int sccp_feat_cbarge(sccp_channel_t * c, char *conferencenum)
{
	/* sorry but this is private code -FS */
	sccp_device_t *d = NULL;

	if (!c || !(d = sccp_channel_getDevice_retained(c)))
		return -1;

	uint8_t instance = sccp_device_find_index_for_line(d, c->line->name);

	sccp_dev_displayprompt(d, instance, c->callid, SKINNY_DISP_KEY_IS_NOT_ACTIVE, 5);
	d = sccp_device_release(d);
	return 1;
}

/*!
 * \brief Hotline Feature
 *
 * Setting the hotline Feature on a device, will make it connect to a predefined extension as soon as the Receiver
 * is picked up or the "New Call" Button is pressed. No number has to be given.
 *
 * \param d SCCP Device
 * \param line SCCP Line
 *
 * \lock
 *      - channel
 */
void sccp_feat_adhocDial(sccp_device_t * d, sccp_line_t * line)
{
	sccp_channel_t *c = NULL;

	if (!d || !d->session || !line)
		return;

	sccp_log((DEBUGCAT_FEATURE | DEBUGCAT_LINE)) (VERBOSE_PREFIX_3 "%s: handling hotline\n", d->id);
	if ((c = sccp_channel_get_active(d))) {
		if ((c->state == SCCP_CHANNELSTATE_DIALING) || (c->state == SCCP_CHANNELSTATE_OFFHOOK)) {
			sccp_copy_string(c->dialedNumber, line->adhocNumber, sizeof(c->dialedNumber));

			c->scheduler.digittimeout = SCCP_SCHED_DEL(c->scheduler.digittimeout);
			sccp_pbx_softswitch(c);
			c = sccp_channel_release(c);
			return;
		}
		sccp_pbx_senddigits(c, line->adhocNumber);
	} else {
		// Pull up a channel
		if (GLOB(hotline)->line) {
			c = sccp_channel_newcall(line, d, line->adhocNumber, SKINNY_CALLTYPE_OUTBOUND, NULL);
		}
	}

	c = c ? sccp_channel_release(c) : NULL;
}

/*!
 * \brief Handler to Notify Features have Changed
 * \param device SCCP Device
 * \param featureType SCCP Feature Type
 * 
 * \lock
 *      - see sccp_hint_handleFeatureChangeEvent() via sccp_event_fire()
 *      - see sccp_util_handleFeatureChangeEvent() via sccp_event_fire()
 */
void sccp_feat_changed(sccp_device_t * device, sccp_linedevices_t * linedevice, sccp_feature_type_t featureType)
{
	if (device) {
		sccp_featButton_changed(device, featureType);

		sccp_event_t event;

		memset(&event, 0, sizeof(sccp_event_t));

		event.type = SCCP_EVENT_FEATURE_CHANGED;
		event.event.featureChanged.device = sccp_device_retain(device);
		event.event.featureChanged.linedevice = linedevice ? sccp_linedevice_retain(linedevice) : NULL;
		event.event.featureChanged.featureType = featureType;
		sccp_event_fire(&event);
	}
}

/*!
 * \brief Handler to Notify Channel State has Changed
 * \param device SCCP Device
 * \param channel SCCP Channel
 */
void sccp_feat_channelStateChanged(sccp_device_t * device, sccp_channel_t * channel)
{
	uint8_t state;

	if (!channel || !device)
		return;

	state = channel->state;
	switch (state) {
		case SCCP_CHANNELSTATE_CONNECTED:
			/* We must update the status here. Not change it. (DD) */
			/*
			   if (device->monitorFeature.enabled && device->monitorFeature.status != channel->monitorEnabled) {
			   sccp_feat_monitor(device, channel);
			   }
			 */
			break;
		case SCCP_CHANNELSTATE_DOWN:
		case SCCP_CHANNELSTATE_ONHOOK:
		case SCCP_CHANNELSTATE_BUSY:
		case SCCP_CHANNELSTATE_CONGESTION:
		case SCCP_CHANNELSTATE_INVALIDNUMBER:
		case SCCP_CHANNELSTATE_ZOMBIE:
			/* \todo: In the event a call is terminated,
			   the channel monitor should be turned off (it implicitly is by ending the call),
			   and the feature button should be reset to disabled state. */
			device->monitorFeature.status = 0;
			sccp_feat_changed(device, NULL, SCCP_FEATURE_MONITOR);
			break;

		default:
			break;
	}

}

/*!
 * \brief Feature Monitor
 * \param device SCCP Device
 * \param line SCCP Line
 * \param lineInstance LineInstance as uint32_t
 * \param channel SCCP Channel
 */
void sccp_feat_monitor(sccp_device_t * device, sccp_line_t * line, const uint32_t lineInstance, sccp_channel_t * channel)
{
	char *featureExtension = NULL;
	boolean_t result = FALSE;

#ifdef CS_SCCP_FEATURE_MONITOR

	if (PBX(getFeatureExtension))
		result = PBX(getFeatureExtension) (channel, &featureExtension);

	if (result && featureExtension && channel) {
		pbx_log(LOG_WARNING, "send digits %s\n", featureExtension);
		result = TRUE;
	} else {
		result = FALSE;
	}

	if (featureExtension) {
		sccp_free(featureExtension);
		featureExtension = NULL;
	}
#endif
}
