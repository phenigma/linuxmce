#include "ZWJobGetNodeProtocolInfo.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

class ZWJobGetNodeProtocolInfo::Private
{
	public:
	
		Private();
		~Private();
	
	private:
};

ZWJobGetNodeProtocolInfo::Private::Private()
{
}

ZWJobGetNodeProtocolInfo::Private::~Private()
{
}

// ----------------------------------

ZWJobGetNodeProtocolInfo::ZWJobGetNodeProtocolInfo(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::GET_ID);
}

ZWJobGetNodeProtocolInfo::~ZWJobGetNodeProtocolInfo()
{
	delete d;
	d = NULL;
}

bool ZWJobGetNodeProtocolInfo::run()
{
	// send	FUNC_ID_MEMORY_GET_ID
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_MEMORY_GET_ID;
	buffer[2] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 2);
}

bool ZWJobGetNodeProtocolInfo::processData(const char * buffer, size_t length)
{
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetNodeProtocolInfo wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 7 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_MEMORY_GET_ID )
			{
				setState(ZWaveJob::STOPPED);
				
				return true;
			}
			else
			{
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetNodeProtocolInfo error: %*.*s", length, length, buffer);
			}
			break;
	}
	
	return false;
}
