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
#include "pluto_main/Table_Users.h"
#include "pluto_telecom/Table_Contact.h"
#include "pluto_telecom/Table_PhoneNumber.h"
#include "pluto_telecom/Table_PhoneType.h"
#include "callmanager.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Telecom_Plugin::Telecom_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Telecom_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = NULL;
	m_pDatabase_pluto_telecom = NULL;
	iCmdCounter = 0;
}

//<-dceag-getconfig-b->
bool Telecom_Plugin::GetConfig()
{
	if( !Telecom_Plugin_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->

	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return false;
	}

	m_pDatabase_pluto_telecom = new Database_pluto_telecom();
	if(!m_pDatabase_pluto_telecom->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),"pluto_telecom",m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to telecom database!");
		m_bQuit=true;
		return false;
	}
	vector<class Row_Device*> vectDevices;
	m_pDatabase_pluto_main->Device_get()->GetRows(DEVICE_FK_DEVICETEMPLATE_FIELD + string("=") + StringUtils::itos(DEVICETEMPLATE_Orbiter_Embedded_Phone_CONST), &vectDevices);
	for(size_t s=0;s<vectDevices.size();++s)
	{
		map_orbiter2embedphone[vectDevices[s]->FK_Device_ControlledVia_get()] = vectDevices[s]->PK_Device_get();
	}

	return true;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Telecom_Plugin::~Telecom_Plugin()
//<-dceag-dest-e->
{
	delete m_pDatabase_pluto_main;
	
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
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find sister plugins");
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

	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::CommandResult ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_PBX_CommandResult_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::Ring ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_PBX_Ring_CONST );
	RegisterMsgInterceptor( ( MessageInterceptorFn )( &Telecom_Plugin::IncomingCall ), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Incoming_Call_CONST );	

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
	g_pPlutoLogger->Write(LV_STATUS, "Phone Book Autocomplete request received for GridID: %s with Params: %s.", 
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
		pDataGrid->SetData(0, FirstBatch, pCell);
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
		pDataGrid->SetData(0, FirstBatch + s, pCell);
	}

	return pDataGrid;
}

class DataGridTable *Telecom_Plugin::PhoneBookListOfNos(string GridID,string Parms,void *ExtraData,int *iPK_Variable,string *sValue_To_Assign,class Message *pMessage)
{
	g_pPlutoLogger->Write(LV_STATUS, "Phone Book List of NOS request received for GridID: %s with Params: %s.", 
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
					CMD_Goto_DesignObj cmd_CMD_Goto_DesignObj(m_dwPK_Device, pDeviceData->m_dwPK_Device, 
										m_dwPK_Device, "1281", pCallData->getID(), "", false, false);
					SendCommand(cmd_CMD_Goto_DesignObj);
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
Telecom_Plugin::ProcessRing(std::string sPhoneExtension, std::string sPhoneCallerID, std::string sPhoneCallID)
{
	int phoneID=0;
	vector<class Row_Device_DeviceData*> vectDeviceData;
	m_pDatabase_pluto_main->Device_DeviceData_get()->GetRows(DEVICE_DEVICEDATA_FK_DEVICEDATA_FIELD+string("=") + StringUtils::itos(DEVICEDATA_PhoneNumber_CONST)+ " AND " +
	                                                         DEVICE_DEVICEDATA_IK_DEVICEDATA_FIELD +string("=") + sPhoneExtension , &vectDeviceData);
	if(vectDeviceData.size()>0)
	{
		phoneID= vectDeviceData[0]->FK_Device_get();
		CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(phoneID);
		if(!pCallData) {
			/*create new call data*/
			pCallData = new CallData();
			pCallData->setOwnerDevID(phoneID);
			pCallData->setID(sPhoneCallID);
			CallManager::getInstance()->addCall(pCallData);
			
			g_pPlutoLogger->Write(LV_STATUS, "Creating calldata for ringing device %d (ext %s, id %s)",phoneID,sPhoneExtension.c_str(),sPhoneCallID.c_str());			
			pCallData->setState(CallData::STATE_NOTDEFINED);
		}
		else
		{
			pCallData->setID(sPhoneCallID);
			pCallData->setState(CallData::STATE_NOTDEFINED);			
			g_pPlutoLogger->Write(LV_WARNING, "This should not happend, already have a call on device %d in state %d",phoneID,pCallData->getState());
			g_pPlutoLogger->Write(LV_WARNING, "However if this is a hardphone...this is probably possible");
		}
	}
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
		/** @param #84 PhoneCallerID */
			/** Caller ID */

void Telecom_Plugin::CMD_PL_Originate(int iPK_Device,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,Message *pMessage)
//<-dceag-c232-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Originate command called with params: DeviceID=%d, PhoneExtension=%s!", iPK_Device, sPhoneExtension.c_str());

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
	if(pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_CONST)
	{
    	pDeviceData = find_Device(map_orbiter2embedphone[iPK_Device]);
 		if(!pDeviceData) {
			g_pPlutoLogger->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
			return;
		}
	}
	if(sPhoneCallerID == "")
	{
		sPhoneCallerID="pluto";
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
					                        sSrcPhoneNumber, sSrcPhoneType, sPhoneExtension, sPhoneCallerID, pCallData->getPendingCmdID());
	    SendCommand(cmd_PBX_Originate);
	}
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
		/** @param #196 IsConference */
			/** Transfer the call to a conference room? */

void Telecom_Plugin::CMD_PL_Transfer(int iPK_Device,int iPK_Users,string sPhoneExtension,bool bIsConference,string &sCMD_Result,Message *pMessage)
//<-dceag-c234-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Transfer command called with params: DeviceID=%d UserID=%d Extension=%s", iPK_Device,iPK_Users,sPhoneExtension.c_str());
	string sPhoneNumber;
	if(iPK_Device != 0)
	{
		/*search device by id*/
		DeviceData_Router *pDeviceData = find_Device(iPK_Device);
		if(!pDeviceData) {
			g_pPlutoLogger->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
			return;
	    }
		if(pDeviceData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Orbiter_CONST)
		{
	    	pDeviceData = find_Device(map_orbiter2embedphone[iPK_Device]);
 			if(!pDeviceData) {
				g_pPlutoLogger->Write(LV_CRITICAL, "No device found with id: %d", iPK_Device);
				return;
			}
		}
		sPhoneNumber = pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
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
			g_pPlutoLogger->Write(LV_CRITICAL, "No user found with id: %d", iPK_Users);
			return;
		}
	}
	if(sPhoneExtension != "")
	{
		sPhoneNumber=sPhoneExtension;
	}
	if(sPhoneNumber == "")
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Nowhere to transfer !!!");
		return;
	}
	
	CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(pMessage->m_dwPK_Device_From);
	if(!pCallData) {
		g_pPlutoLogger->Write(LV_WARNING, "No calldata found for device %d",pMessage->m_dwPK_Device_From);
	
		pCallData = CallManager::getInstance()->findCallByOwnerDevID(map_orbiter2embedphone[pMessage->m_dwPK_Device_From]);
		if(!pCallData) {
			g_pPlutoLogger->Write(LV_WARNING, "No calldata found for device %d",map_orbiter2embedphone[pMessage->m_dwPK_Device_From]);
			return;
		}
	}	
	/*find PBX*/
	DeviceData_Router* pPBXDevice = find_AsteriskDevice();
	if(pPBXDevice) {
		pCallData->setState(CallData::STATE_TRANSFERING);
		pCallData->setPendingCmdID(generate_NewCommandID());
		if(bIsConference)
		{
			DeviceData_Router *pDeviceData = find_Device(pCallData->getOwnerDevID());
			string room="0";
			room += pDeviceData->mapParameters_Find(DEVICEDATA_PhoneNumber_CONST);
			/*send transfer command to PBX*/
			CMD_PBX_Transfer cmd_PBX_Transfer(m_dwPK_Device, pPBXDevice->m_dwPK_Device, room, pCallData->getPendingCmdID(), pCallData->getID(),bIsConference);
			SendCommand(cmd_PBX_Transfer);
			Sleep(2000);
			DCE::CMD_PL_External_Originate cmd_invite(pCallData->getOwnerDevID(),m_dwPK_Device,sPhoneNumber,"pluto",room);
			SendCommand(cmd_invite);			
		}
		else
		{
			/*send transfer command to PBX*/
			CMD_PBX_Transfer cmd_PBX_Transfer(m_dwPK_Device, pPBXDevice->m_dwPK_Device, sPhoneNumber, pCallData->getPendingCmdID(), pCallData->getID(),bIsConference);
			SendCommand(cmd_PBX_Transfer);
		}
		
	}
}
	
//<-dceag-c236-b->

	/** @brief COMMAND: #236 - PL_Hangup */
	/** Hangs up a call */

void Telecom_Plugin::CMD_PL_Hangup(string &sCMD_Result,Message *pMessage)
//<-dceag-c236-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Hangup command called.");
	
	CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(pMessage->m_dwPK_Device_From);
	if(!pCallData) {
		g_pPlutoLogger->Write(LV_WARNING, "No calldata found for device %d",pMessage->m_dwPK_Device_From);
	
		pCallData = CallManager::getInstance()->findCallByOwnerDevID(map_orbiter2embedphone[pMessage->m_dwPK_Device_From]);
		if(!pCallData) {
			g_pPlutoLogger->Write(LV_WARNING, "No calldata found for device %d",map_orbiter2embedphone[pMessage->m_dwPK_Device_From]);
			return;
		}
	}
	g_pPlutoLogger->Write(LV_STATUS, "Will hangup on channelid %s", pCallData->getID().c_str());	
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

void Telecom_Plugin::GetFloorplanDeviceInfo(DeviceData_Router *pDeviceData_Router,EntertainArea *pEntertainArea,int iFloorplanObjectType,int &iPK_FloorplanObjectType_Color,int &Color,string &sDescription,string &OSD,int &PK_DesignObj_Toolbar)
{
}

//<-dceag-c414-b->

	/** @brief COMMAND: #414 - PL External Originate */
	/** Originate an external call */
		/** @param #75 PhoneNumber */
			/** Phone to call */
		/** @param #81 CallerID */
			/** Caller ID */
		/** @param #83 PhoneExtension */
			/** Phone extension to dial */

void Telecom_Plugin::CMD_PL_External_Originate(string sPhoneNumber,string sCallerID,string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c414-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Originate external command called with params: PhoneNumber=%s, PhoneExtension=%s!", sPhoneNumber.c_str(), sPhoneExtension.c_str());

    if(sPhoneExtension.empty()) {
        g_pPlutoLogger->Write(LV_CRITICAL, "Error validating input parameters");
        return;
    }

    string sSrcPhoneType = "Local";

    g_pPlutoLogger->Write(LV_STATUS, "Using source phone with parameters: PhoneChannel=%s, PhoneNumber=%s!", 
        sSrcPhoneType.c_str(), sPhoneNumber.c_str());

	sPhoneNumber+="@trusted";

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
            sPhoneNumber, sSrcPhoneType, sPhoneExtension, sCallerID, pCallData->getPendingCmdID());
        SendCommand(cmd_PBX_Originate);
    }
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



bool 
Telecom_Plugin::IncomingCall( class Socket *pSocket, class Message *pMessage, 
					 			class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo ) {
	SCREEN_DevIncomingCall SCREEN_DevIncomingCall_(m_dwPK_Device,pDeviceFrom->m_dwPK_Device_ControlledVia);
	SendCommand(SCREEN_DevIncomingCall_);								
	return true;
}
//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a DTMF code */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void Telecom_Plugin::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	int phoneID=map_orbiter2embedphone[pMessage->m_dwPK_Device_From];
	if(phoneID>0)
	{
		DCE::CMD_Simulate_Keypress cmd(m_dwPK_Device,phoneID,sPK_Button,sName);
		SendCommand(cmd);
	}
	sCMD_Result="OK";
}
//<-dceag-c334-b->

	/** @brief COMMAND: #334 - Phone_Initiate */
	/** Initiates a call */
		/** @param #83 PhoneExtension */
			/** Extention to dial */

void Telecom_Plugin::CMD_Phone_Initiate(string sPhoneExtension,string &sCMD_Result,Message *pMessage)
//<-dceag-c334-e->
{
	int phoneID=map_orbiter2embedphone[pMessage->m_dwPK_Device_From];
	if(phoneID>0)
	{
		DCE::CMD_Phone_Initiate cmd(m_dwPK_Device,phoneID,sPhoneExtension);
		SendCommand(cmd);
		CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(phoneID);
		if(!pCallData) {
			/*create new call data*/
			pCallData = new CallData();
			pCallData->setOwnerDevID(phoneID);
			CallManager::getInstance()->addCall(pCallData);
		}
		pCallData->setState(CallData::STATE_ORIGINATING);
	}
	
	sCMD_Result="OK";
}
//<-dceag-c335-b->

	/** @brief COMMAND: #335 - Phone_Answer */
	/** Answer a call */

void Telecom_Plugin::CMD_Phone_Answer(string &sCMD_Result,Message *pMessage)
//<-dceag-c335-e->
{
	int phoneID=map_orbiter2embedphone[pMessage->m_dwPK_Device_From];
	if(phoneID>0)
	{
		DCE::CMD_Phone_Answer cmd(m_dwPK_Device,phoneID);
		SendCommand(cmd);
		CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(phoneID);
		if(!pCallData) {
			/*create new call data*/
			pCallData = new CallData();
			pCallData->setOwnerDevID(phoneID);
			CallManager::getInstance()->addCall(pCallData);
			g_pPlutoLogger->Write(LV_WARNING, "Answering a call for which have no data");
		}
		pCallData->setState(CallData::STATE_OPENED);
	}
	sCMD_Result="OK";
}
//<-dceag-c336-b->

	/** @brief COMMAND: #336 - Phone_Drop */
	/** Drop a call */

void Telecom_Plugin::CMD_Phone_Drop(string &sCMD_Result,Message *pMessage)
//<-dceag-c336-e->
{
	int phoneID=map_orbiter2embedphone[pMessage->m_dwPK_Device_From];
	if(phoneID>0)
	{
		DCE::CMD_Phone_Drop cmd(m_dwPK_Device,phoneID);
		SendCommand(cmd);
		CallData *pCallData = CallManager::getInstance()->findCallByOwnerDevID(phoneID);
		if(pCallData) {
			CallManager::getInstance()->removeCall(pCallData);	
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Dropping a call for which have no data");
		}
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
	class Row_Users* rowuser;
	rowuser=m_pDatabase_pluto_main->Users_get()->GetRow(iPK_Users);
	rowuser->FK_UserMode_set(iPK_UserMode);
	m_pDatabase_pluto_main->Users_get()->Commit();
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
	
	cmdline+= string(" ")+sUsers+(" ")+sPassword+string(" ")+sName;
	system(cmdline.c_str());
}
