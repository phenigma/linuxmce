#ifndef _ZWJOBRESET_H_
#define _ZWJOBRESET_H_

#include "ZWaveJob.h"

class ZWJobReset :	public ZWaveJob
{
	public :
	
		ZWJobReset(PlutoZWSerialAPI*);
		
		virtual ~ZWJobReset();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobReset();
		
		class Private;
		Private * d;
};

#endif
