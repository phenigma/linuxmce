/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "RubyIOPool.h"
#include "RubyIOManager.h"

#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "PlutoUtils/FileUtils.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Table_DeviceData.h"

#include "RubyEmbeder.h"
#include "RubySerialIOWrapper.h"

#include "TestRubyEmbederCodeSupplier.h"
#include "TestEmbededClass.h"

using namespace EMBRUBY;
using namespace std;

namespace DCE {

RubyIOPool::RubyIOPool(IOPool* ppool, bool relpool) 
	: defstate_(this), ppool_(ppool), relpool_(relpool)
{
	setState(&defstate_);
}

RubyIOPool::~RubyIOPool() {
	Cleanup();
	if(relpool_) {
		delete ppool_;
	}
}

bool 
RubyIOPool::Init(RubyDCECodeSupplier* pcs, Database_pluto_main* pdb) {
	try {
		if(!pdevdata_) {
			g_pPlutoLogger->Write(LV_WARNING, "Serial IO pool was not initialized with device Data.");
			return false;
		}
		
		pembclass_ = new RubyDCEEmbededClass(pcs, pdevdata_->m_dwPK_Device);
		VALUE value = pembclass_->getValue();
		
		void* data = DATA_PTR(value);
		RubySerialIOWrapper* pWrapper = reinterpret_cast<RubySerialIOWrapper*>(data);
		if(pWrapper == NULL) {
			g_pPlutoLogger->Write(LV_WARNING, "Error instantiating Ruby serial WRAPPER.");
			return false;
		};
		
		/*init connection*/
		if(getConnection() == NULL) {
			g_pPlutoLogger->Write(LV_CRITICAL, "No connection in Serial IO pool.");
			return false;
		}
		
		pWrapper->setConn(RubySerialIOConnectionWrapper(getConnection()));
		pWrapper->setDCEConnector(RubyIOManager::getInstance());
		
		PopulateDevice(pdevdata_, pdb, pWrapper->getDevice());
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed instantiating class: %s.", e.getMessage());
	}
	return true;
}

void 
RubyIOPool::Cleanup() {
	g_pPlutoLogger->Write(LV_STATUS, "Serial IO Pool for device %d terminated.", pdevdata_->m_dwPK_Device);
	delete pembclass_;
}

void 
RubyIOPool::PopulateDevice(DeviceData_Impl* pdevdata, Database_pluto_main* pdb, RubyDeviceWrapper& devwrap) {
	devwrap.setDevId(pdevdata->m_dwPK_Device);

	int numparams = 0;	
	std::map<int, string>::iterator it = pdevdata->m_mapParameters.begin();
	while(it != pdevdata->m_mapParameters.end()) {
		Row_DeviceData *p_Row_DeviceData = pdb->DeviceData_get()->GetRow((*it).first);
		devwrap.setData(FileUtils::ValidCPPName(p_Row_DeviceData->Description_get()).c_str(), (*it).second.c_str());
		it++; numparams++;
	}
	g_pPlutoLogger->Write(LV_STATUS, "Added %d data params to device %d.", numparams, pdevdata->m_dwPK_Device);
		
	std::map<int, RubyDeviceWrapper>& childdevices = devwrap.getChildDevices();
    VectDeviceData_Impl& vDeviceData = pdevdata->m_vectDeviceData_Impl_Children;
    for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		RubyDeviceWrapper& childdevwrap = childdevices[vDeviceData[i]->m_dwPK_Device];
		PopulateDevice(vDeviceData[i], pdb, childdevwrap);
    }
	g_pPlutoLogger->Write(LV_STATUS, "Added %d wrapped child devices to device %d.", vDeviceData.size(), pdevdata->m_dwPK_Device);
}

int 
RubyIOPool::RouteMessage(Message* pMessage) {
	if(!pembclass_) {
		return -1;
	}
	
	pembclass_->CallCmdHandler(pMessage);
	return 0;
}

bool 
RubyIOPool::handleStartup() {
	ppool_->handleStartup();
	return IOPool::handleStartup();	
}

bool 
RubyIOPool::handleIteration() {
	ppool_->handleIteration();
	return IOPool::handleIteration();
}

void 
RubyIOPool::handleTerminate() {
	ppool_->handleTerminate();
	return IOPool::handleTerminate();
}

void 
RubyIOPool::RubyIOState::handleRead(IOConnection* pconn) {
	static Message 	
				datamsg(0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Incoming_Data_CONST, 0);
	RubyIOPool* psm = reinterpret_cast<RubyIOPool*>(getSM());
	psm->RouteMessage(&datamsg);
}

void 
RubyIOPool::RubyIOState::handleStart() {
	static Message 	
				startmsg(0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Initialize_CONST, 0);
	RubyIOPool* psm = reinterpret_cast<RubyIOPool*>(getSM());
	psm->RouteMessage(&startmsg);
}

void 
RubyIOPool::RubyIOState::handleIdle() {
	static Message 	
				idlemsg(0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Process_IDLE_CONST, 0);
	RubyIOPool* psm = reinterpret_cast<RubyIOPool*>(getSM());
	psm->RouteMessage(&idlemsg);
}

void 
RubyIOPool::RubyIOState::handleStop() {
	static Message 	
				stopmsg(0, 0, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Release_CONST, 0);
	RubyIOPool* psm = reinterpret_cast<RubyIOPool*>(getSM());
	psm->RouteMessage(&stopmsg);
}

/*
void 
RubyIOPool::Test() {
	try {
		TestEmbededClass* pClass = new TestEmbededClass();
		VALUE value = pClass->getValue();
		
		void* data = DATA_PTR(value);
		RubySerialIOWrapper* pWrapper = reinterpret_cast<RubySerialIOWrapper*>(data);
		if(pWrapper == NULL) {
			g_pPlutoLogger->Write(LV_WARNING, "Error instantiating Ruby serial WRAPPER.");
			return;
		};
		
		if(getConnection() == NULL) {
			g_pPlutoLogger->Write(LV_CRITICAL, "No connection in Serial IO pool.");
			return;
		}
		
		pWrapper->setConn(getConnection());
	
		pClass->On();
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, e.getMessage());
	}
}
*/

};
