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

#include "DCE/Message.h"
#include "DCE/Logger.h"

using namespace std;
using namespace EMBRUBY;

namespace DCE {

RubyDCEEmbededClass::RubyDCEEmbededClass() {
}

RubyDCEEmbededClass::RubyDCEEmbededClass(int dwPK_Device) 
	: RubyEmbededClassImpl<RubyDCEEmbededClass>(NULL)
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
RubyDCEEmbededClass::CallCmdHandler(RubyDCECodeSupplier* pcg, Message *pMessage) {
	if(!pcg->isCmdImplemented(pMessage->m_dwID)) {
		g_pPlutoLogger->Write(LV_STATUS, "Command %d not supported", pMessage->m_dwID);
		return;
	}
	
	std::list<int> paramids;
	pcg->getParamsOrderForCmd(pMessage->m_dwID, paramids);
	
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

};
