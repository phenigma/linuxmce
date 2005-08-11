#include "SelfUpdate.h"

#include <WINDOWS.H>

#ifndef WINCE
#include <assert.h>
#endif

#include <iostream>
using namespace std;

#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/md5.h"
#include "pluto_main/Define_DeviceCategory.h"

using namespace DCE;

#ifdef WINCE
		#ifdef SMARTPHONE
			const string csOrbiter_Update("/usr/pluto/bin/Orbiter_Smartphone.dat");
		#else
			const string csOrbiter_Update("/usr/pluto/bin/Orbiter_WinCE.dat");
		#endif
#else
		const string csOrbiter_Update("/usr/pluto/bin/Orbiter_Win32.dat");
#endif

#ifdef POCKETFROG
	#include "Orbiter_PocketFrog.h"
#else
	#ifdef WINCE
		#include "OrbiterSDL_WinCE.h"
	#else
		#include "OrbiterSDL_Win32.h"
	#endif
#endif 


//-----------------------------------------------------------------------------------------------------
#include "MainDialog.h"
extern CommandLineParams CmdLineParams;
//-----------------------------------------------------------------------------------------------------
OrbiterSelfUpdate::OrbiterSelfUpdate(Orbiter *pOrbiter)
{
	assert(NULL != pOrbiter);
	m_pOrbiter = pOrbiter;

	char pProcessFilePath[256];
	GetProcessFilePath(pProcessFilePath);
	m_sOrbiterFilePath = string(pProcessFilePath);
}
//-----------------------------------------------------------------------------------------------------
OrbiterSelfUpdate::~OrbiterSelfUpdate()
{
	m_pOrbiter = NULL;
}
//-----------------------------------------------------------------------------------------------------
void OrbiterSelfUpdate::GetProcessFilePath(char *pProcessFilePath)
{
#ifdef WINCE
	wchar_t pProcessNameW[256];
	::GetModuleFileName(NULL, pProcessNameW, sizeof(pProcessNameW));
	wcstombs(pProcessFilePath, pProcessNameW, 256);
#else
    ::GetModuleFileName(NULL, pProcessFilePath, 256);
#endif

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "Orbiter's full path: %s", pProcessFilePath);
#endif
}
//-----------------------------------------------------------------------------------------------------
string OrbiterSelfUpdate::GetOrbiterCheckSum()
{
	string sMD5FilePath = m_sOrbiterFilePath;
	StringUtils::Replace(&sMD5FilePath, ".exe", ".MD5");
	StringUtils::Replace(&sMD5FilePath, ".EXE", ".MD5");

	if(!FileUtils::FileExists(sMD5FilePath))
		FileUtils::WriteBufferIntoFile(sMD5FilePath, "dummy			", 5);

	//read data from the comm file
	size_t iMD5Size;
	char *pMD5Data = FileUtils::ReadFileIntoBuffer(sMD5FilePath, iMD5Size);

	if(NULL == pMD5Data)
		return "";

	string sChecksum(pMD5Data);
	delete pMD5Data;

	return sChecksum;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::UpdateAvailable()
{
	char *pUpdateFile = NULL;
	int iSizeUpdateFile = 0;

	string sChecksum;
	bool bChecksumOnly = true;

	//get the checksum for the update file
	DCE::CMD_Request_File_And_Checksum_Cat CMD_Request_File_And_Checksum_Cat(
		m_pOrbiter->m_dwPK_Device, 
		DEVICECATEGORY_General_Info_Plugins_CONST, 
		false,
		BL_SameHouse,
		csOrbiter_Update.c_str(),
		&pUpdateFile, 
		&iSizeUpdateFile, 
		&sChecksum, 
		&bChecksumOnly
		);

	m_pOrbiter->SendCommand(CMD_Request_File_And_Checksum_Cat);

	if(sChecksum == "") 
	{
		//retriving the checksum from the server failed
		g_pPlutoLogger->Write( LV_CRITICAL,  "The update file is missing on the server" );
		return false; //we'll continue with this version
	}

	string sActualChecksum = GetOrbiterCheckSum();

	g_pPlutoLogger->Write( LV_STATUS,  "Current checksum : %s orbiter %s", sActualChecksum.c_str(), sChecksum.c_str() );	

	if(sActualChecksum == sChecksum)
		return false; //no updates needed
	else
		return true;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::DownloadUpdateBinary()
{
	//we've got an update
	char *pUpdateFile = NULL;
	int iSizeUpdateFile = 0;	

	string sUpdateName = m_pOrbiter->DATA_Get_Update_Name();
	string sStoragePath = m_pOrbiter->DATA_Get_Path();

	DCE::CMD_Request_File_Cat CMD_Request_File_Cat(
		m_pOrbiter->m_dwPK_Device, 
		DEVICECATEGORY_General_Info_Plugins_CONST, 
		false,
		BL_SameHouse,
		"/usr/pluto/bin/" + sUpdateName, //update binary file
		&pUpdateFile,
		&iSizeUpdateFile
		);
	m_pOrbiter->SendCommand(CMD_Request_File_Cat);

	if ( !iSizeUpdateFile )
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "The UpdateBinary application '%s' is missing on the server.", sUpdateName.c_str());
		return false; //we'll continue with this version
	}

    if(sStoragePath == "")
        m_sUpdateBinaryPath = sUpdateName;
    else
        m_sUpdateBinaryPath = sStoragePath + "/" + sUpdateName;

#ifdef WINCE
	wchar_t UpdateFileNameW[256];
	mbstowcs(UpdateFileNameW, m_sUpdateBinaryPath.c_str(), 256);
#define UPDATEBINARY_FILE UpdateFileNameW	
#else
#define UPDATEBINARY_FILE const_cast<char *>(m_sUpdateBinaryPath.c_str())
#endif		

	g_pPlutoLogger->Write( LV_STATUS,  "Ready to delete UpdateBinary application");
	::DeleteFile(UPDATEBINARY_FILE);

	if(!FileUtils::WriteBufferIntoFile(m_sUpdateBinaryPath, pUpdateFile, iSizeUpdateFile))
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Failed to save the UpdateBinary application to location '%s'", m_sUpdateBinaryPath.c_str() );

        string sOldUpdateBinaryPath = m_sUpdateBinaryPath;
        m_sUpdateBinaryPath = g_sBinaryPath + sUpdateName; //try local folder
        if(!FileUtils::WriteBufferIntoFile(m_sUpdateBinaryPath, pUpdateFile, iSizeUpdateFile))    
        {
            g_pPlutoLogger->Write( LV_CRITICAL,  "Failed to save the UpdateBinary application to location '%s'", m_sUpdateBinaryPath.c_str() );
            ::MessageBox(NULL, TEXT("Unable to download UpdateBinary to the specified folder. Please check if the 'Path' device data is correct"),
                TEXT("Pluto Orbiter error"), MB_ICONERROR);

            return false; //we'll continue with this version
        }
	}

	return true;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::CreateCommunicationFile()
{
	string sCommFile = m_pOrbiter->DATA_Get_Communication_file();

#ifdef WINCE
	char sTextBuffer[256];
	wcstombs(sTextBuffer, GetCommandLine(), 256);

	string sData = m_sOrbiterFilePath + " " + sTextBuffer;
#else
	string sData = GetCommandLine();
#endif

	if(!FileUtils::WriteBufferIntoFile(sCommFile, const_cast<char *>(sData.c_str()), sData.length()))
		return false; 
	else
		return true;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::SpawnUpdateBinaryProcess()
{
	g_pPlutoLogger->Write( LV_STATUS,  "Orbiter SelfUpdate: Spawning UpdateBinary process..." );

	//starting the new orbiter
	PROCESS_INFORMATION pi;
	::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	STARTUPINFO si;
	::ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = 0;

	string sUpdateName = m_pOrbiter->DATA_Get_Update_Name();
	string sStoragePath = m_pOrbiter->DATA_Get_Path();
	string sCommFile = m_pOrbiter->DATA_Get_Communication_file();
	string sCmdLine = "";

	sCmdLine += "-d " + StringUtils::ltos(m_pOrbiter->m_dwPK_Device);
	sCmdLine += " -r " + CmdLineParams.sRouter_IP;

	sCmdLine += " -l " + sUpdateName + ".log";
	sCmdLine += " -i " + csOrbiter_Update;
	sCmdLine += " -o \"" + m_sOrbiterFilePath + "\"";
	sCmdLine += " -c " + sCommFile;

	g_pPlutoLogger->Write( LV_STATUS,  "Ready to start: %s comm file %s", m_sUpdateBinaryPath.c_str(), sCmdLine.c_str());

#ifdef WINCE
	wchar_t CmdLineW[512];
	mbstowcs(CmdLineW, sCmdLine.c_str(), 512);
	wchar_t OrbiterPathW[512];
	mbstowcs(OrbiterPathW, m_sUpdateBinaryPath.c_str(), 512);

	if(!::CreateProcess(OrbiterPathW, CmdLineW, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi))
#else
	string sCompleteCmdLine = m_sUpdateBinaryPath + " " + sCmdLine;
	if(!::CreateProcess(NULL, const_cast<char *>(sCompleteCmdLine.c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi))
#endif
	{
        string sErrorMessage = "Failed to start UpdateBinary application '" + m_sUpdateBinaryPath + "'";
		g_pPlutoLogger->Write( LV_CRITICAL,  sErrorMessage.c_str() );

        
        ::MessageBox(NULL, TEXT("Failed to start UpdateBinary application.\r\nPlease be sure that you are running the correct orbiter."), 
			TEXT("Pluto Orbiter failed to update"), MB_ICONWARNING);
		return false; 
	}

	return true; //close the process, a new orbiter is starting
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::LastUpdateFailed()
{
	string sCommFile = m_pOrbiter->DATA_Get_Communication_file();

	if(FileUtils::FileExists(sCommFile))
		return true; //UpdateBinary app should delete this file if everything was ok
	else
		return false;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::Run()
{
#ifdef POCKETFROG
	((Orbiter_PocketFrog *)m_pOrbiter)->WriteStatusOutput("Updating orbiter...");
#else
	#ifdef WINCE
		((OrbiterSDL_WinCE *)m_pOrbiter)->WriteStatusOutput("Updating orbiter...");
	#else
		((OrbiterSDL_Win32 *)m_pOrbiter)->WriteStatusOutput("Updating orbiter...");
	#endif 
#endif

	if(LastUpdateFailed())
	{	
        if(IDYES == ::MessageBox(NULL, TEXT("Last time I tried to update it failed. Should I try again?  \r\nWe recommend you choose 'yes' unless the update keeps failing,\r\nin which case please notify Pluto support."), 
            TEXT("Last update failed"), MB_YESNO | MB_ICONWARNING))
        {
            g_pPlutoLogger->Write( LV_CRITICAL,  "The user chooses to try to update Orbiter again, let's try this..." );
        }
        else
        {
    		g_pPlutoLogger->Write( LV_CRITICAL,  "Last update failed. We won't try to update again." );
	    	return false;
        }
	}

#ifdef DEBUG
	g_pPlutoLogger->Write( LV_STATUS,  "Last update didn't fail or the user decided to try to update again." );
#endif

	if(!UpdateAvailable())
	{
#ifdef DEBUG
		g_pPlutoLogger->Write( LV_STATUS,  "No update available on the server." );
#endif
		return false;
	}
	g_pPlutoLogger->Write( LV_STATUS,  "Update available on the server." );

	if(!DownloadUpdateBinary())
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to download UpdateBinary application from the server." );
		return false;
	}
	g_pPlutoLogger->Write( LV_STATUS,  "UpdateBinary app downloaded from the server." );

	if(!CreateCommunicationFile())
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to create the communication file for UpdateBinary application." );
		return false;
	}
	g_pPlutoLogger->Write( LV_STATUS,  "Communication file created." );

	return SpawnUpdateBinaryProcess();
}
//-----------------------------------------------------------------------------------------------------