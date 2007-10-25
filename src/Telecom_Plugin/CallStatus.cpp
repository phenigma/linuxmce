//
// C++ Implementation: CallStatus
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "CallStatus.h"
#include "DCE/Logger.h"

using namespace DCE;

char * CallStatus::conferencePrefix = "c";
unsigned CallStatus::autoId = 0;

CallStatus::CallType CallStatus::String2CallType(string sCalltype)
{
	if( sCalltype == "Conference" )
		return CallStatus::Conference;
	
	return CallStatus::DirectCall;
}

string CallStatus::CallType2String(CallStatus::CallType calltype)
{
	string sOutput;
	switch( calltype )
	{
		case CallStatus::Conference :
			sOutput = "Conference";
			break;
			
		case CallStatus::DirectCall :
			sOutput = "DirectCall";
		default:
			break;
	}
	
	return sOutput;
}

string CallStatus::GetStringConferenceID(unsigned uID)
{
	char buffer[16];
	
	snprintf(buffer, sizeof(buffer), "%s%04d", conferencePrefix, uID);
	
	return string(buffer);
}

CallStatus::CallStatus()
	: type(CallStatus::DirectCall)
{
    id = StringUtils::itos(autoId++);
}

CallStatus::~CallStatus()
{
}

string CallStatus::GetDebugInfo() const
{
	string sDebug;
	
	if( IsConference() )
	{
		sDebug += "Conference " + StringUtils::itos( GetConferenceID() ) + " : ";
	}
	else
	{
		sDebug += "Simple call : ";
	}
	
	sDebug += id + " : channels ";
	for(map<string, string>::const_iterator it=channels.begin(); it!=channels.end(); ++it)
	{
		sDebug += (*it).first + " ,";
	}
	
	return sDebug;
}
