#ifndef _ZWJOBGETNODEPROTOCOLINFO_H_
#define _ZWJOBGETNODEPROTOCOLINFO_H_

#include "ZWaveJob.h"

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
