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
	const string csOrbiter_Update("/usr/pluto/bin/Orbiter_WinCE.dat");
#else
	const string csOrbiter_Update("/usr/pluto/bin/Orbiter_Win32.dat");
#endif

#ifdef WINCE
#include "OrbiterSDL_WinCE.h"
#else
#include "OrbiterSDL_Win32.h"
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
	int iNumArgs;

#ifdef WINCE
	wchar_t pProcessNameW[256];
	::GetModuleFileName(NULL, pProcessNameW, sizeof(pProcessNameW));
	wcstombs(pProcessFilePath, pProcessNameW, 256);
#else
	LPSTR pCmdLine = GetCommandLine();

	wchar_t pCmdLineW[256];
	mbstowcs(pCmdLineW, pCmdLine, 256);
	LPWSTR* pCmdLineParams = CommandLineToArgvW(pCmdLineW, &iNumArgs);

	wchar_t *pProcessNameW = pCmdLineParams[0];
	wcstombs(pProcessFilePath, pProcessNameW, 256);
#endif
}
//-----------------------------------------------------------------------------------------------------
string OrbiterSelfUpdate::GetOrbiterCheckSum()
{
	string sMD5FilePath = m_sOrbiterFilePath;
	StringUtils::Replace(sMD5FilePath, ".exe", ".MD5");
	StringUtils::Replace(sMD5FilePath, ".EXE", ".MD5");

	//read data from the comm file
	size_t iMD5Size;
	char *pMD5Data = FileUtils::ReadFileIntoBuffer(sMD5FilePath, iMD5Size);

	if(NULL == pMD5Data)
		return "";

	return string(pMD5Data);
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

	if(sActualChecksum == "")
	{
		//no MD5 file found
		g_pPlutoLogger->Write( LV_CRITICAL,  "The MD5 file is missing." );
		return false; //we'll continue with this version
	}

	g_pPlutoLogger->Write( LV_STATUS,  "sActualChecksum : %s", sActualChecksum.c_str() );	
	g_pPlutoLogger->Write( LV_STATUS,  "sChecksum : %s", sChecksum.c_str() );	

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
		g_pPlutoLogger->Write( LV_CRITICAL,  "The update file '%s' is missing on the server.", sUpdateName.c_str());
		return false; //we'll continue with this version
	}

	string sUpdateBinaryPath = sStoragePath + sUpdateName;

#ifdef WINCE
	wchar_t UpdateFileNameW[256];
	mbstowcs(UpdateFileNameW, sUpdateBinaryPath.c_str(), 256);
#define UPDATEBINARY_FILE UpdateFileNameW	
#else
#define UPDATEBINARY_FILE const_cast<char *>(sUpdateBinaryPath.c_str())
#endif		

	g_pPlutoLogger->Write( LV_CRITICAL,  "Ready to delete UpdateBinary.exe file");
	::DeleteFile(UPDATEBINARY_FILE);

	if(!FileUtils::WriteBufferIntoFile(sUpdateBinaryPath, pUpdateFile, iSizeUpdateFile))
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Failed to save the update file to location '%s'", sUpdateBinaryPath.c_str() );
		return false; //we'll continue with this version
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
	string sUpdateBinaryFilePath = sStoragePath + sUpdateName;

	string sCmdLine = "";

	sCmdLine += "-d " + StringUtils::ltos(CmdLineParams.PK_Device);
	sCmdLine += " -r " + CmdLineParams.sRouter_IP;

	sCmdLine += " -l " + sUpdateName + ".log";
	sCmdLine += " -i " + csOrbiter_Update;
	sCmdLine += " -o \"" + m_sOrbiterFilePath + "\"";
	sCmdLine += " -c " + sCommFile;

	g_pPlutoLogger->Write( LV_CRITICAL,  "Ready to start: %s", sUpdateBinaryFilePath.c_str());
	g_pPlutoLogger->Write( LV_CRITICAL,  "Communication file: %s", sCmdLine.c_str());

#ifdef WINCE
	wchar_t CmdLineW[256];
	mbstowcs(CmdLineW, sCmdLine.c_str(), 256);
	wchar_t OrbiterPathW[256];
	mbstowcs(OrbiterPathW, sUpdateBinaryFilePath.c_str(), 256);

	if(!::CreateProcess(OrbiterPathW, CmdLineW, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi))
#else
	sUpdateBinaryFilePath += " " + sCmdLine;
	if(!::CreateProcess(NULL, const_cast<char *>(sUpdateBinaryFilePath.c_str()), NULL, NULL, NULL, 0, NULL, NULL, &si, &pi))
#endif
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Failed to start UpdateBinary application '%s'", sUpdateBinaryFilePath.c_str() );
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
#ifdef WINCE
	((OrbiterSDL_WinCE *)m_pOrbiter)->WriteStatusOutput("Updating orbiter...");
#else
	((OrbiterSDL_Win32 *)m_pOrbiter)->WriteStatusOutput("Updating orbiter...");
#endif 

	if(LastUpdateFailed())
	{	
		g_pPlutoLogger->Write( LV_CRITICAL,  "Last update failed. We won't try to update again." );
		return false;
	}
	g_pPlutoLogger->Write( LV_CRITICAL,  "Last update didn't failed." );

	if(!UpdateAvailable())
	{
		g_pPlutoLogger->Write( LV_STATUS,  "No update available on the server." );
		return false;
	}
	g_pPlutoLogger->Write( LV_CRITICAL,  "Update available on the server." );

	if(!DownloadUpdateBinary())
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to download UpdateBinary application from the server." );
		return false;
	}
	g_pPlutoLogger->Write( LV_CRITICAL,  "Downloaded UpdateBinary app from the server." );

	if(!CreateCommunicationFile())
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "Unable to create the commuication file for UpdateBinary applicaiton." );
		return false;
	}
	g_pPlutoLogger->Write( LV_CRITICAL,  "Created comm file." );

	SpawnUpdateBinaryProcess();
	return true;	//need restart
}
//-----------------------------------------------------------------------------------------------------