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
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_PnpQueue.h"
#include "pluto_main/Table_Device.h"

using namespace DCE;

// Constructor for device detected
PnpQueueEntry::PnpQueueEntry(Database_pluto_main *pDatabase_pluto_main,
	string sDeviceData,
	string sIPAddress,
	string sMacAddress,
	int PK_CommMethod,
	int PK_DeviceTemplate,
	int PK_Device_Reported,
	int PK_PnpProtocol,
	string sPnpSerialNumber,
	string sText,
	string sVendorModelId)
{
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
	m_pRow_PnpQueue->FK_Device_Created_set(0);
	m_pRow_PnpQueue->FK_Device_Reported_set(PK_Device_Reported);
	m_pRow_PnpQueue->Removed_set(0);
	m_pRow_PnpQueue->Stage_set(PNP_DETECT_STAGE_DETECTED);
	m_pRow_PnpQueue->Processed_set(0);
	pDatabase_pluto_main->PnpQueue_get()->Commit();
	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_iPK_DHCPDevice=0;
	m_pOH_Orbiter=NULL;
	ParseDeviceData(sDeviceData);
	FindTopLevelDevice();
}

// Constructor for device removed
PnpQueueEntry::PnpQueueEntry(Database_pluto_main *pDatabase_pluto_main,
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
	m_pRow_PnpQueue->Processed_set(0);
	pDatabase_pluto_main->PnpQueue_get()->Commit();
	m_EBlockedState=pnpqe_blocked_none;
	m_tTimeBlocked=0;
	m_iPK_DHCPDevice=0;
	m_pOH_Orbiter=NULL;
	ParseDeviceData(sDeviceData);
	FindTopLevelDevice();
}

void PnpQueueEntry::Stage_set(int Stage)
{ 
	m_pRow_PnpQueue->Stage_set(Stage); 
	if( Stage==PNP_DETECT_STAGE_DONE || Stage==PNP_REMOVE_STAGE_DONE )
		m_pRow_PnpQueue->Processed_set(1);
	m_pDatabase_pluto_main->PnpQueue_get()->Commit();
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
}

