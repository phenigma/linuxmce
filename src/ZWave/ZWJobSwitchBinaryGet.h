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

#ifndef _ZWJOBSWITCHBINARYGET_H_
#define _ZWJOBSWITCHBINARYGET_H_

#include "ZWaveJob.h"

/**get the binary switch state*/
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
