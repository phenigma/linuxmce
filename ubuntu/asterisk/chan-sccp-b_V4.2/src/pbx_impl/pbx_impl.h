/*!
 * \file        pbx_impl.h
 * \brief       SCCP PBX Asterisk Wrapper Header
 * \author      Diederik de Groot <ddegroot [at] users.sourceforge.net>
 * \note        Reworked, but based on chan_sccp code.
 *              The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *              Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 *
 * $Date: 2015-10-12 09:51:15 +0000 (Mon, 12 Oct 2015) $
 * $Revision: 6218 $
 */
#pragma once

#include <config.h>
//#if HAVE_SYS_SOCKET_H
//#include <sys/socket.h>
//#endif
#include "../common.h"

#ifdef HAVE_ASTERISK
#include "ast/ast.h"
#endif

#define PBX_BRIDGE_TYPE struct ast_bridge

/*!
 * \brief SCCP PBX Callback function
 *
 * Register Callback_Functions for a particular PBX Implementation and PBX Version. This will make it possible to make
 * chan-sccp agnostic to the PBX Software we are connected to. And provides a way to cope with multiple different versions
 * of these PBX Implementation. There Callback functions are implemented under the pbx_impl directory
 */
typedef struct _PbxInterface {
	/* *INDENT-OFF* */
	/* channels */
	boolean_t(*const alloc_pbxChannel) (sccp_channel_t * channel, const void * ids, const PBX_CHANNEL_TYPE * pbxSrcChannel, PBX_CHANNEL_TYPE ** pbxDstChannel);	/* returns a new channel with pbx_channel_ref and ast_module_info_ref*/
	int (*const set_callstate) (constChannelPtr pbx_channel, int state);
	boolean_t(*const checkhangup) (constChannelPtr channel);
	int (*const hangup) (PBX_CHANNEL_TYPE * channel);
	sccp_extension_status_t(*const extension_status) (constChannelPtr channel);

	void (*const setPBXChannelLinkedId) (PBX_CHANNEL_TYPE *pbxchannel, const char *linkedid);

	const char *(*const getChannelName) (const sccp_channel_t *channel);
	void (*const setChannelName) (const sccp_channel_t *channel, const char *name);
	const char *(*const getChannelUniqueID) (const sccp_channel_t *channel);
	const char *(*const getChannelAppl) (const sccp_channel_t *channel);
	const char *(*const getChannelExten) (const sccp_channel_t *channel);
	void (*const setChannelExten) (const sccp_channel_t *channel, const char *exten);
	const char *(*const getChannelLinkedId) (constChannelPtr channel);
	void (*const setChannelLinkedId) (const sccp_channel_t *channel, const char *linkedid);
	enum ast_channel_state (*const getChannelState) (const sccp_channel_t *channel);
	const struct ast_pbx *(*const getChannelPbx) (const sccp_channel_t *channel);
	const char *(*const getChannelContext) (constChannelPtr channel);
	void (*const setChannelContext) (const sccp_channel_t *channel, const char *linkedid);
	const char *(*const getChannelMacroExten) (constChannelPtr channel);
	void (*const setChannelMacroExten) (const sccp_channel_t *channel, const char *linkedid);
	const char *(*const getChannelMacroContext) (constChannelPtr channel);
	void (*const setChannelMacroContext) (const sccp_channel_t *channel, const char *linkedid);
	const char *(*const getChannelCallForward) (constChannelPtr channel);
	void (*const setChannelCallForward) (const sccp_channel_t *channel, const char *fwdnum);

	/** get channel by name */
	boolean_t(*const getChannelByName) (const char *name, PBX_CHANNEL_TYPE **pbx_channel);
	boolean_t(*const getRemoteChannel) (const sccp_channel_t *channel, PBX_CHANNEL_TYPE **pbx_channel);
	void *(*const getChannelByCallback) (int (*is_match)(PBX_CHANNEL_TYPE *, void *),void *data);


	int (*const set_nativeAudioFormats) (constChannelPtr channel, skinny_codec_t codec[], int length);
	int (*const set_nativeVideoFormats) (constChannelPtr channel, uint32_t);
	int (*const getPeerCodecCapabilities) (constChannelPtr channel, void **capabilities);
	int (*const send_digit) (constChannelPtr channel, const char digit);
	int (*const send_digits) (constChannelPtr channel, const char *digits);

	int (*const sched_add) (int when, sccp_sched_cb callback, const void *data);
	int (*const sched_del) (int id);
	int (*const sched_add_ref) (int *id, int when, sccp_sched_cb callback, sccp_channel_t *channel);
	int (*const sched_del_ref) (int *id, const sccp_channel_t *channel);
	int (*const sched_replace_ref) (int *id, int when, ast_sched_cb callback, sccp_channel_t *channel);
	long (*const sched_when) (int id);
	int (*const sched_wait) (int id);

	/* rtp */
//	int (*const rtp_getPeer) (constChannelPtr channel, struct sockaddr_in * address);
	boolean_t(*const rtp_getPeer) (PBX_RTP_TYPE * rtp, struct sockaddr_storage *address);
	boolean_t(*const rtp_getUs) (PBX_RTP_TYPE * rtp, struct sockaddr_storage *address);
	int (*const rtp_setPhoneAddress) (const struct sccp_rtp * rtp, const struct sockaddr_storage * new_peer, int nat_active);
	boolean_t(*const rtp_setWriteFormat) (constChannelPtr channel, skinny_codec_t codec);
	boolean_t(*const rtp_setReadFormat) (constChannelPtr channel, skinny_codec_t codec);
	boolean_t(*const rtp_destroy) (PBX_RTP_TYPE * rtp);
	void (*const rtp_stop) (PBX_RTP_TYPE *rtp);
	int (*const rtp_codec) (sccp_channel_t * channel);
	boolean_t(*const rtp_audio_create) (sccp_channel_t * channel);
	boolean_t(*const rtp_video_create) (sccp_channel_t * channel);
	uint8_t(*const rtp_get_payloadType) (const struct sccp_rtp * rtp, skinny_codec_t codec);
	int(*const rtp_get_sampleRate) (skinny_codec_t codec);
	uint8_t(*const rtp_bridgePeers) (PBX_CHANNEL_TYPE * c0, PBX_CHANNEL_TYPE * c1, int flags, struct ast_frame ** fo, PBX_CHANNEL_TYPE ** rc, int timeoutms);

	/* callerid */
	int (*const get_callerid_name) (constChannelPtr channel, char **cid_name);
	int (*const get_callerid_number) (constChannelPtr channel, char **cid_number);
	int (*const get_callerid_ton) (constChannelPtr channel, char **ton);
	int (*const get_callerid_ani) (constChannelPtr channel, char **ani);
	int (*const get_callerid_subaddr) (constChannelPtr channel, char **subaddr);
	int (*const get_callerid_dnid) (constChannelPtr channel, char **dnid);
	int (*const get_callerid_rdnis) (constChannelPtr channel, char **rdnis);
	int (*const get_callerid_presentation) (constChannelPtr channel);

	void (*const set_callerid_name) (constChannelPtr channel, const char *name);
	void (*const set_dialed_number) (constChannelPtr channel, const char *number);
	void (*const set_callerid_number) (constChannelPtr channel, const char *number);
	void (*const set_callerid_ani) (constChannelPtr channel, const char *ani);
	void (*const set_callerid_dnid) (constChannelPtr channel, const char *dnid);
	void (*const set_callerid_redirectingParty) (constChannelPtr channel, const char *number, const char *name);
	void (*const set_callerid_redirectedParty) (constChannelPtr channel, const char *number, const char *name);
	void (*const set_callerid_presentation) (constChannelPtr channel, sccp_callerid_presentation_t presentation);
	void (*const set_connected_line) (constChannelPtr channel, const char *number, const char *name, uint8_t reason);
	void (*const sendRedirectedUpdate) (constChannelPtr channel, const char *fromNumber, const char *fromName, const char *toNumber, const char *toName, uint8_t reason);

	/* feature section */
	sccp_parkresult_t(*const feature_park) (constChannelPtr hostChannel);
	boolean_t(*const feature_stopMusicOnHold) (constChannelPtr channel);
	boolean_t(*const feature_addToDatabase) (const char *family, const char *key, const char *value);
	boolean_t(*const feature_getFromDatabase) (const char *family, const char *key, char *out, int outlen);
	boolean_t(*const feature_removeFromDatabase) (const char *family, const char *key);
	boolean_t(*const feature_removeTreeFromDatabase) (const char *family, const char *key);
	boolean_t(*const feature_monitor) (const sccp_channel_t *channel);
	boolean_t(*const getFeatureExtension) (constChannelPtr channel, char featureExtension[SCCP_MAX_EXTENSION]);
	boolean_t(*const getPickupExtension) (constChannelPtr channel, char pickupExtension[SCCP_MAX_EXTENSION]);
	boolean_t(*const feature_pickup) (const sccp_channel_t *chan, PBX_CHANNEL_TYPE *target);

	void *(*const eventSubscribe)(constChannelPtr channel, char **featureExtension);
	PBX_CHANNEL_TYPE *(*const findChannelByCallback)(int(*const found_cb)(PBX_CHANNEL_TYPE *c, void *data), void *data, boolean_t lock);

	int(*const moh_start) (PBX_CHANNEL_TYPE * pbx_channel, const char *mclass, const char* interpclass);
	void(*const moh_stop) (PBX_CHANNEL_TYPE * pbx_channel);
	int(*const queue_control) (const PBX_CHANNEL_TYPE * pbx_channel, enum ast_control_frame_type control);
	int(*const queue_control_data) (const PBX_CHANNEL_TYPE * pbx_channel, enum ast_control_frame_type control, const void *data, size_t datalen);

	/* conference */
	boolean_t(*const allocTempPBXChannel) (PBX_CHANNEL_TYPE * pbxSrcChannel, PBX_CHANNEL_TYPE ** pbxDstChannel);
	boolean_t(*const masqueradeHelper) (PBX_CHANNEL_TYPE *pbxChannel, PBX_CHANNEL_TYPE *pbxTmpchannel);
	PBX_CHANNEL_TYPE *(*const requestAnnouncementChannel) (pbx_format_type format, const PBX_CHANNEL_TYPE * requestor, void *data);

	boolean_t(*const set_language)(PBX_CHANNEL_TYPE *pbxChannel, const char *language);

	/* devicestate / extension state */
	skinny_busylampfield_state_t (*const getExtensionState)(const char *extension, const char *context);

	PBX_CHANNEL_TYPE *(*const findPickupChannelByExtenLocked)(PBX_CHANNEL_TYPE *chan, const char *exten, const char *context);

// 	PBX_ENDPOINT_TYPE *(*const endpoint_create)(const char *tech, const char *resource);
//	void (*const endpoint_online)(PBX_ENDPOINT_TYPE *endpoint);
//	void (*const endpoint_offline)(PBX_ENDPOINT_TYPE *endpoint);
//	void (*const endpoint_shutdown)(PBX_ENDPOINT_TYPE *endpoint);

        void (*const set_owner)(sccp_channel_t *channel, PBX_CHANNEL_TYPE *pbx_channel);
	int (*const dumpchan) (PBX_CHANNEL_TYPE *pbx_channel, char *buf, size_t size);
	boolean_t (*const channel_is_bridged) (sccp_channel_t *channel);
	PBX_CHANNEL_TYPE *(*const get_bridged_channel) (PBX_CHANNEL_TYPE *pbx_channel);					/* takes pbx_channel_ref */
	PBX_CHANNEL_TYPE *(*const get_underlying_channel) (PBX_CHANNEL_TYPE *pbx_channel);				/* takes pbx_channel_ref */
	boolean_t (*const attended_transfer) (sccp_channel_t *destination_channel, sccp_channel_t *source_channel);	/* takes pbx_channel_ref on source_channel */
	/* *INDENT-ON* */
} PbxInterface;

extern const PbxInterface iPbx;
// kate: indent-width 8; replace-tabs off; indent-mode cstyle; auto-insert-doxygen on; line-numbers on; tab-indents on; keep-extra-spaces off; auto-brackets off;
