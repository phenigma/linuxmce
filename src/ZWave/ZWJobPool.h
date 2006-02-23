//
// C++ Interface: ZWJobPool
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ZWJOBPOOL_H
#define ZWJOBPOOL_H

#include "ZWaveJob.h"

/**
Pooling ZWave network.

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class ZWJobPool : public ZWaveJob
{
	public:
	
		ZWJobPool(PlutoZWSerialAPI* arg1);
		
		virtual ~ZWJobPool();
	
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobPool();
		
		class Private;
		Private * d;
	
};

#endif
