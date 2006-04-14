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

#ifndef _ZWJOBRESET_H_
#define _ZWJOBRESET_H_

#include "ZWaveJob.h"

/**reset job*/
class ZWJobReset :	public ZWaveJob
{
	public :
	
		ZWJobReset(PlutoZWSerialAPI*);
		
		virtual ~ZWJobReset();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobReset();
		
		class Private;
		Private * d;
};

#endif
