#include "SelfUpdate.h"

#include <WINDOWS.H>
#include <assert.h>

#include <iostream>
using namespace std;

#include "Gen_Devices/AllCommandsRequests.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/md5.h"
#include "pluto_main/Define_DeviceCategory.h"

using namespace DCE;

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
bool OrbiterSelfUpdate::IsTheNewOrbiter()
{
	return string::npos == m_sOrbiterFilePath.find("Orbiter.exe");
}
//-----------------------------------------------------------------------------------------------------
string OrbiterSelfUpdate::GetOrbiterCheckSum()
{
	//load the file into a buffer
	unsigned int iSizeOrbiterFile;
	char *pOrbiterFile = FileUtils::ReadFileIntoBuffer(m_sOrbiterFilePath, iSizeOrbiterFile);

	//compute the checksum
	MD5_CTX ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)pOrbiterFile, (unsigned int)iSizeOrbiterFile);
	unsigned char digest[16];
	MD5Final(digest, &ctx);

	char tmp[3];
	string sChecksum;
	for (int i = 0; i < 16; i++)
	{
		sprintf(tmp, "%02x", digest[i]);
		sChecksum += tmp;
	}

	return sChecksum;
}
//-----------------------------------------------------------------------------------------------------
bool OrbiterSelfUpdate::Run()
{
	if(!IsTheNewOrbiter())
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
#ifdef WINCE
			"Orbiter_WinCE.dat",
#else
			"Orbiter_Win32.dat",
#endif
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


#ifdef WINCE
		//load the file into a buffer
		unsigned int iSizeOrbiterFile;
		char *pOrbiterFile = FileUtils::ReadFileIntoBuffer(m_sOrbiterFilePath, iSizeOrbiterFile);

		//compute the checksum
		MD5_CTX ctx;
		MD5Init(&ctx);
		MD5Update(&ctx, (unsigned char *)pOrbiterFile, (unsigned int)iSizeOrbiterFile);
		unsigned char digest[16];
		MD5Final(digest, &ctx);

		char tmp[3];
		string sActualChecksum;
		for (int i = 0; i < 16; i++)
		{
			sprintf(tmp, "%02x", digest[i]);
			sActualChecksum += tmp;
		}		

		//Stlport is crashing when I'm returning a string in WinCE, so I won't use GetOrbiterCheckSum method
#else
		string sActualChecksum = GetOrbiterCheckSum();
#endif
		
		if(sActualChecksum == sChecksum)
			return false; //no updates needed

		//we've got an update
		DCE::CMD_Request_File_Cat CMD_Request_File_Cat(
			m_pOrbiter->m_dwPK_Device, 
			DEVICECATEGORY_General_Info_Plugins_CONST, 
			false,
			BL_SameHouse,
#ifdef WINCE
			"Orbiter_WinCE.dat",
#else
			"Orbiter_Win32.dat",
#endif
			&pUpdateFile,
			&iSizeUpdateFile
		);
		m_pOrbiter->SendCommand(CMD_Request_File_Cat);

		if ( !iSizeUpdateFile )
		{
			g_pPlutoLogger->Write( LV_CRITICAL,  "The update file is missing on the server." );
			return false; //we'll continue with this version
		}

        //save the file to "OrbiterNew.exe"
#ifdef WINCE
		string sNewOrbiterPath("/OrbiterNew.exe"); //in the root folder		
#else
		string sNewOrbiterPath = m_sOrbiterFilePath;
		sNewOrbiterPath = StringUtils::Replace(sNewOrbiterPath, "Orbiter.exe", "OrbiterNew.exe");
#endif

		if(!FileUtils::WriteBufferIntoFile(sNewOrbiterPath, pUpdateFile, iSizeUpdateFile))
		{
			g_pPlutoLogger->Write( LV_CRITICAL,  "Failed to save the update file" );
			return false; //we'll continue with this version
		}

		g_pPlutoLogger->Write( LV_CRITICAL,  "Orbiter SelfUpdate: Spawning OrbiterNew process..." );

		//starting the new orbiter
		PROCESS_INFORMATION pi;
		::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		STARTUPINFO si;
		::ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = 0;

#ifdef WINCE
		char CmdLine[256];
		wcstombs(CmdLine, ::GetCommandLine(), 256);
		sNewOrbiterPath = sNewOrbiterPath + " " + string(CmdLine);
#else
		string CommandLine = string(::GetCommandLine());
		sNewOrbiterPath = StringUtils::Replace(CommandLine, "Orbiter.exe", "OrbiterNew.exe");
#endif

#ifdef WINCE
		wchar_t NewOrbiterCmdLineW[256];
		mbstowcs(NewOrbiterCmdLineW, sNewOrbiterPath.c_str(), 256);

		#define NEWORBITER_CMD_LINE NewOrbiterCmdLineW	
#else
		#define NEWORBITER_CMD_LINE const_cast<char *>(sNewOrbiterPath.c_str())
#endif

		g_pPlutoLogger->Write( LV_CRITICAL,  "Ready to start OrbiterNew from the file: %s", sNewOrbiterPath.c_str());
		::CreateProcess(NULL, NEWORBITER_CMD_LINE, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);


		return true; //close the process, a new orbiter is starting
	}
	else // OrbiterNew.exe
	{
		g_pPlutoLogger->Write( LV_CRITICAL,  "We are in OrbiterNew.exe now." );

		string sOrbiterPath = m_sOrbiterFilePath;
		sOrbiterPath = StringUtils::Replace(sOrbiterPath, "OrbiterNew.exe", "Orbiter.exe");

#ifdef WINCE
		wchar_t OrbiterPathW[256];
		mbstowcs(OrbiterPathW, m_sOrbiterFilePath.c_str(), 256);
		#define ORBITER_PATH OrbiterPathW	

		wchar_t NewOrbiterPathW[256];
		mbstowcs(NewOrbiterPathW, "/InternalStorage/Orbiter.exe", 256);
		#define NEWORBITER_PATH OrbiterPathW	

#else
		#define ORBITER_PATH m_sOrbiterFilePath.c_str()
		#define NEWORBITER_PATH sOrbiterPath.c_str()
#endif


		while(!::CopyFile(ORBITER_PATH, NEWORBITER_PATH, false))
		{
			g_pPlutoLogger->Write( LV_CRITICAL, "Unable to copy '%s' over '%s'", 
				m_sOrbiterFilePath.c_str(), sOrbiterPath.c_str()
			);

			Sleep(2000);
		}

		g_pPlutoLogger->Write( LV_CRITICAL,  "Orbiter's file updated successfully." );

		//starting the new orbiter
		PROCESS_INFORMATION pi;
		::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		STARTUPINFO si;
		::ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.lpReserved = 0;

#ifdef WINCE
		char CmdLine[256];
		wcstombs(CmdLine, ::GetCommandLine(), 256);
		string CommandLine = "/InternalStorage/Orbiter.exe " + string(CmdLine); 
#else
		string CommandLine = string(::GetCommandLine());
		sOrbiterPath = StringUtils::Replace(CommandLine, "OrbiterNew.exe", "Orbiter.exe");
#endif

		g_pPlutoLogger->Write( LV_CRITICAL,  "Starting the updated orbiter..." );

#ifdef WINCE
		wchar_t OrbiterCmdLineW[256];
		mbstowcs(OrbiterCmdLineW, sOrbiterPath.c_str(), 256);
		#define ORBITER_CMD_LINE OrbiterCmdLineW	
#else
		#define ORBITER_CMD_LINE const_cast<char *>(sOrbiterPath.c_str())
#endif		

		::CreateProcess(NULL, ORBITER_CMD_LINE, NULL, NULL, NULL, 0, NULL, NULL, &si, &pi);
		return true; //close the process, a new orbiter is starting
	}

	return false;	
}
//-----------------------------------------------------------------------------------------------------