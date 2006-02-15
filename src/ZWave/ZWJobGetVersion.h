#ifndef _ZWJOBGETVERSION_H_
#define _ZWJOBGETVERSION_H_
#include "ZWaveJob.h"

class ZWJobGetVersion :
	public ZWaveJob
{
public:
	ZWJobGetVersion(PlutoZWSerialAPI* ser);
	virtual ~ZWJobGetVersion();

	/** It is called to perform the job.*/
	virtual bool run();
	
	/** It is called to process the protocol data flow.*/
	virtual bool processData(const char * buffer, size_t length);
	

};
#endif
