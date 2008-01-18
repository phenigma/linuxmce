#include "InsteonJobSendData.h"
#include "LMCEInsteonAPI.h"
#include "main.h"
#include "InsteonJob.h"


InsteonJobSendData::InsteonJobSendData(LMCEInsteonAPI* insteonAPI)
	: InsteonJob(insteonAPI),
	  logID_("InsteonJobSendData: "),
	  callbackID_(0),
	  onlyShortAnswer_(false)
{
	setType(InsteonJob::SEND_DATA);
}

InsteonJobSendData::~InsteonJobSendData()
{
}

bool InsteonJobSendData::processData(const char* buffer, size_t length)
{
	
	//This needs to be filled in to perform data send
	switch(state())
	{
		default:
		case InsteonJob::IDLE:
		case InsteonJob::STOPPED:
			return true;		
	}
	
	//return handler()->processData(buffer, length);
	return true;
}
