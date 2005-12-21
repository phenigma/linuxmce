#ifndef _ZW_SERIAL_API_H_
#define _ZW_SERIAL_API_H_

class ZW_SerialAPI
{
	public:

		enum ZW_JOB { NOTHING, IDLE, VERSION, GET_INFO, SET_INFO, INITIALIZE, RECEIVE, ADD_NODE, REMOVE_NODE };

		ZW_SerialAPI(void);

		virtual ~ZW_SerialAPI(void);

		bool insertJob();


	private:

		class Private;
		Private * d;
};

#endif
