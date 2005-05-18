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
#include "RubyDCEEmbededClass.h"
#include "RubyDeviceWrapper.h"
#include "RubyCommandWrapper.h"

#include "DCE/Message.h"
#include "DCE/Logger.h"

#include "pluto_main/Define_Command.h"

#include <ruby.h>

using namespace std;
using namespace EMBRUBY;

namespace DCE {

RubyDCEEmbededClass::RubyDCEEmbededClass() {
}

RubyDCEEmbededClass::RubyDCEEmbededClass(RubyDCECodeSupplier* pcs, int dwPK_Device) 
	: RubyEmbededClassImpl<RubyDCEEmbededClass>(NULL), pcs_(pcs)
{
	char buff[20];
	sprintf(buff, "Device_%d", dwPK_Device);
	Init(buff);
	g_pPlutoLogger->Write(LV_STATUS, "Class: %s instantiated.", buff);
}


RubyDCEEmbededClass::~RubyDCEEmbededClass()
{
}

void 
RubyDCEEmbededClass::CallCmdHandler(Message *pMessage) {
	if(!pcs_->isCmdImplemented(pMessage->m_dwID)) {
		if(pMessage->m_dwID != COMMAND_Process_IDLE_CONST) {
			g_pPlutoLogger->Write(LV_STATUS, "Command %d not supported.", pMessage->m_dwID);
		}
		return;
	}
	
	std::list<int> paramids;
	pcs_->getParamsOrderForCmd(pMessage->m_dwID, paramids);
	
	std::list<VALUE> params;
	for(std::list<int>::iterator pmit = paramids.begin(); pmit != paramids.end(); pmit++) {
		params.push_back(StrToValue((pMessage->m_mapParameters[*pmit]).c_str()));
	}
	
	char buff[20];
	sprintf(buff, "cmd_%d", (int)pMessage->m_dwID);
	try {
		callmethod(buff, params);
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, (string("Error while calling method: ") + e.getMessage()).c_str());
	}
}

void 
RubyDCEEmbededClass::CallCmdForChildHandler(Message *pMessage) {
	if(!pcs_->isProcChildCommandAssigned()) {
		g_pPlutoLogger->Write(LV_STATUS, "Command For Child not supported.");
		return;
	}

	std::list<VALUE> params;

	RubyEmbededClassImpl<>* pembclass = NULL;
	try {	
		pembclass = new RubyEmbededClassImpl<>("Command");
		RubyCommandWrapper* pWrapper = reinterpret_cast<RubyCommandWrapper*>(DATA_PTR(pembclass->getValue()));
		
		if(pWrapper == NULL) {
			delete pembclass;
			return;
		} else {
			pWrapper->setDevIdFrom(pMessage->m_dwPK_Device_From);
			pWrapper->setDevIdTo(pMessage->m_dwPK_Device_To);
			pWrapper->setPriority(pMessage->m_dwPriority);
			pWrapper->setType(pMessage->m_dwMessage_Type);
			pWrapper->setId(pMessage->m_dwID);
			
			pWrapper->setParams(pMessage->m_mapParameters);
		};
	} catch(RubyException e) {
		pembclass = NULL;
		g_pPlutoLogger->Write(LV_CRITICAL, "Exception in Ruby occured: %s.", e.getMessage());
		return;
	}
	
	params.push_back(pembclass->getValue());
	
	try {
		callmethod("cmd_ReceiveCommandForChild", params);
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, (string("Error while calling method: ") + e.getMessage()).c_str());
	}
	
	delete pembclass;
}

};
