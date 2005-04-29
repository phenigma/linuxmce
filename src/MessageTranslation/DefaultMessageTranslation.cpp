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
#include "DCE/Logger.h"
#include "DCE/DCEConfig.h"
#include "DefaultMessageTranslation.h"

#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Table_DeviceTemplate_AV.h"

#include <assert.h>

namespace DCE {

/*****************************************************************
DefaultMessageTranslator
*****************************************************************/

DefaultMessageTranslator::DefaultMessageTranslator() 
	: dbprovided_(false), pdb_(NULL)
{
}

DefaultMessageTranslator::~DefaultMessageTranslator() {
	if(!dbprovided_ && pdb_ != NULL) {
		delete pdb_;
	}
}

void 
DefaultMessageTranslator::setDatabase(Database_pluto_main* pdb) {
	if(!dbprovided_ && pdb_ != NULL) {
		delete pdb_;
		dbprovided_ = true;
	}
	
	pdb_ = pdb;
}

Database_pluto_main* 
DefaultMessageTranslator::getDatabase() {
	if(!dbprovided_	&& pdb_ == NULL) {
		g_pPlutoLogger->Write(LV_STATUS, "Default Translation has no Database set, no problems, instantiating our own...");
		
		DCEConfig dceconf;
		pdb_ = new Database_pluto_main();
		if(!pdb_->Connect(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword,
										dceconf.m_sDBName,dceconf.m_iDBPort) )
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
			delete pdb_;
			pdb_ = NULL;
		} else {
			g_pPlutoLogger->Write(LV_STATUS, "Successfully connected to database!");
		}
	}
	return pdb_;
}

bool 
DefaultMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	Command_Impl* pTargetDev = FindTargetDevice(inrepl.getMessage().m_dwPK_Device_To);
	if(!pTargetDev) {
		g_pPlutoLogger->Write(LV_WARNING, "Target Device %d Not Found.", inrepl.getMessage().m_dwPK_Device_To);
		return false;
	}

	int IR_PowerDelay = 0;
	long devtemplid = pTargetDev->m_pData->m_dwPK_DeviceTemplate;
	
	Row_DeviceTemplate_AV* pRowAV = getDatabase()->DeviceTemplate_AV_get()->GetRow(devtemplid);
	if(pRowAV) {
		g_pPlutoLogger->Write(LV_STATUS, "Found AV properties for device!");
		IR_PowerDelay = pRowAV->IR_PowerDelay_get();
	} else {
		g_pPlutoLogger->Write(LV_STATUS, "Device has no AV properties");
	}
	
	/**************************************************************************************
	Determine message queue attributes
	**************************************************************************************/
	int delay = 0, repeat = 1;

	/*general attributes*/	
	if(inrepl.getMessage().m_mapParameters.count(COMMANDPARAMETER_Repeat_Command_CONST) > 0 && 
		inrepl.getMessage().m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST].length() > 0) {
		repeat = atoi(inrepl.getMessage().m_mapParameters[COMMANDPARAMETER_Repeat_Command_CONST].c_str());
		if(repeat == 0) {
			repeat = 1;
		}
	}
	
	/*command specific attributes*/	
	switch(inrepl.getMessage().m_dwID) {
		case COMMAND_Generic_On_CONST:
		case COMMAND_Generic_Off_CONST:
		case COMMAND_Toggle_Power_CONST: {
			g_pPlutoLogger->Write(LV_STATUS, "Got power command for Device: %d", pTargetDev->m_dwPK_Device);
			delay = IR_PowerDelay;
			g_pPlutoLogger->Write(LV_STATUS, "Using delay of: %ld.", delay);
		} break;
		default: {
		/*
			if (inrepl.getMessage().m_mapParameters.count(COMMANDPARAMETER_PK_Command_Input_CONST)>0) {
				delay = IR_ModeDelay;
			} */
		} break;
	}

	inrepl.setPostDelay(delay);
	inrepl.setCount(repeat);
	return false;
}

/*****************************************************************
DefaultMessageDispatcher
*****************************************************************/
void 
DefaultMessageDispatcher::DispatchMessage(MessageReplicator& inrepl) {
	for(int i = 0; i < inrepl.getCount(); i++) {
		Sleep(inrepl.getPreDelay());
		DispatchMessage(&inrepl.getMessage());
		Sleep(inrepl.getPostDelay());
	}
}

};
