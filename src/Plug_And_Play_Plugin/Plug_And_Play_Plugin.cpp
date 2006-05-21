//<-dceag-d-b->
#include "Plug_And_Play_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DCERouter.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_PnpQueue.h"
#include "PnpQueueEntry.h"
#include "PnpQueue.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DataGrid.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Plug_And_Play_Plugin::Plug_And_Play_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Plug_And_Play_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_PnpMutex("pnp")
{
	m_pDatabase_pluto_main=NULL;
	m_pPnpQueue=NULL;

	pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	pthread_cond_init(&m_PnpCond, NULL);
	m_PnpMutex.Init(&m_MutexAttr,&m_PnpCond);
}

//<-dceag-dest-b->
Plug_And_Play_Plugin::~Plug_And_Play_Plugin()
//<-dceag-dest-e->
{
	delete m_pPnpQueue;
}

//<-dceag-getconfig-b->
bool Plug_And_Play_Plugin::GetConfig()
{
	if( !Plug_And_Play_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

    m_pDatabase_pluto_main = new Database_pluto_main( );
    if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit=true;
        return false;
    }

	m_pPnpQueue = new PnpQueue(this);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Plug_And_Play_Plugin::Register()
//<-dceag-reg-e->
{
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	if( !m_pOrbiter_Plugin || !m_pDatagrid_Plugin )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
		return false;
	}

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Plug_And_Play_Plugin::DeviceDetected ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_Detected_CONST );
    RegisterMsgInterceptor( ( MessageInterceptorFn )( &Plug_And_Play_Plugin::DeviceRemoved ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Device_Removed_CONST );

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this, (DCEDataGridGeneratorFn) (&Plug_And_Play_Plugin::PNPDevices)), 
		DATAGRID_New_PNP_Devices_CONST,PK_DeviceTemplate_get());

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
void Plug_And_Play_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Plug_And_Play_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

/*

	COMMANDS TO IMPLEMENT

*/

bool Plug_And_Play_Plugin::DeviceDetected( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PnpQueueEntry *pPnpQueueEntry = new PnpQueueEntry(m_pDatabase_pluto_main,
		pMessage->m_mapParameters[EVENTPARAMETER_DeviceData_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST],
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_CommMethod_CONST].c_str()),
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceTemplate_CONST].c_str()),
		pMessage->m_dwPK_Device_From,
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_PnpProtocol_CONST].c_str()),
		pMessage->m_mapParameters[EVENTPARAMETER_PNP_Serial_Number_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_VendorModelID_CONST]);

	m_pPnpQueue->NewEntry(pPnpQueueEntry);

	return true;
}

bool Plug_And_Play_Plugin::DeviceRemoved( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	PnpQueueEntry *pPnpQueueEntry = new PnpQueueEntry(m_pDatabase_pluto_main,
		pMessage->m_mapParameters[EVENTPARAMETER_DeviceData_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_IP_Address_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST],
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_CommMethod_CONST].c_str()),
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str()),
		pMessage->m_dwPK_Device_From,
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_DeviceTemplate_CONST].c_str()),
		atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_PnpProtocol_CONST].c_str()),
		pMessage->m_mapParameters[EVENTPARAMETER_PNP_Serial_Number_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST],
		pMessage->m_mapParameters[EVENTPARAMETER_VendorModelID_CONST]);

	m_pPnpQueue->NewEntry(pPnpQueueEntry);

	return true;
}

//<-dceag-const2-b->!
//<-dceag-createinst-b->!
//<-dceag-sample-b->!
//<-dceag-c700-b->

	/** @brief COMMAND: #700 - Choose Pnp Device Template */
	/** We have chosen a new pnp device template */
		/** @param #150 PK_DHCPDevice */
			/** The template for the device */
		/** @param #224 PK_PnpQueue */
			/** The queue entry we're selecting for */

void Plug_And_Play_Plugin::CMD_Choose_Pnp_Device_Template(int iPK_DHCPDevice,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c700-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "PnpQueue::PickDeviceTemplate queue %d is invalid", iPK_PnpQueue);
		return;
	}

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL(
		m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters, StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()), SCREEN_NewPnpDevice_CONST);
    SendCommand(CMD_Remove_Screen_From_History_DL);

	pPnpQueueEntry->m_pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	pPnpQueueEntry->m_iPK_DHCPDevice = iPK_DHCPDevice;
	pthread_cond_broadcast( &m_PnpCond );
}

//<-dceag-c804-b->

	/** @brief COMMAND: #804 - Set Pnp Options */
	/** Set options for a pnp device that will soon be created */
		/** @param #5 Value To Assign */
			/** The value to assign to it */
		/** @param #52 PK_DeviceData */
			/** The device data to assign */
		/** @param #224 PK_PnpQueue */
			/** The pnp entry */

void Plug_And_Play_Plugin::CMD_Set_Pnp_Options(string sValue_To_Assign,int iPK_DeviceData,int iPK_PnpQueue,string &sCMD_Result,Message *pMessage)
//<-dceag-c804-e->
{
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(iPK_PnpQueue);
	if( !pPnpQueueEntry )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "PnpQueue::CMD_Set_Pnp_Options queue %d is invalid", iPK_PnpQueue);
		return;
	}
	pPnpQueueEntry->m_pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
	pPnpQueueEntry->m_EBlockedState=PnpQueueEntry::pnpqe_blocked_none;
	pPnpQueueEntry->m_mapPK_DeviceData[iPK_DeviceData] = sValue_To_Assign;
	pthread_cond_broadcast( &m_PnpCond );
}

class DataGridTable *Plug_And_Play_Plugin::PNPDevices( string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, class Message *pMessage )
{	
	DataGridTable *pDataGrid = new DataGridTable( );
	PLUTO_SAFETY_LOCK(pnp,m_PnpMutex);
	PnpQueueEntry *pPnpQueueEntry = m_pPnpQueue->m_mapPnpQueueEntry_Find(atoi(Parms.c_str()));
	if( !pPnpQueueEntry )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Plug_And_Play_Plugin::PNPDevices Parms=%s is invalid", Parms.c_str());
		return pDataGrid;
	}

	int RowCount=0;
	DataGridCell *pCell;
	for( map<int,Row_DHCPDevice *>::iterator it=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.begin();it!=pPnpQueueEntry->m_mapPK_DHCPDevice_possible.end();++it )
	{
		Row_DHCPDevice *pRow_DHCPDevice = it->second;
		Row_DeviceTemplate *pRow_DeviceTemplate = pRow_DHCPDevice->FK_DeviceTemplate_getrow();
		if( !pRow_DeviceTemplate )
			continue;  // Shouldn't happen

		pCell = new DataGridCell( pRow_DeviceTemplate->Description_get(), StringUtils::itos(pRow_DHCPDevice->PK_DHCPDevice_get()) );
        pDataGrid->SetData( 0, RowCount++, pCell );
	}

	return pDataGrid;
}



/*
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
			
			CMD_Create_Device(0,sMac_address,i==0 ? -1 : 0, // only prompt the user for the room for the 1st device 
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
*/
