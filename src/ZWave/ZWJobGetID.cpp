
#include "ZWJobGetID.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "SerialConnection.h"

#include <stdio.h>
#include <string.h>

union String2ULong
{
	char buffer[4];
	unsigned long number;
};

// ----------------------------------

class ZWJobGetID::Private
{
	public:
	
		Private();
		~Private();
	
	private:
};

ZWJobGetID::Private::Private()
{
}

ZWJobGetID::Private::~Private()
{
}

// ----------------------------------

ZWJobGetID::ZWJobGetID(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::GET_ID);
}

ZWJobGetID::~ZWJobGetID()
{
	delete d;
	d = NULL;
}

bool ZWJobGetID::run()
{
	// send	FUNC_ID_MEMORY_GET_ID
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_MEMORY_GET_ID;
	buffer[2] = 0;
	
	time_t currentTime = time(NULL);
	if( !again() )
	{
		setStartTime( currentTime );
	}
	setAnswerTime( currentTime );
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 2);
}

bool ZWJobGetID::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobGetID");
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobGetID wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 7 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_MEMORY_GET_ID )
			{
				union String2ULong s2ul;
				
				memcpy(s2ul.buffer, &buffer[2], 4);
				
				handler()->setHomeID(s2ul.number);
				handler()->setNodeID(buffer[6]);
				
				setAnswerTime( time(NULL) );
				setState(ZWaveJob::STOPPED);
				
				return true;
			}
			else
			{
				g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobGetID data error");
				return handler()->processData(buffer, length);
			}
			break;
	}
	
	return false;
}
