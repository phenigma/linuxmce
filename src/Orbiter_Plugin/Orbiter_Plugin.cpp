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

#include <sstream>

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Define_DesignObj.h"
#include "pluto_main/Define_Variable.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceCategory_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_DeviceData.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_FloorplanType.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_FloorplanObjectType.h"
#include "pluto_main/Table_FloorplanObjectType_Color.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_DHCPDevice.h"
#include "pluto_main/Table_Manufacturer.h"
#include "pluto_main/Table_Room.h"
#include "DCERouter.h"
#include "CreateDevice/CreateDevice.h"
#include "BD/PhoneDevice.h"

#include "PopulateListsInVMC.h"

#include <cctype>
#include <algorithm>

#define VERSION "<=version=>"

#include "../Media_Plugin/EntertainArea.h"
#include "../Media_Plugin/Media_Plugin.h"

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

	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows("1=1",&vectRow_Users);
	for(size_t s=0;s<vectRow_Users.size();++s)
		m_mapOH_User[ vectRow_Users[s]->PK_Users_get() ] = new OH_User(vectRow_Users[s]->PK_Users_get());

    m_bFloorPlansArePrepared = false;
	m_iThreshHold = DATA_Get_ThreshHold();
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Orbiter_Plugin::~Orbiter_Plugin()
//<-dceag-dest-e->
{
    m_mapUnknownDevices.clear();
	delete m_pDatabase_pluto_main;

	for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
		delete (*it).second;

	for(map<int,OH_User *>::iterator it=m_mapOH_User.begin();it!=m_mapOH_User.end();++it)
		delete (*it).second;
	delete g_pPlutoLogger;	// Created in either main or RegisterAsPlugin.  When this exits we won't need it anymore
}

/* Kind of a hack -- The goal was to allow the lighting, telecom, media, climate and security plug-ins to be
replaced with another model.  This plug-in needs to call them for handling floorplans and follow-me.
So rather than including including pointers directly to the plug-ins, I made generic 'follow-me plugin' and 'floorplan plugin'
classes which our lcmts plug-ins are derived from.  However I can't just cast the pointer to those plug-in which I
receive as pointers to Command_Impl, because they are not the first base class (ie multiple derived).  So, here the
lcmts plug-ins are included, the command_impl is cast to that class, then it's recast to a follow-me or floorplan plugin.
Don't know the best solution to allow for this type of abstraction. */

#include "../Lighting_Plugin/Lighting_Plugin.h"
#include "../Climate_Plugin/Climate_Plugin.h"
#include "../Media_Plugin/Media_Plugin.h"
#include "../Telecom_Plugin/Telecom_Plugin.h"
#include "../Security_Plugin/Security_Plugin.h"

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Orbiter_Plugin::Register()
//<-dceag-reg-e->
{
	// Find all the plugins
	ListCommand_Impl *pListCommand_Impl;

	m_pLighting_Floorplan=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Lighting_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Lighting plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	Lighting_Plugin *pLighting_Plugin = ( Lighting_Plugin * ) pListCommand_Impl->front( );
	m_pLighting_Floorplan=( FloorplanInfoProvider * ) pLighting_Plugin;

	m_pClimate_Floorplan=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Climate_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Climate plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	Climate_Plugin *pClimate_Plugin = ( Climate_Plugin * ) pListCommand_Impl->front( );
	m_pClimate_Floorplan=( FloorplanInfoProvider * ) pClimate_Plugin;

	m_pMedia_Floorplan=NULL;
	m_pMedia_Plugin=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Media_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Media plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	m_pMedia_Plugin=( Media_Plugin * ) pListCommand_Impl->front( );
	m_pMedia_Floorplan=( FloorplanInfoProvider * ) m_pMedia_Plugin;

	m_pSecurity_Floorplan=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Security_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Security plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	Security_Plugin *pSecurity_Plugin = ( Security_Plugin * ) pListCommand_Impl->front( );
	m_pSecurity_Floorplan=( FloorplanInfoProvider * ) pSecurity_Plugin;

	m_pTelecom_Floorplan=NULL;
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find( DEVICETEMPLATE_Telecom_Plugin_CONST );
	if( !pListCommand_Impl || pListCommand_Impl->size( )!=1 )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Orbiter cannot find Telecom plug-in %s", ( pListCommand_Impl ? "There were more than 1" : "" ) );
		return false;
	}
	Telecom_Plugin *pTelecom_Plugin = ( Telecom_Plugin * ) pListCommand_Impl->front( );
	m_pTelecom_Floorplan=( FloorplanInfoProvider * ) pTelecom_Plugin;

    // Check for all orbiters
    for(map<int,class DeviceData_Router *>::const_iterator it=m_pRouter->m_mapDeviceData_Router_get()->begin();it!=m_pRouter->m_mapDeviceData_Router_get()->end();++it)
    {
        DeviceData_Router *pDeviceData_Router=(*it).second;
        if( pDeviceData_Router->WithinCategory(DEVICECATEGORY_Orbiter_CONST) )
        {
            OH_Orbiter *pOH_Orbiter = new OH_Orbiter(pDeviceData_Router);
            m_mapOH_Orbiter[pDeviceData_Router->m_dwPK_Device] = pOH_Orbiter;
			m_sPK_Device_AllOrbiters += StringUtils::itos(pDeviceData_Router->m_dwPK_Device) + ",";
            if( pDeviceData_Router->m_sMacAddress.size()==0 )
            {
                g_pPlutoLogger->Write(LV_STATUS,"Orbiter: %d %s doesn't have a mac address.",
                    pDeviceData_Router->m_dwPK_Device,pDeviceData_Router->m_sDescription.c_str());

            }
            else
                m_mapOH_Orbiter_Mac[StringUtils::ToUpper(pDeviceData_Router->m_sMacAddress)] = pOH_Orbiter;
        }
    }


	for(map<string, OH_Orbiter *>::iterator iter = m_mapOH_Orbiter_Mac.begin(); iter != m_mapOH_Orbiter_Mac.end(); ++iter)
	{
		OH_Orbiter *pOH_Orbiter = (*iter).second;

		long dwPKDevice = pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device;
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(dwPKDevice);
		long dwDeviceTemplate = pRow_Device->FK_DeviceTemplate_get();
		Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(dwDeviceTemplate);
		string sDescription = pRow_DeviceTemplate->Description_get();
		string sSourceVMCFileName = sDescription + ".vmc";

		StringUtils::Replace(sSourceVMCFileName, "/", "-");
		StringUtils::Replace(sSourceVMCFileName, ":", "-");
		StringUtils::Replace(sSourceVMCFileName, "\\", "-");
		StringUtils::Replace(sSourceVMCFileName, "*", "-");
		StringUtils::Replace(sSourceVMCFileName, "?", "-");
		sSourceVMCFileName = "/usr/pluto/bin/" + sSourceVMCFileName;

		string sDestFileName = "/usr/pluto/bin/dev_" + StringUtils::ltos(dwPKDevice) + ".vmc";

		PopulateListsInVMC(sSourceVMCFileName, sDestFileName, dwPKDevice, m_pDatabase_pluto_main);
	}

    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterDetected) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_detected_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLinked) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_linked_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::MobileOrbiterLost),0,0,0,0,MESSAGETYPE_EVENT,EVENT_Mobile_orbiter_lost_CONST);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Standard_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Mobile_Orbiters_in_my_room_CONST,0,0,0,0);
    RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::RouteToOrbitersInRoom),0,DEVICETEMPLATE_Orbiters_in_my_room_CONST,0,0,0,0);
	RegisterMsgInterceptor((MessageInterceptorFn)(&Orbiter_Plugin::NewPnpDevice ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_New_PNP_Device_Detected_CONST );

    return Connect(PK_DeviceTemplate_get());

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

bool Orbiter_Plugin::SafeToReload()
{
	if( m_listRegenCommands.size()==0 )
		return true;

	g_pPlutoLogger->Write(LV_STATUS,"Cannot reboot %d pending",(int) m_listRegenCommands.size());
	for(list<int>::iterator it=m_listRegenCommands.begin();it!=m_listRegenCommands.end();++it)
	{
g_pPlutoLogger->Write(LV_STATUS,"Cannot reboot becaues of %d ",*it);
	}


	DisplayMessageOnOrbiter(0,"I'm still regenerating some Orbiter skins, and can't allow the reset until I'm finished.  You will get a message when I have finished.  Please try again then.",false,20,true);
	return false;
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
		UnknownDeviceInfos *pUnknownDeviceInfos_tmp = (*itUnknownDevice).second;
        if(false == pUnknownDeviceInfos_tmp->m_bProcessed) //unprocessed unknown device
        {
            pUnknownDeviceInfos_tmp->m_bProcessed = true; //mark as processed

            sMacAddress = (*itUnknownDevice).first;
            pUnknownDeviceInfos = pUnknownDeviceInfos_tmp;

            break;
        }
    }

    if(NULL == pUnknownDeviceInfos)
	{
		m_bNoUnknownDeviceIsProcessing = false;
        return; //no new unknown device to process
	}

	PhoneDevice pd("",sMacAddress,0);
	vector<Row_DHCPDevice *> vectRow_DHCPDevice;
	ostringstream sql;
	sql << "Mac_Range_Low<=" << pd.m_iMacAddress << " AND Mac_Range_High>=" << pd.m_iMacAddress;
	m_pDatabase_pluto_main->DHCPDevice_get()->GetRows(sql.str(),&vectRow_DHCPDevice);
    g_pPlutoLogger->Write(LV_WARNING, "search %s returned %d rows", sql.str().c_str(), (int) vectRow_DHCPDevice.size() );

	Row_DHCPDevice *pRow_DHCPDevice = vectRow_DHCPDevice.size() ? vectRow_DHCPDevice[0] : NULL;

	if( pRow_DHCPDevice && pRow_DHCPDevice->FK_DeviceCategory_get()==DEVICECATEGORY_Bluetooth_Dongles_CONST )
	{
	    g_pPlutoLogger->Write(LV_STATUS, "skipping detection of %s.  it's just a dongle",sMacAddress.c_str());
		DCE::CMD_Ignore_MAC_Address CMD_Ignore_MAC_Address(m_dwPK_Device,pUnknownDeviceInfos->m_iDeviceIDFrom,sMacAddress);
		SendCommand(CMD_Ignore_MAC_Address);
		m_bNoUnknownDeviceIsProcessing = false;
		return;
	}

    g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s", sMacAddress.c_str());

	string Description;
	if( pRow_DHCPDevice && pRow_DHCPDevice->FK_Manufacturer_get() )
		Description += "  Manufacturer: " + pRow_DHCPDevice->FK_Manufacturer_getrow()->Description_get();
	if( pRow_DHCPDevice && pRow_DHCPDevice->FK_DeviceCategory_get() )
		Description += "  Category: " + pRow_DHCPDevice->FK_DeviceCategory_getrow()->Description_get();
	if( pUnknownDeviceInfos->m_sID.length() )
		Description += "  Bluetooth ID: " + pUnknownDeviceInfos->m_sID;

    DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL(m_dwPK_Device, m_sPK_Device_AllOrbiters, VARIABLE_Misc_Data_1_CONST, sMacAddress);
    DCE::CMD_Set_Variable_DL CMD_Set_Variable_DL2(m_dwPK_Device, m_sPK_Device_AllOrbiters, VARIABLE_Misc_Data_2_CONST, Description);
    DCE::CMD_Goto_Screen_DL CMD_Goto_Screen_DL(m_dwPK_Device, m_sPK_Device_AllOrbiters, 0, StringUtils::itos(DESIGNOBJ_mnuNewPhoneDetected_CONST), "", "", true, false);

    // Send them all 3 in one message for efficiency
    CMD_Goto_Screen_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable_DL.m_pMessage );
    CMD_Goto_Screen_DL.m_pMessage->m_vectExtraMessages.push_back( CMD_Set_Variable_DL2.m_pMessage );
    QueueMessageToRouter(CMD_Goto_Screen_DL.m_pMessage);
}

bool Orbiter_Plugin::MobileOrbiterDetected(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
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

        g_pPlutoLogger->Write(LV_STATUS,"Found: %d rows in devices for %s",(int) vectRow_Device.size(),sMacAddress.c_str());

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
		/*
        if(pOH_Orbiter->m_iFailedToConnectCount >= 3) //if we tried to connect to this phone 3 times
        {
            g_pPlutoLogger->Write(LV_WARNING, "Failed to connect to the phone after 3 retries. We won't try again.");
            return true;
        }
		*/

        int SignalStrength = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Signal_Strength_CONST].c_str());

        if(pOH_Orbiter->m_pDevice_CurrentDetected == pDeviceFrom)
        {
            pOH_Orbiter->m_iLastSignalStrength = SignalStrength;
			g_pPlutoLogger->Write(LV_STATUS, "Redetecting the same device with strength: %d",pOH_Orbiter->m_iLastSignalStrength);
        }
        else
        {
            if( pOH_Orbiter->m_pDevice_CurrentDetected )
            {
/* TODO - this isn't working
                DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(
                    m_dwPK_Device,
					pDeviceFrom->m_dwPK_Device,
                    sMacAddress,
                    &SignalStrength);

                if( SendCommand(CMD_Get_Signal_Strength) )
                    pOH_Orbiter->m_iLastSignalStrength = SignalStrength;
                else
                    pOH_Orbiter->m_iLastSignalStrength = 0;
*/
            }
/*
			if( SignalStrength==0 )  // We don't know the signal strength from the dongle
			{
				if( pOH_Orbiter->m_iLastSignalStrength>threshhold )
					continue; // Do nothing, and don't allow a new connection
			}
			else
			{
				// We know both signal strengths.  Do a comparisson and take the stronger one
			}
*/
            if( pOH_Orbiter->m_pDevice_CurrentDetected &&
				pOH_Orbiter->m_iLastSignalStrength > m_iThreshHold &&
				pOH_Orbiter->m_iLastSignalStrength >= SignalStrength
			)
            {
                g_pPlutoLogger->Write(LV_STATUS,"Mobile Orbiter %s already has a strong association with %d (%d/%d)",
                    sMacAddress.c_str(),
                    pOH_Orbiter->m_pDevice_CurrentDetected->m_sDescription.c_str(),
                    pOH_Orbiter->m_iLastSignalStrength,
                    SignalStrength);

            }
            else
            {
                g_pPlutoLogger->Write(LV_STATUS,"Mobile Orbiter %s told to link with %d", sMacAddress.c_str(),
                    pDeviceFrom->m_dwPK_Device);

				if(NULL != pOH_Orbiter->m_pDevice_CurrentDetected)
				{
					DCE::CMD_Disconnect_From_Mobile_Orbiter cmd_Disconnect_From_Mobile_Orbiter(
						-1,
						pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,
						sMacAddress);
					SendCommand(cmd_Disconnect_From_Mobile_Orbiter);
				}

                DCE::CMD_Link_with_mobile_orbiter CMD_Link_with_mobile_orbiter(
                    -1,
                    pDeviceFrom->m_dwPK_Device,
                    1, //iMediaPosition = On
                    sMacAddress);

				SendCommand(CMD_Link_with_mobile_orbiter);
            }
        }
    }

    return true;
}

bool Orbiter_Plugin::MobileOrbiterLinked(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
    string sMacAddress = pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST];
    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

	if (!pDeviceFrom || !pOH_Orbiter)
    {
        g_pPlutoLogger->Write(LV_WARNING,"Got orbiter detected, but pDeviceFrom is NULL or unknown dev %s",pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
		return false;
    }
    string sVersion = pMessage->m_mapParameters[EVENTPARAMETER_Version_CONST];

    Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
    if( pRow_Device->NeedConfigure_get() == 1 || sVersion != VERSION )
    {
g_pPlutoLogger->Write(LV_STATUS,"Phone needs file - nc: %d version: %s / %s",(int) pRow_Device->NeedConfigure_get(),sVersion.c_str(),VERSION);
        pRow_Device->NeedConfigure_set(0);
        pRow_Device->Table_Device_get()->Commit();

        Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(pRow_Device->FK_DeviceTemplate_get());
        string PlutoMOInstaller = pRow_DeviceTemplate->CommandLine_get();

        DCE::CMD_Send_File_To_Device CMD_Send_File_To_Device(
            m_dwPK_Device,
			pDeviceFrom->m_dwPK_Device,
            PlutoMOInstaller,
            sMacAddress,
            ""
        );

        SendCommand(CMD_Send_File_To_Device);

        g_pPlutoLogger->Write(LV_WARNING, "Sending command CMD_Send_File_To_Device... PlutoMO file: %s, mac: %s", PlutoMOInstaller.c_str(), sMacAddress.c_str());
    }

    //pOH_Orbiter->m_iFailedToConnectCount = 0;//reset tries count

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

    DCE::CMD_Create_Mobile_Orbiter CMD_Create_Mobile_Orbiter(-1/*m_Device*/,pDeviceFrom->m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
		StringUtils::itos(pOH_Orbiter->m_pEntertainArea_Locked ? pOH_Orbiter->m_pEntertainArea_Locked->m_iPK_EntertainArea : 0),sMacAddress,
		pOH_Orbiter->m_dwPK_Room_Locked ? pOH_Orbiter->m_dwPK_Room_Locked : 0);
    SendCommand(CMD_Create_Mobile_Orbiter);

		// See if there's an ent group involved

    //if( pOH_Orbiter->m_pEntGroupAudioZone_LockedOn )
    //{
    //  pOH_Orbiter->m_pController->m_pEntGroup = pMobileOrbiter->m_pEntGroupAudioZone_LockedOn->m_pEntGroup;
    //}


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
    //bool bConnectionFailed = "0" != pMessage->m_mapParameters[EVENTPARAMETER_ConnectionFailed_CONST];

    OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Mac_Find(sMacAddress);

    if(!pOH_Orbiter)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Detected unknown bluetooth device %s",pMessage->m_mapParameters[EVENTPARAMETER_Mac_Address_CONST].c_str());
    }
    else
    {
		/*
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
		*/

        if(pOH_Orbiter->m_pDevice_CurrentDetected == pDeviceFrom)
        {
            pOH_Orbiter->m_pDevice_CurrentDetected=NULL;
			pOH_Orbiter->m_iLastSignalStrength = 0;

			FireFollowMe("LCTS",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
				0,pOH_Orbiter->m_dwPK_Room);
			FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
				0,pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0);
			pOH_Orbiter->m_dwPK_Room=0;
			pOH_Orbiter->m_pEntertainArea=NULL;

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

	int iPK_Users_Last = pOH_Orbiter->PK_Users_get();
	if( pOH_Orbiter->PK_Users_get() && pOH_Orbiter->PK_Users_get()!=iPK_Users && NULL != pOH_Orbiter->m_pDevice_CurrentDetected)
		FireFollowMe("LCMST",pOH_Orbiter->m_pDevice_CurrentDetected->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),0,0);
    pOH_Orbiter->m_pOH_User = m_mapOH_User_Find(iPK_Users);
	if( pOH_Orbiter->PK_Users_get() && pOH_Orbiter->PK_Users_get()!=iPK_Users_Last && pOH_Orbiter->m_dwPK_Room )
		FireFollowMe("LCST",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_dwPK_Room,0);
	if( pOH_Orbiter->PK_Users_get() && pOH_Orbiter->PK_Users_get()!=iPK_Users_Last && pOH_Orbiter->m_pEntertainArea )
		FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea,0);
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
	EntertainArea *pEntertainArea_Last = pOH_Orbiter->m_pEntertainArea;
    pOH_Orbiter->m_pEntertainArea=pEntertainArea;
	SetNowPlaying( pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, pEntertainArea && pEntertainArea->m_pMediaStream ? pEntertainArea->m_pMediaStream->m_sMediaDescription : "" );

	FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
		pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0,
		pEntertainArea_Last ? pEntertainArea_Last->m_iPK_EntertainArea : 0);
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

	int iPK_Room_Prior = pOH_Orbiter->m_dwPK_Room;
	pOH_Orbiter->m_dwPK_Room=iPK_Room;
	FireFollowMe("LCTS",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->PK_Users_get(),
		iPK_Room,iPK_Room_Prior);
}
//<-dceag-c78-b->

	/** @brief COMMAND: #78 - New Mobile Orbiter */
	/** After a new bluetooth device is detected, the Orbiter Handler will display a message on all the Orbiters prompting if this is a phone that should be added.  The Orbiters will fire this command to indicate that 'yes' the device is a phone and needs the sof */
		/** @param #17 PK_Users */
			/** The primary user of the phone. */
		/** @param #44 PK_DeviceTemplate */
			/** What type of phone it is. */
		/** @param #47 Mac address */
			/** The MAC Address of the phone. */

void Orbiter_Plugin::CMD_New_Mobile_Orbiter(int iPK_Users,int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c78-e->
{
    if( !iPK_DeviceTemplate )
        iPK_DeviceTemplate = DEVICETEMPLATE_Nokia_36503660_CONST;  // hack - todo fix this
//		send orbiter to prompt for mobile phone model

    printf("CMD_New_Mobile_Orbiter\n");

    int iFK_Room = 1;

	PLUTO_SAFETY_LOCK(mm, m_UnknownDevicesMutex);	
	UnknownDeviceInfos *pUnknownDeviceInfos = m_mapUnknownDevices[sMac_address];
	mm.Release();

    if( pUnknownDeviceInfos && pUnknownDeviceInfos->m_iDeviceIDFrom && pUnknownDeviceInfos->m_pDeviceFrom->m_pRoom)
        iFK_Room = pUnknownDeviceInfos->m_pDeviceFrom->m_pRoom->m_dwPK_Room;

	CreateDevice createDevice(m_pRouter->iPK_Installation_get(),m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get());
	int PK_Device = createDevice.DoIt(0,iPK_DeviceTemplate,"",sMac_address);

	g_pPlutoLogger->Write(
        LV_STATUS,
		"New mobile orbiter, setting: %d to mac: %s user: %d",
        PK_Device,
        sMac_address.c_str(), iPK_Users
    );

	Row_Device_DeviceData *pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->GetRow(PK_Device,DEVICEDATA_PK_Users_CONST);
	if( !pRow_Device_DeviceData )
	{
		pRow_Device_DeviceData = m_pDatabase_pluto_main->Device_DeviceData_get()->AddRow();
		pRow_Device_DeviceData->FK_Device_set(PK_Device);
		pRow_Device_DeviceData->FK_DeviceData_set(DEVICEDATA_PK_Users_CONST);
	}
	pRow_Device_DeviceData->IK_DeviceData_set( StringUtils::itos(iPK_Users) );
	pRow_Device_DeviceData->Table_Device_DeviceData_get()->Commit();

	// Same thing like in regen orbiter
	string Cmd = "/usr/pluto/bin/RegenOrbiterOnTheFly.sh " + StringUtils::itos(PK_Device) + " " + StringUtils::itos(m_dwPK_Device);
	m_listRegenCommands.push_back(PK_Device);
	g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",Cmd.c_str());
	FileUtils::LaunchProcessInBackground(Cmd);
	g_pPlutoLogger->Write(LV_STATUS,"Execution returned: %s",Cmd.c_str());

    // todo -- need to restart the dce router automatically
    if( !pUnknownDeviceInfos || !pUnknownDeviceInfos->m_iDeviceIDFrom )
        g_pPlutoLogger->Write(LV_CRITICAL,"Got New Mobile Orbiter but can't find device!");
    else
    {
        Row_DeviceTemplate *pRow_DeviceTemplate = m_pDatabase_pluto_main->DeviceTemplate_get()->GetRow(iPK_DeviceTemplate);
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
	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL( m_dwPK_Device, m_sPK_Device_AllOrbiters, StringUtils::itos(DESIGNOBJ_mnuNewPhoneDetected_CONST), "" );
    SendCommand(CMD_Remove_Screen_From_History_DL);

	DisplayMessageOnOrbiter(pMessage->m_dwPK_Device_From,"<%=T1111%>",false);

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
    pRow_Device->Description_set(sID);
    m_pDatabase_pluto_main->UnknownDevices_get()->Commit();

    g_pPlutoLogger->Write(
        LV_STATUS,
        "Added phone to unknown devices table, setting: %d to mac: %s",
        pRow_Device->PK_UnknownDevices_get(),
        pRow_Device->MacAddress_get().c_str()
    );

    m_bNoUnknownDeviceIsProcessing = false;

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL( m_dwPK_Device, m_sPK_Device_AllOrbiters, StringUtils::itos(DESIGNOBJ_mnuNewPhoneDetected_CONST), "" );
    SendCommand(CMD_Remove_Screen_From_History_DL);

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
					(fpObj->m_pDeviceData_Router ? fpObj->m_pDeviceData_Router->m_sDescription : fpObj->m_pEntertainArea->m_sDescription) + "|" +
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
		pFloorplanInfoProvider=m_pLighting_Floorplan;
		break;
	case FLOORPLANTYPE_Entertainment_Zone_CONST:
		pFloorplanInfoProvider=m_pMedia_Floorplan;
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

g_pPlutoLogger->Write(LV_STATUS, "get floorplan for page %d type %d map %p objs %d", Page, iPK_FloorplanType, pFloorplanObjectVectorMap,(fpObjVector ? (int) fpObjVector->size() : 0));

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

			pFloorplanInfoProvider->GetFloorplanDeviceInfo(pDeviceData_Router,fpObj->m_pEntertainArea,fpObj->Type,iPK_FloorplanObjectType_Color,iColor,sDescription,OSD);
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
g_pPlutoLogger->Write(LV_STATUS,"Preparing floorplan");
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;

        Row_Orbiter *pRow_Orbiter = m_pDatabase_pluto_main->Orbiter_get()->GetRow(pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
        if( !pRow_Orbiter )
        {
            g_pPlutoLogger->Write(LV_STATUS,"Cannot find Row_Orbiter for: %d",
                pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
            continue;
        }

        string s = pRow_Orbiter->FloorplanInfo_get();
        string::size_type pos=0;

        g_pPlutoLogger->Write(LV_STATUS, "This is a valid orbiter: %d fpinfo = %s", pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device, s.c_str());
        int NumDevices = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
g_pPlutoLogger->Write(LV_STATUS,"Preparing floorplan %d devices",NumDevices);
        for(int iDevice=0;iDevice<NumDevices;++iDevice)
        {
			// The device can be either a device, or an EntertainArea if this is the media floorplan
            int PK_Device = atoi( StringUtils::Tokenize(s, "\t", pos).c_str());
            g_pPlutoLogger->Write(LV_STATUS, "DeviceID: %d", PK_Device);
            DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(PK_Device);
			EntertainArea *pEntertainArea = NULL;

            if ( pDeviceData_Router == NULL )
            {
				pEntertainArea = m_pMedia_Plugin->m_mapEntertainAreas_Find(PK_Device);
				if( !pEntertainArea )
				{
					g_pPlutoLogger->Write(LV_CRITICAL, "Device referred by the floorplan %d for orbiter %d does not exist", PK_Device, pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device);
					// clean the bogus data
					StringUtils::Tokenize(s, "\t", pos);
					StringUtils::Tokenize(s, "\t", pos);
					StringUtils::Tokenize(s, "\t", pos);
					StringUtils::Tokenize(s, "\t", pos);
					continue;
				}
            }

			int FloorplanObjectType = pDeviceData_Router ?
				atoi(pDeviceData_Router->mapParameters_Find(DEVICEDATA_PK_FloorplanObjectType_CONST).c_str()) :
				m_pDatabase_pluto_main->EntertainArea_get()->GetRow(pEntertainArea->m_iPK_EntertainArea)->FK_FloorplanObjectType_get();
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
            g_pPlutoLogger->Write(LV_STATUS, "Got type %d page %d", FloorplanType,Page);

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
            fp->m_pDeviceData_Router = pDeviceData_Router;
			fp->m_pEntertainArea = pEntertainArea;
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
		return;
	}
	else
		pOH_Orbiter->m_bRegistered = sOnOff=="1";

	if( pOH_Orbiter->m_bRegistered )
	{
		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconl(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting ? "1" : "0","follow_light");
		SendCommand(CMD_Set_Bound_Iconl);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconm(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Media ? "1" : "0","follow_media");
		SendCommand(CMD_Set_Bound_Iconm);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Iconc(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate ? "1" : "0","follow_climate");
		SendCommand(CMD_Set_Bound_Iconc);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icons(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Security ? "1" : "0","follow_security");
		SendCommand(CMD_Set_Bound_Icons);

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icont(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom ? "1" : "0","follow_telecom");
		SendCommand(CMD_Set_Bound_Icont);
	}
}

//<-dceag-c261-b->

	/** @brief COMMAND: #261 - Set Follow-Me */
	/** Specifies whether the orbiter has follow-me on or off for the given user. */
		/** @param #2 PK_Device */
			/** The Orbiter */
		/** @param #9 Text */
			/** Can be 'L', 'M', 'C', 'S', 'T' for Lighting, Media, Climate, Security, Telecom followed by 0 or 1 for off/on. */
		/** @param #17 PK_Users */
			/** The User */

void Orbiter_Plugin::CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users,string &sCMD_Result,Message *pMessage)
//<-dceag-c261-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find( iPK_Device );
	if( !pOH_Orbiter || sText.length()<2 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Invalid set follow me: %d %s",iPK_Device,sText.c_str());
		return;
	}

	bool bOnOff = sText[1]=='0';

g_pPlutoLogger->Write(LV_STATUS,"Orbiter %d set follow me to %s for user %d",iPK_Device,sText.c_str(),iPK_Users);

	switch( sText[0] )
	{
	case 'L':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting : false,"follow_light");
			FireFollowMe("L",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	case 'M':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Media = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Media : false,"follow_media");
			FireFollowMe("M",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff && pOH_Orbiter->m_pEntertainArea ? pOH_Orbiter->m_pEntertainArea->m_iPK_EntertainArea : 0,0);
		}
		break;
	case 'C':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate : false,"follow_climate");
			FireFollowMe("C",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	case 'T':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom : false,"follow_telecom");
			FireFollowMe("T",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	case 'S':
		{
			if( pOH_Orbiter->m_pOH_User )
				pOH_Orbiter->m_pOH_User->m_bFollowMe_Security = bOnOff;
			SetBoundIcons(pOH_Orbiter->PK_Users_get(),pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_bFollowMe_Security : false,"follow_security");
			FireFollowMe("S",pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,	pOH_Orbiter->PK_Users_get(),bOnOff ? pOH_Orbiter->m_dwPK_Room : 0,0);
		}
		break;
	}
}
//<-dceag-c266-b->

	/** @brief COMMAND: #266 - Regen Orbiter */
	/** Regenerates an Orbiter.  When regeneration is complete, the "Regen Orbiter Finished" command will be sent */
		/** @param #2 PK_Device */
			/** The Orbiter to regenerate */
		/** @param #21 Force */
			/** Can be -r to force a full regen, or -a for a quick one */

void Orbiter_Plugin::CMD_Regen_Orbiter(int iPK_Device,string sForce,string &sCMD_Result,Message *pMessage)
//<-dceag-c266-e->
{
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;

		if( iPK_Device==0 || pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device==iPK_Device )
		{
			if( pOH_Orbiter->m_tRegenTime )
			{
				int Minutes = (time(NULL) - pOH_Orbiter->m_tRegenTime) /60;
				DisplayMessageOnOrbiter(iPK_Device,"We already started regenerating the orbiter " + pOH_Orbiter->m_pDeviceData_Router->m_sDescription + " " + StringUtils::itos(Minutes) +
					" minutes ago.  When it is finished, it will return to the main menu automatically.  If you think it is stuck, you may want to reset the Pluto system");
				return;
			}
			else
				pOH_Orbiter->m_tRegenTime = time(NULL);
		}
	}

	// Launch it in the background with &
	string Cmd = "/usr/pluto/bin/RegenOrbiterOnTheFly.sh " + StringUtils::itos(iPK_Device) + " " + StringUtils::itos(m_dwPK_Device) + " " + sForce;
	m_listRegenCommands.push_back(iPK_Device);
	g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",Cmd.c_str());
	FileUtils::LaunchProcessInBackground(Cmd);
	g_pPlutoLogger->Write(LV_STATUS,"Execution returned: %s",Cmd.c_str());
}

//<-dceag-c267-b->

	/** @brief COMMAND: #267 - Regen Orbiter Finished */
	/** Regeneration of the indicated Orbiter has been finished */
		/** @param #2 PK_Device */
			/** The Orbiter */

void Orbiter_Plugin::CMD_Regen_Orbiter_Finished(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c267-e->
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Device);
	if( !pOH_Orbiter )
	{
		// Must be a new orbiter.  We should restart the orbiter
		Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
		if( !pRow_Device )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Regenerate finished with unknown orbiter %d",iPK_Device);
			return;
		}

		// Send this to all orbiters
		DisplayMessageOnOrbiter(0,"Your new " + pRow_Device->Description_get() + " is ready to go.  "
			"All your devices must be reset to use it.  Press Quick Reload Router to do it now.  "
			"It takes about 20 seconds.  Phone calls will not be affected, but your media will.  "
			"Otherwise press the 'back' arrow and you can do it another time by choosing the option "
			"on the 'advanced' menu.",true,60);
		return;
	}
	pOH_Orbiter->m_tRegenTime = 0;
	g_pPlutoLogger->Write(LV_STATUS,"Regen finished for: %d size is: %d",iPK_Device,(int) m_listRegenCommands.size());

	for(list<int>::iterator it = m_listRegenCommands.begin(); it != m_listRegenCommands.end(); ++it)
	{
        if(*it == iPK_Device)
		{
			m_listRegenCommands.erase(it);
			break;
		}
	}
	g_pPlutoLogger->Write(LV_STATUS,"after Regen finished for: %d size is: %d",iPK_Device,(int) m_listRegenCommands.size());
}
//<-dceag-createinst-b->!


bool Orbiter_Plugin::NewPnpDevice( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	int PK_Device = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(PK_Device);
	if( !pRow_Device )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Got invalid pnp device: %d",PK_Device);
		return false;
	}

	DCE::CMD_Goto_Screen_DL CMD_Goto_Screen( m_dwPK_Device, m_sPK_Device_AllOrbiters, 0, StringUtils::itos(DESIGNOBJ_mnuNewPlugAndPlayDevice_CONST), StringUtils::itos(PK_Device), "", true, false );
	// The destination devices must match
	DCE::CMD_Set_Variable_DL CMD_Set_Variable1( m_dwPK_Device, m_sPK_Device_AllOrbiters, VARIABLE_Misc_Data_1_CONST, pRow_Device->Description_get());
	CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable1.m_pMessage);
	DCE::CMD_Set_Variable_DL CMD_Set_Variable2( m_dwPK_Device, m_sPK_Device_AllOrbiters, VARIABLE_Misc_Data_2_CONST, pRow_Device->FK_DeviceTemplate_getrow()->Comments_get());
	CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable2.m_pMessage);
	DCE::CMD_Set_Variable_DL CMD_Set_Variable3( m_dwPK_Device, m_sPK_Device_AllOrbiters, VARIABLE_Misc_Data_3_CONST, StringUtils::itos(PK_Device));
	CMD_Goto_Screen.m_pMessage->m_vectExtraMessages.push_back(CMD_Set_Variable3.m_pMessage);

	QueueMessageToRouter(CMD_Goto_Screen.m_pMessage);

	return false;  // Let anybody else have this who wants it
}

//<-dceag-c274-b->

	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #57 PK_Room */
			/** The room */

void Orbiter_Plugin::CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c274-e->
{
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(iPK_Room);
	if( !pRow_Device || !pRow_Room )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot set device %d to room %d",iPK_Device,iPK_Room);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Setting device %d to room %d",iPK_Device,iPK_Room);
			pRow_Device->FK_Room_set( pRow_Room->PK_Room_get() );
			pRow_Device->Table_Device_get()->Commit();
	}

	DCE::CMD_Remove_Screen_From_History_DL CMD_Remove_Screen_From_History_DL( m_dwPK_Device, m_sPK_Device_AllOrbiters, StringUtils::itos(DESIGNOBJ_mnuNewPlugAndPlayDevice_CONST), StringUtils::itos(iPK_Device) );
	SendCommand(CMD_Remove_Screen_From_History_DL);
}

void Orbiter_Plugin::FireFollowMe(string sMask,int iPK_Orbiter,int iPK_Users,int iPK_RoomOrEntArea,int iPK_RoomOrEntArea_Left)
{
	OH_Orbiter *pOH_Orbiter = m_mapOH_Orbiter_Find(iPK_Orbiter);
	if( !pOH_Orbiter )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot fire follow me for unknown orbiter: %d",iPK_Orbiter);
		return;
	}

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Lighting && sMask.find('L')!=string::npos )
		EVENT_Follow_Me_Lighting(iPK_Orbiter, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Climate && sMask.find('C')!=string::npos )
		EVENT_Follow_Me_Climate(iPK_Orbiter, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Security && sMask.find('S')!=string::npos )
		EVENT_Follow_Me_Security(iPK_Orbiter, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Telecom && sMask.find('T')!=string::npos )
		EVENT_Follow_Me_Telecom(iPK_Orbiter, iPK_RoomOrEntArea, iPK_Users, iPK_RoomOrEntArea_Left);

	if( pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_bFollowMe_Media && sMask.find('M')!=string::npos )
		EVENT_Follow_Me_Media(iPK_Orbiter, iPK_Users, iPK_RoomOrEntArea, iPK_RoomOrEntArea_Left);
}

void Orbiter_Plugin::SetBoundIcons(int iPK_Users,bool bOnOff,string sType)
{
    for(map<int,OH_Orbiter *>::iterator it=m_mapOH_Orbiter.begin();it!=m_mapOH_Orbiter.end();++it)
    {
        OH_Orbiter *pOH_Orbiter = (*it).second;
        if( pOH_Orbiter->PK_Users_get() != iPK_Users )
			continue; // Don't change

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,bOnOff ? "1" : "0",sType);
		SendCommand(CMD_Set_Bound_Icon);
	}
}
