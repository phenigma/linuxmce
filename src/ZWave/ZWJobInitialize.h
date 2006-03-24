//
// C++ Interface: ZWJobIntialize
//
// Description: ZWave initialization, looking for nodes
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//  		Edgar Grimberg <edgar.g@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _ZWJOBINITIALIZE_H_
#define _ZWJOBINITIALIZE_H_

#include "ZWaveJob.h"
/**ZWave initialization, looking for nodes*/
class ZWJobInitialize : public ZWaveJob
{
	public:
	
		ZWJobInitialize(PlutoZWSerialAPI*);
		
		virtual ~ZWJobInitialize();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
		/** Callback to handle receive timeout.*/
		virtual void timeoutHandler();
		
	private:
	
		// disable the default constructor
		ZWJobInitialize();
		
		class Private;
		Private * d;
};

#endif
