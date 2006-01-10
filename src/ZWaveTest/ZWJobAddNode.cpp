//
// C++ Implementation: zwjobaddnode
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobAddNode.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "ZW_classcmd.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobAddNode::Private
{
	public:
		Private();
		
		~Private();
	
	private:
};

ZWJobAddNode::Private::Private()
{
}

ZWJobAddNode::Private::~Private()
{
}

// ----------------------------------

ZWJobAddNode::ZWJobAddNode(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::ADD_NODE);
}

ZWJobAddNode::~ZWJobAddNode()
{
	delete d;
	d = NULL;
}

bool ZWJobAddNode::processData(const char* buffer, size_t length)
{
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobAddNode wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 6 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_SET_LEARN_NODE_STATE &&
				buffer[2] == 1 /* completed*/ )
			{
				// Node Status
				switch( buffer[3] )
				{
					case LEARN_STATE_ROUTING_PENDING :
						break;
					
					case LEARN_STATE_DONE :
						break;
					
					case LEARN_STATE_FAIL :
					default :
						break;
				}
				
				// 
				
				// LEARN_NODE_STATE_OFF
				
				setState( ZWaveJob::STOPPED );
				return true;
			}
			break;
	}
	
	return false;
}

bool ZWJobAddNode::run()
{
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_SET_LEARN_NODE_STATE;
	buffer[2] = 1; // Callback FunctionID
	buffer[3] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 3);
}

