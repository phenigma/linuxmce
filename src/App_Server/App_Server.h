//<-dceag-d-b->
#ifndef App_Server_h
#define App_Server_h

//	DCE Implemenation for #26 App Server

#include "Gen_Devices/App_ServerBase.h"
//<-dceag-d-e->

//<-dceag-decl-b->
namespace DCE
{
	class App_Server : public App_Server_Command
	{
//<-dceag-decl-e->
//<-dceag-const-b->
public:
		// Constructors/Destructor
		App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~App_Server();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

        // Private member variables
        typedef map<string, map<int, pair<string, string> > > vector_map;
        vector_map m_mapAppPids; // PIDs associated with an AppName - used for application mass killing (not massacres :P)

        // Private methods
        bool StartApp(string CmdExecutable, string CmdParams, string AppName, string sCommandsOnSuccess, string sCommandsOnFailure);
        void ProcessExited(int pid, int status);

        void SendMessageList(string messageList);
public:
    // Public member variables


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a keypress event to an application */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawn an application */
		/** @param #13 Filename */
			/** The name of the executable file to spawn */
		/** @param #50 Name */
			/** A name that we'll remember the application by for future kill commands */
		/** @param #51 Arguments */
			/** Command arguments */
		/** @param #94 SendOnFailure */
			/** Send this messages if the process exited with failure error code. */
		/** @param #95 SendOnSuccess */
			/** Send this messages if the process exited with success error code. */

	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess) { string sCMD_Result; CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sSendOnFailure.c_str(),sSendOnSuccess.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #69 - Kill Application */
	/** Kill an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

	virtual void CMD_Kill_Application(string sName) { string sCMD_Result; CMD_Kill_Application(sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Kill_Application(string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #70 - Hide Application */
	/** Hide an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

	virtual void CMD_Hide_Application(string sName) { string sCMD_Result; CMD_Hide_Application(sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Hide_Application(string sName,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #268 - Reboot */
	/** Reboots the computers this is running on. */
		/** @param #2 PK_Device */
			/** The computer to reboot.  This is ignored when AppServer receives the command--it reboots itself.  When General Info Plugin receives it, it reboots the mentioned computer. */

	virtual void CMD_Reboot(int iPK_Device) { string sCMD_Result; CMD_Reboot(iPK_Device,sCMD_Result,NULL);};
	virtual void CMD_Reboot(int iPK_Device,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
