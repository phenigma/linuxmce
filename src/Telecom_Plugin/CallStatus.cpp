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

CallStatus::CallType CallStatus::String2CallType(string)
{
	return CallStatus::DirectCall;
}

string CallStatus::CallType2String(CallStatus::CallType)
{
	return "";
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

