#ifndef _ZWJOBSWITCHCHANGELEVEL_H_
#define _ZWJOBSWITCHCHANGELEVEL_H_

#include "ZWaveJob.h"

class ZWJobSwitchChangeLevel :
	public ZWaveJob
{
public:
	ZWJobSwitchChangeLevel(PlutoZWSerialAPI * zwAPI, unsigned char level, unsigned short nodeID);
	virtual ~ZWJobSwitchChangeLevel();
	/** It is called to perform the job.*/
	virtual bool run();
	
	/** It is called to process the protocol data flow.*/
	virtual bool processData(const char * buffer, size_t length);

private:
	class Private;
	Private *d;
};

#endif
