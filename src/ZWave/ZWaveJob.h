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

#ifndef _ZWAVE_JOB_H_
#define _ZWAVE_JOB_H_

#include <stddef.h>
#include <time.h>

class PlutoZWSerialAPI;

/**Virtual class that stands for an atomic job*/
class ZWaveJob
{
	public:
	
		enum JobState { IDLE=0, RUNNING, STOPPED };
		
		enum ZW_JOB { NOTHING=0, GET_VERSION, GET_ID, GET_INIT_DATA, GET_SUC, GET_NODE_PROTOCOL_INFO,
					  SET_INFO, INITIALIZE, RECEIVE, RESET, ADD_NODE, REMOVE_NODE,
					  SET_SWITCH_LEVEL, GET_SWITCH_BINARY, LIGHT_LEVEL,
					  SET_LEARN_NODE_STATE };

		ZWaveJob(PlutoZWSerialAPI*);
		
		virtual ~ZWaveJob();
		
		/** It is called to perform the job.*/
		virtual bool run() = 0;
		
		/** Try again the last command.*/
		virtual bool runAgain();
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, size_t length) = 0;
		
		/** Returns the job state.*/
		virtual JobState state() const;
		
		/** Sets the job state.*/
		virtual void setState(JobState);
		
		/** Returns the job type.*/
		virtual ZW_JOB type() const;
		
		/** Sets the job type.*/
		virtual void setType(ZW_JOB);
		
		/** Callback to handle receive timeout.*/
		virtual void timeoutHandler();
		
		/** Returns true if the job needs the receiving timeout (time from the command and the answer).*/
		virtual bool hasReceivingTimeout();
		
		/** Returns the current receving timeout value.*/
		virtual time_t receivingTimeout() const;
		
		/** Set the job to use the receiving timeout.
		  * 0 = no timeout
		  */
		virtual void setReceivingTimeout(time_t);
		
	protected:
	
		/** Returns the job handler.*/
		virtual PlutoZWSerialAPI* handler() const;
		
		/** Return true if the 'again' flag is set.*/
		virtual bool again() const;
		
	private:
	
		// disable the default constructor
		ZWaveJob();
		
		class Private;
		Private * d;
};

#endif
