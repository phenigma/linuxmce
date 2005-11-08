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
{
	m_pPlainClientSocket=NULL;
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
ZWave::ZWave(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: ZWave_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
{
}

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
	SyncDeviceList();
	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool ZWave::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
ZWave_Command *Create_ZWave(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new ZWave(pPrimaryDeviceCommand, pData, pEvent, pRouter);
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
void ZWave::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
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
		m_pPlainClientSocket->SendString("ON " + StringUtils::itos(NodeID));
		return;
	}
	else if( pMessage->m_dwID==COMMAND_Generic_Off_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending Off");
		m_pPlainClientSocket->SendString("OFF " + StringUtils::itos(NodeID));
		return;
	}
	else if( pMessage->m_dwID==COMMAND_Set_Level_CONST )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Sending LEVEL");
		m_pPlainClientSocket->SendString("LEVEL" + pMessage->m_mapParameters[COMMANDPARAMETER_Level_CONST] + " " + StringUtils::itos(NodeID));
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



//<-dceag-c709-b->

	/** @brief COMMAND: #709 - Discover New Devices */
	/** Scans for new devices and adds them to the system */

void ZWave::CMD_Discover_New_Devices(string &sCMD_Result,Message *pMessage)
//<-dceag-c709-e->
{
	if( !ConfirmConnection() )
		return;

	m_pPlainClientSocket->SendString("RECEIVECONFIG");
	string sResponse;
	if( !m_pPlainClientSocket->ReceiveString(sResponse,60) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot receive config");
		return;
	}
	if( sResponse!="OK" )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"ZWave failed to receive config: %s",sResponse.c_str());
		return;
	}
	SyncDeviceList();
	Message *pMessage_Out = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_RELOAD,0);
	QueueMessageToRouter(pMessage_Out);
}

void ZWave::SyncDeviceList()
{
	if( !ConfirmConnection() )
		return;

	m_pPlainClientSocket->SendString("DEVICES");
	string sResponse;
	if( !m_pPlainClientSocket->ReceiveString(sResponse,30) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot get list of devices");
		return;
	}

	DeviceData_Base *pDevice_GeneralInfoPlugin = m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_General_Info_Plugins_CONST);
	if( !pDevice_GeneralInfoPlugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot do this without a general info plugin");
		return;
	}

	MySqlHelper mySqlHelper(m_sIPAddress,"root","","pluto_main");
	if( !mySqlHelper.m_bConnected )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot connect to MySql database");
		return;
	}

	map<int,pair<int,int> > mapNodeID;
	string sSQL = "SELECT PK_Device,FK_DeviceTemplate,IK_DeviceData FROM Device "
		"LEFT JOIN Device_DeviceData ON FK_Device=PK_Device "
		"WHERE FK_Device_ControlledVia=" + StringUtils::itos(m_dwPK_Device) + " AND (FK_DeviceData IS NULL OR FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + ")";

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( ( result.r=mySqlHelper.mysql_query_result( sSQL ) ) )
	{
		while( row=mysql_fetch_row( result.r ) )
		{
			if( row[2] )
				mapNodeID[atoi(row[2])] = make_pair<int,int> (atoi(row[0]),atoi(row[1]));
		}
	}

	string::size_type pos=0;
	while(pos<sResponse.size() && pos!=string::npos)
	{
		int NodeID = atoi(StringUtils::Tokenize(sResponse,",",pos).c_str());
		int PK_DeviceTemplate = atoi(StringUtils::Tokenize(sResponse,",",pos).c_str());
		map<int,pair<int,int> >::iterator itNode = mapNodeID.find(NodeID);
		if( itNode==mapNodeID.end() || itNode->second.second!=PK_DeviceTemplate )  // It either isn't in the database, or it changed types
		{
			if( itNode!=mapNodeID.end() )
			{
				int PK_Device = itNode->second.first;
        		DCE::CMD_Delete_Device CMD_Delete_Device(m_dwPK_Device,pDevice_GeneralInfoPlugin->m_dwPK_Device,PK_Device);
				SendCommand(CMD_Delete_Device);
				mapNodeID.erase(itNode);  // We found it
			}
			int PK_Device=0;
			DCE::CMD_Create_Device CMD_Create_Device(m_dwPK_Device,pDevice_GeneralInfoPlugin->m_dwPK_Device,
				PK_DeviceTemplate,"","",0,m_dwPK_Device,&PK_Device);
			if( SendCommand(CMD_Create_Device) && PK_Device )
			{
				if( mySqlHelper.threaded_mysql_query("UPDATE Device_DeviceData SET IK_DeviceData='" + StringUtils::itos(NodeID) +
					"' WHERE FK_Device=" + StringUtils::itos(PK_Device) + " AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST))!=1 )
				{
					if( mySqlHelper.threaded_mysql_query("INSERT INTO Device_DeviceData(FK_Device,FK_DeviceData,IK_DeviceData) VALUES(" +
						StringUtils::itos(PK_Device) + "," + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + ",'" + StringUtils::itos(NodeID) + "';")!=1 )
							g_pPlutoLogger->Write(LV_CRITICAL,"Unable to set device data for %d",PK_Device);
				}
				mySqlHelper.threaded_mysql_query("UPDATE Device SET Description='Node " + StringUtils::itos(NodeID) + "' WHERE PK_Device=" + StringUtils::itos(PK_Device));
			}
			else
				g_pPlutoLogger->Write(LV_CRITICAL,"Failed to create device");
		}
		else
			mapNodeID.erase(itNode);  // We found it
	}

	// These are what's left that are devices that no longer exist
	for( map<int,pair<int,int> >::iterator itNode=mapNodeID.begin();itNode!=mapNodeID.end();++itNode )
	{
		int PK_Device = itNode->second.first;
        DCE::CMD_Delete_Device CMD_Delete_Device(m_dwPK_Device,pDevice_GeneralInfoPlugin->m_dwPK_Device,PK_Device);
		SendCommand(CMD_Delete_Device);
	}
}

bool ZWave::ConfirmConnection()
{
	if( !m_pPlainClientSocket )
		m_pPlainClientSocket = new PlainClientSocket(DATA_Get_Remote_Phone_IP() + ":3999");

	if( m_pPlainClientSocket->m_Socket!=INVALID_SOCKET )
	{
		m_pPlainClientSocket->SendString("PING");
		string sResponse;
		m_pPlainClientSocket->ReceiveString(sResponse);
		g_pPlutoLogger->Write(LV_STATUS,"Sent PING 1 got %s",sResponse.c_str());
		if( sResponse=="PONG" )
			return true;

		m_pPlainClientSocket->Close();
		delete m_pPlainClientSocket;
		m_pPlainClientSocket = new PlainClientSocket(DATA_Get_Remote_Phone_IP() + ":3999");
	}

	if( !m_pPlainClientSocket->Connect() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Failed to connect");
		return false;
	}
	m_pPlainClientSocket->SendString("PING");
	string sResponse;
	m_pPlainClientSocket->ReceiveString(sResponse,10);
	g_pPlutoLogger->Write(LV_STATUS,"Sent PING 2 got %s",sResponse.c_str());
	return sResponse=="PONG";
}
