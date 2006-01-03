/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyIOManager.h"
#include "RubyEmbeder.h"

#include "SerialIOPool.h"
#include "NetworkIOPool.h"
#include "PlutoUtils/LinuxSerialUSB.h"
#include "DCE/DeviceData_Impl.h"
#include "DCE/Command_Impl.h"
#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/Command_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"
#include <signal.h>

using namespace std;
using namespace EMBRUBY;

#define DEFAULT_POOL_TIME		500
#define DEFAULT_DELAY_TIME		2000
#define DEFAULT_RESPONSE_TIME	20

namespace DCE {

RubyIOManager* RubyIOManager::s_instance_ = NULL;

RubyIOManager::RubyIOManager()
	: m_MsgMutex("message mutex",true), pevdisp_(NULL), rootnode_(NULL)
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

RubyDCEDeviceNode* 
RubyIOManager::InstantiateNode(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata) {
	RubyDCEDeviceNode* pNode = NULL;
	pcmdimpl_=pcmdimpl;
	string sport = pcmdimpl->m_pEvent->GetDeviceDataFromDatabase(pcmdimpl->m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST);
	g_pPlutoLogger->Write(LV_STATUS,"RubyIOManager::InstantiateNode port: %s",sport.c_str());
	PORTTYPE porttype = PORTTYPE_UNKNOWN;
	if(!sport.empty() && sport != "0" && StringUtils::StartsWith(sport,"ERR")==false ) {
		porttype = PORTTYPE_SERIAL;
	} else {
		sport = pdevdata->m_mapParameters[DEVICEDATA_TCP_Port_CONST];
		g_pPlutoLogger->Write(LV_STATUS, "Checking TCP Port %s",sport.c_str());
		if(!sport.empty()) {
			porttype = PORTTYPE_NETWORK;
		} else {
			g_pPlutoLogger->Write(LV_CRITICAL, "GSD Port is not specified. Instantiating non-IO Wrapper.");
		}
	}

	if(porttype == PORTTYPE_UNKNOWN) {
		pNode = new RubyDCEDeviceNode();
		cs_.addCode(pcmdimpl, pdevdata, false);
		
	} else {
		cs_.addCode(pcmdimpl, pdevdata, true);
	    IOPool* pnewpool = NULL;
    	switch(porttype) {
        	/*** Serial Device Initialization *********************************/
    	    case PORTTYPE_SERIAL: {
	            int bps = 9600;

        	    string serport = pcmdimpl->m_pEvent->GetDeviceDataFromDatabase(pcmdimpl->m_dwPK_Device,DEVICEDATA_COM_Port_on_PC_CONST);
				serport = TranslateSerialUSB(serport);
            	if(serport.find("/dev/") == 0) {
	                serport.erase(0, strlen("/dev/"));
    	        }

        	    string sbaudrate = pdevdata->m_mapParameters[DEVICEDATA_COM_Port_BaudRate_CONST];
            	if(!sbaudrate.empty() && sbaudrate[0] == 'B') {
	                int tbps = atoi(sbaudrate.substr(1, sbaudrate.length() -1).c_str());
    	            if(tbps != 0) {
        	            bps = tbps;
            	    }
	            } else {
    	            g_pPlutoLogger->Write(LV_STATUS, "Ba>=udRate not specified or invalid, using default: %d.", bps);
        	    }

	            g_pPlutoLogger->Write(LV_STATUS, "Using port %s, at bps: %d.", serport.c_str(), bps);

    	        pnewpool = new SerialIOPool();
        	    SerialIOConnection* pio = reinterpret_cast<SerialIOConnection*>(pnewpool->getConnection());
            	pio->setBPS(bps);
	            pio->setSerialPort(serport.c_str());

	            string sparbitstop = pdevdata->m_mapParameters[DEVICEDATA_COM_Port_ParityBitStop_CONST];
    	        if(!sparbitstop.empty()) {
        	        g_pPlutoLogger->Write(LV_STATUS, "Using paritybit/stopbit: %s.", sparbitstop.c_str());
                	if(sparbitstop == "N81") {
            	        pio->setParityBitStop(epbsN81);
	                } else
    	            if(sparbitstop == "E81") {
        	            pio->setParityBitStop(epbsE81);
            	    } else
                	if(sparbitstop == "O81") {
	                    pio->setParityBitStop(epbsO81);
        	        } else {
    	                /*more to be implemented*/
            	        g_pPlutoLogger->Write(LV_CRITICAL, "Paritybit/Stopbit %s NOT supported.", sparbitstop.c_str());
                	}
	            }
    	        } break;
	       /*** Network Device Initialization *********************************/
    	    case PORTTYPE_NETWORK: {
        	    string netaddr;
	            int netport = -1;

	            int lpar = sport.find("("), rpar = sport.find(")", lpar + 1);
    	        if(lpar > 0 && rpar > 0) {
        	        netport = atoi(sport.substr(lpar + 1, rpar - lpar - 1).c_str());
            	    netaddr = sport.substr(0, lpar);
	            } else {
    	            /*get device ip addr*/
        	        netport = atoi(sport.c_str());
            	    netaddr = pdevdata->GetIPAddress();
	            }

    	        if(!netaddr.empty()) {
        	        g_pPlutoLogger->Write(LV_STATUS, "Using network device with address <%s>, at port: <%d>.", netaddr.c_str(), netport);
            	} else {
    	            g_pPlutoLogger->Write(LV_CRITICAL, "Could not determine network address for device %d.", pdevdata->m_dwPK_Device);
	                return NULL;
        	    }

	            pnewpool = new NetworkIOPool();
    	        NetworkIOConnection* pio = reinterpret_cast<NetworkIOConnection*>(pnewpool->getConnection());
        	    pio->setHost(netaddr.c_str());
            	pio->setPort(netport);

	            } break;

    	    default:
        	    /*we checked for unknown before, so we cannot get here*/
	            return NULL;
	    }

		pNode = new RubyIOPool(pnewpool);
	}

	int delay = atoi(pdevdata->m_mapParameters[DEVICEDATA_Idle_Delay_CONST].c_str());
	if(delay == 0) {
		delay = DEFAULT_DELAY_TIME;
	}
	g_pPlutoLogger->Write(LV_STATUS, "Using Idle Delay: %d.", delay);
	pNode->setIdleDelay(delay);
    pNode->setDeviceData(pdevdata);
	return pNode;
}

int 
RubyIOManager::addDevice(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata) {
	g_pPlutoLogger->Write(LV_STATUS, "Adding device: %d.", pdevdata->m_dwPK_Device);

	/*parse Code nodes and insert node*/
	RubyDCEDeviceNode* pNewNode = addDevice(pcmdimpl, pdevdata, rootnode_);
	if(!pNewNode) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Could not add new Device Node.");
		return -1;
	}

	if(!rootnode_) {
		rootnode_ = pNewNode;
	}
	
	return 0;
}

RubyDCEDeviceNode* 
RubyIOManager::addDevice(Command_Impl* pcmdimpl, DeviceData_Impl* pdevdata, RubyDCEDeviceNode* pNode) {
	if(pNode == NULL) {
		return InstantiateNode(pcmdimpl, pdevdata);
	} 
	RubyDCEDeviceNode* pNewNode = NULL;
	if(pdevdata->m_dwPK_Device_ControlledVia == pNode->getDeviceData()->m_dwPK_Device) {
		pNewNode = InstantiateNode(pcmdimpl, pdevdata);
		if(pNewNode) {
			pNode->addChild(pNewNode);
		}
	} else {
		std::list<RubyDCEDeviceNode*>& children = pNode->getChildren();
		for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
			pNewNode = addDevice(pcmdimpl, pdevdata, *it);
			if(pNewNode) {
				break;
			}
		}
	}
	return pNewNode;
}

int 
RubyIOManager::removeDevice(DeviceData_Impl* pdevdata) {
	/*NOT Implemented*/
	return -1;
}
bool 
RubyIOManager::hasDevice(DeviceData_Base* pdevdata) {
	return hasDevice(pdevdata, rootnode_);
}

bool 
RubyIOManager::hasDevice(DeviceData_Base* pdevdata, RubyDCEDeviceNode* pNode) {
	if(!pNode) {
		return false;
	}

	if(pNode->getDeviceData()->m_dwPK_Device == pdevdata->m_dwPK_Device) {
		return true;
	}

	std::list<RubyDCEDeviceNode*>& children = pNode->getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		if(hasDevice(pdevdata, *it)) {
			return true;
		}
    }
	return false;
}

int 
RubyIOManager::RouteMessage(DeviceData_Base* pdevdata, Message *pMessage) {
	g_pPlutoLogger->Write(LV_STATUS, "Dispatching Message %d to %d...", pMessage->m_dwID, pMessage->m_dwPK_Device_To);

	RubyIOManager* pmanager = RubyIOManager::getInstance();
	int routeToDevice = 0;
	/*find device*/
	if(!pdevdata)
	{
	    g_pPlutoLogger->Write(LV_WARNING, "Running without DeviceData");
		routeToDevice = pMessage->m_dwPK_Device_To;
	}
	else
	{
		DeviceData_Base *ptmpdevdata = pdevdata->m_AllDevices.m_mapDeviceData_Base_Find(pMessage->m_dwPK_Device_To);
		while(ptmpdevdata != NULL && !pmanager->hasDevice(ptmpdevdata)) {
			pdevdata = ptmpdevdata;
			ptmpdevdata =
				pdevdata->m_AllDevices.m_mapDeviceData_Base_Find(ptmpdevdata->m_dwPK_Device_ControlledVia);
		}
		if(ptmpdevdata == NULL) {
		    g_pPlutoLogger->Write(LV_STATUS, "Command will be handled in Parent device.");
			ptmpdevdata = pdevdata;
		}
		routeToDevice = ptmpdevdata->m_dwPK_Device;
	}
		
    g_pPlutoLogger->Write(LV_STATUS, "Routing message to device %d.", routeToDevice);
	mmsg_.Lock();
	std::pair<unsigned, Message*> msg(routeToDevice, pMessage);
	msgqueue_.push_back(msg);
	mmsg_.Unlock();
	emsg_.Signal();
	time_t tTimeout=time(NULL)+DEFAULT_RESPONSE_TIME;
	while( tTimeout>time(NULL) )
	{
		PLUTO_SAFETY_LOCK(mm,m_MsgMutex);
		if( pMessage->m_bRespondedToMessage )
			return 0;
		mm.Release();
		Sleep(100);
	}
	PLUTO_SAFETY_LOCK(mm,m_MsgMutex);
	if( !pMessage->m_bRespondedToMessage )
	{
		pMessage->m_bRespondedToMessage=true;
		g_pPlutoLogger->Write(LV_CRITICAL,"Ruby was unable to handle the command in reasonable amount of time");
	}
	return 0;
}

void handler_for_bad_things(int signal)
{
	g_pPlutoLogger->Write(LV_CRITICAL, "A CRITICAL ERROR HAS OCCURED (CHECK YOUR RUBY CODE)");
	exit(1);
}

void* 
RubyIOManager::_Run() {
	signal(SIGABRT,handler_for_bad_things);
	while(!isStopRequested()) {
		bool msgarrived = emsg_.Wait(DEFAULT_POOL_TIME);
		mmsg_.Lock();
		if(!msgarrived && msgqueue_.size() == 0) {
			mmsg_.Unlock();
			if(rootnode_) {
				rootnode_->handleNoMessage();
			}
			/*	
			POOLMAP::iterator it = pools_.begin();
			while(it != pools_.end()) {
				if((*it).second != NULL) {
					(*it).second->handleIteration();
				}
				it++;
			}
			*/
		} else {
			mmsg_.Unlock();
			
			Message *pmsg = NULL;
			unsigned deviceid = 0;
	
			while(1) {		
				mmsg_.Lock();
				if(msgqueue_.size() > 0) {
					deviceid = (*msgqueue_.begin()).first;
					pmsg = (*msgqueue_.begin()).second;
					msgqueue_.pop_front();
				}
				mmsg_.Unlock();
				
				if(pmsg != NULL) {
					g_pPlutoLogger->Write(LV_STATUS, "Routing message to Ruby Interpreter...");
					if (!rootnode_->handleMessage(pmsg))
					{
						g_pPlutoLogger->Write(LV_CRITICAL, "For obscure reasons could not handle the message");
						pmsg->m_bRespondedToMessage = true;
					}
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
RubyIOManager::SendCommand(RubyCommandWrapper* pcmd) {
	g_pPlutoLogger->Write(LV_STATUS, "Ruby code requested to send command %d, from %d to %d... Sending...", 
								pcmd->getId(), pcmd->getDevIdFrom(), pcmd->getDevIdTo());
	Message* pmsg = new Message(pcmd->getDevIdFrom(), pcmd->getDevIdTo(), pcmd->getPriority(), pcmd->getType(), pcmd->getId(), 0);
	pmsg->m_mapParameters = pcmd->getParams();
	pevdisp_->SendMessage(pmsg);
	g_pPlutoLogger->Write(LV_STATUS, "Command sent.");
}

void 
RubyIOManager::SendMessage(Message* pmsg) {
	g_pPlutoLogger->Write(LV_STATUS, "Ruby code sending message...");
	if(!pcmdimpl_->SendMessage(pmsg))
		g_pPlutoLogger->Write(LV_WARNING, "Failed to send message.");
	else
		g_pPlutoLogger->Write(LV_STATUS, "Message was sent.");
}

void 
RubyIOManager::SendString(string str) {
	g_pPlutoLogger->Write(LV_STATUS, "Ruby code sending string...");
	if(!pcmdimpl_->SendString(str))
		g_pPlutoLogger->Write(LV_WARNING, "Failed to send string.");
	else
		g_pPlutoLogger->Write(LV_STATUS, "String was sent.");
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
	if(rootnode_) {
		rootnode_->Init(&cs_);
		rootnode_->handleStartup();
		rootnode_->handleNoMessage(); // added one iteration processing in order for idle to be called first	
	}
	return true;
}

void
RubyIOManager::handleTerminate() {
	if(rootnode_) {
		rootnode_->handleTerminate();
	}
}

};
