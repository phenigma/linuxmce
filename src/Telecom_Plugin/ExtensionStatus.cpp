//
// C++ Implementation: ExtensionStatus
//
// Description: 
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ExtensionStatus.h"
#include "DCE/Logger.h"

using namespace DCE;

string ExtensionStatus::Availability2String( ExtensionStatus::Availability ava )
{
	string sOutAvailability;
	
	switch( ava )
	{
		case ExtensionStatus::Unregistered :
			sOutAvailability = "Unregistered";
			break;
			
		case ExtensionStatus::Registered :
			sOutAvailability = "Registered";
			break;
			
		case ExtensionStatus::UnknownAvailability :
		default :
			sOutAvailability = "Unknown";
			break;
	}
	
	return sOutAvailability;
}

string ExtensionStatus::Activity2String( ExtensionStatus::Activity act )
{
	string sOutActivity;
	
	switch( act )
	{
		case ExtensionStatus::Ready :
			sOutActivity = "Ready";
			break;
		
		case ExtensionStatus::OnThePhone :
			sOutActivity = "OnThePhone";
			break;
		
		case ExtensionStatus::Ringing :
			sOutActivity = "Ringing";
			break;
		
		case ExtensionStatus::Dialing :
			sOutActivity = "Dialing";
			break;
			
		case ExtensionStatus::UnknownActivity :
		default:
			sOutActivity = "Unknown";
			break;
	}
}

ExtensionStatus::Availability ExtensionStatus::String2Availability( string sAva )
{
	if( sAva == "" )
	{
	}
	else if( "" )
	{
	}
	else if( "" )
	{
	}
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"ExtensionStatus: ");
	return ExtensionStatus::Unregistered;
}

ExtensionStatus::Activity ExtensionStatus::String2Activity( string )
{
	return ExtensionStatus::Ready;
}

ExtensionStatus::ExtensionStatus()
	: availability(ExtensionStatus::Unregistered),
	  activity(ExtensionStatus::Ready)
{
}
