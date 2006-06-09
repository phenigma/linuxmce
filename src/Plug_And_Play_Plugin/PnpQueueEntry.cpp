/*
 PnpQueueEntry

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
#include "PnpQueueEntry.h"
#include "PnpQueue.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Define_DeviceData.h"
#include "Plug_And_Play_Plugin.h"

using namespace DCE;

// Constructor for device detected
PnpQueueEntry::PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,
	Database_pluto_main *pDatabase_pluto_main,
	string sDeviceData,
	string sIPAddress,
	string sMacAddress,
	string sCategory,
	int PK_CommMethod,
	int PK_DeviceTemplate,
	int PK_Device_Reported,
	int PK_PnpProtocol,
	string sPnpSerialNumber,
	string sText,
	string sVendorModelId)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pDatabase_pluto_main=pDatabase_pluto_main;
	m_pRow_PnpQueue = pDatabase_pluto_main->PnpQueue_get()->AddRow();
	m_pRow_PnpQueue->DetectedDate_set(StringUtils::SQLDateTime(time(NULL)));
	m_pRow_PnpQueue->IPaddress_set(sIPAddress);
	m_pRow_PnpQueue->MACaddress_set(sMacAddress);
	m_pRow_PnpQueue->Category_set(sCategory);
	m_pRow_PnpQueue->VendorModelId_set(sVendorModelId);
	m_pRow_PnpQueue->Path_set("");
	m_pRow_PnpQueue->SerialNumber_set(sPnpSerialNumber);
	m_pRow_PnpQueue->FK_CommMethod_set(PK_CommMethod);
	m_pRow_PnpQueue->FK_PnpProtocol_set(PK_PnpProtocol);
	m_pRow_PnpQueue->FK_DeviceTemplate_set(PK_DeviceTemplate);
	m_pRow_PnpQueue->FK_Device_Created_set(0);
	m_pRow_PnpQueue->FK_Device_Reported_set(PK_Device_Reported);
	m_pRow_PnpQueue->Removed_set(0);
	m_pRow_PnpQueue->Stage_set(PNP_DETECT_STAGE_DETECTED);
	m_pRow_PnpQueue->Parms_set(sDeviceData);
	m_pRow_PnpQueue->Processed_set(0);
	pDatabase_pluto_main->PnpQueue_get()->Commit();
	m_sText=sText;
	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_dwPK_PnpQueue_BlockingFor=m_iPK_DHCPDevice=0;
	m_pOH_Orbiter=NULL;
	ParseDeviceData(sDeviceData);
	FindTopLevelDevice();
}

// Constructor for device removed
PnpQueueEntry::PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,
	Database_pluto_main *pDatabase_pluto_main,
	string sDeviceData,
	string sIPAddress,
	string sMacAddress,
	int PK_CommMethod,
	int PK_Device_Created,
	int PK_Device_Reported,
	int PK_DeviceTemplate,
	int PK_PnpProtocol,
	string sPnpSerialNumber,
	string sText,
	string sVendorModelId)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pDatabase_pluto_main=pDatabase_pluto_main;
	m_pRow_PnpQueue = pDatabase_pluto_main->PnpQueue_get()->AddRow();
	m_pRow_PnpQueue->DetectedDate_set(StringUtils::SQLDateTime(time(NULL)));
	m_pRow_PnpQueue->IPaddress_set(sIPAddress);
	m_pRow_PnpQueue->MACaddress_set(sMacAddress);
	m_pRow_PnpQueue->VendorModelId_set(sVendorModelId);
	m_pRow_PnpQueue->Path_set("");
	m_pRow_PnpQueue->SerialNumber_set(sPnpSerialNumber);
	m_pRow_PnpQueue->FK_CommMethod_set(PK_CommMethod);
	m_pRow_PnpQueue->FK_PnpProtocol_set(PK_PnpProtocol);
	m_pRow_PnpQueue->FK_DeviceTemplate_set(PK_DeviceTemplate);
	m_pRow_PnpQueue->FK_Device_Created_set(PK_Device_Created);
	m_pRow_PnpQueue->FK_Device_Reported_set(PK_Device_Reported);
	m_pRow_PnpQueue->Removed_set(1);
	m_pRow_PnpQueue->Stage_set(PNP_REMOVE_STAGE_REMOVED);
	m_pRow_PnpQueue->Parms_set(sDeviceData);
	m_pRow_PnpQueue->Processed_set(0);
	pDatabase_pluto_main->PnpQueue_get()->Commit();
	m_sText=sText;
	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_dwPK_PnpQueue_BlockingFor=m_iPK_DHCPDevice=0;
	m_pOH_Orbiter=NULL;
	ParseDeviceData(sDeviceData);
	FindTopLevelDevice();
}

PnpQueueEntry::PnpQueueEntry(Plug_And_Play_Plugin *pPlug_And_Play_Plugin,Row_PnpQueue *pRow_PnpQueue)
{
	m_pPlug_And_Play_Plugin=pPlug_And_Play_Plugin;
	m_pRow_PnpQueue=pRow_PnpQueue;
	m_pDatabase_pluto_main = m_pRow_PnpQueue->Table_PnpQueue_get()->Database_pluto_main_get();

	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_dwPK_PnpQueue_BlockingFor=m_iPK_DHCPDevice=0;
	m_pOH_Orbiter=NULL;
	ParseDeviceData(m_pRow_PnpQueue->Parms_get());
	FindTopLevelDevice();
	m_pRow_Device_Reported = m_pDatabase_pluto_main->Device_get()->GetRow(m_pRow_PnpQueue->FK_Device_Reported_get());

	// If we haven't yet determined the device template, start at the beginning.
	// m_mapPK_DHCPDevice_possible isn't stored in the database
	if( m_pRow_PnpQueue->FK_DeviceTemplate_get()==0 )
		m_pRow_PnpQueue->Stage_set( m_pRow_PnpQueue->Removed_get()==1 ? PNP_REMOVE_STAGE_REMOVED : PNP_DETECT_STAGE_DETECTED );
}

void PnpQueueEntry::Stage_set(int Stage)
{ 
	g_pPlutoLogger->Write(LV_STATUS, "PnpQueueEntry::Stage_set queue %d now set to %d", m_pRow_PnpQueue->PK_PnpQueue_get(),Stage);
	m_pRow_PnpQueue->Stage_set(Stage); 
	if( Stage==PNP_DETECT_STAGE_DONE || Stage==PNP_REMOVE_STAGE_DONE )
		m_pRow_PnpQueue->Processed_set(1);
	m_pDatabase_pluto_main->PnpQueue_get()->Commit();

	if( m_pRow_Device_Reported )
	{
		string sMessage = StringUtils::itos(m_pRow_PnpQueue->PK_PnpQueue_get()) + " " + StageAsText();
		if( m_pRow_PnpQueue->Category_get().size() )
			sMessage += " " + m_pRow_PnpQueue->Category_get();

		Row_DeviceTemplate *pRow_DeviceTemplate;
		Row_Device *pRow_Device;
		if( m_pRow_PnpQueue->FK_Device_Created_get() && (pRow_Device=m_pRow_PnpQueue->FK_Device_Created_getrow()) )
			sMessage += " Added: " + pRow_Device->Description_get();
		else if( m_pRow_PnpQueue->FK_DeviceTemplate_get() && (pRow_DeviceTemplate=m_pRow_PnpQueue->FK_DeviceTemplate_getrow()) )
			sMessage += pRow_DeviceTemplate->Description_get();
		DCE::CMD_Display_Alert_DL CMD_Display_Alert_DL(m_pRow_Device_Reported->PK_Device_get(),m_sPK_Orbiter_List_For_Prompts,sMessage,
			"pnp_" + StringUtils::itos(m_pRow_PnpQueue->PK_PnpQueue_get()),(Stage==PNP_DETECT_STAGE_DONE || Stage==PNP_REMOVE_STAGE_DONE) ? "2" : "3");
		m_pPlug_And_Play_Plugin->SendCommand(CMD_Display_Alert_DL);
	}
}

void PnpQueueEntry::Block(EBlockedState eBlockedState)
{
	m_EBlockedState=eBlockedState;
	m_tTimeBlocked=time(NULL);
}

string PnpQueueEntry::DeviceDataAsString()
{
	string sResult;
	for(map<int,string>::iterator it=m_mapPK_DeviceData.begin();it!=m_mapPK_DeviceData.end();++it)
		sResult += StringUtils::itos(it->first) + "|" + it->second + "|";
	return sResult;
}

void PnpQueueEntry::ParseDeviceData(string sDeviceData)
{
	string::size_type pos=0;
	while(pos<sDeviceData.size())
	{
		int PK_DeviceData = atoi(StringUtils::Tokenize(sDeviceData,"|",pos).c_str());
		string sValue = StringUtils::Tokenize(sDeviceData,"|",pos);
		m_mapPK_DeviceData[PK_DeviceData]=sValue;
	}
}

void PnpQueueEntry::FindTopLevelDevice()
{
	m_pRow_Device_Reported = m_pRow_PnpQueue->FK_Device_Reported_getrow();
	m_dwPK_Device_TopLevel=0;
	Row_Device *pRow_Device=m_pRow_Device_Reported;
	while(pRow_Device)
	{
		pRow_Device = pRow_Device->FK_Device_ControlledVia_get() ? pRow_Device->FK_Device_ControlledVia_getrow() : NULL;
		if( pRow_Device )
			m_dwPK_Device_TopLevel = pRow_Device->PK_Device_get();
	}
}

void PnpQueueEntry::AssignDeviceData(Row_Device *pRow_Device)
{
	for(map<int,string>::iterator it=m_mapPK_DeviceData.begin();it!=m_mapPK_DeviceData.end();++it)
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),it->first,it->second);
	if( m_pRow_PnpQueue->SerialNumber_get().size() )
		DatabaseUtils::SetDeviceData(m_pDatabase_pluto_main,pRow_Device->PK_Device_get(),DEVICEDATA_Serial_Number_CONST,m_pRow_PnpQueue->SerialNumber_get());
}

bool PnpQueueEntry::IsDuplicate(PnpQueueEntry *pPnpQueueEntry)
{
	if( m_pRow_PnpQueue->Removed_get()==pPnpQueueEntry->m_pRow_PnpQueue->Removed_get() && 
		m_pRow_PnpQueue->Path_get()==pPnpQueueEntry->m_pRow_PnpQueue->Path_get() &&
		m_pRow_PnpQueue->VendorModelId_get()==pPnpQueueEntry->m_pRow_PnpQueue->VendorModelId_get() &&
		m_pRow_PnpQueue->IPaddress_get()==pPnpQueueEntry->m_pRow_PnpQueue->IPaddress_get() &&
		m_pRow_PnpQueue->MACaddress_get()==pPnpQueueEntry->m_pRow_PnpQueue->MACaddress_get() &&
		m_pRow_PnpQueue->SerialNumber_get()==pPnpQueueEntry->m_pRow_PnpQueue->SerialNumber_get() )
	{
		// So far it's a match.  Check if there's a com port, since there can be the same identical device on 2 serial ports
		if( m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)==m_mapPK_DeviceData.end() ||
			pPnpQueueEntry->m_mapPK_DeviceData.find(DEVICEDATA_COM_Port_on_PC_CONST)==pPnpQueueEntry->m_mapPK_DeviceData.end() ||
			m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST]==pPnpQueueEntry->m_mapPK_DeviceData[DEVICEDATA_COM_Port_on_PC_CONST] )
		{
			return true;
		}
	}
	return false;
}

string PnpQueueEntry::StageAsText()
{
	switch(m_pRow_PnpQueue->Stage_get())
	{
	case PNP_DETECT_STAGE_DETECTED:
		return "Detected";
	case PNP_DETECT_STAGE_CONFIRMING_POSSIBLE_DT:
		return "Analyzing";
	case PNP_DETECT_STAGE_RUNNING_DETECTION_SCRIPTS:
		return "Querying device.  Please wait...";
	case PNP_DETECT_STAGE_PROMPTING_USER_FOR_DT:
		return "Confirming";
	case PNP_DETECT_STAGE_PROMPTING_USER_FOR_OPT:
		return "Options";
	case PNP_DETECT_STAGE_ADD_DEVICE:
		return "Adding";
	case PNP_DETECT_STAGE_ADD_SOFTWARE:
		return "Installing";
	case PNP_DETECT_STAGE_START_DEVICE:
		return "Starting";
	case PNP_DETECT_STAGE_DONE:
		return "Done with new device";

	case PNP_REMOVE_STAGE_REMOVED:
		return "Removed";
	case PNP_REMOVE_STAGE_DONE:
		return "Done removing device";
	};
	return "ERROR--Stage";
}

