//<-dceag-d-b->
#include "Generic_Serial_Device.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Generic_Serial_Device::Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Generic_Serial_Device_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Generic_Serial_Device::~Generic_Serial_Device()
//<-dceag-dest-e->
{
	
}

bool Generic_Serial_Device::Connect(int iPK_DeviceTemplate )
{
	if( !Generic_Serial_Device_Command::Connect(iPK_DeviceTemplate) )
		return false;

	string codes;
	DCE::CMD_Get_Infrared_Codes_Cat CMD_Get_Infrared_Codes_Cat(m_dwPK_Device, DEVICECATEGORY_Infrared_Plugins_CONST,
		false, BL_SameHouse, m_dwPK_Device, &codes);
	SendCommand(CMD_Get_Infrared_Codes_Cat);

	long iPK_Command, count;
	string code;
	size_t pos = 0;

	count = atoi(StringUtils::Tokenize(codes, "\t", pos).c_str());
	g_pPlutoLogger->Write(LV_STATUS, "IR Code request got this reply: %s", codes.c_str());
	g_pPlutoLogger->Write(LV_STATUS, "IR Code count: %d", count);
	for(long i = 0; i < count; i++)
	{
		iPK_Command = atoi(StringUtils::Tokenize(codes, "\t", pos).c_str());
		code = StringUtils::Tokenize(codes, "\t", pos);
		m_CodeMap[iPK_Command] = code;
		g_pPlutoLogger->Write(LV_STATUS, "Loaded IR code for Device %ld, Action %ld", m_dwPK_Device, iPK_Command);
	}

	return true;
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Generic_Serial_Device::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Generic_Serial_Device::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!
//<-dceag-reg-b->!
bool Generic_Serial_Device::Register(){return false;}
//<-dceag-createinst-b->!

/*

	COMMANDS TO IMPLEMENT

*/

// Intercept all commands
bool Generic_Serial_Device::ReceivedMessage(class Message *pMessageOriginal)
{
	map<int,string>::iterator itCodeMap;
	int iHandled=0;

	for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
	{
		Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
		if( pMessage->m_dwMessage_Type != MESSAGETYPE_COMMAND || (itCodeMap=m_CodeMap.find(pMessage->m_dwID))==m_CodeMap.end() )
			iHandled += Generic_Serial_Device_Command::ReceivedMessage(pMessage);  // If it's not a command or we don't have an entry for it, let the base class handle it
		else
		{
			TransmitCommand((*itCodeMap).second,pMessage);
		}
	}
	return iHandled!=0;
}

void Generic_Serial_Device::Transmit(const char *pData,int iSize)
{
}
