#pragma once
#include "ZWaveJob.h"

class ZWJobGetInitData :
	public ZWaveJob
{
public:
	ZWJobGetInitData(PlutoZWSerialAPI* ser);
	virtual ~ZWJobGetInitData(void);

	/** It is called to perform the job.*/
	virtual bool run();
	
	/** It is called to process the protocol data flow.*/
	virtual bool processData(const char * buffer, size_t length);
};
