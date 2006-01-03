#include "ZWJobGetSUC.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"

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
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_GET_SUC_NODE_ID;
	buffer[2] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 2);
}

bool ZWJobGetSUC::processData(const char * buffer, size_t length)
{
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetSUC wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 3 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_GET_SUC_NODE_ID )
			{
				handler()->setSucID(buffer[2]);
				
				setState(ZWaveJob::STOPPED);
				
				return true;
			}
			else
			{
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetSUC error: %*.*s", length, length, buffer);
			}
			break;
	}
	
	return false;
}
