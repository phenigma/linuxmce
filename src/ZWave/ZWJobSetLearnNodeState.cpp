//
// C++ Implementation: ZWJobSetLearnNodeState
//
// Description: ZWave Set Learn Node State Command
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobSetLearnNodeState.h"

#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "ZW_classcmd.h"
#include "SerialConnection.h"

#include <stdio.h>
#include <string.h>

class ZWJobSetLearnNodeState::Private
{
	public:
	
		Private();
		
		~Private();
		
		unsigned char mode;
		char learnInfo[LEARN_INFO_MAX];
		size_t learnInfolength;
		bool failed;
	
	private:
};

ZWJobSetLearnNodeState::Private::Private()
	: mode(0),
	  learnInfolength(0),
	  failed(false)
{
	memset(learnInfo, 0, sizeof(learnInfo));
}

ZWJobSetLearnNodeState::Private::~Private()
{
}

ZWJobSetLearnNodeState::ZWJobSetLearnNodeState(PlutoZWSerialAPI* zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::INITIALIZE);
}

ZWJobSetLearnNodeState::~ZWJobSetLearnNodeState()
{
	delete d;
	d = NULL;
}

bool ZWJobSetLearnNodeState::processData(const char* buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobSetLearnNodeState");

	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobSetLearnNodeState wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- SET_LEARN_NODE_STATE ---- 1");
#endif
			if( length >= 6 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_SET_LEARN_NODE_STATE &&
				buffer[2] == 1 /* completed*/ )
			{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- SET_LEARN_NODE_STATE ---- 2");
#endif
				// Node Status
				switch( buffer[3] )
				{
					case LEARN_STATE_ROUTING_PENDING :
						// just wait
						break;
					
					case LEARN_STATE_DONE :
					{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- SET_LEARN_NODE_STATE ---- 3");
#endif
						size_t len = buffer[5];
						if( length >= len + 6 &&
							len + 3 <= sizeof(d->learnInfo) )
						{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- SET_LEARN_NODE_STATE ---- 4");
#endif
							d->learnInfolength = len + 3;
							memcpy(d->learnInfo, &buffer[3], d->learnInfolength);
							d->failed = false;
						}
						else
						{
							// TODO error
							g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobSetLearnNodeState: learn_info too big.");
							d->failed = true;
						}
						setState( ZWaveJob::STOPPED );
						break;
					}
					
					case LEARN_STATE_FAIL :
					default :
						// TODO error
						g_pPlutoLogger->Write(LV_ZWAVE, "ZWJobSetLearnNodeState: failed.");
						d->failed = true;
						setState( ZWaveJob::STOPPED );
						break;
				}
				
				return true;
			}
			break;
	}
	
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- SET_LEARN_NODE_STATE ---- 5");
#endif
	return false;
}

bool ZWJobSetLearnNodeState::run()
{
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_SET_LEARN_NODE_STATE;
	buffer[2] = mode();
	buffer[3] = 1; // Callback FunctionID
	buffer[4] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	d->failed = false;
	
	return handler()->sendData(buffer, 3);
}

void ZWJobSetLearnNodeState::setMode(unsigned char md)
{
	d->mode = md;
}

unsigned char ZWJobSetLearnNodeState::mode() const
{
	return d->mode;
}

int ZWJobSetLearnNodeState::learnInfo(char* buffer, size_t* length)
{
	if( !d->failed && *length >= d->learnInfolength )
	{
#ifdef PLUTO_DEBUG
			g_pPlutoLogger->Write(LV_DEBUG, "----- SET_LEARN_NODE_STATE ---- 6");
#endif
		memcpy(buffer, d->learnInfo, d->learnInfolength);
		*length = d->learnInfolength;
		return 0;
	}
	
	*length = 0;
	
	return -1;
}
