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
#include "thread.h"
#include "mutex.h"
#include "message.h"
#include "Serial/SerialPort.h"

namespace CM11ADEV {

/**
@author igor
*/
class DevicePoll : public Thread 
{
public:
    DevicePoll();
    ~DevicePoll();
	
public:
	int SendRequest(const Message* pMesg);

protected:
	virtual void* _Run();
	
	int SendPacket(CSerialPort* pport, 
			unsigned char highByte, unsigned char lowByte);
	int SendClock(CSerialPort* pport);
	
	virtual int SendAddress(CSerialPort* pport, const Message* pMesg);
	virtual int SendFunction(CSerialPort* pport, const Message* pMesg);

protected:
	Mutex mq;
	std::list<Message> msgqueue;
};

};

#endif
