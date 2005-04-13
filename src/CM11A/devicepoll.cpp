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


using namespace DCE;

#define CM11A_PORT					"ttyS0"
#define CM11A_BPS					4800

#define CM11A_ERROR_SLEEP			5100
#define CM11A_NOREQUES_SLEEP		100 

#define CM11A_SEND_RETRY			10 
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
		
	if(pport->Read((char*)&resp, 1, CM11A_READ_TIMEOUT) == 0) {
		g_pPlutoLogger->Write(LV_STATUS, "No response from CM11A device.");
		return -1;
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Got response: %x from CM11A.", resp);
		if(resp == CM11A_CLOCK_REQ) {
			g_pPlutoLogger->Write(LV_STATUS, "CM11A requested clock SET UP", resp);
			SendClock(pport);
			return -1;
		} else
		if(resp == CM11A_INTERFACE_CQ) {
			g_pPlutoLogger->Write(LV_STATUS, "CM11A wants to notify us of Something...");
			resp = CM11A_COMPUTER_READY;
			pport->Write((char*)&resp, 1);
			int nread = 0;
			do {
				nread = pport->Read((char*)&resp, 1, CM11A_READ_TIMEOUT);
			} while (nread > 0 && resp == CM11A_INTERFACE_CQ);
			if(nread > 0) {
				if(resp > 0 && resp < 128) {
					g_pPlutoLogger->Write(LV_STATUS, "Reading %d bytes of DATA...");
					
					char *buff = new char[resp];
					pport->Read(buff, resp, CM11A_READ_TIMEOUT);
					delete buff;
					g_pPlutoLogger->Write(LV_STATUS, "Data read successfully...");
				} else {
					g_pPlutoLogger->Write(LV_STATUS, "No data to read...");
				}
			} else {
				g_pPlutoLogger->Write(LV_STATUS, "Fake Alarm!");
			}
			return -1;
		} else
		if(resp != chksum) {
			g_pPlutoLogger->Write(LV_CRITICAL, "Bad checksum received (send:%x, recieved:%x).", chksum, resp);
			return -1;
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
