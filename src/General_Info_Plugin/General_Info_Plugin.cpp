/*
 General_Info_Plugin
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

//<-dceag-d-b->
#include "General_Info_Plugin.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "../PlutoUtils/md5.h"

#include "DCERouter.h"
#include "DCE/DeviceData_Router.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_UnknownDevices.h"
#include "pluto_main/Table_Room.h"

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
General_Info_Plugin::General_Info_Plugin(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: General_Info_Plugin_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
{
	m_pDatabase_pluto_main = new Database_pluto_main();
	if(!m_pDatabase_pluto_main->Connect(m_pRouter->sDBHost_get(),m_pRouter->sDBUser_get(),m_pRouter->sDBPassword_get(),m_pRouter->sDBName_get(),m_pRouter->iDBPort_get()) )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
		m_bQuit=true;
		return;
	}
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
General_Info_Plugin::~General_Info_Plugin()
//<-dceag-dest-e->
{
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool General_Info_Plugin::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void General_Info_Plugin::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
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
void General_Info_Plugin::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c68-b->

	/** @brief COMMAND: #68 - Get Device Data */
	/** Return the device data for a device. */
		/** @param #2 PK_Device */
			/** The device which you want data from */
		/** @param #5 Value To Assign */
			/** The value. */
		/** @param #52 PK_DeviceData */
			/** What parameter to get. */
		/** @param #53 UseDefault */
			/** Report the default value, rather than requesting a live value. */

void General_Info_Plugin::CMD_Get_Device_Data(int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign,string &sCMD_Result,Message *pMessage)
//<-dceag-c68-e->
{
	if( bUseDefault )
	{
		DeviceData_Router *pDeviceData_Router = m_pRouter->m_mapDeviceData_Router_Find(iPK_Device);
		if( !pDeviceData_Router )
			*sValue_To_Assign="BAD DEVICE";
		else
		{
			if( pDeviceData_Router->m_mapParameters.find(iPK_DeviceData)==pDeviceData_Router->m_mapParameters.end() )
				*sValue_To_Assign="BAD PARAMETER";
			else
				*sValue_To_Assign=pDeviceData_Router->m_mapParameters[iPK_DeviceData];
		}
		return;
	}
	else
	{
		// TODO -- Send device request
	}
}

//<-dceag-c71-b->

	/** @brief COMMAND: #71 - Request File */
	/** Get the contents of a file from the server */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */

void General_Info_Plugin::CMD_Request_File(string sFilename,char **pData,int *iData_Size,string &sCMD_Result,Message *pMessage)
//<-dceag-c71-e->
{
	size_t Length;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

g_pPlutoLogger->Write(LV_CRITICAL, "base: %s", m_pRouter->sBasePath_get().c_str());

	if( c && Length )
		g_pPlutoLogger->Write(LV_FILEREQUEST, "sending file: %s size: %d", sFilename.c_str(),(int) Length);
	else
		g_pPlutoLogger->Write(LV_CRITICAL, "requested missing file: %s", sFilename.c_str());

	*iData_Size = (int) Length;
	*pData = c;
}

//<-dceag-c79-b->

	/** @brief COMMAND: #79 - Add Unknown Device */
	/** Adds an unknown device into the database.  These are devices that are not part of the Pluto system. */
		/** @param #9 Text */
			/** A description of the device */
		/** @param #10 ID */
			/** The IP Address */
		/** @param #47 Mac address */
			/** The MAC address of the device */

void General_Info_Plugin::CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,Message *pMessage)
//<-dceag-c79-e->
{
	Row_UnknownDevices *pRow_UnknownDevices = m_pDatabase_pluto_main->UnknownDevices_get()->AddRow();
	pRow_UnknownDevices->MacAddress_set(sMac_address);
	m_pDatabase_pluto_main->UnknownDevices_get()->Commit();
g_pPlutoLogger->Write(LV_STATUS,"uknown device, setting: %d to mac: %s",pRow_UnknownDevices->PK_UnknownDevices_get(),pRow_UnknownDevices->MacAddress_get().c_str());
}
//<-dceag-c239-b->

	/** @brief COMMAND: #239 - Request File And Checksum */
	/** Get the contents of a file from the server and the checksum of the file */
		/** @param #13 Filename */
			/** The file to get */
		/** @param #19 Data */
			/** The file's contents */
		/** @param #91 Checksum */
			/** The checksum of the file */
		/** @param #92 Checksum Only */
			/** If this is true, this command will return only the checksum of the file, Data will be null. */

void General_Info_Plugin::CMD_Request_File_And_Checksum(string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only,string &sCMD_Result,Message *pMessage)
//<-dceag-c239-e->
{
	cout << "Command #239 - Request File And Checksum" << endl;
	cout << "Parm #13 - Filename=" << sFilename << endl;
	cout << "Parm #19 - Data  (data value)" << endl;
	cout << "Parm #91 - Checksum=" << sChecksum << endl;
	cout << "Parm #92 - Checksum_Only=" << bChecksum_Only << endl; 

	g_pPlutoLogger->Write(LV_FILEREQUEST, "request missing  file: %s", sFilename.c_str());
	size_t Length = 0;
	char *c = FileUtils::ReadFileIntoBuffer(sFilename, Length);
	if( c==NULL && m_pRouter )
		c = FileUtils::ReadFileIntoBuffer(m_pRouter->sBasePath_get() + sFilename, Length);

	if(c==NULL) //file not found
	{
		g_pPlutoLogger->Write(LV_FILEREQUEST, "The requested file '%s' was not found", sFilename.c_str());
		return;
	}

	*iData_Size = (int) Length;
	*pData = c;

	//compute the checksum
	MD5_CTX ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)*pData, (unsigned int)*iData_Size);
	unsigned char digest[16];
	MD5Final(digest, &ctx);

	char tmp[3];
	string md5;
	for (int i = 0; i < 16; i++)
	{
		sprintf(tmp, "%02x", digest[i]);
		md5 += tmp;
	}

	*sChecksum = md5;

	if(*bChecksum_Only)
	{
		*iData_Size = 0;

		if(NULL != *pData)
		{
			delete *pData;
			*pData = NULL;
		}
	}
}
//<-dceag-c246-b->

	/** @brief COMMAND: #246 - Set Device Data */
	/** Gets the device data for a device */
		/** @param #2 PK_Device */
			/** The device to set */
		/** @param #5 Value To Assign */
			/** The value to assign */
		/** @param #52 PK_DeviceData */
			/** The device data */

void General_Info_Plugin::CMD_Set_Device_Data(int iPK_Device,string sValue_To_Assign,int iPK_DeviceData,string &sCMD_Result,Message *pMessage)
//<-dceag-c246-e->
{
}

//<-dceag-c247-b->

	/** @brief COMMAND: #247 - Get Device State */
	/** Gets a device's current state */

void General_Info_Plugin::CMD_Get_Device_State(string &sCMD_Result,Message *pMessage)
//<-dceag-c247-e->
{
}

//<-dceag-c248-b->

	/** @brief COMMAND: #248 - Get Device Status */
	/** Gets the status for a device */

void General_Info_Plugin::CMD_Get_Device_Status(string &sCMD_Result,Message *pMessage)
//<-dceag-c248-e->
{
}
//<-dceag-c268-b->

	/** @brief COMMAND: #268 - Reboot */
	/** Reboots the given computer. */
		/** @param #2 PK_Device */
			/** The computer to reboot.  This is ignored when AppServer receives the command--it reboots itself.  When General Info Plugin receives it, it reboots the mentioned computer. */

void General_Info_Plugin::CMD_Reboot(int iPK_Device,string &sCMD_Result,Message *pMessage)
//<-dceag-c268-e->
{
}
//<-dceag-createinst-b->!
//<-dceag-c272-b->

	/** @brief COMMAND: #272 - Restart DCERouter */
	/** Causes DCERouter to exit and restart. */

void General_Info_Plugin::CMD_Restart_DCERouter(string &sCMD_Result,Message *pMessage)
//<-dceag-c272-e->
{
	Message *pMessageOut = new Message(m_dwPK_Device,DEVICEID_DCEROUTER,PRIORITY_NORMAL,MESSAGETYPE_SYSCOMMAND,SYSCOMMAND_RELOAD,0);
	SendMessageToRouter(pMessageOut);
}
//<-dceag-c67-b->

	/** @brief COMMAND: #67 - Spawn Application */
	/** Spawns on application */
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

void General_Info_Plugin::CMD_Spawn_Application(string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,string &sCMD_Result,Message *pMessage)
//<-dceag-c67-e->
{
    if ( sName == "" )
        sName = "not named";

    if (sFilename == "" && sArguments == "")
    {
        g_pPlutoLogger->Write(LV_WARNING, "StartApp: Received empty Executable and Parameters for '%s'", sName.c_str());
        return;
    }

#ifndef WIN32
    //parse the args
    const int MaxArgs = 32;

    char * ptr;
    char * params;
    params = ptr = (char *) sArguments.c_str();
    char * args[MaxArgs];
    int i = 0;

    args[0] = (char *) sArguments.c_str();
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

            if ( execvp(sName.c_str(), args) == -1)
                exit(99);
        }

        case -1:
            g_pPlutoLogger->Write(LV_CRITICAL, "Error starting %s, err: %s", sFilename.c_str(), strerror(errno));
            return;
            break;

        default:
            return;
    }
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
    CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	return;
#endif
}
//<-dceag-c274-b->

	/** @brief COMMAND: #274 - Set Room For Device */
	/** Updates the record in the database for a given device putting in a certain room. */
		/** @param #2 PK_Device */
			/** The device */
		/** @param #57 PK_Room */
			/** The room */

void General_Info_Plugin::CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room,string &sCMD_Result,Message *pMessage)
//<-dceag-c274-e->
{
	Row_Device *pRow_Device = m_pDatabase_pluto_main->Device_get()->GetRow(iPK_Device);
	Row_Room *pRow_Room = m_pDatabase_pluto_main->Room_get()->GetRow(iPK_Room);
	if( !pRow_Device || !pRow_Room )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot set device %d to room %d",iPK_Device,iPK_Room);
		return;
	}
g_pPlutoLogger->Write(LV_STATUS,"Setting device %d to room %d",iPK_Device,iPK_Room);
	pRow_Device->FK_Room_set( pRow_Room->PK_Room_get() );
	pRow_Device->Table_Device_get()->Commit();
}
