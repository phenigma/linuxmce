#include "ScreenHandler.h"
#include "pluto_main/Define_Variable.h"
//-----------------------------------------------------------------------------------------------------
ScreenHandler::ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj) : ScreenHandlerBase(p_MapDesignObj)
{
	m_pOrbiter = pOrbiter;
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::GotoDesignObj(int PK_DesignObj)
{
	m_pOrbiter->CMD_Goto_DesignObj(0, StringUtils::ltos(PK_DesignObj), "", "", false, false);
}
//-----------------------------------------------------------------------------------------------------
void ScreenHandler::SCREEN_mnuNewMacAddress(string sMessage1) 
{ 
	m_pOrbiter->CMD_Set_Variable(VARIABLE_Misc_Data_1_CONST, sMessage1);
	ScreenHandlerBase::SCREEN_mnuNewMacAddress(sMessage1);
}
//-----------------------------------------------------------------------------------------------------
