//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "RubySerialIOManager.h"
#include "RubyEmbeder.h"

#include "DCE/DeviceData_Impl.h"
#include "DCE/Logger.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"

using namespace std;
using namespace EMBRUBY;

namespace DCE {

RubySerialIOManager::RubySerialIOManager()
	: pdb_(NULL), pevdisp_(NULL)
{
}


RubySerialIOManager::~RubySerialIOManager() {
	WaitDevices();
}

int 
RubySerialIOManager::addDevice(DeviceData_Impl* pdevdata) {
	g_pPlutoLogger->Write(LV_STATUS, "Adding child device: %d.", pdevdata->m_dwPK_Device);
	cs_.addCode(pdb_, pdevdata);
	
	string port = pdevdata->m_mapParameters[DEVICEDATA_Port_CONST];
	if(port.find("/dev/") == 0) {
		port.erase(0, strlen("/dev/"));
	}
	
	if(port.empty()) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Port parameter not specified.");
		return -1;
	}
	
	RubySerialIOPool*& pserpool = pools_[port];
	if(pserpool != NULL) {
		g_pPlutoLogger->Write(LV_CRITICAL, "There is already a device configured for port: %s.", port.c_str());
		return -1;
	}
/*	
	RubyDeviceWrapper devwrap;
	devwrap.setDevId(pdevdata->m_dwPK_Device);
	std::map<int, string>::iterator it = pdevdata->m_mapParameters.begin();
	while(it != pdevdata->m_mapParameters.end()) {
		Row_DeviceData *p_Row_DeviceData = pdb->DeviceData_get()->GetRow((*it).first);
		devwrap.setData(p_Row_DeviceData->Description_get().c_str(), (*it).second.c_str());
		it++;
	}
	*/	
	pserpool = new RubySerialIOPool(&cs_, pdb_, pdevdata);
	pserpool->setDCEConnector(this);
	
	g_pPlutoLogger->Write(LV_STATUS, "Child device %d added.", pdevdata->m_dwPK_Device);
	return 0;
}

int 
RubySerialIOManager::removeDevice(DeviceData_Impl* pdevdata) {
	return -1;
}

bool 
RubySerialIOManager::hasDevice(DeviceData_Base* pdevdata) {
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL && (*it).second->getDeviceData() && 
				(*it).second->getDeviceData()->m_dwPK_Device == pdevdata->m_dwPK_Device) {
			return true;
		}
		it++;
	}
	return false;
}
	
int 
RubySerialIOManager::RouteMessageToDevice(DeviceData_Base* pdevdata, Message *pMessage) {
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL && (*it).second->getDeviceData() && 
				(*it).second->getDeviceData()->m_dwPK_Device == pdevdata->m_dwPK_Device) {
			return (*it).second->RouteMessage(pMessage);
		}
		it++;
	}
	return -1;
}

void 
RubySerialIOManager::RunDevices() {
	try {
		g_pPlutoLogger->Write(LV_STATUS, "Loading generated code...");
		RubyEmbeder::getInstance()->loadCode(&cs_);
		g_pPlutoLogger->Write(LV_STATUS, "Generated code loaded.");
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed loading code: %s.", e.getMessage());
		return;
	}

	/*start serial pools*/
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL) {
			(*it).second->Run(false);
		}
		it++;
	}
}

void 
RubySerialIOManager::WaitDevices() {
	/*signal pools stop*/
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL) {
			(*it).second->SignalStop();
		}
		it++;
	}
	
	/*wait*/
	it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL) {
			(*it).second->Wait();
			delete (*it).second;
		}
		it++;
	}
}

void 
RubySerialIOManager::SendCommand(RubyCommandWrapper* pcmd) {
	g_pPlutoLogger->Write(LV_STATUS, "Ruby code requested to send command %d, from %d to %d... Sending...", 
								pcmd->getId(), pcmd->getDevIdFrom(), pcmd->getDevIdTo());
	Message* pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getDevIdTo(), pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	pmsg->m_mapParameters = pcmd->getParams();
	pevdisp_->SendMessage(pmsg);
	g_pPlutoLogger->Write(LV_STATUS, "Command sent.");
}

};
