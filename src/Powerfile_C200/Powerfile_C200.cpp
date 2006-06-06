//<-dceag-d-b->
#include "Powerfile_C200.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "PlutoUtils/ProcessUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "pluto_media/Database_pluto_media.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Define_AttributeType.h"

using namespace StringUtils;

namespace nsJobHandler
{
	int g_ID = 0;
}

#ifdef WIN32
	#define WEXITSTATUS(a) 0
#endif

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Powerfile_C200::Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Powerfile_C200_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, m_bStatusCached(false), m_pJob(NULL), m_DriveMutex("Powerfile drive mutex", true)
{
    m_pDatabase_pluto_media = NULL;
	m_pMediaAttributes_LowLevel = NULL;
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Powerfile_C200::~Powerfile_C200()
//<-dceag-dest-e->
{
	for (size_t i = 0; i < m_vectDDF.size(); i++)
		delete m_vectDDF[i];
	delete m_pDatabase_pluto_media;
}

// Call Tokenize directly if you don't need to strip out eventual empty fields
static void ExtractFields(string &sRow, vector<string> &vect_sResult, const char * separator = " \t")
{
	vector<string> vect_sFields;
	vect_sResult.clear();
	vect_sFields.clear();
	Tokenize(sRow, separator, vect_sFields);

	for (size_t i = 0; i < vect_sFields.size(); i++)
	{
		if (vect_sFields[i] == "")
			continue; // skip empty fields since there is no such thing
		vect_sResult.push_back(vect_sFields[i]);
	}
}

bool Powerfile_C200::RippingProgress(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo)
{
	int iDrive_Number = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Drive_Number_CONST].c_str());
	int iResult = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str());
	int iPK_Device = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
	string sDrive = pMessage->m_mapParameters[EVENTPARAMETER_Text_CONST];
	int iPercent = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST].c_str());
	string sName = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];

	g_pPlutoLogger->Write(LV_STATUS, "RippingProgress iResult=%d, iPK_Device=%d", iResult, iPK_Device);
	if (iPK_Device != m_dwPK_Device)
		return false; // it's not one of ours
	
	if (iResult != RIP_RESULT_SUCCESS && iResult != RIP_RESULT_FAILURE && iResult != RIP_RESULT_STILLGOING)
		return false; // we only treat these

	if (iResult == RIP_RESULT_STILLGOING)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Received RippingProgress message for drive %s, percent %d, name: '%s'", sDrive.c_str(), iPercent, sName.c_str());
		int iDrive_Number = -1;
		for (size_t i = 0; i < m_vectDrive.size(); i++)
		{
			if (m_vectDrive[i].first == sDrive)
			{
				iDrive_Number = i;
				break;
			}
		}

		if (iDrive_Number == -1)
		{
			// not handled by us
			return false;
		}

		m_pJob->RippingProgress_Going(iDrive_Number, iPercent, sName);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "Ripping progress for drive number %d, result '%s'", iDrive_Number, iResult == RIP_RESULT_SUCCESS ? "success" : "fail");
		m_pJob->RippingProgress_End(iDrive_Number, iResult);
	}
	
	return true;
}

// Return Disk_Drive_Functions instance for requested drive
Disk_Drive_Functions * Powerfile_C200::GetDDF(int iDrive_Number)
{
	if (iDrive_Number < 0 || iDrive_Number >= m_vectDDF.size())
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Invalid drive index %d (max: %d)", iDrive_Number, m_vectDDF.size() - 1);
		return NULL;
	}
	return m_vectDDF[iDrive_Number];
}

bool Powerfile_C200::Get_Jukebox_Status(string * sJukebox_Status, bool bForce)
{
	bool bComma = false, bResult = false;
	string sOutput;
	if (sJukebox_Status)
		* sJukebox_Status = "";

	if (! m_bStatusCached || bForce)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Getting fresh status info (changer:%s) %s", m_sChanger.c_str(), bForce ? " (Forced)" : "");
		m_vectDriveStatus.clear();
		m_vectSlotStatus.clear();
#ifdef EMULATE_PF
		char * args[] = {"/bin/cat", "/tmp/samples/mtx-status", NULL};
#else
		char * args[] = {"/usr/sbin/mtx", "-f", (char *) m_sChanger.c_str(), "status", NULL};
#endif
		if (ProcessUtils::GetCommandOutput(args[0], args, sOutput))
		{
#ifdef WIN32
			size_t size;
			char *pBuffer = FileUtils::ReadFileIntoBuffer("/temp/status",size);
			sOutput = pBuffer;
#endif
			vector<string> vect_sOutput_Rows;
			
			Tokenize(sOutput, "\n", vect_sOutput_Rows);
			for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
			{
				string sResult = "";
				vector<string> vsFF;
				string sWhoWhat;
				vector<string> vsC; // C = Components

				ExtractFields(vect_sOutput_Rows[i], vsFF);
				if( vsFF.size()==0 )
					g_pPlutoLogger->Write(LV_CRITICAL,"Extract fields failed to get anything from %d: %s",i,vect_sOutput_Rows[i].c_str());
				else if (vsFF[0] == "Data" && vsFF.size()>=4 )
				{
					int iDiscFrom = 0;
					sWhoWhat = vsFF[3];
					Tokenize(sWhoWhat, ":", vsC); // Unit_number:State
					
					sResult = string("D") + vsC[0] + "=" + vsC[1];
					if (vsC[1] == "Full")
					{
						sResult += string("-") + vsFF[6];
						sscanf(vsFF[6].c_str(), "%d", &iDiscFrom);
					}
					m_vectDriveStatus.push_back(pair<int, bool>(iDiscFrom, true));
				}
				else if (vsFF.size() == 3 && vsFF[0] == "Storage" && vsFF[1] == "Element")
				{
					sWhoWhat = vsFF[2];
					Tokenize(sWhoWhat, ":", vsC);
					
					sResult = string("S") + vsC[0] + "=" +vsC[1];
					m_vectSlotStatus.push_back(vsC[1] == "Full");
				}
				else
					g_pPlutoLogger->Write(LV_CRITICAL,"Don't know what to do with vsFF[0]=%s and size=%d",
						vsFF[0].c_str(),(int) vsFF.size());
				
				if (sJukebox_Status && sResult != "")
				{
					if (bComma)
						* sJukebox_Status += ",";
					else
						bComma = true;
					* sJukebox_Status += sResult;
				}
			}
			g_pPlutoLogger->Write(LV_STATUS, "Finished getting device status");
			m_bStatusCached = true;
			bResult = true;
		}
		else
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get device status");
		}
	}
	else // Use cached info
	{
		g_pPlutoLogger->Write(LV_STATUS, "Using cached info");
		size_t i;
		string sResult, sState;
		bool bComma = false;
		
		for (i = 0; i < m_vectDriveStatus.size(); i++)
		{
			sState = m_vectDriveStatus[i].first == 0 ? "Empty" : ("Full-" + StringUtils::itos(m_vectDriveStatus[i].first));
			sResult = "D" + StringUtils::itos(i) + "=" + sState;
			if (sJukebox_Status && sResult != "")
			{
				if (bComma)
					* sJukebox_Status += ",";
				else
					bComma = true;
				* sJukebox_Status += sResult;
			}
		}

		for (i = 0; i < m_vectSlotStatus.size(); i++)
		{
			sState = m_vectSlotStatus[i] ? "Full" : "Empty";
			sResult = "S" + StringUtils::itos(i+1) + "=" + sState;
			if (sJukebox_Status && sResult != "")
			{
				if (bComma)
					* sJukebox_Status += ",";
				else
					bComma = true;
				* sJukebox_Status += sResult;
			}
		}

		g_pPlutoLogger->Write(LV_STATUS, "Finished getting device status");
		bResult = true;
	}

	return bResult;
}

//<-dceag-getconfig-b->
bool Powerfile_C200::GetConfig()
{
	if( !Powerfile_C200_Command::GetConfig() )
		return false;
//<-dceag-getconfig-e->
//m_sIPAddress = ;  // todo
    m_pDatabase_pluto_media = new Database_pluto_media( );
	// TODO: the connection data is stored in pluto.conf; use it
    if (!m_pDatabase_pluto_media->Connect("dcerouter", "root", "", "pluto_media", 3306))
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Cannot connect to database!");
        m_bQuit = true;
        return false;
    }
	m_pMediaAttributes_LowLevel = new MediaAttributes_LowLevel(m_pDatabase_pluto_media, 0);

	string sOutput;
#ifdef EMULATE_PF
	char * args[] = {"/bin/cat", "/tmp/samples/lsscsi", NULL};
#else
	char * args[] = {"/usr/bin/lsscsi", "-g", NULL};
#endif
	if (! ProcessUtils::GetCommandOutput(args[0], args, sOutput))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get device names");
		return false;
	}

#ifdef WIN32
	size_t size;
	char *pBuffer = FileUtils::ReadFileIntoBuffer("/temp/lsscsi",size);
	sOutput=pBuffer;
#endif

	vector<string> vect_sOutput_Rows;
	Tokenize(sOutput, "\n", vect_sOutput_Rows);
	int nDrive = 0;
	for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
	{
		vector<string> vsFF; // FF = Filtered Fields
		ExtractFields(vect_sOutput_Rows[i], vsFF);
		if (vsFF[1] == "cd/dvd" && vsFF[2] == "TOSHIBA" && vsFF[3] == "DVD-ROM" && vsFF[4] == "SD-M1212")
		{
			g_pPlutoLogger->Write(LV_WARNING, "Found DVD unit %d: %s", nDrive, vsFF[6].c_str());
			pair<string, string> devPair(vsFF[6], vsFF[7]);
			m_vectDrive.push_back(devPair);
			Disk_Drive_Functions * pDDF = new Disk_Drive_Functions(this, vsFF[6]);
			m_vectDDF.push_back(pDDF);
			nDrive++;
		}
		else if (vsFF[1] == "mediumx" && vsFF[2] == "Escient" && vsFF[3] == "Powerfile" && vsFF[4] == "C200")
		{
			g_pPlutoLogger->Write(LV_WARNING, "Found changer unit: %s", vsFF[7].c_str());
			m_sChanger = vsFF[7];
		}
	}

	if (!Get_Jukebox_Status(NULL, true))
		return false;
	g_pPlutoLogger->Write(LV_STATUS, "Finished config");

	for (size_t i = 0; i < m_vectDriveStatus.size(); i++)
	{
		if (m_vectDriveStatus[i].first != 0)
			CMD_Unload_from_Drive_into_Slot(m_vectDriveStatus[i].first, i);
	}
	
	string sDefective = DATA_Get_Defective_Units();
	vector<string> vect_sDefective;
	Tokenize(sDefective, ",", vect_sDefective);
	const int iDrive_Number_Max = m_vectDriveStatus.size() - 1;
	for (size_t i = 0; i < vect_sDefective.size(); i++)
	{
		int iDrive_Number = atoi(vect_sDefective[i].c_str());
		if (iDrive_Number < 0 || iDrive_Number > iDrive_Number_Max)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Defective unit %d out of range (0-%d)", iDrive_Number, iDrive_Number_Max);
			continue;
		}
		g_pPlutoLogger->Write(LV_WARNING, "Marking user-marked-as-defective drive %d as OFFLINE", iDrive_Number);
		m_vectDriveStatus[iDrive_Number].second = false;
	}
	
	PurgeInterceptors();
	// MessageInterceptorFn, int Device_From, int Device_To, int Device_Type, int Device_Category, int Message_Type, int Message_ID
	RegisterMsgInterceptor((MessageInterceptorFn)(&Powerfile_C200::RippingProgress), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Ripping_Progress_CONST);
	
	m_pJob = new Powerfile_Job("Powerfile job", this);

	return true;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Powerfile_C200::Register()
//<-dceag-reg-e->
{
	return Connect(PK_DeviceTemplate_get()); 
}

//<-dceag-createinst-b->!

//<-dceag-createinst-e->

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will 
	get all	commands for your children in ReceivedCommandForChild, where 
	pDeviceData_Base is the child device.  If you handle the message, you 
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Powerfile_C200::ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage)
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
void Powerfile_C200::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	g_pPlutoLogger->Write(LV_WARNING, "Unknown command: type %d, id %d", pMessage->m_dwMessage_Type, pMessage->m_dwID);
	if (pMessage->m_dwMessage_Type == MESSAGETYPE_EVENT && pMessage->m_dwID == EVENT_Ripping_Progress_CONST)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Received event Ripping_Progress as unknown command");
	}
	sCMD_Result = "UNKNOWN DEVICE";
}

//<-dceag-sample-b->!

/*

	COMMANDS TO IMPLEMENT

*/

//<-dceag-c701-b->

	/** @brief COMMAND: #701 - Load from Slot into Drive */
	/** Load disc from "Storage Element" (Slot) to "Data Transfer Element" (Drive) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc from */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc to */

void Powerfile_C200::CMD_Load_from_Slot_into_Drive(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c701-e->
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Loading disc from slot %d into drive %d", iSlot_Number, iDrive_Number);
#ifdef EMULATE_PF
	string sCmd = "/bin/true";
#else
	string sCmd = string("mtx -f ") + m_sChanger + " load " + itos(iSlot_Number) + " " + itos(iDrive_Number);
#endif

	sCMD_Result = "FAILED";
	if (m_vectDriveStatus[iDrive_Number].first > 0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Disc unit %d full", iDrive_Number);
	}
	else if (! m_vectSlotStatus[iSlot_Number - 1])
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slot %d empty", iSlot_Number);
	}
	else if (m_vectDriveStatus[iDrive_Number].first < 0 && m_vectDriveStatus[iDrive_Number].first != -iSlot_Number)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Drive asked for by slot %d is reserved by slot %d", iSlot_Number, m_vectDriveStatus[iDrive_Number].first);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
		int status = system(sCmd.c_str());
		if (WEXITSTATUS(status) == 0)
		{
#ifndef EMULATE_PF
			//sCmd = "eject -s " + m_vectDrive[iDrive_Number].first; // this suddenly stopped working
			//g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
			//system(sCmd.c_str());
			sCmd = "mtx -f " + m_vectDrive[iDrive_Number].second + " eject"; // this is a patched version of mtx
			g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
			int iRet = system(sCmd.c_str());
			if (iRet == -1)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Failed to execute mtx");
			}
			else
			{
				if (WEXITSTATUS(iRet) == 1)
				{
					g_pPlutoLogger->Write(LV_WARNING, "Disc loaded but failed to refresh state. Marking drive %d as OFFLINE", iDrive_Number);
					m_vectDriveStatus[iDrive_Number].second = false;
				}
				else
#endif
				{
					g_pPlutoLogger->Write(LV_STATUS, "Loading disc succeeded");
				}
				
				m_vectDriveStatus[iDrive_Number].first = iSlot_Number;
				m_vectSlotStatus[iSlot_Number - 1] = false;

				g_pPlutoLogger->Write(LV_STATUS, "Getting disc type");
				Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
				pDDF->m_mediaDiskStatus = DISCTYPE_NONE;
				pDDF->m_mediaInserted = false;
				pDDF->cdrom_checkdrive(pDDF->m_sDrive.c_str(), &pDDF->m_mediaDiskStatus, false);
				if (pDDF->m_mediaDiskStatus != -1)
				{
					g_pPlutoLogger->Write(LV_STATUS, "Loaded disc of type %d", pDDF->m_mediaDiskStatus);
					pDDF->m_mediaInserted = true;
					sCMD_Result = "OK";
				}
				else
				{
					g_pPlutoLogger->Write(LV_WARNING, "Can't get disc type. Since we're sure there's a disc in there (we just loaded it), assuming defective unit. Marking drive %d as OFFLINE", iDrive_Number);
					m_vectDriveStatus[iDrive_Number].second = false;
				}
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_WARNING, "Loading disc failed");
		}
	}
}

//<-dceag-c702-b->

	/** @brief COMMAND: #702 - Unload from Drive into Slot */
	/** Unload disc from "Data Transfer Element" (Drive) to "Storage Element" (Slot) */
		/** @param #151 Slot Number */
			/** "Storage Element" (Slot) to transfer disc to */
		/** @param #152 Drive Number */
			/** "Data Transfer Element" (Disc Unit) to transfer disc from */

void Powerfile_C200::CMD_Unload_from_Drive_into_Slot(int iSlot_Number,int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c702-e->
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Unloading disc from drive %d into slot %d", iDrive_Number, iSlot_Number);
#ifdef EMULATE_PF
	string sCmd = "/bin/true";
#else
	string sCmd = string("mtx -f ") + m_sChanger + " unload " + itos(iSlot_Number) + " " + itos(iDrive_Number);
#endif
	
	sCMD_Result = "FAILED";
	if (m_vectDriveStatus[iDrive_Number].first == 0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Disc unit %d empty", iDrive_Number);
	}
	else if (m_vectSlotStatus[iSlot_Number - 1])
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slot %d full", iSlot_Number);
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
		int status = system(sCmd.c_str());
		if (WEXITSTATUS(status) == 0)
		{
#ifndef EMULATE_PF
			//sCmd = "eject -s " + m_vectDrive[iDrive_Number].first; // this suddenly stopped working
			//g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
			//system(sCmd.c_str());
			sCmd = "mtx -f " + m_vectDrive[iDrive_Number].second + " eject"; // this is a patched version of mtx
			g_pPlutoLogger->Write(LV_STATUS,"Executing: %s",sCmd.c_str());
			int iRet = system(sCmd.c_str());
			if (iRet == -1)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Failed to execute mtx");
			}
			else
			{
				if (WEXITSTATUS(iRet) == 1)
				{
					g_pPlutoLogger->Write(LV_WARNING, "Disc unloaded but failed to refresh state. Marking drive %d as OFFLINE", iDrive_Number);
					m_vectDriveStatus[iDrive_Number].second = false;
				}
				else
#endif
				{
					g_pPlutoLogger->Write(LV_STATUS, "Unloading disc succeeded");
				}
				
				m_vectDriveStatus[iDrive_Number].first = -iSlot_Number;
				m_vectSlotStatus[iSlot_Number - 1] = true;
				
				Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
				pDDF->m_mediaDiskStatus = DISCTYPE_NONE;
				pDDF->m_mediaInserted = false;
				
				ReleaseDrive_NoMutex(iDrive_Number, iSlot_Number);
				
				sCMD_Result = "OK";
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Unloading disc failed");
		}
	}
}

//<-dceag-c703-b->

	/** @brief COMMAND: #703 - Get Jukebox Status */
	/** Get jukebox slot and unit usage */
		/** @param #21 Force */
			/** If not empty, force=true. Else false. */
		/** @param #153 Jukebox Status */
			/** Jukebox Status in the following format: <Unit>=<State>[,<Unit>=<State>...]
where:
<Unit>=D0,D1 for the disc units, S1..200 for the disc storage slots
<State>=Empty or Full; occupied disc units may return Full-<number> where <number> is the source slot */

void Powerfile_C200::CMD_Get_Jukebox_Status(string sForce,string *sJukebox_Status,string &sCMD_Result,Message *pMessage)
//<-dceag-c703-e->
{
	bool bForce = sForce.size() != 0;
	
	g_pPlutoLogger->Write(LV_STATUS, "Getting jukebox status");

	sCMD_Result = "FAILED";
	if (Get_Jukebox_Status(sJukebox_Status, bForce))
	{
		sCMD_Result = "OK";
	}

}
//<-dceag-c45-b->

	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

void Powerfile_C200::CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage)
//<-dceag-c45-e->
{
}

//<-dceag-c46-b->

	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

void Powerfile_C200::CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage)
//<-dceag-c46-e->
{
}

//<-dceag-c47-b->

	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

void Powerfile_C200::CMD_Reset_Disk_Drive(int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c47-e->
{
	Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
	if (pDDF)
	{
		pDDF->m_mediaInserted = false;
		pDDF->m_mediaDiskStatus = DISCTYPE_NONE;
		pDDF->DisplayMessageOnOrbVFD("Checking disc...");

		pDDF->internal_reset_drive(true);
	}
}

//<-dceag-c48-b->

	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

void Powerfile_C200::CMD_Eject_Disk(int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c48-e->
{
	Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
	if (pDDF)
	{
		// put disc back in origin slot
	}
}

//<-dceag-c49-b->

	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

void Powerfile_C200::CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c49-e->
{
}

//<-dceag-c50-b->

	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

void Powerfile_C200::CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c50-e->
{
}

//<-dceag-c51-b->

	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

void Powerfile_C200::CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c51-e->
{
}

//<-dceag-c52-b->

	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

void Powerfile_C200::CMD_Start_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c52-e->
{
}

//<-dceag-c53-b->

	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

void Powerfile_C200::CMD_Abort_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c53-e->
{
}

//<-dceag-c54-b->

	/** @brief COMMAND: #54 - Mount Disk Image */
	/** Will mount a disk image as a disk. */
		/** @param #13 Filename */
			/** What to mount. If it get's the Device name it will mount the actual disk in the drive. */
		/** @param #59 MediaURL */
			/** The URL which can be used to play the mounted media. */

void Powerfile_C200::CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c54-e->
{
}

//<-dceag-c55-b->

	/** @brief COMMAND: #55 - Abort Ripping */
	/** Aborts ripping a DVD. */

void Powerfile_C200::CMD_Abort_Ripping(string &sCMD_Result,Message *pMessage)
//<-dceag-c55-e->
{
}

//<-dceag-c56-b->

	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

void Powerfile_C200::CMD_Format_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c56-e->
{
}

//<-dceag-c57-b->

	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

void Powerfile_C200::CMD_Close_Tray(string &sCMD_Result,Message *pMessage)
//<-dceag-c57-e->
{
}

//<-dceag-c337-b->

	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #20 Format */
			/** wav, flac, ogg, etc. */
		/** @param #50 Name */
			/** The target disk name, or for cd's, a comma-delimited list of names for each track. */
		/** @param #121 Tracks */
			/** For CD's, this must be a comma-delimted list of tracks (1 based) to rip. */
		/** @param #131 EK_Disc */
			/** The ID of the disc to rip */
		/** @param #152 Drive Number */
			/** Disc unit index number
Disk_Drive: 0
Powerfile: 0, 1, ... */

void Powerfile_C200::CMD_Rip_Disk(int iPK_Users,string sFormat,string sName,string sTracks,int iEK_Disc,int iDrive_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c337-e->
{
	Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
	if (pDDF)
	{
		pDDF->CMD_Rip_Disk(iPK_Users, sFormat, sName, sTracks, iEK_Disc, iDrive_Number, sCMD_Result, pMessage);
	}
}
//<-dceag-c720-b->

	/** @brief COMMAND: #720 - Bulk Rip */
	/** Mass ripping */
		/** @param #13 Filename */
			/** A path where to put the files.  By default it will be /home/[public | user_xxx]/data/[music|movies].

If this parameter starts with a "/" it is considered absolute.  Otherwise it is considered a sub-directory under the aforementioned default */
		/** @param #17 PK_Users */
			/** 0 to rip as public, or the user id to rip as private */
		/** @param #20 Format */
			/** For cd's only, flac,mp3,wav,ogg */
		/** @param #157 Disks */
			/** Comma delimited list of slot number. For all, use "*". */

void Powerfile_C200::CMD_Bulk_Rip(string sFilename,int iPK_Users,string sFormat,string sDisks,string &sCMD_Result,Message *pMessage)
//<-dceag-c720-e->
{
	size_t i;
	g_pPlutoLogger->Write(LV_STATUS, "CMD_Bulk_Rip; sDisks='%s'", sDisks.c_str());

	// to replace this with "append if not empty but don't start anything; make new list if empty and start it"
	if (m_pJob->PendingTasks())
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not in IDLE state (%d tasks pending). Can't start mass rip", m_pJob->PendingTasks());
		sCMD_Result = "FAILED";
		return;
	}
	
	m_pJob->Reset();

	vector<string> vect_sSlots;
	if (sDisks == "*")
	{
		for (size_t i = 0; i < m_vectSlotStatus.size(); i++)
		{
			if (m_vectSlotStatus[i])
				vect_sSlots.push_back(StringUtils::itos(i + 1));
		}
	}
	else
		StringUtils::Tokenize(sDisks, ",", vect_sSlots);

	for (i = 0; i < vect_sSlots.size(); i++)
	{
		string sSlot = vect_sSlots[i];
		int iSlot = atoi(sSlot.c_str());
		PowerfileRip_Task * pTask = new PowerfileRip_Task(iPK_Users,sFormat,sFilename,"Rip Slot " + vect_sSlots[i], 0, m_pJob);
		pTask->m_iSlot = iSlot;
		string::size_type pos=0;
		StringUtils::Tokenize(sSlot, "|", pos);
		while(pos<sSlot.size())
		{
			int Track = atoi(StringUtils::Tokenize(sSlot, "|", pos).c_str());
			if( Track )
				pTask->m_mapTracks[Track]="";
		}
		m_pJob->AddTask(pTask);
	}

	if (!m_pJob->PendingTasks())
	{
		g_pPlutoLogger->Write(LV_WARNING, "Received empty track list");
		sCMD_Result = "FAILED";
		return;
	}

	for (size_t i = 0; i < m_vectDrive.size(); i++)
	{
		if (!m_vectDriveStatus[i].second)
			continue; // unit offline; skip
		Task * pTask = m_pJob->GetNextTask();
		if (pTask)
			pTask->Execute();
		else
			break;
	}
	sCMD_Result = "OK";
}
//<-dceag-c738-b->

	/** @brief COMMAND: #738 - Play Disk */
	/** Play the disk from specified slot. Automatically do the behind logic. Eject a disk from drive if necessary. */
		/** @param #151 Slot Number */
			/** Slot number */

void Powerfile_C200::CMD_Play_Disk(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c738-e->
{
	g_pPlutoLogger->Write(LV_STATUS, "Unimplemented: CMD_Play_Disk; iSlot_Number='%d'", iSlot_Number);
}
//<-dceag-c739-b->

	/** @brief COMMAND: #739 - Get Bulk Ripping Status */
	/** Get status/progress of bulk ripping operation */
		/** @param #158 Bulk rip status */
			/** Returns:
S1-F,S2-S,S3-R,S4-N
F = failed
S = succeeded
R = ripping
N = not processed yet
empty string when idle or reset; last status when all disks were ripped
only slots that were scheduled for ripping will appear in the string */

void Powerfile_C200::CMD_Get_Bulk_Ripping_Status(string *sBulk_rip_status,string &sCMD_Result,Message *pMessage)
//<-dceag-c739-e->
{
#ifdef EMULATE_PF
	* sBulk_rip_status = "S1-F~S2-S~S3-R~S4-N";
#else
	* sBulk_rip_status = m_pJob->ToString();
#endif
	sCMD_Result = "OK";
}
//<-dceag-c740-b->

	/** @brief COMMAND: #740 - Mass identify media */
	/** Scan all loaded discs and identify each one */
		/** @param #157 Disks */
			/** Comma separated list of slots to identify. "*" for all */

void Powerfile_C200::CMD_Mass_identify_media(string sDisks,string &sCMD_Result,Message *pMessage)
//<-dceag-c740-e->
{
	size_t i;
	g_pPlutoLogger->Write(LV_STATUS, "CMD_Mass_identify_media: sDisks: %s", sDisks.c_str());

	// to replace this with "append if not empty but don't start anything; make new list if empty and start it"
	if (m_pJob->PendingTasks())
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not in IDLE state (%d tasks pending). Can't start mass identify", m_pJob->PendingTasks());
		sCMD_Result = "FAILED";
		return;
	}
	m_pJob->Reset();
	
	vector<int> vect_iSlots;
	if (sDisks == "*")
	{
		for (size_t i = 0; i < m_vectSlotStatus.size(); i++)
		{
			if (m_vectSlotStatus[i])
				vect_iSlots.push_back(i + 1);
		}
	}
	else
	{
		vector<string> vect_sDisks;
		Tokenize(sDisks, ",", vect_sDisks);
		for (i = 0; i < vect_sDisks.size(); i++)
			vect_iSlots.push_back(atoi(vect_sDisks[i].c_str()));
	}

	for (i = 0; i < vect_iSlots.size(); i++)
	{
		int iSlot = vect_iSlots[i];
		PowerfileIdentify_Task * pTask = new PowerfileIdentify_Task("Identify Slot " + StringUtils::itos(iSlot), 0, m_pJob);
		pTask->m_iSlot = iSlot;
		m_pJob->AddTask(pTask);
	}

	if (!m_pJob->PendingTasks())
	{
		g_pPlutoLogger->Write(LV_WARNING, "Received empty track list");
		sCMD_Result = "FAILED";
		return;
	}

	for (size_t i = 0; i < m_vectDrive.size(); i++)
	{
		if (!m_vectDriveStatus[i].second)
			continue; // unit offline; skip
		Task * pTask = m_pJob->GetNextTask();
		if (pTask)
			pTask->Execute();
		else
			break;
	}
}

int Powerfile_C200::GetFreeDrive(int iSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	size_t i;

	for (i = 0; i < m_vectDriveStatus.size(); i++)
	{
		if (m_vectDriveStatus[i].second && (m_vectDriveStatus[i].first == 0 || abs(m_vectDriveStatus[i].first) == iSlot))
		{
			g_pPlutoLogger->Write(LV_STATUS, "Free drive %d to slot %d", i, iSlot);
			m_vectDriveStatus[i].first = -iSlot;
			return i;
		}
	}
	g_pPlutoLogger->Write(LV_STATUS, "No free drives for slot %d", iSlot);

	return -1;
}

void Powerfile_C200::ReleaseDrive(int iDrive_Number, int iSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	ReleaseDrive_NoMutex(iDrive_Number, iSlot);
}

void Powerfile_C200::ReleaseDrive_NoMutex(int iDrive_Number, int iSlot)
{
	if (abs(m_vectDriveStatus[iDrive_Number].first) != iSlot)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slot %d tried to release drive %d which is in use by slot %d. Expect the application to mis-behave after this.",
			iSlot, iDrive_Number, abs(m_vectDriveStatus[iDrive_Number].first));
		return;
	}
	m_vectDriveStatus[iDrive_Number].first = 0;

	if (m_pJob)
	{
		Task * pTask = m_pJob->GetNextTask();
		if (pTask)
			pTask->Execute();
	}
}

string Powerfile_Job::ToString()
{
	string sResult;
	bool bComma = false;
	
	for(list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if (bComma)
			sResult += "~";
		else
			bComma = true;
		sResult += pTask->ToString();
	}
	return sResult;
}

string PowerfileRip_Task::ToString()
{
	static const char * textStatus[] = { "N", "R", "F", "S", "C" };
	string sResult = "S" + StringUtils::itos(m_iSlot) + "-" + textStatus[m_eTaskStatus] + " " + m_sResult;
	return sResult;
}

string PowerfileIdentify_Task::ToString()
{
	static const char * textStatus[] = { "N", "I", "F", "S", "C" };
	string sResult = "S" + StringUtils::itos(m_iSlot) + "-" + textStatus[m_eTaskStatus] + " " + m_sResult;
	return sResult;
}

void PowerfileRip_Task::Run()
{
	if (m_eTaskStatus == TASK_NOT_STARTED)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Task posponed. Exiting Run()");
		return; // task postponed
	}
	
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;

	listMediaAttribute listMediaAttribute_;
	int PK_Disc = pPowerfile_Job->m_pPowerfile_C200->m_pMediaAttributes_LowLevel->IsDiscAlreadyIdentified(pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot,listMediaAttribute_);
	Row_Disc *pRow_Disc;
	if( !PK_Disc || (pRow_Disc=pPowerfile_Job->m_pPowerfile_C200->m_pDatabase_pluto_media->Disc_get()->GetRow(PK_Disc))==NULL )
	{
		m_ePreTaskStatus=TASK_FAILED;
		m_sResult="Disc has not been identified";
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot rip disk for device %d slot %d because it has not been identified",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot);
		return;
	}

	if( pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST )
		RipCD(pRow_Disc,listMediaAttribute_);
	else if( pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_DVD_CONST )
		RipDVD(pRow_Disc,listMediaAttribute_);
	else
	{
		m_ePreTaskStatus=TASK_FAILED;
		m_sResult="Disc type is unknown";
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot rip disk for device %d slot %d because the type %d is unknown",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot,pRow_Disc->EK_MediaType_get());
	}
}

void PowerfileRip_Task::RipDVD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_)
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	string sTitle;

	for (listMediaAttribute::iterator it = listMediaAttribute_.begin(); it != listMediaAttribute_.end(); it++)
	{
		MediaAttribute * pMediaAttribute = * it;
		if (pMediaAttribute->m_Title_Track == 0 && pMediaAttribute->m_PK_AttributeType == ATTRIBUTETYPE_Title_CONST)
			sTitle = pMediaAttribute->m_sName;
	}

	if (! sTitle.size())
	{
		m_ePreTaskStatus = TASK_FAILED;
		m_sResult = "DVD Title is not known";
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot rip disk for device %d slot %d because the title is unknown",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device, m_iSlot);
		return;
	}
	m_sPath = sTitle;

	string sPath;
	if (m_iPK_Users)
		sPath = "/home/user_" + StringUtils::itos(m_iPK_Users) + "/data/videos";
	else
		sPath = "/home/public/data/videos";
	if (m_sPath.size())
	{
		if (m_sPath[0] == '/')
			sPath = m_sPath;
		else
			sPath += "/" + m_sPath;
	}
	string sCMD_Result = "OK";

	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;
	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	m_pDDF->CMD_Rip_Disk(m_iPK_Users, m_sFormat, sPath, "", pRow_Disc->PK_Disc_get(), m_iDrive_Number, sCMD_Result, NULL);

	if (sCMD_Result == "OK")
	{
		g_pPlutoLogger->Write(LV_STATUS, "Disc is ripping");
		while (! m_bStop)
		{
			Sleep(100);
		}
		if (m_ePreTaskStatus == TASK_CANCELED)
		{
			DCE::CMD_Kill_Application CMD_Kill_Application(iPK_Device, m_pDDF->m_pDevice_AppServer->m_dwPK_Device,
				"rip_" + StringUtils::itos(iPK_Device) + "_" + StringUtils::itos(m_iDrive_Number), true);
		}
		else if (m_ePreTaskStatus == TASK_COMPLETED)
		{
			pPowerfile_Job->m_pPowerfile_C200->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(pRow_Disc->PK_Disc_get(),
				MEDIATYPE_pluto_CD_CONST, sPath, "");
		}
	}
	else
	{
		m_bStop = true;
		m_ePreTaskStatus = TASK_FAILED;
		m_sResult = "Ripping reported " + sCMD_Result;
		g_pPlutoLogger->Write(LV_STATUS, "Disc failed to rip %s", sCMD_Result.c_str());
	}
	g_pPlutoLogger->Write(LV_STATUS, "Drive %d ripping task finished with status %d", m_iDrive_Number, m_ePreTaskStatus);
}

void PowerfileRip_Task::RipCD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_)
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;

	string sPerformer,sAlbum;
	bool bAllTracks = m_mapTracks.size()==0;
	for(listMediaAttribute::iterator it=listMediaAttribute_.begin();it!=listMediaAttribute_.end();++it)
	{
		MediaAttribute *pMediaAttribute = *it;
		if( pMediaAttribute->m_Title_Track==0 )
		{
			if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Performer_CONST )
				sPerformer = pMediaAttribute->m_sName;
			else if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Album_CONST )
				sAlbum = pMediaAttribute->m_sName;
		}
		else if( bAllTracks || m_mapTracks.find(pMediaAttribute->m_Title_Track)!=m_mapTracks.end() )
		{
			if( pMediaAttribute->m_PK_AttributeType==ATTRIBUTETYPE_Song_CONST )
				m_mapTracks[pMediaAttribute->m_Title_Track] = pMediaAttribute->m_sName;
		}
	}

	if( sPerformer.size()==0 || sAlbum.size()==0 )
	{
		m_ePreTaskStatus=TASK_FAILED;
		m_sResult="CD Performer and Album are not known";
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot rip disk for device %d slot %d because the performer/album are unknown",
			pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device,m_iSlot);
		return;
	}

	DeviceData_Base * pDevice_MediaPlugin = pPowerfile_Job->m_pPowerfile_C200->m_pData->m_AllDevices.m_mapDeviceData_Base_FindFirstOfCategory(DEVICECATEGORY_Media_Plugins_CONST);
	m_sPath = sPerformer + " - " + sAlbum;

	string sTracks;
	for(map<int,string>::iterator it=m_mapTracks.begin();it!=m_mapTracks.end();++it)
	{
		string sSong = it->second;
		if( sSong.size()==0 )
			sSong = "Track " + StringUtils::itos(it->first);
		sTracks += StringUtils::itos(it->first) + "," + sSong + "|";
	}

	string sPath;
	if( m_iPK_Users )
		sPath = "/home/user_" + StringUtils::itos(m_iPK_Users) + "/data/music";
	else
		sPath = "/home/public/data/music";
	if( m_sPath.size() )
	{
		if( m_sPath[0]=='/' )
			sPath = m_sPath;
		else
			sPath += "/" + m_sPath;
	}
	string sCMD_Result="OK";

	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;
	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	m_pDDF->CMD_Rip_Disk(m_iPK_Users, m_sFormat, sPath, sTracks, pRow_Disc->PK_Disc_get(), m_iDrive_Number, sCMD_Result, NULL);

	if( sCMD_Result=="OK" )
	{
		g_pPlutoLogger->Write(LV_STATUS,"Disc is ripping");
		while (! m_bStop)
		{
			Sleep(100);
		}
		if (m_ePreTaskStatus == TASK_CANCELED)
		{
			DCE::CMD_Kill_Application CMD_Kill_Application(iPK_Device, m_pDDF->m_pDevice_AppServer->m_dwPK_Device,
				"rip_" + StringUtils::itos(iPK_Device) + "_" + StringUtils::itos(m_iDrive_Number), true);
		}
		else if (m_ePreTaskStatus == TASK_COMPLETED)
		{
			pPowerfile_Job->m_pPowerfile_C200->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(pRow_Disc->PK_Disc_get(),
				MEDIATYPE_pluto_CD_CONST, sPath, sTracks);
		}
	}
	else
	{
		m_bStop=true;
		m_ePreTaskStatus=TASK_FAILED;
		m_sResult="Ripping reported " + sCMD_Result;
		g_pPlutoLogger->Write(LV_STATUS,"Disc failed to rip %s",sCMD_Result.c_str());
	}
	g_pPlutoLogger->Write(LV_STATUS, "Drive %d ripping task finished with status %d", m_iDrive_Number, m_ePreTaskStatus);
}

void PowerfileIdentify_Task::Run()
{
	if (m_eTaskStatus == TASK_NOT_STARTED)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Task posponed. Exiting Run()");
		return; // task postponed
	}
	
	g_pPlutoLogger->Write(LV_STATUS,"PowerfileIdentify_Task::Run");
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	
	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;

	DCE::CMD_Identify_Media_Cat CMD_Identify_Media_Cat(iPK_Device, DEVICECATEGORY_Media_Identifiers_CONST,
		false, BL_SameComputer, iPK_Device, StringUtils::itos(m_iSlot), m_pDDF->m_sDrive);
	pPowerfile_Job->m_pPowerfile_C200->SendCommand(CMD_Identify_Media_Cat);
	
	time_t TimeOut = time(NULL) + 60; // 60s timeout
	while (! m_bStop && time(NULL) < TimeOut)
	{
		Sleep(100);
	}
	if (m_bStop)
		m_ePreTaskStatus = TASK_COMPLETED;
	else // timeout
		m_ePreTaskStatus = TASK_FAILED;
	g_pPlutoLogger->Write(LV_STATUS, "Drive %d identifying task finished with status %d", m_iDrive_Number, m_ePreTaskStatus);
}

void Powerfile_Task::ThreadStarted()
{
	m_eTaskStatus = TASK_IN_PROGRESS;
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	m_iDrive_Number = pPowerfile_Job->m_pPowerfile_C200->GetFreeDrive(m_iSlot);
	if (m_iDrive_Number == -1)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Asked to run, but no free drive. Task postponed");
		m_eTaskStatus = TASK_NOT_STARTED;
	}
	else
	{
		string sCmdResult = "FAILED";
		pPowerfile_Job->m_pPowerfile_C200->CMD_Load_from_Slot_into_Drive(m_iSlot, m_iDrive_Number, sCmdResult, NULL);
		if (sCmdResult != "OK")
		{
			g_pPlutoLogger->Write(LV_WARNING, "Failed to load disc into drive. Task postponed");
			m_eTaskStatus = TASK_NOT_STARTED;
		}
	}
}

void Powerfile_Task::ThreadEnded()
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	if (m_iDrive_Number != -1)
		pPowerfile_Job->m_pPowerfile_C200->CMD_Unload_from_Drive_into_Slot(m_iSlot, m_iDrive_Number);
	m_eTaskStatus = m_ePreTaskStatus;
}

// returns the disc type
int Powerfile_Job::MediaIdentified(int iSlot)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Received MediaIdentified event for slot %d", iSlot);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if ((pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_ePreTaskStatus == TASK_NOT_STARTED) && pTask->m_iSlot == iSlot)
		{
			pTask->m_bStop = true;
			return pTask->m_pDDF->m_mediaDiskStatus;
		}
	}
	g_pPlutoLogger->Write(LV_CRITICAL, "Slot %d not found in task list!", iSlot);
	return -1;
}

void Powerfile_Job::RippingProgress_End(int iDrive_Number, int iResult)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Received RippingProgress message for drive %d, result %d", iDrive_Number, iResult);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if ((pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_ePreTaskStatus == TASK_NOT_STARTED) && pTask->m_iDrive_Number == iDrive_Number)
		{
			if (iResult == RIP_RESULT_SUCCESS)
			{
				pTask->m_ePreTaskStatus = TASK_COMPLETED;
			}
			else
			{
				pTask->m_sResult="Technical problem reading the disc";
				pTask->m_ePreTaskStatus = TASK_FAILED;
			}
			pTask->m_bStop = true;
			return;
		}
	}
	g_pPlutoLogger->Write(LV_CRITICAL, "Drive %d not found in task list!", iDrive_Number);
}

void Powerfile_Job::RippingProgress_Going(int iDrive_Number, int iPercent, string sName)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if ((pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_ePreTaskStatus == TASK_NOT_STARTED) && pTask->m_iDrive_Number == iDrive_Number)
		{
			pTask->m_sResult = sName + " " + StringUtils::itos(iPercent) + "%";
			return;
		}
	}
	g_pPlutoLogger->Write(LV_CRITICAL, "Drive %d not found in task list!", iDrive_Number);
}

void Powerfile_Job::Remove_PowerfileTask_Slot(int iSlot)
{
	PLUTO_SAFETY_LOCK(jm, m_JobMutex);
	g_pPlutoLogger->Write(LV_STATUS, "Canceling job from slot %d", iSlot);
	for (list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if (((pTask->m_eTaskStatus == TASK_IN_PROGRESS || pTask->m_eTaskStatus == TASK_NOT_STARTED) && pTask->m_ePreTaskStatus == TASK_NOT_STARTED)
			&& pTask->m_iSlot == iSlot)
		{
			pTask->m_sResult = "Canceled by user";
			pTask->m_ePreTaskStatus = TASK_CANCELED;
			pTask->m_bStop = true;

			if (pTask->m_eTaskStatus == TASK_NOT_STARTED)
				pTask->m_eTaskStatus = TASK_CANCELED;
			return;
		}
	}
	g_pPlutoLogger->Write(LV_CRITICAL, "Slot %d not found in task list!", iSlot);
}

//<-dceag-c742-b->

	/** @brief COMMAND: #742 - Media Identified */
	/** A disc has been ID's */
		/** @param #2 PK_Device */
			/** The disk drive */
		/** @param #5 Value To Assign */
			/** The identified data */
		/** @param #10 ID */
			/** The ID of the disc */
		/** @param #19 Data */
			/** The picture/cover art */
		/** @param #20 Format */
			/** The format of the data */
		/** @param #59 MediaURL */
			/** The URL for the disc drive */
		/** @param #193 URL */
			/** The URL for the picture */

void Powerfile_C200::CMD_Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,string sMediaURL,string sURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c742-e->
{

	g_pPlutoLogger->Write(LV_STATUS, "Media Identified. Slot '%s', Format: '%s', MRL: '%s', PK_Device: '%d', Value: '%s'",
		sID.c_str(), sFormat.c_str(), sMediaURL.c_str(), iPK_Device, sValue_To_Assign.c_str());

	listMediaAttribute listMediaAttribute_;
	int PK_Disc = 0;
	int iMediaType = MEDIATYPE_pluto_CD_CONST;
	int iDiscType = m_pJob->MediaIdentified(atoi(sID.c_str()));
	switch (iDiscType)
	{
		case DISCTYPE_CD_AUDIO:
			iMediaType = MEDIATYPE_pluto_CD_CONST;
			g_pPlutoLogger->Write(LV_STATUS, "Disc type: Audio CD");
			break;
		case DISCTYPE_DVD_VIDEO:
			iMediaType = MEDIATYPE_pluto_DVD_CONST;
			g_pPlutoLogger->Write(LV_STATUS, "Disc type: DVD");
			break;
		default:
			g_pPlutoLogger->Write(LV_STATUS, "Unsupported/wrong disc type: %d. Defaulting to Audio CD", iDiscType);
	}

	if( sFormat=="CDDB-TAB" )
		PK_Disc = m_pMediaAttributes_LowLevel->Parse_CDDB_Media_ID(iMediaType, listMediaAttribute_, sValue_To_Assign);
	if( sFormat=="MISC-TAB" )
		PK_Disc = m_pMediaAttributes_LowLevel->Parse_Misc_Media_ID(iMediaType, listMediaAttribute_, sValue_To_Assign);

	if( PK_Disc )
	{
		vector<Row_Disc *> vectRow_Disc;
		m_pDatabase_pluto_media->Disc_get()->GetRows("EK_Device=" + StringUtils::itos(m_dwPK_Device) + " AND Slot=" + sID,&vectRow_Disc);
		for(size_t s=0;s<vectRow_Disc.size();++s)
		{
			vectRow_Disc[s]->EK_Device_set(0);
			vectRow_Disc[s]->Slot_set(0);
		}

		Row_Disc *pRow_Disc = m_pDatabase_pluto_media->Disc_get()->GetRow(PK_Disc);
		if( pRow_Disc )
		{
			pRow_Disc->EK_Device_set(m_dwPK_Device);
			pRow_Disc->Slot_set(atoi(sID.c_str()));
		}
		m_pDatabase_pluto_media->Disc_get()->Commit();
		m_pMediaAttributes_LowLevel->AddPictureToDisc(PK_Disc,pData,iData_Size,sURL);
	}
	m_pMediaAttributes_LowLevel->PurgeListMediaAttribute(listMediaAttribute_);
}
//<-dceag-c743-b->

	/** @brief COMMAND: #743 - Cancel Pending Task */
	/** Cancels a pending task */
		/** @param #151 Slot Number */
			/** Slot to be canceled */

void Powerfile_C200::CMD_Cancel_Pending_Task(int iSlot_Number,string &sCMD_Result,Message *pMessage)
//<-dceag-c743-e->
{
	m_pJob->Remove_PowerfileTask_Slot(iSlot_Number);
	sCMD_Result = "OK";
}
