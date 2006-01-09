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
	
	// ZWave implementation is doing this
	// but ThinkControl not
	// What to do ?
#if 0
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION;
	buffer[2] = 1; // true == listening
	buffer[3] = GENERIC_TYPE_STATIC_CONTROLLER;
	buffer[4] = SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER;
	buffer[5] = 1; // one param
	buffer[6] = COMMAND_CLASS_CONTROLLER_REPLICATION; // param
	buffer[7] = 0;
	
	handler()->sendData(buffer, 7);
#endif
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_NEW_CONTROLLER;
	buffer[2] = NEW_CTRL_STATE_RECEIVE;
	buffer[3] = 1; // Callback FunctionID
	buffer[4] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 4);
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
			
		case ZWaveJob::RUNNING : // ??? May be
			if( length >= 3 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_NEW_CONTROLLER &&
				buffer[2] == 1 )
			{
				setState( ZWaveJob::STOPPED );
				return true;
			}
			break;
	}
	
	return false;
}
