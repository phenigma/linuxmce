#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <time.h>

#include "EventHandler.h"
#include "EventInstance.h"
#include "CriteriaParm.h"
#include "Criteria.h"
#include "EventInfo.h"
#include "DeviceData_Router.h"
#include "Message.h"

#include "pluto_main/Define_CriteriaParmList.h"
#include "pluto_main/Define_ParameterType.h"

#define OPERATOR_EQUALS			1
#define OPERATOR_NOTEQUALS		2
#define OPERATOR_GREATERTHAN	3
#define OPERATOR_LESSTHAN		4

bool EvaluateTimeOfDay(string Expression,void *pExtraInfo);

bool Criteria::EvaluateExpression(class CriteriaParm *pCriteriaParm,class EventInfo *pEventInfo,void *pExtraInfo)
{
	string::size_type pos=0;
	string temp;
	
	time_t long_time;
    time( &long_time );
	struct tm *newtime = localtime( &long_time );

	int iRValue=0;
	string sRValue="";

	if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST)
		iRValue = atoi(pCriteriaParm->m_sValue.c_str());
	else
		sRValue = pCriteriaParm->m_sValue;

	string *sLValue=NULL;
	unsigned long *iLValue=NULL;

	// Available for temporary use
	unsigned long  iTmp;
	string sTmp;

	switch(pCriteriaParm->m_iPK_CriteriaParmList)
	{
	case CRITERIAPARMLIST_Month_CONST:
		iTmp = newtime->tm_mon+1;  // Month is 0 based
		iLValue = &iTmp;
		break;
	case CRITERIAPARMLIST_PK_Device_CONST:
		iLValue = &pEventInfo->m_pDevice->m_dwPK_Device;
		break;
	case CRITERIAPARMLIST_PK_DeviceTemplate_CONST:
		iLValue = &pEventInfo->m_pDevice->m_dwPK_DeviceTemplate;
		break;
	case CRITERIAPARMLIST_Time_of_day_CONST:
		return EvaluateTimeOfDay(StringUtils::ToUpper(pCriteriaParm->m_sValue),pExtraInfo);
	case CRITERIAPARMLIST_PK_DeviceCategory_CONST:
		iLValue = &pEventInfo->m_pDevice->m_dwPK_DeviceCategory;
		break;
	case CRITERIAPARMLIST_PK_Room_CONST:
		if( !pEventInfo->m_pDevice->m_pRoom )
			return false;
		iLValue = &pEventInfo->m_pDevice->m_pRoom->m_dwPK_Room;
		break;
//	case CRITERIAPARMLIST_PK_DeviceGroup_CONST:
//		return pEventInfo->m_pDevice->m_mapDeviceGroups[atoi(pCriteriaParm->m_sValue.c_str())];
	case CRITERIAPARMLIST_House_Mode_CONST:
		iLValue = (unsigned long *) (&pEventInfo->m_PK_HouseMode);
		break;
//	case CRITERIAPARMLIST_Room_Mode_CONST:
//		iLValue = &pEventInfo->PK_C_Mode_Room;
//		break;
	case CRITERIAPARMLIST_Day_Of_Week_CONST:
		iLValue = (unsigned long *) (&newtime->tm_wday);
		break;
	case CRITERIAPARMLIST_Day_Of_Month_CONST:
		iLValue = (unsigned long *) (&newtime->tm_mday);
		break;
	case CRITERIAPARMLIST_Specific_Date_CONST:
		break;
//	case CRITERIAPARMLIST_PK_RoomType_CONST:
//		iLValue = &pEventInfo->m_pDevice->m_pRoom->m_im_iPK_RoomType;
//		break;
	case CRITERIAPARMLIST_PK_EventList_CONST:
		iLValue = (unsigned long *) (&pEventInfo->m_pMessage->m_dwID);
		break;
	case CRITERIAPARMLIST_PK_EventHandler_CONST:
		iLValue = &pEventInfo->m_pEventHandler->m_PK_EventHander;
		break;
	case CRITERIAPARMLIST_PK_EventParameter_CONST:
		{
			int PK_EventParameter = atoi( pCriteriaParm->m_sParm.c_str() );
			temp = pEventInfo->m_pMessage->m_mapParameters[PK_EventParameter];
			sLValue = &temp;
		}
		break;
	default:
		throw(string("Unknown Criteria Parm List" + StringUtils::itos(pCriteriaParm->m_iPK_CriteriaParmList)));
	}

	if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST )
	{
		pCriteriaParm->m_sComparedValue = StringUtils::itos(*iLValue);
	}
	else
	{
		pCriteriaParm->m_sComparedValue = *sLValue;
	}

	if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_string_CONST && sLValue==NULL )
		throw(string("comparing string, string is null"));
	else if( (pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST) && iLValue==NULL )
		throw(string("comparing int, int is null"));

	if( pCriteriaParm->m_Operator==OPERATOR_EQUALS )
	{
		if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_string_CONST )
			return *sLValue==sRValue;
		else if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST)
			return *iLValue==iRValue;
	}
	if( pCriteriaParm->m_Operator==OPERATOR_NOTEQUALS )
	{
		if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_string_CONST )
			return *sLValue!=sRValue;
		else if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST)
			return *iLValue!=iRValue;
	}
	if( pCriteriaParm->m_Operator==OPERATOR_GREATERTHAN )
	{
		if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_string_CONST )
			return *sLValue>sRValue;
		else if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST)
			return *iLValue>iRValue;
	}
	if( pCriteriaParm->m_Operator==OPERATOR_LESSTHAN )
	{
		if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_string_CONST )
			return *sLValue<sRValue;
		else if( pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_int_CONST || pCriteriaParm->m_iPK_ParameterType==PARAMETERTYPE_bool_CONST)
			return *iLValue<iRValue;
	}

	throw(string("Unhandled criteria"));
}

#include "../General_Info_Plugin/General_Info_Plugin.h"
#include "DCERouter.h"

bool EvaluateTimeOfDay(string Expression,void *pExtraInfo)
{
	General_Info_Plugin *pGeneral_Info_Plugin = NULL;
	Router *pRouter = (Router *) pExtraInfo;
	if( pRouter )
	{
		ListDeviceData_Router *pList = pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_General_Info_Plugin_CONST);
		if( pList && pList->size() )
		{
			DeviceData_Router *pDevice = *(pList->begin());
			pGeneral_Info_Plugin = (General_Info_Plugin *) pRouter->m_mapPlugIn_Find(pDevice->m_dwPK_Device);
		}
	}

	if( Expression=="DAY" || Expression=="NIGHT")
	{
		if( !pGeneral_Info_Plugin )
			return false;

		bool bIsDaytime;
		pGeneral_Info_Plugin->CMD_Is_Daytime(&bIsDaytime);
		return (Expression=="DAY" && bIsDaytime) || (Expression=="NIGHT" && !bIsDaytime);
	}

	return false;
}
