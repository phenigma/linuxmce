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

#ifndef _ZWJOBLIGHTLEVEL_H_
#define _ZWJOBLIGHTLEVEL_H_

#include "ZWaveJob.h"

/**Set the light level and gets the status*/
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
