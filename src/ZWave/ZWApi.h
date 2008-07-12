/*
 *      Copyright (C) 2008 Harald Klein <hari@vt100.at>
 *
 *      This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      See the GNU General Public License for more details.
 */

#ifndef _ZW_API_H_

#define _ZW_API_H_

#define SOF				0x01
#define ACK				0x06
#define NAK				0x15
#define CAN				0x18

#define MAGIC_LEN			29

#define REQUEST				0x0
#define RESPONSE			0x1

#define ZW_GET_VERSION			0x15
#define ZW_MEMORY_GET_ID		0x20	// response: 4byte home id, node id
#define ZW_CLOCK_SET			0x30

#define TRANSMIT_OPTION_ACK         	0x01
#define TRANSMIT_OPTION_LOW_POWER   	0x02
#define TRANSMIT_OPTION_AUTO_ROUTE  	0x04
#define TRANSMIT_OPTION_FORCE_ROUTE 	0x08

#define TRANSMIT_COMPLETE_OK      	0x00  
#define TRANSMIT_COMPLETE_NO_ACK  	0x01  
#define TRANSMIT_COMPLETE_FAIL    	0x02
#define TRANSMIT_COMPLETE_NOROUTE 	0x04

#define RECEIVE_STATUS_TYPE_BROAD     			0x04

#define FUNC_ID_SERIAL_API_GET_INIT_DATA		0x02
#define FUNC_ID_APPLICATION_COMMAND_HANDLER             0x04
#define FUNC_ID_ZW_APPLICATION_UPDATE                   0x49

#define FUNC_ID_SERIAL_API_GET_CAPABILITIES             0x07

#define FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE         0x44

#define FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO               0x41

#define FUNC_ID_ZW_SEND_DATA                            0x13

#define FUNC_ID_ZW_SET_LEARN_MODE                       0x50

#define FUNC_ID_ZW_ENABLE_SUC                           0x52
#define FUNC_ID_ZW_SET_SUC_NODE_ID                      0x54
#define FUNC_ID_ZW_GET_SUC_NODE_ID                      0x56

#define ZW_SUC_FUNC_BASIC_SUC				0x00

#define UPDATE_STATE_NODE_INFO_RECEIVED     		0x84
#define UPDATE_STATE_NODE_INFO_REQ_FAILED		0x81

#define BASIC_TYPE_CONTROLLER                           0x01
#define BASIC_TYPE_STATIC_CONTROLLER                    0x02
#define BASIC_TYPE_SLAVE                                0x03
#define BASIC_TYPE_ROUTING_SLAVE                        0x04

#define GENERIC_TYPE_GENERIC_CONTROLLER                 0x01
#define GENERIC_TYPE_STATIC_CONTROLLER                  0x02
#define GENERIC_TYPE_AV_CONTROL_POINT                   0x03
#define GENERIC_TYPE_DISPLAY                            0x06
#define GENERIC_TYPE_GARAGE_DOOR                        0x07
#define GENERIC_TYPE_THERMOSTAT                         0x08
#define GENERIC_TYPE_WINDOW_COVERING                    0x09
#define GENERIC_TYPE_REPEATER_SLAVE                     0x0F
#define GENERIC_TYPE_SWITCH_BINARY                      0x10
#define GENERIC_TYPE_SWITCH_MULTILEVEL                  0x11
#define GENERIC_TYPE_SWITCH_REMOTE                      0x12
#define GENERIC_TYPE_SWITCH_TOGGLE                      0x13
#define GENERIC_TYPE_SENSOR_BINARY                      0x20
#define GENERIC_TYPE_SENSOR_MULTILEVEL                  0x21
#define GENERIC_TYPE_WATER_CONTROL                      0x22
#define GENERIC_TYPE_METER_PULSE                        0x30
#define GENERIC_TYPE_ENTRY_CONTROL                      0x40
#define GENERIC_TYPE_SEMI_INTEROPERABLE                 0x50
#define GENERIC_TYPE_NON_INTEROPERABLE                  0xFF

#define COMMAND_CLASS_BASIC				0x20
#define BASIC_SET					0x01
#define BASIC_GET					0x02
#define BASIC_REPORT					0x03

#define COMMAND_CLASS_BATTERY				0x80
#define BATTERY_REPORT					0x03

#define COMMAND_CLASS_WAKE_UP                         	0x84

#define WAKE_UP_INTERVAL_SET                         	0x04
#define WAKE_UP_NOTIFICATION                         	0x07
#define WAKE_UP_NO_MORE_INFORMATION                  	0x08

#define COMMAND_CLASS_CONTROLLER_REPLICATION          	0x21
#define CTRL_REPLICATION_TRANSFER_GROUP              	0x31

#define COMMAND_CLASS_SWITCH_MULTILEVEL               	0x26
#define SWITCH_MULTILEVEL_SET                        	0x01
#define SWITCH_MULTILEVEL_GET                        	0x02
#define SWITCH_MULTILEVEL_REPORT                        0x03

#define COMMAND_CLASS_SWITCH_ALL			0x27
#define SWITCH_ALL_ON					0x04
#define SWITCH_ALL_OFF					0x05

#define COMMAND_CLASS_SENSOR_BINARY			0x30
#define SENSOR_BINARY_REPORT				0x03

#define COMMAND_CLASS_ALARM				0x71
#define ALARM_REPORT					0x05

#define COMMAND_CLASS_MULTI_CMD                         0x8F
#define MULTI_CMD_VERSION                               0x01
#define MULTI_CMD_ENCAP                                 0x01
#define MULTI_CMD_RESPONSE_ENCAP                        0x02

#define COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE		0x46
#define SCHEDULE_SET                                    0x01
#define SCHEDULE_GET                                    0x02
#define SCHEDULE_CHANGED_GET				0x04
#define SCHEDULE_CHANGED_REPORT				0x05
#define SCHEDULE_OVERRIDE_GET				0x07
#define SCHEDULE_OVERRIDE_REPORT			0x08

#define COMMAND_CLASS_CLOCK				0x81
#define CLOCK_GET					0x05
#define CLOCK_SET                                       0x04
#define CLOCK_REPORT                                    0x06

#include <deque>

namespace ZWApi
{
	struct ZWJob {
	    char buffer[512];
	    size_t len;
	    time_t timeout;
	    int sendcount;
	    int callbackid;
	    bool await_response;
	};
	struct ZWIntent {
	    int type;
	    int nodeid;
	    time_t timeout;
	    int retrycount;
	};

	class ZWApi
	{
		private:

			static pthread_t readThread;
			pthread_mutex_t mutexSendQueue;
			pthread_mutex_t mutex_serial;

			int serialPort;


			std::deque<ZWJob*> ZWSendQueue;
			std::deque<ZWIntent*> ZWIntentQueue;

			std::string deviceList;


			int callbackpool;
			int ournodeid;
			bool await_ack;
			void *myZWave;

		public:
			ZWApi();

			~ZWApi();

			void *init(const char *device, void *myZWave);
			char checksum(char *buf, int len);

			void *decodeFrame(char *frame, size_t length);
			void *receiveFunction(); 
			bool addIntent(int nodeid, int type);
			int getIntent(int type); 
			bool sendFunction(char *buffer, size_t length, int type, bool response = 0);
			std::string getDeviceList();
			bool zwBasicSet(int node_id, int level);

	};

}
#endif
