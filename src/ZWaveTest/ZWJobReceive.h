#ifndef _ZWJOBRECEIVE_H_
#define _ZWJOBRECEIVE_H_

#include "ZWaveJob.h"

class ZWJobReceive : public ZWaveJob
{
	public:
	
		ZWJobReceive(PlutoZWSerialAPI*);
		
		virtual ~ZWJobReceive();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobReceive();
		
		class Private;
		Private * d;
};

#endif
