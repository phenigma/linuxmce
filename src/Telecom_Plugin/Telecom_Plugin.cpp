/*
 Telecom_Plugin

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

//<-dceag-d-b->
#include "Telecom_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"

#include "Gen_Devices/AllScreens.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_telecom/Database_pluto_telecom.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_Screen.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_MediaType.h"
#include "pluto_main/Table_Users.h"
#include "pluto_telecom/Table_Contact.h"
#include "pluto_telecom/Table_PhoneNumber.h"
#include "pluto_telecom/Table_PhoneType.h"
#include "Orbiter_Plugin/Orbiter_Plugin.h"
#include "PlutoUtils/ProcessUtils.h"

#include "DCE/DCEConfig.h"
#include "PlutoUtils/DBHelper.h"
#include "TelecomTask.h"
#include "AssistedTransfer.h"

#include "SerializeClass/ShapesColors.h"
#define MAX_TELECOM_COLORS 5
int UniqueColors[MAX_TELECOM_COLORS];
#include <time.h>

#ifndef WIN32
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#endif

#define CONFERENCE_MAX_NO 50

#define SPEAKINTHEHOUSE_INVALID_EXT "555"
#define SPEAKINTHEHOUSE_CONFERENCE_IVR "997"
#define SPEAKINTHEHOUSE_CONFERENCE_ALL "998"

void * startDisplayThread(void * Arg);

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Telecom_Plugin::Telecom_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Telecom_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_TelecomMutex("vm mutex")
{
	m_pDatabase_pluto_main = NULL;
	m_pDatabase_pluto_telecom = NULL;
	pthread_mutex_init(&mtx_err_messages,0);
	iCmdCounter = 0;
	next_conf_room = 1;
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
	m_TelecomMutex.Init(&m_MutexAttr);
	m_pDevice_pbx=NULL;
	m_displayThread = (pthread_t)0;
	TelecomTask::SetTelecom(this);
}

//<-dceag-getconfig-b->
bool Telecom_Plugin::GetConfig()
{
	if( !Telecom_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit_set(true);
		return false;
	}

	m_pDatabase_pluto_telecom = new Database_pluto_telecom(LoggerWrapper::GetInstance());
	if(!m_pDatabase_pluto_telecom->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),"pluto_telecom",m_pRouter->iDBPort_get()) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to telecom database!");
		m_bQuit_set(true);
		return false;
	}
	vector<class Row_Device*> vectDevices;
	m_pDatabase_pluto_main->Device_get()->GetRows(DEVICE_FK_DEVICETEMPLATE_FIELD + string("=") + StringUtils::itos(DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST), &vectDevices);
	for(size_t s=0;s<vectDevices.size();s++)
	{
		map_orbiter2embedphone[vectDevices[s]->FK_Device_ControlledVia_get()] = vectDevices[s]->PK_Device_get();
		map_embedphone2orbiter[vectDevices[s]->PK_Device_get()] = vectDevices[s]->FK_Device_ControlledVia_get();
	}
	vector<class Row_Device_DeviceData*> vectDeviceData;
	m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows(DEVICE_DEVICEDATA_FK_DEVICEDATA_FIELD+string("=") + StringUtils::itos(DEVICEDATA_PhoneNumber_CONST), &vectDeviceData);
	for(size_t s=0;s<vectDeviceData.size();s++)
	{
		map_ext2device[vectDeviceData[s]->IK_DeviceData_get()] = vectDeviceData[s]->FK_Device_get();
		map_device2ext[vectDeviceData[s]->FK_Device_get()] = vectDeviceData[s]->IK_DeviceData_get();

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Phone device %d with extension %s",
			vectDeviceData[s]->FK_Device_get(), vectDeviceData[s]->IK_DeviceData_get().c_str());
	}
	UniqueColors[0] = PlutoColor(128,0,0).m_Value;
	UniqueColors[1] = PlutoColor(0,128,0).m_Value;
	UniqueColors[2] = PlutoColor(0,0,128).m_Value;
	UniqueColors[3] = PlutoColor(0,128,128).m_Value;
	UniqueColors[4] = PlutoColor(128,128,0).m_Value;

	//get status for voice mail
	vector<Row_Users *> vectRow_Users;
	m_pDatabase_pluto_main->Users_get()->GetRows(USERS_FK_INSTALLATION_MAIN_FIELD + string("=") + 
		StringUtils::itos(m_pRouter->iPK_Installation_get())+string(" OR ")+
		string(USERS_FK_INSTALLATION_MAIN_FIELD)+string(" IS NULL"),&vectRow_Users);

	for(vector<Row_Users *>::iterator it = vectRow_Users.begin(), end = vectRow_Users.end(); it != end; ++it)
	{
		Row_Users *pRow_Users = *it;
		// TODO pRow_Users->Extension_get() should return string
		// eugen & chris
		int iExtension = pRow_Users->Extension_get();
		string sExtension = StringUtils::ltos(iExtension);
		string sUserName = pRow_Users->UserName_get();
		
		if( !sExtension.empty() && !sUserName.empty() )
		{
			map_username2ext[sUserName] = sExtension;
			map_ext2username[sExtension] = sUserName;
			
			const char * args[] = { "/usr/pluto/bin/SendVoiceMailEvent.sh", sExtension.c_str(), NULL };
			ProcessUtils::SpawnDaemon(args[0], (char**)args);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Telecom_Plugin::GetConfig - user %s, ext %d",
												sUserName.c_str(), iExtension);
		}
	}

	m_pDevice_pbx = find_AsteriskDevice();
	if( !m_pDevice_pbx )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Telecom_Plugin::GetConfig - no pbx device");
		return false;
	}

	
	DCE::CMD_Send_Asterisk_Status cmd_Send_Asterisk_Status(m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device);
	cmd_Send_Asterisk_Status.m_pMessage->m_eRetry = MR_Retry;
	SendCommand(cmd_Send_Asterisk_Status);

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Telecom_Plugin::~Telecom_Plugin()
//<-dceag-dest-e->
{
	CleanStatusMaps();
	
	// clean the tasks map
	for(map<string, TelecomTask*>::iterator it=map_id2task.begin(); it!=map_id2task.end(); ++it)
	{
		delete (*it).second;
	}
	map_id2task.clear();
	
	delete m_pDatabase_pluto_main;
	delete m_pDatabase_pluto_telecom;

	pthread_mutex_destroy(&m_TelecomMutex.mutex);
}

void Telecom_Plugin::PrepareToDelete()
{
	Command_Impl::PrepareToDelete();
	if(m_displayThread)
	{
		pthread_join(m_displayThread,NULL);
		m_displayThread = (pthread_t)0;
	}
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Telecom_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=( Datagrid_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Datagrid_Plugin_CONST);
	m_pOrbiter_Plugin=( Orbiter_Plugin * ) m_pRouter->FindPluginByTemplate(DEVICETEMPLATE_Orbiter_Plugin_CONST);
	if( !m_pDatagrid_Plugin || !m_pOrbiter_Plugin )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Cannot find sister plugins to telecom plugin");
		return false;
	}

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::TelecomScenariosGrid))
		,DATAGRID_Telecom_Scenarios_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::PhoneBookAutoCompl))
		,DATAGRID_Phone_Book_Auto_Compl_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::PhoneBookListOfNos))
		,DATAGRID_Phone_Book_List_of_Nos_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::ActiveCallsGrid))
		,DATAGRID_Floorplan_Telecom_Streams_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::RecentCallsGrid)),
		DATAGRID_Recent_Calls_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::SpeedDialGrid)),
		DATAGRID_Speed_Dial_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::UserVoiceMailGrid)),
		DATAGRID_User_VoiceMail_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::ActiveCallsGrid))
		,DATAGRID_Active_Channels_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::ActiveUsersOnCallGrid))
		,DATAGRID_Active_Users_On_Channel_CONST,PK_DeviceTemplate_get());

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::ExternalChannels))
		,DATAGRID_External_Channels_CONST,PK_DeviceTemplate_get());

	//RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::CommandResult ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_PBX_CommandResult_CONST );

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::OrbiterRegistered) ,0,0,0,0,MESSAGETYPE_COMMAND,COMMAND_Orbiter_Registered_CONST);

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::VoIP_Problem) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_VoIP_Problem_Detected_CONST);
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::VoiceMailChanged) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_Voice_Mail_Changed_CONST);
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::TelecomFollowMe ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Follow_Me_Telecom_CONST );

	//telecom update events
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::Ring ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_PBX_Ring_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::Link) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_PBX_Link_CONST);
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::IncomingCall ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Incoming_Call_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::Hangup) ,0,0,0,0,MESSAGETYPE_EVENT,EVENT_PBX_Hangup_CONST);
	
	//telecom snapshot status
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::CallsStatusChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Calls_Status_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::ExtensionsStatusChanged), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Extensions_Status_CONST );
	
    if (pthread_create(&m_displayThread, NULL, startDisplayThread, (void *) this))
    {
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to create Display Thread");
        m_bQuit_set(true);
        exit(1);
    }

	return Connect(PK_DeviceTemplate_get());
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
void Telecom_Plugin::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Telecom_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

class DataGridTable *Telecom_Plugin::TelecomScenariosGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	vector<Row_CommandGroup *> vectRowCommandGroup;
	m_pDatabase_pluto_main->CommandGroup_get()->GetRows( COMMANDGROUP_FK_ARRAY_FIELD + string("=") + StringUtils::itos(ARRAY_Communication_Scenarios_CONST) + " AND "
			+ COMMANDGROUP_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pRouter->iPK_Installation_get()),&vectRowCommandGroup );
	for(size_t s=0;s<vectRowCommandGroup.size();++s)
	{
		Row_CommandGroup *pRow_CommandGroup = vectRowCommandGroup[s];
		pCell = new DataGridCell(pRow_CommandGroup->Description_get(),StringUtils::itos(pRow_CommandGroup->PK_CommandGroup_get()));
		pDataGrid->SetData(0,(int) s,pCell);
	}

	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::PhoneBookAutoCompl(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Phone Book Autocomplete request received for GridID: %s with Params: %s.",
																					GridID.c_str(), Parms.c_str());

	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	// get uid and filter text
	string::size_type pos=0;
	string sPK_Users = StringUtils::Tokenize(Parms,"|",pos);
	string sFind = StringUtils::Tokenize(Parms,"|",pos);
	if( sFind.size()==0 )
		return pDataGrid;

	string sWhere = "(Name like '" + StringUtils::SQLEscape(sFind) + "%' OR Company like '" + StringUtils::SQLEscape(sFind) + "%') "
		"AND (EK_Users IS NULL OR EK_Users=" + sPK_Users + ")";
	vector<Row_Contact *> vectRow_Contact;
	m_pDatabase_pluto_telecom->Contact_get()->GetRows(sWhere,&vectRow_Contact);
	size_t FirstBatch;  // Counter used to remember how many rows we added during the first sql query
	string sPK;  // PK's we got during the first scan so we can exclude them on the second
	for(FirstBatch=0;FirstBatch<vectRow_Contact.size();++FirstBatch)
	{
		Row_Contact *pRow_Contact = vectRow_Contact[FirstBatch];
		string sDescription;
		if( pRow_Contact->Name_get().size() )
		{
			sDescription += pRow_Contact->Name_get();
			if( pRow_Contact->Title_get().size() )
				sDescription += "," + pRow_Contact->Title_get();
			sDescription += "\n";
		}
		if( pRow_Contact->Company_get().size() )
			sDescription += pRow_Contact->Company_get() + "\n";

		if( sPK.size() )
			sPK += "," + StringUtils::itos(pRow_Contact->PK_Contact_get());
		else
			sPK = StringUtils::itos(pRow_Contact->PK_Contact_get());

		pCell = new DataGridCell(sDescription,StringUtils::itos(pRow_Contact->PK_Contact_get()) );
		pDataGrid->SetData(0, (int)FirstBatch, pCell);
	}

	sWhere = (sPK.size() ? "PK_Contact NOT IN("+sPK+") AND " : "") +
		"(Name like '% " + StringUtils::SQLEscape(sFind) + "%' OR Company like '% " + StringUtils::SQLEscape(sFind) + "%') "
		"AND (EK_Users IS NULL OR EK_Users=" + sPK_Users + ")";
	vectRow_Contact.clear();
	m_pDatabase_pluto_telecom->Contact_get()->GetRows(sWhere,&vectRow_Contact);
	for(size_t s=0;s<vectRow_Contact.size();++s)
	{
		Row_Contact *pRow_Contact = vectRow_Contact[s];
		string sDescription;
		if( pRow_Contact->Name_get().size() )
		{
			sDescription += pRow_Contact->Name_get();
			if( pRow_Contact->Title_get().size() )
				sDescription += ", " + pRow_Contact->Title_get();
			sDescription += "\n";
		}
		if( pRow_Contact->Company_get().size() )
			sDescription += pRow_Contact->Company_get() + "\n";

		pCell = new DataGridCell(sDescription, StringUtils::itos(pRow_Contact->PK_Contact_get()));
		pDataGrid->SetData(0, (int)(FirstBatch + s), pCell);
	}

	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::PhoneBookListOfNos(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Phone Book List of NOS request received for GridID: %s with Params: %s.",
																					GridID.c_str(), Parms.c_str());

	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;

	if( Parms.size()==0 )
		return pDataGrid;
	Row_Contact *pRow_Contact = m_pDatabase_pluto_telecom->Contact_get()->GetRow( atoi(Parms.c_str()) );
	if( !pRow_Contact )
		return pDataGrid; // Shouldn't happen

	vector<Row_PhoneNumber *> vectRow_PhoneNumber;
	pRow_Contact->PhoneNumber_FK_Contact_getrows(&vectRow_PhoneNumber);
	int Row=0;
	for(size_t s=0;s<vectRow_PhoneNumber.size();++s)
	{
		Row_PhoneNumber *pRow_PhoneNumber = vectRow_PhoneNumber[s];
		Row_PhoneType *pRow_PhoneType = pRow_PhoneNumber->FK_PhoneType_getrow();
		string sPhoneType,sDescription,sDial=GetDialNumber(pRow_PhoneNumber);

		if( pRow_PhoneType )
			sPhoneType = pRow_PhoneType->Description_get();

		if( pRow_PhoneNumber->CountryCode_get().size() )
			sDescription += "+" + pRow_PhoneNumber->CountryCode_get() + " ";
		if( pRow_PhoneNumber->AreaCode_get().size() )
			sDescription += "(" + pRow_PhoneNumber->AreaCode_get() + ") ";
		if( pRow_PhoneNumber->PhoneNumber_get().size() )
			sDescription += pRow_PhoneNumber->PhoneNumber_get();

		if( sDescription.size()==0 )
			sDescription = pRow_PhoneNumber->DialAs_get();

		if( pRow_PhoneNumber->Extension_get().size() )
			sDescription += "x." + pRow_PhoneNumber->Extension_get();

		if( sDescription.size()==0 )
			continue;

		pCell = new DataGridCell(pRow_PhoneType->Description_get(), sDial);
		pDataGrid->SetData(0,Row, pCell);

		pCell = new DataGridCell(sDescription, sDial);
		pCell->m_Colspan=2;
		pDataGrid->SetData(1,Row, pCell);

		Row++;
	}

	return pDataGrid;
}

bool
Telecom_Plugin::CallsStatusChanged(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Calls status received from PBX.");
	string sStatus = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST].c_str();

	//sample:
	//Channel:Context:Exten:Priority:Stats:Application:Data:CallerID:Accountcode:Amaflags:Duration:Bridged 
	
	//NON-CONFERENCE
	//SIP/200-c190:from-internal::1:Up:Bridged Call:SIP/203-8361:200::3::SIP/203-8361
	//SIP/203-8361:macro-dial:s:10:Up:Dial:SIP/200|15|tr:203::3:29:SIP/200-c190
	//-> callid "call 200-203" has extensions (200, c190), (203, 8361), state non-conf

	//CONFERENCE
	//SIP/201-8104:trusted:0002:1:Up:Conference:0002/S/1:201::3:19:(None)
	//SIP/200-c190:trusted:0002:1:Up:Conference:0002/S/1:200::3:27:(None)
	//SIP/203-8361:trusted:0002:1:Up:Conference:0002/S/1:203::3:760:(None)
	//-> callid "conference 0002" has extensions (200, c190), (201, 8104), (203, 8361), state conference

	typedef multimap<string, pair<string, string> > MAP_CHANNELS;
	typedef map<string, bool> MAP_CALLS;
	
	MAP_CHANNELS mapChannels; //call id (conference number or 1st channel) <-> (channel, callerid)
	MAP_CALLS mapCalls; //call id <-> is conference ?
	
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data
	
	vector<string> vectLines;
	StringUtils::Tokenize(sStatus, "\n", vectLines);
	for(vector<string>::const_iterator it = vectLines.begin(), end = vectLines.end(); it != end; ++it)
	{
		string sLine = *it;

		vector<string> vectTokens;
		string::size_type pos = 0;
		string str;

		do 
		{
			str = StringUtils::Tokenize(sLine, "!", pos);
			vectTokens.push_back(str);
		}
		while(pos < sLine.length());

		if(vectTokens.size() == CallStatus::NUM_FIELDS)
		{
			string sChannel = vectTokens[CallStatus::csChannel];
			string sExten = vectTokens[CallStatus::csExten];
			string sCallerID = vectTokens[CallStatus::csCallerID];
			string sApplication = vectTokens[CallStatus::csApplication];
			string sBridged = vectTokens[CallStatus::csBridged];
			
			// Zap/pseudo-204881222
			if( "Zap/pseudo-" != sChannel.substr(0, 11) )
			{
				//todo: make it device data
				if(sApplication == "MeetMe" || sApplication == "Conference")
				{
					mapChannels.insert(make_pair(sExten, make_pair(sChannel, sCallerID)));
					mapCalls[sExten] = true;
				}
				else
				{
					if( string::npos == sBridged.find("None") )
					{
						MAP_CHANNELS::iterator it = mapChannels.find(sBridged);
						if(it != mapChannels.end())
						{
							mapChannels.insert(make_pair(it->first, make_pair(sChannel, sCallerID)));
						}
						else
						{
							mapChannels.insert(make_pair(sChannel, make_pair(sChannel, sCallerID)));
							mapCalls[sChannel] = false;
						}
					}
					else
					{
						mapChannels.insert(make_pair(sChannel, make_pair(sChannel, sCallerID)));
						mapCalls[sChannel] = false;
					}
				}
			}
		}
	}

	for(MAP_CALLS::iterator it = mapCalls.begin(), end = mapCalls.end(); it != end; ++it)
	{
		CallStatus *pCallStatus = new CallStatus();
		
		if( pCallStatus == NULL )
		{
			// critical
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CallsStatusChanged : Not enough memory!");
			return false;
		}
		
		// register the call to the map
		map_call2status[pCallStatus->GetID()] = pCallStatus;

		if( it->second )
		{
			pCallStatus->SetConferenceID( atoi(it->first.c_str()) );
			pCallStatus->SetCallType( CallStatus::Conference );
			
			// register the conference to the map
			map_conference2status[pCallStatus->GetConferenceID()] = pCallStatus;
			// remove the conference id from pending conferences map
			map_newconference.erase(pCallStatus->GetConferenceID());
		}
		else
		{
			pCallStatus->SetCallType( CallStatus::DirectCall );
		}

		string sExtensions;
		pair<MAP_CHANNELS::iterator, MAP_CHANNELS::iterator> itBounds = mapChannels.equal_range( it->first );
		for( MAP_CHANNELS::iterator itc = itBounds.first; itc != itBounds.second; ++itc )
		{
			pCallStatus->AddChannel(itc->second.first, itc->second.second);
			sExtensions += itc->second.first + "(" + itc->second.second + ") ";
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Call %s has extensions: %s", it->first.c_str(), sExtensions.c_str());
	}

	mapChannels.clear();
	mapCalls.clear();
	
	return false;
}

bool
Telecom_Plugin::ExtensionsStatusChanged(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Extensions status received from PBX.");
	string sStatus = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST].c_str();

	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data

	vector<string> vectLines;
	StringUtils::Tokenize(sStatus, "\n", vectLines);

	for(vector<string>::const_iterator it = vectLines.begin(), end = vectLines.end(); it != end; ++it)
	{
		string sLine = *it;

		vector<string> vectTokens;
		StringUtils::Tokenize(sLine, "/:", vectTokens);
		
		if(vectTokens.size() == ExtensionStatus::NUM_FIELDS)
		{
			string sType = vectTokens[ExtensionStatus::esfType];
			string sExten = vectTokens[ExtensionStatus::esfExtension];
			string sState = vectTokens[ExtensionStatus::esfState];
			
			ExtensionStatus *pExtensionStatus = NULL;
			map<string, ExtensionStatus*>::iterator ite = map_ext2status.find(sExten);
			if(ite != map_ext2status.end())
				pExtensionStatus = ite->second;
			else
				pExtensionStatus = new ExtensionStatus();
			
			if( pExtensionStatus == NULL )
			{
				// critical
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "ExtensionsStatusChanged : Not enough memory or NULL pointer!");
				return false;
			}

			pExtensionStatus->SetID(sExten);

			if( !sState.empty() )
			{
				//Ready, OnThePhone, Ringing or Dialing?
				ExtensionStatus::Activity activity = ExtensionStatus::String2Activity(sState);
				if(activity != ExtensionStatus::UnknownActivity)
				{
					pExtensionStatus->SetActivity(activity);
				}
				else
				{
					//if not Unregistered or Registered?
					ExtensionStatus::Availability availability = ExtensionStatus::String2Availability(sState);
					if(availability != ExtensionStatus::UnknownAvailability)
						pExtensionStatus->SetAvailability(availability);
					else
					{
						//unknown state
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received unknown status for extension %s -> %s!", 
							sExten.c_str(), sState.c_str());
					}
				}
			}
			
			
			if( !sType.empty() )
			{
				ExtensionStatus::ExtensionType type = ExtensionStatus::String2Type(sType);
				if(type != ExtensionStatus::OTHER)
				{
					pExtensionStatus->SetExtensionType(type);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received unknown type for extension %s -> %s!",
											   sExten.c_str(), sType.c_str());
				}
			}
		
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Exten status: adding %s with type %s and state %s",
				sExten.c_str(), sType.c_str(), sState.c_str());	
			map_ext2status[sExten] = pExtensionStatus;
		}
	}

	return false;
}

bool
Telecom_Plugin::Ring( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo ) 
{
	//like this: 203 calls 200
	//EVENT_RING
	// Channel1:   SIP/203-a6f2       Channel2: SIP/200-62bd       CallerID1: 203       CallerID2: 200
	
	string sSource_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Source_Channel_CONST];
	string sDestination_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Channel_CONST];
	string sSource_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Source_Caller_ID_CONST];
	string sDestination_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Caller_ID_CONST];

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Ring event received from PBX: src channel %s, dest channel %s",
		sSource_Channel.c_str(), sDestination_Channel.c_str());

	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data
	
	DumpActiveCalls();

	CallStatus * pFoundSrc = FindCallStatusForChannel(sSource_Channel);
	CallStatus * pFoundDest = FindCallStatusForChannel(sDestination_Channel);
	CallStatus *pCallStatus = NULL;

	if(CallStatus::IsConferenceCallID(sDestination_Channel))
	{
		string sOldSrcCallID = "(none)";
		string sConferenceName = sDestination_Channel;
		unsigned int unConferenceID = CallStatus::ExtractConferenceCallID(sConferenceName);

		pCallStatus = FindConferenceByConferenceID(unConferenceID);
		if(NULL == pCallStatus)
		{
			pCallStatus = new CallStatus();
			pCallStatus->SetConferenceID(unConferenceID);
			pCallStatus->SetCallType(CallStatus::Conference);
		}

		if( NULL != pFoundSrc )
		{
			sOldSrcCallID = pFoundSrc->GetID();

			//remove it from old call
			pFoundSrc->RemoveChannel(sSource_Channel);
			if(pFoundSrc->Closed())
			{
				map_call2status.erase(pFoundSrc->GetID());
				delete pFoundSrc;
				pFoundSrc = NULL;
			}
		}

		//add it to conference
		pCallStatus->AddChannel(sSource_Channel, sSource_Caller_ID);
		map_call2status[pCallStatus->GetID()] = pCallStatus;

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Moved channel %s (%s) from call %s to conference call %s", 
			sSource_Channel.c_str(), sSource_Caller_ID.c_str(), sOldSrcCallID.c_str(), sConferenceName.c_str());
	}
	else if( NULL != pFoundDest )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "The dest channel %s is already into a call", sDestination_Channel.c_str());
		return false;
	}
	else
	{
		pCallStatus = new CallStatus();
		if( pCallStatus == NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Telecom_Plugin::Ring : Not enough memory!");
		}
		
		pCallStatus->SetCallType(CallStatus::DirectCall);
		pCallStatus->AddChannel(sSource_Channel, sSource_Caller_ID);
		pCallStatus->AddChannel(sDestination_Channel, sDestination_Caller_ID);
		
		map_call2status[pCallStatus->GetID()] = pCallStatus;

		if( NULL != pFoundSrc )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Telecom_Plugin::Ring removed src channel %s from call %s",
				sSource_Channel.c_str(), pFoundSrc->GetID().c_str());

			//remove it from old call
			pFoundSrc->RemoveChannel(sSource_Channel);
			if(pFoundSrc->Closed())
			{
				map_call2status.erase(pFoundSrc->GetID());
				delete pFoundSrc;
				pFoundSrc = NULL;
			}
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Telecom_Plugin::Ring call : %s",
			pCallStatus->GetDebugInfo().c_str());
	}

	//destination channel -> exten -> (?) simple phone -> orbiter
	int nOrbiterDeviceID = GetOrbiterDeviceID("", sDestination_Channel);

	if(nOrbiterDeviceID)
	{
		// stop the media
		DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media_
			(nOrbiterDeviceID, DEVICETEMPLATE_VirtDev_Media_Plugin_CONST,0,0,0,"",false);
		SendCommand(CMD_MH_Stop_Media_);
		//Sleep(1000);
		
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(nOrbiterDeviceID);
		if(NULL != pOH_Orbiter)
		{
			string sOrbiters = m_pOrbiter_Plugin->PK_Device_Orbiters_In_Room_get(pOH_Orbiter->m_dwPK_Room, false);

			SCREEN_DevIncomingCall_DL screen_DevIncomingCall_DL(
				m_dwPK_Device, sOrbiters, 
				ExtensionForChannel(sSource_Channel),
				sSource_Channel,
				sDestination_Channel,
				sSource_Caller_ID,
				sDestination_Caller_ID
			);
			SendCommand(screen_DevIncomingCall_DL);
		}
	}
	
	DumpActiveCalls();

	return false;
}

bool
Telecom_Plugin::Link( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo ) 
{
	string sSource_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Source_Channel_CONST];
	string sDestination_Channel = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Channel_CONST];
	string sSource_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Source_Caller_ID_CONST];
	string sDestination_Caller_ID = pMessage->m_mapParameters[EVENTPARAMETER_Destination_Caller_ID_CONST];

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Link event received from PBX: src channel %s, dest channel %s",
	sSource_Channel.c_str(), sDestination_Channel.c_str());

	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);

	DumpActiveCalls();

	CallStatus * pFoundSrc = FindCallStatusForChannel(sSource_Channel);
	string sCallID;

	if(NULL != pFoundSrc)
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "The channels are linked");
		sCallID = pFoundSrc->GetID();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "The channels aren't registered yet!");
		return false;
	}

	//source channel -> exten -> simple phone -> orbiter
	int nSrcOrbiterDeviceID = GetOrbiterDeviceID("", sSource_Channel);

	if(nSrcOrbiterDeviceID)
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(nSrcOrbiterDeviceID);
		if(NULL != pOH_Orbiter)
		{
			string sOrbiters = m_pOrbiter_Plugin->PK_Device_Orbiters_In_Room_get(pOH_Orbiter->m_dwPK_Room, false);

			SCREEN_DevCallInProgress_DL screen_DevCallInProgress_DL(
				m_dwPK_Device, sOrbiters, 
				sSource_Caller_ID,
				sCallID,
				sSource_Channel
			);
			SendCommand(screen_DevCallInProgress_DL);
		}
	}

	//dest channel -> exten -> simple phone -> orbiter
	int nDestOrbiterDeviceID = GetOrbiterDeviceID("", sDestination_Channel);

	if(nDestOrbiterDeviceID)
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(nDestOrbiterDeviceID);
		if(NULL != pOH_Orbiter)
		{
			string sDestOrbiters = m_pOrbiter_Plugin->PK_Device_Orbiters_In_Room_get(pOH_Orbiter->m_dwPK_Room, false);
	
			SCREEN_DevCallInProgress_DL screen_DevCallInProgress_DL(
				m_dwPK_Device, sDestOrbiters, 
				sDestination_Caller_ID,
				sCallID,
				sDestination_Channel
			);
			SendCommand(screen_DevCallInProgress_DL);
		}
	}
	
	for(map<string, TelecomTask*>::const_iterator it=map_id2task.begin(); it!=map_id2task.end(); ++it)
	{
		TelecomTask * pTask = (*it).second;
		if( pTask )
		{
			pTask->ProcessEvent(pMessage);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "TelecomTask NULL");
		}
	}
	
	return false;
}

bool
Telecom_Plugin::IncomingCall( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "IncomingCall : deprecated!");
	return false;
}

bool Telecom_Plugin::Hangup( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo ) 
{
	string sChannel_ID = pMessage->m_mapParameters[EVENTPARAMETER_Channel_ID_CONST];
	string sReason = pMessage->m_mapParameters[EVENTPARAMETER_Reason_CONST];

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup the channel %s", sChannel_ID.c_str());
	
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data

	DumpActiveCalls();

	CallStatus *pCallStatus = FindCallStatusForChannel(sChannel_ID);

	if(NULL != pCallStatus)
	{
		pCallStatus->RemoveChannel(sChannel_ID);

		if(pCallStatus->Closed())
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup the call %s", pCallStatus->GetID().c_str());
			
			map_call2status.erase(pCallStatus->GetID());
			delete pCallStatus;
			pCallStatus = NULL;
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Got hangup for channel %s, but it's not into a call!", sChannel_ID.c_str());
	}

	//destination channel -> exten -> (?) simple phone -> orbiter
	int nOrbiterDeviceID = GetOrbiterDeviceID("", sChannel_ID);

	if(nOrbiterDeviceID)
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(nOrbiterDeviceID);
		if(NULL != pOH_Orbiter)
		{
			string sOrbiters = m_pOrbiter_Plugin->PK_Device_Orbiters_In_Room_get(pOH_Orbiter->m_dwPK_Room, false);

			DCE::SCREEN_Call_Dropped_DL screen_SCREEN_Call_Dropped_DL(m_dwPK_Device, sOrbiters, sReason);
			SendCommand(screen_SCREEN_Call_Dropped_DL); 
		}
	}

	DumpActiveCalls();
	
	// notify the telecom tasks
	for(map<string, TelecomTask*>::const_iterator it=map_id2task.begin(); it!=map_id2task.end(); ++it)
	{
		TelecomTask * pTask = (*it).second;
		if( pTask != NULL )
		{
			pTask->ProcessEvent(pMessage);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "TelecomTask NULL");
		}
	}

	return false;
}

CallStatus * Telecom_Plugin::FindCallStatusForChannel(string sChannelID)
{
	for(map<string, CallStatus*>::const_iterator it=map_call2status.begin();
		   it!=map_call2status.end(); ++it)
	{
		if( (*it).second->HasChannel(sChannelID) )
		{
			return (*it).second;
		}
	}
	
	return NULL;
}

CallStatus * Telecom_Plugin::FindCallStatusForID(string sCallID)
{
	map<string, CallStatus*>::const_iterator itFound = map_call2status.find(sCallID);
	if( itFound != map_call2status.end() )
		return (*itFound).second;
	
	return NULL;
}

string Telecom_Plugin::DirectCall2Conference(CallStatus * pCallStatus)
{
	if( !pCallStatus->IsConference() )
	{
		const map<string, string> & channels = pCallStatus->GetChannels();
		if( 2 == channels.size() )
		{
			map<string, string>::const_iterator itFirst = channels.begin();
			map<string, string>::const_iterator itSecond = (++channels.begin());
			string sNewConfID = GetNewConferenceID();
			CMD_PL_Transfer(0, 0, sNewConfID, (*itFirst).first, (*itSecond).first);
			
			return sNewConfID;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write
				(LV_CRITICAL, "Bad DirectCall: %s", pCallStatus->GetDebugInfo().c_str());
		}
	}
	
	return "";
}

ExtensionStatus* Telecom_Plugin::FindExtensionStatus(string sExt)
{
	map<string, ExtensionStatus*>::const_iterator itFound = map_ext2status.find(sExt);
	if( itFound != map_ext2status.end() )
		return (*itFound).second;
	
	return NULL;
}

string Telecom_Plugin::GetNewConferenceID()
{
	size_t i = 1;
	size_t confMax = map_conference2status.size() + map_newconference.size() + 1;
	for(; i<confMax; i++)
	{
		if( map_conference2status.end() == map_conference2status.find(i) &&
			map_newconference.end() == map_newconference.find(i) )
		{
			break;
		}
	}
	
	if( i > CONFERENCE_MAX_NO )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Maximum conferences number : %u !", CONFERENCE_MAX_NO);
		return "";
	}
	
	map_newconference[i] = "";
	return CallStatus::GetStringConferenceID(static_cast<unsigned int>(i));
}

void Telecom_Plugin::RemoveCallStatus(CallStatus * pCallStatus)
{
	if( pCallStatus )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Remove call : %s", pCallStatus->GetDebugInfo().c_str());
		if( map_call2status.end() != map_call2status.find(pCallStatus->GetID()) )
		{
			map_call2status.erase(pCallStatus->GetID());
			if( pCallStatus->IsConference() )
				map_conference2status.erase(pCallStatus->GetConferenceID());
			
			delete pCallStatus;
			// useless
			// pCallStatus = NULL;
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "No such call : %s", pCallStatus->GetDebugInfo().c_str());
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "RemoveCallStatus : Null call!");
	}
}

void Telecom_Plugin::RemoveExtensionStatus(string sExt)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Remove extension status : %s", sExt.c_str());
	
	map<string, ExtensionStatus*>::iterator itFound = map_ext2status.find(sExt);
	if( itFound != map_ext2status.end() )
	{
		delete (*itFound).second;
		(*itFound).second = NULL;
		map_ext2status.erase(itFound);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "RemoveExtensionStatus : no such extension!");
	}
}

void Telecom_Plugin::CleanStatusMaps()
{
	for(map<string, CallStatus*>::const_iterator it=map_call2status.begin();
		it!=map_call2status.end(); ++it)
	{
		delete (*it).second;
	}
	
	for(map<string, ExtensionStatus*>::const_iterator ite=map_ext2status.begin();
		ite!=map_ext2status.end(); ++ite)
	{
		delete (*ite).second;
	}
	
	map_call2status.clear();
	map_conference2status.clear();
	map_ext2status.clear();
}

void
Telecom_Plugin::ProcessRing(std::string sPhoneExtension, std::string sPhoneCallerID, std::string sPhoneCallID)
{
	//*** DISABLED ***
}


DeviceData_Router*
Telecom_Plugin::find_Device(int iPK_Device) {
    /*search device by id*/
	return m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
}

DeviceData_Router*
Telecom_Plugin::find_AsteriskDevice() {
	DeviceData_Router *pDeviceData = NULL;

	ListDeviceData_Router* pListDeviceData =
								m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_Asterisk_CONST);
	if(pListDeviceData) {
		if(pListDeviceData->size() > 1) {
			string errStr = "More then 1 Asterisk Handlers found:";
			list<DeviceData_Router*>::iterator it = pListDeviceData->begin();
			while(it != pListDeviceData->end()) {
				errStr += ' ' + StringUtils::ltos((*it)->m_dwPK_Device);
				it++;
			}
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, errStr.c_str());
			//return -1;
		}

		pDeviceData = pListDeviceData->front();
	} else {
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Asterisk Handler not found...");
	}
	return pDeviceData;
}

int
Telecom_Plugin::generate_NewCommandID() {
	return ++iCmdCounter;
}


//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c232-b->

	/** @brief COMMAND: #232 - PL_Originate */
	/** Originate a call -- * deprecated, use Make call instead * */
		/** @param #2 PK_Device */
			/** Device (phone) from which to place the call */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */
		/** @param #84 PhoneCallerID */
			/** Caller ID */

void Telecom_Plugin::CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage)
//<-dceag-c232-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "DEPRECATED : Originate command called with params: DeviceID=%d, PhoneExtension=%s!", iPK_Device, sPhoneExtension.c_str());
}


//<-dceag-c234-b->

	/** @brief COMMAND: #234 - PL_Transfer */
	/** Transfers a call to other phone */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */
		/** @param #17 PK_Users */
			/** User ID to transfer call to */
		/** @param #83 PhoneExtension */
			/** Local Extension to transfer call to */
		/** @param #265 Channel 1 */
			/** Channel to transfer */
		/** @param #266 Channel 2 */
			/** Second channel to transfer (can be empty) */

void Telecom_Plugin::CMD_PL_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,string sChannel_1,string sChannel_2,string &sCMD_Result,Message *pMessage)
//<-dceag-c234-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Transfer command called with params: DeviceID=%d UserID=%d Extension=%s", iPK_Device,iPK_Users,sPhoneExtension.c_str());
	string sPhoneNumber = GetPhoneNumber(iPK_Users, sPhoneExtension, iPK_Device);
	if( sPhoneNumber.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Nowhere to transfer !!!");
		sCMD_Result = "ERROR : Nowhere to transfer!";
		return;
	}

	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data

	CallStatus * call1 = FindCallStatusForChannel(sChannel_1);
	CallStatus * call2 = FindCallStatusForChannel(sChannel_2);

	if(m_pDevice_pbx)
	{
		if( call1 == NULL )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Telecom_Plugin::CMD_PL_Transfer: No call found for channel %s", sChannel_1.c_str());
			sCMD_Result = "ERROR : Call not available!";
			return;
		}
	
		/*send transfer command to PBX*/
		CMD_PBX_Transfer cmd_PBX_Transfer(
			m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
			sPhoneNumber, sChannel_1, call2 == NULL ? "" : sChannel_2 );
		SendCommand(cmd_PBX_Transfer);
	}
}

//<-dceag-c236-b->

	/** @brief COMMAND: #236 - PL_Cancel */
	/** Hangs up a call */
		/** @param #2 PK_Device */
			/** The device to hangup the call for (ie the phone or orbiter).  If 0, the from device is assumed.  If -1, all calls are terminated

* deprecated, use channel id instead * */
		/** @param #264 Channel */
			/** The channel to cancel */

void Telecom_Plugin::CMD_PL_Cancel(int iPK_Device,string sChannel,string &sCMD_Result,Message *pMessage)
//<-dceag-c236-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Hangup command called.");
	
	if( iPK_Device )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Hangup : DEPRECATED PK_Device param %d.", iPK_Device);
		return;
	}
// 	if( iPK_Device==-1 )
// 		HangupAllCalls();
// 	else if( iPK_Device==0 )
// 		iPK_Device = pMessage->m_dwPK_Device_From;
	
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);
	
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will hangup on channelid %s", sChannel.c_str());
	
	if(m_pDevice_pbx) {
		/*send transfer command to PBX*/
		CMD_PBX_Hangup cmd_PBX_Hangup(m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device, sChannel);
		SendCommand(cmd_PBX_Hangup);
	}
	sCMD_Result="OK :";
}
//<-dceag-createinst-b->!

void Telecom_Plugin::HangupAllCalls()
{
#ifdef DEBUG
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::HangupAllCalls hanging up %u", map_call2status.size());
#endif

	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);
	
	CallStatus * pCallStatus = NULL;
	for(map<string, CallStatus*>::const_iterator it=map_call2status.begin(); it!=map_call2status.end(); ++it)
	{
		pCallStatus = (*it).second;
		if( pCallStatus )
		{
			const map<string, string> & channels = pCallStatus->GetChannels();
			for(map<string, string>::const_iterator itCh=channels.begin(); itCh!=channels.end(); ++itCh)
			{
				CMD_PBX_Hangup cmd_PBX_Hangup(m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device, (*itCh).first);
				SendCommand(cmd_PBX_Hangup);
			}
		}
	}
}

void Telecom_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data

	int nDeviceID = pDeviceData_Router->m_dwPK_Device;
	string sExt = map_device2ext[nDeviceID];

	//by default, if not into a call but the device is registered to asterisk, the color is black
	//if it's not registered, the color will be gray
	ExtensionStatus *pExtensionStatus = FindExtensionStatusByDevice(nDeviceID);
	if(NULL != pExtensionStatus && ExtensionStatus::Registered == pExtensionStatus->GetAvailability())
	{
		Color = PlutoColor::Black().m_Value;
		sDescription = "Unregistered";
	}
	else
	{
		Color = PlutoColor::Gray().m_Value;
		sDescription = "Registered";
	}

	if(NULL != pExtensionStatus)
		sDescription = ExtensionStatus::Activity2String(pExtensionStatus->GetActivity());
	
	//into a call ?
	int nIndex = 0;
	for(map<string, CallStatus *>::const_iterator it = map_call2status.begin(); it != map_call2status.end(); ++it)
	{
		CallStatus *pCallStatus = it->second;

		string sChannel = FindChannelForExt(pCallStatus, sExt);

		if(!sChannel.empty())
		{
			//change its color
			Color = UniqueColors[nIndex % MAX_TELECOM_COLORS];

			//and the description
			if(NULL != pExtensionStatus)
				sDescription = ExtensionStatus::Availability2String(pExtensionStatus->GetAvailability());

			break;
		}
		
		nIndex++;
	}
	
	OSD = "TODO: OSD?";
}

//<-dceag-c414-b->

	/** @brief COMMAND: #414 - PL External Originate */
	/** Originate an external call -- * deprecated, use Make call instead * */
		/** @param #75 PhoneNumber */
			/** Phone to call */
		/** @param #81 CallerID */
			/** Caller ID */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */

void Telecom_Plugin::CMD_PL_External_Originate(string sPhoneNumber,string sCallerID,string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c414-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DEPRECATED - Telecom_Plugin::CMD_PL_External_Originate !");
}

string Telecom_Plugin::GetDialNumber(Row_PhoneNumber *pRow_PhoneNumber)
{
	if( pRow_PhoneNumber->DialAs_get().size() )
		return pRow_PhoneNumber->DialAs_get();

	// TODO - make this better
	string sDial;
	if( pRow_PhoneNumber->CountryCode_get().size() )
		sDial += "00" + pRow_PhoneNumber->CountryCode_get();

	if( pRow_PhoneNumber->AreaCode_get().size() )
		sDial += pRow_PhoneNumber->AreaCode_get();

	sDial += pRow_PhoneNumber->PhoneNumber_get();

	return sDial;
}

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a DTMF code */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #41 StreamID */
			/** ID of stream to apply */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void Telecom_Plugin::CMD_Simulate_Keypress(string sPK_Button,int iStreamID,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	int phoneID=map_orbiter2embedphone[pMessage->m_dwPK_Device_From];
	if(phoneID>0)
	{
		DCE::CMD_Simulate_Keypress cmd(m_dwPK_Device,phoneID,sPK_Button,0,sName);
		SendCommand(cmd);
	}
	sCMD_Result="OK";
}
//<-dceag-c334-b->

	/** @brief COMMAND: #334 - Phone_Initiate */
	/** Initiates a call */
		/** @param #2 PK_Device */
			/** The device to iniate the call from.  Used by Telecom plugin to forward to a sip phone.  Not needed when sending to a phone directly.  If this is not specified, telecom plugin assumes the correct option is the from device */
		/** @param #83 PhoneExtension */
			/** Extention to dial */

void Telecom_Plugin::CMD_Phone_Initiate(int iPK_Device,string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c334-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "DEPRECATED - Telecom_Plugin::CMD_Phone_Initiate !");
	
	sCMD_Result="OK";
}

//<-dceag-c335-b->

	/** @brief COMMAND: #335 - Phone_Answer */
	/** Answer a call */

void Telecom_Plugin::CMD_Phone_Answer(string &sCMD_Result,Message *pMessage)
//<-dceag-c335-e->
{
	int nEmbeddedPhoneID = 0;
	if(GetEmbeddedPhoneAssociated(pMessage->m_dwPK_Device_From, nEmbeddedPhoneID))
	{
		DCE::CMD_Phone_Answer cmd(m_dwPK_Device, nEmbeddedPhoneID);
		SendCommand(cmd);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Phone_Answer: No embedded phone associated with device %d found", pMessage->m_dwPK_Device_From);
		sCMD_Result="ERROR : No such simplephone!";
	}

	sCMD_Result="OK";
}

//<-dceag-c336-b->

	/** @brief COMMAND: #336 - Phone_Drop */
	/** Drop a call */

void Telecom_Plugin::CMD_Phone_Drop(string &sCMD_Result,Message *pMessage)
//<-dceag-c336-e->
{
	int nEmbeddedPhoneID = 0;
	if(GetEmbeddedPhoneAssociated(pMessage->m_dwPK_Device_From, nEmbeddedPhoneID))
	{
		DCE::CMD_Phone_Drop cmd_Phone_Drop(m_dwPK_Device, nEmbeddedPhoneID);
		SendCommand(cmd_Phone_Drop);
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Phone_Answer: No embedded phone associated with device %d found", pMessage->m_dwPK_Device_From);
		sCMD_Result="ERROR : No such simplephone!";
	}
	
	sCMD_Result="OK";
}
//<-dceag-c744-b->

	/** @brief COMMAND: #744 - Set User Mode */
	/** Set a user to a given mode (sleeping, etc.) */
		/** @param #17 PK_Users */
			/** The user */
		/** @param #194 PK_UserMode */
			/** The user mode */

void Telecom_Plugin::CMD_Set_User_Mode(int iPK_Users,int iPK_UserMode,string &sCMD_Result,Message *pMessage)
//<-dceag-c744-e->
{
	if( !iPK_Users )
	{
		OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(pMessage->m_dwPK_Device_From);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::CMD_Set_User_Mode no user specified.  Doing current user on orbiter %p %d = %d",pOH_Orbiter,pOH_Orbiter ? pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device : -1, pOH_Orbiter && pOH_Orbiter->m_pOH_User ? pOH_Orbiter->m_pOH_User->m_iPK_Users : -1);
		if( pOH_Orbiter && pOH_Orbiter->m_pOH_User && pOH_Orbiter->m_pOH_User->m_iPK_Users )
			iPK_Users = pOH_Orbiter->m_pOH_User->m_iPK_Users;
		else
			return;
	}
	class Row_Users* pRow_Users;
	pRow_Users=m_pDatabase_pluto_main->Users_get()->GetRow(iPK_Users);
	if( pRow_Users )
	{
		pRow_Users->Reload();
		pRow_Users->FK_UserMode_set(iPK_UserMode);
		m_pDatabase_pluto_main->Users_get()->Commit();
	}
	else
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Trying to set user mode for invalid user %d",iPK_Users);

	for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = (*it).second;

		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,
			StringUtils::itos(iPK_UserMode),"","user" + StringUtils::itos(iPK_Users));
		SendCommand(CMD_Set_Bound_Icon);
	}
}
//<-dceag-c751-b->

	/** @brief COMMAND: #751 - PL_Add_VOIP_Account */
	/** Add a VOIP account */
		/** @param #50 Name */
			/** Provider name */
		/** @param #75 PhoneNumber */
			/** Phone number */
		/** @param #99 Password */
			/** Password */
		/** @param #189 Users */
			/** User name */

void Telecom_Plugin::CMD_PL_Add_VOIP_Account(string sName,string sPhoneNumber,string sPassword,string sUsers,string &sCMD_Result,Message *pMessage)
//<-dceag-c751-e->
{
	
	// ONLY if the automatically VoIP management is ON
	// If not then send a popup message to the Orbiter, so that the user will see the problem
	// Eugen
	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByTemplate_Find( DEVICETEMPLATE_Asterisk_CONST );
	if( !pListDeviceData_Router || !pListDeviceData_Router->size() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Telecom_Plugin::CMD_PL_Add_VOIP_Account : no Asterisk device !");
		return;
	}
	DeviceData_Router *pDevice = *pListDeviceData_Router->begin();
	string manualVoIP = pDevice->m_mapParameters_Find( DEVICEDATA_Manual_configuration_CONST );
	
	if( !manualVoIP.empty() && 1 == atoi(manualVoIP.c_str()) )
	{
		// send popup message to pMessage->m_dwPK_Device_From
		DeviceData_Router *pDeviceOrbiter = m_pRouter->m_mapDeviceData_Router_Find( pMessage->m_dwPK_Device_From );
		if( pDeviceOrbiter && pDeviceOrbiter->WithinCategory(DEVICECATEGORY_Orbiter_CONST) )
		{
			DCE::CMD_Goto_Screen
					cmd(m_dwPK_Device, pMessage->m_dwPK_Device_From, "Telecom Notify", SCREEN_PopupMessage_CONST, interuptAlways, true, false);
			cmd.m_pMessage->m_mapParameters[COMMANDPARAMETER_Text_CONST] =
					"Please use the Phones Setup to change VoIP configuration because the manual VoIP configuration is activated.\nOr please first activate the automatic VoIP configuration from Phone Lines.";
			cmd.m_pMessage->m_mapParameters[COMMANDPARAMETER_Command_Line_CONST] = "$Action_Yes";
			SendCommand(cmd);
		}
		
		return;
	}
	
	string cmdline = "";

	if(sName == "broadvoice")
	{
		cmdline += "/usr/pluto/bin/create_amp_broadvoice.pl";
	}
	if(sName == "freeworlddialup")
	{
		cmdline += "/usr/pluto/bin/create_amp_fwd.pl";
	}
	if(sName == "inphonex")
	{
		cmdline += "/usr/pluto/bin/create_amp_inphonex.pl";
	}
	if(sName == "efon")
	{
		cmdline += "/usr/pluto/bin/create_amp_efon.pl";
	}
	if(sName == "teliax")
	{
		cmdline += "/usr/pluto/bin/create_amp_teliax.pl";
	}
	if(sName == "nufone")
	{
		cmdline += "/usr/pluto/bin/create_amp_nufone.pl";
	}
	if(sName == "voiceeclipse")
	{
		cmdline += "/usr/pluto/bin/create_amp_voiceeclipse.pl";
	}

	cmdline+= string(" ")+sUsers+(" ")+sPassword+string(" ")+sPhoneNumber;
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Will call %s",cmdline.c_str());
	system(cmdline.c_str());
}

bool Telecom_Plugin::OrbiterRegistered(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	bool bRegistered = pMessage->m_mapParameters[COMMANDPARAMETER_OnOff_CONST]=="1";
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::OrbiterRegistered orbiter %d registered %d",pMessage->m_dwPK_Device_From,(int) bRegistered);
	
	if( bRegistered )
	{
		vector<Row_Users *> vectRow_Users;
		m_pDatabase_pluto_main->Users_get()->GetRows(USERS_FK_INSTALLATION_MAIN_FIELD + string("=") + StringUtils::itos(m_pRouter->iPK_Installation_get())+string(" OR ")+string(USERS_FK_INSTALLATION_MAIN_FIELD)+string(" IS NULL"),&vectRow_Users);

		PLUTO_SAFETY_LOCK_ERRORSONLY(vm, m_TelecomMutex);
		for(size_t s=0;s<vectRow_Users.size();++s)
		{
			Row_Users *pRow_Users = vectRow_Users[s];
			DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pMessage->m_dwPK_Device_From,
				StringUtils::itos(pRow_Users->FK_UserMode_get()),"","user" + StringUtils::itos(pRow_Users->PK_Users_get()));
			SendCommand(CMD_Set_Bound_Icon);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::OrbiterRegistered Set_Bound_Icon(usermode = %d, user = %d)",pRow_Users->FK_UserMode_get(),pRow_Users->PK_Users_get());

			std::map<int, std::pair<string, string> >::iterator it = m_mapVoiceMailStatus.find(pRow_Users->PK_Users_get());
			if(it != m_mapVoiceMailStatus.end())
			{
				DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon_Voicemail(m_dwPK_Device,pMessage->m_dwPK_Device_From,
					it->second.first, it->second.second, "vm" + StringUtils::ltos(pRow_Users->PK_Users_get()));
				SendCommand(CMD_Set_Bound_Icon_Voicemail);
			}
		}
		vm.Release();
	}
	return false;
}

class DataGridTable *Telecom_Plugin::ActiveCallsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ActiveCalls request received for GridID: %s",GridID.c_str());
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell = NULL;
	int Row = 0;

	for(map<string, CallStatus *>::const_iterator it = map_call2status.begin(); it != map_call2status.end(); ++it)
	{
		CallStatus *pCallStatus = it->second;

		string sText = GetCallName(pCallStatus) + " (" + StringUtils::ltos(static_cast<long>(pCallStatus->GetChannels().size())) + ")";
		string sValue = it->first;

		pCell = new DataGridCell(sText, sValue);
		pCell->m_AltColor = UniqueColors[Row % MAX_TELECOM_COLORS];
		pDataGrid->SetData(0, Row, pCell);

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Row %d: cell text %s, value %s",
			Row, sText.c_str(), sValue.c_str());

		Row++;
	}

	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::ActiveUsersOnCallGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ActiveUsersOnCallGrid request received for GridID: %s, phone call id %s",GridID.c_str(),
		Parms.c_str());
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell = NULL;
	int Row = 0;
	int nIndex = 0;

	for(map<string, CallStatus *>::const_iterator it = map_call2status.begin(); it != map_call2status.end(); ++it)
	{
		CallStatus *pCallStatus = it->second;

		if(it->first == Parms)
		{
			const map<string, string>& channels = pCallStatus->GetChannels();
			for(map<string, string>::const_iterator itc = channels.begin(); itc != channels.end(); ++itc)
			{
				string sText = itc->second;
				string sValue = itc->first;

				if(sText.empty())
					sText = ExtensionForChannel(sValue);

				char *pData = NULL;
				int nSize = 0;
				CMD_Get_Associated_Picture_For_Channel(sValue, &pData, &nSize);

				if(NULL != pData && 0 != nSize)
				{
					pCell = new DataGridCell("", sValue);
					pCell->m_AltColor = UniqueColors[nIndex % MAX_TELECOM_COLORS];
					pCell->m_pGraphicData = pData;
					pCell->m_GraphicLength = nSize;
					pDataGrid->SetData(0, Row, pCell);
				}

				pCell = new DataGridCell(sText, sValue);
				pCell->m_Colspan = 5;
				pCell->m_AltColor = UniqueColors[nIndex % MAX_TELECOM_COLORS];
				pDataGrid->SetData(1, Row, pCell);

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Row %d: cell text %s, value %s",
					Row, sText.c_str(), sValue.c_str());

				Row++;    
			}

			break;
		}

		++nIndex;
	}

	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::ExternalChannels(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);  // Protect the call data
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "ExternalChannels request received!");

	DumpActiveCalls();

	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell = NULL;
	int Row = 0;

	int nIndex = 0;
	for(map<string, CallStatus *>::const_iterator it = map_call2status.begin(); it != map_call2status.end(); ++it)
	{
		CallStatus *pCallStatus = it->second;

		const map<string, string>& channels = pCallStatus->GetChannels();
		for(map<string, string>::const_iterator itc = channels.begin(); itc != channels.end(); ++itc)
		{
			string sChannel = itc->first;
			string sExten = ExtensionForChannel(sChannel);

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Extension %s for channel %s", sExten.c_str(), sChannel.c_str());

			if(0 == map_ext2device[sExten])
			{
				string sText = itc->second;
				string sValue = sExten;

				if(sText.empty())
					sText = sExten;

				pCell = new DataGridCell(sText, sValue);
				pCell->m_AltColor = UniqueColors[nIndex % MAX_TELECOM_COLORS];
				pDataGrid->SetData(0, Row, pCell);

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Row %d: cell text %s, value %s",
					Row, sText.c_str(), sValue.c_str());

				Row++;    
			}
		}

		++nIndex;
	}

	return pDataGrid;
}

static char* sec2str(int sec)
{
	static char buff[32];
	int s=sec%60;
	int m=(sec/60)%60;
	int h=(sec/3600);
	if(h>0)
		sprintf(buff,"%d:%02d:%02d",h,m,s);
	else
		sprintf(buff,"%02d:%02d",m,s);
	return buff;
}

class DataGridTable *Telecom_Plugin::RecentCallsGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "RecentCalls request received for GridID: %s",GridID.c_str());
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	int Row = 0;

	DCEConfig dceconf;
	DBHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, "asteriskcdrdb" ,dceconf.m_iDBPort);
	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( (result_set.r=mySqlHelper.db_wrapper_query_result("SELECT src, dst, calldate, billsec, channel FROM cdr ORDER BY calldate DESC LIMIT 0,20")) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SQL FAILED");
		return pDataGrid;
	}
	while((row = db_wrapper_fetch_row(result_set.r)))
	{
		int ext;
		string sext=row[0];
		string sext2=row[1];
		if((sext2.find_first_not_of("0123456789")==sext2.npos) && (sext2.find("000")!=0))
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"   sext1='%s'",sext.c_str());
			if(sext == "")
			{
				ParseChannel(row[4],&ext,&sext);
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"   sext2='%s'",sext.c_str());
				if(sext == "")
				{
					sext="Unknown";
				}
			}
			if(sext != sext2)
			{
				string text = sext+string(" to ")+sext2+string(" at ")+string(row[2])+string("(")+string(sec2str(atoi(row[3])))+string(")");
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"WILL SHOW  %s",text.c_str());
				pCell = new DataGridCell(text,sext2);
				pCell->m_pMessage=NULL;
				pDataGrid->SetData(0,Row,pCell);
				Row++;
			}
		}
	}

	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::SpeedDialGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "SpeedDial request received for GridID: %s with Params: %s",GridID.c_str(),Parms.c_str());
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
	int Row = 0;
	string::size_type pos=0;
	string something7 = StringUtils::Tokenize(Parms,",",pos);
	string room = StringUtils::Tokenize(Parms,",",pos);
	if( room.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SpeedDial request has no room");
		return pDataGrid;
	}

    ///SELECT CommandGroup.Description, FK_CommandParameter, IK_CommandParameter FROM CommandGroup JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup WHERE FK_Array=4 AND AutoGeneratedDate=0 AND FK_Room=XXX ORDER BY PK_CommandGroup,FK_CommandParameter;
	string sql_buff = "SELECT CommandGroup.Description,FK_CommandParameter,IK_CommandParameter";
	sql_buff += " FROM CommandGroup";
	sql_buff += " JOIN CommandGroup_Command ON CommandGroup_Command.FK_CommandGroup=PK_CommandGroup";
	sql_buff += " JOIN CommandGroup_Command_CommandParameter ON FK_CommandGroup_Command=PK_CommandGroup_Command";
	sql_buff += " JOIN CommandGroup_Room ON CommandGroup_Room.FK_CommandGroup=PK_CommandGroup";
	sql_buff += " WHERE AutoGeneratedDate=0";
	sql_buff += " AND FK_Array="+StringUtils::itos(ARRAY_Communication_Scenarios_CONST);
	sql_buff += " AND FK_Room="+room;
	sql_buff += " ORDER BY PK_CommandGroup,FK_CommandParameter";

	DCEConfig dceconf;
	DBHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName ,dceconf.m_iDBPort);
	PlutoSqlResult result_set;
	DB_ROW row=NULL;
	if( (result_set.r=mySqlHelper.db_wrapper_query_result(sql_buff)) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "SQL FAILED");
		return pDataGrid;
	}
	char desc[256];
	while((row = db_wrapper_fetch_row(result_set.r)))
	{
		int device=0;
		if(atoi(row[1])==COMMANDPARAMETER_PK_Device_CONST)
		{
			device = (NULL != row[2] ? atoi(row[2]) : 0);
			strncpy(desc,row[0],sizeof(desc)-1);
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend (1st row not PK_Device)");
			break;
		}
		if((row = db_wrapper_fetch_row(result_set.r))==NULL)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend (no 2nd row for '%s')",desc);
			break;
		}
		if(atoi(row[1])!=COMMANDPARAMETER_PhoneExtension_CONST)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend (2nd row not PhoneExtension)");
			break;
		}
		if(strcmp(row[0],desc))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend ('%s' != '%s')",desc,row[0]);
			break;
		}
		string phoneExtension = row[2];
		string text = string(row[0])+string(" (")+string(row[2])+string(")");
		
		// TODO: what to do with caller id ??
		// callerid
		if((row = db_wrapper_fetch_row(result_set.r))==NULL)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend (no 3nd row for '%s')",desc);
			break;
		}
		if(atoi(row[1])!=COMMANDPARAMETER_PhoneCallerID_CONST)
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend (3nd row not PhoneCallerId)");
			break;
		}
		if(strcmp(row[0],desc))
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING,"This should not happend ('%s' != '%s')",desc,row[0]);
			break;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS,"WILL SHOW  %s / %d",text.c_str(),device);
		pCell = new DataGridCell(text,"");
		// TODO with Chris, it should me CMD_Make_Call
/*		DCE::CMD_PL_Originate CMD_PL_Originate_(m_dwPK_Device,m_dwPK_Device,device, phoneExtension,"");
		pCell->m_pMessage=CMD_PL_Originate_.m_pMessage;*/
		pDataGrid->SetData(0,Row,pCell);
		Row++;
	}
	return pDataGrid;
}

int Telecom_Plugin::ParseChannel(const std::string channel, int* iextension, string *sextension)
{
	size_t pos, oldpos = 0;

	pos = channel.find('/');
	if(pos < 0) {
		return -1;
	}

	oldpos = pos + 1;
	pos = channel.find('@',oldpos);
	if(pos < 0) {
		pos = channel.find('-',oldpos);
	}
	if(pos < 0) {
		pos = channel.find('/',oldpos);
	}
	if(pos < 0) {
		pos = channel.find('&',oldpos);
	}
	if(pos < 0) {
		pos = channel.find(',',oldpos);
	}
	if(pos < 0) {
		pos = channel.find('|',oldpos);
	}
	if(pos < 0) {
		return -1;
	}

	if(iextension && sextension) {
		*sextension = channel.substr(oldpos, pos - oldpos);
		if(sscanf(sextension->c_str(),"%d",iextension)!=1)
		{
			return -1;
		}
	}
	return 0;
}

bool Telecom_Plugin::VoIP_Problem(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	int iLevel = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST].c_str());
	string sText = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Received VoIP problem : level %d, description :\n%s",iLevel,sText.c_str());
	time_t now=time(NULL);
	pthread_mutex_lock(&mtx_err_messages);
	if(map_err_messages.find(sText) != map_err_messages.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Already have this message");	
	}
	else
	{
		map_err_messages[sText]=(long)now;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will add to queue");	
	}
	pthread_mutex_unlock(&mtx_err_messages);
	if((iLevel == 7) && (sText.length()>0))
	{
		SCREEN_PopupMessage_DL SCREEN_PopupMessage_DL(m_dwPK_Device, m_pOrbiter_Plugin->m_sPK_Device_AllOrbiters_AllowingPopups_get(),
			sText, // Main message
			"", // Command Line
			"telecom_error", // Description
			"0", // sPromptToResetRouter
			"0", // sTimeout
			"1"); // sCannotGoBack
		SendCommand(SCREEN_PopupMessage_DL);
	}
	return false;
}

void * startDisplayThread(void * Arg)
{
    Telecom_Plugin *telecom_pugin = (Telecom_Plugin *) Arg;
    LoggerWrapper::GetInstance()->Write(LV_STATUS, "Started Display Thread");
    telecom_pugin->doDisplayMessages();
    return NULL;
}
void Telecom_Plugin::doDisplayMessages()
{
	while(!m_bQuit_get())
    {
		if(1)//the orbiter is at main menu ?? how to find out??
		{
			pthread_mutex_lock(&mtx_err_messages);
			string message;
			map<string,long>::iterator it = map_err_messages.begin();
			while(it != map_err_messages.end())
			{
				message += (*it).first + string("\n");
				it++;

				if(m_bQuit_get())
					return;
			}
			if(message.length()>0)
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Will write mesaage : %s",message.c_str());
				map_err_messages.clear();
				//Don't send it yet				
				//SCREEN_DialogGenericNoButtons_Cat SCREEN_DialogGenericNoButtons_(m_dwPK_Device, 5, false, BL_SameHouse,message,"0","0","0");
				//SendCommand(SCREEN_DialogGenericNoButtons_);
			}
			pthread_mutex_unlock(&mtx_err_messages);
		}
		Sleep(1000);
	}
}

//<-dceag-c797-b->

	/** @brief COMMAND: #797 - PL_Join_Call */
	/** Will join you to an existing call */
		/** @param #17 PK_Users */
			/** The user to add to call */
		/** @param #83 PhoneExtension */
			/** The extension to add to call */
		/** @param #87 PhoneCallID */
			/** Phone call ID to join to */
		/** @param #263 PK_Device_To */
			/** The device the add to call */

void Telecom_Plugin::CMD_PL_Join_Call(int iPK_Users,string sPhoneExtension,string sPhoneCallID,int iPK_Device_To,string &sCMD_Result,Message *pMessage)
//<-dceag-c797-e->
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);

	string sPhoneNumber = GetPhoneNumber(iPK_Users, sPhoneExtension, iPK_Device_To);
	LoggerWrapper::GetInstance()->Write( LV_STATUS, "Command PL_Join_Call (call=%s, number=%s)",
		sPhoneCallID.c_str(),
		sPhoneNumber.c_str() );
	
	if( sPhoneNumber.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Nowhere to make a call !!!");
		sCMD_Result = "ERROR : Nowhere to make a call!";
		return;
	}
	
	if( sPhoneCallID.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Invalid call ID !");
		sCMD_Result = "ERROR : Invalid call ID!";
		return;
	}
	
	map<string, CallStatus *>::const_iterator it = map_call2status.find(sPhoneCallID);
	if(it == map_call2status.end())
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Invalid call ID !");
		sCMD_Result = "ERROR : Invalid call ID!";
		return;
	}
	
	CallStatus * pCallStatus = (*it).second;
	if( pCallStatus == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "NULL call status !");
		sCMD_Result = "ERROR : No such call available!";
		return;
	}
	
	string sConferenceID;

	if( !pCallStatus->IsConference() )
	{
		string sChannel_1;
		string sChannel_2;
		
		const map<string, string> & channels = pCallStatus->GetChannels();
		for(map<string, string>::const_iterator itCh=channels.begin(); itCh!=channels.end(); ++itCh)
		{
			if( itCh == channels.begin() )
			{
				sChannel_1 = (*itCh).first;
			}
			else
			{
				sChannel_2 = (*itCh).first;
				break;
			}
		}
		
		string sNewConferenceID = GetNewConferenceID();
		CMD_PBX_Transfer cmd_PBX_Transfer(
			m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
			sNewConferenceID, sChannel_1, sChannel_2 );
		SendCommand(cmd_PBX_Transfer);

		sConferenceID = sNewConferenceID;
	}
	else
	{
		sConferenceID = CallStatus::GetStringConferenceID( pCallStatus->GetConferenceID() );
	}

	if( !InternalMakeCall(0, sConferenceID, sPhoneNumber) )
	{
		sCMD_Result = "ERROR : couldn't make a call from " + sConferenceID + " to " + sPhoneNumber;
	}
}

bool Telecom_Plugin::VoiceMailChanged(class Socket *pSocket,class Message *pMessage,class DeviceData_Base *pDeviceFrom,class DeviceData_Base *pDeviceTo)
{
	int userid = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Users_CONST].c_str());

	// TODO: implement: for userid 0, send to all users on the system with Users.AccessGeneralVoiceMail=1
	if (userid == 0)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Telecom_Plugin::VoiceMailChanged: Event for all users not implemented");
		return false;
	}

	string vmcount = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];
	string::size_type pos=0;
	int vm_new = atoi(StringUtils::Tokenize(vmcount," ",pos).c_str());
	int vm_old = atoi(StringUtils::Tokenize(vmcount," ",pos).c_str());
	string value_param = "0";
	string text_param = "0";
	if(vm_old != 0)
	{
		value_param = "1";
		text_param = StringUtils::itos(vm_old);
	}
	if(vm_new != 0)
	{	
		value_param = "2";
		text_param = StringUtils::itos(vm_new);		
	}

	PLUTO_SAFETY_LOCK_ERRORSONLY(vm, m_TelecomMutex);
	m_mapVoiceMailStatus[userid] = std::make_pair<string, string> (value_param, text_param);
	vm.Release();

	for(map<int,OH_Orbiter *>::iterator it=m_pOrbiter_Plugin->m_mapOH_Orbiter.begin();it!=m_pOrbiter_Plugin->m_mapOH_Orbiter.end();++it)
	{
		OH_Orbiter *pOH_Orbiter = (*it).second;
		DCE::CMD_Set_Bound_Icon CMD_Set_Bound_Icon(m_dwPK_Device,pOH_Orbiter->m_pDeviceData_Router->m_dwPK_Device,value_param,text_param,"vm"+StringUtils::itos(userid));
		SendCommand(CMD_Set_Bound_Icon);
	}

	return false;	
}

/* this makes the plugin asterisk dependend */
#define VOICEMAIL_LOCATION "/var/spool/asterisk/voicemail/default/"
class DataGridTable *Telecom_Plugin::UserVoiceMailGrid(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "UserVoiceMailGrid request received for GridID: %s with Params: %s",GridID.c_str(),Parms.c_str());
	DataGridTable *pDataGrid = new DataGridTable();
#ifndef WIN32
	DataGridCell *pCell;
	int Row = 0;
	string userid = Parms;

	if( userid.size()==0 )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "UserVoiceMailGrid request has no user");
		return pDataGrid;
	}

	Row_Users *pRow_Users = m_pDatabase_pluto_main->Users_get()->GetRow(atoi(userid.c_str()));
	if(NULL == pRow_Users)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Got now record for user %s", userid.c_str());
		return pDataGrid;
	}

	string sExtension = StringUtils::ltos(pRow_Users->Extension_get());
	
	string user_path=string(VOICEMAIL_LOCATION)+sExtension+string("/INBOX/");
	DIR * dir=opendir(user_path.c_str());
	struct dirent *dir_ent = NULL;
	
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Getting new voicemails from %s", user_path.c_str());
	while(NULL != dir && (dir_ent = readdir(dir)))
	{
	    struct stat statbuf;
		string buffer = user_path+dir_ent->d_name;
        stat(buffer.c_str(),&statbuf);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Voicemail ? %s", buffer.c_str());

        if((S_ISREG(statbuf.st_mode)) && (dir_ent->d_name[0] != '.') && (strstr(dir_ent->d_name,".txt") != NULL))
        {
			string text = "New message " + StringUtils::itos(Row+1);
			string file_path=user_path+(dir_ent->d_name);
			file_path.replace(file_path.length()-4,4,".wav");
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"WILL SHOW %s / %s",text.c_str(),file_path.c_str());

			string url="http://dcerouter/pluto-admin/amp/recordings/misc/audio.php?recording="+file_path;
			DCE::CMD_MH_Play_Media CMD_MH_Play_Media_(m_dwPK_Device, DEVICETEMPLATE_VirtDev_Media_Plugin_CONST, pMessage->m_dwPK_Device_From, url, MEDIATYPE_pluto_StoredAudio_CONST,0,"",1,0);
			
			pCell = new DataGridCell(text,"");			
			pCell->m_pMessage=CMD_MH_Play_Media_.m_pMessage;
			pDataGrid->SetData(0,Row,pCell);
			Row++;
		}
	}
	user_path=string(VOICEMAIL_LOCATION)+sExtension+string("/INBOX/Old");
	dir=opendir(user_path.c_str());
	dir_ent = NULL;
	while(NULL != dir && (dir_ent = readdir(dir)))
	{
	    struct stat statbuf;
		string buffer = user_path+dir_ent->d_name;
        stat(buffer.c_str(),&statbuf);
        if((S_ISREG(statbuf.st_mode)) && (dir_ent->d_name[0] != '.') && (strstr(dir_ent->d_name,".txt") != NULL))
        {
			string text = "Old message " + StringUtils::itos(Row+1);
			string file_path=user_path+(dir_ent->d_name);
			file_path.replace(file_path.length()-4,4,".wav");
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"WILL SHOW %s / %s",text.c_str(),file_path.c_str());
			string url="http://dcerouter/pluto-admin/amp/recordings/misc/audio.php?recording="+file_path;
			DCE::CMD_MH_Play_Media CMD_MH_Play_Media_(m_dwPK_Device, DEVICETEMPLATE_VirtDev_Media_Plugin_CONST, pMessage->m_dwPK_Device_From, url, MEDIATYPE_pluto_StoredAudio_CONST,0,"",1,0);
			
			pCell = new DataGridCell(text,"");			
			pCell->m_pMessage=CMD_MH_Play_Media_.m_pMessage;
			pDataGrid->SetData(0,Row,pCell);
			Row++;
		}
	}	
#endif
	return pDataGrid;
}
//<-dceag-c826-b->

	/** @brief COMMAND: #826 - Speak in house */
	/** Lets the user speak in the house or to someone at the intercom, whether the user is in the house or outside */
		/** @param #2 PK_Device */
			/** If not 0, this is presumed to be a device associated with a phone number, such a mobile orbiter or a phone extension, and which will be patched in. */
		/** @param #75 PhoneNumber */
			/** If specified this is the number that will be patched in and PK_Device will be ignored. */
		/** @param #103 List PK Device */
			/** The list of speakerphone type devices to broadcast the voice through.  If this and  PK_Device_Related are not specified all devices are assumed.  If specified this overrides PK_Device_Related */
		/** @param #201 PK_Device_Related */
			/** Broadcast through a speakerphone related to this device.  If List PK Device  is blank then this device can be a camera, doorbell, etc., and the List PK Device will be filled in automatically with the most related device, either explicitly related, or in t */

void Telecom_Plugin::CMD_Speak_in_house(int iPK_Device,string sPhoneNumber,string sList_PK_Device,int iPK_Device_Related,string &sCMD_Result,Message *pMessage)
//<-dceag-c826-e->
{
	// If there's no phone number, fine one associated with the device

	int dwDevice_Caller = 0;
	bool bEmbeddedPhone = false;
	if( sPhoneNumber.empty() )
	{
		DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
		dwDevice_Caller = pDevice->m_dwPK_Device;
		if( pDevice )
		{
			sPhoneNumber = pDevice->m_mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
			bEmbeddedPhone = pDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST;

			if( sPhoneNumber.empty() )
				sPhoneNumber = pDevice->m_mapParameters_Find(DEVICEDATA_Mobile_Orbiter_Phone_CONST);
		}
		if( sPhoneNumber.empty() )
		{
			pDevice = m_pRouter->m_mapDeviceData_Router_Find(pMessage->m_dwPK_Device_From);
			if( pDevice )
			{
				dwDevice_Caller = pDevice->m_dwPK_Device;
				bEmbeddedPhone = pDevice->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST;
				sPhoneNumber = pDevice->m_mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
				if( sPhoneNumber.empty() )
					sPhoneNumber = pDevice->m_mapParameters_Find(DEVICEDATA_Mobile_Orbiter_Phone_CONST);
			}
		}
        
		if( sPhoneNumber.empty() ) //maybe we are a OnScreen Orbiter; let's find the simple phone child
		{
			if(NULL != pDevice)
			{
				vector<DeviceData_Router *> vectDeviceData_Router;
                pDevice->FindChildrenWithinCategory(DEVICECATEGORY_Soft_Phones_CONST, vectDeviceData_Router);

				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::CMD_Speak_in_house : found %d simple phones", vectDeviceData_Router.size());
				if(vectDeviceData_Router.size() >= 1)
				{
					DeviceData_Router *pDeviceData_Router = *vectDeviceData_Router.begin();
					dwDevice_Caller = pDeviceData_Router->m_dwPK_Device;
					bEmbeddedPhone = pDeviceData_Router->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST;

					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::CMD_Speak_in_house : found device %d", pDeviceData_Router->m_dwPK_Device);
					sPhoneNumber = pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
					LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::CMD_Speak_in_house : found phone number of embedded phone %s", sPhoneNumber.c_str());
				}
			}
		}

		if( sPhoneNumber.empty() )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Telecom_Plugin::CMD_Speak_in_house -- can't find a phone number");
			return;
		}
	}

	// is this an embedded phone?
	if(!bEmbeddedPhone)
	{
		// 555 = bogus call id, 998 = all speaker phones in house conf room
		if( !InternalMakeCall(0, SPEAKINTHEHOUSE_INVALID_EXT, SPEAKINTHEHOUSE_CONFERENCE_ALL) )
		{
			sCMD_Result = string("ERROR : couldn't make a call from") + SPEAKINTHEHOUSE_INVALID_EXT + "to" + SPEAKINTHEHOUSE_CONFERENCE_ALL;
		}
		return;
	}

	// Find the playback devices if sList_PK_Device isn't specified
	if(sList_PK_Device.empty() )
	{
		if(iPK_Device_Related)  // Use related devices.  This device may be a camera
		{
			DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device_Related);
			if( pDevice )
			{
				list<int> listDevices;
				list<string> listSlavesExtensions;
				for(map<int,DeviceRelation *>::iterator it=pDevice->m_mapDeviceRelation.begin();it!=pDevice->m_mapDeviceRelation.end();++it)
				{
					string sExtension;
					DeviceRelation *pDeviceRelation = (*it).second;
					sExtension = pDeviceRelation->m_pDevice->m_mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
					if( sExtension.empty()==false ) // got one
					{
						listSlavesExtensions.push_back(sExtension);
						listDevices.push_back(pDeviceRelation->m_pDevice->m_dwPK_Device);
					}
				}
				if( 0 == listDevices.size() && NULL != pDevice->m_pRoom ) // still no extension -- just send to all phones in the same room
				{
					for(list<class DeviceData_Router *>::iterator it=pDevice->m_pRoom->m_listDevices.begin();it!=pDevice->m_pRoom->m_listDevices.end();++it)
					{
						DeviceData_Router *pDeviceData_Router = *it;
						string sExtension = pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
						if( sExtension.empty()==false ) // got one
						{
							listSlavesExtensions.push_back(sExtension);
							listDevices.push_back(pDeviceData_Router->m_dwPK_Device);
						}
					}
				}

				if( listDevices.size() != 0 )  // We have a valid one
				{
					listDevices.push_front(dwDevice_Caller); 
					for(list<int>::iterator it = listDevices.begin(); it != listDevices.end(); ++it)
					{
						// all of us will call 997
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Doing a speak in house with %d", *it);
						DCE::CMD_Phone_Initiate cmd(m_dwPK_Device, *it, 0, SPEAKINTHEHOUSE_CONFERENCE_IVR);
						SendCommand(cmd);
					}
					
					return;
				}
			}

			// If we got here there was a problem
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Doing a speak in house no phone relates to %d",iPK_Device_Related);
		}
	}

	if(sPhoneNumber.empty() == false)
	{
		//get all embedded orbiter devices from the house
		ListDeviceData_Router *pListDeviceData = m_pRouter->m_mapDeviceByTemplate_Find(DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST);
		if(NULL != pListDeviceData)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"Doing a speak in ALL house with master %s", sPhoneNumber.c_str());

			list<int> listDevices;
			list<string> listSlavesExtensions;
			for(ListDeviceData_Router::iterator it = pListDeviceData->begin(); it != pListDeviceData->end(); ++it)
			{
				DeviceData_Router *pDeviceData_Router = *it;
				if(NULL != pDeviceData_Router)
				{
					string sExtension = pDeviceData_Router->m_mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
					if(sExtension.empty() == false && sExtension != sPhoneNumber) //all except the master
					{
						LoggerWrapper::GetInstance()->Write(LV_STATUS,"Doing a speak in ALL house with slave %s", sExtension.c_str());
						listSlavesExtensions.push_back(sExtension);
						listDevices.push_back(pDeviceData_Router->m_dwPK_Device);
					}
				}
			}
			
			listDevices.push_front(dwDevice_Caller); 
			for(list<int>::iterator it = listDevices.begin(); it != listDevices.end(); ++it)
			{
				// all of us will call 997
				LoggerWrapper::GetInstance()->Write(LV_STATUS,"Doing a speak in house with %d", *it);
				DCE::CMD_Phone_Initiate cmd(m_dwPK_Device, *it, 0, SPEAKINTHEHOUSE_CONFERENCE_IVR);
				SendCommand(cmd);
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Telecom_Plugin::CMD_Speak_in_house: failed to "
				"get the list with embedded phones from the house");
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Telecom_Plugin::CMD_Speak_in_house: ain't got "
			"the extension/phone number of the caller");
	}
}

bool Telecom_Plugin::TelecomFollowMe( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	return HandleFollowMe(pMessage);
}

void Telecom_Plugin::FollowMe_EnteredRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
// 	// See if we have any pending media for this user
// 	CallData *pCallData_User = NULL,*pCallData_Remote = NULL,*pCallData_Room = NULL;
// 	// Matching the device (ie remote control) has first priority. 
// 	// Otherwise find the user, or whatever was in the prior room
// 
// 	std::list<CallData*> *calls = CallManager::getInstance()->getCallList();
// 
// #ifdef DEBUG
// 	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::FollowMe_EnteredRoom orbiter %d device %d user %d room %d calls %d",
// 		iPK_Orbiter, iPK_Device, iPK_Users, iPK_RoomOrEntArea, (int) calls->size());
// #endif
// 
// 	for(std::list<CallData*>::iterator it = calls->begin();it != calls->end();++it)
// 	{
// 		CallData *pCallData = *it;
// 		if( iPK_Device && iPK_Device==pCallData->m_PK_Device_Remote_get() )
// 			pCallData_Remote = pCallData;
// 
// 		if( iPK_Users && iPK_Users==pCallData->m_PK_Users_get() )
// 			pCallData_User = pCallData;
// 
// 		if( iPK_RoomOrEntArea_Left )
// 		{
// 			DeviceData_Router *pDevice = m_pRouter->m_mapDeviceData_Router_Find(pCallData->getOwnerDevID());
// 			if( pDevice && (int)pDevice->m_dwPK_Room==iPK_RoomOrEntArea_Left )
// 				pCallData_Room = pCallData;
// 		}
// 	}
// 
// 	// The call that matches this remote takes priority over the one for the user and then for the room
// 	CallData *pCallData = pCallData_Remote ? pCallData_Remote : (pCallData_User ? pCallData_User : pCallData_Room);
// 	if( !pCallData )
// 	{
// 		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Telecom_Plugin::FollowMe_EnteredRoom no call to transfer.  size %d",(int) calls->size());
// 		return;
// 	}
// 
// 	// We know the call.  Transfer it to a phone in iPK_RoomOrEntArea
// 	DeviceData_Router *pDevice_HardPhone=NULL,*pDevice_SoftPhone=NULL;  // See if we have a hard or soft phone in that room
// 	ListDeviceData_Router *pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Hard_Phones_CONST);
// 	if( pListDeviceData_Router != NULL )
// 	{
// 		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
// 		{
// 			if( (int)(*it)->m_dwPK_Room==iPK_RoomOrEntArea )
// 			{
// 				pDevice_HardPhone = *it;
// 				break;
// 			}
// 		}
// 	}
// 	
// 	pListDeviceData_Router = m_pRouter->m_mapDeviceByCategory_Find(DEVICECATEGORY_Soft_Phones_CONST);
// 	if( pListDeviceData_Router != NULL )
// 	{
// 		for(ListDeviceData_Router::iterator it=pListDeviceData_Router->begin();it!=pListDeviceData_Router->end();++it)
// 		{
// 			if( (int)(*it)->m_dwPK_Room==iPK_RoomOrEntArea )
// 			{
// 				pDevice_SoftPhone = *it;
// 				break;
// 			}
// 		}
// 	}
// 	
// 	if( pDevice_HardPhone==NULL && pDevice_SoftPhone==NULL )
// 	{
// 		LoggerWrapper::GetInstance()->Write(LV_WARNING,"Telecom_Plugin::FollowMe_EnteredRoom no phones in room %d",iPK_RoomOrEntArea);
// 		return;
// 	}

// TODO:
//	if( pDevice_SoftPhone )
//		CMD_PL_Transfer(pDevice_SoftPhone->m_dwPK_Device,0,"",pCallData->getID(),"" /* from */, false /*not conference*/);
//	else
//		CMD_PL_Transfer(pDevice_HardPhone->m_dwPK_Device,0,"",pCallData->getID(),"" /* from */, false /*not conference*/);
}

void Telecom_Plugin::FollowMe_LeftRoom(int iPK_Event, int iPK_Orbiter, int iPK_Device, int iPK_Users, int iPK_RoomOrEntArea, int iPK_RoomOrEntArea_Left)
{
}

//<-dceag-c921-b->

	/** @brief COMMAND: #921 - Make Call */
	/** Create a call. */
		/** @param #17 PK_Users */
			/** The called user. Only one is supported now. */
		/** @param #83 PhoneExtension */
			/** The phone number to be called. */
		/** @param #262 FK_Device_From */
			/** The device which starts the call. */
		/** @param #263 PK_Device_To */
			/** The called device. */

void Telecom_Plugin::CMD_Make_Call(int iPK_Users,string sPhoneExtension,int iFK_Device_From,int iPK_Device_To,string &sCMD_Result,Message *pMessage)
//<-dceag-c921-e->
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);

	string sPhoneNumber = GetPhoneNumber(iPK_Users, sPhoneExtension, iPK_Device_To);
	if( sPhoneNumber.empty() )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Nowhere to make a call !!!");
		sCMD_Result = "ERROR : Nowhere to make a call !";
		return;
	}
	
	if( !InternalMakeCall(iFK_Device_From, "", sPhoneNumber) )
	{
		sCMD_Result = "ERROR : Coudn't make a call from device " + StringUtils::itos(iFK_Device_From) + " to " + sPhoneNumber;
		return;
	}
}

//<-dceag-c924-b->

	/** @brief COMMAND: #924 - Merge Calls */
	/** Merge two calls */
		/** @param #267 Phone Call ID 1 */
			/** First call id */
		/** @param #268 Phone Call ID 2 */
			/** Second call id */

void Telecom_Plugin::CMD_Merge_Calls(string sPhone_Call_ID_1,string sPhone_Call_ID_2,string &sCMD_Result,Message *pMessage)
//<-dceag-c924-e->
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "CMD_Merge_Calls : %s, %s",
										sPhone_Call_ID_1.c_str(), sPhone_Call_ID_2.c_str() );

	DumpActiveCalls();

	map<string, CallStatus*>::const_iterator itFound_1 = map_call2status.find(sPhone_Call_ID_1);
	map<string, CallStatus*>::const_iterator itFound_2 = map_call2status.find(sPhone_Call_ID_2);
	if( itFound_1 != map_call2status.end() && itFound_2 != map_call2status.end() )
	{
		CallStatus * pCallStatus_1 = (*itFound_1).second;
		CallStatus * pCallStatus_2 = (*itFound_2).second;

		if( NULL != pCallStatus_1 && NULL != pCallStatus_2 )
		{
			string sConferenceID;
			CallStatus *pSecondCall = NULL;

			if(pCallStatus_1->IsConference())
			{
				sConferenceID = CallStatus::GetStringConferenceID(pCallStatus_1->GetConferenceID());
				pSecondCall = pCallStatus_2;
			}
			else if(pCallStatus_2->IsConference()) 
			{
				sConferenceID = CallStatus::GetStringConferenceID(pCallStatus_2->GetConferenceID());
				pSecondCall = pCallStatus_1;
			}
			else
			{
				sConferenceID = GetNewConferenceID();
				pSecondCall = pCallStatus_2;

				const map<string, string> & channels = pCallStatus_1->GetChannels();
				if(channels.size() == 2)
				{
					string sChannel1 = channels.begin()->first;
					string sChannel2 = (++channels.begin())->first;

					CMD_PBX_Transfer cmd_PBX_Transfer(
						m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
						sConferenceID,
						sChannel1, sChannel2);
					SendCommand(cmd_PBX_Transfer);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, 
						"Got a direct call, but it doesn't have 2 channels %s", 
						pCallStatus_1->GetID().c_str());

					DumpActiveCalls();
					return;
				}
			}

			const map<string, string> & channels = pSecondCall->GetChannels();

			if( pSecondCall->IsConference() )
			{
				for(map<string, string>::const_iterator itCh=channels.begin(); itCh!=channels.end(); ++itCh)
				{
					CMD_PBX_Transfer cmd_PBX_Transfer(
						m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
						sConferenceID,
						(*itCh).first, "");
					SendCommand(cmd_PBX_Transfer);
				}
			}
			else
			{
				if(channels.size() == 2)
				{
					string sChannel1 = channels.begin()->first;
					string sChannel2 = (++channels.begin())->first;

					CMD_PBX_Transfer cmd_PBX_Transfer(
						m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
						sConferenceID,
						sChannel1, sChannel2);
					SendCommand(cmd_PBX_Transfer);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, 
						"Got a direct call, but it doesn't have 2 channels %s", 
						pSecondCall->GetID().c_str());
				}
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "CMD_Merge_Calls : NULL call status!");
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "CMD_Merge_Calls : No such calls!");
	}

	DumpActiveCalls();
}

string Telecom_Plugin::GetPhoneNumber(int iPK_Users, string sPhoneExtension, int iPK_Device_To)
{
	string sPhoneNumber;

	if(iPK_Device_To != 0)
	{
		DeviceData_Router *pDeviceData = find_Device(iPK_Device_To);
		if(NULL != pDeviceData) 
		{
			if(
				pDeviceData->m_dwPK_DeviceCategory == DEVICECATEGORY_Standard_Orbiter_CONST ||
				pDeviceData->m_dwPK_DeviceCategory == DEVICECATEGORY_Mobile_Orbiter_CONST
			)
			{
				pDeviceData = find_Device(map_orbiter2embedphone[iPK_Device_To]);
				if(NULL != pDeviceData) 
				{
					sPhoneNumber = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device_To);
				}
			}
			else if(pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST)
			{
				sPhoneNumber = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
			}
			else
			{
				map<int,string>::iterator it = map_device2ext.find(iPK_Device_To);
				if(it != map_device2ext.end() && !it->second.empty())
				{
					sPhoneNumber = it->second;
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not phone device with id: %d", iPK_Device_To);
				}
			}
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device_To);
		}
	}

	if(iPK_Users != 0)
	{
		/*search user by id*/
		class Row_Users* rowuser;
		rowuser=m_pDatabase_pluto_main->Users_get()->GetRow(iPK_Users);
		if(rowuser)
		{
			sPhoneNumber =  StringUtils::itos(rowuser->Extension_get());
		}
		else
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No user found with id: %d", iPK_Users);
			return "";
		}
	}

	if(sPhoneExtension != "")
	{
		sPhoneNumber = sPhoneExtension;
	}
	
	return sPhoneNumber;
}

ExtensionStatus * Telecom_Plugin::FindExtensionStatusByDevice(int iPK_Device, int * pEmbeddedPhone /* = NULL*/)
{
	if(NULL != pEmbeddedPhone)
		*pEmbeddedPhone = 0;
	
	/*search device by id*/
	DeviceData_Router *pDeviceData = find_Device(iPK_Device);
	if(!pDeviceData) 
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
		return NULL;
	}

	if(pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_OnScreen_Orbiter_CONST)
	{
		pDeviceData = find_Device(map_orbiter2embedphone[iPK_Device]);
		if(!pDeviceData) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
			return NULL;
		}
		iPK_Device = map_orbiter2embedphone[iPK_Device];
		
		if(NULL != pEmbeddedPhone)
			*pEmbeddedPhone = iPK_Device;
	}
	else if(pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST)
	{
		if(NULL != pEmbeddedPhone)
			*pEmbeddedPhone = iPK_Device;
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "our device is : id %d template %d",
		pDeviceData->m_dwPK_Device,
		pDeviceData->m_dwPK_DeviceTemplate );
	
	/*find phonetype and phonenumber*/
	string sSrcPhoneNumber = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
	
	return FindExtensionStatus(sSrcPhoneNumber);
}

void Telecom_Plugin::GetChannelsFromExtension(const string & ext, map<string, string> & channelsExt) const
{
	CallStatus *pCallStatus = NULL;
	
	for(map<string, CallStatus*>::const_iterator it=map_call2status.begin(); it!=map_call2status.end(); ++it)
	{
		pCallStatus = (*it).second;
		const map<string, string> & channels = pCallStatus->GetChannels();
		for(map<string, string>::const_iterator itCh=channels.begin(); itCh!=channels.end(); ++itCh)
		{
			if( ext == ((*itCh).first) )
			{
				channelsExt[(*itCh).first] = (*itCh).second;
			}
		}
	}
}

string Telecom_Plugin::GetCallName(CallStatus * pCallStatus) const
{
	string sName = "Call ";
	
	if( pCallStatus->IsConference() )
	{
		sName = "Conference ";
	}
	
	if( pCallStatus->GetChannels().size() )
	{
		map<string, string>::const_iterator itChannel = pCallStatus->GetChannels().begin();
		sName += (*itChannel).second;
	}
	else
	{
		sName += "NO CHANNELS";
	}
		
	return sName;
}

void Telecom_Plugin::DumpActiveCalls()
{
	string sDebugInfo;
	CallStatus *pCallStatus = NULL;
	for(map<string, CallStatus*>::const_iterator it=map_call2status.begin(); it!=map_call2status.end(); ++it)
	{
		pCallStatus = it->second;
		sDebugInfo += pCallStatus->GetDebugInfo() + "\n";
	}

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Active calls %d: \n%s", map_call2status.size(), sDebugInfo.c_str());
}

/*static*/ string Telecom_Plugin::ExtensionForChannel(string sChannel)
{
	size_t pos1 = 0;
	size_t pos2 = 0;

	// SIP/extension-uid
	pos1 = sChannel.find("/");
	pos2 = sChannel.rfind("-");
	if( pos1 != string::npos && pos2 != string::npos && pos1 < pos2 )
		return sChannel.substr(pos1+1, pos2-pos1-1);

	return "";
}

bool Telecom_Plugin::InternalMakeCall(int iFK_Device_From, string sFromExten, string sPhoneNumberToCall)
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "InternalMakeCall (dev %d, exten %s) -> %s",
		iFK_Device_From, sFromExten.c_str(), sPhoneNumberToCall.c_str());

	int iEmbeddedPhone = 0;
	ExtensionStatus * pExtStatus = NULL;
	ExtensionStatus::ExtensionType aExtenType = ExtensionStatus::OTHER;

	if(iFK_Device_From == 0)
	{
		iFK_Device_From = GetOrbiterDeviceID(sFromExten);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "InternalMakeCall the extension %s is assigned to device %d", 
				sFromExten.c_str(), iFK_Device_From);
	}

	if(iFK_Device_From != 0)
		pExtStatus = FindExtensionStatusByDevice(iFK_Device_From, &iEmbeddedPhone);
	else
		pExtStatus = FindExtensionStatus(sFromExten);

	if(NULL != pExtStatus) 
	{
		sFromExten = pExtStatus->GetID();
		aExtenType = pExtStatus->GetExtensionType();

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "InternalMakeCall found exten status exten %s, type %s",
			sFromExten.c_str(), ExtensionStatus::Type2String(aExtenType).c_str());
	}

	//TODO: get caller id from db ?
	string sCallerID = sFromExten;

	if(m_pDevice_pbx)
	{
		if( iEmbeddedPhone != 0 )
		{
			// iFK_Device_From has to be the Orbiter device
			iFK_Device_From = map_embedphone2orbiter[iEmbeddedPhone];
			if( iFK_Device_From == 0 )
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No orbiter for simplephone %d !", iEmbeddedPhone);
			}
			
			// stop the media
			DCE::CMD_MH_Stop_Media CMD_MH_Stop_Media_
				(iFK_Device_From, DEVICETEMPLATE_VirtDev_Media_Plugin_CONST,0,0,0,"",false);
			SendCommand(CMD_MH_Stop_Media_);
			//Sleep(1000);
			
			DCE::CMD_Phone_Initiate cmdInitiate(m_dwPK_Device, iEmbeddedPhone, iEmbeddedPhone, sPhoneNumberToCall);
			SendCommand(cmdInitiate);
		}
		else
		{
			/*send originate command to PBX*/
			CMD_PBX_Originate cmd_PBX_Originate(m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
				sFromExten,
				ExtensionStatus::Type2String(aExtenType),
				sPhoneNumberToCall, sCallerID);
			SendCommand(cmd_PBX_Originate);
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No PBX device!");
		return false;
	}
	
	return true;
}

bool Telecom_Plugin::GetEmbeddedPhoneAssociated(int nDeviceID, int& nEmbeddedPhoneID)
{
	nEmbeddedPhoneID = 0;

	//already an embedded device?
	if(map_embedphone2orbiter.find(nDeviceID) != map_embedphone2orbiter.end())
	{
		nEmbeddedPhoneID = nDeviceID;
		return true;
	}

    //maybe it's the osd orbiter?
	map<int,int>::iterator itFound = map_orbiter2embedphone.find(nDeviceID);
	if(map_orbiter2embedphone.end() !=  itFound)   
	{
		nEmbeddedPhoneID = itFound->second;
		return true;
	}

	//maybe it's a remote orbiter
	OH_Orbiter *pOH_Orbiter = m_pOrbiter_Plugin->m_mapOH_Orbiter_Find(nDeviceID);
	if(NULL != pOH_Orbiter)
	{
		//the list with orbiters from that room
		string sOrbiters = m_pOrbiter_Plugin->PK_Device_Orbiters_In_Room_get(pOH_Orbiter->m_dwPK_Room, false);

		vector<string> vectOrbiters;
		StringUtils::Tokenize(sOrbiters, ",", vectOrbiters);

		//which one of them has a simple phone as child? get the first one
		for(vector<string>::iterator it = vectOrbiters.begin(); it != vectOrbiters.end(); ++it)
		{
			itFound = map_orbiter2embedphone.find(atoi(it->c_str()));
			if(map_orbiter2embedphone.end() != itFound)
			{
				nEmbeddedPhoneID = itFound->second;
				return true;
			}
		}
	}

	//got nothing
	return false;
}

int Telecom_Plugin::GetOrbiterDeviceID(string sExten, string sChannel /*= ""*/)
{
	if(!sChannel.empty())
		sExten = ExtensionForChannel(sChannel);

	map<string,int>::iterator it = map_ext2device.find(sExten);
	
	if(it != map_ext2device.end())
	{
		map<int,int>::iterator ito = map_embedphone2orbiter.find(it->second);
		if(ito != map_embedphone2orbiter.end())
			return ito->second;
	}

	return 0;
}	

CallStatus *Telecom_Plugin::FindConferenceByConferenceID(unsigned int unConferenceID)
{
	for(map<string, CallStatus*>::iterator it = map_call2status.begin(); it != map_call2status.end(); ++it)
	{
		CallStatus *pCallStatus = it->second;

		if(pCallStatus->IsConference() && pCallStatus->GetConferenceID() == unConferenceID)
			return pCallStatus;
	}

	return NULL;
}


//<-dceag-c925-b->

	/** @brief COMMAND: #925 - Assisted Transfer */
	/** Interactive transfer, not just a blind transfer.
The response is a task id */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */
		/** @param #17 PK_Users */
			/** User ID to transfer call to */
		/** @param #83 PhoneExtension */
			/** Extension to transfer call to */
		/** @param #87 PhoneCallID */
			/** Phone call id to transfer  */
		/** @param #257 Task */
			/** Task ID */
		/** @param #264 Channel */
			/** The channel of the owner of the assisted transfer */

void Telecom_Plugin::CMD_Assisted_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,string sPhoneCallID,string sChannel,string *sTask,string &sCMD_Result,Message *pMessage)
//<-dceag-c925-e->
{
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);

	if(NULL != sTask)
		*sTask = "";
	
	CallStatus *pCallStatus = FindCallStatusForChannel(sChannel);
	if(NULL == pCallStatus)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not a valid channel %s!", sChannel.c_str());
		sCMD_Result = "ERROR : Not a valid channel " + sChannel;
		return;
	}

	string sPhoneNumber = GetPhoneNumber(iPK_Users, sPhoneExtension, iPK_Device);
	AssistedTransfer * transfer = new AssistedTransfer(sPhoneNumber, sPhoneCallID, sChannel, pCallStatus->GetID());
	if( transfer )
	{
		map_id2task[transfer->GetID()] = transfer;
		transfer->ProcessJob("initialize");

		if(NULL != sTask)
			*sTask = transfer->GetID();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Not enough memory!");
		sCMD_Result = "ERROR";
	}
}

//<-dceag-c926-b->

	/** @brief COMMAND: #926 - Process Task */
	/** Process a task with actions like "complete transfer now", "merge calls", etc. */
		/** @param #257 Task */
			/** The task id */
		/** @param #258 Job */
			/** The job type */

void Telecom_Plugin::CMD_Process_Task(string sTask,string sJob,string &sCMD_Result,Message *pMessage)
//<-dceag-c926-e->
{
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Process_Task: %s", sTask.c_str());
	
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);
	
	map<string, TelecomTask*>::const_iterator itFound = map_id2task.find(sTask);
	if( itFound == map_id2task.end() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No such task: %s", sTask.c_str());
		sCMD_Result = "ERROR : No such task " + sTask;
		return;
	}
	
	if( (*itFound).second->ProcessJob(sJob) )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Error task: %s , job: %s",
			sTask.c_str(), sJob.c_str());
		sCMD_Result = "ERROR : Couldn't process task " + sTask + " with job " + sJob;
		return;
	}
}

string Telecom_Plugin::FindChannelForExt(string ext)
{
	string sChannelFound;
	for(map<string, CallStatus*>::const_iterator it=map_call2status.begin(); it!=map_call2status.end(); ++it)
	{
		sChannelFound = FindChannelForExt((*it).second, ext);
		if( !sChannelFound.empty() )
		{
			return sChannelFound;
		}
	}
	
	return "";
}

string Telecom_Plugin::FindChannelForExt(CallStatus * pCallStatus, string ext)
{
	const map<string, string> & channels = pCallStatus->GetChannels();
	for( map<string, string>::const_iterator it=channels.begin(); it!=channels.end(); ++it)
	{
		if( ext == ExtensionForChannel((*it).first) )
		{
			return (*it).first;
		}
	}
	
	return "";
}
//<-dceag-c927-b->

	/** @brief COMMAND: #927 - Add Extensions To Call */
	/** Add comma delimited list with extensions to call */
		/** @param #87 PhoneCallID */
			/** The phone call id */
		/** @param #274 Extensions */
			/** Comma delimited list with extensions */

void Telecom_Plugin::CMD_Add_Extensions_To_Call(string sPhoneCallID,string sExtensions,string &sCMD_Result,Message *pMessage)
//<-dceag-c927-e->
{
	//NOTE: these all extensions should be present in this call and ONLY these ones
	// 1) if one of the extensions is in the call, leave it
	// 2) if one of the extensions is not in the call, add it
	// 3) if in the call there is an extension which is not in the list with extension, drop it
	
	if( m_pDevice_pbx == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Add_Extensions_To_Call : No Asterisk device");
		sCMD_Result = "ERROR : No Asterisk device";
		return;
	}
	
	PLUTO_SAFETY_LOCK(vm, m_TelecomMutex);
	
	CallStatus * pCallStatus = FindCallStatusForID(sPhoneCallID);
	if( pCallStatus == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "No such call ID: %s", sPhoneCallID.c_str());
		sCMD_Result = "ERROR : No such call " + sPhoneCallID;
		return;
	}
	
	string sConferenceID = sPhoneCallID;
	if( !pCallStatus->IsConference() )
	{
		sConferenceID = DirectCall2Conference(pCallStatus);
	}
	
	vector<string> vectExtensions;
	vector<string> droppedChannels;
	StringUtils::Tokenize(sExtensions, ",", vectExtensions);
	if( 0 == vectExtensions.size() )
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "CMD_Add_Extensions_To_Call : no extensions");
		sCMD_Result = "ERROR : No extensions to call!";
		return;
	}
	
	const map<string, string> & channels = pCallStatus->GetChannels();
	
	// find all the channels which has to be dropped and all the channels which are kept
	string sChannelExt;
	for(map<string, string>::const_iterator itCh=channels.begin(); itCh!=channels.end(); ++itCh)
	{
		sChannelExt = ExtensionForChannel((*itCh).first);
		if( !sChannelExt.empty() )
		{
			vector<string>::iterator itFound = vectExtensions.begin();
			for(; itFound!=vectExtensions.end(); ++itFound)
			{
				if( (*itFound) == sChannelExt )
				{
					break;
				}
			}
			
			if( itFound != vectExtensions.end() )
			{
				// the extension is already into the call, ignore it
				vectExtensions.erase(itFound);
			}
			else
			{
				droppedChannels.push_back((*itCh).first);
			}
		}
	}
	
	// add all the remaining extensions to the call
	string sChannelID;
	for(vector<string>::iterator itExt=vectExtensions.begin(); itExt!=vectExtensions.end(); ++itExt)
	{
		sChannelID = FindChannelForExt(*itExt);
		if( sChannelID.empty() )
		{
			if( !InternalMakeCall(0, (*itExt), sConferenceID) )
			{
				// what to do ?
			}
		}
		else
		{
			CMD_PBX_Transfer cmd_PBX_Transfer(
				m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device,
				sConferenceID, sChannelID, "" );
			SendCommand(cmd_PBX_Transfer);
		}
	}
	
	// the channels have to be dropped
	for(vector<string>::const_iterator itDrop=droppedChannels.begin(); itDrop!=droppedChannels.end(); ++itDrop)
	{
		CMD_PBX_Hangup cmd_PBX_Hangup(m_dwPK_Device, m_pDevice_pbx->m_dwPK_Device, (*itDrop));
		SendCommand(cmd_PBX_Hangup);
	}
}

//<-dceag-c928-b->

	/** @brief COMMAND: #928 - Get Associated Picture For Channel */
	/** Get the associated picture for channel */
		/** @param #19 Data */
			/** The picture associated */
		/** @param #264 Channel */
			/** The channel id */

void Telecom_Plugin::CMD_Get_Associated_Picture_For_Channel(string sChannel,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c928-e->
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Get_Associated_Picture_For_Channel: got to load picture for channel %s", sChannel.c_str());

	if(NULL != pData && NULL != iData_Size)
	{
		//default path for unknown user
		string sPath = "/usr/pluto/orbiter/skins/Basic/Users/UnknownUser.jpg";

		//TODO: find associated user

		size_t ulSize = 0;
		*pData = FileUtils::ReadFileIntoBuffer(sPath, ulSize);
		*iData_Size = static_cast<int>(ulSize);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "CMD_Get_Associated_Picture_For_Channel: loaded picture size %d", *iData_Size);
	}
}
