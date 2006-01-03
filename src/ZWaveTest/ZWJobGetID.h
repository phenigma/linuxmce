
#include "zwavejob.h"

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
