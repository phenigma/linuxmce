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
RubyDCEEmbededClass::CallCmdHandler(Message *pMessage) {
	std::list<int> paramids;
	std::list<int>::iterator pmit;
	
	map<long, string>::const_iterator it = pMessage->m_mapParameters.begin();
	while(it != pMessage->m_mapParameters.end()) {
		// sort through insertion, need for correct argumnt passing
		pmit = paramids.begin();
		while(pmit != paramids.end() && *pmit < (*it).first) {
			pmit++;
		}
		
		paramids.insert(pmit, (*it).first);
		it++;
	}
	
	std::list<VALUE> params;
//	params.push_back(StrToValue(""));
	for(pmit = paramids.begin(); pmit != paramids.end(); pmit++) {
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
