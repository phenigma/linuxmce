#ifndef _ZWJOBGETSUC_H_
#define _ZWJOBGETSUC_H_

#include "ZWaveJob.h"

class ZWJobGetSUC :
	public ZWaveJob
{
	public:
	
		ZWJobGetSUC(PlutoZWSerialAPI*);
		
		virtual ~ZWJobGetSUC();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobGetSUC();
		
		class Private;
		Private * d;
};

#endif
