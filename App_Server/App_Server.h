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
//<-dceag-const-e->
	// Private member variables
	typedef map<string, vector<int> > vector_map;
	vector_map AppPIDs; // PIDs associated with an AppName - used for application mass killing (not massacres :P)

	// Private methods
	bool StartApp(string CmdExecutable, string CmdParams, string AppName);
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

/* 
	COMMAND: #28 - Simulate Keypress
	COMMENTS: Send a keypress event to an application
	PARAMETERS:
		#26 PK_Button
			What key to simulate being pressed
		#50 Name
			The application to send the keypress to. If not specified, it goes to the DCE device.
*/
	virtual void CMD_Simulate_Keypress(int iPK_Button,string sName) { string sCMD_Result; CMD_Simulate_Keypress(iPK_Button,sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Simulate_Keypress(int iPK_Button,string sName,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #67 - Spawn Application
	COMMENTS: Spawn an application
	PARAMETERS:
		#13 Filename
			The name of the executable file to spawn
		#50 Name
			A name that we'll remember the application by for future kill commands
		#51 Arguments
			Command arguments
*/
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments) { string sCMD_Result; CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #69 - Kill Application
	COMMENTS: Kill an application
	PARAMETERS:
		#50 Name
			Application name given at spawn time
*/
	virtual void CMD_Kill_Application(string sName) { string sCMD_Result; CMD_Kill_Application(sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Kill_Application(string sName,string &sCMD_Result,Message *pMessage);

/* 
	COMMAND: #70 - Hide Application
	COMMENTS: Hide an application
	PARAMETERS:
		#50 Name
			Application name given at spawn time
*/
	virtual void CMD_Hide_Application(string sName) { string sCMD_Result; CMD_Hide_Application(sName.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Hide_Application(string sName,string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
