//<-dceag-d-b->
#include "Orbiter_Plugin.h"
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

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Event.h"
#include "DCERouter/DCERouter.h"

#include <cctype>
#include <algorithm>

//<-dceag-const-b->
Orbiter_Plugin::Orbiter_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Orbiter_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return;
	}
}

//<-dceag-dest-b->
Orbiter_Plugin::~Orbiter_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Orbiter_Plugin::Register()
//<-dceag-reg-e->
{
	// Check for all orbiters
	for(map<int,class DeviceData_Router *>::const_iterator it=m_pRouter->m_mapDeviceData_Router_get()->begin();it!=m_pRouter->m_mapDeviceData_Router_get()->end();++it)
	{
		DeviceData_Router *pDeviceData_Router=(*it).second;
		if( pDeviceData_Router->WithinCategory(DEVICECATEGORY_Orbiter_CONST) )
		{
			if( pDeviceData_Router->m_sMacAddress.size()==0 )
			{
				g_pPlutoLogger->Write(LV_WARNING,"Mobile Orbiter: %d %s doesn't have a mac address.  Skipping",
					pDeviceData_Router->m_iPK_Device,pDeviceData_Router->m_sDescription.c_str());

			}
			else
			{
				OH_Orbiter *pOH_Orbiter = new OH_Orbiter(pDeviceData_Router);
				m_mapOH_Orbiter[pDeviceData_Router->m_iPK_Device] = pOH_Orbiter;

				m_mapOH_Orbiter_Mac[StringUtils::ToUpper(pDeviceData_Router->m_sMacAddress)] = pOH_Orbiter;
			}
		}
	}

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterDetected))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_detected_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLinked))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_linked_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLost))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_lost_CONST);

	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom))
		,0,DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST,0,0,0,0);
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom))
		,0,DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST,0,0,0,0);
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom))
		,0,DEVICETEMPLATE_Orbiters_in_my_room_CONST,0,0,0,0);

	return Connect(); 

}

// Our message interceptor
bool Orbiter_Plugin::RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	if (!pDeviceFrom)
	{
		g_pPlutoLogger->Write(LV_WARNING,"GotRouteToOrbitersInRoom, but pDeviceFrom is NULL");
		return false;
	} 

	// If the sender was an orbiter, get the current room.  Otherwise the permanent room
	OH_Orbiter *pOH_Orbiter_From = m_mapOH_Orbiter_Find(pDeviceFrom->m_iPK_Device);
	int iPK_Room = pOH_Orbiter_From ? pOH_Orbiter_From->m_iPK_Room : pDeviceFrom->m_iPK_Room;

	string sDeviceList="";
	for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = (*it).second;
		if( pOH_Orbiter->m_iPK_Room != iPK_Room || pOH_Orbiter->m_pDeviceData_Router->m_iPK_Device==pDeviceFrom->m_iPK_Device )
			continue; // Don't send to the sender
		if( (pMessage->m_DeviceIDTo==DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST && pOH_Orbiter->m_pDeviceData_Router->m_iPK_DeviceCategory!=DEVICECATEGORY_Mobile_Orbiter_CONST) ||
				(pMessage->m_DeviceIDTo==DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST && pOH_Orbiter->m_pDeviceData_Router->m_iPK_DeviceCategory!=DEVICECATEGORY_Standard_Orbiter_CONST) )
			continue;
		if( sDeviceList.length() )
			sDeviceList += ",";
		sDeviceList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_iPK_Device);
	}
	if( sDeviceList.length() )
	{
		pMessage->m_DeviceIDTo = DEVICEID_LIST;
		pMessage->m_DeviceListTo = sDeviceList;
	}
	return false;  // Continue to process it
}

bool Orbiter_Plugin::MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
printf("Mobile orbiter detected\n");

	if (!pDeviceFrom)
	{
		g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
		return false;
	} 

	string sMacAddress = pMessage->m_Parameters[EVENTPARAMETER_Mac_Address_CONST];
	string sID = pMessage->m_Parameters[EVENTPARAMETER_ID_CONST];
	sMacAddress=StringUtils::ToUpper(sMacAddress);

	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

	if(!pOH_Orbiter)
	{
		m_mapUnknownDevices[sMacAddress] = pMessage->m_DeviceIDFrom;  // We need to remember who detected this device

		// We don't know what this is.  Let's see if it's a known phone, or anything else we recognize
		vector<Row_Device *> vectRow_Device;
		m_pDatabase_pluto_main->Device_get()->GetRows( DEVICE_MACADDRESS_FIELD + string("=") + sMacAddress, &vectRow_Device );
g_pPlutoLogger->Write(LV_STATUS,"found: %d rows in unknown devices for %s",(int) vectRow_Device.size(),sMacAddress.c_str());

		// If we have any records, then it's something that is already in our database, and it's not a phone, since it's
		// not in the OH_Orbiter map.  Just ignore it.
		if( !vectRow_Device.size() )
		{
			// Let's see if we've identified it before in the Unknown Devices table
			vector<Row_UnknownDevices *> vectRow_UnknownDevices;
			m_pDatabase_pluto_main->UnknownDevices_get()->GetRows( UNKNOWNDEVICES_MACADDRESS_FIELD + string("=") + sMacAddress, &vectRow_UnknownDevices );

			// If we found it here, then we've seen it before and the user already added it to the unknown table.  If not
			// we need to ask the user if it's a phone that he wants to use on the system.
			if( !vectRow_Device.size() )
			{
				g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_Parameters[EVENTPARAMETER_Mac_Address_CONST].c_str());

				// A list of all the orbiters we will notify of this device's presence
				string sOrbiterList = "";

				// First see if we can find orbiters in the same room
				if( pDeviceFrom )
				{
					for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
					{
						OH_Orbiter *pOH_Orbiter = (*it).second;
						if( pOH_Orbiter->m_iPK_Room == pDeviceFrom->m_iPK_Room )
							sOrbiterList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_iPK_Device) + ",";
					}
				}

				if( !sOrbiterList.size() )
				{
					// We didn't find any Orbiters.  Use them all
					for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
					{
						OH_Orbiter *pOH_Orbiter = (*it).second;
						sOrbiterList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_iPK_Device) + ",";
					}
				}

				g_pPlutoLogger->Write(LV_STATUS,"Orbiter list: %s",sOrbiterList.c_str());
				sOrbiterList="1"; // HACK!!!
				DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL(m_DeviceID,sOrbiterList,VARIABLE_Misc_Data_1_CONST,sMacAddress);
				DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL2(m_DeviceID,sOrbiterList,VARIABLE_Misc_Data_2_CONST,sID);
				DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL(m_DeviceID,sOrbiterList,0,StringUtils::itos(DESIGNOBJ_New_phone_detected_CONST),"","",true);

				// Send them all 3 in one message for efficiency
				CMD_Goto_Screen_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable_DL.m_pMessage );
				CMD_Goto_Screen_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable_DL2.m_pMessage );
				QueueMessage(CMD_Goto_Screen_DL.m_pMessage);
			}
		}
	}
	else
	{
		int SignalStrength = atoi(pMessage->m_Parameters[EVENTPARAMETER_Signal_Strength_CONST].c_str());

		if(pOH_Orbiter->m_pDevice_CurrentDetected == pDeviceFrom)
		{
			pOH_Orbiter->m_iLastSignalStrength = SignalStrength;
			g_pPlutoLogger->Write(LV_WARNING, "Redetecting the same device?");
		}
		else
		{
			if( pOH_Orbiter->m_pDevice_CurrentDetected )
			{
				DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(
					pDeviceFrom->m_iPK_Device, 
					-1, //pDeviceTo->m_iPK_Device,
					sMacAddress,
					&SignalStrength);

				if( SendCommand(CMD_Get_Signal_Strength) )
					pOH_Orbiter->m_iLastSignalStrength = SignalStrength;
				else
					pOH_Orbiter->m_iLastSignalStrength = 0;
			}

			if( pOH_Orbiter->m_pDevice_CurrentDetected && pOH_Orbiter->m_iLastSignalStrength > SignalStrength )
			{
				g_pPlutoLogger->Write(LV_STATUS,"Mobile Orbiter %s already has a strong association with %d (%d/%d)",
					sMacAddress.c_str(),
					pOH_Orbiter->m_pDevice_CurrentDetected->m_sDescription.c_str(),
					pOH_Orbiter->m_iLastSignalStrength,
					SignalStrength);

			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS,"Mobile Orbiter %s told to link with %d",
					sMacAddress.c_str(),
					pDeviceFrom->m_iPK_Device);
				DCE::CMD_Link_with_mobile_orbiter CMD_Link_with_mobile_orbiter(
					-1,
					pDeviceFrom->m_iPK_Device,
					//-1, //pDeviceTo->m_iPK_Device,
					1, //iMediaPosition = On
					sMacAddress);

				SendCommand(CMD_Link_with_mobile_orbiter);
			}
		}
	}
printf("finished processing\n");

	return true;
}

bool Orbiter_Plugin::MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	if (!pDeviceFrom)
	{
		g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
		return false;
	} 

	string sMacAddress = pMessage->m_Parameters[EVENTPARAMETER_Mac_Address_CONST];
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

	if(!pOH_Orbiter)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_Parameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
	}
	else
	{
		//pMobileOrbiter->m_pController->m_bReady=true;
		//pMobileOrbiter->m_pController->SetDefaultFlags();
		
		if(pOH_Orbiter->m_pDevice_CurrentDetected)
		{
//				pMobileOrbiter->RemovingAssocation();
				
//				ReceivedOCMessage(NULL,new OCMessage(DEVICEID_DCEROUTER,pMobileOrbiter->m_pDevice_CurrentDetected->m_iPKID_Device,
//					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,ACTION_LINK_WITH_MOBILE_ORBITER_CONST,2,C_ACTIONPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
//					C_ACTIONPARAMETER_ON_OFF_CONST,"0"));

		}

		DeviceData_Router *pDevice_PriorDetected = pOH_Orbiter->m_pDevice_CurrentDetected;
		
		// Associated with a new media director.  Show the corresponding menu
		pOH_Orbiter->m_pDevice_CurrentDetected = pDeviceFrom;

		DCE::CMD_Create_Mobile_Orbiter CMD_Create_Mobile_Orbiter(-1/*m_Device*/,pDeviceFrom->m_iPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_iPK_Device,sMacAddress);
		SendCommand(CMD_Create_Mobile_Orbiter);

		// See if there's an ent group involved

		//if( pOH_Orbiter->m_pEntGroupAudioZone_LockedOn )
		//{
		//	pOH_Orbiter->m_pController->m_pEntGroup = pMobileOrbiter->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
		//}


	}

	/*
	{
		else
		{
			pMobileOrbiter->m_pController->m_bReady=true;
			pMobileOrbiter->m_pController->SetDefaultFlags();
			// Remove any old association
			if( pMobileOrbiter->m_pDevice_CurrentDetected )
			{
				pMobileOrbiter->RemovingAssocation();
				
				ReceivedOCMessage(NULL,new OCMessage(DEVICEID_DCEROUTER,pMobileOrbiter->m_pDevice_CurrentDetected->m_iPKID_Device,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,ACTION_LINK_WITH_MOBILE_ORBITER_CONST,2,C_ACTIONPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
					C_ACTIONPARAMETER_ON_OFF_CONST,"0"));
			}

			Device *pDevice_PriorDetected = pMobileOrbiter->m_pDevice_CurrentDetected;
			// Associated with a new media director.  Show the corresponding menu
			pMobileOrbiter->m_pDevice_CurrentDetected=pDeviceFrom;
			// See if there's an ent group involved

			if( pMobileOrbiter->m_pEntGroupAudioZone_LockedOn )
			{
				pMobileOrbiter->m_pController->m_pEntGroup = pMobileOrbiter->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
			}
			else
			{
				pMobileOrbiter->m_pController->m_pEntGroup=NULL;
				if( pDeviceFrom->m_pDeviceParent )
				{
					Device *pEntGroup = m_pPlutoEvents->FindSibling(pDeviceFrom->m_pDeviceParent,MASTERDEVICELIST_ENTERTAIN_UNIT_CONST);
					if( pEntGroup )
					{
						pMobileOrbiter->m_pController->m_pEntGroup = m_pDataGridDevice->m_mapEntGroup_Find(pEntGroup->m_iPKID_Device);
					}
				}
			}

			if( pMobileOrbiter->m_pRoom_LockedOn )
				pMobileOrbiter->m_pDevice_This->m_pRoom=pMobileOrbiter->m_pRoom_LockedOn;
			else 
				pMobileOrbiter->m_pDevice_This->m_pRoom=pDeviceFrom->m_pRoom;

			pMobileOrbiter->NewAssocation(pDevice_PriorDetected);

			pMobileOrbiter->ShowMenu(0,*SafetyMessage); // 0=main menu
		}
	}
	*/
	return false;
}
bool Orbiter_Plugin::MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
{
	if (!pDeviceFrom)
	{
		g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
		return false;
	} 

	string sMacAddress = pMessage->m_Parameters[EVENTPARAMETER_Mac_Address_CONST];
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

	if(!pOH_Orbiter)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_Parameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
	}
	else
	{
		if(pOH_Orbiter->m_pDevice_CurrentDetected == pDeviceFrom)
		{
			/*
			if( pMobileOrbiter->m_pController->m_pEntGroup )
			{
				ReceivedOCMessage(NULL,new OCMessage(DEVICEID_DCEROUTER,DEVICEID_DCEROUTER,
					PRIORITY_NORMAL,MESSAGETYPE_COMMAND,ACTION_LINK_MEDIA_REMOTE_CONST,2,
					C_ACTIONPARAMETER_PKID_DEVICE_CONST,StringUtils::itos(pMobileOrbiter->m_pController->m_pEntGroup->m_PKID_EntGroup).c_str(),
					C_ACTIONPARAMETER_ON_OFF_CONST,"0"));
			}
			ReceivedOCMessage(NULL,new OCMessage(DEVICEID_DCEROUTER,pMobileOrbiter->m_pDevice_CurrentDetected->m_iPKID_Device,
				PRIORITY_NORMAL,MESSAGETYPE_COMMAND,ACTION_LINK_WITH_MOBILE_ORBITER_CONST,2,C_ACTIONPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
				C_ACTIONPARAMETER_ON_OFF_CONST,"0"));
			*/

			//pMobileOrbiter->RemovingAssocation();

			pOH_Orbiter->m_pDevice_CurrentDetected=NULL; 
		}
	}

	return true;
}

/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void Orbiter_Plugin::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

	// Send the message to a specific orbiter, identified by OrbiterID
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_DeviceID,OrbiterID,55,77);

	// Send the message to orbiters 32898 and 27283
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_DeviceID,"32898,27283",55,77);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_DeviceID,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_DeviceID,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);

	// This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
	string sResult="";
	bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_DeviceID,OrbiterID,55,77),sResult);
	// If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

	// A request is like a command, except that it has both "in" and "out" parameters, and the 
	// thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
	// events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
	// the socket is busy.  

	// We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
	// All out parameters are passed by reference.
	// Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
	// This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
	// request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
	// and there were multiple devices "Standard Plug-in" they would all get the message.
	PlutoDate plutoDate;
	char *FileContents;
	int FileSize;
	bool bResult = SendRequest(new DERC::REQ_File_Contents(m_DeviceID,DeviceTemplate_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);

	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();


}
*/

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c26-b->
/* 
	COMMAND: #26 - Set User Mode
	COMMENTS: Change a user's status (at home, sleeping, etc.)
	PARAMETERS:
		#5 Value To Assign
			A Value from the UserMode table
		#17 PK_Users
			The User to change
*/
void Orbiter_Plugin::CMD_Set_User_Mode(string sValue_To_Assign,int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c26-e->
{
	cout << "Need to implement command #26 - Set User Mode" << endl;
	cout << "Parm #5 - Value_To_Assign=" << sValue_To_Assign << endl;
	cout << "Parm #17 - PK_Users=" << iPK_Users << endl;
}

//<-dceag-c58-b->
/* 
	COMMAND: #58 - Set Current User
	COMMENTS: Set what user is currently using the orbiter.  The 'From' device is assumed to be the orbiter.
	PARAMETERS:
		#17 PK_Users
			The user currently using the orbiter.
*/
void Orbiter_Plugin::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_DeviceIDFrom);
	if( !pOH_Orbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_DeviceIDFrom);
		return;
	}

	pOH_Orbiter->m_iPK_Users=iPK_Users;
}

//<-dceag-c59-b->
/* 
	COMMAND: #59 - Set Entertainment Area
	COMMENTS: Set what entertainment area the orbiter (the 'from' in the message) is in.
	PARAMETERS:
		#45 PK_EntertainArea
			The current entertainment area where the orbiter is.
*/
void Orbiter_Plugin::CMD_Set_Entertainment_Area(int iPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_DeviceIDFrom);
	if( !pOH_Orbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_DeviceIDFrom);
		return;
	}

	pOH_Orbiter->m_iPK_EntertainArea=iPK_EntertainArea;
}

//<-dceag-sample-b->! no sample

//<-dceag-c77-b->
/* 
	COMMAND: #77 - Set Current Room
	COMMENTS: Set what room the orbiter is in.
	PARAMETERS:
		#57 PK_Room
			The room
*/
void Orbiter_Plugin::CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c77-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_DeviceIDFrom);
	if( !pOH_Orbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_DeviceIDFrom);
		return;
	}

	pOH_Orbiter->m_iPK_Room=iPK_Room;
}
//<-dceag-c78-b->
/* 
	COMMAND: #78 - New Mobile Orbiter
	COMMENTS: After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that 'yes' the device is a phone and needs the sof
	PARAMETERS:
		#44 PK_DeviceTemplate
			What type of phone it is.
		#47 Mac address
			The MAC Address of the phone.
*/
void Orbiter_Plugin::CMD_New_Mobile_Orbiter(int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c78-e->
{
	if( !iPK_DeviceTemplate )
		iPK_DeviceTemplate = DEVICETEMPLATE_Symbian_60_CONST;  // hack - todo fix this
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->AddRow();
	pRow_Device->FK_DeviceTemplate_set(iPK_DeviceTemplate);
	pRow_Device->MACaddress_set(sMac_address);
	m_pDatabase_pluto_main->Device_get()->Commit();
g_pPlutoLogger->Write(LV_STATUS,"new mobile orbiter, setting: %d to mac: %s",pRow_Device->PK_Device_get(),pRow_Device->MACaddress_get().c_str());
	// todo -- need to restart the dce router automatically
	int iPK_Device_Dongle = m_mapUnknownDevices[sMac_address];
	if( !iPK_Device_Dongle )
		g_pPlutoLogger->Write(LV_CRITICAL,"Got New Mobile Orbiter but can't find device!");
	else
	{
		DCE::CMD_Send_File_To_Device CMD_Send_File_To_Device(m_DeviceID, iPK_Device_Dongle,"PlutoMO.sis",sMac_address,"");
		SendCommand(CMD_Send_File_To_Device);
	}
}
