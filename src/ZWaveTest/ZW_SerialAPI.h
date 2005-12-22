#ifndef _ZW_SERIAL_API_H_
#define _ZW_SERIAL_API_H_

class ZWaveJob;

class ZW_SerialAPI
{
	public:

		enum SerialState { STOPPED, IDLE, RUNNING, WAITTING };
		
		enum ZW_JOB { VERSION, GET_INFO, SET_INFO, INITIALIZE, RECEIVE, ADD_NODE, REMOVE_NODE };

		ZW_SerialAPI();

		virtual ~ZW_SerialAPI();

		virtual bool start();
		
		virtual bool stop();
		
		virtual bool insertJob(ZWaveJob*);

	private:

		class Private;
		Private * d;
};

#endif
