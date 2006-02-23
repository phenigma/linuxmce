//
// C++ Interface: PlutoZWSerialAPI
//
//
// Author:	Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _ZWJOBGETNODEPROTOCOLINFO_H_
#define _ZWJOBGETNODEPROTOCOLINFO_H_

#include "ZWaveJob.h"
/**Job to get the node protocol information*/
class ZWJobGetNodeProtocolInfo : public ZWaveJob
{
	public:
	
		ZWJobGetNodeProtocolInfo(PlutoZWSerialAPI*);
		
		virtual ~ZWJobGetNodeProtocolInfo();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
		/** You will get information about this node.*/
		virtual void setNodeID(unsigned short id);
		
		/** Returns the node id.*/
		virtual unsigned short nodeID();
		
	private:
	
		// disable the default constructor
		ZWJobGetNodeProtocolInfo();
		
		class Private;
		Private * d;
};

#endif
