#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCE/Logger.h"
#include "Router.h"
#include "PlutoOrbiter.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_Variable.h"

// temp
#define VARIABLE_CURRENTLY_PLAYING_CONST 9999

void PlutoOrbiter::Refresh(bool bReacquireGrids)
{
	if( !bReacquireGrids )
		m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER, m_pDeviceData_Router->m_dwPK_Device, PRIORITY_NORMAL, 
			MESSAGETYPE_COMMAND, COMMAND_Refresh_CONST, 0));
	else
		m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER, m_pDeviceData_Router->m_dwPK_Device, PRIORITY_NORMAL, 
			MESSAGETYPE_COMMAND, COMMAND_Refresh_CONST, 1,COMMANDPARAMETER_DataGrid_ID_CONST,"1"));
}

void PlutoOrbiter::Regen()
{
	m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER, m_pDeviceData_Router->m_dwPK_Device, PRIORITY_NORMAL, 
		MESSAGETYPE_COMMAND, COMMAND_Regen_Screen_CONST, 0));
}

void PlutoOrbiter::Beep()
{
	m_pRouter->DispatchMessage(new Message(DEVICEID_DCEROUTER, m_pDeviceData_Router->m_dwPK_Device, PRIORITY_NORMAL, 
		MESSAGETYPE_COMMAND, COMMAND_Orbiter_Beep_CONST, 0));
}

void PlutoOrbiter::GotoScreen(string DesignObj)
{
	m_pRouter->DispatchMessage(new Message(0,m_pDeviceData_Router->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
		COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,DesignObj.c_str()));
}

void PlutoOrbiter::GotoScreen(int DesignObj)
{
	m_pRouter->DispatchMessage(new Message(0,m_pDeviceData_Router->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
		COMMAND_Goto_Screen_CONST,1,COMMANDPARAMETER_PK_DesignObj_CONST,StringUtils::itos(DesignObj).c_str()));
}

void PlutoOrbiter::SetVariable(int PK_Variable,string Value)
{
	m_pRouter->DispatchMessage(new Message(0, m_pDeviceData_Router->m_dwPK_Device, PRIORITY_NORMAL, 
		MESSAGETYPE_COMMAND, COMMAND_Set_Variable_CONST, 2, 
		COMMAND_Set_Variable_CONST, StringUtils::itos(PK_Variable).c_str(),
		COMMANDPARAMETER_Value_To_Assign_CONST,Value.c_str()));
}

void PlutoOrbiter::SetNowPlaying(string s)
{
	if( m_pMobileOrbiter || s==m_sState_NowPlaying )
		return;
	m_sState_NowPlaying=s;
	m_pRouter->DispatchMessage(new Message(0,m_pDeviceData_Router->m_dwPK_Device,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,
		COMMAND_Set_Variable_CONST,2,COMMAND_Set_Variable_CONST,StringUtils::itos(VARIABLE_CURRENTLY_PLAYING_CONST).c_str(),
		COMMANDPARAMETER_Value_To_Assign_CONST,s.c_str()));
}
