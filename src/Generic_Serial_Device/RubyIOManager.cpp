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
#include "RubyIOManager.h"
#include "RubyEmbeder.h"

#include "SerialIOPool.h"
#include "NetworkIOPool.h"

#include "DCE/DeviceData_Impl.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"

using namespace std;
using namespace EMBRUBY;

#define SPORTTYPE_SERIAL		"SER"
#define SPORTTYPE_NETWORK		"NET"

namespace DCE {

RubyIOManager* RubyIOManager::s_instance_ = NULL;

RubyIOManager::RubyIOManager()
	: pdb_(NULL), pevdisp_(NULL)
{
}


RubyIOManager::~RubyIOManager() {
}

RubyIOManager* 
RubyIOManager::getInstance() {
	if(!s_instance_) {
		static RubyIOManager instance;
		s_instance_ = &instance;
	}
	return s_instance_;
}

int 
RubyIOManager::addDevice(DeviceData_Impl* pdevdata) {
	g_pPlutoLogger->Write(LV_STATUS, "Adding child device: %d.", pdevdata->m_dwPK_Device);
	cs_.addCode(pdb_, pdevdata);
	
	string portdesc = pdevdata->m_mapParameters[DEVICEDATA_Port_CONST], port;
	if(portdesc.empty()) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Port parameter not specified.");
		return -1;
	}

	/*check the kind of device we instantiate, based on port string*/
	PORTTYPE porttype = PortType(portdesc, port);
	if(porttype == PORTTYPE_UNKNOWN) {
		g_pPlutoLogger->Write(LV_WARNING, "Could not determine port Type from port Parameter.");
		return -1;
	}
	
	RubyIOPool*& ppool = pools_[port];
	if(ppool != NULL) {
		g_pPlutoLogger->Write(LV_CRITICAL, "There is already a device configured for port: %s.", port.c_str());
		return -1;
	} 

	IOPool* pnewpool = NULL;	
	switch(porttype) {
		/*** Serial Device Initialization *********************************/
		case PORTTYPE_SERIAL: {
			string serport;
			int bps = 9600;
			
			if(port.find("/dev/") == 0) {
				port.erase(0, strlen("/dev/"));
			}
			
			/*/dev/ttyS0(9600)*/
			/*parse conn params*/
			int lpar = port.find("("), rpar = port.find(")", lpar + 1);
			if(lpar > 0 && rpar > 0) {
				bps = atoi(port.substr(lpar + 1, rpar - lpar - 1).c_str());
				serport = port.substr(0, lpar);
				g_pPlutoLogger->Write(LV_STATUS, "Using port %s, at bps: %d.", serport.c_str(), bps);
			} else {
				serport = port;
			}
		
			pnewpool = new SerialIOPool();
			SerialIOConnection* pio = reinterpret_cast<SerialIOConnection*>(pnewpool->getConnection());
			pio->setBPS(bps);
			pio->setSerialPort(serport.c_str());
			
			} break;
		
		/*** Network Device Initialization *********************************/
		case PORTTYPE_NETWORK: {
			string netaddr;
			int netport = -1;
			
			int lpar = port.find("("), rpar = port.find(")", lpar + 1);
			if(lpar > 0 && rpar > 0) {
				netport = atoi(port.substr(lpar + 1, rpar - lpar - 1).c_str());
				netaddr = port.substr(0, lpar);
				g_pPlutoLogger->Write(LV_STATUS, "Using network device with address %s, at port: %d.", netaddr.c_str(), netport);
			} else {
				g_pPlutoLogger->Write(LV_WARNING, "Error in format of the NET port.");
				return -1;
			}
			
			pnewpool = new NetworkIOPool();
			NetworkIOConnection* pio = reinterpret_cast<NetworkIOConnection*>(pnewpool->getConnection());
			pio->setHost(netaddr.c_str());
			pio->setPort(netport);
			
			} break;
		
		default:
			/*we checked for unknown before, so we cannot get here*/
			return -1;
	}
	
	ppool = new RubyIOPool(pnewpool);
	ppool->setDeviceData(pdevdata);
	
	g_pPlutoLogger->Write(LV_STATUS, "Child device %d added.", pdevdata->m_dwPK_Device);
	return 0;
}

int 
RubyIOManager::removeDevice(DeviceData_Impl* pdevdata) {
	return -1;
}

bool 
RubyIOManager::hasDevice(DeviceData_Base* pdevdata) {
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

RubyIOManager::PORTTYPE 
RubyIOManager::PortType(const std::string& portdesc, std::string& port) {
	if(portdesc.length() < 4) {
		return PORTTYPE_UNKNOWN;
	}
	
	string porttype = portdesc.substr(0, 3);
	if(porttype == SPORTTYPE_SERIAL) {
		port = portdesc.substr(4, portdesc.length() - 4);
		return PORTTYPE_SERIAL;
	} else
	if(porttype == SPORTTYPE_NETWORK) {
		port = portdesc.substr(4, portdesc.length() - 4);
		return PORTTYPE_NETWORK;
	}
	return PORTTYPE_UNKNOWN;
}

	
int 
RubyIOManager::RouteMessageToDevice(DeviceData_Base* pdevdata, Message *pMessage) {
	g_pPlutoLogger->Write(LV_STATUS, "Pushing message to stack..."); 
	mmsg_.Lock();
	std::pair<unsigned, Message> msg(pdevdata->m_dwPK_Device, *pMessage);
	msgqueue_.push_back(msg);
	mmsg_.Unlock();
	emsg_.Signal();
	return -1;
}

void* 
RubyIOManager::_Run() {
	while(!isStopRequested()) {
		bool msgarrived = emsg_.Wait(1000);
		mmsg_.Lock();
		if(!msgarrived && msgqueue_.size() == 0) {
			mmsg_.Unlock();
			
			POOLMAP::iterator it = pools_.begin();
			while(it != pools_.end()) {
				if((*it).second != NULL) {
					(*it).second->handleIteration();
				}
				it++;
			}
		} else {
			mmsg_.Unlock();
			
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

bool 
RubyIOManager::DispatchMessageToDevice(Message *pmsg, unsigned deviceid) {
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
RubyIOManager::SendCommand(RubyCommandWrapper* pcmd) {
	g_pPlutoLogger->Write(LV_STATUS, "Ruby code requested to send command %d, from %d to %d... Sending...", 
								pcmd->getId(), pcmd->getDevIdFrom(), pcmd->getDevIdTo());
	Message* pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getDevIdTo(), pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	pmsg->m_mapParameters = pcmd->getParams();
	pevdisp_->SendMessage(pmsg);
	g_pPlutoLogger->Write(LV_STATUS, "Command sent.");
}

bool 
RubyIOManager::handleStartup() {
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
			(*it).second->Init(&cs_, pdb_);
			(*it).second->handleStartup();
		}
		it++;
	}
	return true;
}

void
RubyIOManager::handleTerminate() {
	/*signal pools stop*/
	POOLMAP::iterator it = pools_.begin();
	while(it != pools_.end()) {
		if((*it).second != NULL) {
			(*it).second->handleTerminate();
			delete (*it).second;
		}
		it++;
	}
}

};
