//<-dceag-d-b->
#include "Floorplan_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
Floorplan_Plugin::Floorplan_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Floorplan_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-dest-b->
Floorplan_Plugin::~Floorplan_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Floorplan_Plugin::Register()
//<-dceag-reg-e->
{
	return Connect(); 
}

//<-dceag-sample-b->!
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/


//<-dceag-c183-b->
/* 
	COMMAND: #183 - Get Floorplan Info
	COMMENTS: The orbiter will call this command to get a delimited list of all the floorplan objects and their locations on the map.
	PARAMETERS:
		#5 Value To Assign
			A | delimited list in the format, where {} indicate a repeating value: #pages,{#Types,{#Objects,{DeviceDescription, ObjectDescription, FillX Point, FillY Point, PK_DesignObj, Page, PK_Device, Type}}}
*/
void Floorplan_Plugin::CMD_Get_Floorplan_Info(,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c183-e->
{
	OCController *pController = m_pPlutoEvents->m_mapController[(*SafetyMessage)->m_DeviceIDFrom];
	if( !pController )
	{
		m_pOCLogger->Write(LV_CRITICAL,"Cannot find controller for floorplan layout: %d",(*SafetyMessage)->m_DeviceIDFrom);
		return;
	}
	string ReturnValue = StringUtils::itos((int) pController->m_mapFloorplanObjectVector.size()) + "|";

	
	map<int,FloorplanObjectVectorMap *>::iterator itFloorplanObjectVectorMap;
	for(itFloorplanObjectVectorMap = pController->m_mapFloorplanObjectVector.begin();itFloorplanObjectVectorMap!=pController->m_mapFloorplanObjectVector.end();itFloorplanObjectVectorMap++)
	{

		int Page = (*itFloorplanObjectVectorMap).first;
		FloorplanObjectVectorMap *pfpObjVectorMap = (*itFloorplanObjectVectorMap).second;
		
		if( !pfpObjVectorMap )
		{
			m_pOCLogger->Write(LV_CRITICAL,"There's a null in m_mapFloorplanObjectVector with Page: %d controller: %d",Page,pController->PKID_Controller);
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
				m_pOCLogger->Write(LV_STATUS,"There's a null in m_FloorplanObjectVector with Page: %d Type: %d Controller: %d",Page,Type,pController->PKID_Controller);
				ReturnValue += StringUtils::itos(Type) + "|0|";
				continue;
			}

			ReturnValue += StringUtils::itos(Type) + "|" +
				StringUtils::itos((int) pfpObjVector->size()) + "|";

			for(int i=0;i<(int) pfpObjVector->size();++i)
			{
				FloorplanObject *fpObj = (*pfpObjVector)[i];
				ReturnValue += fpObj->ptrDevice->m_sDescription + "|" + fpObj->ObjectTypeDescription + "|" + StringUtils::itos(fpObj->FillX) + "|" + StringUtils::itos(fpObj->FillY) + "|" +
					fpObj->ObjectID + "|" + StringUtils::itos(fpObj->Page) + "|" + StringUtils::itos(fpObj->PKID_Device) + "|" + 
					StringUtils::itos(fpObj->Type) + "|";
			}
		}
	}
	pSocket->SendOCMessage(new OCMessage(0, (*SafetyMessage)->m_DeviceIDFrom, PRIORITY_NORMAL, MESSAGETYPE_REPLY, 0, 1, 0, ReturnValue.c_str()));
}
