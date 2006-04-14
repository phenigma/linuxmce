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

#ifndef _ZWJOBGETID_H_
#define _ZWJOBGETID_H_

#include "ZWaveJob.h"

/**get the node id and the home id of the zwave box*/
class ZWJobGetID : public ZWaveJob
{
	public:
	
		ZWJobGetID(PlutoZWSerialAPI*);
		
		virtual ~ZWJobGetID();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length);
		
	private:
	
		// disable the default constructor
		ZWJobGetID();
		
		class Private;
		Private * d;
};

#endif
