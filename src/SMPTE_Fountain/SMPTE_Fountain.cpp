//<-dceag-d-b->
#include "SMPTE_Fountain.h"
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
SMPTE_Fountain::SMPTE_Fountain(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: SMPTE_Fountain_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
SMPTE_Fountain::SMPTE_Fountain(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: SMPTE_Fountain_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

//<-dceag-dest-b->
SMPTE_Fountain::~SMPTE_Fountain()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool SMPTE_Fountain::GetConfig()
{
	if( !SMPTE_Fountain_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_bIsActive = false;
	m_pDevice_Xine = m_pData->FindFirstRelatedDeviceOfTemplate(DEVICETEMPLATE_Xine_Player_CONST);
	if( !m_pDevice_Xine )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"SMPTE_Fountain::GetConfig  cannot find xine");
		return false;
	}

	string sConfiguration = DATA_Get_Configuration();
	string::size_type pos = 0;
	while( true )
	{
		string sToken = StringUtils::Tokenize(sConfiguration,"\r\n",pos);
		if( sToken.empty() )
		{
			if( pos>=sConfiguration.size() )
				break;
			else
				continue;
		}
		string::size_type tab = sToken.find('\t');
		if( tab==string::npos )
			continue;
		string sFile = sToken.substr(0,tab);
		string sTime = sToken.substr(tab+1);  // Replace with a long that's the real int value of time code
		m_mapFilesTimeCode[sFile] = atoi(sTime.c_str());
	}
	PurgeInterceptors();  // Since this is not a plugin, be sure to purge so we're not registered more than once
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &SMPTE_Fountain::MediaPlaying ), 0, m_pDevice_Xine->m_dwPK_Device, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Play_Media_CONST );
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool SMPTE_Fountain::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
SMPTE_Fountain_Command *Create_SMPTE_Fountain(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new SMPTE_Fountain(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void SMPTE_Fountain::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void SMPTE_Fountain::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/** Turn on the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void SMPTE_Fountain::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
	m_bIsActive = true;
}

//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/** Turn off the device */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void SMPTE_Fountain::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
	m_bIsActive = false;
}

bool SMPTE_Fountain::MediaPlaying( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	if( !m_bIsActive )
		return false;

	string sFileName = pMessage->m_mapParameters[COMMANDPARAMETER_Filename_CONST];
	if( m_mapFilesTimeCode.find(sFileName)==m_mapFilesTimeCode.end() )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Ignoring unknown file %s",sFileName.c_str());
		return false;
	}

	// Do whatever we need to do

	return false;
}
