//
// C++ Implementation: ZWJobRemoveNode
//
// Description: ZWave Remove Node Command
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ZWJobRemoveNode.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "ZW_classcmd.h"

#include <stdio.h>
#include <string.h>

// ----------------------------------

class ZWJobRemoveNode::Private
{
	public:
		Private();
		
		~Private();
	
	private:
};

ZWJobRemoveNode::Private::Private()
{
}

ZWJobRemoveNode::Private::~Private()
{
}

// ----------------------------------

ZWJobRemoveNode::ZWJobRemoveNode(PlutoZWSerialAPI * zwAPI)
	: ZWaveJob(zwAPI)
{
	d = new Private();
	setType(ZWaveJob::REMOVE_NODE);
}

ZWJobRemoveNode::~ZWJobRemoveNode()
{
	delete d;
	d = NULL;
}

bool ZWJobRemoveNode::processData(const char* buffer, size_t length)
{
	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobRemoveNode wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 3 && 
				buffer[0] == REQUEST &&
				buffer[1] == FUNC_ID_ZW_SET_DEFAULT &&
				buffer[2] == 1 /* completed*/ )
			{
				setState( ZWaveJob::STOPPED );
				return true;
			}
			break;
	}
	
	return false;
}

bool ZWJobRemoveNode::run()
{
	char buffer[10];
	
	buffer[0] = REQUEST;
	buffer[1] = FUNC_ID_ZW_SET_DEFAULT;
	buffer[2] = 1; // Callback FunctionID
	buffer[3] = 0;
	
	setState(ZWaveJob::RUNNING);
	
	return handler()->sendData(buffer, 3);
}

