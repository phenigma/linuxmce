#include "ZWJobReceive.h"
#include "ZW_SerialAPI.h"
#include "ZW_classcmd.h"
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
		unsigned char callbackID;
	
	private:
};

ZWJobReceive::Private::Private()
	: state(ZWJobReceive::STOP),
	  callbackID(0)
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
	// What to do ? To send 00 03 01 02 01 01 21 ?
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
	d->callbackID = handler()->callbackCount();
	buffer[3] = d->callbackID; // Callback FunctionID
	buffer[4] = 0;
	
	time_t currentTime = time(NULL);
	if( !again() )
	{
		setStartTime( currentTime );
	}
	setAnswerTime( currentTime );
	
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
			g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobReceive wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 6 && 
				buffer[0] == REQUEST &&
				buffer[1] == FUNC_ID_ZW_NEW_CONTROLLER &&
				buffer[2] == d->callbackID )
			{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 1");
#endif
				if( ZWJobReceive::STOP == d->state )
				{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 2");
#endif
					setState( ZWaveJob::STOPPED );
					
					// TODO : well, it's stopped but was it a good receiving ?? yes/not
					
					// TODO as in ZWave sources dec_ctrl.c
					// check if the sender controller is SUC
					// if it is, then set it as your SUC
				}
				else
				{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 3");
#endif
					// buffer[3]... buffer[6] etc = LEARN_NODE
					// Node Status
					switch( buffer[3] )
					{
						case NEW_CONTROLLER_LEARNED:
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 4");
#endif
							// what to do here ?
							d->state = ZWJobReceive::RECEIVING;
							break;
						
						case NEW_CONTROLLER_CHANGE_DONE :
						case NEW_CONTROLLER_DONE :
						{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 5");
#endif
							// the receiving it's done, change the controller state
							// back to normal state (stop == not receiving)
							char buf[10];
							buf[0] = REQUEST;
							buf[1] = FUNC_ID_ZW_NEW_CONTROLLER;
							buf[2] = NEW_CTRL_STATE_STOP_OK;
							d->callbackID = handler()->callbackCount();
							buf[3] = d->callbackID; // Callback FunctionID
							buf[4] = 0;
							
							d->state = ZWJobReceive::STOP;
							
							if( handler()->sendData(buf, 4) )
							{
								setAnswerTime( time(NULL) );
								// let's force to get the answer in time
								setReceivingTimeout(4);
								return true;
							}
							
							break;
						}
						
						case NEW_CONTROLLER_FAILED :
						default:
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 6");
#endif
							setState( ZWaveJob::STOPPED );
							break;
					}
				}
				
				setAnswerTime( time(NULL) );
				return true;
			}
			else if( length >= 6 &&
					 buffer[0] == REQUEST &&
					 buffer[1] == FUNC_ID_APPLICATION_COMMAND_HANDLER &&
					 buffer[2] == NEW_CONTROLLER_LEARNED &&
					 (unsigned char)buffer[3] == 239 && // hardcoded in protocol too
					 buffer[5] == COMMAND_CLASS_CONTROLLER_REPLICATION )
			{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 7");
#endif
				// the receiving it's done, change the controller state
				// back to normal state (stop == not receiving)
				char buf[10];
				buf[0] = REQUEST;
				buf[1] = FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
				buf[2] = 0;
				
				d->state = ZWJobReceive::STOP;
				if( handler()->sendData(buf, 2) )
				{
					setAnswerTime( time(NULL) );
					return true;
				}
			}
			else
			{
				return handler()->processData(buffer, length);
			}
			break;
	}
	
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 8");
#endif
	return false;
}

void ZWJobReceive::timeoutHandler()
{
#ifdef PLUTO_DEBUG
	g_pPlutoLogger->Write(LV_DEBUG, "----- RECEIVE ---- 5");
#endif

	setAnswerTime( time(NULL) );
	
	// TRY AGAIN :(
	// the receiving it's done, change the controller state
	// back to normal state (stop == not receiving)
	char buf[10];
	buf[0] = REQUEST;
	buf[1] = FUNC_ID_ZW_NEW_CONTROLLER;
	buf[2] = NEW_CTRL_STATE_STOP_OK;
//	d->callbackID = handler()->callbackCount();
	buf[3] = d->callbackID; // Callback FunctionID
	buf[4] = 0;
	
	d->state = ZWJobReceive::STOP;
	
	if( !handler()->sendData(buf, 4) )
	{
		d->state = ZWJobReceive::STOP;
	}
}
