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
#include "RubyIOManager.h"
#include "DCE/Message.h"
#include "DCE/Logger.h"
#include "DCE/Event_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_ParameterType.h"

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
	if(!pcs_->isCmdImplemented(pMessage->m_dwPK_Device_To, pMessage->m_dwID)) {
		if(pMessage->m_dwID != COMMAND_Process_IDLE_CONST) {
			g_pPlutoLogger->Write(LV_STATUS, "Command %d not supported.", pMessage->m_dwID);
		}
		return;
	}
	
	std::list<int> paramids;
	pcs_->getParamsOrderForCmd(pMessage->m_dwPK_Device_To, pMessage->m_dwID, paramids);
	
	std::list<VALUE> params;
	for(std::list<int>::iterator pmit = paramids.begin(); pmit != paramids.end(); pmit++) {
		params.push_back(StrToValue((pMessage->m_mapParameters[*pmit]).c_str()));
	}
	VALUE result;
	
	char buff[20];
	sprintf(buff, "cmd_%d", (int)pMessage->m_dwID);
	try {
		result=callmethod(buff, params);
		if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
		{
			pMessage->m_bRespondedToMessage=true;
			Message *pMessageOut=new Message(pMessage->m_dwPK_Device_To,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
			char** tmpbuf;
			if(TYPE(result)==T_ARRAY)
			{
				g_pPlutoLogger->Write(LV_STATUS, "Got returnParamArray from Ruby");
		    	tmpbuf=(char**)calloc(RARRAY(result)->len,sizeof(char*));
				for(int i=0;i<RARRAY(result)->len;i++)
				{
					VALUE v=RARRAY(result)->ptr[i];
					if(TYPE(v)!=T_NIL)
					{
						switch(this->pcs_->getParamType(i))
						{
							case PARAMETERTYPE_string_CONST:
								pMessageOut->m_mapParameters[i]=STR2CSTR(v);
								break;
							case PARAMETERTYPE_int_CONST:
						    	tmpbuf[i]=(char*)malloc(20);
								sprintf(tmpbuf[i],"%d",NUM2INT(v));
								pMessageOut->m_mapParameters[i]=tmpbuf[i];
								break;
							case PARAMETERTYPE_bool_CONST:
								pMessageOut->m_mapParameters[i]=(TYPE(v)==T_TRUE?true:false);
								break;
							case PARAMETERTYPE_Data_CONST:
								pMessageOut->m_mapData_Parameters[i]=RSTRING(v)->ptr;
								pMessageOut->m_mapData_Lengths[i]=RSTRING(v)->len;
								break;
							case PARAMETERTYPE_double_CONST:
						    	tmpbuf[i]=(char*)malloc(20);
								sprintf(tmpbuf[i],"%lf",NUM2DBL(v));
								pMessageOut->m_mapParameters[i]=tmpbuf[i];
								break;
						}
						if(this->pcs_->getParamType(i) != PARAMETERTYPE_Data_CONST)
							g_pPlutoLogger->Write(LV_STATUS, "    Parameter %d = \"%s\"",i,pMessageOut->m_mapParameters[i].c_str());
						else
							g_pPlutoLogger->Write(LV_STATUS, "    Parameter %d is %d bytes long",i,pMessageOut->m_mapData_Lengths[i]);
					}
				}
			}
			pMessageOut->m_mapParameters[0]="OK";
			RubyIOManager* pmanager = RubyIOManager::getInstance();
			pmanager->SendMessage(pMessageOut);
			if(TYPE(result)==T_ARRAY)
			{
				for(int i=0;i<RARRAY(result)->len;i++)
				{
					if(tmpbuf[i])
						free(tmpbuf[i]);
				}
				if(tmpbuf)
					free(tmpbuf);
			}
		}
	} catch(RubyException e) {
		g_pPlutoLogger->Write(LV_CRITICAL, (string("Error while calling method: ") + e.getMessage()).c_str());
	}

		
}

void 
RubyDCEEmbededClass::CallCmdForChildHandler(unsigned devid, Message *pMessage) {
	if(!pcs_->isProcChildCommandAssigned(devid)) {
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
