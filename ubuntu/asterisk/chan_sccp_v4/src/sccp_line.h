
/*!
 * \file 	sccp_line.h
 * \brief 	SCCP Line Header
 * \author 	Sergio Chersovani <mlists [at] c-net.it>
 * \note	Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 * 
 * $Date: 2012-01-10 21:44:40 +0000 (Tue, 10 Jan 2012) $
 * $Revision: 3197 $
 */

#    ifndef __SCCP_LINE_H
#define __SCCP_LINE_H

#ifdef CS_DYNAMIC_CONFIG
void sccp_line_pre_reload(void);
void sccp_line_post_reload(void);
#endif
sccp_line_t *sccp_line_create(void);
inline sccp_line_t *__sccp_line_retain(sccp_line_t *l, const char *filename, int lineno, const char *func);
inline sccp_line_t *__sccp_line_release(sccp_line_t *d, const char *filename, int lineno, const char *func);
sccp_line_t *sccp_line_addToGlobals(sccp_line_t * line);
void sccp_line_kill(sccp_line_t * l);
void sccp_line_clean(sccp_line_t * l, boolean_t destroy);
int sccp_line_destroy(const void *ptr);
void sccp_line_delete_nolock(sccp_line_t * l);
void sccp_line_cfwd(sccp_line_t * l, sccp_device_t * device, uint8_t type, char *number);
void sccp_line_addDevice(sccp_line_t * l, sccp_device_t * device, uint8_t lineInstance, struct subscriptionId *subscriptionId);
void sccp_line_removeDevice(sccp_line_t * l, sccp_device_t * device);
void sccp_line_addChannel(sccp_line_t * l, sccp_channel_t * channel);
void sccp_line_removeChannel(sccp_line_t * l, sccp_channel_t * channel);
#    endif										/* __SCCP_LINE_H */
