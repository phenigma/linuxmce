#include "ZWJobGetSUC.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "SerialConnection.h"

#include <stdio.h>

class ZWJobGetSUC::Private
{
	public:
	
		Private();
		~Private();
	
	private:
};

ZWJobGetSUC::Private::Private()
{
}

ZWJobGetSUC::Private::~Private()
{
}

// ----------------------------------

ZWJobGetSUC::ZWJobGetSUC(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::GET_SUC);
}

ZWJobGetSUC::~ZWJobGetSUC()
{
	delete d;
	d = NULL;
}

bool ZWJobGetSUC::run()
{
	// send	FUNC_ID_MEMORY_GET_ID
	char buffer[10];
	
	// enable SUC
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_ENABLE_SUC;
	buffer[2] = 1;
	buffer[3] = 0;
	
	handler()->sendData(buffer, 3);
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_GET_SUC_NODE_ID;
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

bool ZWJobGetSUC::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobGetSUC");

	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobGetSUC wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 3 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_GET_SUC_NODE_ID )
			{
				handler()->setSucID(buffer[2]);
				
				setAnswerTime( time(NULL) );
				setState(ZWaveJob::STOPPED);
				
				return true;
			}
			else
			{
				g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobGetSUC data error");
				return handler()->processData(buffer, length);
			}
			break;
	}
	
	return false;
}
