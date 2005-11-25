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
	class SCREEN_mnuMain : public PreformedCommand
	{
	public:
		SCREEN_mnuMain(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "1" /* screen ID */);
		}
	};

	class SCREEN_mnuLights : public PreformedCommand
	{
	public:
		SCREEN_mnuLights(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "2" /* screen ID */);
		}
	};

	class SCREEN_mnuMedia : public PreformedCommand
	{
	public:
		SCREEN_mnuMedia(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "3" /* screen ID */);
		}
	};

	class SCREEN_mnuClimate : public PreformedCommand
	{
	public:
		SCREEN_mnuClimate(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "4" /* screen ID */);
		}
	};

	class SCREEN_mnuSecurity : public PreformedCommand
	{
	public:
		SCREEN_mnuSecurity(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "5" /* screen ID */);
		}
	};

	class SCREEN_mnuTelephony : public PreformedCommand
	{
	public:
		SCREEN_mnuTelephony(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "6" /* screen ID */);
		}
	};

	class SCREEN_mnuMisc : public PreformedCommand
	{
	public:
		SCREEN_mnuMisc(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "7" /* screen ID */);
		}
	};

	class SCREEN_mnuUserStatus : public PreformedCommand
	{
	public:
		SCREEN_mnuUserStatus(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "8" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallPhonebook : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallPhonebook(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "9" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallFavorites : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallFavorites(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "10" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallDialNumber : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallDialNumber(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "11" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallPlutoUser : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallPlutoUser(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "12" /* screen ID */);
		}
	};

	class SCREEN_mnuSecurityPanel : public PreformedCommand
	{
	public:
		SCREEN_mnuSecurityPanel(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "13" /* screen ID */);
		}
	};

	class SCREEN_mnuSecurityStatusReport : public PreformedCommand
	{
	public:
		SCREEN_mnuSecurityStatusReport(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "14" /* screen ID */);
		}
	};

	class SCREEN_mnuSingleCameraViewOnly : public PreformedCommand
	{
	public:
		SCREEN_mnuSingleCameraViewOnly(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "15" /* screen ID */);
		}
	};

	class SCREEN_mnuIntercom : public PreformedCommand
	{
	public:
		SCREEN_mnuIntercom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "16" /* screen ID */);
		}
	};

	class SCREEN_mnuQuadViewCameras : public PreformedCommand
	{
	public:
		SCREEN_mnuQuadViewCameras(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "17" /* screen ID */);
		}
	};

	class SCREEN_mnuVideoCallInProgress : public PreformedCommand
	{
	public:
		SCREEN_mnuVideoCallInProgress(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "18" /* screen ID */);
		}
	};

	class SCREEN_mnuFileSave : public PreformedCommand
	{
	public:
		SCREEN_mnuFileSave(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "19" /* screen ID */);
		}
	};

	class SCREEN_mnuManagePlaylist : public PreformedCommand
	{
	public:
		SCREEN_mnuManagePlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "20" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallDevice : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallDevice(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "21" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallPhonebook2 : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallPhonebook2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "22" /* screen ID */);
		}
	};

	class SCREEN_mnuMakeCallIntercom : public PreformedCommand
	{
	public:
		SCREEN_mnuMakeCallIntercom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "23" /* screen ID */);
		}
	};

	class SCREEN_mnuCurrentUser : public PreformedCommand
	{
	public:
		SCREEN_mnuCurrentUser(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "24" /* screen ID */);
		}
	};

	class SCREEN_mnuCDTrackCopy : public PreformedCommand
	{
	public:
		SCREEN_mnuCDTrackCopy(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "25" /* screen ID */);
		}
	};

	class SCREEN_mnuCDRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuCDRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "26" /* screen ID */);
		}
	};

	class SCREEN_mnuVoicemailMain : public PreformedCommand
	{
	public:
		SCREEN_mnuVoicemailMain(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "27" /* screen ID */);
		}
	};

	class SCREEN_mnuTransferAllUsers : public PreformedCommand
	{
	public:
		SCREEN_mnuTransferAllUsers(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "28" /* screen ID */);
		}
	};

	class SCREEN_mnuSleeping : public PreformedCommand
	{
	public:
		SCREEN_mnuSleeping(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "29" /* screen ID */);
		}
	};

	class SCREEN_mnuControllerStatus : public PreformedCommand
	{
	public:
		SCREEN_mnuControllerStatus(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "30" /* screen ID */);
		}
	};

	class SCREEN_mnuOtherFolders : public PreformedCommand
	{
	public:
		SCREEN_mnuOtherFolders(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "31" /* screen ID */);
		}
	};

	class SCREEN_mnuConfirmDelete : public PreformedCommand
	{
	public:
		SCREEN_mnuConfirmDelete(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "32" /* screen ID */);
		}
	};

	class SCREEN_mnuWebURL : public PreformedCommand
	{
	public:
		SCREEN_mnuWebURL(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "33" /* screen ID */);
		}
	};

	class SCREEN_mnuBrowser : public PreformedCommand
	{
	public:
		SCREEN_mnuBrowser(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "34" /* screen ID */);
		}
	};

	class SCREEN_mnuModeChanged : public PreformedCommand
	{
	public:
		SCREEN_mnuModeChanged(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "35" /* screen ID */);
		}
	};

	class SCREEN_mnuSensorsNotReady : public PreformedCommand
	{
	public:
		SCREEN_mnuSensorsNotReady(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "36" /* screen ID */);
		}
	};

	class SCREEN_mnuRecordedTVShows : public PreformedCommand
	{
	public:
		SCREEN_mnuRecordedTVShows(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "37" /* screen ID */);
		}
	};

	class SCREEN_mnuDeviceControl : public PreformedCommand
	{
	public:
		SCREEN_mnuDeviceControl(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "38" /* screen ID */);
		}
	};

	class SCREEN_mnuComputing : public PreformedCommand
	{
	public:
		SCREEN_mnuComputing(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "39" /* screen ID */);
		}
	};

	class SCREEN_mnuOpenFile : public PreformedCommand
	{
	public:
		SCREEN_mnuOpenFile(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "40" /* screen ID */);
		}
	};

	class SCREEN_mnuBrowserConsole : public PreformedCommand
	{
	public:
		SCREEN_mnuBrowserConsole(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "41" /* screen ID */);
		}
	};

	class SCREEN_mnuFavoritesPDA : public PreformedCommand
	{
	public:
		SCREEN_mnuFavoritesPDA(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "42" /* screen ID */);
		}
	};

	class SCREEN_mnuPCFileTools : public PreformedCommand
	{
	public:
		SCREEN_mnuPCFileTools(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "43" /* screen ID */);
		}
	};

	class SCREEN_mnuAdvancedOptions : public PreformedCommand
	{
	public:
		SCREEN_mnuAdvancedOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "44" /* screen ID */);
		}
	};

	class SCREEN_mnuMakingCall : public PreformedCommand
	{
	public:
		SCREEN_mnuMakingCall(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "45" /* screen ID */);
		}
	};

	class SCREEN_mnuMakingLinPhoneBroadcast : public PreformedCommand
	{
	public:
		SCREEN_mnuMakingLinPhoneBroadcast(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "46" /* screen ID */);
		}
	};

	class SCREEN_mnuFileList_Music_Movies_Video : public PreformedCommand
	{
	public:
		SCREEN_mnuFileList_Music_Movies_Video(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "47" /* screen ID */);
		}
	};

	class SCREEN_mnuPVR : public PreformedCommand
	{
	public:
		SCREEN_mnuPVR(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "48" /* screen ID */);
		}
	};

	class SCREEN_mnuDVDRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuDVDRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "49" /* screen ID */);
		}
	};

	class SCREEN_mnuDVDMenu : public PreformedCommand
	{
	public:
		SCREEN_mnuDVDMenu(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "50" /* screen ID */);
		}
	};

	class SCREEN_mnuHouseAtAGlance : public PreformedCommand
	{
	public:
		SCREEN_mnuHouseAtAGlance(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "51" /* screen ID */);
		}
	};

	class SCREEN_mnuAdvancedMediaOptions : public PreformedCommand
	{
	public:
		SCREEN_mnuAdvancedMediaOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "52" /* screen ID */);
		}
	};

	class SCREEN_mnuPopupMessage : public PreformedCommand
	{
	public:
		SCREEN_mnuPopupMessage(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "53" /* screen ID */);
		}
	};

	class SCREEN_mnuMusicRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuMusicRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "54" /* screen ID */);
		}
	};

	class SCREEN_mnuLinPhones : public PreformedCommand
	{
	public:
		SCREEN_mnuLinPhones(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "55" /* screen ID */);
		}
	};

	class SCREEN_mnuOrbiters : public PreformedCommand
	{
	public:
		SCREEN_mnuOrbiters(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "56" /* screen ID */);
		}
	};

	class SCREEN_mnuPhones : public PreformedCommand
	{
	public:
		SCREEN_mnuPhones(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "57" /* screen ID */);
		}
	};

	class SCREEN_mnuDVDOptions : public PreformedCommand
	{
	public:
		SCREEN_mnuDVDOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "58" /* screen ID */);
		}
	};

	class SCREEN_mnuAnswertoLinPhones : public PreformedCommand
	{
	public:
		SCREEN_mnuAnswertoLinPhones(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "59" /* screen ID */);
		}
	};

	class SCREEN_mnuUserPinCode : public PreformedCommand
	{
	public:
		SCREEN_mnuUserPinCode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "60" /* screen ID */);
		}
	};

	class SCREEN_mnuPreviewOptions : public PreformedCommand
	{
	public:
		SCREEN_mnuPreviewOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "61" /* screen ID */);
		}
	};

	class SCREEN_mnuTransferOutside : public PreformedCommand
	{
	public:
		SCREEN_mnuTransferOutside(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "62" /* screen ID */);
		}
	};

	class SCREEN_mnuSatelliteCableBox : public PreformedCommand
	{
	public:
		SCREEN_mnuSatelliteCableBox(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "63" /* screen ID */);
		}
	};

	class SCREEN_mnuCurrentLocation : public PreformedCommand
	{
	public:
		SCREEN_mnuCurrentLocation(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "64" /* screen ID */);
		}
	};

	class SCREEN_mnuMediaAttributeSearchInput : public PreformedCommand
	{
	public:
		SCREEN_mnuMediaAttributeSearchInput(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "65" /* screen ID */);
		}
	};

	class SCREEN_mnuMediaAttributeSearchResult : public PreformedCommand
	{
	public:
		SCREEN_mnuMediaAttributeSearchResult(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "66" /* screen ID */);
		}
	};

	class SCREEN_mnuNewPhoneDetected : public PreformedCommand
	{
	public:
		SCREEN_mnuNewPhoneDetected(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "67" /* screen ID */);
		}
	};

	class SCREEN_mnuFirstTime : public PreformedCommand
	{
	public:
		SCREEN_mnuFirstTime(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "68" /* screen ID */);
		}
	};

	class SCREEN_mnuFileList_PlayLists : public PreformedCommand
	{
	public:
		SCREEN_mnuFileList_PlayLists(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "69" /* screen ID */);
		}
	};

	class SCREEN_mnuVideosRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuVideosRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "70" /* screen ID */);
		}
	};

	class SCREEN_mnuFileList_Pictures_Docs : public PreformedCommand
	{
	public:
		SCREEN_mnuFileList_Pictures_Docs(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "71" /* screen ID */);
		}
	};

	class SCREEN_mnuGenericTwoButtonQuestion : public PreformedCommand
	{
	public:
		SCREEN_mnuGenericTwoButtonQuestion(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "72" /* screen ID */);
		}
	};

	class SCREEN_mnuTryCodes : public PreformedCommand
	{
	public:
		SCREEN_mnuTryCodes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "73" /* screen ID */);
		}
	};

	class SCREEN_mnuScreenSaver : public PreformedCommand
	{
	public:
		SCREEN_mnuScreenSaver(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "74" /* screen ID */);
		}
	};

	class SCREEN_mnuNewPlugAndPlayDevice : public PreformedCommand
	{
	public:
		SCREEN_mnuNewPlugAndPlayDevice(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "75" /* screen ID */);
		}
	};

	class SCREEN_mnuPower : public PreformedCommand
	{
	public:
		SCREEN_mnuPower(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "76" /* screen ID */);
		}
	};

	class SCREEN_mnuGenericDvdRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuGenericDvdRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "77" /* screen ID */);
		}
	};

	class SCREEN_mnuViewPlaylist : public PreformedCommand
	{
	public:
		SCREEN_mnuViewPlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "78" /* screen ID */);
		}
	};

	class SCREEN_mnuFileList_withPictures_Mobil : public PreformedCommand
	{
	public:
		SCREEN_mnuFileList_withPictures_Mobil(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "79" /* screen ID */);
		}
	};

	class SCREEN_mnuGenericDvdMenu : public PreformedCommand
	{
	public:
		SCREEN_mnuGenericDvdMenu(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "80" /* screen ID */);
		}
	};

	class SCREEN_mnuViewAttributes : public PreformedCommand
	{
	public:
		SCREEN_mnuViewAttributes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "81" /* screen ID */);
		}
	};

	class SCREEN_mnuPendingTasks : public PreformedCommand
	{
	public:
		SCREEN_mnuPendingTasks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "82" /* screen ID */);
		}
	};

	class SCREEN_mnuAddAttribute : public PreformedCommand
	{
	public:
		SCREEN_mnuAddAttribute(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "83" /* screen ID */);
		}
	};

	class SCREEN_mnuRadioRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuRadioRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "84" /* screen ID */);
		}
	};

	class SCREEN_mnuGenericCDRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuGenericCDRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "85" /* screen ID */);
		}
	};

	class SCREEN_mnuMainMenuPopUp : public PreformedCommand
	{
	public:
		SCREEN_mnuMainMenuPopUp(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "86" /* screen ID */);
		}
	};

	class SCREEN_mnuFileList__Popup_Container : public PreformedCommand
	{
	public:
		SCREEN_mnuFileList__Popup_Container(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "87" /* screen ID */);
		}
	};

	class SCREEN_mnuCreateViewBookmarks : public PreformedCommand
	{
	public:
		SCREEN_mnuCreateViewBookmarks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "88" /* screen ID */);
		}
	};

	class SCREEN_mnuBrowseBookmarks : public PreformedCommand
	{
	public:
		SCREEN_mnuBrowseBookmarks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "89" /* screen ID */);
		}
	};

	class SCREEN_mnuGenericAppController : public PreformedCommand
	{
	public:
		SCREEN_mnuGenericAppController(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "90" /* screen ID */);
		}
	};

	class SCREEN_mnuBookmarksByMediaType : public PreformedCommand
	{
	public:
		SCREEN_mnuBookmarksByMediaType(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "91" /* screen ID */);
		}
	};

	class SCREEN_mnuMainAudi : public PreformedCommand
	{
	public:
		SCREEN_mnuMainAudi(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "92" /* screen ID */);
		}
	};

	class SCREEN_mnuMythTvRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuMythTvRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "93" /* screen ID */);
		}
	};

	class SCREEN_mnuMyth_all_commands : public PreformedCommand
	{
	public:
		SCREEN_mnuMyth_all_commands(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "94" /* screen ID */);
		}
	};

	class SCREEN_mnuMytv_watching_tv : public PreformedCommand
	{
	public:
		SCREEN_mnuMytv_watching_tv(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "95" /* screen ID */);
		}
	};

	class SCREEN_mnuMyth_live_tv_browse_mode : public PreformedCommand
	{
	public:
		SCREEN_mnuMyth_live_tv_browse_mode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "96" /* screen ID */);
		}
	};

	class SCREEN_mnuMyth_playback_recording : public PreformedCommand
	{
	public:
		SCREEN_mnuMyth_playback_recording(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "97" /* screen ID */);
		}
	};

	class SCREEN_mnuGenericKeyboard : public PreformedCommand
	{
	public:
		SCREEN_mnuGenericKeyboard(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "98" /* screen ID */);
		}
	};

	class SCREEN_mnuPingPong : public PreformedCommand
	{
	public:
		SCREEN_mnuPingPong(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "99" /* screen ID */);
		}
	};

	class SCREEN_mnuTVEPG1 : public PreformedCommand
	{
	public:
		SCREEN_mnuTVEPG1(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "100" /* screen ID */);
		}
	};

	class SCREEN_mnuTVEPG2 : public PreformedCommand
	{
	public:
		SCREEN_mnuTVEPG2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "101" /* screen ID */);
		}
	};

	class SCREEN_mnuTVEPG3 : public PreformedCommand
	{
	public:
		SCREEN_mnuTVEPG3(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "102" /* screen ID */);
		}
	};

	class SCREEN_mnuShowInfo : public PreformedCommand
	{
	public:
		SCREEN_mnuShowInfo(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "103" /* screen ID */);
		}
	};

	class SCREEN_mnuCreateViewBookmarksTV : public PreformedCommand
	{
	public:
		SCREEN_mnuCreateViewBookmarksTV(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "104" /* screen ID */);
		}
	};

	class SCREEN_mnuMainMenuSmall : public PreformedCommand
	{
	public:
		SCREEN_mnuMainMenuSmall(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "105" /* screen ID */);
		}
	};

	class SCREEN_mnuWebSites : public PreformedCommand
	{
	public:
		SCREEN_mnuWebSites(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "109" /* screen ID */);
		}
	};

	class SCREEN_mnuWhatModelMobileOrbiter : public PreformedCommand
	{
	public:
		SCREEN_mnuWhatModelMobileOrbiter(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "110" /* screen ID */);
		}
	};

	class SCREEN_mnuLighting : public PreformedCommand
	{
	public:
		SCREEN_mnuLighting(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "111" /* screen ID */);
		}
	};

	class SCREEN_mnuTelecom : public PreformedCommand
	{
	public:
		SCREEN_mnuTelecom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "114" /* screen ID */);
		}
	};

	class SCREEN_mnuMore : public PreformedCommand
	{
	public:
		SCREEN_mnuMore(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "117" /* screen ID */);
		}
	};

	class SCREEN_mnuFilelist : public PreformedCommand
	{
	public:
		SCREEN_mnuFilelist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "120" /* screen ID */);
		}
	};

	class SCREEN_mnuDVDmenu : public PreformedCommand
	{
	public:
		SCREEN_mnuDVDmenu(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "121" /* screen ID */);
		}
	};

	class SCREEN_mnuAttributes : public PreformedCommand
	{
	public:
		SCREEN_mnuAttributes(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "122" /* screen ID */);
		}
	};

	class SCREEN_mnuDVDoptions : public PreformedCommand
	{
	public:
		SCREEN_mnuDVDoptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "123" /* screen ID */);
		}
	};

	class SCREEN_mnuAdvRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuAdvRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "124" /* screen ID */);
		}
	};

	class SCREEN_mnuPlaylist : public PreformedCommand
	{
	public:
		SCREEN_mnuPlaylist(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "125" /* screen ID */);
		}
	};

	class SCREEN_mnuCreateBookmarks : public PreformedCommand
	{
	public:
		SCREEN_mnuCreateBookmarks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "126" /* screen ID */);
		}
	};

	class SCREEN_mnuAudioOptions : public PreformedCommand
	{
	public:
		SCREEN_mnuAudioOptions(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "129" /* screen ID */);
		}
	};

	class SCREEN_mnuSubtitles : public PreformedCommand
	{
	public:
		SCREEN_mnuSubtitles(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "130" /* screen ID */);
		}
	};

	class SCREEN_mnuAngles : public PreformedCommand
	{
	public:
		SCREEN_mnuAngles(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "131" /* screen ID */);
		}
	};

	class SCREEN_mnuNewMacAddress : public PreformedCommand
	{
	public:
		SCREEN_mnuNewMacAddress(long DeviceIDFrom, long DeviceIDTo,
			string sMessage1)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 2, 
				COMMANDPARAMETER_EK_Screen_CONST, "132" /* screen ID */,
				58 /* command parm for message 1 */, sMessage1.c_str());
		}
	};

	class SCREEN_mnuVdrRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuVdrRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "133" /* screen ID */);
		}
	};

	class SCREEN_mnuViewBookmarks : public PreformedCommand
	{
	public:
		SCREEN_mnuViewBookmarks(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "134" /* screen ID */);
		}
	};

	class SCREEN_mnuCdRemote : public PreformedCommand
	{
	public:
		SCREEN_mnuCdRemote(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "137" /* screen ID */);
		}
	};

	class SCREEN_mnuSingleCameraView : public PreformedCommand
	{
	public:
		SCREEN_mnuSingleCameraView(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "140" /* screen ID */);
		}
	};

	class SCREEN_mnuNewPhoneDeected : public PreformedCommand
	{
	public:
		SCREEN_mnuNewPhoneDeected(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "145" /* screen ID */);
		}
	};

	class SCREEN_mnuPowerFile : public PreformedCommand
	{
	public:
		SCREEN_mnuPowerFile(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "146" /* screen ID */);
		}
	};

	class SCREEN_devCallInProgress : public PreformedCommand
	{
	public:
		SCREEN_devCallInProgress(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "177" /* screen ID */);
		}
	};

	class SCREEN_devIncomingCall : public PreformedCommand
	{
	public:
		SCREEN_devIncomingCall(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "178" /* screen ID */);
		}
	};

	class SCREEN_devMoveCallsInProgress : public PreformedCommand
	{
	public:
		SCREEN_devMoveCallsInProgress(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "179" /* screen ID */);
		}
	};

	class SCREEN_mapSecurity : public PreformedCommand
	{
	public:
		SCREEN_mapSecurity(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "180" /* screen ID */);
		}
	};

	class SCREEN_mapLighting : public PreformedCommand
	{
	public:
		SCREEN_mapLighting(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "181" /* screen ID */);
		}
	};

	class SCREEN_mapClimate : public PreformedCommand
	{
	public:
		SCREEN_mapClimate(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "182" /* screen ID */);
		}
	};

	class SCREEN_mapTelecom : public PreformedCommand
	{
	public:
		SCREEN_mapTelecom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "183" /* screen ID */);
		}
	};

	class SCREEN_navmode : public PreformedCommand
	{
	public:
		SCREEN_navmode(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "184" /* screen ID */);
		}
	};

	class SCREEN_pvr_full_screen : public PreformedCommand
	{
	public:
		SCREEN_pvr_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "185" /* screen ID */);
		}
	};

	class SCREEN_epg : public PreformedCommand
	{
	public:
		SCREEN_epg(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "186" /* screen ID */);
		}
	};

	class SCREEN_help1 : public PreformedCommand
	{
	public:
		SCREEN_help1(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "187" /* screen ID */);
		}
	};

	class SCREEN_help2 : public PreformedCommand
	{
	public:
		SCREEN_help2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "188" /* screen ID */);
		}
	};

	class SCREEN_help3 : public PreformedCommand
	{
	public:
		SCREEN_help3(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "189" /* screen ID */);
		}
	};

	class SCREEN_setup_myth : public PreformedCommand
	{
	public:
		SCREEN_setup_myth(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "190" /* screen ID */);
		}
	};

	class SCREEN_setup_myth_2 : public PreformedCommand
	{
	public:
		SCREEN_setup_myth_2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "191" /* screen ID */);
		}
	};

	class SCREEN_myth_tv_setup : public PreformedCommand
	{
	public:
		SCREEN_myth_tv_setup(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "192" /* screen ID */);
		}
	};

	class SCREEN_myth_tv_setup_2 : public PreformedCommand
	{
	public:
		SCREEN_myth_tv_setup_2(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "193" /* screen ID */);
		}
	};

	class SCREEN_pvr_full_screen_with_options_o : public PreformedCommand
	{
	public:
		SCREEN_pvr_full_screen_with_options_o(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "194" /* screen ID */);
		}
	};

	class SCREEN_dvd_full_screen : public PreformedCommand
	{
	public:
		SCREEN_dvd_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "195" /* screen ID */);
		}
	};

	class SCREEN_dvd_full_screen_with_options : public PreformedCommand
	{
	public:
		SCREEN_dvd_full_screen_with_options(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "196" /* screen ID */);
		}
	};

	class SCREEN_storedvideos_full_screen : public PreformedCommand
	{
	public:
		SCREEN_storedvideos_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "197" /* screen ID */);
		}
	};

	class SCREEN_storedvideos_full_screen_with : public PreformedCommand
	{
	public:
		SCREEN_storedvideos_full_screen_with(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "198" /* screen ID */);
		}
	};

	class SCREEN_generic_app_full_screen : public PreformedCommand
	{
	public:
		SCREEN_generic_app_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "199" /* screen ID */);
		}
	};

	class SCREEN_dvd_menu_full_screen : public PreformedCommand
	{
	public:
		SCREEN_dvd_menu_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "200" /* screen ID */);
		}
	};

	class SCREEN_dvd_menu_full_screen_with_opti : public PreformedCommand
	{
	public:
		SCREEN_dvd_menu_full_screen_with_opti(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "201" /* screen ID */);
		}
	};

	class SCREEN_vlc_dvd_full_screen : public PreformedCommand
	{
	public:
		SCREEN_vlc_dvd_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "202" /* screen ID */);
		}
	};

	class SCREEN_vlc_dvd_full_screen_with_optio : public PreformedCommand
	{
	public:
		SCREEN_vlc_dvd_full_screen_with_optio(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "203" /* screen ID */);
		}
	};

	class SCREEN_popLights : public PreformedCommand
	{
	public:
		SCREEN_popLights(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "204" /* screen ID */);
		}
	};

	class SCREEN_popMedia : public PreformedCommand
	{
	public:
		SCREEN_popMedia(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "205" /* screen ID */);
		}
	};

	class SCREEN_popClimate : public PreformedCommand
	{
	public:
		SCREEN_popClimate(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "206" /* screen ID */);
		}
	};

	class SCREEN_popTelecom : public PreformedCommand
	{
	public:
		SCREEN_popTelecom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "207" /* screen ID */);
		}
	};

	class SCREEN_popSecurity : public PreformedCommand
	{
	public:
		SCREEN_popSecurity(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "208" /* screen ID */);
		}
	};

	class SCREEN_popOthers : public PreformedCommand
	{
	public:
		SCREEN_popOthers(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "209" /* screen ID */);
		}
	};

	class SCREEN_Marbella : public PreformedCommand
	{
	public:
		SCREEN_Marbella(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "210" /* screen ID */);
		}
	};

	class SCREEN_home : public PreformedCommand
	{
	public:
		SCREEN_home(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "211" /* screen ID */);
		}
	};

	class SCREEN_car_setup : public PreformedCommand
	{
	public:
		SCREEN_car_setup(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "212" /* screen ID */);
		}
	};

	class SCREEN_media : public PreformedCommand
	{
	public:
		SCREEN_media(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "213" /* screen ID */);
		}
	};

	class SCREEN_computing : public PreformedCommand
	{
	public:
		SCREEN_computing(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "214" /* screen ID */);
		}
	};

	class SCREEN_radio : public PreformedCommand
	{
	public:
		SCREEN_radio(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "215" /* screen ID */);
		}
	};

	class SCREEN_tv : public PreformedCommand
	{
	public:
		SCREEN_tv(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "216" /* screen ID */);
		}
	};

	class SCREEN_cd : public PreformedCommand
	{
	public:
		SCREEN_cd(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "217" /* screen ID */);
		}
	};

	class SCREEN_hard_drive : public PreformedCommand
	{
	public:
		SCREEN_hard_drive(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "218" /* screen ID */);
		}
	};

	class SCREEN_Internet : public PreformedCommand
	{
	public:
		SCREEN_Internet(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "219" /* screen ID */);
		}
	};

	class SCREEN_Email : public PreformedCommand
	{
	public:
		SCREEN_Email(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "220" /* screen ID */);
		}
	};

	class SCREEN_Agenda : public PreformedCommand
	{
	public:
		SCREEN_Agenda(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "221" /* screen ID */);
		}
	};

	class SCREEN_Other : public PreformedCommand
	{
	public:
		SCREEN_Other(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "222" /* screen ID */);
		}
	};

	class SCREEN_Nav : public PreformedCommand
	{
	public:
		SCREEN_Nav(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "223" /* screen ID */);
		}
	};

	class SCREEN_Info : public PreformedCommand
	{
	public:
		SCREEN_Info(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "224" /* screen ID */);
		}
	};

	class SCREEN_Car : public PreformedCommand
	{
	public:
		SCREEN_Car(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "225" /* screen ID */);
		}
	};

	class SCREEN_Setup : public PreformedCommand
	{
	public:
		SCREEN_Setup(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "226" /* screen ID */);
		}
	};

	class SCREEN_Lighting : public PreformedCommand
	{
	public:
		SCREEN_Lighting(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "227" /* screen ID */);
		}
	};

	class SCREEN_Climate : public PreformedCommand
	{
	public:
		SCREEN_Climate(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "228" /* screen ID */);
		}
	};

	class SCREEN_Security : public PreformedCommand
	{
	public:
		SCREEN_Security(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "229" /* screen ID */);
		}
	};

	class SCREEN_Telecom : public PreformedCommand
	{
	public:
		SCREEN_Telecom(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "230" /* screen ID */);
		}
	};

	class SCREEN_full_screen : public PreformedCommand
	{
	public:
		SCREEN_full_screen(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "231" /* screen ID */);
		}
	};

	class SCREEN_video_hard_drive : public PreformedCommand
	{
	public:
		SCREEN_video_hard_drive(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "232" /* screen ID */);
		}
	};

	class SCREEN_audio_hard_drive : public PreformedCommand
	{
	public:
		SCREEN_audio_hard_drive(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "233" /* screen ID */);
		}
	};

	class SCREEN_step01 : public PreformedCommand
	{
	public:
		SCREEN_step01(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "234" /* screen ID */);
		}
	};

	class SCREEN_step02 : public PreformedCommand
	{
	public:
		SCREEN_step02(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "235" /* screen ID */);
		}
	};

	class SCREEN_step03 : public PreformedCommand
	{
	public:
		SCREEN_step03(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "236" /* screen ID */);
		}
	};

	class SCREEN_step04 : public PreformedCommand
	{
	public:
		SCREEN_step04(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "237" /* screen ID */);
		}
	};

	class SCREEN_step05 : public PreformedCommand
	{
	public:
		SCREEN_step05(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "238" /* screen ID */);
		}
	};

	class SCREEN_step06 : public PreformedCommand
	{
	public:
		SCREEN_step06(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "239" /* screen ID */);
		}
	};

	class SCREEN_step07 : public PreformedCommand
	{
	public:
		SCREEN_step07(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "240" /* screen ID */);
		}
	};

	class SCREEN_step08 : public PreformedCommand
	{
	public:
		SCREEN_step08(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "241" /* screen ID */);
		}
	};

	class SCREEN_step09 : public PreformedCommand
	{
	public:
		SCREEN_step09(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "242" /* screen ID */);
		}
	};

	class SCREEN_step10 : public PreformedCommand
	{
	public:
		SCREEN_step10(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "243" /* screen ID */);
		}
	};

	class SCREEN_step11 : public PreformedCommand
	{
	public:
		SCREEN_step11(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "244" /* screen ID */);
		}
	};

	class SCREEN_step12 : public PreformedCommand
	{
	public:
		SCREEN_step12(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "245" /* screen ID */);
		}
	};

	class SCREEN_step13 : public PreformedCommand
	{
	public:
		SCREEN_step13(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "246" /* screen ID */);
		}
	};

	class SCREEN_step14 : public PreformedCommand
	{
	public:
		SCREEN_step14(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "247" /* screen ID */);
		}
	};

	class SCREEN_step15 : public PreformedCommand
	{
	public:
		SCREEN_step15(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "248" /* screen ID */);
		}
	};

	class SCREEN_step16 : public PreformedCommand
	{
	public:
		SCREEN_step16(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "249" /* screen ID */);
		}
	};

	class SCREEN_step17 : public PreformedCommand
	{
	public:
		SCREEN_step17(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "250" /* screen ID */);
		}
	};

	class SCREEN_step18 : public PreformedCommand
	{
	public:
		SCREEN_step18(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "251" /* screen ID */);
		}
	};

	class SCREEN_step19 : public PreformedCommand
	{
	public:
		SCREEN_step19(long DeviceIDFrom, long DeviceIDTo)
		{
			m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, PRIORITY_NORMAL, MESSAGETYPE_COMMAND, COMMAND_Goto_Screen_CONST, 1, 
				COMMANDPARAMETER_EK_Screen_CONST, "252" /* screen ID */);
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
		virtual void SCREEN_mnuMain() { GotoScreen(1); }
		virtual void SCREEN_mnuLights() { GotoScreen(2); }
		virtual void SCREEN_mnuMedia() { GotoScreen(3); }
		virtual void SCREEN_mnuClimate() { GotoScreen(4); }
		virtual void SCREEN_mnuSecurity() { GotoScreen(5); }
		virtual void SCREEN_mnuTelephony() { GotoScreen(6); }
		virtual void SCREEN_mnuMisc() { GotoScreen(7); }
		virtual void SCREEN_mnuUserStatus() { GotoScreen(8); }
		virtual void SCREEN_mnuMakeCallPhonebook() { GotoScreen(9); }
		virtual void SCREEN_mnuMakeCallFavorites() { GotoScreen(10); }
		virtual void SCREEN_mnuMakeCallDialNumber() { GotoScreen(11); }
		virtual void SCREEN_mnuMakeCallPlutoUser() { GotoScreen(12); }
		virtual void SCREEN_mnuSecurityPanel() { GotoScreen(13); }
		virtual void SCREEN_mnuSecurityStatusReport() { GotoScreen(14); }
		virtual void SCREEN_mnuSingleCameraViewOnly() { GotoScreen(15); }
		virtual void SCREEN_mnuIntercom() { GotoScreen(16); }
		virtual void SCREEN_mnuQuadViewCameras() { GotoScreen(17); }
		virtual void SCREEN_mnuVideoCallInProgress() { GotoScreen(18); }
		virtual void SCREEN_mnuFileSave() { GotoScreen(19); }
		virtual void SCREEN_mnuManagePlaylist() { GotoScreen(20); }
		virtual void SCREEN_mnuMakeCallDevice() { GotoScreen(21); }
		virtual void SCREEN_mnuMakeCallPhonebook2() { GotoScreen(22); }
		virtual void SCREEN_mnuMakeCallIntercom() { GotoScreen(23); }
		virtual void SCREEN_mnuCurrentUser() { GotoScreen(24); }
		virtual void SCREEN_mnuCDTrackCopy() { GotoScreen(25); }
		virtual void SCREEN_mnuCDRemote() { GotoScreen(26); }
		virtual void SCREEN_mnuVoicemailMain() { GotoScreen(27); }
		virtual void SCREEN_mnuTransferAllUsers() { GotoScreen(28); }
		virtual void SCREEN_mnuSleeping() { GotoScreen(29); }
		virtual void SCREEN_mnuControllerStatus() { GotoScreen(30); }
		virtual void SCREEN_mnuOtherFolders() { GotoScreen(31); }
		virtual void SCREEN_mnuConfirmDelete() { GotoScreen(32); }
		virtual void SCREEN_mnuWebURL() { GotoScreen(33); }
		virtual void SCREEN_mnuBrowser() { GotoScreen(34); }
		virtual void SCREEN_mnuModeChanged() { GotoScreen(35); }
		virtual void SCREEN_mnuSensorsNotReady() { GotoScreen(36); }
		virtual void SCREEN_mnuRecordedTVShows() { GotoScreen(37); }
		virtual void SCREEN_mnuDeviceControl() { GotoScreen(38); }
		virtual void SCREEN_mnuComputing() { GotoScreen(39); }
		virtual void SCREEN_mnuOpenFile() { GotoScreen(40); }
		virtual void SCREEN_mnuBrowserConsole() { GotoScreen(41); }
		virtual void SCREEN_mnuFavoritesPDA() { GotoScreen(42); }
		virtual void SCREEN_mnuPCFileTools() { GotoScreen(43); }
		virtual void SCREEN_mnuAdvancedOptions() { GotoScreen(44); }
		virtual void SCREEN_mnuMakingCall() { GotoScreen(45); }
		virtual void SCREEN_mnuMakingLinPhoneBroadcast() { GotoScreen(46); }
		virtual void SCREEN_mnuFileList_Music_Movies_Video() { GotoScreen(47); }
		virtual void SCREEN_mnuPVR() { GotoScreen(48); }
		virtual void SCREEN_mnuDVDRemote() { GotoScreen(49); }
		virtual void SCREEN_mnuDVDMenu() { GotoScreen(50); }
		virtual void SCREEN_mnuHouseAtAGlance() { GotoScreen(51); }
		virtual void SCREEN_mnuAdvancedMediaOptions() { GotoScreen(52); }
		virtual void SCREEN_mnuPopupMessage() { GotoScreen(53); }
		virtual void SCREEN_mnuMusicRemote() { GotoScreen(54); }
		virtual void SCREEN_mnuLinPhones() { GotoScreen(55); }
		virtual void SCREEN_mnuOrbiters() { GotoScreen(56); }
		virtual void SCREEN_mnuPhones() { GotoScreen(57); }
		virtual void SCREEN_mnuDVDOptions() { GotoScreen(58); }
		virtual void SCREEN_mnuAnswertoLinPhones() { GotoScreen(59); }
		virtual void SCREEN_mnuUserPinCode() { GotoScreen(60); }
		virtual void SCREEN_mnuPreviewOptions() { GotoScreen(61); }
		virtual void SCREEN_mnuTransferOutside() { GotoScreen(62); }
		virtual void SCREEN_mnuSatelliteCableBox() { GotoScreen(63); }
		virtual void SCREEN_mnuCurrentLocation() { GotoScreen(64); }
		virtual void SCREEN_mnuMediaAttributeSearchInput() { GotoScreen(65); }
		virtual void SCREEN_mnuMediaAttributeSearchResult() { GotoScreen(66); }
		virtual void SCREEN_mnuNewPhoneDetected() { GotoScreen(67); }
		virtual void SCREEN_mnuFirstTime() { GotoScreen(68); }
		virtual void SCREEN_mnuFileList_PlayLists() { GotoScreen(69); }
		virtual void SCREEN_mnuVideosRemote() { GotoScreen(70); }
		virtual void SCREEN_mnuFileList_Pictures_Docs() { GotoScreen(71); }
		virtual void SCREEN_mnuGenericTwoButtonQuestion() { GotoScreen(72); }
		virtual void SCREEN_mnuTryCodes() { GotoScreen(73); }
		virtual void SCREEN_mnuScreenSaver() { GotoScreen(74); }
		virtual void SCREEN_mnuNewPlugAndPlayDevice() { GotoScreen(75); }
		virtual void SCREEN_mnuPower() { GotoScreen(76); }
		virtual void SCREEN_mnuGenericDvdRemote() { GotoScreen(77); }
		virtual void SCREEN_mnuViewPlaylist() { GotoScreen(78); }
		virtual void SCREEN_mnuFileList_withPictures_Mobil() { GotoScreen(79); }
		virtual void SCREEN_mnuGenericDvdMenu() { GotoScreen(80); }
		virtual void SCREEN_mnuViewAttributes() { GotoScreen(81); }
		virtual void SCREEN_mnuPendingTasks() { GotoScreen(82); }
		virtual void SCREEN_mnuAddAttribute() { GotoScreen(83); }
		virtual void SCREEN_mnuRadioRemote() { GotoScreen(84); }
		virtual void SCREEN_mnuGenericCDRemote() { GotoScreen(85); }
		virtual void SCREEN_mnuMainMenuPopUp() { GotoScreen(86); }
		virtual void SCREEN_mnuFileList__Popup_Container() { GotoScreen(87); }
		virtual void SCREEN_mnuCreateViewBookmarks() { GotoScreen(88); }
		virtual void SCREEN_mnuBrowseBookmarks() { GotoScreen(89); }
		virtual void SCREEN_mnuGenericAppController() { GotoScreen(90); }
		virtual void SCREEN_mnuBookmarksByMediaType() { GotoScreen(91); }
		virtual void SCREEN_mnuMainAudi() { GotoScreen(92); }
		virtual void SCREEN_mnuMythTvRemote() { GotoScreen(93); }
		virtual void SCREEN_mnuMyth_all_commands() { GotoScreen(94); }
		virtual void SCREEN_mnuMytv_watching_tv() { GotoScreen(95); }
		virtual void SCREEN_mnuMyth_live_tv_browse_mode() { GotoScreen(96); }
		virtual void SCREEN_mnuMyth_playback_recording() { GotoScreen(97); }
		virtual void SCREEN_mnuGenericKeyboard() { GotoScreen(98); }
		virtual void SCREEN_mnuPingPong() { GotoScreen(99); }
		virtual void SCREEN_mnuTVEPG1() { GotoScreen(100); }
		virtual void SCREEN_mnuTVEPG2() { GotoScreen(101); }
		virtual void SCREEN_mnuTVEPG3() { GotoScreen(102); }
		virtual void SCREEN_mnuShowInfo() { GotoScreen(103); }
		virtual void SCREEN_mnuCreateViewBookmarksTV() { GotoScreen(104); }
		virtual void SCREEN_mnuMainMenuSmall() { GotoScreen(105); }
		virtual void SCREEN_mnuWebSites() { GotoScreen(109); }
		virtual void SCREEN_mnuWhatModelMobileOrbiter() { GotoScreen(110); }
		virtual void SCREEN_mnuLighting() { GotoScreen(111); }
		virtual void SCREEN_mnuTelecom() { GotoScreen(114); }
		virtual void SCREEN_mnuMore() { GotoScreen(117); }
		virtual void SCREEN_mnuFilelist() { GotoScreen(120); }
		virtual void SCREEN_mnuDVDmenu() { GotoScreen(121); }
		virtual void SCREEN_mnuAttributes() { GotoScreen(122); }
		virtual void SCREEN_mnuDVDoptions() { GotoScreen(123); }
		virtual void SCREEN_mnuAdvRemote() { GotoScreen(124); }
		virtual void SCREEN_mnuPlaylist() { GotoScreen(125); }
		virtual void SCREEN_mnuCreateBookmarks() { GotoScreen(126); }
		virtual void SCREEN_mnuAudioOptions() { GotoScreen(129); }
		virtual void SCREEN_mnuSubtitles() { GotoScreen(130); }
		virtual void SCREEN_mnuAngles() { GotoScreen(131); }
		virtual void SCREEN_mnuNewMacAddress(string sMessage1) { GotoScreen(132); }
		virtual void SCREEN_mnuVdrRemote() { GotoScreen(133); }
		virtual void SCREEN_mnuViewBookmarks() { GotoScreen(134); }
		virtual void SCREEN_mnuCdRemote() { GotoScreen(137); }
		virtual void SCREEN_mnuSingleCameraView() { GotoScreen(140); }
		virtual void SCREEN_mnuNewPhoneDeected() { GotoScreen(145); }
		virtual void SCREEN_mnuPowerFile() { GotoScreen(146); }
		virtual void SCREEN_devCallInProgress() { GotoScreen(177); }
		virtual void SCREEN_devIncomingCall() { GotoScreen(178); }
		virtual void SCREEN_devMoveCallsInProgress() { GotoScreen(179); }
		virtual void SCREEN_mapSecurity() { GotoScreen(180); }
		virtual void SCREEN_mapLighting() { GotoScreen(181); }
		virtual void SCREEN_mapClimate() { GotoScreen(182); }
		virtual void SCREEN_mapTelecom() { GotoScreen(183); }
		virtual void SCREEN_navmode() { GotoScreen(184); }
		virtual void SCREEN_pvr_full_screen() { GotoScreen(185); }
		virtual void SCREEN_epg() { GotoScreen(186); }
		virtual void SCREEN_help1() { GotoScreen(187); }
		virtual void SCREEN_help2() { GotoScreen(188); }
		virtual void SCREEN_help3() { GotoScreen(189); }
		virtual void SCREEN_setup_myth() { GotoScreen(190); }
		virtual void SCREEN_setup_myth_2() { GotoScreen(191); }
		virtual void SCREEN_myth_tv_setup() { GotoScreen(192); }
		virtual void SCREEN_myth_tv_setup_2() { GotoScreen(193); }
		virtual void SCREEN_pvr_full_screen_with_options_o() { GotoScreen(194); }
		virtual void SCREEN_dvd_full_screen() { GotoScreen(195); }
		virtual void SCREEN_dvd_full_screen_with_options() { GotoScreen(196); }
		virtual void SCREEN_storedvideos_full_screen() { GotoScreen(197); }
		virtual void SCREEN_storedvideos_full_screen_with() { GotoScreen(198); }
		virtual void SCREEN_generic_app_full_screen() { GotoScreen(199); }
		virtual void SCREEN_dvd_menu_full_screen() { GotoScreen(200); }
		virtual void SCREEN_dvd_menu_full_screen_with_opti() { GotoScreen(201); }
		virtual void SCREEN_vlc_dvd_full_screen() { GotoScreen(202); }
		virtual void SCREEN_vlc_dvd_full_screen_with_optio() { GotoScreen(203); }
		virtual void SCREEN_popLights() { GotoScreen(204); }
		virtual void SCREEN_popMedia() { GotoScreen(205); }
		virtual void SCREEN_popClimate() { GotoScreen(206); }
		virtual void SCREEN_popTelecom() { GotoScreen(207); }
		virtual void SCREEN_popSecurity() { GotoScreen(208); }
		virtual void SCREEN_popOthers() { GotoScreen(209); }
		virtual void SCREEN_Marbella() { GotoScreen(210); }
		virtual void SCREEN_home() { GotoScreen(211); }
		virtual void SCREEN_car_setup() { GotoScreen(212); }
		virtual void SCREEN_media() { GotoScreen(213); }
		virtual void SCREEN_computing() { GotoScreen(214); }
		virtual void SCREEN_radio() { GotoScreen(215); }
		virtual void SCREEN_tv() { GotoScreen(216); }
		virtual void SCREEN_cd() { GotoScreen(217); }
		virtual void SCREEN_hard_drive() { GotoScreen(218); }
		virtual void SCREEN_Internet() { GotoScreen(219); }
		virtual void SCREEN_Email() { GotoScreen(220); }
		virtual void SCREEN_Agenda() { GotoScreen(221); }
		virtual void SCREEN_Other() { GotoScreen(222); }
		virtual void SCREEN_Nav() { GotoScreen(223); }
		virtual void SCREEN_Info() { GotoScreen(224); }
		virtual void SCREEN_Car() { GotoScreen(225); }
		virtual void SCREEN_Setup() { GotoScreen(226); }
		virtual void SCREEN_Lighting() { GotoScreen(227); }
		virtual void SCREEN_Climate() { GotoScreen(228); }
		virtual void SCREEN_Security() { GotoScreen(229); }
		virtual void SCREEN_Telecom() { GotoScreen(230); }
		virtual void SCREEN_full_screen() { GotoScreen(231); }
		virtual void SCREEN_video_hard_drive() { GotoScreen(232); }
		virtual void SCREEN_audio_hard_drive() { GotoScreen(233); }
		virtual void SCREEN_step01() { GotoScreen(234); }
		virtual void SCREEN_step02() { GotoScreen(235); }
		virtual void SCREEN_step03() { GotoScreen(236); }
		virtual void SCREEN_step04() { GotoScreen(237); }
		virtual void SCREEN_step05() { GotoScreen(238); }
		virtual void SCREEN_step06() { GotoScreen(239); }
		virtual void SCREEN_step07() { GotoScreen(240); }
		virtual void SCREEN_step08() { GotoScreen(241); }
		virtual void SCREEN_step09() { GotoScreen(242); }
		virtual void SCREEN_step10() { GotoScreen(243); }
		virtual void SCREEN_step11() { GotoScreen(244); }
		virtual void SCREEN_step12() { GotoScreen(245); }
		virtual void SCREEN_step13() { GotoScreen(246); }
		virtual void SCREEN_step14() { GotoScreen(247); }
		virtual void SCREEN_step15() { GotoScreen(248); }
		virtual void SCREEN_step16() { GotoScreen(249); }
		virtual void SCREEN_step17() { GotoScreen(250); }
		virtual void SCREEN_step18() { GotoScreen(251); }
		virtual void SCREEN_step19() { GotoScreen(252); }

		virtual void ReceivedGotoScreenMessage(int nPK_Screen, Message *pMessage)
		{
			switch(nPK_Screen) 
			{
				case 1:
				{
					SCREEN_mnuMain();
					break;
				}
				case 2:
				{
					SCREEN_mnuLights();
					break;
				}
				case 3:
				{
					SCREEN_mnuMedia();
					break;
				}
				case 4:
				{
					SCREEN_mnuClimate();
					break;
				}
				case 5:
				{
					SCREEN_mnuSecurity();
					break;
				}
				case 6:
				{
					SCREEN_mnuTelephony();
					break;
				}
				case 7:
				{
					SCREEN_mnuMisc();
					break;
				}
				case 8:
				{
					SCREEN_mnuUserStatus();
					break;
				}
				case 9:
				{
					SCREEN_mnuMakeCallPhonebook();
					break;
				}
				case 10:
				{
					SCREEN_mnuMakeCallFavorites();
					break;
				}
				case 11:
				{
					SCREEN_mnuMakeCallDialNumber();
					break;
				}
				case 12:
				{
					SCREEN_mnuMakeCallPlutoUser();
					break;
				}
				case 13:
				{
					SCREEN_mnuSecurityPanel();
					break;
				}
				case 14:
				{
					SCREEN_mnuSecurityStatusReport();
					break;
				}
				case 15:
				{
					SCREEN_mnuSingleCameraViewOnly();
					break;
				}
				case 16:
				{
					SCREEN_mnuIntercom();
					break;
				}
				case 17:
				{
					SCREEN_mnuQuadViewCameras();
					break;
				}
				case 18:
				{
					SCREEN_mnuVideoCallInProgress();
					break;
				}
				case 19:
				{
					SCREEN_mnuFileSave();
					break;
				}
				case 20:
				{
					SCREEN_mnuManagePlaylist();
					break;
				}
				case 21:
				{
					SCREEN_mnuMakeCallDevice();
					break;
				}
				case 22:
				{
					SCREEN_mnuMakeCallPhonebook2();
					break;
				}
				case 23:
				{
					SCREEN_mnuMakeCallIntercom();
					break;
				}
				case 24:
				{
					SCREEN_mnuCurrentUser();
					break;
				}
				case 25:
				{
					SCREEN_mnuCDTrackCopy();
					break;
				}
				case 26:
				{
					SCREEN_mnuCDRemote();
					break;
				}
				case 27:
				{
					SCREEN_mnuVoicemailMain();
					break;
				}
				case 28:
				{
					SCREEN_mnuTransferAllUsers();
					break;
				}
				case 29:
				{
					SCREEN_mnuSleeping();
					break;
				}
				case 30:
				{
					SCREEN_mnuControllerStatus();
					break;
				}
				case 31:
				{
					SCREEN_mnuOtherFolders();
					break;
				}
				case 32:
				{
					SCREEN_mnuConfirmDelete();
					break;
				}
				case 33:
				{
					SCREEN_mnuWebURL();
					break;
				}
				case 34:
				{
					SCREEN_mnuBrowser();
					break;
				}
				case 35:
				{
					SCREEN_mnuModeChanged();
					break;
				}
				case 36:
				{
					SCREEN_mnuSensorsNotReady();
					break;
				}
				case 37:
				{
					SCREEN_mnuRecordedTVShows();
					break;
				}
				case 38:
				{
					SCREEN_mnuDeviceControl();
					break;
				}
				case 39:
				{
					SCREEN_mnuComputing();
					break;
				}
				case 40:
				{
					SCREEN_mnuOpenFile();
					break;
				}
				case 41:
				{
					SCREEN_mnuBrowserConsole();
					break;
				}
				case 42:
				{
					SCREEN_mnuFavoritesPDA();
					break;
				}
				case 43:
				{
					SCREEN_mnuPCFileTools();
					break;
				}
				case 44:
				{
					SCREEN_mnuAdvancedOptions();
					break;
				}
				case 45:
				{
					SCREEN_mnuMakingCall();
					break;
				}
				case 46:
				{
					SCREEN_mnuMakingLinPhoneBroadcast();
					break;
				}
				case 47:
				{
					SCREEN_mnuFileList_Music_Movies_Video();
					break;
				}
				case 48:
				{
					SCREEN_mnuPVR();
					break;
				}
				case 49:
				{
					SCREEN_mnuDVDRemote();
					break;
				}
				case 50:
				{
					SCREEN_mnuDVDMenu();
					break;
				}
				case 51:
				{
					SCREEN_mnuHouseAtAGlance();
					break;
				}
				case 52:
				{
					SCREEN_mnuAdvancedMediaOptions();
					break;
				}
				case 53:
				{
					SCREEN_mnuPopupMessage();
					break;
				}
				case 54:
				{
					SCREEN_mnuMusicRemote();
					break;
				}
				case 55:
				{
					SCREEN_mnuLinPhones();
					break;
				}
				case 56:
				{
					SCREEN_mnuOrbiters();
					break;
				}
				case 57:
				{
					SCREEN_mnuPhones();
					break;
				}
				case 58:
				{
					SCREEN_mnuDVDOptions();
					break;
				}
				case 59:
				{
					SCREEN_mnuAnswertoLinPhones();
					break;
				}
				case 60:
				{
					SCREEN_mnuUserPinCode();
					break;
				}
				case 61:
				{
					SCREEN_mnuPreviewOptions();
					break;
				}
				case 62:
				{
					SCREEN_mnuTransferOutside();
					break;
				}
				case 63:
				{
					SCREEN_mnuSatelliteCableBox();
					break;
				}
				case 64:
				{
					SCREEN_mnuCurrentLocation();
					break;
				}
				case 65:
				{
					SCREEN_mnuMediaAttributeSearchInput();
					break;
				}
				case 66:
				{
					SCREEN_mnuMediaAttributeSearchResult();
					break;
				}
				case 67:
				{
					SCREEN_mnuNewPhoneDetected();
					break;
				}
				case 68:
				{
					SCREEN_mnuFirstTime();
					break;
				}
				case 69:
				{
					SCREEN_mnuFileList_PlayLists();
					break;
				}
				case 70:
				{
					SCREEN_mnuVideosRemote();
					break;
				}
				case 71:
				{
					SCREEN_mnuFileList_Pictures_Docs();
					break;
				}
				case 72:
				{
					SCREEN_mnuGenericTwoButtonQuestion();
					break;
				}
				case 73:
				{
					SCREEN_mnuTryCodes();
					break;
				}
				case 74:
				{
					SCREEN_mnuScreenSaver();
					break;
				}
				case 75:
				{
					SCREEN_mnuNewPlugAndPlayDevice();
					break;
				}
				case 76:
				{
					SCREEN_mnuPower();
					break;
				}
				case 77:
				{
					SCREEN_mnuGenericDvdRemote();
					break;
				}
				case 78:
				{
					SCREEN_mnuViewPlaylist();
					break;
				}
				case 79:
				{
					SCREEN_mnuFileList_withPictures_Mobil();
					break;
				}
				case 80:
				{
					SCREEN_mnuGenericDvdMenu();
					break;
				}
				case 81:
				{
					SCREEN_mnuViewAttributes();
					break;
				}
				case 82:
				{
					SCREEN_mnuPendingTasks();
					break;
				}
				case 83:
				{
					SCREEN_mnuAddAttribute();
					break;
				}
				case 84:
				{
					SCREEN_mnuRadioRemote();
					break;
				}
				case 85:
				{
					SCREEN_mnuGenericCDRemote();
					break;
				}
				case 86:
				{
					SCREEN_mnuMainMenuPopUp();
					break;
				}
				case 87:
				{
					SCREEN_mnuFileList__Popup_Container();
					break;
				}
				case 88:
				{
					SCREEN_mnuCreateViewBookmarks();
					break;
				}
				case 89:
				{
					SCREEN_mnuBrowseBookmarks();
					break;
				}
				case 90:
				{
					SCREEN_mnuGenericAppController();
					break;
				}
				case 91:
				{
					SCREEN_mnuBookmarksByMediaType();
					break;
				}
				case 92:
				{
					SCREEN_mnuMainAudi();
					break;
				}
				case 93:
				{
					SCREEN_mnuMythTvRemote();
					break;
				}
				case 94:
				{
					SCREEN_mnuMyth_all_commands();
					break;
				}
				case 95:
				{
					SCREEN_mnuMytv_watching_tv();
					break;
				}
				case 96:
				{
					SCREEN_mnuMyth_live_tv_browse_mode();
					break;
				}
				case 97:
				{
					SCREEN_mnuMyth_playback_recording();
					break;
				}
				case 98:
				{
					SCREEN_mnuGenericKeyboard();
					break;
				}
				case 99:
				{
					SCREEN_mnuPingPong();
					break;
				}
				case 100:
				{
					SCREEN_mnuTVEPG1();
					break;
				}
				case 101:
				{
					SCREEN_mnuTVEPG2();
					break;
				}
				case 102:
				{
					SCREEN_mnuTVEPG3();
					break;
				}
				case 103:
				{
					SCREEN_mnuShowInfo();
					break;
				}
				case 104:
				{
					SCREEN_mnuCreateViewBookmarksTV();
					break;
				}
				case 105:
				{
					SCREEN_mnuMainMenuSmall();
					break;
				}
				case 109:
				{
					SCREEN_mnuWebSites();
					break;
				}
				case 110:
				{
					SCREEN_mnuWhatModelMobileOrbiter();
					break;
				}
				case 111:
				{
					SCREEN_mnuLighting();
					break;
				}
				case 114:
				{
					SCREEN_mnuTelecom();
					break;
				}
				case 117:
				{
					SCREEN_mnuMore();
					break;
				}
				case 120:
				{
					SCREEN_mnuFilelist();
					break;
				}
				case 121:
				{
					SCREEN_mnuDVDmenu();
					break;
				}
				case 122:
				{
					SCREEN_mnuAttributes();
					break;
				}
				case 123:
				{
					SCREEN_mnuDVDoptions();
					break;
				}
				case 124:
				{
					SCREEN_mnuAdvRemote();
					break;
				}
				case 125:
				{
					SCREEN_mnuPlaylist();
					break;
				}
				case 126:
				{
					SCREEN_mnuCreateBookmarks();
					break;
				}
				case 129:
				{
					SCREEN_mnuAudioOptions();
					break;
				}
				case 130:
				{
					SCREEN_mnuSubtitles();
					break;
				}
				case 131:
				{
					SCREEN_mnuAngles();
					break;
				}
				case 132:
				{
					string sMessage1 = pMessage->m_mapParameters[58];
					SCREEN_mnuNewMacAddress(sMessage1);
					break;
				}
				case 133:
				{
					SCREEN_mnuVdrRemote();
					break;
				}
				case 134:
				{
					SCREEN_mnuViewBookmarks();
					break;
				}
				case 137:
				{
					SCREEN_mnuCdRemote();
					break;
				}
				case 140:
				{
					SCREEN_mnuSingleCameraView();
					break;
				}
				case 145:
				{
					SCREEN_mnuNewPhoneDeected();
					break;
				}
				case 146:
				{
					SCREEN_mnuPowerFile();
					break;
				}
				case 177:
				{
					SCREEN_devCallInProgress();
					break;
				}
				case 178:
				{
					SCREEN_devIncomingCall();
					break;
				}
				case 179:
				{
					SCREEN_devMoveCallsInProgress();
					break;
				}
				case 180:
				{
					SCREEN_mapSecurity();
					break;
				}
				case 181:
				{
					SCREEN_mapLighting();
					break;
				}
				case 182:
				{
					SCREEN_mapClimate();
					break;
				}
				case 183:
				{
					SCREEN_mapTelecom();
					break;
				}
				case 184:
				{
					SCREEN_navmode();
					break;
				}
				case 185:
				{
					SCREEN_pvr_full_screen();
					break;
				}
				case 186:
				{
					SCREEN_epg();
					break;
				}
				case 187:
				{
					SCREEN_help1();
					break;
				}
				case 188:
				{
					SCREEN_help2();
					break;
				}
				case 189:
				{
					SCREEN_help3();
					break;
				}
				case 190:
				{
					SCREEN_setup_myth();
					break;
				}
				case 191:
				{
					SCREEN_setup_myth_2();
					break;
				}
				case 192:
				{
					SCREEN_myth_tv_setup();
					break;
				}
				case 193:
				{
					SCREEN_myth_tv_setup_2();
					break;
				}
				case 194:
				{
					SCREEN_pvr_full_screen_with_options_o();
					break;
				}
				case 195:
				{
					SCREEN_dvd_full_screen();
					break;
				}
				case 196:
				{
					SCREEN_dvd_full_screen_with_options();
					break;
				}
				case 197:
				{
					SCREEN_storedvideos_full_screen();
					break;
				}
				case 198:
				{
					SCREEN_storedvideos_full_screen_with();
					break;
				}
				case 199:
				{
					SCREEN_generic_app_full_screen();
					break;
				}
				case 200:
				{
					SCREEN_dvd_menu_full_screen();
					break;
				}
				case 201:
				{
					SCREEN_dvd_menu_full_screen_with_opti();
					break;
				}
				case 202:
				{
					SCREEN_vlc_dvd_full_screen();
					break;
				}
				case 203:
				{
					SCREEN_vlc_dvd_full_screen_with_optio();
					break;
				}
				case 204:
				{
					SCREEN_popLights();
					break;
				}
				case 205:
				{
					SCREEN_popMedia();
					break;
				}
				case 206:
				{
					SCREEN_popClimate();
					break;
				}
				case 207:
				{
					SCREEN_popTelecom();
					break;
				}
				case 208:
				{
					SCREEN_popSecurity();
					break;
				}
				case 209:
				{
					SCREEN_popOthers();
					break;
				}
				case 210:
				{
					SCREEN_Marbella();
					break;
				}
				case 211:
				{
					SCREEN_home();
					break;
				}
				case 212:
				{
					SCREEN_car_setup();
					break;
				}
				case 213:
				{
					SCREEN_media();
					break;
				}
				case 214:
				{
					SCREEN_computing();
					break;
				}
				case 215:
				{
					SCREEN_radio();
					break;
				}
				case 216:
				{
					SCREEN_tv();
					break;
				}
				case 217:
				{
					SCREEN_cd();
					break;
				}
				case 218:
				{
					SCREEN_hard_drive();
					break;
				}
				case 219:
				{
					SCREEN_Internet();
					break;
				}
				case 220:
				{
					SCREEN_Email();
					break;
				}
				case 221:
				{
					SCREEN_Agenda();
					break;
				}
				case 222:
				{
					SCREEN_Other();
					break;
				}
				case 223:
				{
					SCREEN_Nav();
					break;
				}
				case 224:
				{
					SCREEN_Info();
					break;
				}
				case 225:
				{
					SCREEN_Car();
					break;
				}
				case 226:
				{
					SCREEN_Setup();
					break;
				}
				case 227:
				{
					SCREEN_Lighting();
					break;
				}
				case 228:
				{
					SCREEN_Climate();
					break;
				}
				case 229:
				{
					SCREEN_Security();
					break;
				}
				case 230:
				{
					SCREEN_Telecom();
					break;
				}
				case 231:
				{
					SCREEN_full_screen();
					break;
				}
				case 232:
				{
					SCREEN_video_hard_drive();
					break;
				}
				case 233:
				{
					SCREEN_audio_hard_drive();
					break;
				}
				case 234:
				{
					SCREEN_step01();
					break;
				}
				case 235:
				{
					SCREEN_step02();
					break;
				}
				case 236:
				{
					SCREEN_step03();
					break;
				}
				case 237:
				{
					SCREEN_step04();
					break;
				}
				case 238:
				{
					SCREEN_step05();
					break;
				}
				case 239:
				{
					SCREEN_step06();
					break;
				}
				case 240:
				{
					SCREEN_step07();
					break;
				}
				case 241:
				{
					SCREEN_step08();
					break;
				}
				case 242:
				{
					SCREEN_step09();
					break;
				}
				case 243:
				{
					SCREEN_step10();
					break;
				}
				case 244:
				{
					SCREEN_step11();
					break;
				}
				case 245:
				{
					SCREEN_step12();
					break;
				}
				case 246:
				{
					SCREEN_step13();
					break;
				}
				case 247:
				{
					SCREEN_step14();
					break;
				}
				case 248:
				{
					SCREEN_step15();
					break;
				}
				case 249:
				{
					SCREEN_step16();
					break;
				}
				case 250:
				{
					SCREEN_step17();
					break;
				}
				case 251:
				{
					SCREEN_step18();
					break;
				}
				case 252:
				{
					SCREEN_step19();
					break;
				}

			}
		}
	};
}

#endif
