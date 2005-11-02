//
// C++ Interface: devicepoll
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CM11DEVICEPOLL_H
#define CM11DEVICEPOLL_H

#include <list>
#include <map>
#include "thread.h"
#include "mutex.h"
#include "message.h"
#include "Serial/SerialPort.h"
#include "Gen_Devices/CM11ABase.h"

namespace CM11ADEV {

/**
@author igor
*/
class DevicePoll : public Thread 
{
public:
    DevicePoll();
    ~DevicePoll();

	inline void setSerialPort(const char* serport) {
		serport_ = serport;
	}
	inline const char* getSerialPort() {
		return serport_.c_str();
	}
	inline void setCM11A(DCE::CM11A_Command *cm11a) {
		real_cm11a = cm11a;
	}
	
public:
	int SendRequest(const Message* pMesg);
	std::map <std::string, int> device_status;
	
protected:
	virtual void* _Run();
	
	int SendPacket(CSerialPort* pport, 
			unsigned char highByte, unsigned char lowByte);
	int SendClock(CSerialPort* pport);
	
	virtual int SendAddress(CSerialPort* pport, const Message* pMesg);
	virtual int SendFunction(CSerialPort* pport, const Message* pMesg);

private:
	Mutex mq_;
	std::list<Message> msgqueue_;
	std::string serport_;
	DCE::CM11A_Command *real_cm11a;
	std::map <std::string, int> inverse_device_map;
};

};

#endif
