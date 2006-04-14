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

#ifndef _ZWJOBSWITCHCHANGELEVEL_H_
#define _ZWJOBSWITCHCHANGELEVEL_H_

#include "ZWaveJob.h"

/**change the switch level (the same for dimmer)*/
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

	virtual void timeoutHandler();
	
private:

	class Private;
	Private *d;
};

#endif
