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

#include <signal.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#endif

#ifndef WIN32 // we only have signals on Linux and hte global var is only used there. so we ifndef it.. 
App_Server *g_pAppServer = NULL;

void sh(int i) /* signal handler */
{
    int status = 0;
    pid_t pid = 0;


    pid = wait(&status);

    if ( g_pAppServer )
        g_pAppServer->ProcessExited(pid, status);
}
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
App_Server::App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: App_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
#ifndef WIN32
	g_pAppServer = this;
    signal(SIGCHLD, sh); /* install handler */
#endif
}

//<-dceag-const2-b->!

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
    return Connect(PK_DeviceTemplate_get());
}

/*
    When you receive commands that are destined to one of your children,
    then if that child implements DCE then there will already be a separate class
    created for the child that will get the message.  If the child does not, then you will
    get all commands for your children in ReceivedCommandForChild, where
    pDeviceData_Base is the child device.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void App_Server::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
    sCMD_Result = "UNHANDLED CHILD";
}

/*
    When you received a valid command, but it wasn't for one of your children,
    then ReceivedUnknownCommand gets called.  If you handle the message, you
    should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void App_Server::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
    sCMD_Result = "UNKNOWN DEVICE";
}

/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c28-b->

	/** @brief COMMAND: #28 - Simulate Keypress */
	/** Send a keypress event to an application */
		/** @param #26 PK_Button */
			/** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
		/** @param #50 Name */
			/** The application to send the keypress to. If not specified, it goes to the DCE device. */

void App_Server::CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c28-e->
{
    cout << "Need to implement command #28 - Simulate Keypress" << endl;
    cout << "Parm #26 - PK_Button=" << sPK_Button << endl;
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
		/** @param #94 SendOnFailure */
			/** Send this messages if the process exited with failure error code. */
		/** @param #95 SendOnSuccess */
			/** Send this messages if the process exited with success error code. */

void App_Server::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
    if (! StartApp(sFilename, sArguments, sName, sSendOnSuccess, sSendOnFailure) )
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
/*    vector_map::iterator element = m_mapAppPids.find(sName);
    if (element == m_mapAppPids.end())
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

        m_mapAppPids.erase(element);
    }*/
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
bool App_Server::StartApp(string CmdExecutable, string CmdParams, string AppName, string sCommandsOnSuccess, string sCommandsOnFailure)
{
    if ( AppName == "" )
        AppName = "not named";

    if (CmdExecutable == "" && CmdParams == "")
    {
        g_pPlutoLogger->Write(LV_WARNING, "StartApp: Received empty Executable and Parameters for '%s'", AppName.c_str());
        return false;
    }

#ifndef WIN32
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
        args[i] = params;
        params = ++ptr;
    }
    if (i == 1) i++;
    args[i] = NULL;
    g_pPlutoLogger->Write(LV_STATUS, "Found %d arguments", i);

    for (int x = 0 ; x < i; x++)
        g_pPlutoLogger->Write(LV_STATUS, "Argument %d: %s", x, args[x]);

    pid_t pid = fork();
    switch (pid)
    {
        case 0: //child
        {
            g_pPlutoLogger->Write(LV_STATUS, "Waiting two seconds (in the forked process).");
            sleep(2); // sleep so that the signal doesn't get in until later.

            setenv("DISPLAY", ":0", 1);
            //now, exec the process
            g_pPlutoLogger->Write(LV_STATUS, "Spawning");

            if ( execvp(args[0], args) == -1)
                exit(99);
        }

        case -1:
            g_pPlutoLogger->Write(LV_CRITICAL, "Error starting %s, err: %s", CmdExecutable.c_str(), strerror(errno));
            return false;
            break;

        default:
            vector_map::iterator element = m_mapAppPids.find(AppName);

            if (element == m_mapAppPids.end())
                m_mapAppPids[AppName] = map<int, pair<string, string> >();

            m_mapAppPids[AppName][pid] = make_pair(sCommandsOnSuccess, sCommandsOnFailure);
            return true;
    }
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
    CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	return true;
#endif
}

void App_Server::ProcessExited(int pid, int status)
{
    vector_map::iterator itAppName;

    itAppName = m_mapAppPids.begin();

    while ( itAppName != m_mapAppPids.end() )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Iterating: %s", (*itAppName).first.c_str() );

        map<int, pair<string, string> >::iterator returnCommand;

        returnCommand = (*itAppName).second.find(pid);

        if ( returnCommand != (*itAppName).second.end() ) // found the thing
        {
            SendMessageList( status ? (*returnCommand).second.first : (*returnCommand).second.second);
            (*itAppName).second.erase(returnCommand);
            return;
        }
        else
        {
            g_pPlutoLogger->Write(LV_STATUS, "Pid not found in the data structure");
        }
        itAppName++;
    }
}

void App_Server::SendMessageList(string messageList)
{
    g_pPlutoLogger->Write(LV_STATUS, "Sending this command chain: -->%s<--", messageList.c_str() );

    vector<string> commands;
    StringUtils::Tokenize(messageList, ",", commands);

    g_pPlutoLogger->Write(LV_STATUS, "Command count: %d", commands.size());

    if ( commands.size() == 0 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "Empty command array resulted from the string: %s", messageList.c_str());
        return;
    }

    Message *pMessage = new Message(commands[0]);    // empty message (to only caryy the rest with him).
    for ( unsigned int i = 1; i < commands.size(); i++ )
        pMessage->m_vectExtraMessages.push_back(new Message(commands[i]));

    PreformedCommand command;
    command.m_pMessage = pMessage;
    SendCommand(command);
    g_pPlutoLogger->Write(LV_STATUS, "After sending");
}

//<-dceag-sample-b->!
//<-dceag-c268-b->

	/** @brief COMMAND: #268 - Reboot */
	/** Reboots the computers this is running on. */

void App_Server::CMD_Reboot(string &sCMD_Result,Message *pMessage)
//<-dceag-c268-e->
{
}
