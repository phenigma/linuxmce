
/*!
 * \file 	sccp_socket.h
 * \brief 	SCCP Socket Header
 * \author 	Sergio Chersovani <mlists [at] c-net.it>
 * \note	Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 *
 * $Date: 2012-03-30 10:09:59 +0000 (Fri, 30 Mar 2012) $
 * $Revision: 3339 $  
 */

#    ifndef __SCCP_SOCKET_H
#define __SCCP_SOCKET_H

void *sccp_socket_thread(void *ignore);
void sccp_session_sendmsg(const sccp_device_t * device, sccp_message_t t);
int sccp_session_send(const sccp_device_t * device, sccp_moo_t * r);
int sccp_session_send2(sccp_session_t * s, sccp_moo_t * r);
sccp_session_t *sccp_session_reject(sccp_session_t * session, char *message);
void sccp_session_tokenReject(sccp_session_t * session, uint32_t backoff_time);
void sccp_session_tokenAck(sccp_session_t * session);
void sccp_session_tokenRejectSPCP(sccp_session_t * session, uint32_t features);
void sccp_session_tokenAckSPCP(sccp_session_t * session, uint32_t features);
struct in_addr *sccp_session_getINaddr(sccp_device_t * device, int type);
void sccp_session_getSocketAddr(const sccp_device_t * device, struct sockaddr_in *sin);

#    endif
