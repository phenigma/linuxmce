
/*!
 * \file 	sccp_softkeys.h
 * \brief 	SCCP SoftKeys Header
 * \author 	Sergio Chersovani <mlists [at] c-net.it>
 * \note	Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 *
 * $Date: 2012-01-02 13:56:08 +0000 (Mon, 02 Jan 2012) $
 * $Revision: 3147 $  
 */
#    ifndef __SCCP_SOFTKEYS_H
#define __SCCP_SOFTKEYS_H

#ifdef CS_DYNAMIC_CONFIG
void sccp_softkey_pre_reload(void);
void sccp_softkey_post_reload(void);
#endif

/*!
 * \brief SCCP SoftKeyMap Callback
 *
 * Used to Map Softkeys to there Handling Implementation
 */
struct sccp_softkeyMap_cb {
	uint32_t event;
	void (*const softkeyEvent_cn) (sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
	boolean_t channelIsNecessary;
};
typedef struct sccp_softkeyMap_cb sccp_softkeyMap_cb_t;

const sccp_softkeyMap_cb_t *sccp_getSoftkeyMap_by_SoftkeyEvent(uint32_t event);
void sccp_softkey_setSoftkeyState(sccp_device_t * device, uint8_t softKeySet, uint8_t softKey, boolean_t enable);

void sccp_sk_dial(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_redial(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_newcall(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_hold(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_transfer(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_conference(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_cfwdall(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_cfwdbusy(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_cfwdnoanswer(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);

void sccp_sk_dnd(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_backspace(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_endcall(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);

void sccp_sk_answer(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);

void sccp_sk_conference(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_conflist(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_join(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_meetme(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_barge(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_cbarge(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);

void sccp_sk_dirtrfr(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_select(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);

void sccp_sk_resume(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_park(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_trnsfvm(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_private(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_pickup(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_gpickup(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c);
void sccp_sk_set_keystate(sccp_device_t * d, sccp_line_t * l, const uint32_t lineInstance, sccp_channel_t * c, unsigned int keymode, unsigned int softkeyindex, unsigned int status);

#    endif
