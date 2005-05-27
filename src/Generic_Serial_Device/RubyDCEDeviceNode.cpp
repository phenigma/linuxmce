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
#include "RubyDCEDeviceNode.h"

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

#include "RubyIOManager.h"

using namespace EMBRUBY;
using namespace std;

namespace DCE {

RubyDCEDeviceNode::RubyDCEDeviceNode()
	: parent_(NULL), pembclass_(NULL)
{
}


RubyDCEDeviceNode::~RubyDCEDeviceNode()
{
}

bool 
RubyDCEDeviceNode::Init(RubyDCECodeSupplier* pcs, Database_pluto_main* pdb) {
	try {
		if(!pdevdata_) {
			g_pPlutoLogger->Write(LV_WARNING, "Serial IO pool was not initialized with device Data.");			return false;
		}
		
		pembclass_ = new RubyDCEEmbededClass(pcs, pdevdata_->m_dwPK_Device);
		VALUE value = pembclass_->getValue();
		
		void* data = DATA_PTR(value);
		RubySerialIOWrapper* pWrapper = reinterpret_cast<RubySerialIOWrapper*>(data);
		if(pWrapper == NULL) {
			g_pPlutoLogger->Write(LV_WARNING, "Error instantiating Ruby serial WRAPPER.");
			return false;
		};
		
		pWrapper->setDCEConnector(RubyIOManager::getInstance());
		if(parent_ != NULL) {
		    pWrapper->setParent(parent_->getEmbClass()->getValue());
		}
		
		PopulateDevice(pdevdata_, pdb, pWrapper->getDevice());
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed instantiating class: %s.", e.getMessage());
	}
	
        std::list<RubyDCEDeviceNode*>& children = getChildren();
	for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
	    (*it)->Init(pcs, pdb);
        }
	
	return true;
}

void 
RubyDCEDeviceNode::Cleanup() {
        std::list<RubyDCEDeviceNode*>& children = getChildren();
	for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
	    (*it)->Cleanup();
        }
	g_pPlutoLogger->Write(LV_STATUS, "Serial IO Pool for device %d terminated.", pdevdata_->m_dwPK_Device);
	delete pembclass_;
}

RubyDCEDeviceNode* 
RubyDCEDeviceNode::findChild(unsigned devid) {
    std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		if((*it)->getDeviceData()->m_dwPK_Device == devid) {
			return *it;
		}
    }
	return NULL;
}

bool 
RubyDCEDeviceNode::handleNoMessage() {
	if(!getEmbClass()) {
        return false;
    }
	
	Message idlemsg(0, getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_IDLE_CONST, 0);
	getEmbClass()->CallCmdHandler(&idlemsg);    

	std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
		(*it)->handleNoMessage();
	}
	return true;
}

bool
RubyDCEDeviceNode::handleMessage(Message* pmsg) {
    if(!getEmbClass()) {
        return false;
    }

    bool ret = false;
    if(getDeviceData()->m_dwPK_Device == (unsigned long)pmsg->m_dwPK_Device_To) {
        getEmbClass()->CallCmdHandler(pmsg);
		ret = true;
    } else {
    	std::list<RubyDCEDeviceNode*>& children = getChildren();
	    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
    	     ret |= (*it)->handleMessage(pmsg);
	    }
        if(!ret && findChild(pmsg->m_dwPK_Device_To)) {
            getEmbClass()->CallCmdForChildHandler(getDeviceData()->m_dwPK_Device, pmsg);
            ret = true;
        }
	}

	return ret;
}

bool
RubyDCEDeviceNode::handleStartup() {
    Message startmsg(0, getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Initialize_CONST, 0);
    getEmbClass()->CallCmdHandler(&startmsg);

    
    std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
        (*it)->handleStartup();
    }
    return true;
}

void 
RubyDCEDeviceNode::handleTerminate() {
    Message stopmsg(0, getDeviceData()->m_dwPK_Device, 0, MESSAGETYPE_COMMAND, COMMAND_Process_Release_CONST, 0);
	getEmbClass()->CallCmdHandler(&stopmsg);

    std::list<RubyDCEDeviceNode*>& children = getChildren();
    for(std::list<RubyDCEDeviceNode*>::iterator it = children.begin(); it != children.end(); it++) {
        (*it)->handleTerminate();
    }
}

void 
RubyDCEDeviceNode::PopulateDevice(DeviceData_Impl* pdevdata, Database_pluto_main* pdb, RubyDeviceWrapper& devwrap) {
	devwrap.setDevId(pdevdata->m_dwPK_Device);
	devwrap.setDevTemplId(pdevdata->m_dwPK_DeviceTemplate);

	int numparams = pdevdata->m_mapParameters.size();	
	devwrap.setData(pdevdata->m_mapParameters);
	/*
	std::map<int, string>::iterator it = pdevdata->m_mapParameters.begin();
	while(it != pdevdata->m_mapParameters.end()) {
		Row_DeviceData *p_Row_DeviceData = pdb->DeviceData_get()->GetRow((*it).first);
		if( !p_Row_DeviceData )
		{
			g_pPlutoLogger->Write(LV_STATUS,"RubyIOPool::PopulateDevice with device %d couldn't find parameter %d",
				pdevdata->m_dwPK_Device,(*it).first);
		}
		else
			devwrap.setData(FileUtils::ValidCPPName(p_Row_DeviceData->Description_get()).c_str(), (*it).second.c_str());
		it++; numparams++;
	}*/
	g_pPlutoLogger->Write(LV_STATUS, "Added %d data params to device %d.", numparams, pdevdata->m_dwPK_Device);
		
	std::map<int, RubyDeviceWrapper>& childdevices = devwrap.getChildDevices();
    VectDeviceData_Impl& vDeviceData = pdevdata->m_vectDeviceData_Impl_Children;
    for(VectDeviceData_Impl::size_type i = 0; i < vDeviceData.size(); i++) {
		RubyDeviceWrapper& childdevwrap = childdevices[vDeviceData[i]->m_dwPK_Device];
		PopulateDevice(vDeviceData[i], pdb, childdevwrap);
    }
	g_pPlutoLogger->Write(LV_STATUS, "Added %d wrapped child devices to device %d.", vDeviceData.size(), pdevdata->m_dwPK_Device);
}

};
