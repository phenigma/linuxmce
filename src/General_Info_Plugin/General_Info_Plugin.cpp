
/*
 General_Info_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

//<-dceag-d-b->
#include "General_Info_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/DatabaseUtils.h"
#include "PlutoUtils/ProcessUtils.h"
#include "../PlutoUtils/md5.h"
#include "BD/PhoneDevice.h"
#include "CreateDevice/CreateDevice.h"
#include "UpdateEntArea/UpdateEntArea.h"

#include "DCERouter.h"
#include "DCE/DeviceData_Router.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Table_Device_QuickStart.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Table_QuickStartTemplate.h"
#include "pluto_main/Table_Device_MRU.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Country.h"
#include "pluto_main/Table_Device_Device_Related.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Text.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Define_FloorplanObjectType.h"

#include "DataGrid.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "Event_Plugin/Event_Plugin.h"
#include "Gen_Devices/AllScreens.h"

#include "Web_DHCP_Query.h"
using namespace nsWeb_DHCP_Query;
using namespace DefaultScenarios;

#ifndef WEB_QUERY_DEBUG
static const string sURL_Base = "http://plutohome.com/getRegisteredDevices.php";
#else
static const string sURL_Base = "http://10.0.0.175/plutohome-com/getRegisteredDevices.php";
#endif

#include "DCEConfig.h"
DCEConfig dceConfig; // Needed by CreateDevice

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
General_Info_Plugin::General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: General_Info_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_GipMutex("GeneralInfo")
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_GipMutex.Init( &m_MutexAttr );

	m_bRerunConfigWhenDone=false;
	m_pDatabase_pluto_main=NULL;
}

//<-dceag-getconfig-b->
bool General_Info_Plugin::GetConfig()
{
	if( !General_Info_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return false;
	}
	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
General_Info_Plugin::~General_Info_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool General_Info_Plugin::Register()
//<-dceag-reg-e->
{
    // Get the datagrid plugin
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pEvent_Plugin=( Event_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Event_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin || !m_pEvent_Plugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::PendingTasks ) )
        , DATAGRID_Pending_Tasks_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::QuickStartApps ) )
        , DATAGRID_Quick_Start_Apps_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::MRUDocuments ) )
        , DATAGRID_MRU_Documents_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
        new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::Rooms ) )
        , DATAGRID_Rooms_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack( this, ( DCEDataGridGeneratorFn )( &General_Info_Plugin::RoomTypes ) )
		, DATAGRID_Room_Types_CONST,PK_DeviceTemplate_get() );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::ChildrenInfo)), 
		DATAGRID_Devices_Children_Of_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::AvailableSerialPorts)), 
		DATAGRID_Available_Serial_Ports_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::LightsTypes)), 
		DATAGRID_LightType_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::InstalledAVDevices)), 
		DATAGRID_Installed_AV_Devices_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::SensorType)), 
		DATAGRID_Sensor_Type_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::BookmarkList)), 
		DATAGRID_Mozilla_Bookmarks_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::TypesOfPhones)), 
		DATAGRID_Types_Of_Mobile_Phones_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::PNPDevices)), 
		DATAGRID_New_PNP_Devices_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::UsersGrid)), 
		DATAGRID_Users_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::CountriesGrid)), 
		DATAGRID_Countries_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&General_Info_Plugin::DevicesOfCategory)), 
		DATAGRID_Devices_Of_Category_CONST,PK_DeviceTemplate_get());
	
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::NewMacAddress ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_New_Mac_Address_Detected_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &General_Info_Plugin::ReportingChildDevices ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Reporting_Child_Devices_CONST );

	return Connect(PK_DeviceTemplate_get()); 
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
void General_Info_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void General_Info_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c68-b->

	/** @brief COMMAND: #68 - Get Device Data */
	/** Return the device data for a device. */
		/** @param #2 PK_Device */
			/** The device which you want data from */
		/** @param #5 Value To Assign */
			/** The value. */
		/** @param #52 PK_DeviceData */
			/** What parameter to get. */
		/** @param #53 UseDefault */
			/** Report the default value, rather than requesting a live value. */

void General_Info_Plugin::CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c68-e->
{
	if( bUseDefault )
	{
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
		if( !pDeviceData_Router )
			*sValue_To_Assign="BAD DEVICE";
		else
		{
			if( pDeviceData_Router->m_mapParameters.find(iPK_DeviceData)==pDeviceData_Router->m_mapParameters.end() )
				*sValue_To_Assign="BAD PARAMETER";
			else
				*sValue_To_Assign=pDeviceData_Router->m_mapParameters[iPK_DeviceData];
		}
		return;
	}
	else
	{
		// TODO -- Send device request
	}
}

//<-dceag-c71-b->

	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

void General_Info_Plugin::CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c71-e->
{
	g_pPlutoLogger->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File: file %s", sFilename.c_str());

	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

	if( c && Length )
		g_pPlutoLogger->Write(LV_FILEREQUEST, "sending file: %s size: %d", sFilename.c_str(),(int) Length);
	else
		g_pPlutoLogger->Write(LV_CRITICAL, "requested missing file: %s", sFilename.c_str());

	*iData_Size = (int) Length;
	*pData = c;

	g_pPlutoLogger->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File: ended for file %s", sFilename.c_str());
}

//<-dceag-c79-b->

	/** @brief COMMAND: #79 - Add Unknown Device */
	/** Adds an unknown device into the database.  These are devices that are not part of the Pluto system. */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

void General_Info_Plugin::CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c79-e->
{
	Row_UnknownDevices *pRow_UnknownDevices = m_pDatabase_pluto_main->UnknownDevices_get()->AddRow();
	pRow_UnknownDevices->MacAddress_set(sMac_address);
	m_pDatabase_pluto_main->UnknownDevices_get()->Commit();
g_pPlutoLogger->Write(LV_STATUS,"uknown device, setting: %d to mac: %s",pRow_UnknownDevices->PK_UnknownDevices_get(),pRow_UnknownDevices->MacAddress_get().c_str());

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, sMac_address, SCREEN_NewMacAddress_CONST);

	SendCommand(CMD_Remove_Screen_From_History_DL);
}
//<-dceag-c239-b->

	/** @brief COMMAND: #239 - Request File And Checksum */
	/** Get the contents of a file from the server and the checksum of the file */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */
		/** @param #91 Checksum */
			/** The checksum of the file */
		/** @param #92 Checksum Only */
			/** If this is true, this command will return only the checksum of the file, Data will be null. */

void General_Info_Plugin::CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage)
//<-dceag-c239-e->
{
	cout << "Command #239 - Request File And Checksum" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #91 - Checksum=" << sChecksum << endl;
	cout << "Parm #92 - Checksum_Only=" << bChecksum_Only << endl; 

	g_pPlutoLogger->Write(LV_FILEREQUEST, "request missing  file: %s", sFilename.c_str());
	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

	if(c==NULL) //file not found
	{
		g_pPlutoLogger->Write(LV_FILEREQUEST, "The requested file '%s' was not found", sFilename.c_str());
		return;
	}

	*iData_Size = (int) Length;
	*pData = c;
    *sChecksum = FileUtils::FileChecksum(*pData, *iData_Size);

	if(*bChecksum_Only)
	{
		*iData_Size = 0;

		if(NULL != *pData)
		{
			delete *pData;
			*pData = NULL;
		}
	}

	g_pPlutoLogger->Write(LV_FILEREQUEST, "General_Info_Plugin::CMD_Request_File_And_Checksum: file %s", sFilename.c_str());
}
//<-dceag-c246-b->

	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

void General_Info_Plugin::CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage)
//<-dceag-c246-e->
{
	Row_Device_DeviceData * pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(iPK_Device, iPK_DeviceData);
	if(pRow_Device_DeviceData != NULL)
	{
		pRow_Device_DeviceData->IK_DeviceData_set(sValue_To_Assign);
		m_pDatabase_pluto_main->Device_DeviceData_get()->Commit();
	}
}

//<-dceag-c247-b->

	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */

void General_Info_Plugin::CMD_Get_Device_State(string &sCMD_Result,Message *pMessage)
//<-dceag-c247-e->
{
}

//<-dceag-c248-b->

	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */

void General_Info_Plugin::CMD_Get_Device_Status(string &sCMD_Result,Message *pMessage)
//<-dceag-c248-e->
{
}
//<-dceag-createinst-b->!
//<-dceag-c272-b->

	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */
		/** @param #21 Force */

void General_Info_Plugin::CMD_Restart_DCERouter(string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c272-e->
{
// temp debugging since this wasn't going through
g_pPlutoLogger->Write(LV_STATUS, "Forwarding reload to router");
	Message *pMessageOut = new Message(pMessage->m_dwPK_Device_From,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_RELOAD,0);
	SendMessageToRouter(pMessageOut);
}
//<-dceag-c322-b->

	/** @brief COMMAND: #322 - Wake Device */
	/** Sends a Wake on LAN to the specified device. */
		/** @param #2 PK_Device */
			/** The device to wake up */

void General_Info_Plugin::CMD_Wake_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c322-e->
{
}

//<-dceag-c323-b->

	/** @brief COMMAND: #323 - Halt Device */
	/** Halts, or suspends, the given device. */
		/** @param #2 PK_Device */
			/** The device to halt */
		/** @param #21 Force */
			/** If Force is not specified this will do a suspend if the device supports suspend/resume, otherwise it will do a halt.  Force:  "H"=halt, "S"=suspend, "D"=Display off, "R"=reboot, "N"=net boot, "V"=hard drive boot */

void General_Info_Plugin::CMD_Halt_Device(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c323-e->
{
	DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
	if( !pDevice )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot halt unknown device %d",iPK_Device);
		return;
	}

	DeviceData_Router *pDevice_AppServer=NULL;
	if( pDevice->m_pDevice_Core )
		pDevice_AppServer = (DeviceData_Router *) ((DeviceData_Impl *) (pDevice->m_pDevice_Core))->FindSelfOrChildWithinCategory( DEVICECATEGORY_App_Server_CONST );

	if( !pDevice_AppServer && pDevice->m_pDevice_MD )
		pDevice_AppServer = (DeviceData_Router *) ((DeviceData_Impl *) (pDevice->m_pDevice_MD))->FindSelfOrChildWithinCategory( DEVICECATEGORY_App_Server_CONST );

	if( !pDevice_AppServer )
		pDevice_AppServer = (DeviceData_Router *) pDevice->FindSelfOrChildWithinCategory( DEVICECATEGORY_App_Server_CONST );

	if( !pDevice_AppServer )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot halt unknown device %d without an app server",iPK_Device);
		return;
	}

	if( sForce=="V" || sForce=="N" )
	{
		SetNetBoot(pDevice,sForce=="N");
		DCE::CMD_Halt_Device CMD_Halt_Device(m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,pDevice->m_dwPK_Device,sForce.c_str());
		SendCommand(CMD_Halt_Device);
	}
	else
	{
		Message *pMessage_Out = new Message(pMessage);
		pMessage_Out->m_dwPK_Device_To = pDevice_AppServer->m_dwPK_Device;
		SendMessageToRouter(pMessage_Out);
	}
}

void General_Info_Plugin::SetNetBoot(DeviceData_Router *pDevice,bool bNetBoot)
{
	if( pDevice->m_sMacAddress.length()<17 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot set boot on device %d with no Mac",pDevice->m_dwPK_Device);
		return;
	}

	string sFile = "/tftpboot/pxelinux.cfg/01-" + StringUtils::Replace(pDevice->m_sMacAddress,":","-");

	g_pPlutoLogger->Write(LV_STATUS,"Setting net boot for file %s to %d",sFile.c_str(),(int) bNetBoot);

	if( bNetBoot )
		StringUtils::Replace(sFile,sFile,"LOCALBOOT 0 #ERNEL ","KERNEL ");
	else
		StringUtils::Replace(sFile,sFile,"KERNEL ","LOCALBOOT 0 #ERNEL ");
}

//<-dceag-c365-b->

	/** @brief COMMAND: #365 - Get Room Description */
	/** Given a room or device ID, returns the description.  If device ID, also returns the room ID. */
		/** @param #2 PK_Device */
			/** The ID of the device.  Specify this or the room ID. */
		/** @param #9 Text */
			/** The description of the room */
		/** @param #57 PK_Room */
			/** The ID of the room.  Specify this or the device ID. */

void General_Info_Plugin::CMD_Get_Room_Description(int iPK_Device,string *sText,int *iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c365-e->
{
	if( !(*iPK_Room ) )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
		if( !pDevice )
		{
			*sText = "Bad Device/Room";
			g_pPlutoLogger->Write(LV_CRITICAL,"Bad Device/room");
			return;
		}
		iPK_Device = pDevice->m_dwPK_Device;
		*iPK_Room = pDevice->m_dwPK_Room;
	}

	Room *pRoom = m_pRouter->m_mapRoom_Find(*iPK_Room);
	if( !pRoom )
	{
		*sText = "Bad Room";
		g_pPlutoLogger->Write(LV_CRITICAL,"Bad room");
		return;
	}

	*sText = pRoom->m_sDescription;
}
//<-dceag-c371-b->

	/** @brief COMMAND: #371 - Is Daytime */
	/** Returns true or false to indicate if it is daytime (ie between sunrise and sunset) */
		/** @param #119 True/False */
			/** Returns true if it is daytime. */

void General_Info_Plugin::CMD_Is_Daytime(bool *bTrueFalse,string &sCMD_Result,Message *pMessage)
//<-dceag-c371-e->
{
	(*bTrueFalse) = m_pEvent_Plugin->IsDaytime();
}

class DataGridTable *General_Info_Plugin::PendingTasks( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	vector<string> vectPendingTasks;

    for(map<int,class Command_Impl *>::const_iterator it=m_pRouter->m_mapPlugIn_get()->begin();it!=m_pRouter->m_mapPlugIn_get()->end();++it)
    {
		Command_Impl *pPlugIn = (*it).second;
		// We don't care about the return code, just what tasks are pending
		pPlugIn->PendingTasks(&vectPendingTasks);
	}

    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

	int RowCount=0;
	for(size_t s=0;s<vectPendingTasks.size();++s)
	{
        pCell = new DataGridCell( vectPendingTasks[s] );
        pDataGrid->SetData( 0, RowCount++, pCell );
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::QuickStartApps( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCellIcon,*pCellText;
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);

	int PK_Device_MD=atoi(Parms.c_str());
	DeviceData_Router *pDevice_MD = m_pRouter->m_mapDeviceData_Router_Find(PK_Device_MD);
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow( PK_Device_MD );

	if( !pRow_Device || !pDevice_MD )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"QuickStartApps - invalid MD %s",Parms.c_str());
		return pDataGrid;
	}

	DeviceData_Router *pDevice_AppServer,*pDevice_Orbiter_OSD;
	GetAppServerAndOsdForMD(pDevice_MD,&pDevice_AppServer,&pDevice_Orbiter_OSD);

	if( !pDevice_AppServer || !pDevice_Orbiter_OSD )
	{
		//m_pOrbiter_Plugin->DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"There are no App Servers/OSD on this media director");
		SCREEN_DialogGenericError SCREEN_DialogGenericError(m_dwPK_Device, pMessage->m_dwPK_Device_From,
			"There are no App Servers/OSD on this media director", "0", "0", "0");
		SendCommand(SCREEN_DialogGenericError);
		return pDataGrid;
	}

	vector<Row_Device_QuickStart *> vectRow_Device_QuickStart;
	list<pair<string, string> > *p_Bookmarks=NULL;
	size_t s=0;
	list<pair<string, string> >::iterator it;
	if( ExtraData )
	{
		p_Bookmarks = (list<pair<string, string> > *) ExtraData;
		it=p_Bookmarks->begin();
		if( it == p_Bookmarks->end() )
			return pDataGrid;
	}
	else
		pRow_Device->Device_QuickStart_FK_Device_getrows(&vectRow_Device_QuickStart);

	LastApplication *pLastApplication = m_mapLastApplication_Find(pDevice_MD->m_dwPK_Device);
	if( pLastApplication && pLastApplication->m_sName.size() )
	{
		int PK_DesignObj_OSD=DESIGNOBJ_generic_app_full_screen_CONST;
		int PK_DesignObj_Remote=DESIGNOBJ_mnuGenericAppController_CONST;
		if( pLastApplication->m_iPK_QuickStartTemplate )
		{
			Row_QuickStartTemplate *pRow_QuickStartTemplate = m_pDatabase_pluto_main->QuickStartTemplate_get()->GetRow(pLastApplication->m_iPK_QuickStartTemplate);
			if( pRow_QuickStartTemplate )
			{
				PK_DesignObj_OSD=pRow_QuickStartTemplate->FK_DesignObj_OSD_get();
				PK_DesignObj_Remote=pRow_QuickStartTemplate->FK_DesignObj_get();;
			}
		}

		string sMessage;

		if( pDevice_Orbiter_OSD->m_dwPK_Device==pMessage->m_dwPK_Device_From )  // We chose this from an OSD -- just goto the app screen
		{
			sMessage = 
				//TODO: replace this with GotoScreen's
				StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pMessage->m_dwPK_Device_From) +
				" 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " " + 
				StringUtils::itos(PK_DesignObj_OSD);
		}
		else  // We chose this from a remote--the remote goes to a remote control screen, and the osd to the app
		{
			sMessage = StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pMessage->m_dwPK_Device_From) +
				" 1 " + StringUtils::itos(COMMAND_Goto_Screen_CONST) + " " + 
					StringUtils::ltos(COMMANDPARAMETER_PK_Screen_CONST) + " " + StringUtils::ltos(SCREEN_GenericAppController_CONST) + 
				+ " & " +
				StringUtils::itos(m_dwPK_Device) + " " + StringUtils::itos(pDevice_Orbiter_OSD->m_dwPK_Device) +
				" 1 " + StringUtils::itos(COMMAND_Goto_DesignObj_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_PK_DesignObj_CONST) + " " + 
				StringUtils::itos(PK_DesignObj_OSD);
		}
				
		DCE::CMD_Show_Object CMD_Show_Object(m_dwPK_Device,pMessage->m_dwPK_Device_From,StringUtils::itos(DESIGNOBJ_butResumeControl_CONST),
			0,"","","1");
		DCE::CMD_Show_Object CMD_Show_Object2(m_dwPK_Device,pMessage->m_dwPK_Device_From,StringUtils::itos(DESIGNOBJ_objExitAppOnOSD_CONST),
			0,"","","1");
		DCE::CMD_Set_Text CMD_Set_Text(m_dwPK_Device,pMessage->m_dwPK_Device_From,"",pLastApplication->m_sName,TEXT_STATUS_CONST);
		DCE::CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device,pMessage->m_dwPK_Device_From,VARIABLE_Misc_Data_1_CONST,sMessage);

		CMD_Show_Object.m_pMessage->m_vectExtraMessages.push_back(CMD_Show_Object2.m_pMessage);
		CMD_Show_Object.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Text.m_pMessage);
		CMD_Show_Object.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable.m_pMessage);
		SendCommand(CMD_Show_Object);
	}

	// This will be the template we use for string applications
	Row_QuickStartTemplate *pRow_QuickStartTemplate = p_Bookmarks ? m_pDatabase_pluto_main->QuickStartTemplate_get()->GetRow(1) : NULL;

	int iRow=0;
	while(true)
	{
		string sDescription,sBinary,sArguments;
		size_t iSize;
		char *pBuffer=NULL;

		if( p_Bookmarks )
		{
			sDescription=it->second;
			sBinary=pRow_QuickStartTemplate ? pRow_QuickStartTemplate->Binary_get() : "/usr/pluto/bin/Mozilla.sh";
			sArguments=StringUtils::itos(pOH_Orbiter && pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_iPK_Users : 0) + "\t" + it->first;
		}
		else
		{
            if(!vectRow_Device_QuickStart.size())
                return pDataGrid;

			Row_Device_QuickStart *pRow_Device_QuickStart = vectRow_Device_QuickStart[s];
			sDescription = pRow_Device_QuickStart->Description_get();
			sBinary = pRow_Device_QuickStart->Binary_get();
			sArguments = pRow_Device_QuickStart->Arguments_get();
			pRow_QuickStartTemplate=pRow_Device_QuickStart->FK_QuickStartTemplate_getrow();
            if( pRow_Device_QuickStart->EK_Picture_get() )
                pBuffer = FileUtils::ReadFileIntoBuffer("/home/mediapics/" + StringUtils::itos(pRow_Device_QuickStart->EK_Picture_get()) + "_tn.jpg",iSize);
		}

		if( pRow_QuickStartTemplate )
		{
			if( !pBuffer )
                pBuffer = FileUtils::ReadFileIntoBuffer("/usr/pluto/orbiter/quickstart/" + StringUtils::itos(pRow_QuickStartTemplate->PK_QuickStartTemplate_get()) + "_tn.jpg",iSize);
		}

		if( sBinary.size()==0 )
		{
			g_pPlutoLogger->Write(LV_WARNING,"QuickStart device with no binary to run");
			continue;
		}

		if( sDescription.size()==0 )
			sDescription = sBinary;

		pCellIcon = new DataGridCell( "", "" );
		pCellText = new DataGridCell( sDescription, "" );
		pCellText->m_Colspan=3;
		pDataGrid->SetData( 0, iRow, pCellIcon );
		pDataGrid->SetData( 1, iRow, pCellText );
        if( pBuffer )
        {
            pCellIcon->m_pGraphicData = pBuffer;
            pCellIcon->m_GraphicLength = iSize;
        }

		pCellIcon->m_pMessage = BuildMessageToSpawnApp(m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From),
			pDevice_MD,pDevice_AppServer,pDevice_Orbiter_OSD,
			sBinary,sArguments,sDescription,(pRow_QuickStartTemplate ? pRow_QuickStartTemplate->PK_QuickStartTemplate_get() : 0));
		pCellText->m_pMessage = new Message(pCellIcon->m_pMessage);

		if( (p_Bookmarks && ++it == p_Bookmarks->end()) || 
			(!p_Bookmarks && ++s>=vectRow_Device_QuickStart.size()) )
				break;
		iRow++;
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::MRUDocuments( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    DataGridTable *pDataGrid = new DataGridTable( );
//    DataGridCell *pCell;


	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::TypesOfPhones( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	vector<Row_DeviceTemplate *> vectRow_DeviceTemplate;
	m_pDatabase_pluto_main->DeviceTemplate_get()->GetRows("FK_DeviceCategory="+StringUtils::itos(DEVICECATEGORY_Mobile_Orbiter_CONST),&vectRow_DeviceTemplate);
	for(size_t s=0;s<vectRow_DeviceTemplate.size();++s)
	{
		Row_DeviceTemplate *pRow_DeviceTemplate = vectRow_DeviceTemplate[s];
		pCell = new DataGridCell(pRow_DeviceTemplate->Description_get(),StringUtils::itos(pRow_DeviceTemplate->PK_DeviceTemplate_get()));
		pDataGrid->SetData(0,s,pCell);
	}
	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::PNPDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	g_pPlutoLogger->Write(LV_WARNING, "General_Info_Plugin::PNPDevices Parms=%s", Parms.c_str());
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;
	string::size_type pos=0;

	// This will be a new background thread
	PhoneDevice pd("", Parms, 0);

	// ask user what device this is (db result)
	vector<Row_DHCPDevice *> vectRow_DHCPDevice;
	m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(StringUtils::i64tos(pd.m_iMacAddress) + ">=Mac_Range_Low AND " + StringUtils::i64tos(pd.m_iMacAddress) + "<=Mac_Range_High",&vectRow_DHCPDevice);
	int iRow=0,iColumn=0;
	int iMaxColumns = atoi( pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str() );
	for(size_t s=0;s<vectRow_DHCPDevice.size();++s)
	{
		pCell = new DataGridCell(vectRow_DHCPDevice[s]->FK_DeviceTemplate_getrow()->Description_get(),StringUtils::itos(vectRow_DHCPDevice[s]->PK_DHCPDevice_get()));
		pDataGrid->SetData(iColumn,iRow,pCell);
		iColumn++;
		if( iColumn>=iMaxColumns )
		{
			iColumn=0;
			iRow++;
		}
	}

	// ask user what device this is (web result)
	mapMacPKDescription::iterator mapit;
	g_pPlutoLogger->Write(LV_STATUS, "General_Info_Plugin::PNPDevices got empty list from database.");
	//Parms = "00:30:59:01:e3:9a";
	g_pPlutoLogger->Write(LV_WARNING, "General_Info_Plugin::PNPDevices Parms=%s", Parms.c_str());
	if (vectRow_DHCPDevice.size() == 0 && (mapit = m_mapMacPKDescription.find(Parms)) != m_mapMacPKDescription.end())
	{
		g_pPlutoLogger->Write(LV_STATUS, "General_Info_Plugin::PNPDevices Using data from the web.");
		
		// second = description, first = PK_Web
		pCell = new DataGridCell(mapit->second.second, StringUtils::itos(- mapit->second.first));
		pDataGrid->SetData(0, 0, pCell);
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::UsersGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 4;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("JOIN Installation_Users ON FK_Users=PK_Users WHERE FK_Installation="
		+ StringUtils::itos(m_pRouter->iPK_Installation_get()) + " ORDER BY UserName",&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
	{
		Row_Users *pRow_Users = vectRow_Users[s];
		pCell = new DataGridCell(pRow_Users->UserName_get(),StringUtils::itos(pRow_Users->PK_Users_get()));
			pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::DevicesOfCategory( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=" + 
		Parms + " ORDER BY Device.Description",&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		Row_Room *pRow_Room = pRow_Device->FK_Room_getrow();

		string sDescription = pRow_Device->Description_get();
		if( pRow_Room )
			sDescription += "(" + pRow_Room->Description_get() + ")";

		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Device->PK_Device_get()));
		pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::CountriesGrid( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
    int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 1;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int PK_Country=0;
	int iRow=0,iCol=0;
	vector<Row_Country *> vectRow_Country;
	m_pDatabase_pluto_main->Country_get()->GetRows("1=1 ORDER BY Description",&vectRow_Country);
	for(size_t s=0;s<vectRow_Country.size();++s)
	{
		Row_Country *pRow_Country = vectRow_Country[s];
		if( !PK_Country && Parms.size() && StringUtils::StartsWith(pRow_Country->Description_get(),Parms,true) )
			PK_Country = pRow_Country->PK_Country_get();

		pCell = new DataGridCell(pRow_Country->Description_get(),StringUtils::itos(pRow_Country->PK_Country_get()));
			pDataGrid->SetData( iCol++, iRow, pCell );
		if( iCol>=iWidth )
		{
			iCol=0;
			iRow++;
		}
	}

	if( PK_Country )
		*sValue_To_Assign = StringUtils::itos(PK_Country);

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::Rooms( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 4;

    DataGridTable *pDataGrid = new DataGridTable( );
    DataGridCell *pCell;

	int iRow=0,iCol=0;
	string sql = "SELECT PK_Room,Description FROM Room WHERE FK_Installation=" + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " ORDER BY Description";
	PlutoSqlResult result;
    MYSQL_ROW row;
	if( mysql_query(m_pDatabase_pluto_main->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_main->m_pMySQL)) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell( row[1], row[0] );
			pDataGrid->SetData( iCol++, iRow, pCell );
			if( iCol>=iWidth )
			{
				iCol=0;
				iRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::RoomTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0, iCol=0;
	string sql = 
		"SELECT PK_RoomType, RoomType.Description, Sum(IF(NOT IFNULL(FK_Installation, 0), 0, 1)) As Count "
		"FROM RoomType LEFT JOIN Room ON FK_RoomType = PK_RoomType "
		"WHERE FK_Installation = " + StringUtils::itos(m_pRouter->iPK_Installation_get()) + " OR FK_Installation IS NULL " 
		"GROUP BY PK_RoomType "
		"ORDER BY Description";
	PlutoSqlResult result;
	MYSQL_ROW row;
	if(mysql_query(m_pDatabase_pluto_main->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_main->m_pMySQL)) )
	{
		while((row=mysql_fetch_row(result.r)))
		{
			string sPK_RoomType = row[0];
			string sDescription = row[1];
			string sNumberOfRoomTypes = row[2];

			pCell = new DataGridCell(sDescription, sPK_RoomType);
			pDataGrid->SetData(0, iRow, pCell);

			pCell = new DataGridCell(sNumberOfRoomTypes, sPK_RoomType);
			pDataGrid->SetData(1, iRow++, pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::ChildrenInfo(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	string::size_type pos=0;
	string sPK_Device_Parent = StringUtils::Tokenize(Parms,",",pos);

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0;
	string sql = 
		"SELECT PK_Device, Device.Description AS Description, Room.Description As Room, DeviceTemplate.Description As ChildType "
		"FROM Device "
		"LEFT JOIN Room ON FK_Room = PK_Room "
		"JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate "
		"WHERE FK_Device_ControlledVia = " + sPK_Device_Parent;
	PlutoSqlResult result;
	MYSQL_ROW row;
	if(mysql_query(m_pDatabase_pluto_main->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_main->m_pMySQL)) )
	{
		while((row=mysql_fetch_row(result.r)))
		{
			string PK_ChildDevice = row[0];
			string sChildName = row[1];
			string sRoomName = row[2] ? row[2] : "";
			string sChildType = row[3];

			pCell = new DataGridCell(sChildName, PK_ChildDevice);
			pDataGrid->SetData(0, iRow, pCell);

			pCell = new DataGridCell(sRoomName, PK_ChildDevice);
			pDataGrid->SetData(1, iRow, pCell);

			pCell = new DataGridCell(sChildType, PK_ChildDevice);
			pDataGrid->SetData(2, iRow++, pCell);
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::AvailableSerialPorts(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 3;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	vector< pair<int,string> > vectAllPorts;
	DatabaseUtils::GetUnusedPortsOnAllPCs(m_pDatabase_pluto_main,vectAllPorts,m_pRouter->iPK_Installation_get());

	int iRow=0, iCol=0;

	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::AvailableSerialPorts - found %d ports",(int) vectAllPorts.size());
	for(size_t s=0;s<vectAllPorts.size();++s)
	{
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(vectAllPorts[s].first);
		if( !pRow_Device )
			continue;

		pCell = new DataGridCell(pRow_Device->Description_get() + " " + DatabaseUtils::HumanReadablePort(m_pDatabase_pluto_main,vectAllPorts[s].first,vectAllPorts[s].second)
			, StringUtils::itos(vectAllPorts[s].first) + "," + vectAllPorts[s].second);
		pDataGrid->SetData(iCol++, iRow, pCell);

		if(iCol >= iWidth)
		{
			iCol = 0;
			iRow++;
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::LightsTypes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 3;

	string::size_type pos=0;
	string sFloorPlanType = StringUtils::Tokenize(Parms,",",pos);

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0,iCol=0;
	string sql = 
		"SELECT PK_FloorplanObjectType, Description "
		"FROM FloorplanObjectType "
		"WHERE FK_FloorplanType = " + sFloorPlanType + " "
		"ORDER BY Description";

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( mysql_query(m_pDatabase_pluto_main->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_main->m_pMySQL)) )
	{
		while( ( row=mysql_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell(row[1], row[0]);
			pDataGrid->SetData( iCol++, iRow, pCell );
			if( iCol>=iWidth )
			{
				iCol=0;
				iRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::InstalledAVDevices(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	int iWidth = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Width_CONST].c_str());
	if( !iWidth )
		iWidth = 2;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	string sPK_DeviceCategory_Parent = StringUtils::ltos(DEVICECATEGORY_AV_CONST);
	int iRow=0,iCol=0;
	string sql = 
		"SELECT DISTINCT D.PK_Device, D.Description FROM DeviceTemplate DT "
		"JOIN DeviceCategory DC1 ON DT.FK_DeviceCategory = DC1.PK_DeviceCategory "
		"LEFT JOIN DeviceCategory DC2 ON DC1.PK_DeviceCategory = DC2.FK_DeviceCategory_Parent "
		"JOIN Device D ON D.FK_DeviceTemplate = DT.PK_DeviceTemplate "
		"WHERE "
			"(DT.FK_DeviceCategory = " + sPK_DeviceCategory_Parent + " OR "
			"DC1.FK_DeviceCategory_Parent = " + sPK_DeviceCategory_Parent + " OR "
			"DC2.FK_DeviceCategory_Parent = " + sPK_DeviceCategory_Parent + ") "
		"AND FK_Device_RouteTo is NULL "  // No embedded devices
		"AND D.FK_Installation = " + StringUtils::itos(m_pRouter->iPK_Installation_get());
	if( Parms.size() )
	{
		string sExcludeCategories = " NOT IN (" + Parms + ")";
		sql += " AND (DT.FK_DeviceCategory IS NULL OR DT.FK_DeviceCategory " + sExcludeCategories + ") " + 
		"AND (DC1.FK_DeviceCategory_Parent IS NULL OR DC1.FK_DeviceCategory_Parent " + sExcludeCategories + ") " + 
		"AND (DC2.FK_DeviceCategory_Parent IS NULL OR DC2.FK_DeviceCategory_Parent " + sExcludeCategories + ") ";
	}

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( mysql_query(m_pDatabase_pluto_main->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_main->m_pMySQL)) )
	{
		while((row = mysql_fetch_row( result.r )))
		{
			pCell = new DataGridCell(row[1], row[0]);
			pDataGrid->SetData(iCol++, iRow, pCell );

			if(iCol >= iWidth)
			{
				iCol = 0;
				iRow++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *General_Info_Plugin::SensorType(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage)
{
	string::size_type pos=0;

	DataGridTable *pDataGrid = new DataGridTable( );
	DataGridCell *pCell;

	int iRow=0;
	string sql = 
		"SELECT * FROM DeviceTemplate WHERE FK_DeviceCategory = " + 
		StringUtils::ltos(DEVICECATEGORY_Security_Device_CONST);

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( mysql_query(m_pDatabase_pluto_main->m_pMySQL,sql.c_str())==0 && (result.r = mysql_store_result(m_pDatabase_pluto_main->m_pMySQL)) )
	{
		while( ( row=mysql_fetch_row( result.r ) ) )
		{
			pCell = new DataGridCell(row[1], row[0]);
			pDataGrid->SetData(0, iRow++, pCell );
		}
	}

	return pDataGrid;
}

//<-dceag-c395-b->

	/** @brief COMMAND: #395 - Check for updates */
	/** Check all PC's in the system to see if there are available updates on any of them by having all AppServer's run /usr/pluto/bin/Config_Device_Changes.sh */

void General_Info_Plugin::CMD_Check_for_updates(string &sCMD_Result,Message *pMessage)
//<-dceag-c395-e->
{
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates");
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);

	if( PendingConfigs() )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Schedule a m_bRerunConfigWhenDone");
		m_bRerunConfigWhenDone=true;
		return;
	}

	ListDeviceData_Router *pListDeviceData_Router = 
		m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_App_Server_CONST);

	g_pPlutoLogger->Write(LV_WARNING,"General plugin checking for updates %p",pListDeviceData_Router);

	if( !pListDeviceData_Router )
		return;

	DeviceData_Router *pDevice_AppServerOnCore=NULL; // We will use this to be sure we don't run 2 app server's
	bool bAlreadyRanOnCore=false;
	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
		if( pDevice->m_pDevice_ControlledVia && pDevice->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Media_Director_CONST) )
		{
			if( pDevice->m_pDevice_Core )
				bAlreadyRanOnCore=true;

			if( !m_mapMediaDirectors_PendingConfig[pDevice->m_pDevice_ControlledVia->m_dwPK_Device] )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice->m_dwPK_Device,"/usr/pluto/bin/Config_Device_Changes.sh","cdc",
					"F\tStartLocalDevice\tNoVideo","",StringUtils::itos(pDevice->m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " +
					StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Check_for_updates_done_CONST),false,false,false);
				string sResponse;
				if( !SendCommand(CMD_Spawn_Application,&sResponse) || sResponse!="OK" )
					g_pPlutoLogger->Write(LV_CRITICAL,"Failed to send spawn application to %d",pDevice->m_dwPK_Device);
				else
					m_mapMediaDirectors_PendingConfig[pDevice->m_pDevice_ControlledVia->m_dwPK_Device]=true;
			}
		}
		else if( pDevice->m_pDevice_ControlledVia && pDevice->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Core_CONST) )
			pDevice_AppServerOnCore = pDevice;
	}

	if( pDevice_AppServerOnCore && !bAlreadyRanOnCore && !m_mapMediaDirectors_PendingConfig[pDevice_AppServerOnCore->m_pDevice_ControlledVia->m_dwPK_Device] )
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_AppServerOnCore->m_dwPK_Device,"/usr/pluto/bin/Config_Device_Changes.sh","cdc",
			"F\tStartLocalDevice\tNoVideo","",StringUtils::itos(pDevice_AppServerOnCore->m_dwPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " +
			StringUtils::itos(MESSAGETYPE_COMMAND) + " " + StringUtils::itos(COMMAND_Check_for_updates_done_CONST),false,false,false);
		string sResponse;
		if( !SendCommand(CMD_Spawn_Application,&sResponse) || sResponse!="OK" )
			g_pPlutoLogger->Write(LV_CRITICAL,"Failed to send spawn application to %d",pDevice_AppServerOnCore->m_dwPK_Device);
		else
			m_mapMediaDirectors_PendingConfig[pDevice_AppServerOnCore->m_pDevice_ControlledVia->m_dwPK_Device]=true;
	}
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates %d pending configs",(int) m_mapMediaDirectors_PendingConfig.size());
	for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
		g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates md %d=%d",it->first,(int) it->second);
#endif
}

//<-dceag-c396-b->

	/** @brief COMMAND: #396 - Check for updates done */
	/** An App Server finished running /usr/pluto/bin/Config_Device_Changes.sh and notifies the g.i. plugin. */

void General_Info_Plugin::CMD_Check_for_updates_done(string &sCMD_Result,Message *pMessage)
//<-dceag-c396-e->
{
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates_done");
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);

	DeviceData_Router *pDevice_AppServer = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From);
	if( !pDevice_AppServer || !pDevice_AppServer->m_pDevice_ControlledVia )
		return; // Shouldn't happen

	m_mapMediaDirectors_PendingConfig[pDevice_AppServer->m_pDevice_ControlledVia->m_dwPK_Device]=false;

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates_done from %d, %d pending configs",pMessage->m_dwPK_Device_From,(int) m_mapMediaDirectors_PendingConfig.size());
	for(map<int,bool>::iterator it=m_mapMediaDirectors_PendingConfig.begin();it!=m_mapMediaDirectors_PendingConfig.end();++it)
		g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Check_for_updates_done md %d=%d",it->first,(int) it->second);
#endif

	if( PendingConfigs() )
		return; // Others are still running

	if( m_bRerunConfigWhenDone )
	{
		m_bRerunConfigWhenDone=false;
		g_pPlutoLogger->Write(LV_STATUS,"New requests came in the meantime.  Rerun again");
		CMD_Check_for_updates();  // Do it again
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Done updating config");
		DoneCheckingForUpdates();
	}
}

class DataGridTable * General_Info_Plugin::BookmarkList(string GridID, string Parms, void * ExtraData,int *iPK_Variable,string *sValue_To_Assign, class Message *pMessage)
{
	string::size_type pos=0;
	StringUtils::Tokenize(Parms,",",pos);  // Skip the MD
	list<pair<string, string> > Bookmarks = GetUserBookmarks(StringUtils::Tokenize(Parms,",",pos));
	return QuickStartApps( GridID, Parms, (void *) &Bookmarks, iPK_Variable, sValue_To_Assign, pMessage );
}

pair<string, string> strpair(string x, string y)
{
	return pair<string, string>(x, y);
}

list<pair<string, string> > General_Info_Plugin::GetUserBookmarks(string sPK_User)
{
    list<pair<string, string> > Bookmarks;
    // the following code reads the Mozilla bookmarks

    g_pPlutoLogger->Write(LV_STATUS,"Reading bookmarks from %s", 
        ("/home/user_" + sPK_User + "/bookmarks.html").c_str());

    size_t Size;
    char *Buffer = FileUtils::ReadFileIntoBuffer("/home/user_" + sPK_User + "/bookmarks.html", Size);
    if( Buffer )
    {
        char * BufferTop = Buffer;
        char *BraceA;
        char *PosInBuffer=Buffer;
        while( (BraceA=strstr(Buffer,"<A")) )
        {
            char *HRef = strstr(BraceA,"HREF");
            if( !HRef )
            {
                Buffer++;  // Skip past this and continue
                continue;
            }

            char *FirstQuote = strchr(HRef,'"');
            if( !FirstQuote )
            {
                Buffer++;  // Skip past this and continue
                continue;
            }

            char *SecondQuote = strchr(FirstQuote+1,'"');
            if( !SecondQuote )
            {
                Buffer++;  // Skip past this and continue
                continue;
            }
            *SecondQuote=0;

            string Link(FirstQuote+1);
            char *LastBrace = strchr(SecondQuote+1,'>');
            if( !LastBrace )
            {
                Buffer++;  // Skip past this and continue
                continue;
            }

            char * EndBraceA = strstr(LastBrace+1, "</A>");
            *EndBraceA = 0;
            string LinkText(LastBrace+1);

            Buffer = EndBraceA+1;
            g_pPlutoLogger->Write(LV_STATUS,"add bookmarks %s / %s",Link.c_str(), LinkText.c_str());
            Bookmarks.push_back(pair<string, string>(Link, LinkText));
        }

        delete[] BufferTop;
    }

    if( Bookmarks.size()==0 )
    {
        Bookmarks.push_back(make_pair<string,string> ("http://dcerouter/pluto-admin","Pluto Admin"));
        Bookmarks.push_back(make_pair<string,string> ("http://dcerouter/support","Pluto Support"));
    }

    return Bookmarks;
}
//<-dceag-c697-b->

	/** @brief COMMAND: #697 - Set Active Application */
	/** Tell General Info Plugin what computing application is running on an MD */
		/** @param #2 PK_Device */
			/** The media director */
		/** @param #50 Name */
			/** The name of the application */
		/** @param #146 PK_QuickStartTemplate */
			/** The quick start template */

void General_Info_Plugin::CMD_Set_Active_Application(int iPK_Device,string sName,int iPK_QuickStartTemplate,string &sCMD_Result,Message *pMessage)
//<-dceag-c697-e->
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	LastApplication *pLastApplication = m_mapLastApplication_Find(iPK_Device);
	if( !pLastApplication )
	{
		pLastApplication = new LastApplication();
		m_mapLastApplication[iPK_Device]=pLastApplication;
	}
	pLastApplication->m_sName=sName;
	pLastApplication->m_iPK_QuickStartTemplate=iPK_QuickStartTemplate;
}

void General_Info_Plugin::GetAppServerAndOsdForMD(DeviceData_Router *pDevice_MD,DeviceData_Router **pDevice_AppServer,DeviceData_Router **pDevice_Orbiter_OSD)
{
	vector<DeviceData_Router *> vectDevice_AppServer;
	pDevice_MD->FindChildrenWithinCategory(DEVICECATEGORY_App_Server_CONST,vectDevice_AppServer);
	if( vectDevice_AppServer.size() )
		*pDevice_AppServer = vectDevice_AppServer[0];
	else
		*pDevice_AppServer = NULL;

	vector<DeviceData_Router *> vectDevice_Orbiter_OSD;
	pDevice_MD->FindChildrenWithinCategory(DEVICECATEGORY_Standard_Orbiter_CONST,vectDevice_Orbiter_OSD);
	if( vectDevice_Orbiter_OSD.size() )
		*pDevice_Orbiter_OSD = vectDevice_Orbiter_OSD[0];
	else
		*pDevice_Orbiter_OSD = NULL;
}

Message *General_Info_Plugin::BuildMessageToSpawnApp(DeviceData_Router *pDevice_OrbiterRequesting,DeviceData_Router *pDevice_MD,
	DeviceData_Router *pDevice_AppServer,DeviceData_Router *pDevice_Orbiter_OSD,
	string sBinary,string sArguments,string sDescription,int PK_QuickStartTemplate)
{
	int PK_DesignObj_OSD=DESIGNOBJ_generic_app_full_screen_CONST;
	int PK_DesignObj_Remote=DESIGNOBJ_mnuGenericAppController_CONST;
	if( PK_QuickStartTemplate )
	{
		Row_QuickStartTemplate *pRow_QuickStartTemplate = m_pDatabase_pluto_main->QuickStartTemplate_get()->GetRow(PK_QuickStartTemplate);
		if( pRow_QuickStartTemplate )
		{
			if( pRow_QuickStartTemplate->FK_DesignObj_get() )
				PK_DesignObj_Remote=pRow_QuickStartTemplate->FK_DesignObj_get();
			if( pRow_QuickStartTemplate->FK_DesignObj_OSD_get() )
				PK_DesignObj_OSD=pRow_QuickStartTemplate->FK_DesignObj_OSD_get();
			if( sBinary.size()==0 )
				sBinary=pRow_QuickStartTemplate->Binary_get();
			if( sArguments.size()==0 )
				sArguments=pRow_QuickStartTemplate->Arguments_get();
			if( sDescription.size()==0 )
				sDescription=pRow_QuickStartTemplate->Description_get();
		}
	}

	string sMessage = "0 " + StringUtils::itos(pDevice_Orbiter_OSD->m_dwPK_Device) + 
		" 1 4 16 " + StringUtils::itos(PK_DesignObj_OSD) + 
		" & 0 " + StringUtils::itos(m_dwPK_Device) + " 1 " + StringUtils::itos(COMMAND_Set_Active_Application_CONST) +
		" " + StringUtils::itos(COMMANDPARAMETER_Name_CONST) + " \"\" " + StringUtils::itos(COMMANDPARAMETER_PK_Device_CONST) + " " +
		StringUtils::itos(pDevice_MD->m_dwPK_Device);

	if( pDevice_OrbiterRequesting && pDevice_Orbiter_OSD->m_dwPK_Device!=pDevice_OrbiterRequesting->m_dwPK_Device )
		sMessage += " & 0 " + StringUtils::itos(pDevice_OrbiterRequesting->m_dwPK_Device ) + 
		" 1 4 16 " + StringUtils::itos(PK_DesignObj_Remote);

	DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_AppServer->m_dwPK_Device,
		sBinary,"generic_app",sArguments,
		sMessage,sMessage,true,false,true);

	if( pDevice_OrbiterRequesting )
	{
		DCE::CMD_Set_Active_Application CMD_Set_Active_Application(pDevice_OrbiterRequesting->m_dwPK_Device,m_dwPK_Device,
			pDevice_MD->m_dwPK_Device,sDescription,PK_QuickStartTemplate);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Active_Application.m_pMessage);
	}

	// If this is the same on screen orbiter on which the app will run, we will send the user 
	// to a screen that retains a small strip at the bottom to terminate the app and return to the orbiter.
	// Otherwise, we will send the OSD to the full screen app, and the orbiter will become a remote control
	if( pDevice_OrbiterRequesting && pDevice_Orbiter_OSD->m_dwPK_Device==pDevice_OrbiterRequesting->m_dwPK_Device )
	{
		// This is the OSD orbiter
		//TODO: replace "mozilla" with custom class name from database
		DCE::CMD_Activate_Window CMD_Activate_Window_(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device, "Firefox-bin");
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Activate_Window_.m_pMessage);

		DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device,0,
			StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj.m_pMessage);

		DCE::CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device,
			VARIABLE_Array_Desc_CONST,sDescription);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable.m_pMessage);
	}
	else
	{
		// Do this on the OSD orbiter
		DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj(m_dwPK_Device,pDevice_Orbiter_OSD->m_dwPK_Device,0,
			StringUtils::itos(PK_DesignObj_OSD),"","",false,false);
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj.m_pMessage);

		// Do this on the controlling orbiter
		if( pDevice_OrbiterRequesting )
		{
			//DCE::CMD_Goto_DesignObj CMD_Goto_DesignObj2(m_dwPK_Device,pDevice_OrbiterRequesting->m_dwPK_Device,0,
			//	StringUtils::itos(PK_DesignObj_Remote),"","",false,false);
			SCREEN_GenericAppController SCREEN_GenericAppController(m_dwPK_Device,pDevice_OrbiterRequesting->m_dwPK_Device);
			//CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_Goto_DesignObj2.m_pMessage);
			CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(SCREEN_GenericAppController.m_pMessage);
		}
	}

	// Turn the MD on
	if( pDevice_MD )
	{
		DCE::CMD_On CMD_On(m_dwPK_Device,pDevice_MD->m_dwPK_Device,0,"");
		CMD_Spawn_Application.m_pMessage->m_vectExtraMessages.push_back(CMD_On.m_pMessage);
	}
	return CMD_Spawn_Application.m_pMessage;
}

bool General_Info_Plugin::ReportingChildDevices( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string sError = pMessage->m_mapParameters[EVENTPARAMETER_Error_Message_CONST];
	if( sError.size() )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"General_Info_Plugin::ReportingChildDevices Device %d failed to report its children: %s",
			pMessage->m_dwPK_Device_From,sError.c_str());
		return true;
	}

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pMessage->m_dwPK_Device_From);
	if( !pRow_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"General_Info_Plugin::ReportingChildDevices Device %d is invalid",
			pMessage->m_dwPK_Device_From);
		return true;
	}

	string sChildren = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
	map<int,bool> mapCurrentChildren;
	vector<string> vectLines;
	StringUtils::Tokenize(sChildren,"\n",vectLines);
	for(size_t s=0;s<vectLines.size();++s)
	{
		// This will add the child device if it doesn't exist
		Row_Device *pRow_Device_Child = ProcessChildDevice(pRow_Device,vectLines[s]);
		if( pRow_Device_Child )
			mapCurrentChildren[pRow_Device_Child->PK_Device_get()]=true;
	}

	// See if any child devices have since disappeared
	string sSQL = "FK_Device_ControlledVia=" + StringUtils::itos(pMessage->m_dwPK_Device_From);
	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows(sSQL,&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device = vectRow_Device[s];
		if( mapCurrentChildren[pRow_Device->PK_Device_get()]==false )
		{
			g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::ReportingChildDevices removing dead device %d %s",
				pRow_Device->PK_Device_get(),pRow_Device->Description_get().c_str());
			CMD_Delete_Device(pRow_Device->PK_Device_get());
		}
	}
	m_pDatabase_pluto_main->Device_get()->Commit();
	m_pDatabase_pluto_main->Device_DeviceData_get()->Commit();

	return true;
}

// It will be like this:
// [internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n

Row_Device *General_Info_Plugin::ProcessChildDevice(Row_Device *pRow_Device,string sLine)
{
	string::size_type pos=0;
	string sInternalID = StringUtils::Tokenize(sLine,"\t",pos);
	string sDescription = StringUtils::Tokenize(sLine,"\t",pos);
	string sRoomName = StringUtils::Tokenize(sLine,"\t",pos);
	int PK_DeviceTemplate = atoi(StringUtils::Tokenize(sLine,"\t",pos).c_str());
	string sPK_FloorplanObjectType = StringUtils::Tokenize(sLine,"\t",pos);
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(PK_DeviceTemplate);
	if( !PK_DeviceTemplate )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice Line %s malformed",sLine.c_str());
		return NULL;
	}

	// Find the child device with this internal id
	vector<Row_Device *> vectRow_Device_Child;
	m_pDatabase_pluto_main->Device_get()->GetRows("JOIN Device_DeviceData ON FK_Device=PK_Device "
		" WHERE FK_Device_ControlledVia=" + StringUtils::itos(pRow_Device->PK_Device_get()) +
		" AND FK_DeviceData=" + StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) +
		" AND IK_DeviceData='" + StringUtils::SQLEscape(sInternalID) + "'",&vectRow_Device_Child);
	
	bool bCreatedNew=false;
	Row_Device *pRow_Device_Child;
	if( vectRow_Device_Child.size() )
	{
		pRow_Device_Child = vectRow_Device_Child[0];
		pRow_Device_Child->Reload();   // Don't overwrite the room or other data that may already be there
		return pRow_Device_Child; // For the time being, don't do anything because it's resetting the device's psc_mod causing orbiter to report the router needs a reload
	}
	else
	{
		// Create it since it doesn't exist
		int iPK_Device;
		bCreatedNew=true;
		CMD_Create_Device(PK_DeviceTemplate,"",0,"",
			StringUtils::itos(DEVICEDATA_PortChannel_Number_CONST) + "|" + sInternalID,
			0,pRow_Device->PK_Device_get(),0,0,&iPK_Device);
		pRow_Device_Child = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
		if( !pRow_Device_Child )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"General_Info_Plugin::ProcessChildDevice failed to create child %d",iPK_Device);
			return NULL;
		}
	}

	// Don't reset the description if it's already there, the user may have overridden the default name
	if( pRow_Device_Child->Description_get().size()==0 )
	{
		if( sDescription.size() )
			pRow_Device_Child->Description_set(sDescription);
		else
			pRow_Device_Child->Description_set(sInternalID);
	}
	else if( bCreatedNew )
	{
		string sOldDescription = pRow_Device_Child->Description_get();
		if( sDescription.size() )
			pRow_Device_Child->Description_set(sOldDescription + " " + sDescription);
		else
			pRow_Device_Child->Description_set(sOldDescription + " " + sInternalID);
	}
	vector<Row_Room *> vectRow_Room;
	m_pDatabase_pluto_main->Room_get()->GetRows("Description like '" + StringUtils::SQLEscape(sRoomName) + "'",&vectRow_Room);
	if( vectRow_Room.size() )
		pRow_Device_Child->FK_Room_set( vectRow_Room[0]->PK_Room_get() );
	else
		g_pPlutoLogger->Write(LV_STATUS,"Device %d %s in unknown room %s",
			pRow_Device_Child->PK_Device_get(),sDescription.c_str(),sRoomName.c_str());

	pRow_Device_Child->FK_DeviceTemplate_set( PK_DeviceTemplate );

	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(pRow_Device_Child->PK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(pRow_Device_Child->PK_Device_get());
		pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_PK_FloorplanObjectType_CONST);
	}

	pRow_Device_DeviceData->IK_DeviceData_set( sPK_FloorplanObjectType );
	return pRow_Device_Child;
}

bool General_Info_Plugin::NewMacAddress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);
	
	string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
	string sIPAddress = pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST];
	if( sMacAddress.size()<11 || sIPAddress.size()<7 )
		return false; // invalid mac address or IP Address

	vector<Row_UnknownDevices *> vectRow_UnknownDevices;
	m_pDatabase_pluto_main->UnknownDevices_get()->GetRows("MacAddress like '%" + sMacAddress + "%'",&vectRow_UnknownDevices);
	if( vectRow_UnknownDevices.size() )
	{
		g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::NewMacAddress %s already unknown",sMacAddress.c_str());
		return false;  // already in the unknown list
	}

	vector<Row_Device *> vectRow_Device;
	m_pDatabase_pluto_main->Device_get()->GetRows("MacAddress like '%" + sMacAddress + "%'",&vectRow_Device);
	if( vectRow_Device.size() )
	{
		g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::NewMacAddress %s already a device",sMacAddress.c_str());
		return false;  // already in the unknown list
	}

	PhoneDevice pd("", sMacAddress, 0);

	vector<Row_DHCPDevice *> vectRow_DHCPDevice;
	m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(StringUtils::i64tos(pd.m_iMacAddress) + ">=Mac_Range_Low AND " + StringUtils::i64tos(pd.m_iMacAddress) + "<=Mac_Range_High",&vectRow_DHCPDevice);
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::NewMacAddress %s has %d candidates",sMacAddress.c_str(),(int) vectRow_DHCPDevice.size());
	if( vectRow_DHCPDevice.size()>0 )
	{ 
		DCE::SCREEN_NewMacAddress_DL SCREEN_NewMacAddress_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(), sMacAddress, sIPAddress);
		SCREEN_NewMacAddress_DL.m_pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST]=sMacAddress;
		SendCommand(SCREEN_NewMacAddress_DL);

		return false;
	}

	g_pPlutoLogger->Write(LV_STATUS, "General_Info_Plugin::NewMacAddress querying web");
	
	Web_DHCP_Query Web_Query(sURL_Base);
	Web_DHCP_Query_Params Web_Params;
	Web_DHCP_Query_Result Web_Result;

	Web_Params["MAC"] = sMacAddress;
	Web_Query.Query(Web_Params, Web_Result);

	if (Web_Result.size() == 0 || Web_Result[0].size() == 0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Empty result from web query");
		return false;
	}
	else if (Web_Result[0][0] == "FAIL")
	{
		if (Web_Result[0].size() >= 2)
			g_pPlutoLogger->Write(LV_WARNING, "Query failed. Server said: %s", Web_Result[0][1].c_str());
		else
			g_pPlutoLogger->Write(LV_WARNING, "Query failed. Server didn't return reason message.");
		return false;
	}
	else if (Web_Result[0].size() < 2)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Query failed. Server returned incomplete answer");
		return false;
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "Query succeeded. Server returned: '%s', '%s'", Web_Result[0][0].c_str(), Web_Result[0][1].c_str());
		
		int iPK_WebQuery = atoi(Web_Result[0][0].c_str());
		string sWeb_Description = Web_Result[0][1];
		if (iPK_WebQuery <= 0)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Invalid PK in result: %d", iPK_WebQuery);
			return false;
		}
		else
		{
			m_mapMacPKDescription[sMacAddress] = pairPKDescription(iPK_WebQuery, sWeb_Description);
			
			DCE::SCREEN_NewMacAddress_DL SCREEN_NewMacAddress_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get(), sMacAddress, sIPAddress);
			SendCommand(SCREEN_NewMacAddress_DL);

			return false;
		}
	}

	return false;
}

struct Web_DeviceData
{
	string m_sMacAddress;
	int m_iPK_DeviceTemplate;
	map<int, string> m_mapDeviceData;
};

//<-dceag-c700-b->

	/** @brief COMMAND: #700 - New Plug and Play Device */
	/** A new pnp device has been added */
		/** @param #47 Mac address */
			/** The Mac Address */
		/** @param #58 IP Address */
			/** The IP Address */
		/** @param #109 Data String */
			/** Extra device data to create the device */
		/** @param #150 PK_DHCPDevice */
			/** The template for the device */

void General_Info_Plugin::CMD_New_Plug_and_Play_Device(string sMac_address,string sIP_Address,string sData_String,int iPK_DHCPDevice,string &sCMD_Result,Message *pMessage)
//<-dceag-c700-e->
{
	PLUTO_SAFETY_LOCK(gm,m_GipMutex);

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_New_Plug_and_Play_Device %d mac %s data %s",
		iPK_DHCPDevice,sMac_address.c_str(),sData_String.c_str());
#endif
	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, sMac_address, SCREEN_NewMacAddress_CONST);
	SendCommand(CMD_Remove_Screen_From_History_DL);

	vector<Web_DeviceData> vectWeb_DeviceData;

	mapMacPKDescription::iterator mapit;
	if (m_mapMacPKDescription.size() != 0 && (mapit = m_mapMacPKDescription.find(sMac_address)) != m_mapMacPKDescription.end())
	{
		Web_DHCP_Query Web_Query(sURL_Base);
		Web_DHCP_Query_Params Web_Params;
		Web_DHCP_Query_Result Web_Result;

		Web_Params["MAC"] = sMac_address;
		Web_Params["PK"] = StringUtils::itos(mapit->second.first); // PK_Web
		Web_Query.Query(Web_Params, Web_Result);

		if (Web_Result.size() == 0 || Web_Result[0].size() == 0)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Empty result from web query");
		}
		else if (Web_Result[0][0] == "FAIL")
		{
			if (Web_Result[0].size() >= 2)
				g_pPlutoLogger->Write(LV_WARNING, "Query failed. Server said: %s", Web_Result[1][1].c_str());
			else
				g_pPlutoLogger->Write(LV_WARNING, "Query failed. Server didn't return reason message.");
		}
		else if (Web_Result.size() < 2)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Server said it has data, but returned 0 records");
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Query succeeded");
			
			for (int i = 1; i < Web_Result.size(); i++)
			{
				if (Web_Result[i].size() % 2 != 0)
				{
					string sMsg;
					for (int k = 0; k < Web_Result[i].size(); k++)
						sMsg += Web_Result[i][k] + ", ";
					g_pPlutoLogger->Write(LV_WARNING, "Received incomplete parameters on line %d (%d): %s", i + 1, Web_Result[i].size(), sMsg.c_str());
					continue;
				}
				
				Web_DeviceData localWeb_DeviceData;
				localWeb_DeviceData.m_iPK_DeviceTemplate = atoi(Web_Result[i][0].c_str());
				localWeb_DeviceData.m_sMacAddress = Web_Result[i][1];

				for (int j = 2; j < Web_Result[i].size(); j++)
				{
					int iPK_DeviceData = atoi(Web_Result[i][j].c_str());
					string sDeviceData = Web_Result[i][j];
					localWeb_DeviceData.m_mapDeviceData[iPK_DeviceData] = sDeviceData;
				}
				
				vectWeb_DeviceData.push_back(localWeb_DeviceData);
			}
		}
	}
	
	int iPK_Device;
	if (iPK_DHCPDevice < 0)
	{
		int iPK_Device_Related = 0;
		for (size_t i = 0; i < vectWeb_DeviceData.size(); i++)
		{
			Web_DeviceData &WDD = vectWeb_DeviceData[i];

			if (i == 0) // 1st device is our PNP device, and all the others will be related to it
			{
				g_pPlutoLogger->Write(LV_STATUS, "Created PNP device %d from MAC %s", iPK_Device, sMac_address.c_str());
				iPK_Device_Related = iPK_Device;
			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS, "Created device %d, related to PNP device %d", iPK_Device, iPK_Device_Related);
			}

			bool bSeparator = false;
			if (sData_String.size() > 0)
			{
				sData_String += "|";
				bSeparator = true;
			}
			
			for (map<int, string>::iterator it = WDD.m_mapDeviceData.begin(); it != WDD.m_mapDeviceData.end(); it++)
			{
				if (bSeparator)
					sData_String += "|";
				sData_String += it->first + "|" + it->second;
			}
			
			CMD_Create_Device(0,sMac_address,i==0 ? -1 : 0 /* only prompt the user for the room for the 1st device */,
				sIP_Address,sData_String,iPK_DHCPDevice,0,pMessage->m_dwPK_Device_From,iPK_Device_Related,&iPK_Device);
		}
		iPK_Device = iPK_Device_Related; // for the Orbiter to display when it asks the user to select a room
	}
	else
	{
		int iPK_Device=0;
		CMD_Create_Device(0,sMac_address,-1,sIP_Address,sData_String,iPK_DHCPDevice,0,pMessage->m_dwPK_Device_From,0,&iPK_Device);
		g_pPlutoLogger->Write(LV_STATUS, "Created PNP device %d from mac %s", iPK_Device, sMac_address.c_str());
	}
}

//<-dceag-c718-b->

	/** @brief COMMAND: #718 - Create Device */
	/** Creates a new device of the given template */
		/** @param #2 PK_Device */
			/** The new device number */
		/** @param #44 PK_DeviceTemplate */
			/** The template */
		/** @param #47 Mac address */
			/** The mac address */
		/** @param #57 PK_Room */
			/** The room for the device.  0=no room, -1=ask user */
		/** @param #58 IP Address */
			/** The IP of the device */
		/** @param #109 Data String */
			/** Extra device data to be assigned when creating the device. It should look like this:
devicedata_id1|devicedata_value1|devicedata_id2|devicedata_value2| etc. */
		/** @param #150 PK_DHCPDevice */
			/** Only needed if this is a dhcp pnp device */
		/** @param #156 PK_Device_ControlledVia */
			/** The controlled via */
		/** @param #198 PK_Orbiter */
			/** The orbiter which should be used to prompt the user for any extra information.  Zero means all orbiters */
		/** @param #201 PK_Device_Related */
			/** Will make the new device relate to this one if possible */

void General_Info_Plugin::CMD_Create_Device(int iPK_DeviceTemplate,string sMac_address,int iPK_Room,string sIP_Address,string sData_String,int iPK_DHCPDevice,int iPK_Device_ControlledVia,int iPK_Orbiter,int iPK_Device_Related,int *iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c718-e->
{
	Row_DHCPDevice *pRow_DHCPDevice = NULL;
	if( iPK_DHCPDevice )
	{
		pRow_DHCPDevice = m_pDatabase_pluto_main->DHCPDevice_get()->GetRow(iPK_DHCPDevice);
		if( !iPK_DeviceTemplate )
			iPK_DeviceTemplate = pRow_DHCPDevice->FK_DeviceTemplate_get();
	}

	if( !iPK_DeviceTemplate )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"General_Info_Plugin::CMD_Create_Device Device Template not specified");
		return;
	}
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device template %d mac %s room %d ip %d data %s",
		iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str());
#endif
	OH_Orbiter *pOH_Orbiter = NULL;
	if( iPK_Orbiter )
		pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(iPK_Orbiter);

	if( !OkayToCreateDevice(iPK_DHCPDevice,iPK_DeviceTemplate,sMac_address,sIP_Address,pOH_Orbiter,sData_String) )
	{
#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device exiting -- not ok to create template %d mac %s room %d ip %d data %s",
			iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str());
#endif
		return;
	}

	CreateDevice createDevice(m_pRouter->iPK_Installation_get(),m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get());
	createDevice.m_bInstallPackagesInBackground = true;
	*iPK_Device = createDevice.DoIt(iPK_DHCPDevice,iPK_DeviceTemplate,sIP_Address,sMac_address,iPK_Device_ControlledVia,sData_String,iPK_Device_Related,iPK_Room > 0 ? iPK_Room : 0);

#ifdef DEBUG
		g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device created %d template %d mac %s room %d ip %d data %s",
			*iPK_Device,iPK_DeviceTemplate,sMac_address.c_str(),iPK_Room,sIP_Address.c_str(),sData_String.c_str());
#endif
	PostCreateDevice(*iPK_Device,iPK_DeviceTemplate,pOH_Orbiter);

	g_pPlutoLogger->Write(LV_STATUS,"Created device %d",*iPK_Device);
	CMD_Check_for_updates();

	UpdateEntArea updateEntArea;
	if( updateEntArea.Connect(m_pData->m_dwPK_Installation,m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		updateEntArea.GetMediaAndRooms();
		updateEntArea.SetEAInRooms();
		updateEntArea.AddDefaultScenarios();
	}

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(*iPK_Device);
	if( pRow_Device )
	{
		if( iPK_Room==-1 )
		{
			g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::CMD_Create_Device adding %d to m_listNewPnpDevicesWaitingForARoom size: %d",
				*iPK_Device,(int) m_listNewPnpDevicesWaitingForARoom.size());
			m_listNewPnpDevicesWaitingForARoom.push_back(*iPK_Device);
			if( iPK_Orbiter )
			{
				DCE::SCREEN_NewPlugAndPlayDevice SCREEN_NewPlugAndPlayDevice(m_dwPK_Device,iPK_Orbiter,*iPK_Device,
					pRow_Device->Description_get(),pRow_Device->FK_DeviceTemplate_getrow()->Comments_get());
				SendCommand(SCREEN_NewPlugAndPlayDevice);
			}
			else
			{
				DCE::SCREEN_NewPlugAndPlayDevice_DL SCREEN_NewPlugAndPlayDevice_DL(m_dwPK_Device,m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters,*iPK_Device,
					pRow_Device->Description_get(),pRow_Device->FK_DeviceTemplate_getrow()->Comments_get());
				SendCommand(SCREEN_NewPlugAndPlayDevice_DL);
			}
		}
		else 
		{
			if( !iPK_Room )
			{
				Row_Device *pRow_Device_ControlledVia = pRow_Device->FK_Device_ControlledVia_getrow();
				if( pRow_Device_ControlledVia )
					iPK_Room = pRow_Device_ControlledVia->FK_Room_get();
				g_pPlutoLogger->Write(LV_WARNING,"Temp - CreateDevice, room was empty, now it's %d %p %d",
					iPK_Room,pRow_Device_ControlledVia,pRow_Device->FK_Device_ControlledVia_get());
			}
			Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(iPK_Room);
			if( pRow_Room )
				SetRoomForDevice(pRow_Device,pRow_Room);
		}
		g_pPlutoLogger->Write(LV_STATUS,"Database reports row as ip %s mac %s",
			pRow_Device->IPaddress_get().c_str(),pRow_Device->MACaddress_get().c_str());
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find %d in database",*iPK_Device);

	Message *pMessage_Event = new Message(m_dwPK_Device,DEVICEID_EVENTMANAGER,PRIORITY_NORMAL,MESSAGETYPE_EVENT,EVENT_New_Device_Created_CONST,
		1,EVENTPARAMETER_PK_Device_CONST,StringUtils::itos(*iPK_Device).c_str());
	QueueMessageToRouter(pMessage_Event);

}

//<-dceag-c719-b->

	/** @brief COMMAND: #719 - Delete Device */
	/** Deletes a device */
		/** @param #2 PK_Device */
			/** The device to delete */

void General_Info_Plugin::CMD_Delete_Device(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c719-e->
{
	// First delete any embedded devices
	string sSQL = "SELECT PK_Device FROM Device where FK_Device_RouteTo=" + StringUtils::itos(iPK_Device);
	PlutoSqlResult result_set;
    MYSQL_ROW row;
	if( (result_set.r=m_pRouter->mysql_query_result(sSQL)) )
		while ((row = mysql_fetch_row(result_set.r)))
			if( row[0] && atoi(row[0]) )
				CMD_Delete_Device( atoi(row[0]) );
	
	m_pDatabase_pluto_main->threaded_mysql_query("UPDATE Device SET FK_Device_ControlledVia=NULL WHERE FK_Device_ControlledVia=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device WHERE PK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM CommandGroup_Command WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_Command WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_CommandGroup WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_DeviceData WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_DeviceGroup WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_Device_Related WHERE FK_Device=" + StringUtils::itos(iPK_Device) + " OR FK_Device_Related=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_EntertainArea WHERE FK_Device=" + StringUtils::itos(iPK_Device));

	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_HouseMode WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_Orbiter WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_StartupScript WHERE FK_Device=" + StringUtils::itos(iPK_Device));

	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_Users WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Package_Device WHERE FK_Device=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM PaidLicense WHERE FK_Device=" + StringUtils::itos(iPK_Device));

	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM Device_Device_Pipe WHERE FK_Device_From=" + StringUtils::itos(iPK_Device) + " OR FK_Device_To=" + StringUtils::itos(iPK_Device));
	m_pDatabase_pluto_main->threaded_mysql_query("DELETE FROM PaidLicense WHERE FK_Device=" + StringUtils::itos(iPK_Device));
}

//<-dceag-c274-b->

	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #50 Name */
			/** If PK_Room is empty, a new room with this name will be created */
		/** @param #57 PK_Room */
			/** The room */

void General_Info_Plugin::CMD_Set_Room_For_Device(int iPK_Device,string sName,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c274-e->
{
    PLUTO_SAFETY_LOCK(mm, m_GipMutex);
	size_t sBefore=m_listNewPnpDevicesWaitingForARoom.size();

	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	Row_Room *pRow_Room;
	if( iPK_Room )
		pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(iPK_Room);
	else
	{
		if( sName.size()==0 )
		{
			//DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"You must type in a name for the room");
			SCREEN_DialogGenericNoButtons SCREEN_DialogGenericNoButtons(m_dwPK_Device, pMessage->m_dwPK_Device_From,
				"You must type in a name for the room", "0", "0", "0");
			SendCommand(SCREEN_DialogGenericNoButtons);

			return;
		}
		pRow_Room = m_pDatabase_pluto_main->Room_get()->AddRow();
		pRow_Room->Description_set(sName);
		pRow_Room->FK_Installation_set(m_pRouter->iPK_Installation_get());
		m_pDatabase_pluto_main->Room_get()->Commit();
	}

	if( !pRow_Device || !pRow_Room )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot set device %d to room %d",iPK_Device,iPK_Room);
		return;
	}
	else
		SetRoomForDevice(pRow_Device,pRow_Room);

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_get(), 
		StringUtils::itos(iPK_Device), SCREEN_NewPlugAndPlayDevice_CONST);
	SendCommand(CMD_Remove_Screen_From_History_DL);
	m_listNewPnpDevicesWaitingForARoom.remove(pRow_Device->PK_Device_get());

	UpdateEntArea updateEntArea;
	if( updateEntArea.Connect(m_pData->m_dwPK_Installation,m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		updateEntArea.GetMediaAndRooms();
		updateEntArea.SetEAInRooms();
		updateEntArea.AddDefaultScenarios();
	}

bool bStillRunningConfig = PendingConfigs();
g_pPlutoLogger->Write(LV_STATUS,"CMD_Set_Room_For_Device: before %d after %d pending %d",
(int) sBefore,(int) m_listNewPnpDevicesWaitingForARoom.size(),(int) bStillRunningConfig);
	// If there pnp devices waiting for the room, and we finished specifying the last one, and we're
	// not still getting the software, let the user know his device is done
	if( sBefore && m_listNewPnpDevicesWaitingForARoom.size()==0 && !bStillRunningConfig )
	{
		if( !m_pOrbiter_Plugin->CheckForNewWizardDevices(NULL) )  // Don't display the 'device is done' if there are still some config settings we need
		{
			//DisplayMessageOnOrbiter("","<%=T" + StringUtils::itos(TEXT_New_Devices_Configured_CONST) + "%>",true);
			SCREEN_DialogGenericNoButtons_DL SCREEN_DialogGenericNoButtons_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(),
				"<%=T" + StringUtils::itos(TEXT_New_Devices_Configured_CONST) + "%>", "1", "0", "0");
			SendCommand(SCREEN_DialogGenericNoButtons_DL);
		}
	}
}

void General_Info_Plugin::DoneCheckingForUpdates()
{
	PLUTO_SAFETY_LOCK(mm, m_GipMutex);

	// We must have started the check for updates because we added a new device.  However we finished
	// getting room info from the user, so he's ready to go
	if( m_listNewPnpDevicesWaitingForARoom.size()==0 && !m_pOrbiter_Plugin->CheckForNewWizardDevices(NULL) )
	{
		//DisplayMessageOnOrbiter("","<%=T" + StringUtils::itos(TEXT_New_Devices_Configured_CONST) + "%>",true);
		SCREEN_DialogGenericNoButtons_DL SCREEN_DialogGenericNoButtons_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(),
			"<%=T" + StringUtils::itos(TEXT_New_Devices_Configured_CONST) + "%>", "1", "0", "0");
		SendCommand(SCREEN_DialogGenericNoButtons_DL);
	}
}

bool General_Info_Plugin::OkayToCreateDevice(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sMac_address,string sIP_Address,OH_Orbiter *pOH_Orbiter,string sData_String)
{
	// See if there is anything special we need to do for this type of device
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);
	if( !pRow_DeviceTemplate )
		return true;

	DeviceCategory *pDeviceCategory = m_pRouter->m_mapDeviceCategory_Find(pRow_DeviceTemplate->FK_DeviceCategory_get());
	if( !pDeviceCategory )
		return true;

	// TODO -- THIS SHOULD BE A 'REGISTER CREATE DEVICE' INTERCEPTOR WHERE YOU REGISTER A DEVICETEMPLATE/CATEGORY, AND A PRE/POST CREATE CALLBACK
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Network_Storage_CONST) )
		return OkayToCreateDevice_NetworkStorage(iPK_DHCPDevice,iPK_DeviceTemplate,sMac_address,sIP_Address,pOH_Orbiter,sData_String);

	return true;
}

bool General_Info_Plugin::OkayToCreateDevice_AlarmPanel(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sMac_address,string sIP_Address,OH_Orbiter *pOH_Orbiter,string sData_String)
{
	return true;
}

void General_Info_Plugin::PostCreateDevice(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter)
{
	// See if there is anything special we need to do for this type of device
	Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);
	if( !pRow_DeviceTemplate )
		return;

	DeviceCategory *pDeviceCategory = m_pRouter->m_mapDeviceCategory_Find(pRow_DeviceTemplate->FK_DeviceCategory_get());
	if( !pDeviceCategory )
		return;

	// TODO -- THIS SHOULD BE A 'REGISTER CREATE DEVICE' INTERCEPTOR WHERE YOU REGISTER A DEVICETEMPLATE/CATEGORY, AND A PRE/POST CREATE CALLBACK
	if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Network_Storage_CONST) )
		PostCreateDevice_NetworkStorage(iPK_Device,iPK_DeviceTemplate,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Surveillance_Cameras_CONST) )
		PostCreateDevice_Cameras(iPK_Device,iPK_DeviceTemplate,pOH_Orbiter);
	else if( pDeviceCategory->WithinCategory(DEVICECATEGORY_Security_Device_CONST) )
		PostCreateSecurityDevice(iPK_Device,iPK_DeviceTemplate,pOH_Orbiter);
}

void General_Info_Plugin::PostCreateDevice_AlarmPanel(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter)
{
}

void General_Info_Plugin::PostCreateSecurityDevice(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter)
{
	int PK_FloorplanObjectType=0;
	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(iPK_Device,DEVICEDATA_PK_FloorplanObjectType_CONST);
	if( pRow_Device_DeviceData )
		PK_FloorplanObjectType = atoi(pRow_Device_DeviceData->IK_DeviceData_get().c_str());

	string sDefaultSecuritySetting;
	if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_DOOR_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Door_Sensor_CONST )
		sDefaultSecuritySetting = "1,0,N1,1,1,2,N1"; // Monitor mode, security alert when armed, announcement when entertaining
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_WINDOW_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Window_Sensor_CONST )
		sDefaultSecuritySetting = "0,0,N1,2,2,0,N1"; // Make an announcement when the user is at home, security breach when away
	else if( iPK_DeviceTemplate==DEVICETEMPLATE_Glass_Break_Sensor_CONST )
		sDefaultSecuritySetting = "0,2,N1,N1,N1,2,N1"; // Always announcement or trigger an alarm
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_MOTION_DETECTOR_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Motion_Detector_CONST )
		sDefaultSecuritySetting = "0,0,N1,0,0,0,N1"; // Only active when away
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_SMOKE_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Smoke_Detector_CONST )
		sDefaultSecuritySetting = "1,N1,N1,N1,N1,N1,N1"; // Always an alarm
	else if( PK_FloorplanObjectType==FLOORPLANOBJECTTYPE_SECURITY_AIRQUALITY_CONST || iPK_DeviceTemplate==DEVICETEMPLATE_Air_Quality_Sensor_CONST )
		sDefaultSecuritySetting = "1,N1,N1,N1,N1,N1,N1"; // Always an alarm

	DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,iPK_Device,DEVICEDATA_Alert_CONST,sDefaultSecuritySetting);
}

bool General_Info_Plugin::OkayToCreateDevice_NetworkStorage(int iPK_DHCPDevice,int iPK_DeviceTemplate,string sMac_address,string sIP_Address,OH_Orbiter *pOH_Orbiter,string sData)
{
	if( StringUtils::StartsWith(sData,StringUtils::itos(DEVICEDATA_Use_Automatically_CONST) + "|") || !pOH_Orbiter )
	{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::OkayToCreateDevice_NetworkStorage already configured %d template %d mac %s data %s",
		iPK_DHCPDevice,iPK_DeviceTemplate,sMac_address.c_str(),sData.c_str());
#endif
		return true;
	}

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::OkayToCreateDevice_NetworkStorage not configured %d template %d mac %s data %s",
		iPK_DHCPDevice,iPK_DeviceTemplate,sMac_address.c_str(),sData.c_str());
#endif

	DCE::SCREEN_NAS_Options SCREEN_NAS_Options(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, 
		iPK_DeviceTemplate,sMac_address, sIP_Address,iPK_DHCPDevice);
	SendCommand(SCREEN_NAS_Options);
	return false;
}

void General_Info_Plugin::PostCreateDevice_NetworkStorage(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::PostCreateDevice_NetworkStorage device  %d template %d",
		iPK_Device,iPK_DeviceTemplate);
#endif
	CMD_Check_Mounts();
}

void General_Info_Plugin::PostCreateDevice_Cameras(int iPK_Device,int iPK_DeviceTemplate,OH_Orbiter *pOH_Orbiter)
{
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_STATUS,"General_Info_Plugin::PostCreateDevice_Cameras device  %d template %d",
		iPK_Device,iPK_DeviceTemplate);
#endif
	string sSQL = "SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory="
		+ StringUtils::itos(DEVICECATEGORY_Security_Device_CONST) + " LIMIT 1";

	PlutoSqlResult result_set;
	if( (result_set.r=m_pDatabase_pluto_main->mysql_query_result(sSQL)) && result_set.r->row_count && pOH_Orbiter )
	{
		DCE::SCREEN_Sensors_Viewed_By_Camera SCREEN_Sensors_Viewed_By_Camera(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
			iPK_Device);
		SendCommand(SCREEN_Sensors_Viewed_By_Camera);
	}
}

//<-dceag-c752-b->

	/** @brief COMMAND: #752 - Check Mounts */
	/** Re-mount all network storage devices */

void General_Info_Plugin::CMD_Check_Mounts(string &sCMD_Result,Message *pMessage)
//<-dceag-c752-e->
{
	ListDeviceData_Router *pListDeviceData_Router = 
		m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_App_Server_CONST);

#ifdef DEBUG
	g_pPlutoLogger->Write(LV_WARNING,"General_Info_Plugin::CMD_Check_Mounts %p",pListDeviceData_Router);
#endif

	if( !pListDeviceData_Router )
		return;

	DeviceData_Router *pDevice_AppServerOnCore=NULL; // We will use this to be sure we don't run 2 app server's
	bool bAlreadyRanOnCore=false;
	for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
	{
		DeviceData_Router *pDevice = *it;
		if( pDevice->m_pDevice_ControlledVia && pDevice->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Media_Director_CONST) )
		{
			if( pDevice->m_pDevice_Core )
				bAlreadyRanOnCore=true;

			if( !m_mapMediaDirectors_PendingConfig[pDevice->m_pDevice_ControlledVia->m_dwPK_Device] )
			{
				DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice->m_dwPK_Device,"/usr/pluto/bin/DoAllMounts.sh","dm",
					"","","",false,false,false);
				SendCommand(CMD_Spawn_Application);
			}
		}
		else if( pDevice->m_pDevice_ControlledVia && pDevice->m_pDevice_ControlledVia->WithinCategory(DEVICECATEGORY_Core_CONST) )
			pDevice_AppServerOnCore = pDevice;
	}

	if( pDevice_AppServerOnCore && !bAlreadyRanOnCore && !m_mapMediaDirectors_PendingConfig[pDevice_AppServerOnCore->m_pDevice_ControlledVia->m_dwPK_Device] )
	{
		DCE::CMD_Spawn_Application CMD_Spawn_Application(m_dwPK_Device,pDevice_AppServerOnCore->m_dwPK_Device,"/usr/pluto/bin/DoAllMounts.sh","dm",
			"","","",false,false,false);
		SendCommand(CMD_Spawn_Application);
	}
}

void General_Info_Plugin::SetRoomForDevice(Row_Device *pRow_Device,Row_Room *pRow_Room)
{
	pRow_Device->Reload();
	g_pPlutoLogger->Write(LV_STATUS,"Setting device %d to with ip %s mac %s to room %d",
		pRow_Device->PK_Device_get(),pRow_Device->IPaddress_get().c_str(),pRow_Device->MACaddress_get().c_str(),pRow_Room->PK_Room_get());
	pRow_Device->FK_Room_set( pRow_Room->PK_Room_get() );
	pRow_Device->Table_Device_get()->Commit();

	vector<Row_Device *> vectRow_Device;
	pRow_Device->Device_FK_Device_ControlledVia_getrows(&vectRow_Device);
	for(size_t s=0;s<vectRow_Device.size();++s)
	{
		Row_Device *pRow_Device_Child = vectRow_Device[s];
		SetRoomForDevice(pRow_Device_Child,pRow_Room);
	}
}
//<-dceag-c765-b->

	/** @brief COMMAND: #765 - Set Device Relations */
	/** Set which devices are related to which */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #103 List PK Device */
			/** The devices it relates to */
		/** @param #205 Reverse */
			/** If true, the device list are the source devices and the PK_Device is the relation */

void General_Info_Plugin::CMD_Set_Device_Relations(int iPK_Device,string sList_PK_Device,bool bReverse,string &sCMD_Result,Message *pMessage)
//<-dceag-c765-e->
{
	string::size_type pos=0;
	while(pos<sList_PK_Device.size())
	{
		int PK_Device,PK_Device_Related;
		if( bReverse )
		{
			PK_Device = atoi(StringUtils::Tokenize(sList_PK_Device,"|",pos).c_str());
			PK_Device_Related = iPK_Device;
		}
		else
		{
			PK_Device_Related = atoi(StringUtils::Tokenize(sList_PK_Device,"|",pos).c_str());
			PK_Device = iPK_Device;
		}
		if( !PK_Device || !PK_Device_Related )
			continue;

		Row_Device_Device_Related *pRow_Device_Device_Related = m_pDatabase_pluto_main->Device_Device_Related_get()->GetRow(PK_Device,PK_Device_Related);
		if( pRow_Device_Device_Related )
			continue;  // It's already there

		pRow_Device_Device_Related = m_pDatabase_pluto_main->Device_Device_Related_get()->AddRow();
		pRow_Device_Device_Related->FK_Device_set(PK_Device);
		pRow_Device_Device_Related->FK_Device_Related_set(PK_Device_Related);
	}
	m_pDatabase_pluto_main->Device_Device_Related_get()->Commit();
}

//<-dceag-c789-b->

	/** @brief COMMAND: #789 - Force Update Packages */
	/** For the system to upgrade */

void General_Info_Plugin::CMD_Force_Update_Packages(string &sCMD_Result,Message *pMessage)
//<-dceag-c789-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Forcing package update");
	ProcessUtils::SpawnApplication("/usr/pluto/bin/ForceUpdates.sh", "", "ForceUpdates", NULL, false);
}

//<-dceag-c790-b->

	/** @brief COMMAND: #790 - Get_iPK_DeviceFromUID */
	/** Get the device ID from the UID from the USB port. */
		/** @param #206 UID */
			/** UID from USB port */

void General_Info_Plugin::CMD_Get_iPK_DeviceFromUID(string sUID,string &sCMD_Result,Message *pMessage)
//<-dceag-c790-e->
{
	sCMD_Result = "";
	string sSQL = 	string("where IK_DeviceData = '") +
					sUID +
					string("' and FK_DeviceData = ") +
					StringUtils::itos(DEVICEDATA_UID_CONST) +
					" limit 1";
	
	vector<class Row_Device_DeviceData*> vectRow_Device_DeviceData;
	vector<class Row_Device_DeviceData*>::iterator vectRow_Device_DeviceData_iterator;
	
	Table_Device_DeviceData *p_Table_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get();
	if( p_Table_Device_DeviceData != NULL )
	{
		p_Table_Device_DeviceData->GetRows(sSQL, &vectRow_Device_DeviceData);
		
		Row_Device_DeviceData *pRow_Device_DeviceData = NULL;
		for(vectRow_Device_DeviceData_iterator = vectRow_Device_DeviceData.begin(); 
				vectRow_Device_DeviceData_iterator != vectRow_Device_DeviceData.end(); 
				++vectRow_Device_DeviceData_iterator)
		{
			pRow_Device_DeviceData = * vectRow_Device_DeviceData_iterator;
			if( pRow_Device_DeviceData != NULL )
			{
				sCMD_Result = StringUtils::itos( pRow_Device_DeviceData->FK_Device_get() );
			}
			// just the first row
			break;
		}
	}
	
#if 0
	string sSQL = "select FK_Device from Device_DeviceData where IK_DeviceData = '" ;
	sSQL += sUID + string("' and FK_DeviceData = ") + StringUtils::itos(DEVICEDATA_UID_CONST) + " limit 1";
	g_pPlutoLogger->Write(LV_DEBUG, "CMD_Get_iPK_DeviceFromUID query: %s", sSQL.c_str());
	
	//got the UID, got the deviceType, now look for the device id and put it in the returnValue
	PlutoSqlResult result_set ;
	
	result_set.r = m_pDatabase_pluto_main->mysql_query_result(sSQL);
	sCMD_Result = "";
	if(result_set.r != NULL)
	{
		MYSQL_ROW row = NULL;
		if( (row = mysql_fetch_row(result_set.r) ) != NULL)
		{
			if(row[0] != NULL)
				sCMD_Result = row[0];
			else
				g_pPlutoLogger->Write(LV_DEBUG, "CMD_Get_iPK_DeviceFromUID no results");
		}	
		else
			g_pPlutoLogger->Write(LV_DEBUG, "CMD_Get_iPK_DeviceFromUID no results");
	}
	else
		g_pPlutoLogger->Write(LV_DEBUG, "CMD_Get_iPK_DeviceFromUID result set NULL");
#endif
}

//<-dceag-c791-b->

	/** @brief COMMAND: #791 - Set Enable Status */
	/** This command will enable or disable a device. A reload router will be needed in order to take effect. */
		/** @param #2 PK_Device */
			/** The id of the device to be enabled/disabled */
		/** @param #208 Enable */
			/** If this is true ( = 1), the device will be enabled. Otherwise, the device will be disabled. */

void General_Info_Plugin::CMD_Set_Enable_Status(int iPK_Device,bool bEnable,string &sCMD_Result,Message *pMessage)
//<-dceag-c791-e->
{
	//get the device row from 'Device' table
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);

	if(NULL != pRow_Device)
	{
		//the device exists; setting 'Disabled' flag
		pRow_Device->Disabled_set(bEnable ? 0 : 1);
		pRow_Device->Table_Device_get()->Commit();
	}
	else
	{
		//no row, no valid device id; are you missing something ? 
		g_pPlutoLogger->Write(LV_WARNING, "Failed to set enable status for device %d: the device doesn't exists",
			iPK_Device);
	}
}//<-dceag-c792-b->

	/** @brief COMMAND: #792 - Get All HAL Model ID */
	/** Returns a list of HAL Model ID-s */

void General_Info_Plugin::CMD_Get_All_HAL_Model_ID(string &sCMD_Result,Message *pMessage)
//<-dceag-c792-e->
{
	sCMD_Result = "";
	string sSQL = string("where FK_DeviceData = ") + StringUtils::itos(DEVICEDATA_HAL_Model_CONST);
	
	vector<Row_DeviceTemplate_DeviceData*> vectRow_DeviceTemplate_DeviceData;
	vector<Row_DeviceTemplate_DeviceData*>::iterator vectRow_DeviceTemplate_DeviceData_iterator;
	
	Table_DeviceTemplate_DeviceData *p_Table_DeviceTemplate_DeviceData = m_pDatabase_pluto_main->DeviceTemplate_DeviceData_get();
	if( p_Table_DeviceTemplate_DeviceData != NULL )
	{
		p_Table_DeviceTemplate_DeviceData->GetRows(sSQL, &vectRow_DeviceTemplate_DeviceData);
		
		bool firstLine = true;
		
		Row_DeviceTemplate_DeviceData *pRow_DeviceTemplate_DeviceData = NULL;
		for(vectRow_DeviceTemplate_DeviceData_iterator = vectRow_DeviceTemplate_DeviceData.begin(); 
				vectRow_DeviceTemplate_DeviceData_iterator != vectRow_DeviceTemplate_DeviceData.end(); 
				++vectRow_DeviceTemplate_DeviceData_iterator)
		{
			pRow_DeviceTemplate_DeviceData = *vectRow_DeviceTemplate_DeviceData_iterator;
			if(pRow_DeviceTemplate_DeviceData != NULL)
			{
				if(!firstLine)
				{
					sCMD_Result += "\n";
				}
				sCMD_Result +=	StringUtils::itos(pRow_DeviceTemplate_DeviceData->FK_DeviceTemplate_get()) +
								" " + 
								pRow_DeviceTemplate_DeviceData->IK_DeviceData_get();
				firstLine = false;
			}
		}
	}
}
