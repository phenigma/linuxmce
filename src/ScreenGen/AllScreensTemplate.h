#ifndef AllScreens_h
#define AllScreens_h
#include "Message.h"
#include "PreformedCommand.h"

// Orbiter will create an instance of a class derived from ScreenHandler
// It must call ReceivedGotoScreenMessage when it receives such a message
// and it must implement GotoDesignObj, as well as override any
// screens that have to do something, like SCREEN_DisplayMessageToUser

namespace DCE
{
	 //  Commands
$GEN_CLASSES$
	class ScreenHandlerBase
	{
		map<int,int> *m_p_MapDesignObj;

		int m_p_MapDesignObj_Find(int PK_Screen) { map<int,int>::iterator it = m_p_MapDesignObj->find(PK_Screen); return it==m_p_MapDesignObj->end() ? NULL : (*it).second; }
		ScreenHandlerBase(map<int,int> *p_MapDesignObj) { m_p_MapDesignObj=p_MapDesignObj; }

		virtual void GotoDesignObj(int PK_DesignObj)=0;
		virtual void GotoScreen(int PK_Screen)
		{
			int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
			if( PK_DesignObj )
				GotoDesignObj(PK_DesignObj);
		}

		/*  Display a message for the user */
$GEN_METHODS$
		virtual void ReceivedGotoScreenMessage(Message *pMessage)
		{
			switch(pMessage->m_dwID)
			{
$GEN_SWITCH_BLOCK$
			}
		}
	};
}

#endif
