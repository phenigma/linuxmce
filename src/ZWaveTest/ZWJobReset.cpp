
#include "ZWJobReset.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "ZW_classcmd.h"

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
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_SERIAL_API_APPL_NODE_INFORMATION;
	buffer[2] = 1; // true == listening
	buffer[3] = GENERIC_TYPE_STATIC_CONTROLLER;
	buffer[4] = SPECIFIC_TYPE_PORTABLE_REMOTE_CONTROLLER;
	buffer[5] = 1; // one param
	buffer[6] = COMMAND_CLASS_CONTROLLER_REPLICATION; // param
	buffer[7] = 0;
	
	handler()->sendData(buffer, 7);
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_SET_DEFAULT;
	buffer[2] = 1; // Callback FunctionID
	buffer[3] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 3);
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
			if( length >= 3 && 
				buffer[0] == REQUEST &&
				buffer[1] == FUNC_ID_ZW_SET_DEFAULT &&
				buffer[2] == 1 )
			{
				setState( ZWaveJob::STOPPED );
				return true;
			}
			break;
	}
	
	return false;
}
