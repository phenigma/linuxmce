#ifndef _ZWAVE_JOB_H_
#define _ZWAVE_JOB_H_
#include <stddef.h>

class PlutoZWSerialAPI;

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
		
	protected:
	
		/** Returns the job handler.*/
		virtual PlutoZWSerialAPI* handler() const;
		
	private:
	
		// disable the default constructor
		ZWaveJob();
		
		class Private;
		Private * d;
};

#endif
