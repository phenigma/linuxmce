#include "ZWJobReceive.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "SerialConnection.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobReceive::Private
{
	public:
	
		Private();
		~Private();
		
		ZWJobReceive::ReceivingState state;
	
	private:
};

ZWJobReceive::Private::Private()
	: state(ZWJobReceive::STOP)
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
	
	d->state = ZWJobReceive::START;
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 4);
}

bool ZWJobReceive::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobReceive");

	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobReceive wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 6 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_NEW_CONTROLLER &&
				buffer[2] == 1 )
			{
				if( ZWJobReceive::STOP == d->state )
				{
					setState( ZWaveJob::STOPPED );
					// TODO as in ZWave sources dec_ctrl.c
					// check if the sender controller is SUC
					// if it is, then set it as your SUC
				}
				else
				{
					// buffer[3]... buffer[6] etc = LEARN_NODE
					// Node Status
					switch( buffer[3] )
					{
						case NEW_CONTROLLER_LEARNED:
							// what to do here ?
							d->state = ZWJobReceive::RECEIVING;
							break;
						
						case NEW_CONTROLLER_CHANGE_DONE :
						case NEW_CONTROLLER_DONE :
						{
							// the receiving it's done, change the controller state
							// back to normal state (stop == not receiving)
							char buf[10];
							buf[0] = REQUEST;
							buf[1] = FUNC_ID_ZW_NEW_CONTROLLER;
							buf[2] = NEW_CTRL_STATE_STOP_OK;
							buf[3] = 1; // Callback FunctionID
							buf[4] = 0;
							
							d->state = ZWJobReceive::STOP;
							handler()->sendData(buf, 4);
							break;
						}
						
						case NEW_CONTROLLER_FAILED :
						default:
							setState( ZWaveJob::STOPPED );
							break;
					}
				}
				
				return true;
			}
			break;
	}
	
	return false;
}
