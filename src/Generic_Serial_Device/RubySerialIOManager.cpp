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
#include "DCE/Message.h"
#include "pluto_main/Define_Command.h"
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
}

int 
RubySerialIOManager::addDevice(DeviceData_Impl* pdevdata) {
	g_pPlutoLogger->Write(LV_STATUS, "Adding child device: %d.", pdevdata->m_dwPK_Device);
	cs_.addCode(pdb_, pdevdata);
	
	string port = pdevdata->m_mapParameters[DEVICEDATA_Port_CONST];
	int bps = 9600;
	
	if(port.find("/dev/") == 0) {
		port.erase(0, strlen("/dev/"));
	}
	
	if(port.empty()) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Port parameter not specified.");
		return -1;
	}

	/*/dev/ttyS0(9600)*/
	/*parse conn params*/
	int lpar = port.find("("), rpar = port.find(")", lpar + 1);
	if(lpar > 0 && rpar > 0) {
		bps = atoi(port.substr(lpar + 1, rpar - lpar - 1).c_str());
		port = port.substr(0, lpar);
		g_pPlutoLogger->Write(LV_STATUS, "Using port %s, at bps: %d.", port.c_str(), bps);
	}

	RubySerialIOPool*& pserpool = pools_[port];
	if(pserpool != NULL) {
		g_pPlutoLogger->Write(LV_CRITICAL, "There is already a device configured for port: %s.", port.c_str());
		return -1;
	} 
	
	pserpool = new RubySerialIOPool(&cs_, pdb_, pdevdata);
	pserpool->setDCEConnector(this);
	
	SerialIOConnection* pio = reinterpret_cast<SerialIOConnection*>(pserpool->getConnection());
	pio->setBPS(bps);
	pio->setSerialPort(port.c_str());
	
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
	g_pPlutoLogger->Write(LV_STATUS, "Pushing message to stack..."); 
	mmsg_.Lock();
	std::pair<unsigned, Message> msg(pdevdata->m_dwPK_Device, *pMessage);
	msgqueue_.push_back(msg);
	mmsg_.Unlock();
	emsg_.Signal();
	return -1;
}

void* 
RubySerialIOManager::_Run() {
	while(!isStopRequested()) {
		if(!emsg_.Wait(500)) {
			ProcessIdle();
		} else {
			Message *pmsg = NULL;
			unsigned deviceid = 0;
	
			while(1) {		
				mmsg_.Lock();
				if(msgqueue_.size() > 0) {
					deviceid = (*msgqueue_.begin()).first;
					pmsg = new Message((*msgqueue_.begin()).second);
					msgqueue_.pop_front();
				}
				mmsg_.Unlock();
				
				if(pmsg != NULL) {
					DispatchMessageToDevice(pmsg, deviceid);
					delete pmsg;
					pmsg = NULL;
				} else {
					break;
				}
			}
		}
	}
	return 0;
}

void 
RubySerialIOManager::ProcessIdle() {
	static Message 	
				idlemsg(0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Process_IDLE_CONST, 0),
				datamsg(0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Incoming_Data_CONST, 0);
	
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL) {
			IOConnection* pconn = (*it).second->getConnection();
			if(pconn != NULL && pconn->isDataAvailable()) {
				(*it).second->RouteMessage(&datamsg);
			} else {
				(*it).second->RouteMessage(&idlemsg);
			}
		}
		it++;
	}
}

bool 
RubySerialIOManager::DispatchMessageToDevice(Message *pmsg, unsigned deviceid) {
	bool bret = false;
	
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL && (*it).second->getDeviceData() && 
				(!deviceid || (*it).second->getDeviceData()->m_dwPK_Device == deviceid)) {
			g_pPlutoLogger->Write(LV_STATUS, "Routing message to Ruby Interpreter...");
			(*it).second->RouteMessage(pmsg);
			bret = true;
		}
		it++;
	}
	return bret;
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

bool 
RubySerialIOManager::handleStartup() {
	try {
		g_pPlutoLogger->Write(LV_STATUS, "Loading generated code...");
		RubyEmbeder::getInstance()->loadCode(&cs_);
		g_pPlutoLogger->Write(LV_STATUS, "Generated code loaded.");
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed loading code: %s.", e.getMessage());
		return false;
	}

	/*start serial pools*/
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL) {
			(*it).second->Run(false);
		}
		it++;
	}
	return true;
}

void
RubySerialIOManager::handleTerminate() {
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

};
