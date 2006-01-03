#ifndef _ZWJOBINITIALIZE_H_
#define _ZWJOBINITIALIZE_H_

#include "ZWaveJob.h"

class ZWJobInitialize : public ZWaveJob
{
	public:
	
		ZWJobInitialize(PlutoZWSerialAPI*);
		
		virtual ~ZWJobInitialize();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobInitialize();
		
		class Private;
		Private * d;
};

#endif
