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

#ifndef _ZWJOBRECEIVE_H_
#define _ZWJOBRECEIVE_H_

#include "ZWaveJob.h"

/**receive job*/
class ZWJobReceive : public ZWaveJob
{
	public:
	
		enum ReceivingState { STOP=0, START, RECEIVING };
		
		ZWJobReceive(PlutoZWSerialAPI*);
		
		virtual ~ZWJobReceive();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
		/** Callback to handle receive timeout.*/
		virtual void timeoutHandler();
		
	private:
	
		// disable the default constructor
		ZWJobReceive();
		
		class Private;
		Private * d;
};

#endif
