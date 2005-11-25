#ifndef __SCREEN_HANDLER_H__
#define __SCREEN_HANDLER_H__
//-----------------------------------------------------------------------------------------------------
#include "Gen_Devices/AllScreens.h"
#include "Orbiter.h"
//-----------------------------------------------------------------------------------------------------
class ScreenHandler : public ScreenHandlerBase
{
protected:
	Orbiter *m_pOrbiter;

public: 
	ScreenHandler(Orbiter *pOrbiter, map<int,int> *p_MapDesignObj);

	virtual void GotoDesignObj(int PK_DesignObj);
	virtual void SCREEN_mnuNewMacAddress(string sMessage1);
};
//-----------------------------------------------------------------------------------------------------
#endif