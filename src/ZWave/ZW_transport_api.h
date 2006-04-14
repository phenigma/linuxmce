//
// C++ Interface: PlutoZWSerialAPI
//
//
// Author:	Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _ZW_TRANSPORT_API_H_
#define _ZW_TRANSPORT_API_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/* Max number of nodes in a Z-wave system */
#define ZW_MAX_NODES        232

/* Transmit frame option flags */
#define TRANSMIT_OPTION_ACK         0x01   /* request acknowledge from destination node */
#define TRANSMIT_OPTION_LOW_POWER   0x02   /* transmit at low output power level (1/3 of normal RF range)*/
#define TRANSMIT_OPTION_AUTO_ROUTE  0x04   /* request retransmission via repeater nodes */

/* Received frame status flags */
#define RECEIVE_STATUS_ROUTED_BUSY  0x01
#define RECEIVE_STATUS_LOW_POWER    0x02   /* received at low output power level */


/* Predefined Node ID's */
#define NODE_BROADCAST              0xFF    /* broadcast */
#define ZW_TEST_NOT_A_NODEID        0x00    /* */

/* Transmit complete codes */
#define TRANSMIT_COMPLETE_OK      0x00
#define TRANSMIT_COMPLETE_NO_ACK  0x01  /* retransmission error */
#define TRANSMIT_COMPLETE_FAIL    0x02  /* transmit error */
#define TRANSMIT_ROUTING_NOT_IDLE 0x03 /*transmit error*/
#ifdef ZW_CONTROLLER
/* Assign route transmit complete but no routes was found */
#define TRANSMIT_COMPLETE_NOROUTE 0x04  /* no route found in assignroute */
                                        /* therefore nothing was transmitted */
#endif

#ifdef ZW_SLAVE_ROUTING
/* ZW_REQUEST_NETWORK_UPDATE callback values */
#define ZW_ROUTE_UPDATE_DONE      0x00
#define ZW_ROUTE_UPDATE_ABORT     0x01
#define ZW_ROUTE_UPDATE_WAIT      0x02
#define ZW_ROUTE_UPDATE_DISABLED  0x03
#endif /*ZW_SLAVE_ROUTING*/

/* ZW_REDISCOVERY_NEEDED callback values.
   Note that they are different from ZW_REQUEST_NETWORK_UPDATE callbacks*/
#define ZW_ROUTE_LOST_FAILED      0x04  /*Node Asked wont help us*/
#define ZW_ROUTE_LOST_ACCEPT      0x05  /*Accepted to help*/

#endif /* _ZW_TRANSPORT_API_H_ */

