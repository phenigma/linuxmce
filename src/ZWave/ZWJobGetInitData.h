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

#ifndef _ZWJOBINITDATA_H_
#define _ZWJOBINITDATA_H_
#include "ZWaveJob.h"

/**get the initializing data*/
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

#endif
