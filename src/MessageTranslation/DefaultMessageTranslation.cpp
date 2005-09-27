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
#include "PlutoUtils/MySQLHelper.h"
#include <assert.h>

namespace DCE {

/*****************************************************************
DefaultMessageTranslator
*****************************************************************/

DefaultMessageTranslator::DefaultMessageTranslator() 
{
}

DefaultMessageTranslator::~DefaultMessageTranslator() {
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
	DCEConfig dceconf;
	MySqlHelper mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort);
	PlutoSqlResult result_set;
	MYSQL_ROW row=NULL;
	if( (result_set.r=mySqlHelper.mysql_query_result("SELECT IR_PowerDelay FROM DeviceTemplate_AV WHERE FK_DeviceTemplate=" + devtemplid )) && (row = mysql_fetch_row(result_set.r)) )
	{
		IR_PowerDelay = atoi(row[0]);
	}
	else
	{
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
