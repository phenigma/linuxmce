/* 
	App_Server
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

//<-dceag-d-b->
#include "App_Server.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

//<-dceag-const-b->
App_Server::App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: App_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
}

//<-dceag-dest-b->
App_Server::~App_Server()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool App_Server::Register()
//<-dceag-reg-e->
{
	return Connect(); 
}


/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a keypress event to an application */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void App_Server::CMD_Simulate_Keypress(int iPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
	cout << "Need to implement command #28 - Simulate Keypress" << endl;
	cout << "Parm #26 - PK_Button=" << iPK_Button << endl;
	cout << "Parm #50 - Name=" << sName << endl;
}

//<-dceag-c67-b->

	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawn an application */
		/** @param #13 Filename */
			/** The name of the executable file to spawn */
		/** @param #50 Name */
			/** A name that we'll remember the application by for future kill commands */
		/** @param #51 Arguments */
			/** Command arguments */

void App_Server::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
	cout << "command #67 - Spawn Application" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #50 - Name=" << sName << endl;
	cout << "Parm #51 - Arguments=" << sArguments << endl;

	if (! StartApp(sFilename, sArguments, sName))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Can't spawn '%s': %s %s.", sName.c_str(), sFilename.c_str(), sArguments.c_str());
		sCMD_Result = "Failed";
	}
	else
	{
		sCMD_Result = "OK";
	}
}

//<-dceag-c69-b->

	/** @brief COMMAND: #69 - Kill Application */
	/** Kill an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

void App_Server::CMD_Kill_Application(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c69-e->
{
	cout << "command #69 - Kill Application" << endl;
	cout << "Parm #50 - Name=" << sName << endl;

#ifndef WIN32
	vector_map::iterator element = AppPIDs.find(sName);
	if (element == AppPIDs.end())
	{
		g_pPlutoLogger->Write(LV_STATUS, "No application '%s' found. None killed", sName.c_str());
	}
	else
	{
		const vector<int> & our_vect = element->second;
		g_pPlutoLogger->Write(LV_STATUS, "Found %d '%s' applications. Killing them all (really)", our_vect.size(), sName.c_str());
		vector<int>::const_iterator i;
		for (i = our_vect.begin(); i < our_vect.end(); i++)
			kill(* i, SIGKILL); // we don't SIGTERM first; we simply kill them all; where your shutgun at? :)
		AppPIDs.erase(element);
	}
#endif
}

//<-dceag-c70-b->

	/** @brief COMMAND: #70 - Hide Application */
	/** Hide an application */
		/** @param #50 Name */
			/** Application name given at spawn time */

void App_Server::CMD_Hide_Application(string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c70-e->
{
	cout << "Need to implement command #70 - Hide Application" << endl;
	cout << "Parm #50 - Name=" << sName << endl;
}

/* Private */
bool App_Server::StartApp(string CmdExecutable, string CmdParams, string AppName)
{
	if (CmdExecutable == "" && CmdParams == "")
	{
		g_pPlutoLogger->Write(LV_WARNING, "StartApp: Received empty Executable and Parameters for '%s'", AppName.c_str());
		return false;
	}
	
#ifndef WIN32
	cout << "StartApp: " << CmdExecutable << ", " << CmdParams << ", "
		<< AppName << "." << endl;
	
	//parse the args
	const int MaxArgs = 32;

	char * ptr;
	char * params;
	params = ptr = (char *) CmdParams.c_str();
	char * args[MaxArgs];
	int i = 0;

	args[0] = (char *) CmdExecutable.c_str();
	args[1] = params;
	while (++i < MaxArgs - 1)
	{
		ptr = strchr(ptr, ' ');
		if (ptr == NULL)
			break;
		* ptr = 0;
		cout << "Arg " << i << ": " << params << "." << endl;
		args[i] = params;
		params = ++ptr;
	}
	if (i == 1) i++;
	args[i] = NULL;
	cout << "Arg count: " << i << endl;

	for (int x = 0 ; x < i; x++)
		cout << "Arg " << x << ": " << args[x] << "." << endl;
	
	pid_t pid = fork();
	switch (pid)
	{
		case 0: //child
		{
			setenv("DISPLAY", ":0", 1);
			//now, exec the process
			execvp(args[0], args);
			exit(99);
		}

		case -1:
			g_pPlutoLogger->Write(LV_CRITICAL, "Error starting %s, err: %s", CmdExecutable.c_str(), strerror(errno));
			return false;
			break;

		default:
			vector_map::iterator element = AppPIDs.find(AppName);
			if (element == AppPIDs.end())
			{
				vector<int> temp_vect;
				temp_vect.insert(temp_vect.end(), pid);
				AppPIDs[AppName] = temp_vect;
			}
			else
			{
				element->second.insert(element->second.end(), pid);
			}
			return true;
	}
#else
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory (&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory (&pi, sizeof(pi));
	CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
#endif
}

//<-dceag-sample-b->! no sample

