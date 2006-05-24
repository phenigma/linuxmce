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
	class SCREEN_Main : public PreformedCommand
	{
	public:
		SCREEN_Main(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Main_DL : public PreformedCommand
	{
	public:
		SCREEN_Main_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Main_DT : public PreformedCommand
	{
	public:
		SCREEN_Main_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Main_Cat : public PreformedCommand
	{
	public:
		SCREEN_Main_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "1" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights : public PreformedCommand
	{
	public:
		SCREEN_Lights(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights_DL : public PreformedCommand
	{
	public:
		SCREEN_Lights_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights_DT : public PreformedCommand
	{
	public:
		SCREEN_Lights_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Lights_Cat : public PreformedCommand
	{
	public:
		SCREEN_Lights_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "2" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media : public PreformedCommand
	{
	public:
		SCREEN_Media(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media_DL : public PreformedCommand
	{
	public:
		SCREEN_Media_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media_DT : public PreformedCommand
	{
	public:
		SCREEN_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Media_Cat : public PreformedCommand
	{
	public:
		SCREEN_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "3" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate : public PreformedCommand
	{
	public:
		SCREEN_Climate(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate_DL : public PreformedCommand
	{
	public:
		SCREEN_Climate_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate_DT : public PreformedCommand
	{
	public:
		SCREEN_Climate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Climate_Cat : public PreformedCommand
	{
	public:
		SCREEN_Climate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "4" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security : public PreformedCommand
	{
	public:
		SCREEN_Security(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security_DL : public PreformedCommand
	{
	public:
		SCREEN_Security_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security_DT : public PreformedCommand
	{
	public:
		SCREEN_Security_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Security_Cat : public PreformedCommand
	{
	public:
		SCREEN_Security_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "5" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony : public PreformedCommand
	{
	public:
		SCREEN_Telephony(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony_DL : public PreformedCommand
	{
	public:
		SCREEN_Telephony_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony_DT : public PreformedCommand
	{
	public:
		SCREEN_Telephony_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Telephony_Cat : public PreformedCommand
	{
	public:
		SCREEN_Telephony_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "6" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc : public PreformedCommand
	{
	public:
		SCREEN_Misc(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc_DL : public PreformedCommand
	{
	public:
		SCREEN_Misc_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc_DT : public PreformedCommand
	{
	public:
		SCREEN_Misc_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_Misc_Cat : public PreformedCommand
	{
	public:
		SCREEN_Misc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "7" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_UserStatus : public PreformedCommand
	{
	public:
		SCREEN_UserStatus(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */);
		}
	};

	class SCREEN_UserStatus_DL : public PreformedCommand
	{
	public:
		SCREEN_UserStatus_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */);
		}
	};

	class SCREEN_UserStatus_DT : public PreformedCommand
	{
	public:
		SCREEN_UserStatus_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */);
		}
	};

	class SCREEN_UserStatus_Cat : public PreformedCommand
	{
	public:
		SCREEN_UserStatus_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "8" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "9" /* screen ID */);
		}
	};

	class SCREEN_MakeCallFavorites : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */);
		}
	};

	class SCREEN_MakeCallFavorites_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */);
		}
	};

	class SCREEN_MakeCallFavorites_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */);
		}
	};

	class SCREEN_MakeCallFavorites_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallFavorites_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "10" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDialNumber : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDialNumber_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDialNumber_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDialNumber_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDialNumber_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "11" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPlutoUser : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPlutoUser_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPlutoUser_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPlutoUser_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPlutoUser_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "12" /* screen ID */);
		}
	};

	class SCREEN_SecurityPanel : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */);
		}
	};

	class SCREEN_SecurityPanel_DL : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */);
		}
	};

	class SCREEN_SecurityPanel_DT : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */);
		}
	};

	class SCREEN_SecurityPanel_Cat : public PreformedCommand
	{
	public:
		SCREEN_SecurityPanel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "13" /* screen ID */);
		}
	};

	class SCREEN_SecurityStatusReport : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */);
		}
	};

	class SCREEN_SecurityStatusReport_DL : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */);
		}
	};

	class SCREEN_SecurityStatusReport_DT : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */);
		}
	};

	class SCREEN_SecurityStatusReport_Cat : public PreformedCommand
	{
	public:
		SCREEN_SecurityStatusReport_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "14" /* screen ID */);
		}
	};

	class SCREEN_SingleCameraViewOnly : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_SingleCameraViewOnly_DL : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_SingleCameraViewOnly_DT : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_SingleCameraViewOnly_Cat : public PreformedCommand
	{
	public:
		SCREEN_SingleCameraViewOnly_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "15" /* screen ID */,
				2 /* PK Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Intercom : public PreformedCommand
	{
	public:
		SCREEN_Intercom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */);
		}
	};

	class SCREEN_Intercom_DL : public PreformedCommand
	{
	public:
		SCREEN_Intercom_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */);
		}
	};

	class SCREEN_Intercom_DT : public PreformedCommand
	{
	public:
		SCREEN_Intercom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */);
		}
	};

	class SCREEN_Intercom_Cat : public PreformedCommand
	{
	public:
		SCREEN_Intercom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "16" /* screen ID */);
		}
	};

	class SCREEN_QuadViewCameras : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras(long DeviceIDFrom, long DeviceIDTo,
			string sList_PK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_QuadViewCameras_DL : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sList_PK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_QuadViewCameras_DT : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sList_PK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_QuadViewCameras_Cat : public PreformedCommand
	{
	public:
		SCREEN_QuadViewCameras_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sList_PK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "17" /* screen ID */,
				103 /* PK Devices List */, sList_PK_Device.c_str());
		}
	};

	class SCREEN_VideoCallInProgress : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */);
		}
	};

	class SCREEN_VideoCallInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */);
		}
	};

	class SCREEN_VideoCallInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */);
		}
	};

	class SCREEN_VideoCallInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideoCallInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "18" /* screen ID */);
		}
	};

	class SCREEN_FileSave : public PreformedCommand
	{
	public:
		SCREEN_FileSave(long DeviceIDFrom, long DeviceIDTo,
			string sText, string sPrivate, string sPublic, string sCaption)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */,
				9 /* Default User Value */, sText.c_str(), 160 /* Private */, sPrivate.c_str(), 161 /* Public */, sPublic.c_str(), 162 /* Caption */, sCaption.c_str());
		}
	};

	class SCREEN_FileSave_DL : public PreformedCommand
	{
	public:
		SCREEN_FileSave_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sText, string sPrivate, string sPublic, string sCaption)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */,
				9 /* Default User Value */, sText.c_str(), 160 /* Private */, sPrivate.c_str(), 161 /* Public */, sPublic.c_str(), 162 /* Caption */, sCaption.c_str());
		}
	};

	class SCREEN_FileSave_DT : public PreformedCommand
	{
	public:
		SCREEN_FileSave_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sText, string sPrivate, string sPublic, string sCaption)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */,
				9 /* Default User Value */, sText.c_str(), 160 /* Private */, sPrivate.c_str(), 161 /* Public */, sPublic.c_str(), 162 /* Caption */, sCaption.c_str());
		}
	};

	class SCREEN_FileSave_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileSave_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sText, string sPrivate, string sPublic, string sCaption)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "19" /* screen ID */,
				9 /* Default User Value */, sText.c_str(), 160 /* Private */, sPrivate.c_str(), 161 /* Public */, sPublic.c_str(), 162 /* Caption */, sCaption.c_str());
		}
	};

	class SCREEN_ManagePlaylist : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */);
		}
	};

	class SCREEN_ManagePlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */);
		}
	};

	class SCREEN_ManagePlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */);
		}
	};

	class SCREEN_ManagePlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_ManagePlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "20" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDevice : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDevice_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDevice_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */);
		}
	};

	class SCREEN_MakeCallDevice_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallDevice_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "21" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook2 : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook2_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook2_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */);
		}
	};

	class SCREEN_MakeCallPhonebook2_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallPhonebook2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "22" /* screen ID */);
		}
	};

	class SCREEN_MakeCallIntercom : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */);
		}
	};

	class SCREEN_MakeCallIntercom_DL : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */);
		}
	};

	class SCREEN_MakeCallIntercom_DT : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */);
		}
	};

	class SCREEN_MakeCallIntercom_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakeCallIntercom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "23" /* screen ID */);
		}
	};

	class SCREEN_CurrentUser : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */);
		}
	};

	class SCREEN_CurrentUser_DL : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */);
		}
	};

	class SCREEN_CurrentUser_DT : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */);
		}
	};

	class SCREEN_CurrentUser_Cat : public PreformedCommand
	{
	public:
		SCREEN_CurrentUser_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "24" /* screen ID */);
		}
	};

	class SCREEN_CDTrackCopy : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Users)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str());
		}
	};

	class SCREEN_CDTrackCopy_DL : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Users)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str());
		}
	};

	class SCREEN_CDTrackCopy_DT : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Users)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str());
		}
	};

	class SCREEN_CDTrackCopy_Cat : public PreformedCommand
	{
	public:
		SCREEN_CDTrackCopy_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Users)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "25" /* screen ID */,
				17 /* PK Users */, StringUtils::ltos(iPK_Users).c_str());
		}
	};

	class SCREEN_CDRemote : public PreformedCommand
	{
	public:
		SCREEN_CDRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */);
		}
	};

	class SCREEN_CDRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_CDRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */);
		}
	};

	class SCREEN_CDRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_CDRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */);
		}
	};

	class SCREEN_CDRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_CDRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "26" /* screen ID */);
		}
	};

	class SCREEN_VoicemailMain : public PreformedCommand
	{
	public:
		SCREEN_VoicemailMain(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */);
		}
	};

	class SCREEN_VoicemailMain_DL : public PreformedCommand
	{
	public:
		SCREEN_VoicemailMain_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */);
		}
	};

	class SCREEN_VoicemailMain_DT : public PreformedCommand
	{
	public:
		SCREEN_VoicemailMain_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */);
		}
	};

	class SCREEN_VoicemailMain_Cat : public PreformedCommand
	{
	public:
		SCREEN_VoicemailMain_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "27" /* screen ID */);
		}
	};

	class SCREEN_TransferAllUsers : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */);
		}
	};

	class SCREEN_TransferAllUsers_DL : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */);
		}
	};

	class SCREEN_TransferAllUsers_DT : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */);
		}
	};

	class SCREEN_TransferAllUsers_Cat : public PreformedCommand
	{
	public:
		SCREEN_TransferAllUsers_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "28" /* screen ID */);
		}
	};

	class SCREEN_Sleeping : public PreformedCommand
	{
	public:
		SCREEN_Sleeping(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */);
		}
	};

	class SCREEN_Sleeping_DL : public PreformedCommand
	{
	public:
		SCREEN_Sleeping_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */);
		}
	};

	class SCREEN_Sleeping_DT : public PreformedCommand
	{
	public:
		SCREEN_Sleeping_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */);
		}
	};

	class SCREEN_Sleeping_Cat : public PreformedCommand
	{
	public:
		SCREEN_Sleeping_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "29" /* screen ID */);
		}
	};

	class SCREEN_ControllerStatus : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */);
		}
	};

	class SCREEN_ControllerStatus_DL : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */);
		}
	};

	class SCREEN_ControllerStatus_DT : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */);
		}
	};

	class SCREEN_ControllerStatus_Cat : public PreformedCommand
	{
	public:
		SCREEN_ControllerStatus_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "30" /* screen ID */);
		}
	};

	class SCREEN_OtherFolders : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */);
		}
	};

	class SCREEN_OtherFolders_DL : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */);
		}
	};

	class SCREEN_OtherFolders_DT : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */);
		}
	};

	class SCREEN_OtherFolders_Cat : public PreformedCommand
	{
	public:
		SCREEN_OtherFolders_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "31" /* screen ID */);
		}
	};

	class SCREEN_ConfirmDelete : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */);
		}
	};

	class SCREEN_ConfirmDelete_DL : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */);
		}
	};

	class SCREEN_ConfirmDelete_DT : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */);
		}
	};

	class SCREEN_ConfirmDelete_Cat : public PreformedCommand
	{
	public:
		SCREEN_ConfirmDelete_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "32" /* screen ID */);
		}
	};

	class SCREEN_WebURL : public PreformedCommand
	{
	public:
		SCREEN_WebURL(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */);
		}
	};

	class SCREEN_WebURL_DL : public PreformedCommand
	{
	public:
		SCREEN_WebURL_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */);
		}
	};

	class SCREEN_WebURL_DT : public PreformedCommand
	{
	public:
		SCREEN_WebURL_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */);
		}
	};

	class SCREEN_WebURL_Cat : public PreformedCommand
	{
	public:
		SCREEN_WebURL_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "33" /* screen ID */);
		}
	};

	class SCREEN_Browser : public PreformedCommand
	{
	public:
		SCREEN_Browser(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */);
		}
	};

	class SCREEN_Browser_DL : public PreformedCommand
	{
	public:
		SCREEN_Browser_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */);
		}
	};

	class SCREEN_Browser_DT : public PreformedCommand
	{
	public:
		SCREEN_Browser_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */);
		}
	};

	class SCREEN_Browser_Cat : public PreformedCommand
	{
	public:
		SCREEN_Browser_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "34" /* screen ID */);
		}
	};

	class SCREEN_ModeChanged : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged(long DeviceIDFrom, long DeviceIDTo,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_ModeChanged_DL : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_ModeChanged_DT : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_ModeChanged_Cat : public PreformedCommand
	{
	public:
		SCREEN_ModeChanged_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "35" /* screen ID */,
				165 /* PK_HouseMode */, sPK_HouseMode.c_str(), 166 /* HouseModeTime */, sHouseModeTime.c_str(), 167 /* ExitDelay */, sExitDelay.c_str(), 168 /* Alerts */, sAlerts.c_str());
		}
	};

	class SCREEN_SensorsNotReady : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady(long DeviceIDFrom, long DeviceIDTo,
			string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_SensorsNotReady_DL : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_SensorsNotReady_DT : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_SensorsNotReady_Cat : public PreformedCommand
	{
	public:
		SCREEN_SensorsNotReady_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "36" /* screen ID */,
				163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_RecordedTVShows : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */);
		}
	};

	class SCREEN_RecordedTVShows_DL : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */);
		}
	};

	class SCREEN_RecordedTVShows_DT : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */);
		}
	};

	class SCREEN_RecordedTVShows_Cat : public PreformedCommand
	{
	public:
		SCREEN_RecordedTVShows_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "37" /* screen ID */);
		}
	};

	class SCREEN_DeviceControl : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */);
		}
	};

	class SCREEN_DeviceControl_DL : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */);
		}
	};

	class SCREEN_DeviceControl_DT : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */);
		}
	};

	class SCREEN_DeviceControl_Cat : public PreformedCommand
	{
	public:
		SCREEN_DeviceControl_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "38" /* screen ID */);
		}
	};

	class SCREEN_Computing : public PreformedCommand
	{
	public:
		SCREEN_Computing(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */);
		}
	};

	class SCREEN_Computing_DL : public PreformedCommand
	{
	public:
		SCREEN_Computing_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */);
		}
	};

	class SCREEN_Computing_DT : public PreformedCommand
	{
	public:
		SCREEN_Computing_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */);
		}
	};

	class SCREEN_Computing_Cat : public PreformedCommand
	{
	public:
		SCREEN_Computing_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "39" /* screen ID */);
		}
	};

	class SCREEN_OpenFile : public PreformedCommand
	{
	public:
		SCREEN_OpenFile(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */);
		}
	};

	class SCREEN_OpenFile_DL : public PreformedCommand
	{
	public:
		SCREEN_OpenFile_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */);
		}
	};

	class SCREEN_OpenFile_DT : public PreformedCommand
	{
	public:
		SCREEN_OpenFile_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */);
		}
	};

	class SCREEN_OpenFile_Cat : public PreformedCommand
	{
	public:
		SCREEN_OpenFile_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "40" /* screen ID */);
		}
	};

	class SCREEN_BrowserConsole : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */);
		}
	};

	class SCREEN_BrowserConsole_DL : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */);
		}
	};

	class SCREEN_BrowserConsole_DT : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */);
		}
	};

	class SCREEN_BrowserConsole_Cat : public PreformedCommand
	{
	public:
		SCREEN_BrowserConsole_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "41" /* screen ID */);
		}
	};

	class SCREEN_FavoritesPDA : public PreformedCommand
	{
	public:
		SCREEN_FavoritesPDA(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */);
		}
	};

	class SCREEN_FavoritesPDA_DL : public PreformedCommand
	{
	public:
		SCREEN_FavoritesPDA_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */);
		}
	};

	class SCREEN_FavoritesPDA_DT : public PreformedCommand
	{
	public:
		SCREEN_FavoritesPDA_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */);
		}
	};

	class SCREEN_FavoritesPDA_Cat : public PreformedCommand
	{
	public:
		SCREEN_FavoritesPDA_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "42" /* screen ID */);
		}
	};

	class SCREEN_PCFileTools : public PreformedCommand
	{
	public:
		SCREEN_PCFileTools(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */);
		}
	};

	class SCREEN_PCFileTools_DL : public PreformedCommand
	{
	public:
		SCREEN_PCFileTools_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */);
		}
	};

	class SCREEN_PCFileTools_DT : public PreformedCommand
	{
	public:
		SCREEN_PCFileTools_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */);
		}
	};

	class SCREEN_PCFileTools_Cat : public PreformedCommand
	{
	public:
		SCREEN_PCFileTools_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "43" /* screen ID */);
		}
	};

	class SCREEN_AdvancedOptions : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */);
		}
	};

	class SCREEN_AdvancedOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */);
		}
	};

	class SCREEN_AdvancedOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */);
		}
	};

	class SCREEN_AdvancedOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdvancedOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "44" /* screen ID */);
		}
	};

	class SCREEN_MakingCall : public PreformedCommand
	{
	public:
		SCREEN_MakingCall(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */);
		}
	};

	class SCREEN_MakingCall_DL : public PreformedCommand
	{
	public:
		SCREEN_MakingCall_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */);
		}
	};

	class SCREEN_MakingCall_DT : public PreformedCommand
	{
	public:
		SCREEN_MakingCall_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */);
		}
	};

	class SCREEN_MakingCall_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakingCall_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "45" /* screen ID */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast_DL : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast_DT : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */);
		}
	};

	class SCREEN_MakingLinPhoneBroadcast_Cat : public PreformedCommand
	{
	public:
		SCREEN_MakingLinPhoneBroadcast_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "46" /* screen ID */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */);
		}
	};

	class SCREEN_FileList_Music_Movies_Video_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_Music_Movies_Video_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "47" /* screen ID */);
		}
	};

	class SCREEN_PVR : public PreformedCommand
	{
	public:
		SCREEN_PVR(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */);
		}
	};

	class SCREEN_PVR_DL : public PreformedCommand
	{
	public:
		SCREEN_PVR_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */);
		}
	};

	class SCREEN_PVR_DT : public PreformedCommand
	{
	public:
		SCREEN_PVR_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */);
		}
	};

	class SCREEN_PVR_Cat : public PreformedCommand
	{
	public:
		SCREEN_PVR_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "48" /* screen ID */);
		}
	};

	class SCREEN_DVDRemote : public PreformedCommand
	{
	public:
		SCREEN_DVDRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */);
		}
	};

	class SCREEN_DVDRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */);
		}
	};

	class SCREEN_DVDRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */);
		}
	};

	class SCREEN_DVDRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "49" /* screen ID */);
		}
	};

	class SCREEN_DVDMenu : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */);
		}
	};

	class SCREEN_DVDMenu_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */);
		}
	};

	class SCREEN_DVDMenu_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */);
		}
	};

	class SCREEN_DVDMenu_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDMenu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "50" /* screen ID */);
		}
	};

	class SCREEN_MapMedia : public PreformedCommand
	{
	public:
		SCREEN_MapMedia(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */);
		}
	};

	class SCREEN_MapMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_MapMedia_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */);
		}
	};

	class SCREEN_MapMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_MapMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */);
		}
	};

	class SCREEN_MapMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "51" /* screen ID */);
		}
	};

	class SCREEN_AdvancedMediaOptions : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */);
		}
	};

	class SCREEN_AdvancedMediaOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */);
		}
	};

	class SCREEN_AdvancedMediaOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */);
		}
	};

	class SCREEN_AdvancedMediaOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdvancedMediaOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "52" /* screen ID */);
		}
	};

	class SCREEN_PopupMessage : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */);
		}
	};

	class SCREEN_PopupMessage_DL : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */);
		}
	};

	class SCREEN_PopupMessage_DT : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */);
		}
	};

	class SCREEN_PopupMessage_Cat : public PreformedCommand
	{
	public:
		SCREEN_PopupMessage_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "53" /* screen ID */);
		}
	};

	class SCREEN_MusicRemote : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */);
		}
	};

	class SCREEN_MusicRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */);
		}
	};

	class SCREEN_MusicRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */);
		}
	};

	class SCREEN_MusicRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_MusicRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "54" /* screen ID */);
		}
	};

	class SCREEN_LinPhones : public PreformedCommand
	{
	public:
		SCREEN_LinPhones(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */);
		}
	};

	class SCREEN_LinPhones_DL : public PreformedCommand
	{
	public:
		SCREEN_LinPhones_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */);
		}
	};

	class SCREEN_LinPhones_DT : public PreformedCommand
	{
	public:
		SCREEN_LinPhones_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */);
		}
	};

	class SCREEN_LinPhones_Cat : public PreformedCommand
	{
	public:
		SCREEN_LinPhones_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "55" /* screen ID */);
		}
	};

	class SCREEN_Orbiters : public PreformedCommand
	{
	public:
		SCREEN_Orbiters(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */);
		}
	};

	class SCREEN_Orbiters_DL : public PreformedCommand
	{
	public:
		SCREEN_Orbiters_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */);
		}
	};

	class SCREEN_Orbiters_DT : public PreformedCommand
	{
	public:
		SCREEN_Orbiters_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */);
		}
	};

	class SCREEN_Orbiters_Cat : public PreformedCommand
	{
	public:
		SCREEN_Orbiters_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "56" /* screen ID */);
		}
	};

	class SCREEN_Phones : public PreformedCommand
	{
	public:
		SCREEN_Phones(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */);
		}
	};

	class SCREEN_Phones_DL : public PreformedCommand
	{
	public:
		SCREEN_Phones_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */);
		}
	};

	class SCREEN_Phones_DT : public PreformedCommand
	{
	public:
		SCREEN_Phones_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */);
		}
	};

	class SCREEN_Phones_Cat : public PreformedCommand
	{
	public:
		SCREEN_Phones_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "57" /* screen ID */);
		}
	};

	class SCREEN_DVDOptions : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */);
		}
	};

	class SCREEN_DVDOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */);
		}
	};

	class SCREEN_DVDOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */);
		}
	};

	class SCREEN_DVDOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "58" /* screen ID */);
		}
	};

	class SCREEN_AnswertoLinPhones : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */);
		}
	};

	class SCREEN_AnswertoLinPhones_DL : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */);
		}
	};

	class SCREEN_AnswertoLinPhones_DT : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */);
		}
	};

	class SCREEN_AnswertoLinPhones_Cat : public PreformedCommand
	{
	public:
		SCREEN_AnswertoLinPhones_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "59" /* screen ID */);
		}
	};

	class SCREEN_UserPinCode : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */);
		}
	};

	class SCREEN_UserPinCode_DL : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */);
		}
	};

	class SCREEN_UserPinCode_DT : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */);
		}
	};

	class SCREEN_UserPinCode_Cat : public PreformedCommand
	{
	public:
		SCREEN_UserPinCode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "60" /* screen ID */);
		}
	};

	class SCREEN_PreviewOptions : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */);
		}
	};

	class SCREEN_PreviewOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */);
		}
	};

	class SCREEN_PreviewOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */);
		}
	};

	class SCREEN_PreviewOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_PreviewOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "61" /* screen ID */);
		}
	};

	class SCREEN_TransferOutside : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */);
		}
	};

	class SCREEN_TransferOutside_DL : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */);
		}
	};

	class SCREEN_TransferOutside_DT : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */);
		}
	};

	class SCREEN_TransferOutside_Cat : public PreformedCommand
	{
	public:
		SCREEN_TransferOutside_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "62" /* screen ID */);
		}
	};

	class SCREEN_SatelliteCableBox : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */);
		}
	};

	class SCREEN_SatelliteCableBox_DL : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */);
		}
	};

	class SCREEN_SatelliteCableBox_DT : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */);
		}
	};

	class SCREEN_SatelliteCableBox_Cat : public PreformedCommand
	{
	public:
		SCREEN_SatelliteCableBox_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "63" /* screen ID */);
		}
	};

	class SCREEN_CurrentLocation : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */);
		}
	};

	class SCREEN_CurrentLocation_DL : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */);
		}
	};

	class SCREEN_CurrentLocation_DT : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */);
		}
	};

	class SCREEN_CurrentLocation_Cat : public PreformedCommand
	{
	public:
		SCREEN_CurrentLocation_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "64" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchInput : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchInput_DL : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchInput_DT : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchInput_Cat : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchInput_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "65" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchResult : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchResult_DL : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchResult_DT : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */);
		}
	};

	class SCREEN_MediaAttributeSearchResult_Cat : public PreformedCommand
	{
	public:
		SCREEN_MediaAttributeSearchResult_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "66" /* screen ID */);
		}
	};

	class SCREEN_NewPhoneDetected : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected(long DeviceIDFrom, long DeviceIDTo,
			string sMac_address, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_NewPhoneDetected_DL : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sMac_address, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_NewPhoneDetected_DT : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sMac_address, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_NewPhoneDetected_Cat : public PreformedCommand
	{
	public:
		SCREEN_NewPhoneDetected_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sMac_address, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "67" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 163 /* Description */, sDescription.c_str());
		}
	};

	class SCREEN_FirstTime : public PreformedCommand
	{
	public:
		SCREEN_FirstTime(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */);
		}
	};

	class SCREEN_FirstTime_DL : public PreformedCommand
	{
	public:
		SCREEN_FirstTime_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */);
		}
	};

	class SCREEN_FirstTime_DT : public PreformedCommand
	{
	public:
		SCREEN_FirstTime_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */);
		}
	};

	class SCREEN_FirstTime_Cat : public PreformedCommand
	{
	public:
		SCREEN_FirstTime_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "68" /* screen ID */);
		}
	};

	class SCREEN_FileList_PlayLists : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */);
		}
	};

	class SCREEN_FileList_PlayLists_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */);
		}
	};

	class SCREEN_FileList_PlayLists_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */);
		}
	};

	class SCREEN_FileList_PlayLists_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_PlayLists_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "69" /* screen ID */);
		}
	};

	class SCREEN_VideosRemote : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */);
		}
	};

	class SCREEN_VideosRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */);
		}
	};

	class SCREEN_VideosRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */);
		}
	};

	class SCREEN_VideosRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideosRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "70" /* screen ID */);
		}
	};

	class SCREEN_FileList_Pictures_Docs : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */);
		}
	};

	class SCREEN_FileList_Pictures_Docs_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */);
		}
	};

	class SCREEN_FileList_Pictures_Docs_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */);
		}
	};

	class SCREEN_FileList_Pictures_Docs_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_Pictures_Docs_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "71" /* screen ID */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */);
		}
	};

	class SCREEN_GenericTwoButtonQuestion_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericTwoButtonQuestion_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "72" /* screen ID */);
		}
	};

	class SCREEN_TryCodes : public PreformedCommand
	{
	public:
		SCREEN_TryCodes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */);
		}
	};

	class SCREEN_TryCodes_DL : public PreformedCommand
	{
	public:
		SCREEN_TryCodes_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */);
		}
	};

	class SCREEN_TryCodes_DT : public PreformedCommand
	{
	public:
		SCREEN_TryCodes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */);
		}
	};

	class SCREEN_TryCodes_Cat : public PreformedCommand
	{
	public:
		SCREEN_TryCodes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "73" /* screen ID */);
		}
	};

	class SCREEN_ScreenSaver : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */);
		}
	};

	class SCREEN_ScreenSaver_DL : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */);
		}
	};

	class SCREEN_ScreenSaver_DT : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */);
		}
	};

	class SCREEN_ScreenSaver_Cat : public PreformedCommand
	{
	public:
		SCREEN_ScreenSaver_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "74" /* screen ID */);
		}
	};

	class SCREEN_NewPlugAndPlayDevice : public PreformedCommand
	{
	public:
		SCREEN_NewPlugAndPlayDevice(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device, string sDescription, string ssComments)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_NewPlugAndPlayDevice_DL : public PreformedCommand
	{
	public:
		SCREEN_NewPlugAndPlayDevice_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device, string sDescription, string ssComments)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_NewPlugAndPlayDevice_DT : public PreformedCommand
	{
	public:
		SCREEN_NewPlugAndPlayDevice_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device, string sDescription, string ssComments)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_NewPlugAndPlayDevice_Cat : public PreformedCommand
	{
	public:
		SCREEN_NewPlugAndPlayDevice_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device, string sDescription, string ssComments)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "75" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 163 /* Description */, sDescription.c_str(), 164 /* Comments */, ssComments.c_str());
		}
	};

	class SCREEN_Power : public PreformedCommand
	{
	public:
		SCREEN_Power(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */);
		}
	};

	class SCREEN_Power_DL : public PreformedCommand
	{
	public:
		SCREEN_Power_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */);
		}
	};

	class SCREEN_Power_DT : public PreformedCommand
	{
	public:
		SCREEN_Power_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */);
		}
	};

	class SCREEN_Power_Cat : public PreformedCommand
	{
	public:
		SCREEN_Power_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "76" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdRemote : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "77" /* screen ID */);
		}
	};

	class SCREEN_ViewPlaylist : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */);
		}
	};

	class SCREEN_ViewPlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */);
		}
	};

	class SCREEN_ViewPlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */);
		}
	};

	class SCREEN_ViewPlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_ViewPlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "78" /* screen ID */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */);
		}
	};

	class SCREEN_FileList_withPictures_Mobil_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_withPictures_Mobil_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "79" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdMenu : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdMenu_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdMenu_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */);
		}
	};

	class SCREEN_GenericDvdMenu_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericDvdMenu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "80" /* screen ID */);
		}
	};

	class SCREEN_ViewAttributes : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */);
		}
	};

	class SCREEN_ViewAttributes_DL : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */);
		}
	};

	class SCREEN_ViewAttributes_DT : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */);
		}
	};

	class SCREEN_ViewAttributes_Cat : public PreformedCommand
	{
	public:
		SCREEN_ViewAttributes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "81" /* screen ID */);
		}
	};

	class SCREEN_PendingTasks : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */);
		}
	};

	class SCREEN_PendingTasks_DL : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */);
		}
	};

	class SCREEN_PendingTasks_DT : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */);
		}
	};

	class SCREEN_PendingTasks_Cat : public PreformedCommand
	{
	public:
		SCREEN_PendingTasks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "82" /* screen ID */);
		}
	};

	class SCREEN_AddAttribute : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */);
		}
	};

	class SCREEN_AddAttribute_DL : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */);
		}
	};

	class SCREEN_AddAttribute_DT : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */);
		}
	};

	class SCREEN_AddAttribute_Cat : public PreformedCommand
	{
	public:
		SCREEN_AddAttribute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "83" /* screen ID */);
		}
	};

	class SCREEN_RadioRemote : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */);
		}
	};

	class SCREEN_RadioRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */);
		}
	};

	class SCREEN_RadioRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */);
		}
	};

	class SCREEN_RadioRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_RadioRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "84" /* screen ID */);
		}
	};

	class SCREEN_GenericCDRemote : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */);
		}
	};

	class SCREEN_GenericCDRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */);
		}
	};

	class SCREEN_GenericCDRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */);
		}
	};

	class SCREEN_GenericCDRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericCDRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "85" /* screen ID */);
		}
	};

	class SCREEN_FileList_Popup_Container : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */);
		}
	};

	class SCREEN_FileList_Popup_Container_DL : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */);
		}
	};

	class SCREEN_FileList_Popup_Container_DT : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */);
		}
	};

	class SCREEN_FileList_Popup_Container_Cat : public PreformedCommand
	{
	public:
		SCREEN_FileList_Popup_Container_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "86" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarks : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarks_DL : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarks_DT : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarks_Cat : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "87" /* screen ID */);
		}
	};

	class SCREEN_BrowseBookmarks : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */);
		}
	};

	class SCREEN_BrowseBookmarks_DL : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */);
		}
	};

	class SCREEN_BrowseBookmarks_DT : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */);
		}
	};

	class SCREEN_BrowseBookmarks_Cat : public PreformedCommand
	{
	public:
		SCREEN_BrowseBookmarks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "88" /* screen ID */);
		}
	};

	class SCREEN_GenericAppController : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */);
		}
	};

	class SCREEN_GenericAppController_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */);
		}
	};

	class SCREEN_GenericAppController_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */);
		}
	};

	class SCREEN_GenericAppController_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericAppController_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "89" /* screen ID */);
		}
	};

	class SCREEN_BookmarksByMediaType : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */);
		}
	};

	class SCREEN_BookmarksByMediaType_DL : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */);
		}
	};

	class SCREEN_BookmarksByMediaType_DT : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */);
		}
	};

	class SCREEN_BookmarksByMediaType_Cat : public PreformedCommand
	{
	public:
		SCREEN_BookmarksByMediaType_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "90" /* screen ID */);
		}
	};

	class SCREEN_MythTvRemote : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */);
		}
	};

	class SCREEN_MythTvRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */);
		}
	};

	class SCREEN_MythTvRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */);
		}
	};

	class SCREEN_MythTvRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_MythTvRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "92" /* screen ID */);
		}
	};

	class SCREEN_Myth_all_commands : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */);
		}
	};

	class SCREEN_Myth_all_commands_DL : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */);
		}
	};

	class SCREEN_Myth_all_commands_DT : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */);
		}
	};

	class SCREEN_Myth_all_commands_Cat : public PreformedCommand
	{
	public:
		SCREEN_Myth_all_commands_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "93" /* screen ID */);
		}
	};

	class SCREEN_Mytv_watching_tv : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */);
		}
	};

	class SCREEN_Mytv_watching_tv_DL : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */);
		}
	};

	class SCREEN_Mytv_watching_tv_DT : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */);
		}
	};

	class SCREEN_Mytv_watching_tv_Cat : public PreformedCommand
	{
	public:
		SCREEN_Mytv_watching_tv_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "94" /* screen ID */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode_DL : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode_DT : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */);
		}
	};

	class SCREEN_Myth_live_tv_browse_mode_Cat : public PreformedCommand
	{
	public:
		SCREEN_Myth_live_tv_browse_mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "95" /* screen ID */);
		}
	};

	class SCREEN_Myth_playback_recording : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */);
		}
	};

	class SCREEN_Myth_playback_recording_DL : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */);
		}
	};

	class SCREEN_Myth_playback_recording_DT : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */);
		}
	};

	class SCREEN_Myth_playback_recording_Cat : public PreformedCommand
	{
	public:
		SCREEN_Myth_playback_recording_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "96" /* screen ID */);
		}
	};

	class SCREEN_GenericKeyboard : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */);
		}
	};

	class SCREEN_GenericKeyboard_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */);
		}
	};

	class SCREEN_GenericKeyboard_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */);
		}
	};

	class SCREEN_GenericKeyboard_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericKeyboard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "97" /* screen ID */);
		}
	};

	class SCREEN_PingPong : public PreformedCommand
	{
	public:
		SCREEN_PingPong(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */);
		}
	};

	class SCREEN_PingPong_DL : public PreformedCommand
	{
	public:
		SCREEN_PingPong_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */);
		}
	};

	class SCREEN_PingPong_DT : public PreformedCommand
	{
	public:
		SCREEN_PingPong_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */);
		}
	};

	class SCREEN_PingPong_Cat : public PreformedCommand
	{
	public:
		SCREEN_PingPong_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "98" /* screen ID */);
		}
	};

	class SCREEN_TVEPG1 : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */);
		}
	};

	class SCREEN_TVEPG1_DL : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */);
		}
	};

	class SCREEN_TVEPG1_DT : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */);
		}
	};

	class SCREEN_TVEPG1_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVEPG1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "99" /* screen ID */);
		}
	};

	class SCREEN_TVEPG2 : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */);
		}
	};

	class SCREEN_TVEPG2_DL : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */);
		}
	};

	class SCREEN_TVEPG2_DT : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */);
		}
	};

	class SCREEN_TVEPG2_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVEPG2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "100" /* screen ID */);
		}
	};

	class SCREEN_TVEPG3 : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */);
		}
	};

	class SCREEN_TVEPG3_DL : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */);
		}
	};

	class SCREEN_TVEPG3_DT : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */);
		}
	};

	class SCREEN_TVEPG3_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVEPG3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "101" /* screen ID */);
		}
	};

	class SCREEN_ShowInfo : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */);
		}
	};

	class SCREEN_ShowInfo_DL : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */);
		}
	};

	class SCREEN_ShowInfo_DT : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */);
		}
	};

	class SCREEN_ShowInfo_Cat : public PreformedCommand
	{
	public:
		SCREEN_ShowInfo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "102" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarksTV : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarksTV_DL : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarksTV_DT : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */);
		}
	};

	class SCREEN_CreateViewBookmarksTV_Cat : public PreformedCommand
	{
	public:
		SCREEN_CreateViewBookmarksTV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "103" /* screen ID */);
		}
	};

	class SCREEN_WebSites : public PreformedCommand
	{
	public:
		SCREEN_WebSites(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */);
		}
	};

	class SCREEN_WebSites_DL : public PreformedCommand
	{
	public:
		SCREEN_WebSites_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */);
		}
	};

	class SCREEN_WebSites_DT : public PreformedCommand
	{
	public:
		SCREEN_WebSites_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */);
		}
	};

	class SCREEN_WebSites_Cat : public PreformedCommand
	{
	public:
		SCREEN_WebSites_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "105" /* screen ID */);
		}
	};

	class SCREEN_WhatModelMobileOrbiter : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter(long DeviceIDFrom, long DeviceIDTo,
			string sMac_address)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_WhatModelMobileOrbiter_DL : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sMac_address)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_WhatModelMobileOrbiter_DT : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sMac_address)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_WhatModelMobileOrbiter_Cat : public PreformedCommand
	{
	public:
		SCREEN_WhatModelMobileOrbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sMac_address)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "106" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str());
		}
	};

	class SCREEN_Filelist : public PreformedCommand
	{
	public:
		SCREEN_Filelist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */);
		}
	};

	class SCREEN_Filelist_DL : public PreformedCommand
	{
	public:
		SCREEN_Filelist_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */);
		}
	};

	class SCREEN_Filelist_DT : public PreformedCommand
	{
	public:
		SCREEN_Filelist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */);
		}
	};

	class SCREEN_Filelist_Cat : public PreformedCommand
	{
	public:
		SCREEN_Filelist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "107" /* screen ID */);
		}
	};

	class SCREEN_DVDmenu : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */);
		}
	};

	class SCREEN_DVDmenu_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */);
		}
	};

	class SCREEN_DVDmenu_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */);
		}
	};

	class SCREEN_DVDmenu_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDmenu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "108" /* screen ID */);
		}
	};

	class SCREEN_Attributes : public PreformedCommand
	{
	public:
		SCREEN_Attributes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */);
		}
	};

	class SCREEN_Attributes_DL : public PreformedCommand
	{
	public:
		SCREEN_Attributes_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */);
		}
	};

	class SCREEN_Attributes_DT : public PreformedCommand
	{
	public:
		SCREEN_Attributes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */);
		}
	};

	class SCREEN_Attributes_Cat : public PreformedCommand
	{
	public:
		SCREEN_Attributes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "109" /* screen ID */);
		}
	};

	class SCREEN_DVDoptions : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */);
		}
	};

	class SCREEN_DVDoptions_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */);
		}
	};

	class SCREEN_DVDoptions_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */);
		}
	};

	class SCREEN_DVDoptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDoptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "110" /* screen ID */);
		}
	};

	class SCREEN_AdvRemote : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */);
		}
	};

	class SCREEN_AdvRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */);
		}
	};

	class SCREEN_AdvRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */);
		}
	};

	class SCREEN_AdvRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_AdvRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "111" /* screen ID */);
		}
	};

	class SCREEN_Playlist : public PreformedCommand
	{
	public:
		SCREEN_Playlist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */);
		}
	};

	class SCREEN_Playlist_DL : public PreformedCommand
	{
	public:
		SCREEN_Playlist_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */);
		}
	};

	class SCREEN_Playlist_DT : public PreformedCommand
	{
	public:
		SCREEN_Playlist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */);
		}
	};

	class SCREEN_Playlist_Cat : public PreformedCommand
	{
	public:
		SCREEN_Playlist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "112" /* screen ID */);
		}
	};

	class SCREEN_AudioOptions : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */);
		}
	};

	class SCREEN_AudioOptions_DL : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */);
		}
	};

	class SCREEN_AudioOptions_DT : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */);
		}
	};

	class SCREEN_AudioOptions_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudioOptions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "113" /* screen ID */);
		}
	};

	class SCREEN_Subtitles : public PreformedCommand
	{
	public:
		SCREEN_Subtitles(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */);
		}
	};

	class SCREEN_Subtitles_DL : public PreformedCommand
	{
	public:
		SCREEN_Subtitles_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */);
		}
	};

	class SCREEN_Subtitles_DT : public PreformedCommand
	{
	public:
		SCREEN_Subtitles_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */);
		}
	};

	class SCREEN_Subtitles_Cat : public PreformedCommand
	{
	public:
		SCREEN_Subtitles_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "114" /* screen ID */);
		}
	};

	class SCREEN_Angles : public PreformedCommand
	{
	public:
		SCREEN_Angles(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */);
		}
	};

	class SCREEN_Angles_DL : public PreformedCommand
	{
	public:
		SCREEN_Angles_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */);
		}
	};

	class SCREEN_Angles_DT : public PreformedCommand
	{
	public:
		SCREEN_Angles_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */);
		}
	};

	class SCREEN_Angles_Cat : public PreformedCommand
	{
	public:
		SCREEN_Angles_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "115" /* screen ID */);
		}
	};

	class SCREEN_NewPnpDevice : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPnpDevice_DL : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPnpDevice_DT : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NewPnpDevice_Cat : public PreformedCommand
	{
	public:
		SCREEN_NewPnpDevice_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "116" /* screen ID */,
				163 /* What to call this for the user */, sDescription.c_str(), 224 /* The pnp queue event */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_VdrRemote : public PreformedCommand
	{
	public:
		SCREEN_VdrRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */);
		}
	};

	class SCREEN_VdrRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_VdrRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */);
		}
	};

	class SCREEN_VdrRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_VdrRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */);
		}
	};

	class SCREEN_VdrRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_VdrRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "117" /* screen ID */);
		}
	};

	class SCREEN_CdRemote : public PreformedCommand
	{
	public:
		SCREEN_CdRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */);
		}
	};

	class SCREEN_CdRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_CdRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */);
		}
	};

	class SCREEN_CdRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_CdRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */);
		}
	};

	class SCREEN_CdRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_CdRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "118" /* screen ID */);
		}
	};

	class SCREEN_PowerFile : public PreformedCommand
	{
	public:
		SCREEN_PowerFile(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */);
		}
	};

	class SCREEN_PowerFile_DL : public PreformedCommand
	{
	public:
		SCREEN_PowerFile_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */);
		}
	};

	class SCREEN_PowerFile_DT : public PreformedCommand
	{
	public:
		SCREEN_PowerFile_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */);
		}
	};

	class SCREEN_PowerFile_Cat : public PreformedCommand
	{
	public:
		SCREEN_PowerFile_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "119" /* screen ID */);
		}
	};

	class SCREEN_DevCallInProgress : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */);
		}
	};

	class SCREEN_DevCallInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */);
		}
	};

	class SCREEN_DevCallInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */);
		}
	};

	class SCREEN_DevCallInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_DevCallInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "120" /* screen ID */);
		}
	};

	class SCREEN_DevIncomingCall : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */);
		}
	};

	class SCREEN_DevIncomingCall_DL : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */);
		}
	};

	class SCREEN_DevIncomingCall_DT : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */);
		}
	};

	class SCREEN_DevIncomingCall_Cat : public PreformedCommand
	{
	public:
		SCREEN_DevIncomingCall_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "121" /* screen ID */);
		}
	};

	class SCREEN_DevMoveCallsInProgress : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */);
		}
	};

	class SCREEN_DevMoveCallsInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */);
		}
	};

	class SCREEN_DevMoveCallsInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */);
		}
	};

	class SCREEN_DevMoveCallsInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_DevMoveCallsInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "122" /* screen ID */);
		}
	};

	class SCREEN_MapSecurity : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */);
		}
	};

	class SCREEN_MapSecurity_DL : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */);
		}
	};

	class SCREEN_MapSecurity_DT : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */);
		}
	};

	class SCREEN_MapSecurity_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapSecurity_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "123" /* screen ID */);
		}
	};

	class SCREEN_MapLighting : public PreformedCommand
	{
	public:
		SCREEN_MapLighting(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */);
		}
	};

	class SCREEN_MapLighting_DL : public PreformedCommand
	{
	public:
		SCREEN_MapLighting_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */);
		}
	};

	class SCREEN_MapLighting_DT : public PreformedCommand
	{
	public:
		SCREEN_MapLighting_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */);
		}
	};

	class SCREEN_MapLighting_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapLighting_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "124" /* screen ID */);
		}
	};

	class SCREEN_MapClimate : public PreformedCommand
	{
	public:
		SCREEN_MapClimate(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */);
		}
	};

	class SCREEN_MapClimate_DL : public PreformedCommand
	{
	public:
		SCREEN_MapClimate_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */);
		}
	};

	class SCREEN_MapClimate_DT : public PreformedCommand
	{
	public:
		SCREEN_MapClimate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */);
		}
	};

	class SCREEN_MapClimate_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapClimate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "125" /* screen ID */);
		}
	};

	class SCREEN_MapTelecom : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */);
		}
	};

	class SCREEN_MapTelecom_DL : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */);
		}
	};

	class SCREEN_MapTelecom_DT : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */);
		}
	};

	class SCREEN_MapTelecom_Cat : public PreformedCommand
	{
	public:
		SCREEN_MapTelecom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "126" /* screen ID */);
		}
	};

	class SCREEN_NavMode : public PreformedCommand
	{
	public:
		SCREEN_NavMode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */);
		}
	};

	class SCREEN_NavMode_DL : public PreformedCommand
	{
	public:
		SCREEN_NavMode_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */);
		}
	};

	class SCREEN_NavMode_DT : public PreformedCommand
	{
	public:
		SCREEN_NavMode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */);
		}
	};

	class SCREEN_NavMode_Cat : public PreformedCommand
	{
	public:
		SCREEN_NavMode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "127" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreen : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "128" /* screen ID */);
		}
	};

	class SCREEN_Epg : public PreformedCommand
	{
	public:
		SCREEN_Epg(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */);
		}
	};

	class SCREEN_Epg_DL : public PreformedCommand
	{
	public:
		SCREEN_Epg_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */);
		}
	};

	class SCREEN_Epg_DT : public PreformedCommand
	{
	public:
		SCREEN_Epg_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */);
		}
	};

	class SCREEN_Epg_Cat : public PreformedCommand
	{
	public:
		SCREEN_Epg_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "129" /* screen ID */);
		}
	};

	class SCREEN_Help01 : public PreformedCommand
	{
	public:
		SCREEN_Help01(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */);
		}
	};

	class SCREEN_Help01_DL : public PreformedCommand
	{
	public:
		SCREEN_Help01_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */);
		}
	};

	class SCREEN_Help01_DT : public PreformedCommand
	{
	public:
		SCREEN_Help01_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */);
		}
	};

	class SCREEN_Help01_Cat : public PreformedCommand
	{
	public:
		SCREEN_Help01_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "130" /* screen ID */);
		}
	};

	class SCREEN_Help02 : public PreformedCommand
	{
	public:
		SCREEN_Help02(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */);
		}
	};

	class SCREEN_Help02_DL : public PreformedCommand
	{
	public:
		SCREEN_Help02_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */);
		}
	};

	class SCREEN_Help02_DT : public PreformedCommand
	{
	public:
		SCREEN_Help02_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */);
		}
	};

	class SCREEN_Help02_Cat : public PreformedCommand
	{
	public:
		SCREEN_Help02_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "131" /* screen ID */);
		}
	};

	class SCREEN_Help03 : public PreformedCommand
	{
	public:
		SCREEN_Help03(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */);
		}
	};

	class SCREEN_Help03_DL : public PreformedCommand
	{
	public:
		SCREEN_Help03_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */);
		}
	};

	class SCREEN_Help03_DT : public PreformedCommand
	{
	public:
		SCREEN_Help03_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */);
		}
	};

	class SCREEN_Help03_Cat : public PreformedCommand
	{
	public:
		SCREEN_Help03_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "132" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv01 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv01_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv01_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv01_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv01_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "133" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv02 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv02_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv02_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv02_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv02_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "134" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv03 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv03_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv03_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv03_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv03_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "135" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv04 : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv04_DL : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv04_DT : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */);
		}
	};

	class SCREEN_SetupMythTv04_Cat : public PreformedCommand
	{
	public:
		SCREEN_SetupMythTv04_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "136" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreenOpt(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */);
		}
	};

	class SCREEN_PvrFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_PvrFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "137" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreen : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "138" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */);
		}
	};

	class SCREEN_DVDFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "139" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreen : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "140" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */);
		}
	};

	class SCREEN_VideosFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideosFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "141" /* screen ID */);
		}
	};

	class SCREEN_GenericAppFullScreen : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */);
		}
	};

	class SCREEN_GenericAppFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */);
		}
	};

	class SCREEN_GenericAppFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */);
		}
	};

	class SCREEN_GenericAppFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_GenericAppFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "142" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreen : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "143" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */);
		}
	};

	class SCREEN_DVDMenuFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_DVDMenuFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "144" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreen : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "145" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreenOpt : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreenOpt_DL : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreenOpt_DT : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */);
		}
	};

	class SCREEN_VlcFullScreenOpt_Cat : public PreformedCommand
	{
	public:
		SCREEN_VlcFullScreenOpt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "146" /* screen ID */);
		}
	};

	class SCREEN_popLights : public PreformedCommand
	{
	public:
		SCREEN_popLights(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popLights_DL : public PreformedCommand
	{
	public:
		SCREEN_popLights_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popLights_DT : public PreformedCommand
	{
	public:
		SCREEN_popLights_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popLights_Cat : public PreformedCommand
	{
	public:
		SCREEN_popLights_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "147" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popMedia : public PreformedCommand
	{
	public:
		SCREEN_popMedia(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_popMedia_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_popMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_popMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "148" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popClimate : public PreformedCommand
	{
	public:
		SCREEN_popClimate(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popClimate_DL : public PreformedCommand
	{
	public:
		SCREEN_popClimate_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popClimate_DT : public PreformedCommand
	{
	public:
		SCREEN_popClimate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popClimate_Cat : public PreformedCommand
	{
	public:
		SCREEN_popClimate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "149" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popTelecom : public PreformedCommand
	{
	public:
		SCREEN_popTelecom(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popTelecom_DL : public PreformedCommand
	{
	public:
		SCREEN_popTelecom_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popTelecom_DT : public PreformedCommand
	{
	public:
		SCREEN_popTelecom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popTelecom_Cat : public PreformedCommand
	{
	public:
		SCREEN_popTelecom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "150" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popSecurity : public PreformedCommand
	{
	public:
		SCREEN_popSecurity(long DeviceIDFrom, long DeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popSecurity_DL : public PreformedCommand
	{
	public:
		SCREEN_popSecurity_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popSecurity_DT : public PreformedCommand
	{
	public:
		SCREEN_popSecurity_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popSecurity_Cat : public PreformedCommand
	{
	public:
		SCREEN_popSecurity_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sLocation)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "151" /* screen ID */,
				195 /* Location */, sLocation.c_str());
		}
	};

	class SCREEN_popOthers : public PreformedCommand
	{
	public:
		SCREEN_popOthers(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */);
		}
	};

	class SCREEN_popOthers_DL : public PreformedCommand
	{
	public:
		SCREEN_popOthers_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */);
		}
	};

	class SCREEN_popOthers_DT : public PreformedCommand
	{
	public:
		SCREEN_popOthers_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */);
		}
	};

	class SCREEN_popOthers_Cat : public PreformedCommand
	{
	public:
		SCREEN_popOthers_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "152" /* screen ID */);
		}
	};

	class SCREEN_AudiCarSetup : public PreformedCommand
	{
	public:
		SCREEN_AudiCarSetup(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */);
		}
	};

	class SCREEN_AudiCarSetup_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiCarSetup_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */);
		}
	};

	class SCREEN_AudiCarSetup_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiCarSetup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */);
		}
	};

	class SCREEN_AudiCarSetup_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiCarSetup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "153" /* screen ID */);
		}
	};

	class SCREEN_AudiMedia : public PreformedCommand
	{
	public:
		SCREEN_AudiMedia(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */);
		}
	};

	class SCREEN_AudiMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiMedia_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */);
		}
	};

	class SCREEN_AudiMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */);
		}
	};

	class SCREEN_AudiMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "154" /* screen ID */);
		}
	};

	class SCREEN_AudiComputing : public PreformedCommand
	{
	public:
		SCREEN_AudiComputing(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */);
		}
	};

	class SCREEN_AudiComputing_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiComputing_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */);
		}
	};

	class SCREEN_AudiComputing_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiComputing_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */);
		}
	};

	class SCREEN_AudiComputing_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiComputing_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "155" /* screen ID */);
		}
	};

	class SCREEN_AudiRadio : public PreformedCommand
	{
	public:
		SCREEN_AudiRadio(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */);
		}
	};

	class SCREEN_AudiRadio_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiRadio_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */);
		}
	};

	class SCREEN_AudiRadio_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiRadio_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */);
		}
	};

	class SCREEN_AudiRadio_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiRadio_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "156" /* screen ID */);
		}
	};

	class SCREEN_AudiTV : public PreformedCommand
	{
	public:
		SCREEN_AudiTV(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */);
		}
	};

	class SCREEN_AudiTV_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiTV_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */);
		}
	};

	class SCREEN_AudiTV_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiTV_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */);
		}
	};

	class SCREEN_AudiTV_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiTV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "157" /* screen ID */);
		}
	};

	class SCREEN_AudiCD : public PreformedCommand
	{
	public:
		SCREEN_AudiCD(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */);
		}
	};

	class SCREEN_AudiCD_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiCD_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */);
		}
	};

	class SCREEN_AudiCD_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiCD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */);
		}
	};

	class SCREEN_AudiCD_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiCD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "158" /* screen ID */);
		}
	};

	class SCREEN_AudiStoredMedia : public PreformedCommand
	{
	public:
		SCREEN_AudiStoredMedia(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */);
		}
	};

	class SCREEN_AudiStoredMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiStoredMedia_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */);
		}
	};

	class SCREEN_AudiStoredMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiStoredMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */);
		}
	};

	class SCREEN_AudiStoredMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiStoredMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "159" /* screen ID */);
		}
	};

	class SCREEN_AudiInternet : public PreformedCommand
	{
	public:
		SCREEN_AudiInternet(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */);
		}
	};

	class SCREEN_AudiInternet_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiInternet_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */);
		}
	};

	class SCREEN_AudiInternet_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiInternet_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */);
		}
	};

	class SCREEN_AudiInternet_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiInternet_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "160" /* screen ID */);
		}
	};

	class SCREEN_AudiEmail : public PreformedCommand
	{
	public:
		SCREEN_AudiEmail(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */);
		}
	};

	class SCREEN_AudiEmail_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiEmail_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */);
		}
	};

	class SCREEN_AudiEmail_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiEmail_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */);
		}
	};

	class SCREEN_AudiEmail_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiEmail_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "161" /* screen ID */);
		}
	};

	class SCREEN_AudiAgenda : public PreformedCommand
	{
	public:
		SCREEN_AudiAgenda(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */);
		}
	};

	class SCREEN_AudiAgenda_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiAgenda_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */);
		}
	};

	class SCREEN_AudiAgenda_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiAgenda_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */);
		}
	};

	class SCREEN_AudiAgenda_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiAgenda_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "162" /* screen ID */);
		}
	};

	class SCREEN_AudiOther : public PreformedCommand
	{
	public:
		SCREEN_AudiOther(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */);
		}
	};

	class SCREEN_AudiOther_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiOther_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */);
		}
	};

	class SCREEN_AudiOther_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiOther_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */);
		}
	};

	class SCREEN_AudiOther_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiOther_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "163" /* screen ID */);
		}
	};

	class SCREEN_AudiNav : public PreformedCommand
	{
	public:
		SCREEN_AudiNav(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */);
		}
	};

	class SCREEN_AudiNav_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiNav_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */);
		}
	};

	class SCREEN_AudiNav_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiNav_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */);
		}
	};

	class SCREEN_AudiNav_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiNav_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "164" /* screen ID */);
		}
	};

	class SCREEN_AudiInfo : public PreformedCommand
	{
	public:
		SCREEN_AudiInfo(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */);
		}
	};

	class SCREEN_AudiInfo_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiInfo_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */);
		}
	};

	class SCREEN_AudiInfo_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiInfo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */);
		}
	};

	class SCREEN_AudiInfo_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiInfo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "165" /* screen ID */);
		}
	};

	class SCREEN_AudiCar : public PreformedCommand
	{
	public:
		SCREEN_AudiCar(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */);
		}
	};

	class SCREEN_AudiCar_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiCar_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */);
		}
	};

	class SCREEN_AudiCar_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiCar_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */);
		}
	};

	class SCREEN_AudiCar_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiCar_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "166" /* screen ID */);
		}
	};

	class SCREEN_AudiSetup : public PreformedCommand
	{
	public:
		SCREEN_AudiSetup(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */);
		}
	};

	class SCREEN_AudiSetup_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiSetup_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */);
		}
	};

	class SCREEN_AudiSetup_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiSetup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */);
		}
	};

	class SCREEN_AudiSetup_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiSetup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "167" /* screen ID */);
		}
	};

	class SCREEN_AudiLighting : public PreformedCommand
	{
	public:
		SCREEN_AudiLighting(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */);
		}
	};

	class SCREEN_AudiLighting_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiLighting_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */);
		}
	};

	class SCREEN_AudiLighting_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiLighting_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */);
		}
	};

	class SCREEN_AudiLighting_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiLighting_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "168" /* screen ID */);
		}
	};

	class SCREEN_AudiClimate : public PreformedCommand
	{
	public:
		SCREEN_AudiClimate(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */);
		}
	};

	class SCREEN_AudiClimate_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiClimate_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */);
		}
	};

	class SCREEN_AudiClimate_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiClimate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */);
		}
	};

	class SCREEN_AudiClimate_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiClimate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "169" /* screen ID */);
		}
	};

	class SCREEN_AudiSecurity : public PreformedCommand
	{
	public:
		SCREEN_AudiSecurity(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */);
		}
	};

	class SCREEN_AudiSecurity_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiSecurity_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */);
		}
	};

	class SCREEN_AudiSecurity_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiSecurity_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */);
		}
	};

	class SCREEN_AudiSecurity_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiSecurity_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "170" /* screen ID */);
		}
	};

	class SCREEN_AudiTelecom : public PreformedCommand
	{
	public:
		SCREEN_AudiTelecom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */);
		}
	};

	class SCREEN_AudiTelecom_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiTelecom_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */);
		}
	};

	class SCREEN_AudiTelecom_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiTelecom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */);
		}
	};

	class SCREEN_AudiTelecom_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiTelecom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "171" /* screen ID */);
		}
	};

	class SCREEN_AudiFullScreen : public PreformedCommand
	{
	public:
		SCREEN_AudiFullScreen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */);
		}
	};

	class SCREEN_AudiFullScreen_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiFullScreen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */);
		}
	};

	class SCREEN_AudiFullScreen_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiFullScreen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */);
		}
	};

	class SCREEN_AudiFullScreen_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiFullScreen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "172" /* screen ID */);
		}
	};

	class SCREEN_AudiVideoRemote : public PreformedCommand
	{
	public:
		SCREEN_AudiVideoRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */);
		}
	};

	class SCREEN_AudiVideoRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiVideoRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */);
		}
	};

	class SCREEN_AudiVideoRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiVideoRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */);
		}
	};

	class SCREEN_AudiVideoRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiVideoRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "173" /* screen ID */);
		}
	};

	class SCREEN_AudiAudioRemote : public PreformedCommand
	{
	public:
		SCREEN_AudiAudioRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */);
		}
	};

	class SCREEN_AudiAudioRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiAudioRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */);
		}
	};

	class SCREEN_AudiAudioRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiAudioRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */);
		}
	};

	class SCREEN_AudiAudioRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiAudioRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "174" /* screen ID */);
		}
	};

	class SCREEN_VideoWizard : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */);
		}
	};

	class SCREEN_VideoWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */);
		}
	};

	class SCREEN_VideoWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */);
		}
	};

	class SCREEN_VideoWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_VideoWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "175" /* screen ID */);
		}
	};

	class SCREEN_DialogCannotPlayMedia : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia(long DeviceIDFrom, long DeviceIDTo,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotPlayMedia_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotPlayMedia_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotPlayMedia_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotPlayMedia_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "176" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogGenericNoButtons : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericNoButtons(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "177" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericNoButtons_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericNoButtons_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "177" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericNoButtons_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericNoButtons_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "177" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericNoButtons_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericNoButtons_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "177" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress(long DeviceIDFrom, long DeviceIDTo,
			string sPK_Device_From, string sPK_Device_Ripping)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sPK_Device_From, string sPK_Device_Ripping)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sPK_Device_From, string sPK_Device_Ripping)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogRippingInProgress_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInProgress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sPK_Device_From, string sPK_Device_Ripping)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "178" /* screen ID */,
				184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 185 /* PK_RippingDevice */, sPK_Device_Ripping.c_str());
		}
	};

	class SCREEN_DialogCheckingDrive : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */);
		}
	};

	class SCREEN_DialogCheckingDrive_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */);
		}
	};

	class SCREEN_DialogCheckingDrive_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */);
		}
	};

	class SCREEN_DialogCheckingDrive_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogCheckingDrive_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "179" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToSavePlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToSavePlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "180" /* screen ID */);
		}
	};

	class SCREEN_DialogPlaylistSaved : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */);
		}
	};

	class SCREEN_DialogPlaylistSaved_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */);
		}
	};

	class SCREEN_DialogPlaylistSaved_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */);
		}
	};

	class SCREEN_DialogPlaylistSaved_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogPlaylistSaved_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "181" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */);
		}
	};

	class SCREEN_DialogUnableToLoadPlaylist_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogUnableToLoadPlaylist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "182" /* screen ID */);
		}
	};

	class SCREEN_DialogRippingError : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sTimeout)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingError_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sTimeout)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingError_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sTimeout)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingError_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingError_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sTimeout)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "183" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 182 /* Timeout */, sTimeout.c_str());
		}
	};

	class SCREEN_DialogRippingInstructions : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */);
		}
	};

	class SCREEN_DialogRippingInstructions_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */);
		}
	};

	class SCREEN_DialogRippingInstructions_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */);
		}
	};

	class SCREEN_DialogRippingInstructions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogRippingInstructions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "184" /* screen ID */);
		}
	};

	class SCREEN_DialogGenericError : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericError_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericError_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGenericError_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogGenericError_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 5, 
				COMMANDPARAMETER_PK_Screen_CONST, "185" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 181 /* Prompt To Reset Router */, sPromptToResetRouter.c_str(), 182 /* Timeout */, sTimeout.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark(long DeviceIDFrom, long DeviceIDTo,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogCannotBookmark_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogCannotBookmark_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sErrors)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "186" /* screen ID */,
				18 /* Errors */, sErrors.c_str());
		}
	};

	class SCREEN_DialogAskToResume : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */,
				159 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen).c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str());
		}
	};

	class SCREEN_DialogAskToResume_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */,
				159 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen).c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str());
		}
	};

	class SCREEN_DialogAskToResume_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */,
				159 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen).c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str());
		}
	};

	class SCREEN_DialogAskToResume_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogAskToResume_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 8, 
				COMMANDPARAMETER_PK_Screen_CONST, "187" /* screen ID */,
				159 /* Screen for remote control to go to after prompting */, StringUtils::ltos(iPK_Screen).c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 186 /* PK_Device_MediaSource */, sPK_Device_MediaSource.c_str(), 187 /* StreamID */, sStreamID_String.c_str(), 188 /* Position */, sPosition.c_str(), 189 /* Users */, sUsers.c_str(), 190 /* PK_MediaType */, sPK_MediaType_String.c_str());
		}
	};

	class SCREEN_DialogGC100Error : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGC100Error_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGC100Error_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogGC100Error_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogGC100Error_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sCannotGoBack)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "188" /* screen ID */,
				163 /* Description */, sDescription.c_str(), 183 /* Cannot Go Back */, sCannotGoBack.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions(long DeviceIDFrom, long DeviceIDTo,
			string sDescription, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sDescription, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sDescription, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogPhoneInstructions_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogPhoneInstructions_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sDescription, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "189" /* screen ID */,
				163 /* Instructions */, sDescription.c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed(long DeviceIDFrom, long DeviceIDTo,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed_DL : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed_DT : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_DialogSendFileToPhoneFailed_Cat : public PreformedCommand
	{
	public:
		SCREEN_DialogSendFileToPhoneFailed_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 6, 
				COMMANDPARAMETER_PK_Screen_CONST, "190" /* screen ID */,
				47 /* Mac Address */, sMac_address.c_str(), 137 /* Command Line */, sCommand_Line.c_str(), 184 /* PK_DeviceFrom */, sPK_Device_From.c_str(), 191 /* Phone Name */, sPhoneName.c_str(), 192 /* PK_Device_AppServer */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_AudiHome : public PreformedCommand
	{
	public:
		SCREEN_AudiHome(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */);
		}
	};

	class SCREEN_AudiHome_DL : public PreformedCommand
	{
	public:
		SCREEN_AudiHome_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */);
		}
	};

	class SCREEN_AudiHome_DT : public PreformedCommand
	{
	public:
		SCREEN_AudiHome_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */);
		}
	};

	class SCREEN_AudiHome_Cat : public PreformedCommand
	{
	public:
		SCREEN_AudiHome_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "191" /* screen ID */);
		}
	};

	class SCREEN_RoomsWizard : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */);
		}
	};

	class SCREEN_RoomsWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */);
		}
	};

	class SCREEN_RoomsWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */);
		}
	};

	class SCREEN_RoomsWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_RoomsWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "192" /* screen ID */);
		}
	};

	class SCREEN_UsersWizard : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */);
		}
	};

	class SCREEN_UsersWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */);
		}
	};

	class SCREEN_UsersWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */);
		}
	};

	class SCREEN_UsersWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_UsersWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "193" /* screen ID */);
		}
	};

	class SCREEN_CountryWizard : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */);
		}
	};

	class SCREEN_CountryWizard_DL : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */);
		}
	};

	class SCREEN_CountryWizard_DT : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */);
		}
	};

	class SCREEN_CountryWizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_CountryWizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "194" /* screen ID */);
		}
	};

	class SCREEN_TV_provider : public PreformedCommand
	{
	public:
		SCREEN_TV_provider(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "195" /* screen ID */);
		}
	};

	class SCREEN_TV_provider_DL : public PreformedCommand
	{
	public:
		SCREEN_TV_provider_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "195" /* screen ID */);
		}
	};

	class SCREEN_TV_provider_DT : public PreformedCommand
	{
	public:
		SCREEN_TV_provider_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "195" /* screen ID */);
		}
	};

	class SCREEN_TV_provider_Cat : public PreformedCommand
	{
	public:
		SCREEN_TV_provider_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "195" /* screen ID */);
		}
	};

	class SCREEN_TV_Manufacturer : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */);
		}
	};

	class SCREEN_TV_Manufacturer_DL : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */);
		}
	};

	class SCREEN_TV_Manufacturer_DT : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */);
		}
	};

	class SCREEN_TV_Manufacturer_Cat : public PreformedCommand
	{
	public:
		SCREEN_TV_Manufacturer_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "196" /* screen ID */);
		}
	};

	class SCREEN_Receiver : public PreformedCommand
	{
	public:
		SCREEN_Receiver(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */);
		}
	};

	class SCREEN_Receiver_DL : public PreformedCommand
	{
	public:
		SCREEN_Receiver_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */);
		}
	};

	class SCREEN_Receiver_DT : public PreformedCommand
	{
	public:
		SCREEN_Receiver_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */);
		}
	};

	class SCREEN_Receiver_Cat : public PreformedCommand
	{
	public:
		SCREEN_Receiver_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "197" /* screen ID */);
		}
	};

	class SCREEN_LightsSetup : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */);
		}
	};

	class SCREEN_LightsSetup_DL : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */);
		}
	};

	class SCREEN_LightsSetup_DT : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */);
		}
	};

	class SCREEN_LightsSetup_Cat : public PreformedCommand
	{
	public:
		SCREEN_LightsSetup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "198" /* screen ID */);
		}
	};

	class SCREEN_AlarmPanel : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */);
		}
	};

	class SCREEN_AlarmPanel_DL : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */);
		}
	};

	class SCREEN_AlarmPanel_DT : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */);
		}
	};

	class SCREEN_AlarmPanel_Cat : public PreformedCommand
	{
	public:
		SCREEN_AlarmPanel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "199" /* screen ID */);
		}
	};

	class SCREEN_VOIP_Provider : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */);
		}
	};

	class SCREEN_VOIP_Provider_DL : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */);
		}
	};

	class SCREEN_VOIP_Provider_DT : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */);
		}
	};

	class SCREEN_VOIP_Provider_Cat : public PreformedCommand
	{
	public:
		SCREEN_VOIP_Provider_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "200" /* screen ID */);
		}
	};

	class SCREEN_AV_Devices : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */);
		}
	};

	class SCREEN_AV_Devices_DL : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */);
		}
	};

	class SCREEN_AV_Devices_DT : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */);
		}
	};

	class SCREEN_AV_Devices_Cat : public PreformedCommand
	{
	public:
		SCREEN_AV_Devices_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "201" /* screen ID */);
		}
	};

	class SCREEN_NAS_Options : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options(long DeviceIDFrom, long DeviceIDTo,
			int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NAS_Options_DL : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NAS_Options_DT : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_NAS_Options_Cat : public PreformedCommand
	{
	public:
		SCREEN_NAS_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_PnpQueue)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "202" /* screen ID */,
				224 /* PK_PnpQueue */, StringUtils::ltos(iPK_PnpQueue).c_str());
		}
	};

	class SCREEN_New_Phone_Enter_Number : public PreformedCommand
	{
	public:
		SCREEN_New_Phone_Enter_Number(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "203" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_New_Phone_Enter_Number_DL : public PreformedCommand
	{
	public:
		SCREEN_New_Phone_Enter_Number_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "203" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_New_Phone_Enter_Number_DT : public PreformedCommand
	{
	public:
		SCREEN_New_Phone_Enter_Number_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "203" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_New_Phone_Enter_Number_Cat : public PreformedCommand
	{
	public:
		SCREEN_New_Phone_Enter_Number_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device, string sPhoneName)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "203" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str(), 191 /* Phone Name */, sPhoneName.c_str());
		}
	};

	class SCREEN_Need_Reload_Router : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */);
		}
	};

	class SCREEN_Need_Reload_Router_DL : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */);
		}
	};

	class SCREEN_Need_Reload_Router_DT : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */);
		}
	};

	class SCREEN_Need_Reload_Router_Cat : public PreformedCommand
	{
	public:
		SCREEN_Need_Reload_Router_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "204" /* screen ID */);
		}
	};

	class SCREEN_Need_Regen_Orbiter : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */);
		}
	};

	class SCREEN_Need_Regen_Orbiter_DL : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */);
		}
	};

	class SCREEN_Need_Regen_Orbiter_DT : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */);
		}
	};

	class SCREEN_Need_Regen_Orbiter_Cat : public PreformedCommand
	{
	public:
		SCREEN_Need_Regen_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "205" /* screen ID */);
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera_DL : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera_DT : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_Sensors_Viewed_By_Camera_Cat : public PreformedCommand
	{
	public:
		SCREEN_Sensors_Viewed_By_Camera_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_PK_Screen_CONST, "206" /* screen ID */,
				2 /* PK_Device */, StringUtils::ltos(iPK_Device).c_str());
		}
	};

	class SCREEN_mnuPVROSD : public PreformedCommand
	{
	public:
		SCREEN_mnuPVROSD(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */);
		}
	};

	class SCREEN_mnuPVROSD_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuPVROSD_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */);
		}
	};

	class SCREEN_mnuPVROSD_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuPVROSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */);
		}
	};

	class SCREEN_mnuPVROSD_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuPVROSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "207" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "208" /* screen ID */);
		}
	};

	class SCREEN_Demo_Media : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */);
		}
	};

	class SCREEN_Demo_Media_DL : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */);
		}
	};

	class SCREEN_Demo_Media_DT : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */);
		}
	};

	class SCREEN_Demo_Media_Cat : public PreformedCommand
	{
	public:
		SCREEN_Demo_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "209" /* screen ID */);
		}
	};

	class SCREEN_Download_are_ready_to_install : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install(long DeviceIDFrom, long DeviceIDTo,
			int iPK_Device, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Download_are_ready_to_install_DL : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install_DL(long DeviceIDFrom, string sDeviceIDTo,
			int iPK_Device, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Download_are_ready_to_install_DT : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			int iPK_Device, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_Download_are_ready_to_install_Cat : public PreformedCommand
	{
	public:
		SCREEN_Download_are_ready_to_install_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			int iPK_Device, string sPK_Device_AppServer)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 3, 
				COMMANDPARAMETER_PK_Screen_CONST, "210" /* screen ID */,
				2 /* Device to be rebooted */, StringUtils::ltos(iPK_Device).c_str(), 192 /* AppServer device which will execute half command */, sPK_Device_AppServer.c_str());
		}
	};

	class SCREEN_mnuMainMenu2 : public PreformedCommand
	{
	public:
		SCREEN_mnuMainMenu2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "211" /* screen ID */);
		}
	};

	class SCREEN_mnuMainMenu2_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuMainMenu2_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "211" /* screen ID */);
		}
	};

	class SCREEN_mnuMainMenu2_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuMainMenu2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "211" /* screen ID */);
		}
	};

	class SCREEN_mnuMainMenu2_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuMainMenu2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "211" /* screen ID */);
		}
	};

	class SCREEN_mnuAmbiance : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */);
		}
	};

	class SCREEN_mnuAmbiance_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */);
		}
	};

	class SCREEN_mnuAmbiance_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */);
		}
	};

	class SCREEN_mnuAmbiance_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuAmbiance_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "212" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsDiscrete : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsDiscrete(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "215" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsDiscrete_DL : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsDiscrete_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "215" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsDiscrete_DT : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsDiscrete_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "215" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsDiscrete_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsDiscrete_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "215" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsToggle : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsToggle_DL : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsToggle_DT : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmInputsToggle_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmInputsToggle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "216" /* screen ID */);
		}
	};

	class SCREEN_TVOnOffCodes : public PreformedCommand
	{
	public:
		SCREEN_TVOnOffCodes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "217" /* screen ID */);
		}
	};

	class SCREEN_TVOnOffCodes_DL : public PreformedCommand
	{
	public:
		SCREEN_TVOnOffCodes_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "217" /* screen ID */);
		}
	};

	class SCREEN_TVOnOffCodes_DT : public PreformedCommand
	{
	public:
		SCREEN_TVOnOffCodes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "217" /* screen ID */);
		}
	};

	class SCREEN_TVOnOffCodes_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVOnOffCodes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "217" /* screen ID */);
		}
	};

	class SCREEN_mnuPVR_OSD_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_mnuPVR_OSD_Full_Screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */);
		}
	};

	class SCREEN_mnuPVR_OSD_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuPVR_OSD_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */);
		}
	};

	class SCREEN_mnuPVR_OSD_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuPVR_OSD_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */);
		}
	};

	class SCREEN_mnuPVR_OSD_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuPVR_OSD_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "218" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_Full_Screen : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_Full_Screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_Full_Screen_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_Full_Screen_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_Full_Screen_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_Full_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */);
		}
	};

	class SCREEN_mnuPVRRecording_Full_Screen_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuPVRRecording_Full_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "219" /* screen ID */);
		}
	};

	class SCREEN_CurrentlyActiveRemote : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */);
		}
	};

	class SCREEN_CurrentlyActiveRemote_DL : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */);
		}
	};

	class SCREEN_CurrentlyActiveRemote_DT : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */);
		}
	};

	class SCREEN_CurrentlyActiveRemote_Cat : public PreformedCommand
	{
	public:
		SCREEN_CurrentlyActiveRemote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "220" /* screen ID */);
		}
	};

	class SCREEN_TVMultipleInputs : public PreformedCommand
	{
	public:
		SCREEN_TVMultipleInputs(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "221" /* screen ID */);
		}
	};

	class SCREEN_TVMultipleInputs_DL : public PreformedCommand
	{
	public:
		SCREEN_TVMultipleInputs_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "221" /* screen ID */);
		}
	};

	class SCREEN_TVMultipleInputs_DT : public PreformedCommand
	{
	public:
		SCREEN_TVMultipleInputs_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "221" /* screen ID */);
		}
	};

	class SCREEN_TVMultipleInputs_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVMultipleInputs_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "221" /* screen ID */);
		}
	};

	class SCREEN_mnuLights : public PreformedCommand
	{
	public:
		SCREEN_mnuLights(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */);
		}
	};

	class SCREEN_mnuLights_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuLights_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */);
		}
	};

	class SCREEN_mnuLights_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuLights_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */);
		}
	};

	class SCREEN_mnuLights_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuLights_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "222" /* screen ID */);
		}
	};

	class SCREEN_mnuPlaybackControl : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */);
		}
	};

	class SCREEN_mnuPlaybackControl_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */);
		}
	};

	class SCREEN_mnuPlaybackControl_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */);
		}
	};

	class SCREEN_mnuPlaybackControl_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaybackControl_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "223" /* screen ID */);
		}
	};

	class SCREEN_mnuSpeedControl : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */);
		}
	};

	class SCREEN_mnuSpeedControl_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */);
		}
	};

	class SCREEN_mnuSpeedControl_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */);
		}
	};

	class SCREEN_mnuSpeedControl_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuSpeedControl_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "224" /* screen ID */);
		}
	};

	class SCREEN_mnuVolume : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */);
		}
	};

	class SCREEN_mnuVolume_DL : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */);
		}
	};

	class SCREEN_mnuVolume_DT : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */);
		}
	};

	class SCREEN_mnuVolume_Cat : public PreformedCommand
	{
	public:
		SCREEN_mnuVolume_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "225" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffTogle : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffTogle(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "226" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffTogle_DL : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffTogle_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "226" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffTogle_DT : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffTogle_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "226" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffTogle_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffTogle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "226" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffDiscret : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffDiscret(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "227" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffDiscret_DL : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffDiscret_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "227" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffDiscret_DT : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffDiscret_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "227" /* screen ID */);
		}
	};

	class SCREEN_TVConfirmOnOffDiscret_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVConfirmOnOffDiscret_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "227" /* screen ID */);
		}
	};

	class SCREEN_Internal_Disk_Drive_Wizard : public PreformedCommand
	{
	public:
		SCREEN_Internal_Disk_Drive_Wizard(long DeviceIDFrom, long DeviceIDTo,
			string sData_String, int iPK_Device_ControlledVia, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "228" /* screen ID */,
				109 /* A block device associated with the internal disk drive  */, sData_String.c_str(), 156 /* The id of the device which controls the device which will be created */, StringUtils::ltos(iPK_Device_ControlledVia).c_str(), 163 /* Description message. */, sDescription.c_str());
		}
	};

	class SCREEN_Internal_Disk_Drive_Wizard_DL : public PreformedCommand
	{
	public:
		SCREEN_Internal_Disk_Drive_Wizard_DL(long DeviceIDFrom, string sDeviceIDTo,
			string sData_String, int iPK_Device_ControlledVia, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "228" /* screen ID */,
				109 /* A block device associated with the internal disk drive  */, sData_String.c_str(), 156 /* The id of the device which controls the device which will be created */, StringUtils::ltos(iPK_Device_ControlledVia).c_str(), 163 /* Description message. */, sDescription.c_str());
		}
	};

	class SCREEN_Internal_Disk_Drive_Wizard_DT : public PreformedCommand
	{
	public:
		SCREEN_Internal_Disk_Drive_Wizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,
			string sData_String, int iPK_Device_ControlledVia, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "228" /* screen ID */,
				109 /* A block device associated with the internal disk drive  */, sData_String.c_str(), 156 /* The id of the device which controls the device which will be created */, StringUtils::ltos(iPK_Device_ControlledVia).c_str(), 163 /* Description message. */, sDescription.c_str());
		}
	};

	class SCREEN_Internal_Disk_Drive_Wizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_Internal_Disk_Drive_Wizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,
			string sData_String, int iPK_Device_ControlledVia, string sDescription)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 4, 
				COMMANDPARAMETER_PK_Screen_CONST, "228" /* screen ID */,
				109 /* A block device associated with the internal disk drive  */, sData_String.c_str(), 156 /* The id of the device which controls the device which will be created */, StringUtils::ltos(iPK_Device_ControlledVia).c_str(), 163 /* Description message. */, sDescription.c_str());
		}
	};

	class SCREEN_TVDSPMode : public PreformedCommand
	{
	public:
		SCREEN_TVDSPMode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "229" /* screen ID */);
		}
	};

	class SCREEN_TVDSPMode_DL : public PreformedCommand
	{
	public:
		SCREEN_TVDSPMode_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "229" /* screen ID */);
		}
	};

	class SCREEN_TVDSPMode_DT : public PreformedCommand
	{
	public:
		SCREEN_TVDSPMode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "229" /* screen ID */);
		}
	};

	class SCREEN_TVDSPMode_Cat : public PreformedCommand
	{
	public:
		SCREEN_TVDSPMode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "229" /* screen ID */);
		}
	};

	class SCREEN_Media_Tracks : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */);
		}
	};

	class SCREEN_Media_Tracks_DL : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */);
		}
	};

	class SCREEN_Media_Tracks_DT : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */);
		}
	};

	class SCREEN_Media_Tracks_Cat : public PreformedCommand
	{
	public:
		SCREEN_Media_Tracks_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "230" /* screen ID */);
		}
	};

	class SCREEN_Which_Wizard : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */);
		}
	};

	class SCREEN_Which_Wizard_DL : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard_DL(long DeviceIDFrom, string sDeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, sDeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */);
		}
	};

	class SCREEN_Which_Wizard_DT : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */);
		}
	};

	class SCREEN_Which_Wizard_Cat : public PreformedCommand
	{
	public:
		SCREEN_Which_Wizard_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_PK_Screen_CONST, "231" /* screen ID */);
		}
	};


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
		virtual void SCREEN_Main(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Lights(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Media(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Climate(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Security(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Telephony(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Misc(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_UserStatus(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallPhonebook(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallFavorites(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallDialNumber(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallPlutoUser(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SecurityPanel(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SecurityStatusReport(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SingleCameraViewOnly(long PK_Screen, int iPK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Intercom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_QuadViewCameras(long PK_Screen, string sList_PK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideoCallInProgress(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileSave(long PK_Screen, string sText, string sPrivate, string sPublic, string sCaption){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ManagePlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallDevice(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallPhonebook2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakeCallIntercom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CurrentUser(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CDTrackCopy(long PK_Screen, int iPK_Users){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CDRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VoicemailMain(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TransferAllUsers(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Sleeping(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ControllerStatus(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_OtherFolders(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ConfirmDelete(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WebURL(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Browser(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ModeChanged(long PK_Screen, string sPK_HouseMode, string sHouseModeTime, string sExitDelay, string sAlerts){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SensorsNotReady(long PK_Screen, string sDescription){ GotoScreen(PK_Screen); }
		virtual void SCREEN_RecordedTVShows(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DeviceControl(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Computing(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_OpenFile(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_BrowserConsole(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FavoritesPDA(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PCFileTools(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdvancedOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakingCall(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MakingLinPhoneBroadcast(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_Music_Movies_Video(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PVR(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDMenu(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapMedia(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdvancedMediaOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PopupMessage(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MusicRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_LinPhones(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Orbiters(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Phones(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AnswertoLinPhones(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_UserPinCode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PreviewOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TransferOutside(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SatelliteCableBox(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CurrentLocation(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MediaAttributeSearchInput(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MediaAttributeSearchResult(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NewPhoneDetected(long PK_Screen, string sMac_address, string sDescription){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FirstTime(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_PlayLists(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideosRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_Pictures_Docs(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericTwoButtonQuestion(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TryCodes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ScreenSaver(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NewPlugAndPlayDevice(long PK_Screen, int iPK_Device, string sDescription, string ssComments){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Power(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericDvdRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ViewPlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_withPictures_Mobil(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericDvdMenu(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ViewAttributes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PendingTasks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AddAttribute(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_RadioRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericCDRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_FileList_Popup_Container(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CreateViewBookmarks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_BrowseBookmarks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericAppController(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_BookmarksByMediaType(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MythTvRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Myth_all_commands(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Mytv_watching_tv(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Myth_live_tv_browse_mode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Myth_playback_recording(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericKeyboard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PingPong(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVEPG1(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVEPG2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVEPG3(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_ShowInfo(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CreateViewBookmarksTV(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WebSites(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_WhatModelMobileOrbiter(long PK_Screen, string sMac_address){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Filelist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDmenu(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Attributes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDoptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AdvRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Playlist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudioOptions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Subtitles(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Angles(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NewPnpDevice(long PK_Screen, string sDescription, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VdrRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CdRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PowerFile(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DevCallInProgress(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DevIncomingCall(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DevMoveCallsInProgress(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapSecurity(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapLighting(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapClimate(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_MapTelecom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NavMode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PvrFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Epg(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Help01(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Help02(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Help03(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv01(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv02(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv03(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_SetupMythTv04(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_PvrFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideosFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideosFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_GenericAppFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDMenuFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DVDMenuFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VlcFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VlcFullScreenOpt(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popLights(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popMedia(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popClimate(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popTelecom(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popSecurity(long PK_Screen, string sLocation){ GotoScreen(PK_Screen); }
		virtual void SCREEN_popOthers(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiCarSetup(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiMedia(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiComputing(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiRadio(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiTV(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiCD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiStoredMedia(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiInternet(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiEmail(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiAgenda(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiOther(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiNav(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiInfo(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiCar(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiSetup(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiLighting(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiClimate(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiSecurity(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiTelecom(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiFullScreen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiVideoRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiAudioRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VideoWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogCannotPlayMedia(long PK_Screen, string sErrors){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogGenericNoButtons(long PK_Screen, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogRippingInProgress(long PK_Screen, string sPK_Device_From, string sPK_Device_Ripping){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogCheckingDrive(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogUnableToSavePlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogPlaylistSaved(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogUnableToLoadPlaylist(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogRippingError(long PK_Screen, string sDescription, string sTimeout){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogRippingInstructions(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogGenericError(long PK_Screen, string sDescription, string sPromptToResetRouter, string sTimeout, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogCannotBookmark(long PK_Screen, string sErrors){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogAskToResume(long PK_Screen, int iPK_Screen, string sPK_Device_From, string sPK_Device_MediaSource, string sStreamID_String, string sPosition, string sUsers, string sPK_MediaType_String){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogGC100Error(long PK_Screen, string sDescription, string sCannotGoBack){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogPhoneInstructions(long PK_Screen, string sDescription, string sPhoneName){ GotoScreen(PK_Screen); }
		virtual void SCREEN_DialogSendFileToPhoneFailed(long PK_Screen, string sMac_address, string sCommand_Line, string sPK_Device_From, string sPhoneName, string sPK_Device_AppServer){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AudiHome(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_RoomsWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_UsersWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CountryWizard(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TV_provider(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TV_Manufacturer(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Receiver(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_LightsSetup(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AlarmPanel(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_VOIP_Provider(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_AV_Devices(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_NAS_Options(long PK_Screen, int iPK_PnpQueue){ GotoScreen(PK_Screen); }
		virtual void SCREEN_New_Phone_Enter_Number(long PK_Screen, int iPK_Device, string sPhoneName){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Need_Reload_Router(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Need_Regen_Orbiter(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Sensors_Viewed_By_Camera(long PK_Screen, int iPK_Device){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuPVROSD(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuPVRRecording(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Demo_Media(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Download_are_ready_to_install(long PK_Screen, int iPK_Device, string sPK_Device_AppServer){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuMainMenu2(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuAmbiance(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVConfirmInputsDiscrete(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVConfirmInputsToggle(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVOnOffCodes(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuPVR_OSD_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuPVRRecording_Full_Screen(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_CurrentlyActiveRemote(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVMultipleInputs(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuLights(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuPlaybackControl(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuSpeedControl(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_mnuVolume(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVConfirmOnOffTogle(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVConfirmOnOffDiscret(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Internal_Disk_Drive_Wizard(long PK_Screen, string sData_String, int iPK_Device_ControlledVia, string sDescription){ GotoScreen(PK_Screen); }
		virtual void SCREEN_TVDSPMode(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Media_Tracks(long PK_Screen){ GotoScreen(PK_Screen); }
		virtual void SCREEN_Which_Wizard(long PK_Screen){ GotoScreen(PK_Screen); }

		virtual void ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage)
		{
			switch(nPK_Screen) 
			{
				case 1:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Main(nPK_Screen, sLocation);
					break;
				}
				case 2:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Lights(nPK_Screen, sLocation);
					break;
				}
				case 3:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Media(nPK_Screen, sLocation);
					break;
				}
				case 4:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Climate(nPK_Screen, sLocation);
					break;
				}
				case 5:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Security(nPK_Screen, sLocation);
					break;
				}
				case 6:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Telephony(nPK_Screen, sLocation);
					break;
				}
				case 7:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_Misc(nPK_Screen, sLocation);
					break;
				}
				case 8:
				{
					SCREEN_UserStatus(nPK_Screen);
					break;
				}
				case 9:
				{
					SCREEN_MakeCallPhonebook(nPK_Screen);
					break;
				}
				case 10:
				{
					SCREEN_MakeCallFavorites(nPK_Screen);
					break;
				}
				case 11:
				{
					SCREEN_MakeCallDialNumber(nPK_Screen);
					break;
				}
				case 12:
				{
					SCREEN_MakeCallPlutoUser(nPK_Screen);
					break;
				}
				case 13:
				{
					SCREEN_SecurityPanel(nPK_Screen);
					break;
				}
				case 14:
				{
					SCREEN_SecurityStatusReport(nPK_Screen);
					break;
				}
				case 15:
				{
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					SCREEN_SingleCameraViewOnly(nPK_Screen, iPK_Device);
					break;
				}
				case 16:
				{
					SCREEN_Intercom(nPK_Screen);
					break;
				}
				case 17:
				{
					string sList_PK_Device = pMessage->m_mapParameters[103];
					SCREEN_QuadViewCameras(nPK_Screen, sList_PK_Device);
					break;
				}
				case 18:
				{
					SCREEN_VideoCallInProgress(nPK_Screen);
					break;
				}
				case 19:
				{
					string sText = pMessage->m_mapParameters[9];
					string sPrivate = pMessage->m_mapParameters[160];
					string sPublic = pMessage->m_mapParameters[161];
					string sCaption = pMessage->m_mapParameters[162];
					SCREEN_FileSave(nPK_Screen, sText, sPrivate, sPublic, sCaption);
					break;
				}
				case 20:
				{
					SCREEN_ManagePlaylist(nPK_Screen);
					break;
				}
				case 21:
				{
					SCREEN_MakeCallDevice(nPK_Screen);
					break;
				}
				case 22:
				{
					SCREEN_MakeCallPhonebook2(nPK_Screen);
					break;
				}
				case 23:
				{
					SCREEN_MakeCallIntercom(nPK_Screen);
					break;
				}
				case 24:
				{
					SCREEN_CurrentUser(nPK_Screen);
					break;
				}
				case 25:
				{
					int iPK_Users = atoi(pMessage->m_mapParameters[17].c_str());
					SCREEN_CDTrackCopy(nPK_Screen, iPK_Users);
					break;
				}
				case 26:
				{
					SCREEN_CDRemote(nPK_Screen);
					break;
				}
				case 27:
				{
					SCREEN_VoicemailMain(nPK_Screen);
					break;
				}
				case 28:
				{
					SCREEN_TransferAllUsers(nPK_Screen);
					break;
				}
				case 29:
				{
					SCREEN_Sleeping(nPK_Screen);
					break;
				}
				case 30:
				{
					SCREEN_ControllerStatus(nPK_Screen);
					break;
				}
				case 31:
				{
					SCREEN_OtherFolders(nPK_Screen);
					break;
				}
				case 32:
				{
					SCREEN_ConfirmDelete(nPK_Screen);
					break;
				}
				case 33:
				{
					SCREEN_WebURL(nPK_Screen);
					break;
				}
				case 34:
				{
					SCREEN_Browser(nPK_Screen);
					break;
				}
				case 35:
				{
					string sPK_HouseMode = pMessage->m_mapParameters[165];
					string sHouseModeTime = pMessage->m_mapParameters[166];
					string sExitDelay = pMessage->m_mapParameters[167];
					string sAlerts = pMessage->m_mapParameters[168];
					SCREEN_ModeChanged(nPK_Screen, sPK_HouseMode, sHouseModeTime, sExitDelay, sAlerts);
					break;
				}
				case 36:
				{
					string sDescription = pMessage->m_mapParameters[163];
					SCREEN_SensorsNotReady(nPK_Screen, sDescription);
					break;
				}
				case 37:
				{
					SCREEN_RecordedTVShows(nPK_Screen);
					break;
				}
				case 38:
				{
					SCREEN_DeviceControl(nPK_Screen);
					break;
				}
				case 39:
				{
					SCREEN_Computing(nPK_Screen);
					break;
				}
				case 40:
				{
					SCREEN_OpenFile(nPK_Screen);
					break;
				}
				case 41:
				{
					SCREEN_BrowserConsole(nPK_Screen);
					break;
				}
				case 42:
				{
					SCREEN_FavoritesPDA(nPK_Screen);
					break;
				}
				case 43:
				{
					SCREEN_PCFileTools(nPK_Screen);
					break;
				}
				case 44:
				{
					SCREEN_AdvancedOptions(nPK_Screen);
					break;
				}
				case 45:
				{
					SCREEN_MakingCall(nPK_Screen);
					break;
				}
				case 46:
				{
					SCREEN_MakingLinPhoneBroadcast(nPK_Screen);
					break;
				}
				case 47:
				{
					SCREEN_FileList_Music_Movies_Video(nPK_Screen);
					break;
				}
				case 48:
				{
					SCREEN_PVR(nPK_Screen);
					break;
				}
				case 49:
				{
					SCREEN_DVDRemote(nPK_Screen);
					break;
				}
				case 50:
				{
					SCREEN_DVDMenu(nPK_Screen);
					break;
				}
				case 51:
				{
					SCREEN_MapMedia(nPK_Screen);
					break;
				}
				case 52:
				{
					SCREEN_AdvancedMediaOptions(nPK_Screen);
					break;
				}
				case 53:
				{
					SCREEN_PopupMessage(nPK_Screen);
					break;
				}
				case 54:
				{
					SCREEN_MusicRemote(nPK_Screen);
					break;
				}
				case 55:
				{
					SCREEN_LinPhones(nPK_Screen);
					break;
				}
				case 56:
				{
					SCREEN_Orbiters(nPK_Screen);
					break;
				}
				case 57:
				{
					SCREEN_Phones(nPK_Screen);
					break;
				}
				case 58:
				{
					SCREEN_DVDOptions(nPK_Screen);
					break;
				}
				case 59:
				{
					SCREEN_AnswertoLinPhones(nPK_Screen);
					break;
				}
				case 60:
				{
					SCREEN_UserPinCode(nPK_Screen);
					break;
				}
				case 61:
				{
					SCREEN_PreviewOptions(nPK_Screen);
					break;
				}
				case 62:
				{
					SCREEN_TransferOutside(nPK_Screen);
					break;
				}
				case 63:
				{
					SCREEN_SatelliteCableBox(nPK_Screen);
					break;
				}
				case 64:
				{
					SCREEN_CurrentLocation(nPK_Screen);
					break;
				}
				case 65:
				{
					SCREEN_MediaAttributeSearchInput(nPK_Screen);
					break;
				}
				case 66:
				{
					SCREEN_MediaAttributeSearchResult(nPK_Screen);
					break;
				}
				case 67:
				{
					string sMac_address = pMessage->m_mapParameters[47];
					string sDescription = pMessage->m_mapParameters[163];
					SCREEN_NewPhoneDetected(nPK_Screen, sMac_address, sDescription);
					break;
				}
				case 68:
				{
					SCREEN_FirstTime(nPK_Screen);
					break;
				}
				case 69:
				{
					SCREEN_FileList_PlayLists(nPK_Screen);
					break;
				}
				case 70:
				{
					SCREEN_VideosRemote(nPK_Screen);
					break;
				}
				case 71:
				{
					SCREEN_FileList_Pictures_Docs(nPK_Screen);
					break;
				}
				case 72:
				{
					SCREEN_GenericTwoButtonQuestion(nPK_Screen);
					break;
				}
				case 73:
				{
					SCREEN_TryCodes(nPK_Screen);
					break;
				}
				case 74:
				{
					SCREEN_ScreenSaver(nPK_Screen);
					break;
				}
				case 75:
				{
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					string sDescription = pMessage->m_mapParameters[163];
					string ssComments = pMessage->m_mapParameters[164];
					SCREEN_NewPlugAndPlayDevice(nPK_Screen, iPK_Device, sDescription, ssComments);
					break;
				}
				case 76:
				{
					SCREEN_Power(nPK_Screen);
					break;
				}
				case 77:
				{
					SCREEN_GenericDvdRemote(nPK_Screen);
					break;
				}
				case 78:
				{
					SCREEN_ViewPlaylist(nPK_Screen);
					break;
				}
				case 79:
				{
					SCREEN_FileList_withPictures_Mobil(nPK_Screen);
					break;
				}
				case 80:
				{
					SCREEN_GenericDvdMenu(nPK_Screen);
					break;
				}
				case 81:
				{
					SCREEN_ViewAttributes(nPK_Screen);
					break;
				}
				case 82:
				{
					SCREEN_PendingTasks(nPK_Screen);
					break;
				}
				case 83:
				{
					SCREEN_AddAttribute(nPK_Screen);
					break;
				}
				case 84:
				{
					SCREEN_RadioRemote(nPK_Screen);
					break;
				}
				case 85:
				{
					SCREEN_GenericCDRemote(nPK_Screen);
					break;
				}
				case 86:
				{
					SCREEN_FileList_Popup_Container(nPK_Screen);
					break;
				}
				case 87:
				{
					SCREEN_CreateViewBookmarks(nPK_Screen);
					break;
				}
				case 88:
				{
					SCREEN_BrowseBookmarks(nPK_Screen);
					break;
				}
				case 89:
				{
					SCREEN_GenericAppController(nPK_Screen);
					break;
				}
				case 90:
				{
					SCREEN_BookmarksByMediaType(nPK_Screen);
					break;
				}
				case 92:
				{
					SCREEN_MythTvRemote(nPK_Screen);
					break;
				}
				case 93:
				{
					SCREEN_Myth_all_commands(nPK_Screen);
					break;
				}
				case 94:
				{
					SCREEN_Mytv_watching_tv(nPK_Screen);
					break;
				}
				case 95:
				{
					SCREEN_Myth_live_tv_browse_mode(nPK_Screen);
					break;
				}
				case 96:
				{
					SCREEN_Myth_playback_recording(nPK_Screen);
					break;
				}
				case 97:
				{
					SCREEN_GenericKeyboard(nPK_Screen);
					break;
				}
				case 98:
				{
					SCREEN_PingPong(nPK_Screen);
					break;
				}
				case 99:
				{
					SCREEN_TVEPG1(nPK_Screen);
					break;
				}
				case 100:
				{
					SCREEN_TVEPG2(nPK_Screen);
					break;
				}
				case 101:
				{
					SCREEN_TVEPG3(nPK_Screen);
					break;
				}
				case 102:
				{
					SCREEN_ShowInfo(nPK_Screen);
					break;
				}
				case 103:
				{
					SCREEN_CreateViewBookmarksTV(nPK_Screen);
					break;
				}
				case 105:
				{
					SCREEN_WebSites(nPK_Screen);
					break;
				}
				case 106:
				{
					string sMac_address = pMessage->m_mapParameters[47];
					SCREEN_WhatModelMobileOrbiter(nPK_Screen, sMac_address);
					break;
				}
				case 107:
				{
					SCREEN_Filelist(nPK_Screen);
					break;
				}
				case 108:
				{
					SCREEN_DVDmenu(nPK_Screen);
					break;
				}
				case 109:
				{
					SCREEN_Attributes(nPK_Screen);
					break;
				}
				case 110:
				{
					SCREEN_DVDoptions(nPK_Screen);
					break;
				}
				case 111:
				{
					SCREEN_AdvRemote(nPK_Screen);
					break;
				}
				case 112:
				{
					SCREEN_Playlist(nPK_Screen);
					break;
				}
				case 113:
				{
					SCREEN_AudioOptions(nPK_Screen);
					break;
				}
				case 114:
				{
					SCREEN_Subtitles(nPK_Screen);
					break;
				}
				case 115:
				{
					SCREEN_Angles(nPK_Screen);
					break;
				}
				case 116:
				{
					string sDescription = pMessage->m_mapParameters[163];
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_NewPnpDevice(nPK_Screen, sDescription, iPK_PnpQueue);
					break;
				}
				case 117:
				{
					SCREEN_VdrRemote(nPK_Screen);
					break;
				}
				case 118:
				{
					SCREEN_CdRemote(nPK_Screen);
					break;
				}
				case 119:
				{
					SCREEN_PowerFile(nPK_Screen);
					break;
				}
				case 120:
				{
					SCREEN_DevCallInProgress(nPK_Screen);
					break;
				}
				case 121:
				{
					SCREEN_DevIncomingCall(nPK_Screen);
					break;
				}
				case 122:
				{
					SCREEN_DevMoveCallsInProgress(nPK_Screen);
					break;
				}
				case 123:
				{
					SCREEN_MapSecurity(nPK_Screen);
					break;
				}
				case 124:
				{
					SCREEN_MapLighting(nPK_Screen);
					break;
				}
				case 125:
				{
					SCREEN_MapClimate(nPK_Screen);
					break;
				}
				case 126:
				{
					SCREEN_MapTelecom(nPK_Screen);
					break;
				}
				case 127:
				{
					SCREEN_NavMode(nPK_Screen);
					break;
				}
				case 128:
				{
					SCREEN_PvrFullScreen(nPK_Screen);
					break;
				}
				case 129:
				{
					SCREEN_Epg(nPK_Screen);
					break;
				}
				case 130:
				{
					SCREEN_Help01(nPK_Screen);
					break;
				}
				case 131:
				{
					SCREEN_Help02(nPK_Screen);
					break;
				}
				case 132:
				{
					SCREEN_Help03(nPK_Screen);
					break;
				}
				case 133:
				{
					SCREEN_SetupMythTv01(nPK_Screen);
					break;
				}
				case 134:
				{
					SCREEN_SetupMythTv02(nPK_Screen);
					break;
				}
				case 135:
				{
					SCREEN_SetupMythTv03(nPK_Screen);
					break;
				}
				case 136:
				{
					SCREEN_SetupMythTv04(nPK_Screen);
					break;
				}
				case 137:
				{
					SCREEN_PvrFullScreenOpt(nPK_Screen);
					break;
				}
				case 138:
				{
					SCREEN_DVDFullScreen(nPK_Screen);
					break;
				}
				case 139:
				{
					SCREEN_DVDFullScreenOpt(nPK_Screen);
					break;
				}
				case 140:
				{
					SCREEN_VideosFullScreen(nPK_Screen);
					break;
				}
				case 141:
				{
					SCREEN_VideosFullScreenOpt(nPK_Screen);
					break;
				}
				case 142:
				{
					SCREEN_GenericAppFullScreen(nPK_Screen);
					break;
				}
				case 143:
				{
					SCREEN_DVDMenuFullScreen(nPK_Screen);
					break;
				}
				case 144:
				{
					SCREEN_DVDMenuFullScreenOpt(nPK_Screen);
					break;
				}
				case 145:
				{
					SCREEN_VlcFullScreen(nPK_Screen);
					break;
				}
				case 146:
				{
					SCREEN_VlcFullScreenOpt(nPK_Screen);
					break;
				}
				case 147:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_popLights(nPK_Screen, sLocation);
					break;
				}
				case 148:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_popMedia(nPK_Screen, sLocation);
					break;
				}
				case 149:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_popClimate(nPK_Screen, sLocation);
					break;
				}
				case 150:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_popTelecom(nPK_Screen, sLocation);
					break;
				}
				case 151:
				{
					string sLocation = pMessage->m_mapParameters[195];
					SCREEN_popSecurity(nPK_Screen, sLocation);
					break;
				}
				case 152:
				{
					SCREEN_popOthers(nPK_Screen);
					break;
				}
				case 153:
				{
					SCREEN_AudiCarSetup(nPK_Screen);
					break;
				}
				case 154:
				{
					SCREEN_AudiMedia(nPK_Screen);
					break;
				}
				case 155:
				{
					SCREEN_AudiComputing(nPK_Screen);
					break;
				}
				case 156:
				{
					SCREEN_AudiRadio(nPK_Screen);
					break;
				}
				case 157:
				{
					SCREEN_AudiTV(nPK_Screen);
					break;
				}
				case 158:
				{
					SCREEN_AudiCD(nPK_Screen);
					break;
				}
				case 159:
				{
					SCREEN_AudiStoredMedia(nPK_Screen);
					break;
				}
				case 160:
				{
					SCREEN_AudiInternet(nPK_Screen);
					break;
				}
				case 161:
				{
					SCREEN_AudiEmail(nPK_Screen);
					break;
				}
				case 162:
				{
					SCREEN_AudiAgenda(nPK_Screen);
					break;
				}
				case 163:
				{
					SCREEN_AudiOther(nPK_Screen);
					break;
				}
				case 164:
				{
					SCREEN_AudiNav(nPK_Screen);
					break;
				}
				case 165:
				{
					SCREEN_AudiInfo(nPK_Screen);
					break;
				}
				case 166:
				{
					SCREEN_AudiCar(nPK_Screen);
					break;
				}
				case 167:
				{
					SCREEN_AudiSetup(nPK_Screen);
					break;
				}
				case 168:
				{
					SCREEN_AudiLighting(nPK_Screen);
					break;
				}
				case 169:
				{
					SCREEN_AudiClimate(nPK_Screen);
					break;
				}
				case 170:
				{
					SCREEN_AudiSecurity(nPK_Screen);
					break;
				}
				case 171:
				{
					SCREEN_AudiTelecom(nPK_Screen);
					break;
				}
				case 172:
				{
					SCREEN_AudiFullScreen(nPK_Screen);
					break;
				}
				case 173:
				{
					SCREEN_AudiVideoRemote(nPK_Screen);
					break;
				}
				case 174:
				{
					SCREEN_AudiAudioRemote(nPK_Screen);
					break;
				}
				case 175:
				{
					SCREEN_VideoWizard(nPK_Screen);
					break;
				}
				case 176:
				{
					string sErrors = pMessage->m_mapParameters[18];
					SCREEN_DialogCannotPlayMedia(nPK_Screen, sErrors);
					break;
				}
				case 177:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sPromptToResetRouter = pMessage->m_mapParameters[181];
					string sTimeout = pMessage->m_mapParameters[182];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_DialogGenericNoButtons(nPK_Screen, sDescription, sPromptToResetRouter, sTimeout, sCannotGoBack);
					break;
				}
				case 178:
				{
					string sPK_Device_From = pMessage->m_mapParameters[184];
					string sPK_Device_Ripping = pMessage->m_mapParameters[185];
					SCREEN_DialogRippingInProgress(nPK_Screen, sPK_Device_From, sPK_Device_Ripping);
					break;
				}
				case 179:
				{
					SCREEN_DialogCheckingDrive(nPK_Screen);
					break;
				}
				case 180:
				{
					SCREEN_DialogUnableToSavePlaylist(nPK_Screen);
					break;
				}
				case 181:
				{
					SCREEN_DialogPlaylistSaved(nPK_Screen);
					break;
				}
				case 182:
				{
					SCREEN_DialogUnableToLoadPlaylist(nPK_Screen);
					break;
				}
				case 183:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sTimeout = pMessage->m_mapParameters[182];
					SCREEN_DialogRippingError(nPK_Screen, sDescription, sTimeout);
					break;
				}
				case 184:
				{
					SCREEN_DialogRippingInstructions(nPK_Screen);
					break;
				}
				case 185:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sPromptToResetRouter = pMessage->m_mapParameters[181];
					string sTimeout = pMessage->m_mapParameters[182];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_DialogGenericError(nPK_Screen, sDescription, sPromptToResetRouter, sTimeout, sCannotGoBack);
					break;
				}
				case 186:
				{
					string sErrors = pMessage->m_mapParameters[18];
					SCREEN_DialogCannotBookmark(nPK_Screen, sErrors);
					break;
				}
				case 187:
				{
					int iPK_Screen = atoi(pMessage->m_mapParameters[159].c_str());
					string sPK_Device_From = pMessage->m_mapParameters[184];
					string sPK_Device_MediaSource = pMessage->m_mapParameters[186];
					string sStreamID_String = pMessage->m_mapParameters[187];
					string sPosition = pMessage->m_mapParameters[188];
					string sUsers = pMessage->m_mapParameters[189];
					string sPK_MediaType_String = pMessage->m_mapParameters[190];
					SCREEN_DialogAskToResume(nPK_Screen, iPK_Screen, sPK_Device_From, sPK_Device_MediaSource, sStreamID_String, sPosition, sUsers, sPK_MediaType_String);
					break;
				}
				case 188:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sCannotGoBack = pMessage->m_mapParameters[183];
					SCREEN_DialogGC100Error(nPK_Screen, sDescription, sCannotGoBack);
					break;
				}
				case 189:
				{
					string sDescription = pMessage->m_mapParameters[163];
					string sPhoneName = pMessage->m_mapParameters[191];
					SCREEN_DialogPhoneInstructions(nPK_Screen, sDescription, sPhoneName);
					break;
				}
				case 190:
				{
					string sMac_address = pMessage->m_mapParameters[47];
					string sCommand_Line = pMessage->m_mapParameters[137];
					string sPK_Device_From = pMessage->m_mapParameters[184];
					string sPhoneName = pMessage->m_mapParameters[191];
					string sPK_Device_AppServer = pMessage->m_mapParameters[192];
					SCREEN_DialogSendFileToPhoneFailed(nPK_Screen, sMac_address, sCommand_Line, sPK_Device_From, sPhoneName, sPK_Device_AppServer);
					break;
				}
				case 191:
				{
					SCREEN_AudiHome(nPK_Screen);
					break;
				}
				case 192:
				{
					SCREEN_RoomsWizard(nPK_Screen);
					break;
				}
				case 193:
				{
					SCREEN_UsersWizard(nPK_Screen);
					break;
				}
				case 194:
				{
					SCREEN_CountryWizard(nPK_Screen);
					break;
				}
				case 195:
				{
					SCREEN_TV_provider(nPK_Screen);
					break;
				}
				case 196:
				{
					SCREEN_TV_Manufacturer(nPK_Screen);
					break;
				}
				case 197:
				{
					SCREEN_Receiver(nPK_Screen);
					break;
				}
				case 198:
				{
					SCREEN_LightsSetup(nPK_Screen);
					break;
				}
				case 199:
				{
					SCREEN_AlarmPanel(nPK_Screen);
					break;
				}
				case 200:
				{
					SCREEN_VOIP_Provider(nPK_Screen);
					break;
				}
				case 201:
				{
					SCREEN_AV_Devices(nPK_Screen);
					break;
				}
				case 202:
				{
					int iPK_PnpQueue = atoi(pMessage->m_mapParameters[224].c_str());
					SCREEN_NAS_Options(nPK_Screen, iPK_PnpQueue);
					break;
				}
				case 203:
				{
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					string sPhoneName = pMessage->m_mapParameters[191];
					SCREEN_New_Phone_Enter_Number(nPK_Screen, iPK_Device, sPhoneName);
					break;
				}
				case 204:
				{
					SCREEN_Need_Reload_Router(nPK_Screen);
					break;
				}
				case 205:
				{
					SCREEN_Need_Regen_Orbiter(nPK_Screen);
					break;
				}
				case 206:
				{
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					SCREEN_Sensors_Viewed_By_Camera(nPK_Screen, iPK_Device);
					break;
				}
				case 207:
				{
					SCREEN_mnuPVROSD(nPK_Screen);
					break;
				}
				case 208:
				{
					SCREEN_mnuPVRRecording(nPK_Screen);
					break;
				}
				case 209:
				{
					SCREEN_Demo_Media(nPK_Screen);
					break;
				}
				case 210:
				{
					int iPK_Device = atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_Device_AppServer = pMessage->m_mapParameters[192];
					SCREEN_Download_are_ready_to_install(nPK_Screen, iPK_Device, sPK_Device_AppServer);
					break;
				}
				case 211:
				{
					SCREEN_mnuMainMenu2(nPK_Screen);
					break;
				}
				case 212:
				{
					SCREEN_mnuAmbiance(nPK_Screen);
					break;
				}
				case 215:
				{
					SCREEN_TVConfirmInputsDiscrete(nPK_Screen);
					break;
				}
				case 216:
				{
					SCREEN_TVConfirmInputsToggle(nPK_Screen);
					break;
				}
				case 217:
				{
					SCREEN_TVOnOffCodes(nPK_Screen);
					break;
				}
				case 218:
				{
					SCREEN_mnuPVR_OSD_Full_Screen(nPK_Screen);
					break;
				}
				case 219:
				{
					SCREEN_mnuPVRRecording_Full_Screen(nPK_Screen);
					break;
				}
				case 220:
				{
					SCREEN_CurrentlyActiveRemote(nPK_Screen);
					break;
				}
				case 221:
				{
					SCREEN_TVMultipleInputs(nPK_Screen);
					break;
				}
				case 222:
				{
					SCREEN_mnuLights(nPK_Screen);
					break;
				}
				case 223:
				{
					SCREEN_mnuPlaybackControl(nPK_Screen);
					break;
				}
				case 224:
				{
					SCREEN_mnuSpeedControl(nPK_Screen);
					break;
				}
				case 225:
				{
					SCREEN_mnuVolume(nPK_Screen);
					break;
				}
				case 226:
				{
					SCREEN_TVConfirmOnOffTogle(nPK_Screen);
					break;
				}
				case 227:
				{
					SCREEN_TVConfirmOnOffDiscret(nPK_Screen);
					break;
				}
				case 228:
				{
					string sData_String = pMessage->m_mapParameters[109];
					int iPK_Device_ControlledVia = atoi(pMessage->m_mapParameters[156].c_str());
					string sDescription = pMessage->m_mapParameters[163];
					SCREEN_Internal_Disk_Drive_Wizard(nPK_Screen, sData_String, iPK_Device_ControlledVia, sDescription);
					break;
				}
				case 229:
				{
					SCREEN_TVDSPMode(nPK_Screen);
					break;
				}
				case 230:
				{
					SCREEN_Media_Tracks(nPK_Screen);
					break;
				}
				case 231:
				{
					SCREEN_Which_Wizard(nPK_Screen);
					break;
				}

			default:
				BadGotoScreen(nPK_Screen);
				break;
			}
		}
	};
}

#endif
