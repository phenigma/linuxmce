/*!
 * \file        sccp_event.h
 * \brief       SCCP Event Header
 * \author      Marcello Ceschia <marcelloceschia [at] users.sourceforge.net>
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 * \since       2009-09-02
 *
 * $Date: 2013-03-13 19:13:55 +0100 (Wed, 13 Mar 2013) $
 * $Revision: 4361 $  
 */

#ifndef SCCP_EVENT_H_
#define SCCP_EVENT_H_

//#include <config.h>
#include "common.h"

/* structures */

/* event types to notify modular systems */

/*!
 * \brief SCCP Event Type ENUM
 */
typedef enum {
        /* *INDENT-OFF* */
	SCCP_EVENT_LINE_CREATED 	= 1 << 0,
	SCCP_EVENT_LINE_CHANGED 	= 1 << 1,
	SCCP_EVENT_LINE_DELETED 	= 1 << 2,
	SCCP_EVENT_DEVICE_ATTACHED 	= 1 << 3,				/*!< device attached to line */
	SCCP_EVENT_DEVICE_DETACHED 	= 1 << 4,				/*!< device removed from line */
	SCCP_EVENT_DEVICE_PREREGISTERED = 1 << 5,				/*!< device preregistered (tokenReq) */
	SCCP_EVENT_DEVICE_REGISTERED 	= 1 << 6,				/*!< device registers successfully */
	SCCP_EVENT_DEVICE_UNREGISTERED 	= 1 << 7,				/*!< device isnt registered any more */
	SCCP_EVENT_FEATURE_CHANGED 	= 1 << 8,				/*!< some feature (e.g. cfwd, dnd) is changed */
	SCCP_EVENT_LINESTATUS_CHANGED 	= 1 << 9				/*!< the status of a line is changed is changed */
	/* *INDENT-ON* */
} sccp_event_type_t;												/*!< SCCP Event Type ENUM */

#define NUMBER_OF_EVENT_TYPES 10

static const struct sccp_event_type {
	sccp_event_type_t event_type;
	const char *const text;
} sccp_event_types[] = {
	/* *INDENT-OFF* */
	{SCCP_EVENT_LINE_CREATED,		"Line Created"},
	{SCCP_EVENT_LINE_CHANGED,		"Line Changed"},
	{SCCP_EVENT_LINE_DELETED,		"Line Deleted"},
	{SCCP_EVENT_DEVICE_ATTACHED,		"Device Attached"},
	{SCCP_EVENT_DEVICE_DETACHED,		"Device Detached"},
	{SCCP_EVENT_DEVICE_PREREGISTERED,	"Device Preregistered"},
	{SCCP_EVENT_DEVICE_REGISTERED,		"Device Registered"},
	{SCCP_EVENT_DEVICE_UNREGISTERED,	"Device Unregistered"},
	{SCCP_EVENT_FEATURE_CHANGED,		"Feature Changed"},
	{SCCP_EVENT_LINESTATUS_CHANGED,		"LineStatus Changed"},
	/* *INDENT-ON* */
};

#define sccp_event_retain(_x) 		sccp_refcount_retain(_x, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define sccp_event_release(_x) 		sccp_refcount_release(_x, __FILE__, __LINE__, __PRETTY_FUNCTION__)

/*!
 * \brief SCCP Event Structure
 */
typedef struct sccp_event sccp_event_t;
typedef void (*sccp_event_callback_t) (const sccp_event_t * event);

extern struct sccp_event_subscriptions *sccp_event_listeners;

/*!
 * \brief SCCP Event Structure
 */
struct sccp_event {
	sccp_event_type_t type;											/*!< Event Type */

	/*! 
	 * \brief SCCP Event Data Union
	 */
	union sccp_event_data {
		struct {
			sccp_line_t *line;									/*!< SCCP Line */
		} lineCreated;											/*!< Event Line Created Structure */
		struct {
			sccp_device_t *device;									/*!< SCCP Device */
		} deviceRegistered;										/*!< Event Device Registered Structure */
		struct {
			sccp_linedevices_t *linedevice;								/*!< SCCP device line */
		} deviceAttached;										/*!< Event Device Attached Structure */

		struct {
			sccp_device_t *device;									/*!< SCCP device */
			sccp_linedevices_t *linedevice;								/*!< SCCP linedevice */
			sccp_feature_type_t featureType;							/*!< what feature is changed */
		} featureChanged;										/*!< Event feature changed Structure */

		struct {
			sccp_device_t *device;									/*!< SCCP device */
			sccp_line_t *line;									/*!< SCCP line */
			uint8_t state;										/*!< state */
		} lineStatusChanged;										/*!< Event feature changed Structure */

	} event;												/*!< SCCP Event Data Union */
};														/*!< SCCP Event Structure */

typedef struct sccp_event_subscriber sccp_event_subscriber_t;

/*!
 * \brief SCCP Event Subscriber Structure
 */
struct sccp_event_subscriber {
	sccp_event_type_t eventType;
	sccp_event_callback_t callback_function;

	//      SCCP_LIST_ENTRY(sccp_event_subscriber_t) list;
};

/*!
 * \brief SCCP Event Subscribtions Structure
 */
// struct sccp_event_subscriptions {
//      SCCP_LIST_HEAD(, sccp_event_subscriber_t) subscriber;
// };

void sccp_event_unsubscribe(sccp_event_type_t eventType, sccp_event_callback_t cb);
void sccp_event_subscribe(sccp_event_type_t eventType, sccp_event_callback_t cb, boolean_t allowASyncExecution);

void sccp_event_module_start(void);
void sccp_event_module_stop(void);

void sccp_event_fire(const sccp_event_t * event);

#endif														/* SCCP_EVENT_H_ */
