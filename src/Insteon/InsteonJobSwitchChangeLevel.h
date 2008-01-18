//
// C++ Interface: LMCEInsteonAPI
//
//


#ifndef INSTEONJOBSWITCHCHANGELEVEL_H_
#define INSTEONJOBSWITCHCHANGELEVEL_H_



#include "InsteonJobSendData.h"

/**change the switch level (the same for dimmer)*/
class InsteonJobSwitchChangeLevel :
	public InsteonJobSendData
{
	public:
	
		InsteonJobSwitchChangeLevel(LMCEInsteonAPI * insteonAPI, unsigned char level, char* nodeID);
		
		virtual ~InsteonJobSwitchChangeLevel();
		
		/** It is called to perform the job.*/
		virtual bool run();
		
		virtual void timeoutHandler();
		
	private:
	
		class Private;
		Private *d;
};

#endif /*INSTEONJOBSWITCHCHANGELEVEL_H_*/
