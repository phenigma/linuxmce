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
#include "Generic_Serial_Device_MessageProcessor.h"
#include "DCE/Logger.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommandParameter.h"

namespace DCE {

Generic_Serial_Device_MessageProcessor::Generic_Serial_Device_MessageProcessor()
{
}


Generic_Serial_Device_MessageProcessor::~Generic_Serial_Device_MessageProcessor()
{
}

bool 
Generic_Serial_Device_MessageProcessor::Translate(Message* pmsg, MessageTimedQueue* pqueue) {
	if(!pcmdimpl_) {
		return false;
	}

	Command_Impl* pTargetDev = FindTargetDevice(pmsg->m_dwPK_Device_To);
	if(!pTargetDev) {
		g_pPlutoLogger->Write(LV_WARNING, "Target Device %d Not Found.", pmsg->m_dwPK_Device_To);
		return false;
	}
	
	long delay=0, count=1;
	map<int,string>::iterator dparam;
	
	if (pmsg->m_dwID == COMMAND_Generic_On_CONST 
			|| pmsg->m_dwID == COMMAND_Generic_Off_CONST 
			|| pmsg->m_dwID == COMMAND_Toggle_Power_CONST)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Got power command for Device: %d", pTargetDev->m_dwPK_Device);
		dparam = pTargetDev->m_pData->m_mapParameters.find(DEVICEDATA_IR_Power_Delay_CONST);
		if (dparam != pTargetDev->m_pData->m_mapParameters.end()) {
			delay = atoi((*dparam).second.c_str());
			g_pPlutoLogger->Write(LV_STATUS, "Using delay of: %ld.", delay);
		} else {
			g_pPlutoLogger->Write(LV_STATUS, "No delay found.");
		}
	}
	else if (pmsg->m_mapParameters.count(COMMANDPARAMETER_PK_Command_Input_CONST)>0)
	{
		dparam = pTargetDev->m_pData->m_mapParameters.find(DEVICEDATA_IR_Mode_Delay_CONST);
		if (dparam != pTargetDev->m_pData->m_mapParameters.end())
		{
			delay = atoi((*dparam).second.c_str());
			g_pPlutoLogger->Write(LV_STATUS, "Found mode delay of: %d", delay);
		}
	}

	if(pmsg->m_mapParameters.count(COMMANDPARAMETER_Repeat_CONST) > 0) {
		string countstr = pmsg->m_mapParameters[COMMANDPARAMETER_Repeat_CONST];
		if(!countstr.empty()) {
			count = atoi(countstr.c_str());
			if(count == 0) {
				count = 1;
			}
		}
	}

	map<long, string>::iterator param;
	switch(pmsg->m_dwID) {
		case COMMAND_Input_Select_CONST: {
				param = pmsg->m_mapParameters.find(COMMANDPARAMETER_PK_Command_Input_CONST);
				if (param != pmsg->m_mapParameters.end()) {
					int cmd = atoi((*param).second.c_str());
					Message msg(pmsg->m_dwPK_Device_From, pmsg->m_dwPK_Device_To, PRIORITY_NORMAL, MESSAGETYPE_COMMAND,
									cmd, 0);
					g_pPlutoLogger->Write(LV_STATUS, "Added <Input Select> translated to %d with delay %d.", cmd, delay);
					pqueue->PushToBack(&msg, count, delay);
				} else {
					g_pPlutoLogger->Write(LV_WARNING, "PK_Command_Input dparameter not found.");
					
				}
			} break;
		default: {
				g_pPlutoLogger->Write(LV_STATUS, 
					"Adding Untranslated Message to Translation Queue: Device: %d, Command: %d, Count: %d, Delay: %d", 
							pTargetDev->m_dwPK_Device, pmsg->m_dwID, count, delay);
				pqueue->PushToBack(pmsg, count, delay);
			} break;
	}
	return true;
}

Command_Impl* 
Generic_Serial_Device_MessageProcessor::FindTargetDevice(long devid) {
	return FindTargetDevice(devid, pcmdimpl_);
}

Command_Impl* 
Generic_Serial_Device_MessageProcessor::FindTargetDevice(long devid, Command_Impl* pfromdev) {
	if(devid == pfromdev->m_dwPK_Device) {
		return pfromdev;
	}
	
	MapCommand_Impl::iterator it = pfromdev->m_mapCommandImpl_Children.begin();
	while(it != pfromdev->m_mapCommandImpl_Children.end()) {
		Command_Impl* pcmdimpl = FindTargetDevice(devid, (*it).second);
		if(pcmdimpl) {
			return pcmdimpl;
		}
		it++;
	}
	
	return NULL;
}


};
