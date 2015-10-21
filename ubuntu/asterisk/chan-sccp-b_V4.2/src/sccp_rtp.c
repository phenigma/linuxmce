/*!
 * \file        sccp_rtp.c
 * \brief       SCCP RTP Class
 * \author      Sergio Chersovani <mlists [at] c-net.it>
 * \note                Reworked, but based on chan_sccp code.
 *              The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *              Modified by Jan Czmok and Julien Goodwin
 * \note                This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 *
 * $Date: 2015-10-10 09:29:55 +0000 (Sat, 10 Oct 2015) $
 * $Revision: 6200 $
 */

/*! 
 * =============================
 * Example Networks
 * =============================
 * tokyo 200.0.0.254 / 172.20.0.0
 * 7905:172.20.0.5
 * 
 * havana 150.0.0.254 / 192.168.0.0
 * 7970:192.168.0.5
 * 
 * amsterdam 100.0.0.254 / 10.10.0.0 (IP-Forward) (NoNat)
 * (PBX):100.0.0.1 & 10.10.0.1)
 * 7941: 10.10.0.5
 * 7942: 10.10.0.6
 * 
 * berlin 80.0.0.254 / 10.20.0.0 (Port Forward) (Nat) sccp.conf needs externip
 * (PBX): 10.20.0.1
 * 7941: 10.20.0.5
 * 
 * ====================================
 * Example Calls
 * ====================================
 * amsterdam -> amsterdam via amsterdam : inDirectRTP
 * 172.20.0.5 ->                                           10.0.0.1:PBX:10.0.0.1                                           -> 10.0.0.6
 * leg1:                                                    us           them
 * leg2:                                                   them           us
 * 
 * amsterdam -> amsterdam via amsterdam : DirectRTP
 * 172.20.0.5 ->                                           10.0.0.1:PBX:10.0.0.1                                           -> 10.0.0.6
 * leg1:us                                                                                                                      them
 * leg2:them                                                                                                                     us
 * 
 * tokyo -> amsterdam via amsterdam (Single NAT : IP-Forward) inDirectRTP
 * 172.20.0.5 -> 200.0.0.254 -> Internet ->               100.0.0.1:PBX:10.0.0.1                                           -> 10.0.0.5
 * leg1:              us                                                  them
 * leg2:                                                     them                                                                us
 * 
 * tokyo -> amsterdam via amsterdam (Single NAT : IP-Forward) DirectRTP
 * 172.20.0.5 -> 200.0.0.254 -> Internet ->               100.0.0.1:PBX:10.0.0.1                                           -> 10.0.0.5
 * leg1:              us                                                                                                        them
 * leg2:             them                                                                                                        us
 * 
 * tokyo -> havana via amsterdam (Single NAT : IP-Forward) inDirectRTP
 * 172.20.0.5 -> 200.0.0.254 -> Internet ->               100.0.0.1:PBX:100.0.0.1               -> Internet -> 150.0.0.254 -> 192.168.0.5
 * leg1:              us                                                  them
 * leg2:                                                    them                                                   us
 * 
 * tokyo -> havana via amsterdam (Single NAT : IP-Forward) DirectRTP
 * 172.20.0.5 -> 200.0.0.254 -> Internet ->               100.0.0.1:PBX:100.0.0.1               -> Internet -> 150.0.0.254 -> 192.168.0.5
 * leg1:              us                                                                                          them
 * leg2:             them                                                                                          us
 * 
 * tokyo -> havana via berlin (Double Nat : Port-Forward on PBX Location) inDirectRTP
 * 172.20.0.5 -> 200.0.0.254 -> Internet -> 80.0.0.254 -> 10.20.0.1:PBX:10.20.0.1 -> 80.0.0.254 -> Internet -> 150.0.0.254 -> 192.168.0.5
 * leg1:              us                                                               them
 * leg2:                                       them                                                                us
 * 
 * tokyo -> havana via berlin (Double Nat : Port-Forward on PBX Location) DirectRTP
 * 172.20.0.5 -> 200.0.0.254 -> Internet -> 80.0.0.254 -> 10.20.0.1:PBX:10.20.0.1 -> 80.0.0.254 -> Internet -> 150.0.0.254 -> 192.168.0.5
 * leg1:              us                                                                                          them
 * leg2:             them                                                                                          us
 * 
 * ====================================
 * How to name the addresses
 * ====================================
 * 172.20.0.5 / 192.168.0.5 = phone  			= physicalIP	=> (and phone_remote for the phone on the other side of the channel) This information does not get send to the pbx
 * 200.0.0.254 / 150.0.0.254 = d->session->sin		= reachableVia	=> (can be equal to physicalIP), remote ip-address + port of the connection (gotten from physicalIP)
									=> written to phone(us) during openreceivechannel
									=> written to phone_remote(them) during startmediatransmission
 * 100.0.0.254 / 100.0.0.254 = externalip		= rtp->remote	=> only required when double nat
 * 100.0.0.1 / 100.0.0.1 = d->session->ourip		= rtp->remote	=> local ip-address + port of the phone's connection 
 * 10.0.0.1 =  d->session->ourip			= rtp->remote	=> local ip-address + port of the phone's connection
 */

#include <config.h>
#include "common.h"
#include "sccp_channel.h"
#include "sccp_device.h"
#include "sccp_line.h"
#include "sccp_utils.h"
#include "sccp_rtp.h"
#include "sccp_socket.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 6200 $");

/*!
 * \brief create a new rtp server for audio data
 * \param c SCCP Channel
 */
int sccp_rtp_createAudioServer(constChannelPtr c)
{
	boolean_t rtpResult = FALSE;
	boolean_t isMappedIPv4;

	if (!c) {
		return FALSE;
	}
	if (c->rtp.audio.rtp) {
		sccp_log((DEBUGCAT_RTP)) (VERBOSE_PREFIX_3 "we already have a rtp server, we use this one\n");
		return TRUE;
	}
	sccp_rtp_t *audio = (sccp_rtp_t *) &(c->rtp.audio);
	struct sockaddr_storage *phone_remote = &audio->phone_remote;

	if (iPbx.rtp_audio_create) {
		rtpResult = (boolean_t) iPbx.rtp_audio_create((sccp_channel_t *) c);
	} else {
		pbx_log(LOG_ERROR, "we should start our own rtp server, but we dont have one\n");
		return FALSE;
	}

	if (!sccp_rtp_getUs(audio, phone_remote)) {
		pbx_log(LOG_WARNING, "%s: Did not get our rtp part\n", c->currentDeviceId);
		return FALSE;
	}

	uint16_t port = sccp_rtp_getServerPort(&c->rtp.audio);
	sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: (createAudioServer) RTP Server Port: %d\n", c->currentDeviceId, port);

	/* depending on the client connection, we us ipv4 or ipv6 */
	AUTO_RELEASE sccp_device_t *device = sccp_channel_getDevice_retained(c);

	if (device) {
		sccp_session_getOurIP(device->session, phone_remote, 0);
		sccp_socket_setPort(phone_remote, port);

		char buf[NI_MAXHOST + NI_MAXSERV];
		sccp_copy_string(buf, sccp_socket_stringify(phone_remote), sizeof(buf));
		isMappedIPv4 = sccp_socket_ipv4_mapped(phone_remote, (struct sockaddr_storage *) phone_remote);
		sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: (createAudioServer) updated remote phone ip to : %s, family:%s, mapped: %s\n", device->id, buf, sccp_socket_is_IPv4(phone_remote) ? "IPv4" : "IPv6", isMappedIPv4 ? "True" : "False");
	}

	return rtpResult;
}

/*!
 * \brief create a new rtp server for video data
 * \param c SCCP Channel
 */
int sccp_rtp_createVideoServer(constChannelPtr c)
{
	boolean_t rtpResult = FALSE;

	if (!c) {
		return FALSE;
	}
	if (c->rtp.video.rtp) {
		pbx_log(LOG_ERROR, "we already have a rtp server, why dont we use this?\n");
		return TRUE;
	}

	if (iPbx.rtp_video_create) {
		rtpResult = (boolean_t) iPbx.rtp_video_create((sccp_channel_t *) c);
	} else {
		pbx_log(LOG_ERROR, "we should start our own rtp server, but we dont have one\n");
	}

	if (!sccp_rtp_getUs(&c->rtp.video, &((sccp_channel_t *) c)->rtp.video.phone_remote)) {
		pbx_log(LOG_WARNING, "%s: Did not get our rtp part\n", c->currentDeviceId);
	}

	return rtpResult;
}

/*!
 * \brief Stop an RTP Source.
 * \param c SCCP Channel
 */
void sccp_rtp_stop(constChannelPtr channel)
{
	if (!channel) {
		return;
	}
	if (iPbx.rtp_stop) {
		if (channel->rtp.audio.rtp) {
			PBX_RTP_TYPE *rtp = (PBX_RTP_TYPE *) channel->rtp.audio.rtp;		/* discard const */
			sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_4 "%s: Stopping PBX audio rtp transmission on channel %08X\n", channel->currentDeviceId, channel->callid);
			iPbx.rtp_stop(rtp);
		}
		if (channel->rtp.video.rtp) {
			PBX_RTP_TYPE *rtp = (PBX_RTP_TYPE *) channel->rtp.video.rtp;		/* discard const */
			sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_4 "%s: Stopping PBX video rtp transmission on channel %08X\n", channel->currentDeviceId, channel->callid);
			iPbx.rtp_stop(rtp);
		}
	} else {
		pbx_log(LOG_ERROR, "no pbx function to stop rtp\n");
	}
}

/*!
 * \brief Destroy RTP Source.
 * \param c SCCP Channel
 */
void sccp_rtp_destroy(constChannelPtr c)
{
	sccp_line_t *l = c->line;
	
	sccp_rtp_t *audio = (sccp_rtp_t *) &(c->rtp.audio);
	sccp_rtp_t *video = (sccp_rtp_t *) &(c->rtp.video);

	if (audio->rtp) {
		sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: destroying PBX rtp server on channel %s-%08X\n", c->currentDeviceId, l ? l->name : "(null)", c->callid);
		iPbx.rtp_destroy(audio->rtp);
		audio->rtp = NULL;
	}

	if (video->rtp) {
		sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: destroying PBX vrtp server on channel %s-%08X\n", c->currentDeviceId, l ? l->name : "(null)", c->callid);
		iPbx.rtp_destroy(video->rtp);
		video->rtp = NULL;
	}
}

/*!
 * \brief set the address the phone should send rtp media.
 * \param c SCCP Channel
 * \param rtp SCCP RTP
 * \param new_peer socket info to remote device
 */
void sccp_rtp_set_peer(constChannelPtr c, sccp_rtp_t * const rtp, struct sockaddr_storage *new_peer)
{
	/* validate socket */
	if (sccp_socket_getPort(new_peer) == 0) {
		sccp_log((DEBUGCAT_RTP)) (VERBOSE_PREFIX_2 "%s: ( sccp_rtp_set_peer ) remote information are invalid, dont change anything\n", c->currentDeviceId);
		return;
	}

	/* check if we have new infos */
	if (socket_equals(new_peer, &rtp->phone_remote)) {
		sccp_log((DEBUGCAT_RTP)) (VERBOSE_PREFIX_2 "%s: (sccp_rtp_set_peer) remote information are equal to the current one, ignore change\n", c->currentDeviceId);
		return;
	}
	//memcpy(&c->rtp.audio.phone_remote, new_peer, sizeof(c->rtp.audio.phone_remote));
	memcpy(&rtp->phone_remote, new_peer, sizeof(rtp->phone_remote));

	// inet_ntop(rtp->phone_remote.ss_family, sccp_socket_getAddr(&rtp->phone_remote), addressString, sizeof(addressString));
	// pbx_log(LOG_NOTICE, "%s: ( sccp_rtp_set_peer ) Set remote address to %s:%d\n", c->currentDeviceId, addressString, sccp_socket_getPort(new_peer));
	pbx_log(LOG_NOTICE, "%s: ( sccp_rtp_set_peer ) Set remote address to %s\n", c->currentDeviceId, sccp_socket_stringify(&rtp->phone_remote));

	if (rtp->readState & SCCP_RTP_STATUS_ACTIVE) {
		/* Shutdown any early-media or previous media on re-invite */
		/*! \todo we should wait for the acknowledgement to get back. We don't have a function/procedure in place to do this at this moment in time (sccp_dev_send_wait) */
		sccp_log((DEBUGCAT_RTP)) (VERBOSE_PREFIX_2 "%s: (sccp_rtp_set_peer) Stop media transmission on channel %d\n", c->currentDeviceId, c->callid);

		/*! \todo we should check if this is a video or autio rtp */
		// sccp_channel_stopmediatransmission(c);
		// sccp_channel_startMediaTransmission(c);
		sccp_channel_updateMediaTransmission(c);
	}

}

/*!
 * \brief set the address where the phone should send rtp media.
 * \param c SCCP Channel
 * \param rtp SCCP RTP
 * \param new_peer socket info to remote device
 */
void sccp_rtp_set_phone(constChannelPtr c, sccp_rtp_t * const rtp, struct sockaddr_storage *new_peer)
{
	/* validate socket */
	if (sccp_socket_getPort(new_peer) == 0) {
		sccp_log((DEBUGCAT_RTP)) (VERBOSE_PREFIX_2 "%s: (sccp_rtp_set_phone) remote information are invalid, dont change anything\n", c->currentDeviceId);
		return;
	}

	AUTO_RELEASE sccp_device_t *device = sccp_channel_getDevice_retained(c);

	if (device) {

		/* check if we have new infos */
		/*! \todo if we enable this, we get an audio issue when resume on the same device, so we need to force asterisk to update -MC */
		/*
		if (socket_equals(new_peer, &c->rtp.audio.phone)) {
			sccp_log((DEBUGCAT_RTP)) (VERBOSE_PREFIX_2 "%s: (sccp_rtp_set_phone) remote information are equal to the current one, ignore change\n", c->currentDeviceId);
			//return;
		} 
		*/

		memcpy(&rtp->phone, new_peer, sizeof(rtp->phone));

		//update pbx
		if (iPbx.rtp_setPhoneAddress) {
			iPbx.rtp_setPhoneAddress(rtp, new_peer, device->nat >= SCCP_NAT_ON ? 1 : 0);
		}

		char buf1[NI_MAXHOST + NI_MAXSERV];

		sccp_copy_string(buf1, sccp_socket_stringify(&rtp->phone_remote), sizeof(buf1));
		char buf2[NI_MAXHOST + NI_MAXSERV];

		sccp_copy_string(buf2, sccp_socket_stringify(&rtp->phone), sizeof(buf2));
		sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: Tell PBX   to send RTP/UDP media from %s to %s (NAT: %s)\n", DEV_ID_LOG(device), buf1, buf2, sccp_nat2str(device->nat));
	}
}

int sccp_rtp_updateNatRemotePhone(constChannelPtr c, sccp_rtp_t *const rtp)
{
	int res = 0;
	//sccp_rtp_t *audio = (sccp_rtp_t *) &(channel->rtp.audio);
	AUTO_RELEASE sccp_device_t *d = sccp_channel_getDevice_retained(c);
	if (d) {
		struct sockaddr_storage sus = { 0 };
		sccp_session_getOurIP(d->session, &sus, 0);
		uint16_t usFamily = sccp_socket_is_IPv6(&sus) ? AF_INET6 : AF_INET;
		//sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: (startMediaTransmission) us: %s, usFamily: %s\n", d->id, sccp_socket_stringify(&sus), (usFamily == AF_INET6) ? "IPv6" : "IPv4");

		struct sockaddr_storage *phone_remote = &rtp->phone_remote;
		uint16_t remoteFamily = (rtp->phone_remote.ss_family == AF_INET6 && !sccp_socket_is_mapped_IPv4(phone_remote)) ? AF_INET6 : AF_INET;
		//sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: (startMediaTransmission) remote: %s, remoteFamily: %s\n", d->id, sccp_socket_stringify(phone_remote), (remoteFamily == AF_INET6) ? "IPv6" : "IPv4");

		/*! \todo move the refreshing of the hostname->ip-address to another location (for example scheduler) to re-enable dns hostname lookup */
		if (d->nat >= SCCP_NAT_ON) {
			if ((usFamily == AF_INET) != remoteFamily) {						/* device needs correction for ipv6 address in remote */
				uint16_t port = sccp_rtp_getServerPort(rtp);					/* get rtp server port */

				if (!sccp_socket_getExternalAddr(phone_remote)) {				/* Use externip (PBX behind NAT Firewall */
					memcpy(phone_remote, &sus, sizeof(struct sockaddr_storage));		/* Fallback: use ip-address of incoming interface */
				}
				sccp_socket_ipv4_mapped(phone_remote, phone_remote);				/*!< we need this to convert mapped IPv4 to real IPv4 address */
				sccp_socket_setPort(phone_remote, port);

			} else if ((usFamily == AF_INET6) != remoteFamily) {					/* the device can do IPv6 but should send it to IPv4 address (directrtp possible) */
				struct sockaddr_storage sas;

				memcpy(&sas, phone_remote, sizeof(struct sockaddr_storage));
				sccp_socket_ipv4_mapped(&sas, &sas);
			}
			sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: (startMediaTransmission) new remote: %s, new remoteFamily: %s\n", d->id, sccp_socket_stringify(phone_remote), (remoteFamily == AF_INET6) ? "IPv6" : "IPv4");
			res = 1;
		}
		
		char buf1[NI_MAXHOST + NI_MAXSERV];
		char buf2[NI_MAXHOST + NI_MAXSERV];

		sccp_copy_string(buf1, sccp_socket_stringify(&rtp->phone), sizeof(buf1));
		sccp_copy_string(buf2, sccp_socket_stringify(phone_remote), sizeof(buf2));

		sccp_log(DEBUGCAT_RTP) (VERBOSE_PREFIX_3 "%s: Tell Phone to send RTP/UDP media from %s to %s (NAT: %s)\n", DEV_ID_LOG(d), buf1, buf2, sccp_nat2str(d->nat));
	}
	return res;
}


/*!
 * \brief Get Audio Peer RTP Information
 */
sccp_rtp_info_t sccp_rtp_getAudioPeerInfo(constChannelPtr c, sccp_rtp_t **rtp)
{
	sccp_rtp_info_t result = SCCP_RTP_INFO_NORTP;

	AUTO_RELEASE sccp_device_t *device = sccp_channel_getDevice_retained(c);

	if (!device) {
		return SCCP_RTP_INFO_NORTP;
	}

	*rtp = &(((sccp_channel_t *) c)->rtp.audio);

	result = SCCP_RTP_INFO_AVAILABLE;
	// \todo add apply_ha(d->ha, &sin) check here instead
	if (device->directrtp && device->nat == SCCP_NAT_OFF && !c->conference) {
		result |= SCCP_RTP_INFO_ALLOW_DIRECTRTP;
	}
	return result;
}

/*!
 * \brief Get Video Peer RTP Information
 */
sccp_rtp_info_t sccp_rtp_getVideoPeerInfo(constChannelPtr c, sccp_rtp_t ** rtp)
{
	sccp_rtp_info_t result = SCCP_RTP_INFO_NORTP;

	AUTO_RELEASE sccp_device_t *device = sccp_channel_getDevice_retained(c);

	if (!device) {
		return SCCP_RTP_INFO_NORTP;
	}

	*rtp = &(((sccp_channel_t *) c)->rtp.video);

	result = SCCP_RTP_INFO_AVAILABLE;
	if (device->directrtp && device->nat == SCCP_NAT_OFF && !c->conference) {
		result |= SCCP_RTP_INFO_ALLOW_DIRECTRTP;
	}
	return result;
}

/*!
 * \brief Get Audio Peer
 */
boolean_t sccp_rtp_getAudioPeer(constChannelPtr c, struct sockaddr_storage **new_peer)
{
	sccp_rtp_t *audio = (sccp_rtp_t *) &(c->rtp.audio);
	*new_peer = &audio->phone_remote;
	return TRUE;
}

/*!
 * \brief Get Video Peer
 */
boolean_t sccp_rtp_getVideoPeer(constChannelPtr c, struct sockaddr_storage **new_peer)
{
	sccp_rtp_t *video = (sccp_rtp_t *) &(c->rtp.video);
	*new_peer = &video->phone_remote;
	return TRUE;
}


/*!
 * \brief Get Payload Type
 */
uint8_t sccp_rtp_get_payloadType(const sccp_rtp_t * const rtp, skinny_codec_t codec)
{
	if (iPbx.rtp_get_payloadType) {
		return iPbx.rtp_get_payloadType(rtp, codec);
	} else {
		return 97;
	}
}

/*!
 * \brief Retrieve Phone Socket Information
 */
boolean_t sccp_rtp_getUs(const sccp_rtp_t *rtp, struct sockaddr_storage *us)
{
	if (rtp->rtp) {
		iPbx.rtp_getUs(rtp->rtp, us);
		return TRUE;
	} else {
		// us = &rtp->phone_remote;
		return FALSE;
	}
}

uint16_t sccp_rtp_getServerPort(const sccp_rtp_t * const rtp)
{
	uint16_t port = 0;
	struct sockaddr_storage sas;

	sccp_rtp_getUs(rtp, &sas);

	port = sccp_socket_getPort(&sas);
	return port;
}

/*!
 * \brief Retrieve Phone Socket Information
 */
boolean_t sccp_rtp_getPeer(const sccp_rtp_t * const rtp, struct sockaddr_storage *them)
{
	if (rtp->rtp) {
		iPbx.rtp_getPeer(rtp->rtp, them);
		return TRUE;
	} else {
		return FALSE;
	}
}

/*!
 * \brief Get Sample Rate
 */
int sccp_rtp_get_sampleRate(skinny_codec_t codec)
{
	if (iPbx.rtp_get_sampleRate) {
		return iPbx.rtp_get_sampleRate(codec);
	} else {
		return 3840;
	}
}

// kate: indent-width 8; replace-tabs off; indent-mode cstyle; auto-insert-doxygen on; line-numbers on; tab-indents on; keep-extra-spaces off; auto-brackets off;
