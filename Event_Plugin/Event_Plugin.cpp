//<-dceag-d-b->
#include "Event_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter/DCERouter.h"
#include "Event.h"
#include "EventInfo.h"
#include "EventHandler.h"
#include "EventInstance.h"
#include "Criteria.h"
#include "CriteriaParm.h"
#include "CriteriaParmNesting.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Criteria.h"
#include "pluto_main/Table_CriteriaParm.h"
#include "pluto_main/Table_CriteriaParmList.h"
#include "pluto_main/Table_CriteriaParmNesting.h"
#include "pluto_main/Table_EventHandler.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Event_Plugin::Event_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Event_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_dwID_EventInstance=0;
    m_pDatabase_pluto_main = new Database_pluto_main( );
    if( !m_pDatabase_pluto_main->Connect( m_pRouter->sDBHost_get( ), m_pRouter->sDBUser_get( ), m_pRouter->sDBPassword_get( ), m_pRouter->sDBName_get( ), m_pRouter->iDBPort_get( ) ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        m_bQuit=true;
        return;
    }

	Row_Installation *pRow_Installation = m_pDatabase_pluto_main->Installation_get()->GetRow( m_pRouter->iPK_Installation_get() );

	vector<Row_Criteria *> vectRow_Criteria;
	if(pRow_Installation)
		pRow_Installation->Criteria_FK_Installation_getrows(&vectRow_Criteria);
	for(size_t s=0;s<vectRow_Criteria.size();++s)
	{
		Row_Criteria *pRow_Criteria = vectRow_Criteria[s];
if( pRow_Criteria->PK_Criteria_get()==508 )
{
int k=2;
}
		CriteriaParmNesting *pCriteriaParmNesting = LoadCriteriaParmNesting(NULL,pRow_Criteria->FK_CriteriaParmNesting_getrow());
		Criteria *pCriteria = new Criteria(pRow_Criteria->PK_Criteria_get(),pCriteriaParmNesting);
		m_mapCriteria[pRow_Criteria->PK_Criteria_get()] = pCriteria;
	}

	vector<Row_EventHandler *> vectRow_EventHandler;
	if(pRow_Installation)
		pRow_Installation->EventHandler_FK_Installation_getrows(&vectRow_EventHandler);
	for(size_t s=0;s<vectRow_EventHandler.size();++s)
	{
		Row_EventHandler *pRow_EventHandler = vectRow_EventHandler[s];

		EventHandler *pEventHandler = new EventHandler(pRow_EventHandler->PK_EventHandler_get(),
			pRow_EventHandler->FK_Event_get(),m_pRouter->m_mapCommandGroup_Find(pRow_EventHandler->FK_CommandGroup_get()),
			m_mapCriteria_Find(pRow_EventHandler->FK_Criteria_get()));

		ListEventHandler *pListEventHandler = m_mapListEventHandler_Find(pRow_EventHandler->FK_Event_get());
		if( !pListEventHandler )
		{
			pListEventHandler = new ListEventHandler();
  			m_mapListEventHandler[pRow_EventHandler->FK_Event_get()] = pListEventHandler;
		}
		pListEventHandler->push_back(pEventHandler);
	}
}

//<-dceag-const2-b->!

CriteriaParmNesting *Event_Plugin::LoadCriteriaParmNesting(CriteriaParmNesting *pCriteriaParmNesting_Parent,Row_CriteriaParmNesting *pRow_CriteriaParmNesting)
{
	CriteriaParmNesting *pCriteriaParmNesting = new CriteriaParmNesting(pRow_CriteriaParmNesting->IsNot_get(),pRow_CriteriaParmNesting->IsAnd_get());
	if( pCriteriaParmNesting_Parent )
		pCriteriaParmNesting_Parent->m_vectCriteriaParmNesting.push_back(pCriteriaParmNesting);
	vector<Row_CriteriaParmNesting *> vectRow_CriteriaParmNesting;
	pRow_CriteriaParmNesting->CriteriaParmNesting_FK_CriteriaParmNesting_Parent_getrows(&vectRow_CriteriaParmNesting);
	for(size_t s=0;s<vectRow_CriteriaParmNesting.size();++s)
	{
		Row_CriteriaParmNesting *pRow_CriteriaParmNesting = vectRow_CriteriaParmNesting[s];
		LoadCriteriaParmNesting(pCriteriaParmNesting,pRow_CriteriaParmNesting);
	}
	vector<Row_CriteriaParm *> vectRow_CriteriaParm;
	pRow_CriteriaParmNesting->CriteriaParm_FK_CriteriaParmNesting_getrows(&vectRow_CriteriaParm);
	for(size_t s=0;s<vectRow_CriteriaParm.size();++s)
	{
		Row_CriteriaParm *pRow_CriteriaParm = vectRow_CriteriaParm[s];
		pCriteriaParmNesting->m_vectCriteriaParm.push_back( new CriteriaParm(
			pRow_CriteriaParm->PK_CriteriaParm_get(),pRow_CriteriaParm->FK_CriteriaParmList_get(),
			pRow_CriteriaParm->FK_CriteriaParmList_getrow()->FK_ParameterType_get(),pRow_CriteriaParm->Operator_get(),
			pRow_CriteriaParm->Value_get(),pRow_CriteriaParm->Parm_get()) );
	}

	return pCriteriaParmNesting;
}

//<-dceag-dest-b->
Event_Plugin::~Event_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Event_Plugin::Register()
//<-dceag-reg-e->
{
    RegisterMsgInterceptor((MessageInterceptorFn)(&Event_Plugin::ProcessEvent) ,0,0,0,0,MESSAGETYPE_EVENT,0);
	return Connect(); 
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
void Event_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Event_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

bool Event_Plugin::ProcessEvent(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	ListEventHandler *pListEventHandler = m_mapListEventHandler_Find(pMessage->m_dwID);
	if( pListEventHandler==NULL ) // No handlers for this type of event
	{
		g_pPlutoLogger->Write(LV_EVENT,"Event #%d has no handlers",pMessage->m_dwID);
		return true;
	}

	EventInfo *pEventInfo = new EventInfo(pMessage->m_dwID,pMessage,(DeviceData_Router *)pDeviceFrom,1 /*m_iPKID_C_HouseMode*/);
//	m_listEventInfo.push_back(pEventInfo);

//	g_pPlutoLogger->Write(LV_EVENT,"Event #%d has %d handlers",pEventInfo->m_iPKID_Event,(int)pEvent->m_vectEventHandlers.size());
	for(ListEventHandler::iterator it=pListEventHandler->begin();it!=pListEventHandler->end();++it)
	{
		EventHandler *pEventHandler = *it;
//		pEventInfo->m_vectEventHandlers.push_back(pEventHandler);
//		pEventInfo->pEventHandler = pEventHandler;

		if(pEventHandler->m_pCommandGroup==NULL)
			continue;  // This event handler doesn't have anything to do anyway

		bool bResult = true;  // it's true unless there's a criteria that evaluates to false
		if( pEventHandler->m_pCriteria!=NULL )
		{
			try
			{
				if( !pEventHandler->m_pCriteria->Evaluate(pEventInfo) )
					bResult=false;
			}
			catch(exception e)
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Problem with criteria for Event ID: %d - result: %s",pEventHandler->m_PK_EventHander,"??");
				bResult=false;
			}
		}

		g_pPlutoLogger->Write(LV_EVENTHANDLER,"Evaluated Event Handler: %d to: %d",pEventHandler->m_PK_EventHander,(int) bResult);
		if( bResult ) 
		{
			EventInstance *pEventInstance = new EventInstance(m_dwID_EventInstance++,pEventHandler);
			pEventInstance->pMessage = new Message(pMessage);
			pEventInstance->m_PKID_Device_OriginatedThisEvent = pMessage->m_dwPK_Device_From;
			ExecuteEvent(pEventInstance);
		}
	}

	delete pEventInfo;
	return true;
}

void Event_Plugin::ExecuteEvent(EventInstance *pEventInstance)
{
	ExecCommandGroup(pEventInstance->m_ptrEventHandler->m_pCommandGroup->m_PK_CommandGroup);
	delete pEventInstance;  // We will probably need to keep this for a while for some events like security problems
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/



