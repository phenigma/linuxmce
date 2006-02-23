//
// C++ Interface: ZWJobAddNode
//
// Description: ZWave Add Node Command
//
//
// Author:	Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZWJOBADDNODE_H
#define ZWJOBADDNODE_H

#include "ZWaveJob.h"

/**
ZWave Add Node Command

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class ZWJobAddNode : public ZWaveJob
{
	public:
	
		ZWJobAddNode(PlutoZWSerialAPI*);
	
		virtual ~ZWJobAddNode();
	
		/** It is called to perform the job.*/
		virtual bool processData(const char* buffer, size_t length);
		
		/** It is called to process the protocol data flow.*/
		virtual bool run();
		
	private:
	
		// disable the default constructor
		ZWJobAddNode();
		
		class Private;
		Private * d;

};

#endif
