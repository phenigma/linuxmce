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
	class SCREEN_DisplayMessageToUser : public PreformedCommand
	{
	public:
		SCREEN_DisplayMessageToUser(long DeviceIDFrom, long DeviceIDTo,
			string sMessage1, string sMessage2)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL, 1, 7 /* goto screen */,
				10 /* PK_Screen */,29 /* screen ID */,
				12 /* command parm for message 1 */, sMessage1, 13 /* command parm for message 2 */, sMessage2);
		}
	};

	class SCREEN_AdvancedOptions : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL, 1, 7 /* goto screen */,
				10 /* PK_Screen */,30 /* screen ID */);
		}
	};


	class ScreenHandlerBase
	{
	protected:
		map<int,int> *m_p_MapDesignObj;

	public:
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
		virtual void SCREEN_DisplayMessageToUser(string sMessage1, string sMessage2) { GotoScreen(29); }
		virtual void SCREEN_AdvancedOptions() { GotoScreen(30); }

		virtual void ReceivedGotoScreenMessage(Message *pMessage)
		{
			switch(pMessage->m_dwID)
			{
				case 29:
				{
					string sMessage1 = pMessage->m_mapParameters[12];
					string sMessage2 = pMessage->m_mapParameters[13];
					SCREEN_DisplayMessageToUser(sMessage1, sMessage2);
					break;
				}
				case 30:
				{
					SCREEN_AdvancedOptions();
					break;
				}

			}
		}
	};
}

#endif
