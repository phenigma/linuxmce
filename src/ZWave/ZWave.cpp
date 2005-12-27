//<-dceag-d-b->
#include "ZWave.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/MySQLHelper.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
ZWave::ZWave(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: ZWave_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_ZWaveMutex("zwave")
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_ZWaveMutex.Init( &m_MutexAttr );

	m_pPlainClientSocket=NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
ZWave::~ZWave()
//<-dceag-dest-e->
{
	
}

//<-dceag-getconfig-b->
bool ZWave::GetConfig()
{
	if( !ZWave_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	if( !ConfirmConnection() )
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot connect to windows zwave");
	CMD_Report_Child_Devices();
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool ZWave::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void ZWave::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);

	int NodeID = atoi(pDeviceData_Impl->m_mapParameters_Find(DEVICEDATA_PortChannel_Number_CONST).c_str());
	if( !NodeID )
	{
		sCMD_Result = "BAD CHILD";
		g_pPlutoLogger->Write(LV_CRITICAL,"Child device doesn't have a zwave node id");
		return;
	}
	if( !ConfirmConnection() )
	{
		sCMD_Result = "NO ZWAVE";
		return;
	}

	sCMD_Result = "OK";
	if( pMessage->m_dwID==COMMAND_Generic_On_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending ON");
		string sOutput = "ON " + StringUtils::itos(NodeID);
		m_pPlainClientSocket->SendString(sOutput);
		string sResponse;
		bool bResult = m_pPlainClientSocket->ReceiveString(sResponse,20);
		g_pPlutoLogger->Write(LV_STATUS,"Got response %d-%s",(int) bResult,sResponse.c_str());
		if( sResponse!="OK " + sOutput )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Expected response <%s> but got <%s>",("OK " + sOutput).c_str(),sResponse.c_str());
			sCMD_Result = "DEVICE DIDN'T RESPOND";
			return;
		}
		return;
	}
	else if( pMessage->m_dwID==COMMAND_Generic_Off_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending Off");
		string sOutput = "OFF " + StringUtils::itos(NodeID);
		m_pPlainClientSocket->SendString(sOutput);
		string sResponse;
		bool bResult = m_pPlainClientSocket->ReceiveString(sResponse,20);
		g_pPlutoLogger->Write(LV_STATUS,"Got response %d-%s",(int) bResult,sResponse.c_str());
		if( sResponse!="OK " + sOutput )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Expected response <%s> but got <%s>",("OK " + sOutput).c_str(),sResponse.c_str());
			sCMD_Result = "DEVICE DIDN'T RESPOND";
			return;
		}

		return;
	}
	else if( pMessage->m_dwID==COMMAND_Set_Level_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending LEVEL");
		string sOutput = "LEVEL" + pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST] + " " + StringUtils::itos(NodeID);
		m_pPlainClientSocket->SendString(sOutput);
		string sResponse;
		bool bResult = m_pPlainClientSocket->ReceiveString(sResponse,20);
		g_pPlutoLogger->Write(LV_STATUS,"Got response %d-%s",(int) bResult,sResponse.c_str());
		if( sResponse!="OK " + sOutput )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Expected response <%s> but got <%s>",("OK " + sOutput).c_str(),sResponse.c_str());
			sCMD_Result = "DEVICE DIDN'T RESPOND";
			return;
		}
		return;
	}
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void ZWave::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

void *DoDownloadConfiguration(void *p)
{
	ZWave *pZWave = (ZWave *) p;
	pZWave->DownloadConfiguration();
	return NULL;
}

void ZWave::DownloadConfiguration()
{
//EVENT_Download_Config_Done("");
//return;
	for(int iRetries=0;;++iRetries)
	{
		PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
		g_pPlutoLogger->Write(LV_STATUS,"TZWave::ReportChildDevices trying to get list of devices");
		if( !ConfirmConnection() )
		{
			g_pPlutoLogger->Write(LV_WARNING,"Cannot connect to ZWave %d",iRetries);
			if( iRetries>4 )
			{
				EVENT_Download_Config_Done("Cannot connect to ZWave");
				return;
			}
			zm.Release();
			Sleep(1000);
			continue;
		}

		m_pPlainClientSocket->SendString("RECEIVECONFIG");
		string sResponse;
		if( !m_pPlainClientSocket->ReceiveString(sResponse,120) || sResponse!="OK" )
		{
			g_pPlutoLogger->Write(LV_WARNING,"ZWave::ReportChildDevices Cannot receive string %d-%s",iRetries,sResponse.c_str());
			if( iRetries>4 )
			{
				EVENT_Download_Config_Done("Device didn't respond: " + sResponse);
				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot get list of devices");
				return;
			}
			zm.Release();
			Sleep(1000);
			continue;
		}

		g_pPlutoLogger->Write(LV_STATUS,"ZWave::ReportChildDevices got %s",sResponse.c_str());
		EVENT_Download_Config_Done("");
		return;
	}
}

void ZWave::ReportChildDevices()
{
	/*
EVENT_Reporting_Child_Devices("",
"1\tNode 1\t\t37\t9\n"
"2\tNode 2\t\t38\t10\n"
"3\tNode 3\t\t37\t11\n"
);
return;
*/
	for(int iRetries=0;;++iRetries)
	{
		PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
		g_pPlutoLogger->Write(LV_STATUS,"TZWave::ReportChildDevices trying to get list of devices");
		if( !ConfirmConnection() )
		{
			g_pPlutoLogger->Write(LV_WARNING,"Cannot connect to ZWave %d",iRetries);
			if( iRetries>4 )
			{
				EVENT_Reporting_Child_Devices("Cannot connect to ZWave","");
				return;
			}
			zm.Release();
			Sleep(1000);
			continue;
		}

		m_pPlainClientSocket->SendString("DEVICES");
		string sResponse;
		if( !m_pPlainClientSocket->ReceiveString(sResponse,30) )
		{
			g_pPlutoLogger->Write(LV_WARNING,"ZWave::ReportChildDevices Cannot receive string %d-%s",iRetries,sResponse.c_str());
			if( iRetries>4 )
			{
				EVENT_Reporting_Child_Devices("Device didn't respond: " + sResponse,"");
				g_pPlutoLogger->Write(LV_CRITICAL,"Cannot get list of devices");
				return;
			}
			zm.Release();
			Sleep(1000);
			continue;
		}

		StringUtils::Replace(&sResponse,"|","\n");
		g_pPlutoLogger->Write(LV_STATUS,"ZWave::ReportChildDevices got %s",sResponse.c_str());
		EVENT_Reporting_Child_Devices("",sResponse);
		return;
	}
}

bool ZWave::ConfirmConnection(int RetryCount)
{
//return true;
	PLUTO_SAFETY_LOCK(zm,m_ZWaveMutex);
	if( !m_pPlainClientSocket )
		m_pPlainClientSocket = new PlainClientSocket(DATA_Get_Remote_Phone_IP() + ":3999");

	if( m_pPlainClientSocket->m_Socket!=INVALID_SOCKET )
	{
		m_pPlainClientSocket->SendString("PING");
		string sResponse;
		bool bResult = m_pPlainClientSocket->ReceiveString(sResponse);
		g_pPlutoLogger->Write(LV_STATUS,"Sent PING 1 got %d-%s",(int) bResult,sResponse.c_str());
		if( sResponse=="PONG" )
			return true;

		m_pPlainClientSocket->Close();
		delete m_pPlainClientSocket;
		m_pPlainClientSocket = new PlainClientSocket(DATA_Get_Remote_Phone_IP() + ":3999");
	}

	if( !m_pPlainClientSocket->Connect() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to connect");
		if( RetryCount<3 )
		{
			Sleep(500);
			return ConfirmConnection(RetryCount+1);
		}

		return false;
	}
	m_pPlainClientSocket->SendString("PING");
	string sResponse;
	bool bResult=m_pPlainClientSocket->ReceiveString(sResponse,10);
	g_pPlutoLogger->Write(LV_STATUS,"Sent PING 2 got %d-%s",(int) bResult,sResponse.c_str());
	if( sResponse=="PONG" ) 
		return true;
	if( RetryCount<3 )
	{
		Sleep(500);
		return ConfirmConnection(RetryCount+1);
	}

	return false;
}

void *DoReportChildDevices(void *p)
{
	ZWave *pZWave = (ZWave *) p;
	pZWave->ReportChildDevices();
	return NULL;
}

//<-dceag-c756-b->

	/** @brief COMMAND: #756 - Report Child Devices */
	/** Report all the child sensors this has by firing an event 'Reporting Child Devices' */

void ZWave::CMD_Report_Child_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c756-e->
{
	pthread_t t;
	pthread_create(&t, NULL, DoReportChildDevices, (void*)this);
}
//<-dceag-c757-b->

	/** @brief COMMAND: #757 - Download Configuration */
	/** Download new configuration data for this device */
		/** @param #9 Text */
			/** Any information the device may want to do the download */

void ZWave::CMD_Download_Configuration(string sText,string &sCMD_Result,Message *pMessage)
//<-dceag-c757-e->
{
	pthread_t t;
	pthread_create(&t, NULL, DoDownloadConfiguration, (void*)this);
}
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

void ZWave::CMD_Send_Command_To_Child(string sID,int iPK_Command,string sParameters,string &sCMD_Result,Message *pMessage)
//<-dceag-c760-e->
{
	if( !ConfirmConnection() )
	{
		sCMD_Result = "NO ZWAVE";
		return;
	}

	sCMD_Result = "OK";
	if( iPK_Command==COMMAND_Generic_On_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending ON");
		string sOutput = "ON " + sID;
		m_pPlainClientSocket->SendString(sOutput);
		string sResponse;
		bool bResult = m_pPlainClientSocket->ReceiveString(sResponse,20);
		g_pPlutoLogger->Write(LV_STATUS,"Got response %d-%s",(int) bResult,sResponse.c_str());
		if( sResponse!="OK " + sOutput )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Expected response <%s> but got <%s>",("OK " + sOutput).c_str(),sResponse.c_str());
			sCMD_Result = "DEVICE DIDN'T RESPOND";
			return;
		}
		return;
	}
	else if( iPK_Command==COMMAND_Generic_Off_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending Off");
		string sOutput = "OFF " + sID;
		m_pPlainClientSocket->SendString(sOutput);
		string sResponse;
		bool bResult = m_pPlainClientSocket->ReceiveString(sResponse,20);
		g_pPlutoLogger->Write(LV_STATUS,"Got response %d-%s",(int) bResult,sResponse.c_str());
		if( sResponse!="OK " + sOutput )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Expected response <%s> but got <%s>",("OK " + sOutput).c_str(),sResponse.c_str());
			sCMD_Result = "DEVICE DIDN'T RESPOND";
			return;
		}
		return;
	}
}
