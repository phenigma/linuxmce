/*
 *      Copyright (C) 2008 Harald Klein <hari@vt100.at>
 *
 *      This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
 *      This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *      of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *      See the GNU General Public License for more details.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/file.h>

#include <iostream>

#include "Serial.h"
#include "ZWApi.h"

#include "ZWave.h"

#include "Generic_Serial_Device/IOUtils.h"

#include "Gen_Devices/ZWaveBase.h"


extern "C" void *start( void* );

void *start( void *p )
{
ZWApi::ZWApi *base = static_cast<ZWApi::ZWApi*>(p);
base->receiveFunction();
}

ZWApi::ZWApi::ZWApi(void *myZWave) {
	this->myZWave = myZWave;
}

ZWApi::ZWApi::~ZWApi() {
}

void *ZWApi::ZWApi::init(const char *device) {
	char mybuf[1024];

 	// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Initialization...");


        callbackpool = 1;
        ournodeid = -1;
        await_ack = 0;

	OpenSerialPortEx(device,&serialPort);
        mybuf[0]=NAK;
        WriteSerialStringEx(serialPort,mybuf,1);

	pthread_mutex_init(&mutexSendQueue, NULL);

	static pthread_t readThread;
	pthread_create(&readThread, NULL, start, (void*)this);
	// pthread_create(&readThread, NULL, start, (void*)serialPort);
	//
 	// DCE::LoggerWrapper::GetInstance()->Write(LV_ZWAVE, "Request version");
	mybuf[0] = ZW_GET_VERSION;
	sendFunction( mybuf , 1, REQUEST, 0); 

	mybuf[0] = ZW_MEMORY_GET_ID;
	sendFunction( mybuf , 1, REQUEST, 0); 

	mybuf[0] = FUNC_ID_SERIAL_API_GET_CAPABILITIES;
	sendFunction( mybuf , 1, REQUEST, 0); 
	
	mybuf[0] = FUNC_ID_ZW_GET_SUC_NODE_ID;
	sendFunction( mybuf , 1, REQUEST, 0); 
	
	mybuf[0] = FUNC_ID_SERIAL_API_GET_INIT_DATA;
	sendFunction( mybuf , 1, REQUEST, 0); 


}


char ZWApi::ZWApi::checksum(char *buf, int len) {
	int ret = 0xff;
	for (int i=0;i<len;i++) ret ^= buf[i];
	return ret;

}

void *ZWApi::ZWApi::decodeFrame(char *frame, size_t length) {
	char tempbuf[512];
	char tempbuf2[512];
	if (frame[0] == RESPONSE) {
		switch (frame[1]) {
			case FUNC_ID_ZW_GET_SUC_NODE_ID:
				printf("Got reply to GET_SUC_NODE_ID, node: %d\n",frame[2]);
				if (frame[2] == 0) {
					printf("No SUC, we become SUC\n");
					tempbuf[0]=FUNC_ID_ZW_ENABLE_SUC;
					tempbuf[1]=1;
					tempbuf[2]=ZW_SUC_FUNC_BASIC_SUC;
					sendFunction( tempbuf , 3, REQUEST, 0); 
				}
				break;
			;;
			case ZW_MEMORY_GET_ID:
				printf("Got reply to ZW_MEMORY_GET_ID: %d\n",frame[6]);
				ournodeid = frame[6];
				break;
			;;
			case FUNC_ID_SERIAL_API_GET_INIT_DATA:
				printf("Got reply to FUNC_ID_SERIAL_API_GET_INIT_DATA:\n");
				if (frame[4] == MAGIC_LEN) {
					for (int i=5;i<5+MAGIC_LEN;i++) {
						for (int j=0;j<8;j++) {
							// printf("test node %d\n",(i-5)*8+j+1);
							if (frame[i] && (0x01 << j)) {
								printf("found node %d\n",(i-5)*8+j+1);
								// requesting node protocol information
								// tempbuf[0]=0x60; // req. node info frame - test
								addIntent((i-5)*8+j+1,1);
								tempbuf[0]=FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
								tempbuf[1]=(i-5)*8+j+1;
								sendFunction( tempbuf , 2, REQUEST, 0); 
							}
						}
					}
				}
				break;
			;;
			case FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO:
				int tmp_nodeid;
				printf("Got reply to FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO:\n");
				tmp_nodeid = getIntent(1);
				if (frame[6] != 0) {
					// printf("***FOUND NODE:\n");
					printf("***FOUND NODE: %d\n",tmp_nodeid);
					if (((unsigned char)frame[2]) && (0x01 << 7)) {
						printf("listening node\n");
					}
					if (((unsigned char)frame[3]) && (0x01 << 7)) {
						printf("optional functionality\n");
					}
					switch (frame[5]) {
						case BASIC_TYPE_CONTROLLER:
							printf("BASIC TYPE: Controller\n");
							break;
						;;
						case BASIC_TYPE_STATIC_CONTROLLER:
							printf("BASIC TYPE: Static Controller\n");
							break;
						;;
						case BASIC_TYPE_SLAVE:
							printf("BASIC TYPE: Slave\n");
							break;
						;;
						case BASIC_TYPE_ROUTING_SLAVE:
							printf("BASIC TYPE: Routing Slave\n");
							break;
						;;
						default:
							printf("BASIC TYPE: %x\n",frame[5]);
							break;
						;;
					}
					switch (frame[6]) {
						case GENERIC_TYPE_GENERIC_CONTROLLER:
							printf("GENERIC TYPE: Generic Controller\n");
							break;
						;;
						case GENERIC_TYPE_STATIC_CONTROLLER:
							printf("GENERIC TYPE: Static Controller\n");
							break;
						;;
						case GENERIC_TYPE_THERMOSTAT:
							printf("GENERIC TYPE: Thermostat\n");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Standard_Thermostat_CONST);
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_SWITCH_MULTILEVEL:
							printf("GENERIC TYPE: Multilevel Switch\n");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Light_Switch_dimmable_CONST);
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_SWITCH_REMOTE:
							printf("GENERIC TYPE: Remote Switch\n");
							break;
						;;
						case GENERIC_TYPE_SWITCH_BINARY:
							printf("GENERIC TYPE: Binary Switch\n");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Light_Switch_onoff_CONST);
							deviceList += tempbuf2;
							break;
						;;
						case GENERIC_TYPE_SENSOR_BINARY:
							printf("GENERIC TYPE: Sensor Binary\n");
							sprintf(tempbuf2, "%d\t\t\t%d\t\n", tmp_nodeid, DEVICETEMPLATE_Generic_Sensor_CONST);
							deviceList += tempbuf2;
							break;
						;;
						default:
							printf("GENERIC TYPE: %x\n",frame[6]);
							break;
						;;

					}
					printf("SPECIFIC: %d\n",frame[7]);
				}
				break;
			;;
			case FUNC_ID_ZW_SEND_DATA:
				printf("ZW_SEND Response:\n");
				break;

			default:
				break;
			;;
		}

	} else if (frame[0] == REQUEST) {
		switch (frame[1]) {
			case FUNC_ID_APPLICATION_COMMAND_HANDLER:
				printf("FUNC_ID_APPLICATION_COMMAND_HANDLER:");
				switch (frame[5]) {
					case COMMAND_CLASS_CONTROLLER_REPLICATION:
						printf ("COMMAND_CLASS_CONTROLLER_REPLICATION\n");
						// 0x1 0xb 0x0 0x4 0x2 0xef 0x5 0x21 0x31 0x7 0x1 0x1 0xf (#######!1####)
						if (frame[6] == CTRL_REPLICATION_TRANSFER_GROUP) {
							// we simply ack the group information for now
							tempbuf[0]=FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
							sendFunction( tempbuf , 1, REQUEST, 0); 
						} else {
							// ack everything else, too
							tempbuf[0]=FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
							sendFunction( tempbuf , 1, REQUEST, 0); 
						}
						break;
					;;
					case COMMAND_CLASS_WAKE_UP:
						printf ("COMMAND_CLASS_WAKE_UP - ");
						// 0x1 0x8 0x0 0x4 0x4 0x2 0x2 0x84 0x7 0x74 (#########t)
						// if ((COMMAND_CLASS_WAKE_UP == frame[5]) && ( frame[6] == WAKE_UP_NOTIFICATION)) {
						if (frame[6] == WAKE_UP_NOTIFICATION) {
							// handle broadcasts from unconfigured devices
							if (frame[2] && RECEIVE_STATUS_TYPE_BROAD ) { 
								printf("Got broadcast wakeup from node %i, doing WAKE_UP_INTERVAL_SET\n",frame[3]);
								tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
								// node id
								tempbuf[1]=frame[3];
								tempbuf[2]=6;
								tempbuf[3]=COMMAND_CLASS_WAKE_UP;
								tempbuf[4]=WAKE_UP_INTERVAL_SET;
								tempbuf[5]=0;
								tempbuf[6]=0x2;
								tempbuf[7]=0x58;
								tempbuf[8]=ournodeid;
								tempbuf[9]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
								sendFunction( tempbuf , 10, REQUEST, 1); 
							} else {
								printf("Got unicast wakeup from node %i, doing WAKE_UP_NO_MORE_INFORMATION\n",frame[3]);
								// send to sleep
								tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
								// node id
								tempbuf[1]=frame[3];
								tempbuf[2]=2;
								tempbuf[3]=COMMAND_CLASS_WAKE_UP;
								tempbuf[4]=WAKE_UP_NO_MORE_INFORMATION;
								tempbuf[5]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
								sendFunction( tempbuf , 6, REQUEST, 1); 
								// ...
							}//
						}
						break;
					case COMMAND_CLASS_SENSOR_BINARY:
						printf ("COMMAND_CLASS_SENSOR_BINARY - ");
						if (frame[6] == SENSOR_BINARY_REPORT) {
							printf("Got sensor report from node %i, level: %i\n",frame[3],frame[7]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							if ((unsigned char)frame[7] == 0xff) {
								tmp_zwave->SendSensorTrippedEvents (frame[3],true);
							} else {
								tmp_zwave->SendSensorTrippedEvents (frame[3],false);
							}

						}
						break;
					;;
					case COMMAND_CLASS_SWITCH_MULTILEVEL:
						printf("COMMAND_CLASS_SWITCH_MULTILEVEL - ");
						if (frame[6] == SWITCH_MULTILEVEL_REPORT) {
							printf("Got switch multilevel report from node %i, level: %i\n",frame[3],frame[7]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							tmp_zwave->SendLightChangedEvents (frame[3],(unsigned char)frame[7]);
						}
						break;
					case COMMAND_CLASS_SWITCH_ALL:
						printf("COMMAND_CLASS_SWITCH_ALL - ");
						if (frame[6] == SWITCH_ALL_ON) {
							printf("Got switch all ON from node %i\n",frame[3]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							tmp_zwave->SendLightChangedEvents (0,99);
						}
						if (frame[6] == SWITCH_ALL_OFF) {
							printf("Got switch all OFF from node %i\n",frame[3]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							tmp_zwave->SendLightChangedEvents (0,0);
						}
						break;
					case COMMAND_CLASS_ALARM:
						printf("COMMAND_CLASS_ALARM - ");
						if (frame[6] == ALARM_REPORT) {
							printf("Got ALARM from node %i, type: %i, level: %i\n",frame[3],frame[7],frame[8]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							tmp_zwave->SendSensorTrippedEvents (frame[3],true);
							tmp_zwave->SendSensorTrippedEvents (frame[3],false);
						}
						break;
					case COMMAND_CLASS_BASIC:
						printf("COMMAND_CLASS_BASIC - ");
						if (frame[6] == BASIC_REPORT) {
							// char tmp[32];
							// sprintf(&tmp,"%d",frame[7]);
							printf("Got basic report from node %i, value: %i\n",frame[3],(unsigned char)frame[7]);
							DCE::ZWave *tmp_zwave = static_cast<DCE::ZWave*>(myZWave);
							tmp_zwave->SendLightChangedEvents (frame[3],(unsigned char)frame[7]);
						}						
						break;
					case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
						printf("COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE: - ");
						if (frame[6] == SCHEDULE_GET) {
							printf("Got SCHEDULE_GET from node %i for day: %i\n",frame[3],frame[7]);
							// report no schedules for the given day
							tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
							// node id
							tempbuf[1]=frame[3];
							tempbuf[2]=5;
							tempbuf[3]=COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
							tempbuf[4]=SCHEDULE_SET;
							tempbuf[5]=frame[7];
							tempbuf[6]=0;
							tempbuf[7]=0x7f; // unused state, end setpoints
							tempbuf[8]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
							sendFunction( tempbuf , 9, REQUEST, 1); 

						}
						break;
					case COMMAND_CLASS_MULTI_CMD:
						printf("COMMAND_CLASS_MULTI_CMD - ");
						if (frame[6] == MULTI_CMD_ENCAP) {
							time_t timestamp;
							struct tm *timestruct;
							int offset = 0;
							// int cmd_length = 0;
							// int cmd_count = 0;

							printf("Got encapsulated multi command from node %i\n",frame[3]);
							timestamp = time(NULL);
							timestruct = localtime(&timestamp);
							printf("Time: %i %i %i\n",timestruct->tm_wday, timestruct->tm_hour, timestruct->tm_min);
							// iterate over commands
							offset = 8;	
							for (int i=0; i<frame[7]; i++) {
								printf("COMMAND LENGTH: %d, CLASS: %x\n",frame[offset],(unsigned char) frame[offset+1]);
								switch (frame[offset+1]) {
									case COMMAND_CLASS_BATTERY:
										if (BATTERY_REPORT == (unsigned char) frame[offset+2]) {
											printf("COMMAND_CLASS_BATTERY:BATTERY_REPORT: Battery level: %d\n",frame[offset+3]);
										}
										break;
									case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
										if (SCHEDULE_CHANGED_GET == (unsigned char) frame[offset+2]) {
											printf("COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:SCHEDULE_CHANGED_GET\n");
										}
										if (SCHEDULE_OVERRIDE_GET == (unsigned char) frame[offset+2]) {
											printf("COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:SCHEDULE_OVERRIDE_GET\n");
										}
										if (SCHEDULE_OVERRIDE_REPORT == (unsigned char) frame[offset+2]) {
											printf("COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:SCHEDULE_OVERRIDE_REPORT: Setback state: %d\n",frame[offset+4]);
										}
										break;
									case COMMAND_CLASS_CLOCK:
										if (CLOCK_GET == (unsigned char) frame[offset+2]) {
                                                                                        printf("COMMAND_CLASS_CLOCK:CLOCK_GET\n");
										}
										break;
									case COMMAND_CLASS_WAKE_UP:
										if (WAKE_UP_NOTIFICATION == (unsigned char) frame[offset+2]) {
                                                                                        printf("COMMAND_CLASS_WAKE_UP:WAKE_UP_NOTIFICATION\n");
										}	
										break;
									default:
									;;
								}
								offset += frame[offset]+1;
							}
							tempbuf[0]=FUNC_ID_ZW_SEND_DATA;
							// node id
							tempbuf[1]=frame[3];
							tempbuf[2]=15; // cmd_length;
							tempbuf[3]=COMMAND_CLASS_MULTI_CMD;
							tempbuf[4]=MULTI_CMD_ENCAP;
							tempbuf[5]=3; // cmd_count; 
							tempbuf[6]=4; //length of next command
							tempbuf[7]=COMMAND_CLASS_CLOCK;
							tempbuf[8]=CLOCK_REPORT;
							tempbuf[9]=((timestruct->tm_wday == 0 ? 7 : timestruct->tm_wday) << 5 ) | timestruct->tm_hour;
							tempbuf[10]=timestruct->tm_min == 60 ? 0 : timestruct->tm_min;
							tempbuf[11]=3; // length of next command
							tempbuf[12]=COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE;
							tempbuf[13]=SCHEDULE_CHANGED_REPORT;
							tempbuf[14]=17;
							tempbuf[15]=2; //length of next command
							tempbuf[16]=COMMAND_CLASS_WAKE_UP;
							tempbuf[17]=WAKE_UP_NO_MORE_INFORMATION;
							tempbuf[18]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
							sendFunction( tempbuf , 19, REQUEST, 1); 
 
						}
						break;
					default:
						printf("unhandled command class\n");
						break;
					;;


				}
				break;
			;;
			case FUNC_ID_ZW_APPLICATION_UPDATE:
				switch(frame[2]) {
					case UPDATE_STATE_NODE_INFO_RECEIVED:
						printf("FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_RECEIVED received from node %d - ",frame[3]);
						switch(frame[5]) {
							case BASIC_TYPE_SLAVE:
								printf("BASIC_TYPE_SLAVE:");
								switch(frame[6]) {
									case GENERIC_TYPE_SWITCH_MULTILEVEL:
										printf("GENERIC_TYPE_SWITCH_MULTILEVEL\n");
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_SWITCH_MULTILEVEL;
										tempbuf[4] = SWITCH_MULTILEVEL_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										sendFunction (tempbuf,6,REQUEST,1);
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_BASIC;
										tempbuf[4] = BASIC_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										sendFunction (tempbuf,6,REQUEST,1);
										break;
									case GENERIC_TYPE_SWITCH_BINARY:
										printf("GENERIC_TYPE_SWITCH_BINARY\n");
										sendFunction (tempbuf,6,REQUEST,1);
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_BASIC;
										tempbuf[4] = BASIC_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										sendFunction (tempbuf,6,REQUEST,1);
										break;
									default:
										printf("unhandled class\n");
										break;
									;;
								}
							default:
								break;
							;;

						}
						break;
					case UPDATE_STATE_NODE_INFO_REQ_FAILED:
						printf("FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_REQ_FAILED received\n");
						break;
					;;
					default:
						break;
					;;
				}	
				break;
			;;
			default:
			;;


		}

	} else {
		// should not happen
	}
}

void *ZWApi::ZWApi::receiveFunction() {
	printf("receiveFunction started\n");
	char retbuf[1];
	char mybuf2[1024];
	size_t len2;
	while (1) {
		// read a byte from the serial port
		mybuf2[0]=0;
		len2 = ReadSerialStringEx(serialPort,mybuf2, 1, 100);
		// len2 = serialPort->Read(mybuf2, 1, 100);
		if (len2 == 1) {
			switch(mybuf2[0]) {
				case SOF:
					// printf("SOF Found\n");
					// if we await an ack instead, trigger resend to be sure
					if (await_ack) {
						printf("Error: SOF Found while awaiting ack.. triggering resend\n");
						await_ack = 0;
					}
					// read the length byte
					len2 += ReadSerialStringEx(serialPort,mybuf2+1, 1, 100);
					// printf("Packet Length: %i\n",mybuf2[1]);

					// read the rest of the frame
					len2 += ReadSerialStringEx(serialPort,mybuf2+2,mybuf2[1],100);

					if (len2>0) printf("%s\n",DCE::IOUtils::FormatHexAsciiBuffer(mybuf2, len2,"33").c_str());
					// verify checksum
					if ((unsigned char) mybuf2[len2-1] == (unsigned char) checksum(mybuf2+1,mybuf2[1])) {
						// printf("Checksum correct - sending ACK\n");
						retbuf[0]=ACK;
						WriteSerialStringEx(serialPort,retbuf,1);
						decodeFrame(mybuf2+2,len2-3);
					} else {
						printf("Checksum incorrect %x - sending NAK\n",(unsigned char) checksum(mybuf2+1,mybuf2[1]));
						retbuf[0]=NAK;
						WriteSerialStringEx(serialPort,retbuf,1);

					}
					break;
				;;
				case CAN:
					printf("CAN Received\n");
						// trigger resend
						await_ack = 0;
					break;
				;;
				case NAK:
					printf("NAK Received\n");
						// trigger resend
						await_ack = 0;
					break;
				;;
				case ACK:
					printf("ACK Received\n");
					// if we await an ack pop the command, it got an ACK
					if (await_ack) {
						await_ack = 0;
						pthread_mutex_lock (&mutexSendQueue);
						ZWSendQueue.pop_front();
						pthread_mutex_unlock (&mutexSendQueue);
					}
					break;
				;;
				default:
					printf("ERROR! out of frame flow\n");
				;;
			}
		} else {
			pthread_mutex_lock (&mutexSendQueue);
			if (ZWSendQueue.size()>0 && await_ack != 1) {
				// printf("Elements on queue: %i\n",ZWSendQueue.size());
				// printf("Pointer: %p\n",ZWSendQueue.front());
				printf("%s\n",DCE::IOUtils::FormatHexAsciiBuffer(ZWSendQueue.front()->buffer, ZWSendQueue.front()->len,"31").c_str());

				WriteSerialStringEx(serialPort,ZWSendQueue.front()->buffer, ZWSendQueue.front()->len);
				await_ack = 1;
			}
			pthread_mutex_unlock (&mutexSendQueue);

		}
	}
}

bool ZWApi::ZWApi::sendFunction(char *buffer, size_t length, int type, bool response) {
	ZWJob *newJob;
	newJob = new ZWJob;
	int i, index;
	index = 0;
	i = 0;

	// printf("Adding job: %p\n",newJob);
	newJob->await_response = response && (type == REQUEST);
	newJob->sendcount = 0;

	newJob->buffer[index++] = SOF;
	newJob->buffer[index++] = length + 2 + (response ? 1 : 0);
	newJob->buffer[index++] = type == RESPONSE ? RESPONSE : REQUEST;
	for (i=0;i<length;i++) newJob->buffer[index++] = buffer[i];
	newJob->len = length + 4 + (response ? 1 : 0);
	pthread_mutex_lock (&mutexSendQueue);
	if (response) {
		newJob->buffer[index++] = callbackpool;
	}
	newJob->callbackid = callbackpool++;

	newJob->buffer[index] = checksum(newJob->buffer+1,length+2+( response ? 1 : 0) );
	ZWSendQueue.push_back(newJob);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;
}

bool ZWApi::ZWApi::addIntent(int nodeid, int type) {
	ZWIntent *newIntent;
	newIntent = new ZWIntent;

	newIntent->nodeid = nodeid;
	newIntent->type = type;

	pthread_mutex_lock (&mutexSendQueue);
	ZWIntentQueue.push_back(newIntent);
	pthread_mutex_unlock (&mutexSendQueue);
	return true;

}

int ZWApi::ZWApi::getIntent(int type) {
	int nodeid = -1;

	if (ZWIntentQueue.size()>0) {
		// printf("Elements on queue: %i\n",ZWSendQueue.size());
		// printf("Pointer: %p\n",ZWSendQueue.front());
		nodeid = ZWIntentQueue.front()->nodeid;
		pthread_mutex_lock (&mutexSendQueue);
		ZWIntentQueue.pop_front();
		pthread_mutex_unlock (&mutexSendQueue);
	}
	return nodeid;
}

std::string ZWApi::ZWApi::getDeviceList() {
	return deviceList;
}

bool ZWApi::ZWApi::zwBasicSet(int node_id, int level) {
	char mybuf[1024];

        mybuf[0] = FUNC_ID_ZW_SEND_DATA;
	mybuf[1] = node_id;
	mybuf[2] = 3;
	mybuf[3] = COMMAND_CLASS_BASIC;
	mybuf[4] = BASIC_SET;
	mybuf[5] = level;
	mybuf[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        sendFunction( mybuf , 7, REQUEST, 1);


}

