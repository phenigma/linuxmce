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

App_Server *g_pAppServer = NULL;

void sh(int i) /* signal handler */
{
    int status;
    pid_t pid;

#ifndef WIN32
    pid = wait(&status);
#endif
    if ( g_pAppServer )
        g_pAppServer->ProcessExited(pid, status);
}

//<-dceag-const-b->
App_Server::App_Server(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: App_Server_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
    g_pAppServer = this;
#ifndef WIN32
    signal(SIGCHLD, sh); /* install handler */
#endif
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
    for ( int i = 1; i < commands.size(); i++ )
        pMessage->m_vectExtraMessages.push_back(new Message(commands[i]));

    PreformedCommand command;
    command.m_pMessage = pMessage;
    SendCommand(command);
    g_pPlutoLogger->Write(LV_STATUS, "After sending");
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void App_Server::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DCE:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Examples:
	
	// Send a specific the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.
	DCE::CMD_Simulate_Mouse_Click CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);
	SendCommand(CMD_Simulate_Mouse_Click);

	// Send the message to orbiters 32898 and 27283 (ie a device list, hence the _DL)
	// And we want a response, which will be "OK" if the command was successfull
	string sResponse;
	DCE::CMD_Simulate_Mouse_Click_DL CMD_Simulate_Mouse_Click_DL(m_dwPK_Device,"32898,27283",55,77)
	SendCommand(CMD_Simulate_Mouse_Click_DL,&sResponse);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	// Note the _Cat for category
	DCE::CMD_Simulate_Mouse_Click_Cat CMD_Simulate_Mouse_Click_Cat(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77)
    SendCommand(CMD_Simulate_Mouse_Click_Cat);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	// Note the _DT.
	DCE::CMD_Simulate_Mouse_Click_DT CMD_Simulate_Mouse_Click_DT(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);
	SendCommand(CMD_Simulate_Mouse_Click_DT);

	// This command has a normal string parameter, but also an int as an out parameter
	int iValue;
	DCE::CMD_Get_Signal_Strength CMD_Get_Signal_Strength(m_dwDeviceID, DestDevice, sMac_address,&iValue);
	// This send command will wait for the destination device to respond since there is
	// an out parameter
	SendCommand(CMD_Get_Signal_Strength);  

	// This time we don't care about the out parameter.  We just want the command to 
	// get through, and don't want to wait for the round trip.  The out parameter, iValue,
	// will not get set
	SendCommandNoResponse(CMD_Get_Signal_Strength);  

	// This command has an out parameter of a data block.  Any parameter that is a binary
	// data block is a pair of int and char *
	// We'll also want to see the response, so we'll pass a string for that too

	int iFileSize;
	char *pFileContents
	string sResponse;
	DCE::CMD_Request_File CMD_Request_File(m_dwDeviceID, DestDevice, "filename",&pFileContents,&iFileSize,&sResponse);
	SendCommand(CMD_Request_File);

	// If the device processed the command (in this case retrieved the file),
	// sResponse will be "OK", and iFileSize will be the size of the file
	// and pFileContents will be the file contents.  **NOTE**  We are responsible
	// free deleting pFileContents.


	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();
	// Fire the "Touch or click" which takes an X and Y parameter
	EVENT_Touch_or_click(10,150);
}
*/
//<-dceag-sample-e->
