#ifndef INSTEONJOBSENDDATA_H_
#define INSTEONJOBSENDDATA_H_


#include "InsteonJob.h"
#include <string>


class InsteonJobSendData : public InsteonJob
{
	public:
	
		InsteonJobSendData(LMCEInsteonAPI*);
	
		virtual ~InsteonJobSendData();
	
		/** It is called to process the protocol data flow.*/
		virtual bool processData(const char* buffer, size_t length);
		
	protected:
	
		std::string logID_;
		unsigned char callbackID_;
		bool onlyShortAnswer_;
		
	private:
	
		// disable the default constructor
		InsteonJobSendData();
};

#endif /*INSTEONJOBSENDDATA_H_*/
