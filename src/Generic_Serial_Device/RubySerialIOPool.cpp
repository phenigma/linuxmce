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
#include "RubySerialIOPool.h"

#include "DCE/Logger.h"
#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "PlutoUtils/FileUtils.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Table_DeviceData.h"

#include "RubyEmbeder.h"
#include "RubySerialIOWrapper.h"

#include "TestRubyEmbederCodeSupplier.h"
#include "TestEmbededClass.h"


using namespace EMBRUBY;
using namespace std;

namespace DCE {

RubySerialIOPool::RubySerialIOPool(RubyDCECodeSupplier* pcs, Database_pluto_main* pdb, DeviceData_Impl* pdevdata) 
	: pdb_(pdb), pdevdata_(pdevdata), pcs_(pcs), pdceconn_(NULL), pembclass_(NULL)
{
	setState(&defstate_);
}


RubySerialIOPool::~RubySerialIOPool()
{
}

bool 
RubySerialIOPool::handleStartup() {
	try {
		pembclass_ = new RubyDCEEmbededClass(pdevdata_->m_dwPK_Device);
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
		
		RubySerialIOConnectionWrapper conn(getConnection());
		pWrapper->setConn(conn);
		
		pWrapper->setDCEConnector(pdceconn_);
		
		if(!pdevdata_) {
			g_pPlutoLogger->Write(LV_WARNING, "Serial IO pool was not initialized with device Data.");
		} else {
			PopulateDevice(pdevdata_, pWrapper->getDevice());
		}
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed instantiating class: %s.", e.getMessage());
	}
	return true;
}

void 
RubySerialIOPool::PopulateDevice(DeviceData_Impl* pdevdata, RubyDeviceWrapper& devwrap) {
	devwrap.setDevId(pdevdata->m_dwPK_Device);

	int numparams = 0;	
	std::map<int, string>::iterator it = pdevdata->m_mapParameters.begin();
	while(it != pdevdata->m_mapParameters.end()) {
		Row_DeviceData *p_Row_DeviceData = pdb_->DeviceData_get()->GetRow((*it).first);
		devwrap.setData(FileUtils::ValidCPPName(p_Row_DeviceData->Description_get()).c_str(), (*it).second.c_str());
		it++; numparams++;
	}
	g_pPlutoLogger->Write(LV_STATUS, "Added %d data params to device %d.", numparams, pdevdata->m_dwPK_Device);
		
	std::map<int, RubyDeviceWrapper>& childdevices = devwrap.getChildDevices();
    VectDeviceData_Impl& vDeviceData = pdevdata->m_vectDeviceData_Impl_Children;
    for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		RubyDeviceWrapper& childdevwrap = childdevices[vDeviceData[i]->m_dwPK_Device];
		PopulateDevice(vDeviceData[i], childdevwrap);
    }
	g_pPlutoLogger->Write(LV_STATUS, "Added %d wrapped child devices to device %d.", vDeviceData.size(), pdevdata->m_dwPK_Device);
}

void 
RubySerialIOPool::handleTerminate() {
	g_pPlutoLogger->Write(LV_STATUS, "Serial IO Pool for device %d terminated.", pdevdata_->m_dwPK_Device);
	delete pembclass_;
}


int 
RubySerialIOPool::RouteMessage(Message* pMessage) {
	if(!pembclass_ || !pcs_) {
		return -1;
	}
	
	pembclass_->CallCmdHandler(pcs_, pMessage);
	return 0;
}

void 
RubySerialIOPool::Test() {
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

};
