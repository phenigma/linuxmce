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
}

DevicePoll::~DevicePoll() {
}

int 
DevicePoll::SendRequest(const Message* pMesg) {
	int ret = 0; // ok
	
	mq.Lock();
	if(msgqueue.size() < MAX_QUEUE_SIZE) {
		msgqueue.push_back(*pMesg);
		ret = -1;
	}
	mq.Unlock();
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
					resp = -1;
	
	g_pPlutoLogger->Write(LV_STATUS, "Sending header with Checksum: %x.", chksum);
	pport->Write((char*)sendbuff, sizeof(sendbuff) / sizeof(unsigned char));
		
	if(pport->Read((char*)&resp, 1, CM11A_READ_TIMEOUT) == 0) {
		g_pPlutoLogger->Write(LV_STATUS, "No response from CM11A device.");
		return -1;
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Got response: %x from CM11A.", resp);
		
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

	unsigned char dim = pMesg->getDimmLevel() * CM11A_MAX_DIM_LEVEL * 1.0 / 100;
	if(dim > 0) {
		g_pPlutoLogger->Write(LV_STATUS, "Using Dim Level: %d.", dim);
	}
	
	return SendPacket(pport,
						/*high*/(dim << 3) | CM11A_STANDARD_FUNCTION, 
						/*low*/pMesg->getHouseCode() << 4 | pMesg->getFunctionCode());
	
}

void* DevicePoll::_Run() {
	g_pPlutoLogger->Write(LV_STATUS, "Device Poll thread started.");

	while(true) {// will be changed with thread end request
		try {
			CSerialPort serprt(CM11A_PORT, CM11A_BPS, epbsN81);

			// get the msg from queue
			mq.Lock();
			if(msgqueue.size() > 0) {
				Message msg = msgqueue.front();
				msgqueue.pop_front();
				mq.Unlock();

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
				mq.Unlock();
				Sleep(CM11A_NOREQUES_SLEEP);
			}
		
		} catch(...) {
			g_pPlutoLogger->Write(LV_CRITICAL, "Exception occured on CM11A's serial port. Sleeping for %0.1f second.", 
													1.0 * CM11A_ERROR_SLEEP / 1000);
			Sleep(CM11A_ERROR_SLEEP);
		}
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "Device Poll thread ending.");
	return 0;
}

};
