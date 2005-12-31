#ifndef AllScreens_h
#define AllScreens_h
#include "Message.h"
#include "PreformedCommand.h"
#include "pluto_main/Define_CommandParameter.h"
#include "pluto_main/Define_Command.h"

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
	protected:
		map<int,int> *m_p_MapDesignObj;

	public:
		int m_p_MapDesignObj_Find(int PK_Screen) { map<int,int>::iterator it = m_p_MapDesignObj->find(PK_Screen); return it==m_p_MapDesignObj->end() ? NULL : (*it).second; }
		ScreenHandlerBase(map<int,int> *p_MapDesignObj) { m_p_MapDesignObj=p_MapDesignObj; }
		virtual ~ScreenHandlerBase() {}

		virtual void GotoDesignObj(int PK_DesignObj,string sID="",bool bStore_Variables=false,bool bCant_Go_Back=false)=0;
		virtual void GotoScreen(int PK_Screen,string sID="",bool bStore_Variables=false,bool bCant_Go_Back=false)
		{
			int PK_DesignObj = m_p_MapDesignObj_Find(PK_Screen);
			if( PK_DesignObj )
				GotoDesignObj(PK_DesignObj,sID,bStore_Variables,bCant_Go_Back);
			else
				BadGotoScreen(PK_Screen);
		}

		virtual void BadGotoScreen(int PK_Screen) {}

		/*  Display a message for the user */
$GEN_METHODS$
		virtual void ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage)
		{
			switch(nPK_Screen) 
			{
$GEN_SWITCH_BLOCK$
			default:
				BadGotoScreen(nPK_Screen);
				break;
			}
		}
	};
}

#endif
