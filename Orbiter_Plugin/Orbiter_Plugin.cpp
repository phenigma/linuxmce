/*
 Orbiter_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

//<-dceag-d-b->
#include "Orbiter_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Table_FloorplanObjectType.h"
#include "pluto_main/Table_FloorplanObjectType_Color.h"
#include "pluto_main/Table_Orbiter.h"
#include "DCERouter/DCERouter.h"

#include <cctype>
#include <algorithm>

#define VERSION "<=version=>"

#include "../Security_Plugin/Security_Plugin.h"
#include "../Lighting_Plugin/Lighting_Plugin.h"
#include "../Climate_Plugin/Climate_Plugin.h"
#include "../Media_Plugin/Media_Plugin.h"
#include "../Telecom_Plugin/Telecom_Plugin.h"

//<-dceag-const-b->!
Orbiter_Plugin::Orbiter_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Orbiter_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter), m_UnknownDevicesMutex("Unknown devices varibles")
//<-dceag-const-e->
{
    m_bNoUnknownDeviceIsProcessing = false;
    m_UnknownDevicesMutex.Init(NULL);

    m_pDatabase_pluto_main = new Database_pluto_main();
    if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
        m_bQuit=true;
        return;
    }

    m_bFloorPlansArePrepared = false;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Orbiter_Plugin::~Orbiter_Plugin()
//<-dceag-dest-e->
{
    m_mapUnknownDevices.clear();
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Orbiter_Plugin::Register()
//<-dceag-reg-e->
{
	// Find all the plugins
	ListCommand_Impl *pListCommand_Impl;

	m_pLighting_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Lighting_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Lighting plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	m_pLighting_Plugin=( Lighting_Plugin * ) pListCommand_Impl->front( );

	m_pClimate_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Climate_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Climate plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	m_pClimate_Plugin=( Climate_Plugin * ) pListCommand_Impl->front( );

	m_pMedia_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Media_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Media plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	m_pMedia_Plugin=( Media_Plugin * ) pListCommand_Impl->front( );

	m_pSecurity_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Security_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Security plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	m_pSecurity_Plugin=( Security_Plugin * ) pListCommand_Impl->front( );

	m_pTelecom_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Telecom_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Telecom plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	m_pTelecom_Plugin=( Telecom_Plugin * ) pListCommand_Impl->front( );

    // Check for all orbiters
    for(map<int,class DeviceData_Router *>::const_iterator it=m_pRouter->m_mapDeviceData_Router_get()->begin();it!=m_pRouter->m_mapDeviceData_Router_get()->end();++it)
    {
        DeviceData_Router *pDeviceData_Router=(*it).second;
        if( pDeviceData_Router->WithinCategory(DEVICECATEGORY_Orbiter_CONST) )
        {
            OH_Orbiter *pOH_Orbiter = new OH_Orbiter(pDeviceData_Router);
            m_mapOH_Orbiter[pDeviceData_Router->m_dwPK_Device] = pOH_Orbiter;
            if( pDeviceData_Router->m_sMacAddress.size()==0 )
            {
                g_pPlutoLogger->Write(LV_STATUS,"Mobile Orbiter: %d %s doesn't have a mac address.",
                    pDeviceData_Router->m_dwPK_Device,pDeviceData_Router->m_sDescription.c_str());

            }
            else
                m_mapOH_Orbiter_Mac[StringUtils::ToUpper(pDeviceData_Router->m_sMacAddress)] = pOH_Orbiter;
        }
    }

    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterDetected) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_detected_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLinked) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_linked_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLost),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_lost_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Orbiters_in_my_room_CONST,0,0,0,0);

    return Connect();

}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Orbiter_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void Orbiter_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

// Our message interceptor
bool Orbiter_Plugin::RouteToOrbitersInRoom(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    if (!pDeviceFrom)
    {
        g_pPlutoLogger->Write(LV_WARNING,"GotRouteToOrbitersInRoom, but pDeviceFrom is NULL");
        return false;
    }

    // If the sender was an orbiter, get the current room.  Otherwise the permanent room
    OH_Orbiter *pOH_Orbiter_From = m_mapOH_Orbiter_Find(pDeviceFrom->m_dwPK_Device);
    int iPK_Room = pOH_Orbiter_From ? pOH_Orbiter_From->m_dwPK_Room : pDeviceFrom->m_dwPK_Room;

    string sDeviceList="";
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        if( pOH_Orbiter->m_dwPK_Room != iPK_Room || pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device==pDeviceFrom->m_dwPK_Device )
            continue; // Don't send to the sender
        if( (pMessage->m_dwPK_Device_To==DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST && pOH_Orbiter->m_pDeviceData_Router->m_dwPK_DeviceCategory!=DEVICECATEGORY_Mobile_Orbiter_CONST) ||
                (pMessage->m_dwPK_Device_To==DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST && pOH_Orbiter->m_pDeviceData_Router->m_dwPK_DeviceCategory!=DEVICECATEGORY_Standard_Orbiter_CONST) )
            continue;
        if( sDeviceList.length() )
            sDeviceList += ",";
        sDeviceList += StringUtils::itos(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
    }
    if( sDeviceList.length() )
    {
        pMessage->m_dwPK_Device_To = DEVICEID_LIST;
        pMessage->m_sPK_Device_List_To = sDeviceList;
    }
    return false;  // Continue to process it
}

void Orbiter_Plugin::ProcessUnknownDevice()
{
g_pPlutoLogger->Write(LV_STATUS,"in process");
    m_bNoUnknownDeviceIsProcessing = true;

    PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);

    string sMacAddress;
    UnknownDeviceInfos *pUnknownDeviceInfos = NULL;

    map<string,UnknownDeviceInfos *>::iterator itUnknownDevice;
    for(itUnknownDevice = m_mapUnknownDevices.begin(); itUnknownDevice != m_mapUnknownDevices.end(); ++itUnknownDevice)
    {
        if(false == (*itUnknownDevice).second->m_bProcessed) //unprocessed unknown device
        {
            (*itUnknownDevice).second->m_bProcessed = true; //mark as processed

            sMacAddress = (*itUnknownDevice).first;
            pUnknownDeviceInfos = (*itUnknownDevice).second;

            break;
        }
    }

    if(NULL == pUnknownDeviceInfos)
        return; //no new unknown device to process

    g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s", sMacAddress.c_str());

    // A list of all the orbiters we will notify of this device's presence
    list<int> listOrbiter;

    // We didn't find any Orbiters.  Use them all
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        listOrbiter.push_back(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
    }

    string sOrbiterIDList;
    list<int>::iterator itOrbiter;
    for(itOrbiter = listOrbiter.begin(); itOrbiter != listOrbiter.end(); ++itOrbiter)
        sOrbiterIDList += StringUtils::itos(*itOrbiter) + ",";

    g_pPlutoLogger->Write(LV_STATUS,"Orbiter list: %s", sOrbiterIDList.c_str());

    for(itOrbiter = listOrbiter.begin(); itOrbiter != listOrbiter.end(); ++itOrbiter)
    {
        int iOrbiterID = *itOrbiter;

        DCE::CMD_Set_Variable CMD_Set_Variable(m_dwPK_Device, iOrbiterID, VARIABLE_Misc_Data_1_CONST, sMacAddress);
        DCE::CMD_Set_Variable CMD_Set_Variable2(m_dwPK_Device, iOrbiterID, VARIABLE_Misc_Data_2_CONST, pUnknownDeviceInfos->m_sID);
        DCE::CMD_Goto_Screen CMD_Goto_Screen(m_dwPK_Device, iOrbiterID, 0, StringUtils::itos(DESIGNOBJ_New_phone_detected_CONST), "", "", true);

        // Send them all 3 in one message for efficiency
        CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable.m_pMessage );
        CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable2.m_pMessage );
        QueueMessageToRouter(CMD_Goto_Screen.m_pMessage);
    }

    //TODO: Find why CMD_Set_Variable_DL is not working
    //CMD_Set_Variable_DL doesn't work!!!
    /*
    DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL(m_dwPK_Device, sOrbiterIDList, VARIABLE_Misc_Data_1_CONST, sMacAddress);
    DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL2(m_dwPK_Device, sOrbiterIDList, VARIABLE_Misc_Data_2_CONST, sID);
    DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL(m_dwPK_Device, sOrbiterIDList, 0, StringUtils::itos(DESIGNOBJ_New_phone_detected_CONST), "", "", true);

    // Send them all 3 in one message for efficiency
    CMD_Goto_Screen_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable_DL.m_pMessage );
    CMD_Goto_Screen_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable_DL2.m_pMessage );
    QueueMessageToRouter(CMD_Goto_Screen_DL.m_pMessage);*/

    listOrbiter.clear();
}

bool Orbiter_Plugin::MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
printf("Mobile orbiter detected\n");

    if (!pDeviceFrom)
    {
        g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
        return false;
    }

    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    string sID = pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST];
    sMacAddress=StringUtils::ToUpper(sMacAddress);

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

    if(!pOH_Orbiter)
    {
        PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);

        if(NULL == m_mapUnknownDevices_Find(sMacAddress))
            m_mapUnknownDevices[sMacAddress] = new UnknownDeviceInfos((DeviceData_Router *) pDeviceFrom, pMessage->m_dwPK_Device_From, sID);  // We need to remember who detected this device

        mm.Release();

        // We don't know what this is.  Let's see if it's a known phone, or anything else we recognize
        vector<Row_Device *> vectRow_Device;
        m_pDatabase_pluto_main->Device_get()->GetRows( DEVICE_MACADDRESS_FIELD + string("='") + sMacAddress + "' ", &vectRow_Device );

        g_pPlutoLogger->Write(LV_STATUS,"Found: %d rows in unknown devices for %s",(int) vectRow_Device.size(),sMacAddress.c_str());

        // If we have any records, then it's something that is already in our database, and it's not a phone, since it's
        // not in the OH_Orbiter map.  Just ignore it.
        if( !vectRow_Device.size() )
        {
            // Let's see if we've identified it before in the Unknown Devices table
            vector<Row_UnknownDevices *> vectRow_UnknownDevices;
            m_pDatabase_pluto_main->UnknownDevices_get()->GetRows( UNKNOWNDEVICES_MACADDRESS_FIELD + string("='") + sMacAddress + "' ", &vectRow_UnknownDevices );

            // If we found it here, then we've seen it before and the user already added it to the unknown table.  If not
            // we need to ask the user if it's a phone that he wants to use on the system.
            if( !vectRow_UnknownDevices.size() )
            {
g_pPlutoLogger->Write(LV_STATUS,"Need to process.  Bit flag is: %d",(int) m_bNoUnknownDeviceIsProcessing);

                if(!m_bNoUnknownDeviceIsProcessing) //the list was empty... we are processing the first unknown device
                    ProcessUnknownDevice();
            }
        }
    }
    else
    {
        if(pOH_Orbiter->m_iFailedToConnectCount >= 3) //if we tried to connect to this phone 3 times
        {
            g_pPlutoLogger->Write(LV_WARNING, "Failed to connect to the phone after 3 retries. We won't try again.");
            return true;
        }

        int SignalStrength = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Signal_Strength_CONST].c_str());

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
                    pDeviceFrom->m_dwPK_Device,
                    -1, //pDeviceTo->m_dwPK_Device,
                    sMacAddress,
                    &SignalStrength);

                if( SendCommand(CMD_Get_Signal_Strength) )
                    pOH_Orbiter->m_iLastSignalStrength = SignalStrength;
                else
                    pOH_Orbiter->m_iLastSignalStrength = 0;
            }

            if( pOH_Orbiter->m_pDevice_CurrentDetected && pOH_Orbiter->m_iLastSignalStrength >= SignalStrength )
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
                    pDeviceFrom->m_dwPK_Device);
                DCE::CMD_Link_with_mobile_orbiter CMD_Link_with_mobile_orbiter(
                    -1,
                    pDeviceFrom->m_dwPK_Device,
                    1, //iMediaPosition = On
                    sMacAddress);

                SendCommand(CMD_Link_with_mobile_orbiter);
            }
        }
    }
    printf("finished processing\n");

    return true;
}

bool Orbiter_Plugin::MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    if (!pDeviceFrom)
    {
        g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
        return false;
    }

    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

    if(!pOH_Orbiter)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
    }
    else
    {
        string sVersion = pMessage->m_mapParameters[EVENTPARAMETER_Version_CONST];

        Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
        if( pRow_Device->NeedConfigure_get() == 1 || sVersion != VERSION )
        {
            pRow_Device->NeedConfigure_set(0);
            pRow_Device->Table_Device_get()->Commit();

            Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(pRow_Device->FK_DeviceTemplate_get());
            string PlutoMOInstaller = pRow_DeviceTemplate->CommandLine_get();

            DCE::CMD_Send_File_To_Device CMD_Send_File_To_Device(
                m_dwPK_Device,
                pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
                PlutoMOInstaller,
                sMacAddress,
                ""
            );

            SendCommand(CMD_Send_File_To_Device);

            g_pPlutoLogger->Write(LV_WARNING, "Sending command CMD_Send_File_To_Device... PlutoMO file: %s, mac: %s", PlutoMOInstaller.c_str(), sMacAddress.c_str());
        }

        pOH_Orbiter->m_iFailedToConnectCount = 0;//reset tries count

        //pMobileOrbiter->m_pController->m_bReady=true;
        //pMobileOrbiter->m_pController->SetDefaultFlags();

        if(pOH_Orbiter->m_pDevice_CurrentDetected)
        {
//              pMobileOrbiter->RemovingAssocation();

//              ReceivedOCMessage(NULL,new OCMessage(DEVICEID_DCEROUTER,pMobileOrbiter->m_pDevice_CurrentDetected->m_iPKID_Device,
//                  PRIORITY_NORMAL,MESSAGETYPE_COMMAND,ACTION_LINK_WITH_MOBILE_ORBITER_CONST,2,C_ACTIONPARAMETER_ID_CONST,pMobileOrbiter->m_sID.c_str(),
//                  C_ACTIONPARAMETER_ON_OFF_CONST,"0"));

        }

        DeviceData_Router *pDevice_PriorDetected = pOH_Orbiter->m_pDevice_CurrentDetected;

        // Associated with a new media director.  Show the corresponding menu
        pOH_Orbiter->m_pDevice_CurrentDetected = (DeviceData_Router *) pDeviceFrom;

        DCE::CMD_Create_Mobile_Orbiter CMD_Create_Mobile_Orbiter(-1/*m_Device*/,pDeviceFrom->m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,sMacAddress);
        SendCommand(CMD_Create_Mobile_Orbiter);

        // See if there's an ent group involved

        //if( pOH_Orbiter->m_pEntGroupAudioZone_LockedOn )
        //{
        //  pOH_Orbiter->m_pController->m_pEntGroup = pMobileOrbiter->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
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
bool Orbiter_Plugin::MobileOrbiterLost(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    if (!pDeviceFrom)
    {
        g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL");
        return false;
    }

    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    bool bConnectionFailed = "0" != pMessage->m_mapParameters[EVENTPARAMETER_ConnectionFailed_CONST];

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

    if(!pOH_Orbiter)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
    }
    else
    {
        if(bConnectionFailed)
        {
            pOH_Orbiter->m_iFailedToConnectCount++;
            g_pPlutoLogger->Write(
                LV_WARNING,
                "Failed to connect to app on %s device, loop %d",
                sMacAddress.c_str(),
                pOH_Orbiter->m_iFailedToConnectCount
            );
        }

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


/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c58-b->

	/** @brief COMMAND: #58 - Set Current User */
	/** Set what user is currently using the orbiter.  The 'From' device is assumed to be the orbiter. */
		/** @param #17 PK_Users */
			/** The user currently using the orbiter. */

void Orbiter_Plugin::CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c58-e->
{
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
        return;
    }

    pOH_Orbiter->m_iPK_Users=iPK_Users;
}

//<-dceag-c59-b->

	/** @brief COMMAND: #59 - Set Entertainment Area */
	/** Set what entertainment area the orbiter (the 'from' in the message) is in. */
		/** @param #45 PK_EntertainArea */
			/** The current entertainment area where the orbiter is. */

void Orbiter_Plugin::CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,Message *pMessage)
//<-dceag-c59-e->
{
	EntertainArea *pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(atoi(sPK_EntertainArea.c_str()));
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
        return;
    }
	if( !pEntertainArea )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Could not find entertainment area object for : %s", sPK_EntertainArea.c_str());
		return;
	}
	
    pOH_Orbiter->m_pEntertainArea=pEntertainArea;
	SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pEntertainArea->m_pMediaStream ? pEntertainArea->m_pMediaStream->m_sMediaDescription : "" );
}

//<-dceag-sample-b->! no sample

//<-dceag-c77-b->

	/** @brief COMMAND: #77 - Set Current Room */
	/** Set what room the orbiter is in. */
		/** @param #57 PK_Room */
			/** The room */

void Orbiter_Plugin::CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c77-e->
{
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Set current user from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
        return;
    }

    pOH_Orbiter->m_dwPK_Room=iPK_Room;
}
//<-dceag-c78-b->

	/** @brief COMMAND: #78 - New Mobile Orbiter */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that 'yes' the device is a phone and needs the sof */
		/** @param #44 PK_DeviceTemplate */
			/** What type of phone it is. */
		/** @param #47 Mac address */
			/** The MAC Address of the phone. */

void Orbiter_Plugin::CMD_New_Mobile_Orbiter(int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c78-e->
{
    if( !iPK_DeviceTemplate )
        iPK_DeviceTemplate = DEVICETEMPLATE_Nokia_36503660_CONST;  // hack - todo fix this

    printf("CMD_New_Mobile_Orbiter\n");

    int iFK_Room = 1;
    UnknownDeviceInfos *pUnknownDeviceInfos = m_mapUnknownDevices[sMac_address];

    if( pUnknownDeviceInfos && pUnknownDeviceInfos->m_iDeviceIDFrom && pUnknownDeviceInfos->m_pDeviceFrom->m_pRoom)
        iFK_Room = pUnknownDeviceInfos->m_pDeviceFrom->m_pRoom->m_dwPK_Room;

    Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->AddRow();
    pRow_Device->FK_DeviceTemplate_set(iPK_DeviceTemplate);
    pRow_Device->MACaddress_set(sMac_address);
    pRow_Device->FK_Installation_set(m_pRouter->iPK_Installation_get());
    pRow_Device->FK_Room_set(iFK_Room);
    pRow_Device->Description_set("Mobile orbiter");
    m_pDatabase_pluto_main->Device_get()->Commit();

    g_pPlutoLogger->Write(
        LV_STATUS,
        "New mobile orbiter, setting: %d to mac: %s",
        pRow_Device->PK_Device_get(),
        pRow_Device->MACaddress_get().c_str()
    );

    // todo -- need to restart the dce router automatically
    if( !pUnknownDeviceInfos->m_iDeviceIDFrom )
        g_pPlutoLogger->Write(LV_CRITICAL,"Got New Mobile Orbiter but can't find device!");
    else
    {
        Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(pRow_Device->FK_DeviceTemplate_get());
        string PlutoMOInstaller = pRow_DeviceTemplate->CommandLine_get();

        DCE::CMD_Send_File_To_Device CMD_Send_File_To_Device(
            m_dwPK_Device,
            pUnknownDeviceInfos->m_iDeviceIDFrom,
            PlutoMOInstaller,
            sMac_address,
            ""
        );

        SendCommand(CMD_Send_File_To_Device);

        g_pPlutoLogger->Write(LV_WARNING, "Sending command CMD_Send_File_To_Device... PlutoMO file: %s, mac: %s", PlutoMOInstaller.c_str(), sMac_address.c_str());
    }

g_pPlutoLogger->Write(LV_STATUS,"setting process flag to false");
    m_bNoUnknownDeviceIsProcessing = false;

    ProcessUnknownDevice();
}
//<-dceag-c79-b->

	/** @brief COMMAND: #79 - Add Unknown Device */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that the device is a phone and it should be added */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

void Orbiter_Plugin::CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c79-e->
{
    Row_UnknownDevices *pRow_Device = m_pDatabase_pluto_main->UnknownDevices_get()->AddRow();
    pRow_Device->MacAddress_set(sMac_address);
    pRow_Device->IPAddress_set(sID);
    m_pDatabase_pluto_main->UnknownDevices_get()->Commit();

    g_pPlutoLogger->Write(
        LV_STATUS,
        "Added phone to unknown devices table, setting: %d to mac: %s",
        pRow_Device->PK_UnknownDevices_get(),
        pRow_Device->MacAddress_get().c_str()
    );

    m_bNoUnknownDeviceIsProcessing = false;

    ProcessUnknownDevice();
}


//<-dceag-c183-b->

	/** @brief COMMAND: #183 - Get Floorplan Layout */
	/** Gets the layout of all floorplans for the orbiter. */
		/** @param #5 Value To Assign */
			/** A | delimited list in the format, where {} indicate a repeating value: #pages,{#Types,{#Objects,{DeviceDescription, ObjectDescription, FillX Point, FillY Point, PK_DesignObj, Page, PK_Device, Type}}} */

void Orbiter_Plugin::CMD_Get_Floorplan_Layout(string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c183-e->
{
    if ( ! m_bFloorPlansArePrepared )
        PrepareFloorplanInfo();

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
    if( !pOH_Orbiter )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find orbiter for floorplan layout: %d",pMessage->m_dwPK_Device_From);
        return;
    }

    (*sValue_To_Assign) = StringUtils::itos((int) pOH_Orbiter->m_mapFloorplanObjectVector.size()) + "|";

    g_pPlutoLogger->Write(LV_STATUS, "Set value of floorplan layout to: %s", (*sValue_To_Assign).c_str());

    map<int,FloorplanObjectVectorMap *>::iterator itFloorplanObjectVectorMap;

    g_pPlutoLogger->Write(LV_STATUS, "Size of the map is: %d", pOH_Orbiter->m_mapFloorplanObjectVector.size() );
    for(itFloorplanObjectVectorMap = pOH_Orbiter->m_mapFloorplanObjectVector.begin();itFloorplanObjectVectorMap!=pOH_Orbiter->m_mapFloorplanObjectVector.end();itFloorplanObjectVectorMap++)
    {
        int Page = (*itFloorplanObjectVectorMap).first;
        FloorplanObjectVectorMap *pfpObjVectorMap = (*itFloorplanObjectVectorMap).second;

        if( !pfpObjVectorMap )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"There's a null in m_mapFloorplanObjectVector with Page: %d controller: %d",Page,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
            (*sValue_To_Assign) += StringUtils::itos(Page) + "|0|";
            continue;
        }

        (*sValue_To_Assign) += StringUtils::itos(Page) + "|" + StringUtils::itos((int) pfpObjVectorMap->size()) + "|";

        map<int,FloorplanObjectVector *>::iterator itFloorplanObjectVector;
        for(itFloorplanObjectVector = pfpObjVectorMap->begin();itFloorplanObjectVector!=pfpObjVectorMap->end();itFloorplanObjectVector++)
        {
            int Type = (*itFloorplanObjectVector).first;
            FloorplanObjectVector *pfpObjVector = (*itFloorplanObjectVector).second;

            if( !pfpObjVector )
            {
                // 2004 I don't fully understand why this null gets in there, but it's harmless--it just
                // means there are no floorplan objects of this type on the given page
                g_pPlutoLogger->Write(LV_STATUS,"There's a null in m_FloorplanObjectVector with Page: %d Type: %d Controller: %d",Page,Type,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
                (*sValue_To_Assign) += StringUtils::itos(Type) + "|0|";
                continue;
            }

            (*sValue_To_Assign) += StringUtils::itos(Type) + "|" +
                StringUtils::itos((int) pfpObjVector->size()) + "|";

            for(int i=0;i<(int) pfpObjVector->size();++i)
            {
                FloorplanObject *fpObj = (*pfpObjVector)[i];

                (*sValue_To_Assign) += fpObj->ObjectTypeDescription + "|" + StringUtils::itos(fpObj->FillX) + "|" + StringUtils::itos(fpObj->FillY) + "|" +
                    fpObj->ObjectID + "|" + StringUtils::itos(fpObj->Page) + "|" + StringUtils::itos(fpObj->PK_Device) + "|" +
                    StringUtils::itos(fpObj->Type) + "|";
            }
        }
    }
}

//<-dceag-c186-b->

	/** @brief COMMAND: #186 - Get Current Floorplan */
	/** Gets the current Floorplan status (ie what items are on/off, etc.) for the specified Floorplan type. */
		/** @param #5 Value To Assign */
			/** The status of all the devices within the floorplan. */
		/** @param #10 ID */
			/** The page number (ie the floorplan ID based). */
		/** @param #46 PK_FloorplanType */
			/** The type of floorplan (lights, climate, etc.) */

void Orbiter_Plugin::CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c186-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
	if( !pOH_Orbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find controller for floorplan: %d",pMessage->m_dwPK_Device_From);
		return;
	}
	int Page = atoi(sID.c_str());

	FloorplanInfoProvider *pFloorplanInfoProvider=NULL;
	switch( iPK_FloorplanType )
	{
	case FLOORPLANTYPE_Lighting_Zone_CONST:
		pFloorplanInfoProvider=m_pLighting_Plugin;
		break;
	case FLOORPLANTYPE_Entertainment_Zone_CONST:
		pFloorplanInfoProvider=m_pMedia_Plugin;
//		m_pMedia_Plugin->Populate_CurrentMediaOptions(Page,sValue_To_Assign);
		break;
/*
	case FLOORPLANTYPE_Climate_Zone_CONST:
		pFloorplanInfoProvider=m_pClimate_Plugin;
		break;
	case FLOORPLANTYPE_Phones_CONST:
		pFloorplanInfoProvider=m_pTelecom_Plugin;
		break;
	case FLOORPLANTYPE_Security_Zone_CONST:
		pFloorplanInfoProvider=m_pSecurity_Plugin;
		break;
*/
	};

	if( !pFloorplanInfoProvider )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Dont have a provider for floorplan: %d",pMessage->m_dwPK_Device_From);
		return;
	}

	FloorplanObjectVectorMap *pFloorplanObjectVectorMap = pOH_Orbiter->m_mapFloorplanObjectVector_Find(Page);
	FloorplanObjectVector *fpObjVector=NULL;
	if( pFloorplanObjectVectorMap )
		fpObjVector = (*pFloorplanObjectVectorMap)[iPK_FloorplanType];

	if( fpObjVector )
	{
		for(int i=0;i<(int) fpObjVector->size();++i)
		{
			string OSD="";
			int iPK_FloorplanObjectType_Color=0,iColor=0;
			string sDescription;
			Row_FloorplanObjectType_Color *pRow_FloorplanObjectType_Color = NULL;

			FloorplanObject *fpObj = (*fpObjVector)[i];
			DeviceData_Router *pDeviceData_Router = fpObj->m_pDeviceData_Router;

			pFloorplanInfoProvider->GetFloorplanDeviceInfo(pDeviceData_Router,fpObj->Type,iPK_FloorplanObjectType_Color,iColor,sDescription,OSD);
			if( iPK_FloorplanObjectType_Color )
				pRow_FloorplanObjectType_Color = m_pDatabase_pluto_main->FloorplanObjectType_Color_get()->GetRow(iPK_FloorplanObjectType_Color);

			if( !iColor && pRow_FloorplanObjectType_Color )
				iColor = pRow_FloorplanObjectType_Color->Color_get();
			if( sDescription.length()==0 && pRow_FloorplanObjectType_Color )
				sDescription = pRow_FloorplanObjectType_Color->Description_get();

			(*sValue_To_Assign) += StringUtils::itos(iColor) + "|" + sDescription + "|" + OSD + "|";
		}
	}
	if( (*sValue_To_Assign).length()==0 )
		(*sValue_To_Assign)="0";
}

void Orbiter_Plugin::PrepareFloorplanInfo()
{
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;

        Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
        if( !pRow_Orbiter )
        {
            g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find Row_Orbiter for: %d",
                pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
            continue;
        }

        string s = pRow_Orbiter->FloorplanInfo_get();
        string::size_type pos=0;

        g_pPlutoLogger->Write(LV_STATUS, "This is a valid orbiter: %d fpinfo = ", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, s.c_str());
        int NumDevices = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
        for(int iDevice=0;iDevice<NumDevices;++iDevice)
        {
            int PK_Device = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
            g_pPlutoLogger->Write(LV_STATUS, "DeviceID: %d", PK_Device);
            DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);

            if ( pDeviceData_Router == NULL )
            {
                g_pPlutoLogger->Write(LV_CRITICAL, "Device referred by the floorplan %d for orbiter %d does not exist", PK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
                // clean the bogus data
                StringUtils::Tokenize(s, "\t", pos);
                StringUtils::Tokenize(s, "\t", pos);
                StringUtils::Tokenize(s, "\t", pos);
                StringUtils::Tokenize(s, "\t", pos);
                continue;
            }

            int FloorplanObjectType = atoi(pDeviceData_Router->mapParameters_Find(DEVICEDATA_PK_FloorplanObjectType_CONST).c_str());
            Row_FloorplanObjectType *pRow_FloorplanObjectType = m_pDatabase_pluto_main->FloorplanObjectType_get()->GetRow(FloorplanObjectType);

            int Page = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
            string ObjectID = StringUtils::Tokenize(s, "\t", pos);
            int FillX = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
            int FillY = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());

            g_pPlutoLogger->Write(LV_STATUS, "Got this data for the floorplan %d %d %d, %s", Page, FillX, FillY, ObjectID.c_str());
            if ( pRow_FloorplanObjectType == NULL )
            {
                g_pPlutoLogger->Write(LV_STATUS, "Invalid Floorplan object type: %d", FloorplanObjectType);
                continue;
            }

            string Description = pRow_FloorplanObjectType->Description_get();
            g_pPlutoLogger->Write(LV_STATUS, "Got description %s", Description.c_str());
            int FloorplanType = pRow_FloorplanObjectType->FK_FloorplanType_get();
            g_pPlutoLogger->Write(LV_STATUS, "Got type %d", FloorplanType);

            FloorplanObjectVectorMap *pFpObjMap = pOH_Orbiter->m_mapFloorplanObjectVector_Find(Page);
            if( !pFpObjMap )
            {
                pFpObjMap = new FloorplanObjectVectorMap();
                pOH_Orbiter->m_mapFloorplanObjectVector[Page] = pFpObjMap;
            }

            FloorplanObjectVector *fpVect = (*pFpObjMap)[FloorplanType];
            if( !fpVect )
            {
                fpVect = new FloorplanObjectVector();
                (*pFpObjMap)[FloorplanType] = fpVect;
            }
            FloorplanObject *fp = new FloorplanObject();
            fp->PK_Device = PK_Device;
            fp->m_pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
            fp->Type = FloorplanObjectType;
            fp->Page = Page;
            fp->FillX = FillX;
            fp->FillY = FillY;
            fp->ObjectID = ObjectID;;
            fp->ObjectTypeDescription = Description;
            fpVect->push_back(fp);
        }
    }

    m_bFloorPlansArePrepared = true;
}
//<-dceag-c255-b->

	/** @brief COMMAND: #255 - Orbiter Registered */
	/** Indicates the orbiter has registered, or unregistered */
		/** @param #8 On/Off */
			/** 1 means it is registering, 0 means it is closing */

void Orbiter_Plugin::CMD_Orbiter_Registered(string sOnOff,string &sCMD_Result,Message *pMessage)
//<-dceag-c255-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find( pMessage->m_dwPK_Device_From );
	if( !pOH_Orbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Received registration from unknown orbiter: %d",pMessage->m_dwPK_Device_From);
	}
	else
		pOH_Orbiter->m_bRegistered = sOnOff=="1";
}

