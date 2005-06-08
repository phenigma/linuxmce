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
	if(!RubyDCEDeviceNode::Init(pcs, pdb)) {
		return false;
	}
	
	RubySerialIOWrapper* pWrapper = reinterpret_cast<RubySerialIOWrapper*>(DATA_PTR(getEmbClass()->getValue()));
	
	/*init connection*/
	if(getConnection() == NULL) {
		g_pPlutoLogger->Write(LV_CRITICAL, "No connection in Serial IO pool.");
		return false;
	}
	
	pWrapper->setConn(RubySerialIOConnectionWrapper(getConnection()));
	return true;
}

void 
RubyIOPool::Cleanup() {
	RubyDCEDeviceNode::Cleanup();
}

bool 
RubyIOPool::handleStartup() {
	ppool_->handleStartup();
	if(!IOPool::handleStartup()) {
		return false;
	}	
	return RubyDCEDeviceNode::handleStartup();
}

bool 
RubyIOPool::handleIteration() {
printf("RubyIOPool::handleIteration\n",);
	ppool_->handleIteration();
	return IOPool::handleIteration();
}

void 
RubyIOPool::handleTerminate() {
	ppool_->handleTerminate();
	IOPool::handleTerminate();
	RubyDCEDeviceNode::handleTerminate();
}

void 
RubyIOPool::RubyIOState::handleRead(IOConnection* pconn) {
	RubyIOPool* psm = reinterpret_cast<RubyIOPool*>(getSM());
printf("RubyIOPool::RubyIOState::handleRead %p\n",psm);
	Message datamsg(0, psm->getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Incoming_Data_CONST, 0);
	psm->handleMessage(&datamsg);
}

};
