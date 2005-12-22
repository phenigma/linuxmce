#ifndef _ZWAVE_JOB_H_
#define _ZWAVE_JOB_H_

class ZW_SerialAPI;

class ZWaveJob
{
	public:
	
		enum JobState { IDLE, RUNNING, STOPPED };
		
		ZWaveJob(ZW_SerialAPI*);
		
		virtual ~ZWaveJob();
		
		/** It is called to perform the job.*/
		virtual bool run() = 0;
		
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char * buffer, unsigned length) = 0;
		
		/** Returns the job state.*/
		virtual JobState state() const;
		
		/** Sets the job state.*/
		virtual void setState(JobState);
		
	private:
	
		class Private;
		Private * d;
};

#endif
