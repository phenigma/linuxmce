#include "ZWaveNode.h"

#include "main.h"
#include "SerialConnection.h"

class ZWaveNode::Private
{
	public:
	
		Private(unsigned long hid, unsigned short nid);
		
		~Private();
		
		unsigned short nodeID;
		
		unsigned long homeID;
		
		NodeType type;
	
		unsigned char capabilities;
		
		unsigned char security;
		
	private:

};

ZWaveNode::Private::Private(unsigned long hid, unsigned short nid)
	: nodeID(nid),
	  homeID(hid),
	  capabilities(0),
	  security(0)
{
	type.basic = 0;
	type.generic = 0;
	type.specific = 0;
}

ZWaveNode::Private::~Private()
{
}

ZWaveNode::ZWaveNode(unsigned long homeID, unsigned short nodeID)
{
	d = new Private(homeID, nodeID);
	if( d == NULL )
	{
		// error message
	}
}

ZWaveNode::~ZWaveNode()
{
	delete d;
	d = NULL;
}

unsigned short ZWaveNode::nodeID() const
{
	return d->nodeID;
}

unsigned long ZWaveNode::homeID() const
{
	return d->homeID;
}

const NodeType& ZWaveNode::type() const
{
	return d->type;
}

void ZWaveNode::setBasicType(unsigned char basic)
{
	d->type.basic = basic;
}

void ZWaveNode::setGenericType(unsigned char generic)
{
	d->type.generic = generic;
}

void ZWaveNode::setSpecificType(unsigned char specific)
{
	d->type.specific = specific;
}

unsigned char ZWaveNode::capabilities() const
{
	return d->capabilities;
}

void ZWaveNode::setCapabilities(unsigned char capa)
{
	d->capabilities = capa;
}

unsigned char ZWaveNode::security()
{
	return d->security;
}

void ZWaveNode::setSecurity(unsigned char sec)
{
	d->security = sec;
}
