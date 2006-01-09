#include "ZWJobReceive.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobReceive::Private
{
	public:
	
		Private();
		~Private();
	
	private:
};

ZWJobReceive::Private::Private()
{
}

ZWJobReceive::Private::~Private()
{
}

// ----------------------------------

ZWJobReceive::ZWJobReceive(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::RECEIVE);
}

ZWJobReceive::~ZWJobReceive()
{
	delete d;
	d = NULL;
}

bool ZWJobReceive::run()
{
	// send	FUNC_ID_MEMORY_GET_ID
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_NEW_CONTROLLER;
	buffer[2] = 0;
	buffer[3] = 0;
	buffer[4] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 2);
}

bool ZWJobReceive::processData(const char * buffer, size_t length)
{
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobReceive wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			break;
	}
	
	return false;
}
