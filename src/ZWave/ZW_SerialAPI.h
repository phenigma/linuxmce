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


/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#define SOF 0x01
#define ACK 0x06
#define NAK 0x15

/* Frame types */
#define REQUEST                                         0x00
#define RESPONSE                                        0x01

/* Flags used in SERIAL_API_GET_INIT_DATA */
#define GET_INIT_DATA_FLAG_SLAVE_API                    0x01
#define GET_INIT_DATA_FLAG_TIMER_SUPPORT                0x02
#define GET_INIT_DATA_FLAG_CONTROLLER_STATUS            0x04
#define GET_INIT_DATA_FLAG_IS_SUC                       0x08

/* Function IDs */
#define FUNC_ID_SERIAL_API_GET_INIT_DATA                0x02
#define FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION        0x03
#define FUNC_ID_APPLICATION_COMMAND_HANDLER             0x04

#define FUNC_ID_ZW_SET_RF_RECEIVE_MODE                  0x10
//#define FUNC_ID_ZW_SET_SLEEP_MODE                     0x11
#define FUNC_ID_ZW_SEND_NODE_INFORMATION                0x12
#define FUNC_ID_ZW_SEND_DATA                            0x13
#define FUNC_ID_ZW_SEND_DATA_MULTI                      0x14
#define FUNC_ID_ZW_GET_VERSION                          0x15
#define FUNC_ID_MEMORY_GET_ID                           0x20
#define FUNC_ID_MEMORY_GET_BYTE                         0x21
#define FUNC_ID_MEMORY_PUT_BYTE                         0x22
#define FUNC_ID_MEMORY_GET_BUFFER                       0x23
#define FUNC_ID_MEMORY_PUT_BUFFER                       0x24
#define FUNC_ID_CLOCK_SET                               0x30
#define FUNC_ID_CLOCK_GET                               0x31
#define FUNC_ID_CLOCK_CMP                               0x32
#define FUNC_ID_RTC_TIMER_CREATE                        0x33
#define FUNC_ID_RTC_TIMER_READ                          0x34
#define FUNC_ID_RTC_TIMER_DELETE                        0x35
#define FUNC_ID_RTC_TIMER_CALL                          0x36
/* Controller only */
#define FUNC_ID_ZW_SET_LEARN_NODE_STATE                 0x40
#define FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO               0x41
#define FUNC_ID_ZW_SET_DEFAULT                          0x42
#define FUNC_ID_ZW_NEW_CONTROLLER                       0x43
#define FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE         0x44
#define FUNC_ID_ZW_REPLICATION_SEND_DATA                0x45
#define FUNC_ID_ZW_ASSIGN_RETURN_ROUTE                  0x46
#define FUNC_ID_ZW_DELETE_RETURN_ROUTE                  0x47
#define FUNC_ID_ZW_REQUEST_NODE_NEIGHBOR_UPDATE         0x48
#define FUNC_ID_APPLICATION_CONTROLLER_UPDATE           0x49
/* Controller only end*/

/* Slave only */
#define FUNC_ID_ZW_SET_LEARN_MODE                       0x50
/* Slave only end */

#define FUNC_ID_ZW_ASSIGN_SUC_RETURN_ROUTE              0x51
#define FUNC_ID_ZW_ENABLE_SUC                           0x52
#define FUNC_ID_ZW_REQUEST_NETWORK_UPDATE               0x53
#define FUNC_ID_ZW_SET_SUC_NODE_ID                      0x54
#define FUNC_ID_ZW_DELETE_SUC_RETURN_ROUTE              0x55
#define FUNC_ID_ZW_GET_SUC_NODE_ID                      0x56

#ifdef TIMER_SUPPORT
#define FUNC_ID_TIMER_START                             0x70
#define FUNC_ID_TIMER_RESTART                           0x71
#define FUNC_ID_TIMER_CANCEL                            0x72
#define FUNC_ID_TIMER_CALL                              0x73
#endif

/*Installer API*/
#define FUNC_ID_GET_ROUTING_TABLE_LINE                  0x80
#define FUNC_ID_GET_TX_COUNTER                          0x81
#define FUNC_ID_RESET_TX_COUNTER                        0x82
#define FUNC_ID_STORE_NODEINFO                          0x83
#define FUNC_ID_STORE_HOMEID                            0x84
/*Installer API only end*/

#define FUNC_ID_LOCK_ROUTE_RESPONSE                     0x90

#ifdef ZW_ROUTING_DEMO
#define FUNC_ID_ZW_SEND_DATA_ROUTE_DEMO					0x91
#endif
/* Application specific external eeprom offsets... */
#define MAGIC_VALUE_OFFSET                              0
#define CONTROLLER_STATUS_OFFSET                        1

#define CONTROLLER_STATUS_SLAVE_MASK                    0x01

#define MAGIC_VALUE                                     0x42

/*Defined libraries*/
#define ZW_LIB_CONTROLLER_STATIC  0x01
#define ZW_LIB_CONTROLLER         0x02
#define ZW_LIB_SLAVE_ENHANCED     0x03
#define ZW_LIB_SLAVE              0x04
#define ZW_LIB_INSTALLER          0x05
#define ZW_NO_INTELLIGENT_LIFE    0x06

/*remote status mask defines*/
#define REMOTE_STATUS_SLAVE_BIT        0x01  /*1 indicates slave*/

/* SetLearnNodeState parameter */
#define LEARN_NODE_STATE_OFF    0
#define LEARN_NODE_STATE_NEW    1
#define LEARN_NODE_STATE_UPDATE 2
#define LEARN_NODE_STATE_DELETE 3

// SetLearnNodeState callback status 
#define LEARN_STATE_ROUTING_PENDING    0x80
#define LEARN_STATE_DONE               0x40
#define LEARN_STATE_FAIL               0x20

// idleLearnState callback status
#define UPDATE_STATE_ROUTING_PENDING  0x80
#define UPDATE_STATE_ADD_DONE         0x40       
#define UPDATE_STATE_DELETE_DONE      0x20 
#define UPDATE_STATE_SUC_ID           0x10

/* ZW_NewController parameter */
#define NEW_CTRL_STATE_SEND				0x01
#define NEW_CTRL_STATE_RECEIVE			0x02
#define NEW_CTRL_STATE_STOP_OK			0x03
#define NEW_CTRL_STATE_STOP_FAIL		0x04
#define NEW_CTRL_STATE_ABORT			0x05
#define NEW_CTRL_STATE_CHANGE			0x06
#define NEW_CTRL_STATE_DELETE			0x07
#define NEW_CTRL_STATE_MAKE_NEW         0x08
// NewController callback status 
#define NEW_CONTROLLER_FAILED			0x00
#define NEW_CONTROLLER_DONE				0x01
#define NEW_CONTROLLER_LEARNED			0x02
#define NEW_CONTROLLER_CHANGE_DONE		0x03
#define NEW_CONTROLLER_DELETED			0x04
#define NEW_CONTROLLER_MAKE_NEW_DONE    0x05

/* Listening bit in NODEINFO capability */
#define NODEINFO_LISTENING_MASK     0x80
#define NODEINFO_LISTENING_SUPPORT  0x80

#define ZW_SUC_SET_SUCCEEDED    0x05
#define ZW_SUC_SET_FAILED       0x06

/* From ZW_transport.h --- begin */
/* Transmit frame option flags */
#define TRANSMIT_OPTION_ACK         0x01   /* request acknowledge from destination node */
#define TRANSMIT_OPTION_LOW_POWER   0x02   /* transmit at low output power level (1/3 of normal RF range)*/  
/*#ifdef ZW_CONTROLLER*/
#define TRANSMIT_OPTION_AUTO_ROUTE  0x04   /* request retransmission via repeater nodes */
#define TRANSMIT_OPTION_FORCE_ROUTE 0x08   /* request transmission via repeater nodes */
/*#endif*/
        
/* Received frame status flags */
#define RECEIVE_STATUS_ROUTED_BUSY  0x01
#define RECEIVE_STATUS_LOW_POWER	0x02   /* received at low output power level */


/* Predefined Node ID's */
#define NODE_BROADCAST    0xFF    /* broadcast */

/* Transmit complete codes */
#define TRANSMIT_COMPLETE_OK      0x00  
#define TRANSMIT_COMPLETE_NO_ACK  0x01  /* retransmission error */
#define TRANSMIT_COMPLETE_FAIL    0x02  /* transmit error */ 
/*#ifdef ZW_CONTROLLER*/
/* Assign route transmit complete but no routes was found */
#define TRANSMIT_COMPLETE_NOROUTE 0x04  /* no route found in assignroute */
                                        /* therefore nothing was transmitted */

