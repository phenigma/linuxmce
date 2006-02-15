#ifndef _ZWJOBSWITCHBINARYGET_H_
#define _ZWJOBSWITCHBINARYGET_H_

#include "ZWaveJob.h"

class ZWJobSwitchBinaryGet :
	public ZWaveJob
{
public:
	ZWJobSwitchBinaryGet(PlutoZWSerialAPI * zwAPI, unsigned short nodeID);
	virtual ~ZWJobSwitchBinaryGet();
	/** It is called to perform the job.*/
	virtual bool run();
	
	/** It is called to process the protocol data flow.*/
	virtual bool processData(const char * buffer, size_t length);

private:
	class Private;
	Private *d;
};

#endif //_ZWJOBSWITCHBINARYGET_H_
