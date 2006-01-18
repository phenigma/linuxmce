#include "ZWJobGetNodeProtocolInfo.h"
#include "ZW_SerialAPI.h"
#include "PlutoZWSerialAPI.h"
#include "main.h"
#include "SerialConnection.h"

#include <stdio.h>
#include <string.h>

class ZWJobGetNodeProtocolInfo::Private
{
	public:
	
		Private();
		~Private();
		
		unsigned short nodeID;
	
	private:
};

ZWJobGetNodeProtocolInfo::Private::Private()
	: nodeID(0)
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
	setType(ZWaveJob::GET_NODE_PROTOCOL_INFO);
}

ZWJobGetNodeProtocolInfo::~ZWJobGetNodeProtocolInfo()
{
	delete d;
	d = NULL;
}

bool ZWJobGetNodeProtocolInfo::run()
{
	if( d->nodeID != 0 )
	{
		char buffer[10];
		
		buffer[0] = REQUEST;
		buffer[1] = FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
		buffer[2] = (unsigned char)d->nodeID;
		buffer[3] = 0;
		
		setState(ZWaveJob::RUNNING);
		
		return handler()->sendData(buffer, 3);
	}
	
	g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetNodeProtocolInfo wrong node id.");
	return false;
}

bool ZWJobGetNodeProtocolInfo::processData(const char * buffer, size_t length)
{
	SerialConnection::printDataBuffer(buffer, length, "ZWJobGetNodeProtocolInfo");

	switch( state() )
	{
		default :
		case ZWaveJob::IDLE :
		case ZWaveJob::STOPPED :
			g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetNodeProtocolInfo wrong state.");
			break;
			
		case ZWaveJob::RUNNING :
			if( length >= 8 && 
				buffer[0] == RESPONSE &&
				buffer[1] == FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO )
			{
				ZWaveNode * node = handler()->getNode(d->nodeID);
				
				if( node == NULL )
				{
					node = new ZWaveNode(handler()->homeID(), d->nodeID);
					if( !handler()->insertNode(node) )
					{
						delete node;
						node = NULL;
					}
				}
				
				if( node != NULL )
				{
					node->setCapabilities(buffer[2]);
					node->setSecurity(buffer[3]);
					// buffer[4] is reserved
					node->setBasicType(buffer[5]);
					node->setGenericType(buffer[6]);
					node->setSpecificType(buffer[7]);
					
					setState(ZWaveJob::STOPPED);
					
					return true;
				}
				else
				{
					g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetNodeProtocolInfo there isn't such node ID: %d.",
						d->nodeID);
				}
			}
			else
			{
				g_pPlutoLogger->Write(LV_WARNING, "ZWJobGetNodeProtocolInfo data error");
			}
			break;
	}
	
	return false;
}

void ZWJobGetNodeProtocolInfo::setNodeID(unsigned short id)
{
	d->nodeID = id;
}

unsigned short ZWJobGetNodeProtocolInfo::nodeID()
{
	return d->nodeID;
}
		
