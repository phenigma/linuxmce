#include "PlutoUtils/CommonIncludes.h"	
#include "DCEMI_PS_Orbiter.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"
#include "../pluto_main/Define_DesignObj.h"
#include "../pluto_main/Define_Variable.h"
#include "PlutoOrbiter.h"
#include "MobileOrbiter.h"
#include "DCEMI_PS_MobileOrbiter.h"

//temp 
#define COMMAND_BIND_TO_DEVICE_STATUS_CONST 9999
#define COMMAND_BIND_TO_HOUSE_STATUS_CONST 9999
#define DESIGNOBJ_DEVINCOMINGCALL_CONST 9999
#define VARIABLE_CALLER_ID_NUMBER_CONST 9999
#define DESIGNOBJ_ICOINCOMINGCALLPHOTO_CONST 9999
#define DEVICECATEGORY_ORBITER_CELL_CONST 9999

bool DCEMI_PS_Orbiter::Register()
{
	m_pDCEMI_PS_MobileOrbiter = (class DCEMI_PS_MobileOrbiter *) m_pRouter->m_mapPlugIn_Find(DCEPLUGIN_PS_Mobile_Orbiter_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::UpdateDesignObjImage))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Update_Object_Image_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::NavGoto))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Goto_Screen_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::SimulateKeypress))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Simulate_Keypress_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::VerifyPin))
		,0,0,0,0,MESSAGETYPE_VERIFYPIN,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::SetUserMode))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Set_User_Mode_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::BindToDevice))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_BIND_TO_DEVICE_STATUS_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::BindToHouse))
		,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_BIND_TO_HOUSE_STATUS_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::PinRequired))
		,0,0,0,0,MESSAGETYPE_PIN_REQUIRED,0);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Orbiter::CurrentController))
		,0,DEVICEID_CONTROLLER,0,0,0,0);

	return true;
}

bool DCEMI_PS_Orbiter::UpdateDesignObjImage(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	PlutoOrbiter *ptrPlutoOrbiter = m_mapPlutoOrbiter_Find(pMessage->m_dwPK_Device_To);
	if( ptrPlutoOrbiter && ptrPlutoOrbiter->m_pMobileOrbiter && ptrPlutoOrbiter->m_pMobileOrbiter->m_pDevice_CurrentDetected )
	{
		pMessage->m_dwPK_Device_To = ptrPlutoOrbiter->m_pMobileOrbiter->m_pDevice_CurrentDetected->m_dwPK_Device;
		pMessage->m_mapParameters[COMMANDPARAMETER_PK_DesignObj_CONST] = ptrPlutoOrbiter->m_pMobileOrbiter->m_sID + "|" + pMessage->m_mapParameters[COMMANDPARAMETER_PK_DesignObj_CONST];
	}
	return true;
}

bool DCEMI_PS_Orbiter::NavGoto(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	PlutoOrbiter *ptrPlutoOrbiter = m_mapPlutoOrbiter_Find(pMessage->m_dwPK_Device_To);
	if( ptrPlutoOrbiter )
	{
		ptrPlutoOrbiter->m_sCurrentScreen = pMessage->m_mapParameters[COMMANDPARAMETER_PK_DesignObj_CONST];

		if( ptrPlutoOrbiter->m_pMobileOrbiter )
		{
			ptrPlutoOrbiter->m_pMobileOrbiter->ShowMenu(atoi(ptrPlutoOrbiter->m_sCurrentScreen.c_str()),pMessage);
		}
		else
		{
			// trap for special screens
			if( ptrPlutoOrbiter->m_sCurrentScreen.find(StringUtils::itos(DESIGNOBJ_DEVINCOMINGCALL_CONST))==0 )
			{
				// See if we have a picture for the caller ID
				string CallerID = ptrPlutoOrbiter->m_mapVariables[VARIABLE_CALLER_ID_NUMBER_CONST]->m_sCurrentValue;
				if( CallerID.substr(0,1)=="1" )
					CallerID = CallerID.substr(1);

				string Filename = "/home/callerid_photos/" + CallerID + ".jpg";
				size_t Length;
				char *c = FileUtils::ReadFileIntoBuffer(Filename, Length);
				if (c!=NULL)
				{
					Message *pImageMessage = new Message(0, ptrPlutoOrbiter->m_pDeviceData_Router->m_dwPK_Device, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Update_Object_Image_CONST, 
						2, COMMANDPARAMETER_PK_DesignObj_CONST, (ptrPlutoOrbiter->m_sCurrentScreen + "." + StringUtils::itos(DESIGNOBJ_ICOINCOMINGCALLPHOTO_CONST)).c_str(),
							COMMANDPARAMETER_Type_CONST,"1"); 
					pImageMessage->m_mapData_Parameters[COMMANDPARAMETER_Data_CONST]=c;
					pImageMessage->m_mapData_Lengths[COMMANDPARAMETER_Data_CONST]=(int) Length;
					m_pRouter->DispatchMessage(pImageMessage);
				}
			}
		}
	}
	return true;
}
bool DCEMI_PS_Orbiter::SimulateKeypress(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom || pDeviceFrom->m_dwPK_DeviceCategory!=DEVICECATEGORY_ORBITER_CELL_CONST )
		return true;

	MobileOrbiter *pMobileOrbiter = m_pDCEMI_PS_MobileOrbiter->m_mapMobileOrbiter_Find(pDeviceFrom->m_dwPK_Device);
	if( !pMobileOrbiter )
	{
		g_pPlutoLogger->Write(LV_STATUS,"keypress from unknown bluetooth device %d",pDeviceFrom->m_dwPK_Device);
	}
	else
	{
		string Key = pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST];
		pMobileOrbiter->Keypress(Key);
	}
	return true;
}

bool DCEMI_PS_Orbiter::VerifyPin(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	string Response="0";
	string query = "SELECT * FROM Users WHERE Pincode='" + pMessage->m_mapParameters[1] + "'";

	if( pMessage->m_dwID )
		query += " AND FK_S_Users=" + StringUtils::itos(pMessage->m_dwID);

	PlutoSqlResult result;
	MYSQL_ROW row;
	result.r=m_pRouter->mysql_query_result(query);
	g_pPlutoLogger->Write(LV_WARNING,"Got verify PIN %s", query.c_str());
	if( result.r )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Got a result ");
		if ((row = mysql_fetch_row(result.r)))
		{
			g_pPlutoLogger->Write(LV_WARNING,"Got a record %s ", row[0]);
			Response = row[0];
		}
	}
	g_pPlutoLogger->Write(LV_WARNING,"result was %s", Response.c_str());
	pSocket->SendMessage(new Message(0,0,0,0,0,1,1, Response.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false;
}


bool DCEMI_PS_Orbiter::SetUserMode(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
	int PK_User = atoi(pMessage->m_mapParameters[C_COMMANDPARAMETER_USER_CONST].c_str());
	int Value = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Value_To_Assign_CONST].c_str());
	m_pPlutoEvents->SetUserMode(PK_User,Value,NULL);  // If this was coming from a controller, it will be handled in the ExecuteEvent code
	*/
	return true;
}

bool DCEMI_PS_Orbiter::BindToDevice(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
	int PK_Device=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_PK_Device_CONST].c_str());
	string DesignObj=pMessage->m_mapParameters[COMMANDPARAMETER_PK_DesignObj_CONST];
	int PK_PlutoOrbiter=atoi(pMessage->m_mapParameters[C_COMMANDPARAMETER_CONTROLLER_ID_CONST].c_str());
	int ID = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST].c_str());

	DeviceData_Router *pDevice = m_pPlutoEvents->m_mapDevice_Find(PK_Device);
	if( !pDevice )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Unable to bind to device indicator for: %d",PK_Device);
		return true;
	}
	Indicator *i = new Indicator();
	i->m_PK_Controller = PK_PlutoOrbiter;
	i->m_DesignObj = DesignObj;
	i->m_Usage = ID;
	pDevice->m_vectIndicators.push_back(i);
	*/
	return true;
}

bool DCEMI_PS_Orbiter::BindToHouse(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
	// This isn't coming from a controller, otherwise it would go through execute event
	HouseStatusIndicatorBindingList *pHouse = new HouseStatusIndicatorBindingList();
	pHouse->m_PK_Controller=pMessage->m_dwPK_Device_From;

	m_pPlutoEvents->m_vectHouseStatusIndicator.push_back(pHouse);
	*/
	return true;
}

bool DCEMI_PS_Orbiter::PinRequired(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	string Response="0";
	string query = "SELECT * FROM Users_Device_PinCode WHERE FK_Users=" + StringUtils::itos(pMessage->m_dwID) + " AND FK_Device=" + StringUtils::itos(pMessage->m_dwPK_Device_From);

	PlutoSqlResult result;
	MYSQL_ROW row;
	result.r=m_pRouter->mysql_query_result(query);
	g_pPlutoLogger->Write(LV_WARNING,"Got verify PIN required %s", query.c_str());
	if( result.r )
	{
		g_pPlutoLogger->Write(LV_WARNING,"Got a result ");
		if ((row = mysql_fetch_row(result.r)))
		{
			Response = "1";
		}
	}
	g_pPlutoLogger->Write(LV_WARNING,"result was %s", Response.c_str());
	pSocket->SendMessage(new Message(0,0,0,0,0,1,1, Response.c_str()));
	pMessage->m_bRespondedToMessage=true;
	return false;
}


bool DCEMI_PS_Orbiter::CurrentController(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	if( !pDeviceFrom )
		return true;  // Don't know what to do with this, let others have a go
	/*
	else
	{
		PlutoOrbiter *ptrPlutoOrbiter = pDeviceFrom->GetCurrentController();
		if( !ptrPlutoOrbiter )
		{
			if( pDeviceFrom->m_dwPK_Device_ControlledVia )
			{
				DeviceData_Router *pParentDevice = m_pRouter->m_mapDeviceData_Router_Find(pDeviceFrom->m_dwPK_Device_ControlledVia);
				if( pParentDevice )
					ptrPlutoOrbiter = pParentDevice->GetCurrentController();
			}
		}
		if( !ptrPlutoOrbiter )  // If we still don't have a controller, just send it to all associated controllers
		{
			pDeviceFrom->SendToController(pMessage);
			return false;
		}
		else
			pMessage->m_dwPK_Device_To=ptrPlutoOrbiter->m_pDeviceData_Router->m_dwPK_Device;
	}
	*/
	return true;
}
