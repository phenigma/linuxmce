//<-dceag-d-b->
#include "Generic_Serial_Device.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "DCE/DCEConfig.h"
#include "pluto_main/Database_pluto_main.h"

#include "RubyIOManager.h"

#define GSD_COMMAND_LINE	"Generic_Serial_Device"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Generic_Serial_Device::Generic_Serial_Device(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Generic_Serial_Device_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	DCEConfig dceconf;

	m_pdbPlutoMain = new Database_pluto_main();
	if(!m_pdbPlutoMain->Connect(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword,
									dceconf.m_sDBName,dceconf.m_iDBPort) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		return;
	}
	g_pPlutoLogger->Write(LV_STATUS, "Successfully connected to database!");
	
	RubyIOManager* pmanager = RubyIOManager::getInstance();
	pmanager->setDatabase(m_pdbPlutoMain);
	pmanager->setEventDispatcher(GetEvents());
	
	Generic_Serial_Device_MessageProcessor::setCommandImpl(this);
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Generic_Serial_Device::~Generic_Serial_Device()
//<-dceag-dest-e->
{
	delete m_pdbPlutoMain;
}

bool Generic_Serial_Device::Connect(int iPK_DeviceTemplate )
{
	if( !Generic_Serial_Device_Command::Connect(iPK_DeviceTemplate) )
		return false;
	return true;
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
void Generic_Serial_Device::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
/*	
	// find descendant device
    DeviceData_Impl* pDeviceData_Impl = RecursiveFindChildDevice(pMessage->m_dwPK_Device_To, m_pData);
    if(!pDeviceData_Impl) {
        g_pPlutoLogger->Write(LV_CRITICAL, "Child device %d not found.", pMessage->m_dwPK_Device_To);
        return;
    }
*/	
	/*We cannot route message to nephew devices or younger. 
		Must route them to child.*/

	if(Generic_Serial_Device_MessageProcessor::ProcessMessage(pMessage)) {
	    g_pPlutoLogger->Write(LV_STATUS, "Message processed by Translator.");
		return;
	}

	DispatchMessage(pMessage);
		
	/*find child*/
	/*
	while(pDeviceData_Base != NULL && !sermanager_.hasDevice(pDeviceData_Base)) {
		pDeviceData_Base =
			m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pDeviceData_Base->m_dwPK_Device_ControlledVia);
	}
	
	if(pDeviceData_Base == NULL) {
	    g_pPlutoLogger->Write(LV_CRITICAL, "Error in parsing Controlled Via hierarchy.");
		return;
	}
	
    g_pPlutoLogger->Write(LV_STATUS, "Routing message to child device %d.", pDeviceData_Base->m_dwPK_Device);
	sermanager_.RouteMessageToDevice(pDeviceData_Base, pMessage);
	*/
}

/*
DeviceData_Impl* Generic_Serial_Device::RecursiveFindChildDevice(unsigned dwPK_Device, DeviceData_Impl* pDeviceData_Impl) 
{	
	if(pDeviceData_Impl->m_dwPK_Device == dwPK_Device) {
		return pDeviceData_Impl;
	} else {
		VectDeviceData_Impl& vDeviceData = pDeviceData_Impl->m_vectDeviceData_Impl_Children;
		for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
			pDeviceData_Impl = RecursiveFindChildDevice(dwPK_Device, vDeviceData[i]);
			if(pDeviceData_Impl != NULL) {
				return pDeviceData_Impl;
			}
		}
	}

	return NULL;
}
*/

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Generic_Serial_Device::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
    g_pPlutoLogger->Write(LV_STATUS, "Received UNKNOWN command.");
	
	if(Generic_Serial_Device_MessageProcessor::ProcessMessage(pMessage)) {
	    g_pPlutoLogger->Write(LV_STATUS, "Message processed by Translator.");
		return;
	}
	
	DispatchMessage(pMessage);
/*	
	if(sermanager_.hasDevice(m_pData)) {
		sermanager_.RouteMessageToDevice(m_pData, pMessage);
	} else {
	    g_pPlutoLogger->Write(LV_WARNING, "Received command for me but I am not registered as Generic Serial Device.");
	}
*/	
}

//<-dceag-sample-b->!
//<-dceag-reg-b->!
bool Generic_Serial_Device::Register(){return false;}
//<-dceag-createinst-b->!

/*

	COMMANDS TO IMPLEMENT

*/

// Intercept all commands
/*
bool Generic_Serial_Device::ReceivedMessage(class Message *pMessageOriginal)
{
	map<int,string>::iterator itCodeMap;
	int iHandled=0;

	for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
	{
		Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
		if( pMessage->m_dwMessage_Type != MESSAGETYPE_COMMAND || (itCodeMap=m_CodeMap.find(pMessage->m_dwID))==m_CodeMap.end() )
			iHandled += Generic_Serial_Device_Command::ReceivedMessage(pMessage);  // If it's not a command or we don't have an entry for it, let the base class handle it
		else
		{
//			TransmitCommand((*itCodeMap).second,pMessage);
		}
	}
	return iHandled!=0;
}
*/
void Generic_Serial_Device::Transmit(const char *pData,int iSize)
{
}

void Generic_Serial_Device::DispatchMessage(Message* pmsg) {
	g_pPlutoLogger->Write(LV_STATUS, "Dispatching Message %d...", pmsg->m_dwID);

	RubyIOManager* pmanager = RubyIOManager::getInstance();
	
	/*find child*/
	DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pmsg->m_dwPK_Device_To);
	while(pDeviceData_Base != NULL && !pmanager->hasDevice(pDeviceData_Base)) {
		pDeviceData_Base =
			m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pDeviceData_Base->m_dwPK_Device_ControlledVia);
	}
	
	if(pDeviceData_Base == NULL) {
	    g_pPlutoLogger->Write(LV_CRITICAL, "Error in parsing Controlled Via hierarchy.");
		return;
	}
	
    g_pPlutoLogger->Write(LV_STATUS, "Routing message to child device %d.", pDeviceData_Base->m_dwPK_Device);
	pmanager->RouteMessageToDevice(pDeviceData_Base, pmsg);
}

void Generic_Serial_Device::RunThread() {
	/* register serial devices
		1. method if device has Generic_Serial_Device specified as command line, then we are standalone driver, controlling a single device
	 	2. if no command line is specfified, we control child devices (performance hit, as we run only in single thread)
	*/
	RubyIOManager* pmanager = RubyIOManager::getInstance();
    
	g_pPlutoLogger->Write(LV_STATUS, "Device %d has commad line %s.", m_dwPK_Device, m_pData->m_sCommandLine.c_str());
	if(m_pData->m_sCommandLine == GSD_COMMAND_LINE) {
		/*we are in case 1*/
		pmanager->addDevice(m_pData);
	} else {
		/*we are in case 2*/
		VectDeviceData_Impl& vDeviceData = m_pData->m_vectDeviceData_Impl_Children;
		for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
			pmanager->addDevice(vDeviceData[i]);
		}
	}
	
	pmanager->Run(false);
	Generic_Serial_Device_Command::RunThread();
	pmanager->Wait(true);
    g_pPlutoLogger->Write(LV_STATUS, "Generic Serial Device RunThread ended.");
}
