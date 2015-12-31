
/*!
 * \file 	sccp_mwi.h
 * \brief 	SCCP Message Waiting Indicator Header
 * \author 	Marcello Ceschia <marcello [at] ceschia.de>
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 * 
 * $Date: 2012-01-02 13:56:08 +0000 (Mon, 02 Jan 2012) $
 * $Revision: 3147 $  
 */

#    ifndef SCCP_MWI_H_
#define SCCP_MWI_H_

#ifdef CS_AST_HAS_EVENT
#    include "asterisk/event.h"
#endif

/*!
 * \brief SCCP Mailbox Line Type Definition
 *
 * holding line information for mailbox subscription
 *
 */
typedef struct sccp_mailboxLine sccp_mailboxLine_t;

/*!
 * \brief SCCP Mailbox Line Type Structure 
 *
 * holding line information for mailbox subscription
 *
 */
struct sccp_mailboxLine {
	sccp_line_t *line;
	 SCCP_LIST_ENTRY(sccp_mailboxLine_t) list;
};

/*!
 * \brief SCCP Mailbox Subscriber List Type Definition
 */
typedef struct sccp_mailbox_subscriber_list sccp_mailbox_subscriber_list_t;

/*!
 * \brief SCCP Mailbox Subscriber List Structure
 * 
 * we hold a mailbox event subscription in sccp_mailbox_subscription_t.
 * Each line that holds a subscription for this mailbox is listed in
 */
struct sccp_mailbox_subscriber_list {
	char mailbox[60];
	char context[60];

	 SCCP_LIST_HEAD(, sccp_mailboxLine_t) sccp_mailboxLine;
	 SCCP_LIST_ENTRY(sccp_mailbox_subscriber_list_t) list;

	/*!
	 * \brief Current Voicemail Statistic Structure 
	 */
	struct {
		int newmsgs;							/*!< New Messages */
		int oldmsgs;							/*!< Old Messages */
	} currentVoicemailStatistic;						/*!< Current Voicemail Statistic Structure */

	/*!
	 * \brief Previous Voicemail Statistic Structure 
	 */
	struct {
		int newmsgs;							/*!< New Messages */
		int oldmsgs;							/*!< Old Messages */
	} previousVoicemailStatistic;						/*!< Previous Voicemail Statistic Structure */

#ifdef CS_AST_HAS_EVENT
	/*!
	 * \brief Asterisk Event Subscribers Structure
	 */
	struct pbx_event_sub *event_sub;
#else
	int schedUpdate;
#endif
};										/*!< SCCP Mailbox Subscriber List Structure */

void sccp_mwi_module_start(void);
void sccp_mwi_module_stop(void);
void sccp_mwi_check(sccp_device_t * device);

void sccp_mwi_unsubscribeMailbox(sccp_mailbox_t ** mailbox);

#ifdef CS_AST_HAS_EVENT
void sccp_mwi_event(const struct ast_event *event, void *data);
#else
int sccp_mwi_checksubscription(const void *ptr);
#endif

#    endif										/*SCCP_MWI_H_ */
