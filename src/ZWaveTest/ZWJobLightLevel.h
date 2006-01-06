#ifndef _ZWJOBLIGHTLEVEL_H_
#define _ZWJOBLIGHTLEVEL_H_

#include "ZWaveJob.h"

class ZWJobLightLevel : public ZWaveJob
{
	public:
	
		ZWJobLightLevel(PlutoZWSerialAPI*, unsigned char level, unsigned char nodeID);
		
		virtual ~ZWJobLightLevel();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobLightLevel();
		
		class Private;
		Private * d;
};

#endif
