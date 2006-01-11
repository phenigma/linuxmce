//
// C++ Interface: ZWJobSetLearnNodeState
//
// Description: ZWave Set Learn Node State Command
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ZWJOBSETLEARNNODESTATE_H
#define ZWJOBSETLEARNNODESTATE_H

#include "ZWaveJob.h"

/**
ZWave Set Learn Node State Command
 
	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class ZWJobSetLearnNodeState : public ZWaveJob
{
	public:
	
		ZWJobSetLearnNodeState(PlutoZWSerialAPI*);
		
		virtual ~ZWJobSetLearnNodeState();
		
		virtual bool processData(const char* buffer, size_t length);
		
		virtual bool run();
		
		/** Sets the running mode: 
		  * LEARN_NODE_STATE_OFF
		  * LEARN_NODE_STATE_NEW
		  * LEARN_NODE_STATE_UPDATE
		  * LEARN_NODE_STATE_DELETE
		  */
		virtual void setMode(unsigned char);
		
		/** Returns the running mode.*/
		virtual unsigned char mode() const;
		
		/** Fill the buffer with the LEARN_INFO data.
		  * @param buffer will be filled with data
		  * @param length will contain the length of the data.
		  * @return 0 for success
		  */
		virtual int learnInfo(char* buffer, size_t* length);
		
	private:
	
		// disable the default constructor
		ZWJobSetLearnNodeState();
		
		class Private;
		Private * d;
	
};

#endif
