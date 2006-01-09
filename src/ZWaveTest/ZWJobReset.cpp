
#include "ZWJobReset.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobReset::Private
{
	public:
	
		Private();
		~Private();
	
	private:
};

ZWJobReset::Private::Private()
{
}

ZWJobReset::Private::~Private()
{
}

// ----------------------------------

ZWJobReset::ZWJobReset(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::RESET);
}

ZWJobReset::~ZWJobReset()
{
	delete d;
	d = NULL;
}

bool ZWJobReset::run()
{
	// send	FUNC_ID_MEMORY_GET_ID
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION;
	buffer[2] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 2);
}

bool ZWJobReset::processData(const char * buffer, size_t length)
{
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobReset wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			break;
	}
	
	return false;
}
