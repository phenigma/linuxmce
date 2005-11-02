//
// C++ Implementation: devicepoll
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "devicepoll.h"

#include <time.h>

#include "DCE/Logger.h"
#include "cm11aconsts.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Message.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceTemplate.h"
using namespace DCE;

#define CM11A_PORT					"ttyS0"
#define CM11A_BPS					4800

#define CM11A_ERROR_SLEEP			5100
#define CM11A_NOREQUES_SLEEP		100 

#define CM11A_SEND_RETRY			5
#define CM11A_READ_TIMEOUT			15000 

#define CM11A_MAX_DIM_LEVEL			22

#define MAX_QUEUE_SIZE	1000

namespace CM11ADEV {

DevicePoll::DevicePoll() {
	serport_ = CM11A_PORT;
}

DevicePoll::~DevicePoll() {
}

int 
DevicePoll::SendRequest(const Message* pMesg) {
	int ret = 0; // ok
	
	mq_.Lock();
	if(msgqueue_.size() < MAX_QUEUE_SIZE) {
		msgqueue_.push_back(*pMesg);
		ret = -1;
	}
	mq_.Unlock();
	return 0;
}

int 
DevicePoll::SendPacket(CSerialPort* pport, 
			unsigned char highByte, unsigned char lowByte)
{
	g_pPlutoLogger->Write(LV_STATUS, "Sending packet with HighByte=%x, LowByte=%x.", 
										highByte, lowByte);
										
	unsigned char sendbuff[2] =  { highByte, lowByte };
	
	unsigned char chksum = (sendbuff[0] + sendbuff[1]) & 0xff, 
					resp = (unsigned char)-1;
	
	g_pPlutoLogger->Write(LV_STATUS, "Sending header with Checksum: %x.", chksum);
	pport->Write((char*)sendbuff, sizeof(sendbuff) / sizeof(unsigned char));

	if(pport->Read((char*)&resp, 1, CM11A_READ_TIMEOUT) == 0)
	{
		g_pPlutoLogger->Write(LV_STATUS, "No response from CM11A device.");
		return -1;
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "Got response: %x from CM11A.", resp);
		if(resp == CM11A_CLOCK_REQ)
		{
			g_pPlutoLogger->Write(LV_STATUS, "CM11A requested clock SET UP", resp);
			SendClock(pport);
			return -1;
		}
		else
		{ 
			if(resp != chksum)
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "Bad checksum received (send:%x, recieved:%x).", chksum, resp);
				return -1;
			}
		}
	}
	// perform ACK handshake
	g_pPlutoLogger->Write(LV_STATUS, "Sending ACK.");
	
	sendbuff[0] = CM11A_ACK;
	pport->Write((char*)sendbuff, 1);
	if(pport->Read((char*)&resp, 1, CM11A_READ_TIMEOUT) == 0) {
		g_pPlutoLogger->Write(LV_CRITICAL, "No response from CM11A device.");
		return -1;
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Got response: %x from CM11A.", resp);
	
		if(resp != CM11A_INTERFACE_READY) {
			g_pPlutoLogger->Write(LV_STATUS, "Acknowledge of adress failed.");
			return -1;
		}
	}
	
	return 0;
}			

int 
DevicePoll::SendClock(CSerialPort* pport) {
	unsigned char sbuff[9];
	time_t ttime = time((long *) NULL);
	
	struct tm *tp;
	tp = localtime(&ttime);

	sbuff[0] = CM11A_SET_TIME;             /* CM11A timer download code */
	sbuff[1] = tp->tm_sec ;
	sbuff[2] = tp->tm_min + (((tp->tm_hour) %2) * 60 ) ;  /* minutes 0 - 119 */
	sbuff[3] = tp->tm_hour / 2 ;                 /* hour / 2         0 - 11 */
	sbuff[4] = tp->tm_yday % 256 ;               /* mantisa of julian date */
	sbuff[5] = ((tp->tm_yday / 256 ) << 7);      /* radix of julian date */
	sbuff[5] |= (0x01 << (tp->tm_wday)); /* bits 0-6 =  single bit mask day */
								/* of week ( smtwtfs ) */
	sbuff[6] = 0x61; /* hard coded house code A for monitoring, probably shouldn't be hard coded */
	pport->Write((char*)sbuff, 7);
	pport->Read((char*)sbuff, 1, 10000);
	return true;
}

int 
DevicePoll::SendAddress(CSerialPort* pport, const Message* pMesg) {
	g_pPlutoLogger->Write(LV_STATUS, "Sending address with HouseCode=%d, DeviceCode=%d.", 
										pMesg->getHouseCode(), pMesg->getDeviceCode());
	
	return SendPacket(pport,
						/*high*/CM11A_STANDARD_ADDRESS, /*low*/pMesg->getHouseCode() << 4 | pMesg->getDeviceCode());
}

int
DevicePoll::SendFunction(CSerialPort* pport, const Message* pMesg) {
	g_pPlutoLogger->Write(LV_STATUS, "Sending function with Code: %d.", 
												pMesg->getFunctionCode());

	unsigned char dim = (unsigned char)(pMesg->getDimmLevel() * CM11A_MAX_DIM_LEVEL * 1.0 / 100);
	if(dim > 0) {
		g_pPlutoLogger->Write(LV_STATUS, "Using Dim Level: %d.", dim);
	}
	
	return SendPacket(pport,
						/*high*/(dim << 3) | CM11A_STANDARD_FUNCTION, 
						/*low*/pMesg->getHouseCode() << 4 | pMesg->getFunctionCode());
	
}

void* DevicePoll::_Run() {

	g_pPlutoLogger->Write(LV_STATUS, "Device Poll thread started.");

	VectDeviceData_Impl kids = real_cm11a->m_pData->m_vectDeviceData_Impl_Children;

	for(unsigned int kn=0; kn< kids.size(); kn ++)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Child device: #%d(%s)   Category:%d",kids[kn]->m_dwPK_Device,kids[kn]->m_mapParameters[12].c_str(),kids[kn]->m_dwPK_DeviceCategory);
		inverse_device_map[kids[kn]->m_mapParameters[12]]=kn;
	}
	

	while(!isStopRequested()) {// will be changed with thread end request
		try {
			CSerialPort serprt(serport_.c_str(), CM11A_BPS, epbsN81);

			// get the msg from queue
			mq_.Lock();
			if(msgqueue_.size() > 0) {
				Message msg = msgqueue_.front();
				msgqueue_.pop_front();
				mq_.Unlock();

				int sendretry = 0;
				for(; sendretry < CM11A_SEND_RETRY; sendretry++) {
					if(!SendAddress(&serprt, &msg)) {
						break;
					}
					Sleep(100);
				}
				
				if(sendretry < CM11A_SEND_RETRY) { // adress sent seccessfully
					g_pPlutoLogger->Write(LV_STATUS, "Address sent successfully.");
				} else  {
					g_pPlutoLogger->Write(LV_CRITICAL, "Failed sending address.");
					continue;
				}
				
				sendretry = 0;
				for(; sendretry < CM11A_SEND_RETRY; sendretry++) {
					if(!SendFunction(&serprt, &msg)) {
						break;
					}
					Sleep(100);
				}
				
				if(sendretry < CM11A_SEND_RETRY) { // adress sent seccessfully
					g_pPlutoLogger->Write(LV_STATUS, "Function sent successfully.");
				} else  {
					g_pPlutoLogger->Write(LV_CRITICAL, "Failed sending function.");
				}
			} else {
				mq_.Unlock();
				
				if (!serprt.IsReadEmpty()) {
					unsigned char resp;
					if(serprt.Read((char*)&resp, 1, CM11A_READ_TIMEOUT) != 0)
					{
						g_pPlutoLogger->Write(LV_STATUS, "Got response: %x from CM11A.", resp);
						if(resp == CM11A_INTERFACE_CQ) {
							resp = CM11A_COMPUTER_READY;
							serprt.Write((char*)&resp, 1);
							int nread = 0;
							do {
								nread = serprt.Read((char*)&resp, 1, CM11A_READ_TIMEOUT);
							} while (nread > 0 && resp == CM11A_INTERFACE_CQ);
							if(nread > 0) {
								if(resp > 0 && resp < 128) {
									g_pPlutoLogger->Write(LV_STATUS, "Reading %d bytes of DATA...",resp);

									char *buff = new char[resp];
									serprt.Read(buff, resp, CM11A_READ_TIMEOUT);
									char mask=buff[0];
									string recv_msg="";
									char hex_buff[128];
									hex_buff[0]=0;
									list<std::string> used_addresses;
									int i;
									for (i=0;i<resp;i++) {
										sprintf(hex_buff+strlen(hex_buff),"%02X ",buff[i] & 0xFF);
									}
									g_pPlutoLogger->Write(LV_STATUS, "Data read successfully <%s>",hex_buff);
									for (i=1;i<resp;i++) {
										if ((mask & (1 << (i-1))) == 0)
										{ //an address
											char* addr = Message::getAddress(buff[i]);
											used_addresses.push_back(addr);
											if(recv_msg.length()>0)
											{
												recv_msg+=",";
											}
											recv_msg+=addr;
										}
										else 
										{ //a function
											char func=buff[i] & 0x0F;
											char data=0;
											std::string message= Message::getHouseCode(buff[i]);
											switch(func) {
												case CM11A_FUNC_ALL_U_OFF : message+=" ALL UNITS OFF"; break;
												case CM11A_FUNC_ALL_L_ON  : message+=" ALL LIGHTS ON"; break;
												case CM11A_FUNC_ON        : message+=" ON"; break;
												case CM11A_FUNC_OFF       : message+=" OFF"; break;
												case CM11A_FUNC_DIM       : message+=" DIM"; data=(buff[i]*100)/210; i++; break;
												case CM11A_FUNC_BRIGHT    : message+=" BRIGHT"; data=(buff[i]*100)/210; i++; break;
												case CM11A_FUNC_ALL_L_OFF : message+=" ALL LIGHTS OFF"; break;
												case CM11A_FUNC_EXT       : message+=" EXTENDED"; break;
												case CM11A_FUNC_HREQ      : message+=" HAIL REQUEST"; break;
												case CM11A_FUNC_HACK      : message+=" HAIL ACK"; break;
												case CM11A_FUNC_PSD1      : message+=" PRESET DIM 1"; break;
												case CM11A_FUNC_PSD2      : message+=" PRESET DIM 2"; break;
												case CM11A_FUNC_EXTDT     : message+=" EXTENDED DATA"; break;
												case CM11A_FUNC_STATON    : message+=" STATUS ON"; break;
												case CM11A_FUNC_STATOFF   : message+=" STATUS OFF"; break;
												case CM11A_FUNC_STATREQ   : message+=" STATUS REQUEST"; break;
											}
											for(std::list<std::string>::iterator it = used_addresses.begin(); it != used_addresses.end(); it++)
											{
												g_pPlutoLogger->Write(LV_STATUS, "Looking for device status for %s found %d", (*it).c_str(), device_status.find(*it) != device_status.end() ? 1 : 0);
												if (device_status.find(*it) != device_status.end()) {
													switch(func) {
														case CM11A_FUNC_ON      : device_status[*it]=100; break;
														case CM11A_FUNC_OFF     : device_status[*it]=0; break;
														case CM11A_FUNC_DIM     : device_status[*it]+=data; break;
														case CM11A_FUNC_BRIGHT  : device_status[*it]-=data; break;
													}
												} else {
													g_pPlutoLogger->Write(LV_STATUS, "Probably need to send EVENT that %s is %s",(*it).c_str(),message.c_str());
													DeviceData_Impl *child = kids[inverse_device_map[*it]];
													if(child->m_dwPK_DeviceCategory == DEVICECATEGORY_Security_Device_CONST)
													{
														DCE::Message *pMessageOut=new DCE::Message(child->m_dwPK_Device,DEVICETEMPLATE_VirtDev_Security_Plugin_CONST,PRIORITY_NORMAL,MESSAGETYPE_EVENT,EVENT_Sensor_Tripped_CONST,0);
														pMessageOut->m_mapParameters[EVENTPARAMETER_Tripped_CONST]=(func==CM11A_FUNC_ON?"1":"0");
														real_cm11a->m_pEvent->SendMessage(pMessageOut);
													}
												}
											}

											if(recv_msg.length()>0) {
											recv_msg+=" ";
											}
											recv_msg+=message;
										}
									}
									delete[] buff;
									g_pPlutoLogger->Write(LV_STATUS, "Data decoded into [%s]",recv_msg.c_str());
								}
								else {
									g_pPlutoLogger->Write(LV_STATUS, "No data to read...");
								}
							} else {
								g_pPlutoLogger->Write(LV_STATUS, "Fake Alarm!");
							}
						}
					}
				}
				
				Sleep(CM11A_NOREQUES_SLEEP);
			}
		
			      
		} catch(...) {
			g_pPlutoLogger->Write(LV_CRITICAL, "Exception occured on CM11A's serial port: %s. Sleeping for %0.1f second.", 
													serport_.c_str(), 1.0 * CM11A_ERROR_SLEEP / 1000);
			Sleep(CM11A_ERROR_SLEEP);
		}
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "Device Poll thread ending.");
	return 0;
}

};
