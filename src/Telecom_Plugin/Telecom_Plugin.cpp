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
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCERouter.h"

#include "Datagrid_Plugin/Datagrid_Plugin.h"
#include "DCE/DataGrid.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Define_Array.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_DataGrid.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Table_Users.h"

#include "callmanager.h"

#ifndef WIN32
#include "ldapmanager.h"

using namespace LDAPSERVER;


#define LDAP_HOST			"localhost"
#define LDAP_PORT			389
#define LDAP_BINDNAME		"cn=admin,dc=plutohome,dc=org"
#define LDAP_BINDSECRET		"secret"

#define LDAP_PHONE_BUSINESS	"telephoneNumber"
#define LDAP_PHONE_HOME		"homePhone"
#define LDAP_PHONE_MOBILE	"mobile"

static struct _PHONEMAPENTRY {
	const char* ldapattr;
	const char* display;
} _phonemap[] = {
		{"telephoneNumber", "Business"},
		{"homePhone", "Home"},
		{"mobile", "Mobile"},
};
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Telecom_Plugin::Telecom_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Telecom_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return;
	}

	iCmdCounter = 0;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Telecom_Plugin::~Telecom_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	delete g_pPlutoLogger;	// Created in either main or RegisterAsPlugin.  When this exits we won't need it anymore
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Telecom_Plugin::Register()
//<-dceag-reg-e->
{
	m_pDatagrid_Plugin=NULL;
	ListCommand_Impl *pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Datagrid_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Media grids cannot find datagrid handler %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pDatagrid_Plugin=(Datagrid_Plugin *) pListCommand_Impl->front();
	m_pOrbiter_Plugin=NULL;
	
	pListCommand_Impl = m_pRouter->m_mapPlugIn_DeviceTemplate_Find(DEVICETEMPLATE_Orbiter_Plugin_CONST);

	if( !pListCommand_Impl || pListCommand_Impl->size()!=1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Orbiter grids cannot find Orbiter plug-in %s",(pListCommand_Impl ? "There were more than 1" : ""));
		return false;
	}

	m_pOrbiter_Plugin=(Orbiter_Plugin *) pListCommand_Impl->front();
	
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::TelecomScenariosGrid))
		,DATAGRID_Telecom_Scenarios_CONST);
		
	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::PhoneBookAutoCompl))
		,DATAGRID_Phone_Book_Auto_Compl_CONST);

	m_pDatagrid_Plugin->RegisterDatagridGenerator(
		new DataGridGeneratorCallBack(this,(DCEDataGridGeneratorFn)(&Telecom_Plugin::PhoneBookListOfNos))
		,DATAGRID_Phone_Book_List_of_Nos_CONST);

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::CommandResult ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_PBX_CommandResult_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::Ring ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_PBX_Ring_CONST );

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
void Telecom_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
	g_pPlutoLogger->Write(LV_STATUS, "Phone Book Autocomplete request received for GridID: %s with Params: %s.", 
																					GridID.c_str(), Parms.c_str());
																				
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
#ifndef WIN32
	// get uid and filter text 
	string uid, filtertxt;
	int index = Parms.find('|');
	if(index < 0) {
		uid = Parms;
	} else  {
		uid = Parms.substr(0, index);
		filtertxt = Parms.substr(index + 1, Parms.length() - index);
	}
	
	if(uid.length() == 0) {
		g_pPlutoLogger->Write(LV_CRITICAL, "No user ID passed in parameters.");
		return pDataGrid;
	}
			
	// find user name given the user ID sent in Parms
	Row_Users *pUsers = m_pDatabase_pluto_main->Users_get()->GetRow(atol(uid.c_str()));
	if(!pUsers) {
		g_pPlutoLogger->Write(LV_STATUS, "User with ID: %s not found.", uid.c_str());
		return pDataGrid;
	}
	
	string uname = pUsers->UserName_get();
	
	LDAPManager ldapm(LDAP_HOST, LDAP_PORT);
	try {
		ldapm.SimpleBind(LDAP_BINDNAME, LDAP_BINDSECRET);
	
		string searchbase = "dc=" + uname + ", dc=plutohome, dc=org";
		string searchfilter = "(&(objectclass=person)" + 
											((filtertxt.length() > 0) ? "(cn=*" + filtertxt + "*)" : "")
											+ ")";
		
		g_pPlutoLogger->Write(LV_STATUS, "Searching LDAP server from Base: %s with Filter: %s", 
													searchbase.c_str(), searchfilter.c_str());
		
		LDAPEntryCollectionPtr pcol = 
						ldapm.Search(searchbase.c_str(), LDAP_SCOPE_SUBTREE, searchfilter.c_str(), NULL, 0);
	
						
		int numEntries = 0;
				
		LDAPEntryPtr pe = pcol->First();
		while(pe.get() != NULL) {
			const LDAPATTRIBUTESVEC& attrs = pe->GetAttrs();

			for(LDAPATTRIBUTESVEC::const_iterator it = attrs.begin();
								it != attrs.end(); it++) 
			{
				if((*it).GetName() == "cn") {
					const LDAPVALUESVEC& values = (*it).GetValues();
						if(values.size() > 0 && values.begin()->length() > 0) {
							pCell = new DataGridCell(*values.begin(), uname + "|" + *values.begin());
							pDataGrid->SetData(0, numEntries++, pCell);
						}
				}
			}
			pe = pcol->Next(pe);
		}

		ldapm.Unbind();
	} catch(LDAPException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "LDAP raised exception: %s.", e.GetErrString());
	}
	
#endif	
	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::PhoneBookListOfNos(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	g_pPlutoLogger->Write(LV_STATUS, "Phone Book List of NOS request received for GridID: %s with Params: %s.", 
																					GridID.c_str(), Parms.c_str());
	
	DataGridTable *pDataGrid = new DataGridTable();
	DataGridCell *pCell;
#ifndef WIN32
	string uname, cn;
	
	int index = Parms.find('|');
	if(index > 0) {
		uname = Parms.substr(0, index);
		cn = Parms.substr(index + 1, Parms.length() - index);
	}
	
	if(uname.length() == 0 || cn.length() == 0) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Invalid params passed.");
		return pDataGrid;
	}
	
	g_pPlutoLogger->Write(LV_STATUS, "Searching phones for User: %s, Entry: %s", 
													uname.c_str(), cn.c_str());

	LDAPManager ldapm(LDAP_HOST, LDAP_PORT);
	try {
		ldapm.SimpleBind(LDAP_BINDNAME, LDAP_BINDSECRET);
	
		string searchbase = "dc=" + uname + ", dc=plutohome, dc=org";
		string searchfilter = "(cn=" + cn + ")";
		
		g_pPlutoLogger->Write(LV_STATUS, "Searching LDAP server from Base: %s with Filter: %s", 
													searchbase.c_str(), searchfilter.c_str());
		
		LDAPEntryCollectionPtr pcol = 
						ldapm.Search(searchbase.c_str(), LDAP_SCOPE_SUBTREE, searchfilter.c_str(), NULL, 0);
		
		int numEntries = 0;
				
		LDAPEntryPtr pe = pcol->First();
		if(pe.get() != NULL) {
			const LDAPATTRIBUTESVEC& attrs = pe->GetAttrs();
			for(LDAPATTRIBUTESVEC::const_iterator it = attrs.begin();
								it != attrs.end(); it++) 
			{
				for(unsigned int i = 0; 
						i < sizeof(_phonemap) / sizeof(struct _PHONEMAPENTRY); i++) 
				{
					if((*it).GetName() == _phonemap[i].ldapattr) {
						const LDAPVALUESVEC& values = (*it).GetValues();
						if(values.size() > 0 && values.begin()->length() > 0) {
							pCell = new DataGridCell( string(_phonemap[i].display) + " [" + *values.begin() +"]", *values.begin() );
							pDataGrid->SetData(0, numEntries++, pCell);
						}
					}
				}
			}
		}

		ldapm.Unbind();
	} catch(LDAPException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "LDAP raised exception: %s.", e.GetErrString());
	}
#endif	
	return pDataGrid;
}


bool 
Telecom_Plugin::CommandResult( class Socket *pSocket, class Message *pMessage,
		                                class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo ) {
		
	g_pPlutoLogger->Write(LV_STATUS, "Command Result received from PBX.");
	int iCommandID = atoi(pMessage->m_mapParameters[EVENTPARAMETER_CommandID_CONST].c_str());
	int iResult = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str());
	string sMessage = pMessage->m_mapParameters[EVENTPARAMETER_Message_CONST];

	ProcessResult(iCommandID, iResult, sMessage);
	return true;
}

void 
Telecom_Plugin::ProcessResult(int iCommandID, int iResult, std::string sMessage) {
	CallManager *pCallManager = CallManager::getInstance();
	CallData *pCallData	= pCallManager->findCallByPendingCmdID(iCommandID);
	if(!pCallData) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Obsolete command reply received.");
		return;
	}
	
	DeviceData_Router* pDeviceData = find_Device(pCallData->getOwnerDevID());
	if(!pDeviceData) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Device %d is no more Available. Destroying call.", pCallData->getOwnerDevID());
		pCallManager->removeCall(pCallData);
		return;
	}

	switch(pCallData->getState()) {
		case CallData::STATE_ORIGINATING:
			if(!iResult) {
				pCallData->setState(CallData::STATE_OPENED);
				pCallData->setID(sMessage);
				
				/*switch orbiter screen*/
				if(pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_CONST) {
					CMD_Goto_Screen cmd_CMD_Goto_Screen(m_dwPK_Device, pDeviceData->m_dwPK_Device, 
										m_dwPK_Device, "1281", pCallData->getID(), "", false, false);
					SendCommand(cmd_CMD_Goto_Screen);
				}
			} else {
				/*switch orbiter screen*/
				pCallManager->removeCall(pCallData);
				
			}
			break;
		case CallData::STATE_TRANSFERING:
			if(!iResult) {
			} else {
			}
			pCallData->setState(CallData::STATE_OPENED);
			break;
	}
}

bool 
Telecom_Plugin::Ring( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo ) {
	g_pPlutoLogger->Write(LV_STATUS, "Ring event received from PBX.");
	
	string sPhoneExtension = pMessage->m_mapParameters[EVENTPARAMETER_PhoneExtension_CONST];
	string sPhoneCallID = pMessage->m_mapParameters[EVENTPARAMETER_PhoneCallID_CONST];
	string sPhoneCallerID = pMessage->m_mapParameters[EVENTPARAMETER_PhoneCallerID_CONST];
	
	ProcessRing(sPhoneExtension, sPhoneCallerID, sPhoneCallID);
	return true;
}

void 
Telecom_Plugin::ProcessRing(std::string sPhoneExtension, std::string sPhoneCallerID, std::string sPhoneCallID) {
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
			g_pPlutoLogger->Write(LV_CRITICAL, errStr.c_str());
			//return -1;
		}
		
		pDeviceData = pListDeviceData->front();
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Asterisk Handler not found...");
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
	/** Originate a call */
		/** @param #2 PK_Device */
			/** Device (phone) from which to place the call */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */

void Telecom_Plugin::CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c232-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Originate cammand called with params: DeviceID=%d, PhoneExtension=%s!", iPK_Device, sPhoneExtension.c_str());

	if(sPhoneExtension.empty()) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Error validating input parameters");
		return;
	}
	
    /*search device by id*/
    DeviceData_Router *pDeviceData = find_Device(iPK_Device);
 	if(!pDeviceData) {
		g_pPlutoLogger->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
		return;
	}

	/*find phonetype and phonenumber*/
	string sSrcPhoneType = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneType_CONST);
	string sSrcPhoneNumber = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);

	g_pPlutoLogger->Write(LV_STATUS, "Using source phone with parameters: PhoneType=%s, PhoneNumber=%s!", 
													sSrcPhoneType.c_str(), sSrcPhoneNumber.c_str());
				
	
	/*find PBX*/
	DeviceData_Router* pPBXDevice = find_AsteriskDevice();
	if(pPBXDevice) {
	
		CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(pMessage->m_dwPK_Device_From);
		if(!pCallData) {
			/*create new call data*/
			pCallData = new CallData();
			pCallData->setOwnerDevID(pMessage->m_dwPK_Device_From);
			CallManager::getInstance()->addCall(pCallData);
		}
		
		pCallData->setState(CallData::STATE_ORIGINATING);

		/*send originate command to PBX*/
		pCallData->setPendingCmdID(generate_NewCommandID());
		CMD_PBX_Originate cmd_PBX_Originate(m_dwPK_Device, pPBXDevice->m_dwPK_Device,
					                        sSrcPhoneNumber, sSrcPhoneType, sPhoneExtension, "CALLERID", pCallData->getPendingCmdID());
	    SendCommand(cmd_PBX_Originate);
	}
}




//<-dceag-c234-b->

	/** @brief COMMAND: #234 - PL_TransferConferenceDevice */
	/** Transfers a call to other phone */
		/** @param #2 PK_Device */
			/** Device ID to transfer call to */

void Telecom_Plugin::CMD_PL_TransferConferenceDevice(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c234-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Transfer cammand called with params: DeviceID=%d!", iPK_Device);
	
	/*search device by id*/
	DeviceData_Router *pDeviceData = find_Device(iPK_Device);
	if(!pDeviceData) {
		g_pPlutoLogger->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
		return;
    }

	/*find phonetype and phonenumber*/
    string sPhoneType = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneType_CONST);
	string sPhoneNumber = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
	
	CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(pMessage->m_dwPK_Device_From);
	if(!pCallData) {
		return;
	}
	
	/*find PBX*/
	DeviceData_Router* pPBXDevice = find_AsteriskDevice();
	if(pPBXDevice) {
		pCallData->setState(CallData::STATE_TRANSFERING);
		pCallData->setPendingCmdID(generate_NewCommandID());
				
		/*send transfer command to PBX*/
		CMD_PBX_Transfer cmd_PBX_Transfer(m_dwPK_Device, pPBXDevice->m_dwPK_Device, 
									"0" + sPhoneNumber, pCallData->getPendingCmdID(), pCallData->getID());
		SendCommand(cmd_PBX_Transfer);
	}
}
	
//<-dceag-c236-b->

	/** @brief COMMAND: #236 - PL_Hangup */
	/** Hangs up a call */

void Telecom_Plugin::CMD_PL_Hangup(string &sCMD_Result,Message *pMessage)
//<-dceag-c236-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Hangup cammand called.");
	
	CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(pMessage->m_dwPK_Device_From);
	if(!pCallData) {
		return;
	}
	
	/*find PBX*/
	DeviceData_Router* pPBXDevice = find_AsteriskDevice();
	if(pPBXDevice) {
		/*send transfer command to PBX*/
		CMD_PBX_Hangup cmd_PBX_Hangup(m_dwPK_Device, pPBXDevice->m_dwPK_Device, 
								0, pCallData->getID());
		SendCommand(cmd_PBX_Hangup);
		
		CallManager::getInstance()->removeCall(pCallData);
	}	
}
//<-dceag-createinst-b->!
