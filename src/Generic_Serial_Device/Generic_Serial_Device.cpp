/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

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

#include "DCE/DCEConfig.h"
#include "pluto_main/Database_pluto_main.h"

#include "RubyIOManager.h"

#define GSD_COMMAND_LINE	"Generic_Serial_Device"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Generic_Serial_Device::Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Generic_Serial_Device_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pdbPlutoMain = NULL;
}

//<-dceag-getconfig-b->
bool Generic_Serial_Device::GetConfig()
{
	if( !Generic_Serial_Device_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	DCEConfig dceconf;

	m_pdbPlutoMain = new Database_pluto_main();
	if(!m_pdbPlutoMain->Connect(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword,
									dceconf.m_sDBName,dceconf.m_iDBPort) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		return false;
	}
	g_pPlutoLogger->Write(LV_STATUS, "Successfully connected to database!");
	
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	pmanager->setDatabase(m_pdbPlutoMain);
	pmanager->setEventDispatcher(GetEvents());
	
	GSDMessageProcessor::setCommandImpl(this);
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Generic_Serial_Device::~Generic_Serial_Device()
//<-dceag-dest-e->
{
	delete m_pdbPlutoMain;
}

bool Generic_Serial_Device::Connect(int iPK_DeviceTemplate )
{
	if( !Generic_Serial_Device_Command::Connect(iPK_DeviceTemplate) )
		return false;
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

	if(GSDMessageProcessor::ProcessMessage(pMessage)) {
	    g_pPlutoLogger->Write(LV_STATUS, "Message processed by Translator.");
		return;
	}

	DispatchMessage(pMessage);
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
    g_pPlutoLogger->Write(LV_STATUS, "Received UNKNOWN command.");
	
	if(GSDMessageProcessor::ProcessMessage(pMessage)) {
	    g_pPlutoLogger->Write(LV_STATUS, "Message processed by Translator.");
		return;
	}
	
	DispatchMessage(pMessage);
}

//<-dceag-sample-b->!
//<-dceag-reg-b->!
bool Generic_Serial_Device::Register(){return false;}
//<-dceag-createinst-b->!

/*

	COMMANDS TO IMPLEMENT

*/

// Intercept all commands
/*
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
//			TransmitCommand((*itCodeMap).second,pMessage);
		}
	}
	return iHandled!=0;
}
*/
void Generic_Serial_Device::Transmit(const char *pData,int iSize)
{
}

void Generic_Serial_Device::DispatchMessage(Message* pmsg) {
	g_pPlutoLogger->Write(LV_STATUS, "Routing Message %d to %d...", pmsg->m_dwID, pmsg->m_dwPK_Device_To);

	RubyIOManager* pmanager = RubyIOManager::getInstance();
	DeviceData_Base *pDeviceData_Base = 
			m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pmsg->m_dwPK_Device_To);
	pmanager->RouteMessage(pDeviceData_Base, pmsg);
}

void Generic_Serial_Device::RunThread() {
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	ParseDeviceHierarchy(m_pData);
	
	pmanager->Run(false);
	Generic_Serial_Device_Command::RunThread();
	pmanager->Wait(true);
    g_pPlutoLogger->Write(LV_STATUS, "Generic Serial Device RunThread ended.");
}

void Generic_Serial_Device::ParseDeviceHierarchy(DeviceData_Impl *pdevdata) {
	g_pPlutoLogger->Write(LV_STATUS, "Device %d has commad line <%s>.", pdevdata->m_dwPK_Device, pdevdata->m_sCommandLine.c_str());
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	if(pdevdata->m_sCommandLine == GSD_COMMAND_LINE || pdevdata->m_bImplementsDCE) {
		if(pmanager->addDevice(this, pdevdata) >= 0) {
			VectDeviceData_Impl& vDeviceData = pdevdata->m_vectDeviceData_Impl_Children;
			for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
				if(!pmanager->hasDevice(vDeviceData[i])) {
					ParseDeviceHierarchy(vDeviceData[i]);
				}
			}
		}
	} else {
	    g_pPlutoLogger->Write(LV_STATUS, "Device %d has no GSD specified as command line. No ruby code will be executed.",
						pdevdata->m_dwPK_Device);
	}
}
//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** The "seek" variant which scans and stops on valid stations. */

void Generic_Serial_Device::CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
{
}
//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** The "seek" variant which scans and stops on valid stations. */

void Generic_Serial_Device::CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
{
}
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/**  */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void Generic_Serial_Device::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
{
}
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/**  */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void Generic_Serial_Device::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
{
}
//<-dceag-c93-b->

	/** @brief COMMAND: #93 - Scan Fwd/Fast Fwd */
	/** The "fine tune" command which increases the frequency 1 notch a time regardless of signal strength. */

void Generic_Serial_Device::CMD_Scan_FwdFast_Fwd(string &sCMD_Result,Message *pMessage)
//<-dceag-c93-e->
{
}
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/**  */

void Generic_Serial_Device::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
{
}

//<-dceag-c125-b->

	/** @brief COMMAND: #125 - Scan Back/Rewind */
	/** The "fine tune" command which decreases the frequency 1 notch a time regardless of signal strength. */

void Generic_Serial_Device::CMD_Scan_BackRewind(string &sCMD_Result,Message *pMessage)
//<-dceag-c125-e->
{
}
//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/**  */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void Generic_Serial_Device::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
{
}
//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/**  */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Generic_Serial_Device::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
{
}
//<-dceag-c350-b->

	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

void Generic_Serial_Device::CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage)
//<-dceag-c350-e->
{
}
//<-dceag-c351-b->

	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

void Generic_Serial_Device::CMD_Process_IDLE(string &sCMD_Result,Message *pMessage)
//<-dceag-c351-e->
{
}
//<-dceag-c355-b->

	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

void Generic_Serial_Device::CMD_Process_Initialize(string &sCMD_Result,Message *pMessage)
//<-dceag-c355-e->
{
}
//<-dceag-c356-b->

	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

void Generic_Serial_Device::CMD_Process_Release(string &sCMD_Result,Message *pMessage)
//<-dceag-c356-e->
{
}
//<-dceag-c373-b->

	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

void Generic_Serial_Device::CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage)
//<-dceag-c373-e->
{
}
//<-dceag-c384-b->

	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

void Generic_Serial_Device::CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage)
//<-dceag-c384-e->
{
}

