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

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "DCE/DCEConfig.h"
#include "RubyIOManager.h"

#define GSD_COMMAND_LINE	"Generic_Serial_Device"

pthread_t Generic_Serial_Device::m_LoggingThread = (pthread_t)NULL;
bool Generic_Serial_Device::m_bStopLogging = true;
int Generic_Serial_Device::m_FdPipe[2] = { -1, -1 };

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Generic_Serial_Device::Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Generic_Serial_Device_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

void* DoLogging(void* p)
{
	Generic_Serial_Device * pGSD = (Generic_Serial_Device*)p;
	if(pGSD != NULL)
	{
		char buffer[1024];
		int iMax = sizeof(buffer) - 1;
		int iRead = 0;
		int flags = 0;
		pGSD->m_bStopLogging = false;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging start.");
		if (-1 == (flags = fcntl(pGSD->m_FdPipe[1], F_GETFL, 0)))
			flags = 0;
		fcntl(pGSD->m_FdPipe[1], F_SETFL, flags | O_NONBLOCK);
		while(  !pGSD->m_bStopLogging )
		{
			iRead = read(pGSD->m_FdPipe[1], buffer, iMax);
			if( 0 > iRead || iRead > iMax )
			{
				// TODO: it can be just a non blocking operation, see EAGAIN
/*				LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging read error %d.", iRead);
				break;*/
			}
			else
			{
				buffer[iRead] = 0;
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "%s", buffer);
			}
			
			usleep(1000);
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging end.");
	}
	
	return NULL;
}

//<-dceag-getconfig-b->
bool Generic_Serial_Device::GetConfig()
{
	if( !Generic_Serial_Device_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	pmanager->setEventDispatcher(GetEvents());
	GSDMessageProcessor::setAllDevices(&(GetData()->m_AllDevices));
	Start();
	
	// start logging thread
	if( !socketpair(AF_UNIX, SOCK_STREAM, 0, m_FdPipe) )
	{
		if( -1 != dup2(m_FdPipe[0], 1) )
		{
			if( !pthread_create(&m_LoggingThread, NULL, DoLogging, (void*)this) )
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging thread is started.");
			}
			else
			{
				m_LoggingThread = (pthread_t)NULL;
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging dup2 error.");
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging pipe error.");
	}
	
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Generic_Serial_Device::~Generic_Serial_Device()
//<-dceag-dest-e->
{
	m_bStopLogging = true;
	if( m_LoggingThread )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging before join.");
		pthread_join(m_LoggingThread, NULL);
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "DoLogging after join.");
	}
	if( -1 != m_FdPipe[0] )
	{
		close(m_FdPipe[0]);
		m_FdPipe[0] = -1;
	}
	if( -1 != m_FdPipe[1] )
	{
		close(m_FdPipe[1]);
		m_FdPipe[1] = -1;
	}
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
void Generic_Serial_Device::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "OK";

	if( ( (pMessage->m_eExpectedResponse==ER_ReplyMessage || pMessage->m_eExpectedResponse==ER_DeliveryConfirmation) && 
			!pMessage->m_bRespondedToMessage ) ||
		!GSDMessageProcessor::ProcessMessage(pMessage) )
	{
		// TODO: should those messages be translated in future ?
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message %d NOT pre-processed.", pMessage->m_dwID);
		sCMD_Result = "OK";
		DispatchMessage(pMessage);
	}
	else
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message processed by Translator.");
	}
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
	sCMD_Result = "OK";
//    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Received UNKNOWN command.");
	
	if( ( (pMessage->m_eExpectedResponse==ER_ReplyMessage || pMessage->m_eExpectedResponse==ER_DeliveryConfirmation) && 
			!pMessage->m_bRespondedToMessage ) ||
		!GSDMessageProcessor::ProcessMessage(pMessage) )
	{
		// TODO: should those messages be translated in future ?
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message %d NOT pre-processed.", pMessage->m_dwID);
		sCMD_Result = "OK";
		DispatchMessage(pMessage);
	}
	else
	{
	    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Message processed by Translator.");
	}
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
	if( pmsg == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Generic_Serial_Device::DispatchMessage( NULL )");
		return;
	}
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Routing Message %d to %d...", pmsg->m_dwID, pmsg->m_dwPK_Device_To);

	RubyIOManager* pmanager = RubyIOManager::getInstance();
	DeviceData_Base *pDeviceData_Base = 
			m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pmsg->m_dwPK_Device_To);
	if( pDeviceData_Base != NULL )
	{
		pmanager->RouteMessage(pDeviceData_Base, pmsg);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Routing Message for Device_To %d which is NULL", pmsg->m_dwPK_Device_To);
	}
}

void Generic_Serial_Device::RunThread() {
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	ParseDeviceHierarchy(m_pData);
	
	pmanager->Run(false);
	Generic_Serial_Device_Command::RunThread();
	pmanager->Wait(true);
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Generic Serial Device RunThread ended.");
}

void Generic_Serial_Device::ParseDeviceHierarchy(DeviceData_Impl *pdevdata) {
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	if(pdevdata->m_bRunningWithoutDeviceData)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Running without DeviceData, will try to add device");
		pmanager->addDevice(this, pdevdata);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device %d has commad line <%s>.", pdevdata->m_dwPK_Device, pdevdata->m_sCommandLine.c_str());
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
	    	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Device %d has no GSD specified as command line. No ruby code will be executed.",pdevdata->m_dwPK_Device);
		}
	}
}
//<-dceag-c19-b->

	/** @brief COMMAND: #19 - Set House Mode */
	/** Set house mode */
		/** @param #5 Value To Assign */
			/** A value from the HouseMode table, or -1=toggle monitor mode */
		/** @param #17 PK_Users */
			/** The user setting the mode.  If this is 0, it will match any user who has permission to set the house mode. */
		/** @param #99 Password */
			/** The password or PIN of the user.  This can be plain text or md5. */
		/** @param #100 PK_DeviceGroup */
			/** DeviceGroups are treated as zones.  If this device group is specified, only the devices in these zones (groups) will be set. */
		/** @param #101 Handling Instructions */
			/** How to handle any sensors that we are trying to arm, but are blocked.  Valid choices are: R-Report, change to a screen on the orbiter reporting this and let the user decide, W-Wait, arm anyway, but wait for the sensors to clear and then arm them, B-Bypass */

void Generic_Serial_Device::CMD_Set_House_Mode(string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions,string &sCMD_Result,Message *pMessage)
//<-dceag-c19-e->
//<-dceag-c350-b->

	/** @brief COMMAND: #350 - Process Incoming Data */
	/** This Internal command is sent to Ruby interpreter when data is availabe on input. Is used only in Generic Serial Devices. */

void Generic_Serial_Device::CMD_Process_Incoming_Data(string &sCMD_Result,Message *pMessage)
//<-dceag-c350-e->
//<-dceag-c351-b->

	/** @brief COMMAND: #351 - Process IDLE */
	/** This Internal command is sent to Ruby interpreter when it is in IDLE state. */

void Generic_Serial_Device::CMD_Process_IDLE(string &sCMD_Result,Message *pMessage)
//<-dceag-c351-e->
//<-dceag-c355-b->

	/** @brief COMMAND: #355 - Process Initialize */
	/** This Internal command is sent to Ruby interpreter when initialize occurs. */

void Generic_Serial_Device::CMD_Process_Initialize(string &sCMD_Result,Message *pMessage)
//<-dceag-c355-e->
//<-dceag-c356-b->

	/** @brief COMMAND: #356 - Process Release */
	/** This Internal command is sent to Ruby interpreter when release occurs. */

void Generic_Serial_Device::CMD_Process_Release(string &sCMD_Result,Message *pMessage)
//<-dceag-c356-e->
//<-dceag-c373-b->

	/** @brief COMMAND: #373 - Private Method Listing */
	/** Used for ruby code mapping where user can add several private helper members. */

void Generic_Serial_Device::CMD_Private_Method_Listing(string &sCMD_Result,Message *pMessage)
//<-dceag-c373-e->
//<-dceag-c384-b->

	/** @brief COMMAND: #384 - Process Receive Command For Child */
	/** Method that will be called when command arrives for child device */

void Generic_Serial_Device::CMD_Process_Receive_Command_For_Child(string &sCMD_Result,Message *pMessage)
//<-dceag-c384-e->
//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void Generic_Serial_Device::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
//<-dceag-c759-b->

	/** @brief COMMAND: #759 - Get Sensors List */
	/** Get the list of sensors */
		/** @param #109 Data String */
			/** [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n */

void Generic_Serial_Device::CMD_Get_Sensors_List(string *sData_String,string &sCMD_Result,Message *pMessage)
//<-dceag-c759-e->
//<-dceag-c84-b->

	/** @brief COMMAND: #84 - Get Video Frame */
	/** Get's a picture from a specified surveilance camera */
		/** @param #19 Data */
			/** The video frame */
		/** @param #20 Format */
			/** Format of the frame */
		/** @param #23 Disable Aspect Lock */
			/** Disable Aspect Ratio */
		/** @param #41 StreamID */
			/** The ID of the stream */
		/** @param #60 Width */
			/** Frame width */
		/** @param #61 Height */
			/** Frame height */

void Generic_Serial_Device::CMD_Get_Video_Frame(string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat,string &sCMD_Result,Message *pMessage)
//<-dceag-c84-e->
//<-dceag-c200-b->

	/** @brief COMMAND: #200 - Move Up */
	/** Move camera up */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_Move_Up(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c200-e->
//<-dceag-c201-b->

	/** @brief COMMAND: #201 - Move Down */
	/** Move camera down */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_Move_Down(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c201-e->
//<-dceag-c202-b->

	/** @brief COMMAND: #202 - Move Left */
	/** Move camera to the left */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_Move_Left(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c202-e->
//<-dceag-c203-b->

	/** @brief COMMAND: #203 - Move Right */
	/** Move camera to the right */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_Move_Right(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c203-e->
//<-dceag-c684-b->

	/** @brief COMMAND: #684 - Zoom In */
	/** Zoom in */

void Generic_Serial_Device::CMD_Zoom_In(string &sCMD_Result,Message *pMessage)
//<-dceag-c684-e->
//<-dceag-c685-b->

	/** @brief COMMAND: #685 - Zoom Out */
	/** Zoom out */

void Generic_Serial_Device::CMD_Zoom_Out(string &sCMD_Result,Message *pMessage)
//<-dceag-c685-e->
//<-dceag-c63-b->

	/** @brief COMMAND: #63 - Skip Fwd - Channel/Track Greater */
	/** The "seek" variant which scans and stops on valid stations. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_Skip_Fwd_ChannelTrack_Greater(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c63-e->
//<-dceag-c64-b->

	/** @brief COMMAND: #64 - Skip Back - Channel/Track Lower */
	/** The "seek" variant which scans and stops on valid stations. */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_Skip_Back_ChannelTrack_Lower(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c64-e->
//<-dceag-c89-b->

	/** @brief COMMAND: #89 - Vol Up */
	/**  */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume up this many times */

void Generic_Serial_Device::CMD_Vol_Up(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c89-e->
//<-dceag-c90-b->

	/** @brief COMMAND: #90 - Vol Down */
	/**  */
		/** @param #72 Repeat Command */
			/** If specified, repeat the volume down this many times. */

void Generic_Serial_Device::CMD_Vol_Down(int iRepeat_Command,string &sCMD_Result,Message *pMessage)
//<-dceag-c90-e->
//<-dceag-c93-b->

	/** @brief COMMAND: #93 - Scan Fwd/Fast Fwd */
	/** The "fine tune" command which increases the frequency 1 notch a time regardless of signal strength. */

void Generic_Serial_Device::CMD_Scan_FwdFast_Fwd(string &sCMD_Result,Message *pMessage)
//<-dceag-c93-e->
//<-dceag-c97-b->

	/** @brief COMMAND: #97 - Mute */
	/**  */

void Generic_Serial_Device::CMD_Mute(string &sCMD_Result,Message *pMessage)
//<-dceag-c97-e->
//<-dceag-c125-b->

	/** @brief COMMAND: #125 - Scan Back/Rewind */
	/** The "fine tune" command which decreases the frequency 1 notch a time regardless of signal strength. */

void Generic_Serial_Device::CMD_Scan_BackRewind(string &sCMD_Result,Message *pMessage)
//<-dceag-c125-e->
//<-dceag-c192-b->

	/** @brief COMMAND: #192 - On */
	/**  */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */
		/** @param #98 PK_Device_Pipes */
			/** Normally when a device is turned on the corresponding "pipes" are enabled by default. if this parameter is blank.  If this parameter is 0, no pipes will be enabled.  This can also be a comma seperated list of devices, meaning only the pipes to those devic */

void Generic_Serial_Device::CMD_On(int iPK_Pipe,string sPK_Device_Pipes,string &sCMD_Result,Message *pMessage)
//<-dceag-c192-e->
//<-dceag-c193-b->

	/** @brief COMMAND: #193 - Off */
	/**  */
		/** @param #97 PK_Pipe */
			/** Normally when a device is turned on all the inputs and outputs are selected automatically.  If this parameter is specified, only the settings along this pipe will be set. */

void Generic_Serial_Device::CMD_Off(int iPK_Pipe,string &sCMD_Result,Message *pMessage)
//<-dceag-c193-e->
//<-dceag-c273-b->

	/** @brief COMMAND: #273 - EIB_Write */
	/** Peforms a WRITE request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */
		/** @param #109 Data String */
			/** EIB Data */
		/** @param #110 DataType */
			/** EIB Data Type:
[1]-switch (1b)
[2]-dimming-control (4b)
[3]-time (3B)
[4]-date (4B)
[5]-value (2B)
[6]-scaling (1B)
[7]-drive control
[9]-float
[10]-16bit counter
[11]-32bit counter
[13]-ASCII character
[14]-8bit counter
[15]-character string */

void Generic_Serial_Device::CMD_EIB_Write(string sAddress,string sData_String,int iDataType,string &sCMD_Result,Message *pMessage)
//<-dceag-c273-e->
//<-dceag-c275-b->

	/** @brief COMMAND: #275 - EIB_Read */
	/** Performs a READ request on the bus */
		/** @param #108 Address */
			/** EIB Group Address */

void Generic_Serial_Device::CMD_EIB_Read(string sAddress,string &sCMD_Result,Message *pMessage)
//<-dceag-c275-e->
//<-dceag-c190-b->

	/** @brief COMMAND: #190 - Enter/Go */
	/** Terminate input of a channel */
		/** @param #41 StreamID */
			/** ID of stream to apply */

void Generic_Serial_Device::CMD_EnterGo(int iStreamID,string &sCMD_Result,Message *pMessage)
//<-dceag-c190-e->
//<-dceag-c204-b->

	/** @brief COMMAND: #204 - 0 */
	/** 0 */

void Generic_Serial_Device::CMD_0(string &sCMD_Result,Message *pMessage)
//<-dceag-c204-e->
//<-dceag-c205-b->

	/** @brief COMMAND: #205 - 1 */
	/** 1 */

void Generic_Serial_Device::CMD_1(string &sCMD_Result,Message *pMessage)
//<-dceag-c205-e->
//<-dceag-c206-b->

	/** @brief COMMAND: #206 - 2 */
	/** 2 */

void Generic_Serial_Device::CMD_2(string &sCMD_Result,Message *pMessage)
//<-dceag-c206-e->
//<-dceag-c207-b->

	/** @brief COMMAND: #207 - 3 */
	/** 3 */

void Generic_Serial_Device::CMD_3(string &sCMD_Result,Message *pMessage)
//<-dceag-c207-e->
//<-dceag-c208-b->

	/** @brief COMMAND: #208 - 4 */
	/** 4 */

void Generic_Serial_Device::CMD_4(string &sCMD_Result,Message *pMessage)
//<-dceag-c208-e->
//<-dceag-c209-b->

	/** @brief COMMAND: #209 - 5 */
	/** 5 */

void Generic_Serial_Device::CMD_5(string &sCMD_Result,Message *pMessage)
//<-dceag-c209-e->
//<-dceag-c210-b->

	/** @brief COMMAND: #210 - 6 */
	/** 6 */

void Generic_Serial_Device::CMD_6(string &sCMD_Result,Message *pMessage)
//<-dceag-c210-e->
//<-dceag-c211-b->

	/** @brief COMMAND: #211 - 7 */
	/** 7 */

void Generic_Serial_Device::CMD_7(string &sCMD_Result,Message *pMessage)
//<-dceag-c211-e->
//<-dceag-c212-b->

	/** @brief COMMAND: #212 - 8 */
	/** 8 */

void Generic_Serial_Device::CMD_8(string &sCMD_Result,Message *pMessage)
//<-dceag-c212-e->
//<-dceag-c213-b->

	/** @brief COMMAND: #213 - 9 */
	/** 9 */

void Generic_Serial_Device::CMD_9(string &sCMD_Result,Message *pMessage)
//<-dceag-c213-e->
//<-dceag-c363-b->

	/** @brief COMMAND: #363 - Back / Clear Entry */
	/** Clear what you've entered when you made a mistake */

void Generic_Serial_Device::CMD_Back_Clear_Entry(string &sCMD_Result,Message *pMessage)
//<-dceag-c363-e->
//<-dceag-c364-b->

	/** @brief COMMAND: #364 - -/--/--- */
	/** Changes between entering single, double or triple digit numbers. */

void Generic_Serial_Device::CMD_(string &sCMD_Result,Message *pMessage)
//<-dceag-c364-e->
//<-dceag-c820-b->

	/** @brief COMMAND: #820 - Assign Return Route */
	/** Assign Return Route between 2 nodes from ZWave network */
		/** @param #239 NodeID */
			/** Source Node ID */
		/** @param #240 DestNodeID */
			/** Destination Node ID */

void Generic_Serial_Device::CMD_Assign_Return_Route(int iNodeID,int iDestNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c820-e->
//<-dceag-c840-b->

	/** @brief COMMAND: #840 - SetWakeUp */
	/** Set the WakeUp value for Z-Wave PIR devices. */
		/** @param #48 Value */
			/** WakeUp timeout value (1-255 min) */
		/** @param #239 NodeID */
			/** Z-Wave node ID of the device. */

void Generic_Serial_Device::CMD_SetWakeUp(int iValue,int iNodeID,string &sCMD_Result,Message *pMessage)
//<-dceag-c840-e->
//<-dceag-c841-b->

	/** @brief COMMAND: #841 - Set Config Param */
	/** Set the configuration parameters for Z-Wave devices (PIR) */
		/** @param #48 Value */
			/** The value of parameter. */
		/** @param #239 NodeID */
			/** Z-Wave node id */
		/** @param #248 Parameter ID */
			/** The configuration parameter ID. */

void Generic_Serial_Device::CMD_Set_Config_Param(int iValue,int iNodeID,int iParameter_ID,string &sCMD_Result,Message *pMessage)
//<-dceag-c841-e->
//<-dceag-c842-b->

	/** @brief COMMAND: #842 - Set Association */
	/** Set the association of Z-Wave nodes into a group. */
		/** @param #239 NodeID */
			/** Z-Wave node ID which associates some nodes in its group. */
		/** @param #249 Group ID */
			/** The group number which includes the associated nodes. */
		/** @param #250 Nodes List */
			/** The list of associated Z-Wave nodes (comma separated). */

void Generic_Serial_Device::CMD_Set_Association(int iNodeID,int iGroup_ID,string sNodes_List,string &sCMD_Result,Message *pMessage)
//<-dceag-c842-e->
//<-dceag-c184-b->

	/** @brief COMMAND: #184 - Set Level */
	/** Sets the brightness of the light.  The value is 0 for off, up to 100 for full brightness. Several devices such as Drapes Switch can accept relative levels (+1 to step forward/-1 to step backword). */
		/** @param #76 Level */
			/** The level to set, as a value between 0 (off) and 100 (full).  It can be preceeded with a - or + indicating a relative value.  +20 means up 20%. */

void Generic_Serial_Device::CMD_Set_Level(string sLevel,string &sCMD_Result,Message *pMessage)
//<-dceag-c184-e->
//<-dceag-c760-b->

	/** @brief COMMAND: #760 - Send Command To Child */
	/** After reporting new child devices, there may be children we want to test, but we haven't done a quick reload router and can't send them messages directly.  This way we can send 'live' messages to children */
		/** @param #10 ID */
			/** The internal ID used for this device--not the Pluto device ID. */
		/** @param #154 PK_Command */
			/** The command to send */
		/** @param #202 Parameters */
			/** Parameters for the command in the format:
PK_CommandParameter|Value|... */

void Generic_Serial_Device::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
//<-dceag-c776-b->

	/** @brief COMMAND: #776 - Reset */
	/** Reset device. */
		/** @param #51 Arguments */
			/** Argument string
NOEMON or CANBUS */

void Generic_Serial_Device::CMD_Reset(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c776-e->
//<-dceag-c788-b->

	/** @brief COMMAND: #788 - StatusReport */
	/** Test comand. Asq a report */
		/** @param #51 Arguments */
			/** Argument string */

void Generic_Serial_Device::CMD_StatusReport(string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c788-e->
