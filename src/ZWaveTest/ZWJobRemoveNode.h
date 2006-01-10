//
// C++ Interface: ZWJobRemoveNode
//
// Description: ZWave Remove Node Command
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZWJOBREMOVENODE_H
#define ZWJOBREMOVENODE_H

#include "ZWaveJob.h"

/**
ZWave Remove Node Command

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class ZWJobRemoveNode : public ZWaveJob
{
	public:
	
		ZWJobRemoveNode(PlutoZWSerialAPI* arg1);
	
		virtual ~ZWJobRemoveNode();
	
		/** It is called to perform the job.*/
		virtual bool processData(const char* buffer, size_t length);
		
		/** It is called to process the protocol data flow.*/
		virtual bool run();
		
	private:
	
		// disable the default constructor
		ZWJobRemoveNode();
		
		class Private;
		Private * d;

};

#endif
