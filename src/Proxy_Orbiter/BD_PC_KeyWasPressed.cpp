#include "PlutoUtils/CommonIncludes.h"
#include <iostream>
#include <fstream>
#include "BD/BDCommandProcessor.h"
#include "BD_PC_KeyWasPressed.h"

#include "../Proxy_Orbiter/Proxy_Orbiter.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_Proxy_Orbiter.h"
#include "../DCE/Logger.h"	

BD_PC_KeyWasPressed::BD_PC_KeyWasPressed(int Key, int EventType) 
	
{
	m_Key=Key;
    m_EventType = EventType;

}

void BD_PC_KeyWasPressed::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_Key);
    Write_long(m_EventType);
}

void BD_PC_KeyWasPressed::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_Key = Read_long();
    m_EventType = Read_long();
}


bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
/*
	xBDCommandProcessor_Proxy_Orbiter *m_pProcessor = 
		(BDCommandProcessor_Proxy_Orbiter *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pProxy_Orbiter->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter || pOrbiter->m_pOrbiter->m_bQuit)
	{
        g_pPlutoLogger->Write(LV_WARNING, "No keypress command will be dispatch: Orbiter was killed or is exiting.");
		return false;
	}

    if(m_EventType == 2) //execute
    {
        g_pPlutoLogger->Write(LV_WARNING, "Received BD_PC_KeyWasPressed from PlutoMO. Phone code: %d, 'execute'", m_Key);
        pOrbiter->m_pOrbiter->HandleButtonEvent(m_Key);
    }
    else
    {
	    g_pPlutoLogger->Write(LV_WARNING, "Received BD_PC_KeyWasPressed from PlutoMO. Phone code: %d, %s", m_Key,
            m_EventType ? "key up" : "key down");

	    Orbiter::Event orbiterEvent;
        orbiterEvent.type = m_EventType ? Orbiter::Event::BUTTON_UP : Orbiter::Event::BUTTON_DOWN;
	    orbiterEvent.data.button.m_iPK_Button = m_Key;
	    pOrbiter->m_pOrbiter->ProcessEvent(orbiterEvent);
    }
*/
	return true;
}
