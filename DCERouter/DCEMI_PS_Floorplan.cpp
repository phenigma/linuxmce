#include "PlutoUtils/CommonIncludes.h"	
#include "DCEMI_PS_Floorplan.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_FloorplanType.h"
#include "PlutoOrbiter.h"
#include "DCEMI_PS_Orbiter.h"
#include "DCEMI_PS_DataGrid.h"
#include "DCEMI_PS_Media.h"
#include "DataGrid.h"


// temp
#define C_FLOORPLANTYPE_ENTERTAINMENT_ZONE_CONST 99999

bool DCEMI_PS_Floorplan::Register()
{
	m_pDCEMI_PS_Orbiter = (class DCEMI_PS_Orbiter *) m_pRouter->m_mapPlugIn_Find(DCEPLUGIN_PS_Orbiter_CONST);
	m_pDCEMI_PS_DataGrid = (class DCEMI_PS_DataGrid *) m_pRouter->m_mapPlugIn_Find(DCEPLUGIN_PS_Data_Grid_CONST);
	m_pDCEMI_PS_Media = (class DCEMI_PS_Media *) m_pRouter->m_mapPlugIn_Find(DCEPLUGIN_PS_Media_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Floorplan::FloorplanStatus))
		,0,0,0,0,MESSAGETYPE_FLOORPLAN_STATUS,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Floorplan::FloorplanLayout))
		,0,0,0,0,MESSAGETYPE_FLOORPLAN_LAYOUT,0);

	return true;
}

bool DCEMI_PS_Floorplan::FloorplanStatus(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;

	PlutoOrbiter *pController = m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter_Find(pMessage->m_dwPK_Device_From);
	if( !pController )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find controller for floorplan: %d",pMessage->m_dwPK_Device_From);
		return true;
	}
	int Type = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Type_CONST].c_str());
	int Page = pMessage->m_dwID;
	string ReturnValue;

//		g_pPlutoLogger->Write(LV_STATUS,"Getting floorplan status for type: %d Page: %d",Type,Page);
	unsigned int iUniqueColors[] = {255,65280,16711680,4194368,32896};
	if( Type==C_FLOORPLANTYPE_ENTERTAINMENT_ZONE_CONST )
	{
		string DATAGRID_ID = "streams_" + StringUtils::itos(pDeviceFrom->m_iPK_Device);
		
		DataGridMap::iterator dg = m_pDCEMI_PS_DataGrid->m_DataGrids.find(DATAGRID_ID);
		if (dg != m_pDCEMI_PS_DataGrid->m_DataGrids.end())
		{
			DataGridTable *ptrDataGridTable = (*dg).second;
			if( ptrDataGridTable )
			{
				delete (*dg).second;
				m_pDCEMI_PS_DataGrid->m_DataGrids.erase(dg);
			}
		}

		DataGridTable *pDataGrid = new DataGridTable();
		m_pDCEMI_PS_DataGrid->m_DataGrids[DATAGRID_ID] = pDataGrid;
		DataGridCell *pCell;

		int iRow=0;
		mapEntZone::iterator itEntZone;
		for(itEntZone = m_pDCEMI_PS_Media->m_mapEntZone.begin();itEntZone!=m_pDCEMI_PS_Media->m_mapEntZone.end();++itEntZone)
		{
			EntZone *pEntZone = (*itEntZone).second;
			if( !pEntZone )
				continue;

//				g_pPlutoLogger->Write(LV_STATUS,"Floorplan - EPG System: %s # zones: %d multi: %s watching: %p",
//					pSystem->m_ptrDevice->m_sDescription.c_str(),(int) pSystem->m_mapAVZones.size(),
//					(pSystem->m_bMultiZoneOnly ? "Y" : "N"),pSystem->m_pWatchingStream);

			string Description="";
			if( pEntZone->m_pDCEMediaStream )
			{
				/*
				pEntZone->Temp=iRow;
				MediaTypeInfo *pStreamInfo = m_mapMediaTypeInfo[pEntZone->m_pDCEMediaStream->m_MediaType];

				Description = "~" + pEntZone->m_ptrDevice->m_sDescription + "\n" + 
					(pStreamInfo ? pStreamInfo->Description : "") + "\n";

				if( pEntZone->m_pDCEMediaStream->pProvider_Station__Schedule )
					Description += pEntZone->m_pDCEMediaStream->pProvider_Station__Schedule->m_Title20;
				else if( pEntZone->m_pDCEMediaStream->m_sDescription!="" )
					Description += pEntZone->m_pDCEMediaStream->m_sDescription;
*/
			}
			else
				continue;
/*
			pCell = new DataGridCell(Description,StringUtils::itos(pEntZone->m_ptrDevice->m_iPK_Device));
			pCell->m_AltColor = iRow+1;
			pDataGrid->SetData(0,iRow++, pCell);
*/
		}
/*
		string Description="";
		if( pSystem->m_pWatchingStream )
		{
			pSystem->Temp=iRow;
			MediaTypeInfo *pStreamInfo = m_pRouter->m_pDataGridDevice->m_mapMediaTypeInfo[pSystem->m_pWatchingStream->m_MediaType];

			Description = "~" + pSystem->m_ptrDevice->m_sDescription + "\n" + 
				(pStreamInfo ? pStreamInfo->Description : "") + "\n";

			if( pSystem->m_pWatchingStream->pProvider_Station__Schedule )
				Description += pSystem->m_pWatchingStream->pProvider_Station__Schedule->m_Title20;
			else if( pSystem->m_pWatchingStream->m_sDescription!="" )
				Description += pSystem->m_pWatchingStream->m_sDescription;
		}
		else if( pSystem->m_pRecordingStream && pSystem->m_pRecordingStream->pProvider_Station__Schedule )
		{
			pSystem->Temp=iRow;
			Description = "~" + pSystem->m_ptrDevice->m_sDescription + "\nRecording: " + pSystem->m_pRecordingStream->pProvider_Station__Schedule->m_Title20;
		}
		else
			continue;

		pCell = new DataGridCell(Description,StringUtils::itos(pSystem->m_PK_EntGroup));
		pCell->m_AltColor = iRow+1;
		pDataGrid->SetData(0,iRow++, pCell);
*/
	}

	FloorplanObjectVectorMap *pFloorplanObjectVectorMap = pController->m_mapFloorplanObjectVector_Find(Page);
	FloorplanObjectVector *fpObjVector=NULL;
	if( pFloorplanObjectVectorMap )
		fpObjVector = (*pFloorplanObjectVectorMap)[Type];

	if( fpObjVector )
	{
		for(int i=0;i<(int) fpObjVector->size();++i)
		{
			int Color=0;
			string Description="";
			string OSD="";

			FloorplanObject *fpObj = (*fpObjVector)[i];
			DeviceData_Router *pDevice = fpObj->ptrDevice;

			if( Type==C_FLOORPLANTYPE_ENTERTAINMENT_ZONE_CONST )
			{
				
				class EntZone *pEntZone = m_pDCEMI_PS_Media->m_mapEntZone_Find(pDevice->m_iPK_Device);
				
				if( pEntZone )
				{
/*
					if( pEntZone->m_pDCEMediaStream )
						Color = iUniqueColors[pEntZone->Temp];
*/
				}
			}
			else
			{
				/*
				switch(fpObj->Type)
				{
					// These all use the same color scheme
				case C_FLOORPLANOBJECTTYPE_SECURITY_DOOR_CONST:
				case C_FLOORPLANOBJECTTYPE_SECURITY_WINDOW_CONST:
				case C_FLOORPLANOBJECTTYPE_SECURITY_MOTION_DETECTOR_CONST:
					if( pDevice->m_bBypassed )
					{
						Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_BYPASSED_CONST]->Color;
						Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_BYPASSED_CONST]->Description;
					}
					else if( pDevice->m_bAlert )
					{
						Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_TRIGGERED_CONST]->Color;
						Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_TRIGGERED_CONST]->Description;
					}
					else if( m_pPlutoEvents->DeviceIsSecurityBreach(pDevice) )
					{
						string DefaultState = pDevice->m_mapDeviceParameters_Find(C_DEVICEDATA_DEFAULT_STATE_CONST);
						// This device is armed
						if( pDevice->m_sState!=DefaultState )
						{
							// Same as triggered
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_TRIGGERED_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_TRIGGERED_CONST]->Description;
						}
						else
						{
							// Normal armed
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_ARMED_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_ARMED_CONST]->Description;
						}
					}
					else
					{
						string DefaultState = pDevice->m_mapDeviceParameters_Find(C_DEVICEDATA_DEFAULT_STATE_CONST);
						// Not armed
						if( pDevice->m_sState!=DefaultState )
						{
							// Open
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_OPEN_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_OPEN_CONST]->Description;
						}
						else
						{
							// Closed
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_CLOSED_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_SECURITY_DOOR_CLOSED_CONST]->Description;
						}
					}
					break; 
				case C_FLOORPLANOBJECTTYPE_CLIMATE_THERMOMETER_CONST:
				case C_FLOORPLANOBJECTTYPE_CLIMATE_THERMOSTAT_CONST:
					if( pDevice->m_pClimateInformation->eClimateType==climate_OFF )
					{
						Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_OFF_CONST]->Color;
						Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_OFF_CONST]->Description;
					}
					else
					{
						if( pDevice->m_pClimateInformation->eClimateType==climate_AUTO )
						{
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_FAN_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_FAN_CONST]->Description;
						}
						else if( pDevice->m_pClimateInformation->eClimateType==climate_COOL )
						{
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_COOLING_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_COOLING_CONST]->Description;
						}
						else if( pDevice->m_pClimateInformation->eClimateType==climate_HEAT )
						{
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_HEATING_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_HEATING_CONST]->Description;
						}
						else if( pDevice->m_pClimateInformation->eClimateType==climate_FAN )
						{
							Color = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_FAN_CONST]->Color;
							Description = m_mapFloorplanItemStatus[C_FLOORPLANOBJECTTYPE_COLOR_CLIMATE_THERMOSTAT_FAN_CONST]->Description;
						}
						Description += " set:" + StringUtils::itos(pDevice->m_pClimateInformation->m_iSetPoint) + " cur: " + StringUtils::itos(pDevice->m_pClimateInformation->m_iTemperature);
						OSD = StringUtils::itos(pDevice->m_pClimateInformation->m_iSetPoint) + "\xF8\n(" + StringUtils::itos(pDevice->m_pClimateInformation->m_iTemperature) + "\xF8)";
					}
					break;
					default:
						break;
				};
				*/
			}
			ReturnValue += StringUtils::itos(Color) + "|" + Description + "|" + OSD + "|";
		}
	}
	if( ReturnValue.length()==0 )
		ReturnValue="0";
	pSocket->SendMessage(new Message(0, pMessage->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, 0, ReturnValue.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false;
}

bool DCEMI_PS_Floorplan::FloorplanLayout(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	map<int,class PlutoOrbiter *>::iterator it;
	for(it=m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter.begin();it!=m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter.end();++it)
	{
		class PlutoOrbiter *p = (*it).second;
		int x = (*it).first;
		int k=2;
	}

	PlutoOrbiter *pController = m_pDCEMI_PS_Orbiter->m_mapPlutoOrbiter_Find(pMessage->m_dwPK_Device_From);
	if( !pController )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find controller for floorplan layout: %d",pMessage->m_dwPK_Device_From);
		return true;
	}
	string ReturnValue = StringUtils::itos((int) pController->m_mapFloorplanObjectVector.size()) + "|";

	
	map<int,FloorplanObjectVectorMap *>::iterator itFloorplanObjectVectorMap;
	for(itFloorplanObjectVectorMap = pController->m_mapFloorplanObjectVector.begin();itFloorplanObjectVectorMap!=pController->m_mapFloorplanObjectVector.end();itFloorplanObjectVectorMap++)
	{

		int Page = (*itFloorplanObjectVectorMap).first;
		FloorplanObjectVectorMap *pfpObjVectorMap = (*itFloorplanObjectVectorMap).second;
		
		if( !pfpObjVectorMap )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"There's a null in m_mapFloorplanObjectVector with Page: %d controller: %d",Page,pController->m_pDeviceData_Router->m_iPK_Device);
			ReturnValue += StringUtils::itos(Page) + "|0|";
			continue;
		}
		ReturnValue += StringUtils::itos(Page) + "|" +
			StringUtils::itos((int) pfpObjVectorMap->size()) + "|";

		map<int,FloorplanObjectVector *>::iterator itFloorplanObjectVector;
		for(itFloorplanObjectVector = pfpObjVectorMap->begin();itFloorplanObjectVector!=pfpObjVectorMap->end();itFloorplanObjectVector++)
		{
			int Type = (*itFloorplanObjectVector).first;
			FloorplanObjectVector *pfpObjVector = (*itFloorplanObjectVector).second;

			if( !pfpObjVector )
			{
				// AB 4/8/2004 I don't fully understand why this null gets in there, but it's harmless--it just
				// means there are no floorplan objects of this type on the given page
				g_pPlutoLogger->Write(LV_STATUS,"There's a null in m_FloorplanObjectVector with Page: %d Type: %d Controller: %d",Page,Type,pController->m_pDeviceData_Router->m_iPK_Device);
				ReturnValue += StringUtils::itos(Type) + "|0|";
				continue;
			}

			ReturnValue += StringUtils::itos(Type) + "|" +
				StringUtils::itos((int) pfpObjVector->size()) + "|";

			for(int i=0;i<(int) pfpObjVector->size();++i)
			{
				FloorplanObject *fpObj = (*pfpObjVector)[i];
				ReturnValue += fpObj->ptrDevice->m_sDescription + "|" + fpObj->ObjectTypeDescription + "|" + StringUtils::itos(fpObj->FillX) + "|" + StringUtils::itos(fpObj->FillY) + "|" +
					fpObj->ObjectID + "|" + StringUtils::itos(fpObj->Page) + "|" + StringUtils::itos(fpObj->PK_Device) + "|" + 
					StringUtils::itos(fpObj->Type) + "|";
			}
		}
	}
	pSocket->SendMessage(new Message(0, pMessage->m_dwPK_Device_From, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, 0, ReturnValue.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false;
}

