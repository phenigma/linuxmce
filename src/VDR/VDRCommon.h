#ifndef VDRCommon_h
#define VDRCommon_h

#include "DCE/PlainClientSocket.h"

#define VDR_SOCKET_TIMEOUT	3  // SECONDS

bool SendVDRCommand(string sIP, string sCommand,string &sVDRResponse)
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING,"Going to send command %s",sCommand.c_str());
	PlainClientSocket _PlainClientSocket(sIP + ":2001");
	if( !_PlainClientSocket.Connect() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Unable to connect to VDR client");
		return false;
	}
LoggerWrapper::GetInstance()->Write(LV_STATUS,"connected");
	string sResponse;
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="220" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ready got %s",sResponse.c_str());
		return false;
	}

	if( !_PlainClientSocket.SendString(sCommand) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send string");
		return false;
	}

	for(int iLines=0;;++iLines)
	{
		if( iLines==1399 )
			int k=1;
		if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with command got %d lines then failed: %s",iLines,sVDRResponse.c_str());
			return false;
		}
		
		if( sResponse.substr(0,3)!="250" && sResponse.substr(0,3)!="215" )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with command got %d lines then got: %s",iLines,sResponse.c_str());
			return false;
		}

		if( sResponse.size()>4 )
		{
			if( sVDRResponse.empty()==false )
				sVDRResponse += "\n";
			sVDRResponse += sResponse.substr(4);
		}

		if( sResponse[3]!='-' )
			break;
	}
LoggerWrapper::GetInstance()->Write(LV_WARNING,"VDR Responded %s",sResponse.c_str());
	if( !_PlainClientSocket.SendString("QUIT") )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Could not send string");
		return false;
	}

	sResponse="";
	if( !_PlainClientSocket.ReceiveString(sResponse,VDR_SOCKET_TIMEOUT) || sResponse.substr(0,3)!="221" )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"VDR not ok with quit got %s",sResponse.c_str());
		return false;
	}
	
	_PlainClientSocket.Close();
	return true;
}

#endif
