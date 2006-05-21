/*
 PnpQueue

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/DatabaseUtils.h"
#include "PnpQueue.h"
#include "PnpQueueEntry.h"
#include "Plug_And_Play_Plugin.h"
#include "BD/PhoneDevice.h"
#include "General_Info_Plugin/Web_DHCP_Query.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "Gen_Devices/AllScreens.h"
#include "DCERouter.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_DeviceData.h"
using namespace DCE;
using namespace nsWeb_DHCP_Query;

#ifndef WEB_QUERY_DEBUG
static const string sURL_Base = "http://plutohome.com/getRegisteredDevices.php";
#else
static const string sURL_Base = "http://10.0.0.175/plutohome-com/getRegisteredDevices.php";
#endif

void *PnpThread(void *p)
{
	PnpQueue *pPnpQueue = (PnpQueue *) p;
	pPnpQueue->Run();
	return NULL;
}

PnpQueue::PnpQueue(class Plug_And_Play_Plugin *pPlug_And_Play_Plugin)
	: m_Pnp_PreCreateOptions(this,pPlug_And_Play_Plugin)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pDatabase_pluto_main=m_pPlug_And_Play_Plugin->m_pDatabase_pluto_main;

	m_bThreadRunning=true;
	pthread_t pthread_id; 
	if(pthread_create( &pthread_id, NULL, PnpThread, (void*)this) )
	{
		m_bThreadRunning=false;
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot create PNP thread" );
	}
	else
		pthread_detach(pthread_id);
}

PnpQueue::~PnpQueue()
{
	while( m_bThreadRunning )
	{
		pthread_cond_broadcast( &m_pPlug_And_Play_Plugin->m_PnpCond );
		Sleep(5);
	}
}

void PnpQueue::Run()
{
	PLUTO_SAFETY_LOCK(pnp,m_pPlug_And_Play_Plugin->m_PnpMutex);

	bool bOnlyBlockedEntries=false;  // Set this to true if every entry in the list is blocked and we don't need to process anymore until something happens
	while( !m_pPlug_And_Play_Plugin->m_bQuit )
	{
		if( m_mapPnpQueueEntry.size()==0 || bOnlyBlockedEntries )  // bOnlyBlockedEntries will be true if the last loop had only blocked entries
			pnp.TimedCondWait(180,0);  // Release the mutex so other items can be added to the queue and so we can handle incoming events.  Only sleep for 3 minutes in case an Orbiter is blocked and not responding

		bOnlyBlockedEntries = m_mapPnpQueueEntry.size()>0;
		for(map<int,class PnpQueueEntry *>::iterator it=m_mapPnpQueueEntry.begin();it!=m_mapPnpQueueEntry.end();)  // The pnp mutex is held so we can safely do what we like
		{
			PnpQueueEntry *pPnpQueueEntry = it->second;
			if( pPnpQueueEntry->m_EBlockedState != PnpQueueEntry::pnpqe_blocked_none )
			{
				it++;
				continue;
			}
			else
				bOnlyBlockedEntries=false;  // There are some entries that are not blocked, so loop again

			if( Process(pPnpQueueEntry)==true )  // Meaning it needs to be removed from the list
				m_mapPnpQueueEntry.erase( it++ );  // Remove it
			else
				it++;
		}

		Sleep(1000); // In case we're in a polling loop, don't hog all the CPU.  Nothing here is that time critical 
	}
	m_bThreadRunning=false;
}

void PnpQueue::NewEntry(PnpQueueEntry *pPnpQueueEntry)
{
	if( !pPnpQueueEntry->m_pRow_Device_Reported )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"PnpQueue::NewEntry -- don't know who reported this");
		return;
	}
	PLUTO_SAFETY_LOCK(pnp,m_pPlug_And_Play_Plugin->m_PnpMutex);
	m_mapPnpQueueEntry[pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get()]=pPnpQueueEntry;
	pnp.Release();
	pthread_cond_broadcast( &m_pPlug_And_Play_Plugin->m_PnpCond );
}

bool PnpQueue::Process(PnpQueueEntry *pPnpQueueEntry)
{
	switch( pPnpQueueEntry->m_pRow_PnpQueue->Stage_get() )
	{
	case	PNP_DETECT_STAGE_DETECTED:
		return Process_Detect_Stage_Detected(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT:
		return Process_Detect_Stage_Confirm_Possible_DT(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT:
		return Process_Detect_Stage_Prompting_User_For_DT(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT:
		return Process_Detect_Stage_Prompting_User_For_Options(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_ADD_DEVICE:
		return Process_Detect_Stage_Add_Device(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_ADD_SOFTWARE:
		return Process_Detect_Stage_Add_Software(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_START_DEVICE:
		return Process_Detect_Stage_Start_Device(pPnpQueueEntry);
	case	PNP_DETECT_STAGE_DONE:
		return true;

	case PNP_REMOVE_STAGE_REMOVED:
		return Process_Remove_Stage_Removed(pPnpQueueEntry);
	case PNP_REMOVE_STAGE_DONE:
		return true;
	};
	g_pPlutoLogger->Write(LV_CRITICAL,"PnpQueue::Process ID %d is an unprocessable state %d",
		pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->Stage_get());
	return true;  // Remove this because we can't do anything with it
}

bool PnpQueue::Process_Detect_Stage_Detected(PnpQueueEntry *pPnpQueueEntry)
{
	LocateDevice(pPnpQueueEntry);
	Row_Device *pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow() : NULL;  // This will be NULL if it's a new device
	if( pRow_Device_Created )
	{
		pPnpQueueEntry->AssignDeviceData(pRow_Device_Created);
		int PK_Device_Topmost = DatabaseUtils::GetTopMostDevice(m_pDatabase_pluto_main,pRow_Device_Created->PK_Device_get());
		if( PK_Device_Topmost != pPnpQueueEntry->m_dwPK_Device_TopLevel )
		{
			// This has moved from one machine to another.  It should have been disabled back when it was removed from the prior machine
			if( pRow_Device_Created->Disabled_get()==0 )
				g_pPlutoLogger->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Detected for queue %d device %d moved but wasn't disbled",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get());

			int PK_Device_ControlledVia=DatabaseUtils::FindControlledViaCandidate(m_pDatabase_pluto_main,pRow_Device_Created->PK_Device_get(),
				pRow_Device_Created->FK_DeviceTemplate_get(),pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),pPnpQueueEntry->m_pRow_Device_Reported->FK_Installation_get());
			if( !PK_Device_ControlledVia )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Detected for queue %d device %d no valid FindControlledViaCandidate",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get());
				PK_Device_ControlledVia = pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get();
			}

			pRow_Device_Created->FK_Device_ControlledVia_set(PK_Device_ControlledVia);
			pRow_Device_Created->Disabled_set(false);
			m_pDatabase_pluto_main->Device_get()->Commit();
			
			Message *pMessage_Kill = new Message(m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get(),PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_QUIT,0);
			m_pPlug_And_Play_Plugin->QueueMessageToRouter(pMessage_Kill); // Kill the device at the old location
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_ADD_SOFTWARE);
			g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d was existing device, changing controlled via",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			return Process_Detect_Stage_Add_Software(pPnpQueueEntry);
		}
		else if( pRow_Device_Created->Disabled_get()!=0 )
		{
			pRow_Device_Created->Disabled_set(0);
			m_pDatabase_pluto_main->Device_get()->Commit();
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_ADD_SOFTWARE);
			g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d was existing device, but disabled",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			return Process_Detect_Stage_Add_Software(pPnpQueueEntry);
		}
		else
		{
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
			g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d was existing device, nothing to do",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			return true;
		}
	}
	else
	{
		// It's a new device.  Did we get a valid device template already?
		Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow() : NULL;  // This will be NULL if there's no device template
		if( pRow_DeviceTemplate )
		{
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT);
			g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d is new device, already know template",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			return Process_Detect_Stage_Prompting_User_For_Options(pPnpQueueEntry);
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT);
		g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Detected queue %d is new device, processing",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		return Process_Detect_Stage_Confirm_Possible_DT(pPnpQueueEntry);
	}
}

bool PnpQueue::Process_Detect_Stage_Confirm_Possible_DT(PnpQueueEntry *pPnpQueueEntry)
{
	vector<Row_UnknownDevices *> vectRow_UnknownDevices;
	string sSqlWhere;
	if( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("MacAddress like '%") + pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() + "%'";
	if( pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("VendorModelId='") + pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() + "'";
	if( pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get().size() )
		sSqlWhere += (sSqlWhere.size() ? " AND " : "") + string("SerialNumber='") + pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get() + "'";

	if( sSqlWhere.size() )
	{
		m_pDatabase_pluto_main->UnknownDevices_get()->GetRows(sSqlWhere,&vectRow_UnknownDevices);
		if( vectRow_UnknownDevices.size() )
		{
			g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d already unknown",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
			pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
			return true;
		}
	}

	if( pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get().size()>=11 )
	{
		// It's IP based
		PhoneDevice pd("", pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(), 0);
		sSqlWhere = StringUtils::i64tos(pd.m_iMacAddress) + ">=Mac_Range_Low AND " + StringUtils::i64tos(pd.m_iMacAddress) + "<=Mac_Range_High";
	}
	else if( pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get().size() )  // It's usb or similar that has a vendor/model ID
		sSqlWhere = "VendorModelID='" + pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() + "'";
	else // Brute force, like RS232 or similar, where we have to check every device that matches the com method
		sSqlWhere = "JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_CommMethod=" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_CommMethod_get());

	vector<Row_DHCPDevice *> vectRow_DHCPDevice;
	m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sSqlWhere,&vectRow_DHCPDevice);
	g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d has %d candidates",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) vectRow_DHCPDevice.size());
	if( vectRow_DHCPDevice.size()>0 )
	{
		g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d could be %d devices",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),(int) vectRow_DHCPDevice.size());
		for(vector<Row_DHCPDevice *>::iterator it=vectRow_DHCPDevice.begin();it!=vectRow_DHCPDevice.end();++it)
		{
			Row_DHCPDevice *pRow_DHCPDevice = *it;
			pPnpQueueEntry->m_mapPK_DHCPDevice_possible[pRow_DHCPDevice->PK_DHCPDevice_get()]=pRow_DHCPDevice;  // This is a possibility
		}
	}
	
	if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==0 && CheckForDeviceTemplateOnWeb(pPnpQueueEntry) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "PnpQueue::Process_Detect_Stage_Confirm_Possible_DT queue %d identified on web",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT);
		return Process_Detect_Stage_Prompting_User_For_Options(pPnpQueueEntry);
	}

	RunPnpDetectionScript(pPnpQueueEntry);

	return Process_Detect_Stage_Prompting_User_For_DT(pPnpQueueEntry);
}

/*
struct Web_DeviceData
{
	string m_sMacAddress;
	int m_iPK_DeviceTemplate;
	map<int, string> m_mapDeviceData;
};
*/
bool PnpQueue::CheckForDeviceTemplateOnWeb(PnpQueueEntry *pPnpQueueEntry)
{
	/*
	string sMacAddress=pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get();
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
*/
	return false;
}

bool PnpQueue::Process_Detect_Stage_Prompting_User_For_DT(PnpQueueEntry *pPnpQueueEntry)
{
	if( pPnpQueueEntry->m_iPK_DHCPDevice ) // See if the user already picked this from the menu
	{
		Row_DHCPDevice *pRow_DHCPDevice = m_pDatabase_pluto_main->DHCPDevice_get()->GetRow(pPnpQueueEntry->m_iPK_DHCPDevice);
		if( pRow_DHCPDevice ) // Should always be true
			pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_set( pRow_DHCPDevice->FK_DeviceTemplate_get() );
		else
			pPnpQueueEntry->m_iPK_DHCPDevice=0;  // Something went really wrong
	}

	Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow() : NULL;  // This will be NULL if there's no device template
	if( pRow_DeviceTemplate )  // We know what it is, ask the user for options
	{
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT);
		return Process_Detect_Stage_Prompting_User_For_Options(pPnpQueueEntry);
	}

	if( pPnpQueueEntry->m_mapPK_DHCPDevice_possible.size()==0 )  // We have no possibilities.  We'll have to skip it
	{
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);
		return true;
	}

	if( pPnpQueueEntry->m_EBlockedState == PnpQueueEntry::pnpqe_blocked_prompting_device_template && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
		return false; // We're waiting for user input.  Give the user more time.

	pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_prompting_device_template);
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT);
	DCE::SCREEN_NewPnpDevice_DL SCREEN_NewPnpDevice_DL(m_pPlug_And_Play_Plugin->m_dwPK_Device, m_pPlug_And_Play_Plugin->m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(), pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
	m_pPlug_And_Play_Plugin->SendCommand(SCREEN_NewPnpDevice_DL);
	return false;  // Now we wait
}

bool PnpQueue::Process_Detect_Stage_Prompting_User_For_Options(PnpQueueEntry *pPnpQueueEntry)
{
	if( pPnpQueueEntry->m_EBlockedState == PnpQueueEntry::pnpqe_blocked_prompting_options && time(NULL)-pPnpQueueEntry->m_tTimeBlocked<TIMEOUT_PROMPTING_USER )
		return false; // We're waiting for user input.  Give the user more time.

	if( m_Pnp_PreCreateOptions.OkayToCreateDevice(pPnpQueueEntry)==false )  // See if the user needs to specify some options
		return false;  // The user needs to specify some options

	// We're good to go, let's create the device
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_ADD_DEVICE);
	return Process_Detect_Stage_Add_Device(pPnpQueueEntry);
}

bool PnpQueue::Process_Detect_Stage_Add_Device(PnpQueueEntry *pPnpQueueEntry)
{
	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	Row_DeviceTemplate *pRow_DeviceTemplate = pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_getrow();
	if( !pRow_DeviceTemplate || !pCommand_Impl_GIP ) // They must all be here
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Add_Device something went wrong for queue %d (%p)!",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),pCommand_Impl_GIP);
		return true; // Delete this, something went terribly wrong
	}

	int iPK_Device=0;
	DCE::CMD_Create_Device CMD_Create_Device( m_pPlug_And_Play_Plugin->m_dwPK_Device, pCommand_Impl_GIP->m_dwPK_Device, 
		pRow_DeviceTemplate->PK_DeviceTemplate_get(), pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get(), -1, pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get(),
		pPnpQueueEntry->DeviceDataAsString(),pPnpQueueEntry->m_iPK_DHCPDevice,0 /* let it find the parent based on the relationship */,
		pPnpQueueEntry->m_pOH_Orbiter ? pPnpQueueEntry->m_pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device : 0,
		pPnpQueueEntry->m_pRow_Device_Reported->PK_Device_get(),&iPK_Device);

	m_pPlug_And_Play_Plugin->SendCommand(CMD_Create_Device);
	if( !iPK_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"PnpQueue::Process_Detect_Stage_Add_Device couldn't create device queue %d",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		return true; // Delete this, something went terribly wrong
	}
	pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CreateDevice already adds the software and starts it, so we're done
	return true; 
}

bool PnpQueue::Process_Detect_Stage_Add_Software(PnpQueueEntry *pPnpQueueEntry)
{
	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP )
	{
		string sResponse;
		DCE::CMD_Check_for_updates CMD_Check_for_updates(m_pPlug_And_Play_Plugin->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device);
		if( !m_pPlug_And_Play_Plugin->SendCommand(CMD_Check_for_updates,&sResponse) )
		{
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_installing_software);
			return false;  // This will cause us to block for 3 minutes
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CMD_Check_for_updates starts any devices, so no need to do anything here
	}

	return true;
}

bool PnpQueue::Process_Detect_Stage_Start_Device(PnpQueueEntry *pPnpQueueEntry)
{
	Command_Impl *pCommand_Impl_GIP = m_pPlug_And_Play_Plugin->m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_General_Info_Plugin_CONST);
	if( pCommand_Impl_GIP )
	{
		string sResponse;
		// Check for updates will also start any devices
		DCE::CMD_Check_for_updates CMD_Check_for_updates(m_pPlug_And_Play_Plugin->m_dwPK_Device,pCommand_Impl_GIP->m_dwPK_Device);
		if( !m_pPlug_And_Play_Plugin->SendCommand(CMD_Check_for_updates,&sResponse) )
		{
			pPnpQueueEntry->Block(PnpQueueEntry::pnpqe_blocked_installing_software);
			return false;  // This will cause us to block for 3 minutes
		}
		pPnpQueueEntry->Stage_set(PNP_DETECT_STAGE_DONE);   // CMD_Check_for_updates starts any devices, so no need to do anything here
	}

	return true;
}

bool PnpQueue::Process_Remove_Stage_Removed(PnpQueueEntry *pPnpQueueEntry)
{
	LocateDevice(pPnpQueueEntry);
	Row_Device *pRow_Device_Created = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_getrow() : NULL;  // This will be NULL if it's a new device
	if( pRow_Device_Created )
	{
		pRow_Device_Created->Disabled_set(1);
		m_pDatabase_pluto_main->Device_get()->Commit();
		Message *pMessage_Kill = new Message(m_pPlug_And_Play_Plugin->m_dwPK_Device,pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get(),PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_QUIT,0);
		m_pPlug_And_Play_Plugin->QueueMessageToRouter(pMessage_Kill); // Kill the device at the old location
	}
	pPnpQueueEntry->Stage_set(PNP_REMOVE_STAGE_DONE);
	return true;
}

bool PnpQueue::LocateDevice(PnpQueueEntry *pPnpQueueEntry)
{
	Row_Device *pRow_Device = pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ? m_pDatabase_pluto_main->Device_get()->GetRow( pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_get() ) : NULL;
	if( pRow_Device )
		return true;  // This device already exists

	bool bFirstWhere=true;
	vector<Row_Device *> vectRow_Device;
	string sPK_Device_TopLevel = StringUtils::itos(pPnpQueueEntry->m_dwPK_Device_TopLevel);
	string sSql_Primary = "JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate "
		"LEFT JOIN Device AS P1 ON Device.FK_Device_ControlledVia = P1.PK_Device "
		"LEFT JOIN Device AS P2 ON P1.FK_Device_ControlledVia = P2.PK_Device "
		"LEFT JOIN DHCPDevice ON DHCPDevice.FK_DeviceTemplate=PK_DeviceTemplate "
		"LEFT JOIN Device_DeviceData As SerialNumber ON SerialNumber.FK_Device=Device.PK_Device AND SerialNumber.FK_DeviceData=" TOSTRING(DEVICEDATA_Serial_Number_CONST) " "
		"LEFT JOIN Device_DeviceData As ComPort ON ComPort.FK_Device=Device.PK_Device AND ComPort.FK_DeviceData=" TOSTRING(DEVICEDATA_COM_Port_on_PC_CONST) " "
		"LEFT JOIN DeviceTemplate_DeviceData As OnePerPC ON OnePerPC.FK_DeviceTemplate=PK_DeviceTemplate AND OnePerPC.FK_DeviceData=" TOSTRING(DEVICEDATA_Only_One_Per_PC_CONST) " "
		"WHERE (Device.FK_Device_ControlledVia=" + sPK_Device_TopLevel + " OR P1.FK_Device_ControlledVia=" + sPK_Device_TopLevel +
		" OR P2.FK_Device_ControlledVia=" + sPK_Device_TopLevel + ")";


	string sSql_Model,sSql_SerialNumber;

	if( pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get() )
		sSql_Model += " AND Device.FK_DeviceTemplate=" + StringUtils::itos(pPnpQueueEntry->m_pRow_PnpQueue->FK_DeviceTemplate_get());

	string sMacAddress=pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get();
	if( sMacAddress.size() )
		sSql_Model += " AND MacAddress like '%" + sMacAddress + "%'";

	string sVendorModelId = pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get();
	if( sVendorModelId.size() )
		sSql_Model += " AND VendorModelId='" + sVendorModelId + "'";

	string sSerialNumber = pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get();
	if( sSerialNumber.size() )
		sSql_SerialNumber += " AND SerialNumber.IK_DeviceData='" + sSerialNumber + "'";

	if( pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)!=pPnpQueueEntry->m_mapPK_DeviceData.end() )
		// This may be a serial device.  If we already have a device on this port we will use it
		sSql_SerialNumber += " AND (ComPort.IK_DeviceData='" + pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST] + "' OR Device.Disabled=1)";

	if( sSql_Model.size()==0 )
	{
		g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::LocateDevice queue %d has no identifying attributes",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get());
		return false;
	}
	
	// See if this exact item with the same serial number exists already
	m_pDatabase_pluto_main->Device_get()->GetRows(sSql_Primary + sSql_Model + sSql_SerialNumber,&vectRow_Device);
	if( vectRow_Device.size() )
	{
		pRow_Device = vectRow_Device[0];
		pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
		g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::LocateDevice( queue %d mac:%s already a device",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sMacAddress.c_str());
		return true;
	}

	// Nope.  See if there is another similar item, perhaps a different unit (serial #), but the 'one per pc' is set.  If so, it's a match since we can only have 1 per pc anyway
	string sOnePerPc = " AND OnePerPC.IK_DeviceData IS NOT NULL AND OnePerPC.IK_DeviceData=1";
	m_pDatabase_pluto_main->Device_get()->GetRows(sSql_Primary + sSql_Model + sOnePerPc,&vectRow_Device);
	if( vectRow_Device.size() )
	{
		pRow_Device = vectRow_Device[0];
		pPnpQueueEntry->m_pRow_PnpQueue->FK_Device_Created_set(pRow_Device->PK_Device_get());
		g_pPlutoLogger->Write(LV_STATUS,"PnpQueue::LocateDevice( queue %d mac:%s already a device",pPnpQueueEntry->m_pRow_PnpQueue->PK_PnpQueue_get(),sMacAddress.c_str());
		return true;
	}

	return false;
}

void PnpQueue::RunPnpDetectionScript(PnpQueueEntry *pPnpQueueEntry)
{
}

