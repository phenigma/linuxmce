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

#include "exec_grab_output.h"
#include "PlutoUtils/StringUtils.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_main/Define_DeviceData.h"

using namespace StringUtils;

namespace nsJobHandler
{
	int g_ID = 0;
}

#ifdef WIN32
	#define WEXITSTATUS(a) 0
#endif

#define PF_Constructor_Initializers , m_bStatusCached(false), m_pJob(NULL), m_DriveMutex("Powerfile drive mutex", true)

//<-dceag-const-b->
// The primary constructor when the class is created as a stand-alone device
Powerfile_C200::Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: Powerfile_C200_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
PF_Constructor_Initializers
{
}

//<-dceag-const2-b->
// The constructor when the class is created as an embedded instance within another stand-alone device
Powerfile_C200::Powerfile_C200(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
	: Powerfile_C200_Command(pPrimaryDeviceCommand, pData, pEvent, pRouter)
//<-dceag-const2-e->
PF_Constructor_Initializers
{
}

//<-dceag-dest-b->
Powerfile_C200::~Powerfile_C200()
//<-dceag-dest-e->
{
	for (size_t i = 0; i < m_vectDDF.size(); i++)
		delete m_vectDDF[i];
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

bool Powerfile_C200::MediaIdentified(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo)
{
	int iSlot = atoi(pMessage->m_mapParameters[EVENTPARAMETER_ID_CONST].c_str());
	string sName = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];
	string sFormat = pMessage->m_mapParameters[EVENTPARAMETER_Format_CONST];
	string sMRL = pMessage->m_mapParameters[EVENTPARAMETER_MRL_CONST];
	int iPK_Device = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());
	string sValue = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST];

	g_pPlutoLogger->Write(LV_STATUS, "Media Identified. Slot '%d', Name: '%s', Format: '%s', MRL: '%s', PK_Device: '%d', Value: '%s'",
		iSlot, sName, sFormat, sMRL, iPK_Device, sValue);
	m_pJob->MediaIdentified(iSlot);
	return true;
}

bool Powerfile_C200::RippingProgress(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo)
{
	int iDrive_Number = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Drive_Number_CONST].c_str());
	int iResult = atoi(pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str());
	int iPK_Device = atoi(pMessage->m_mapParameters[EVENTPARAMETER_PK_Device_CONST].c_str());

	if (iPK_Device != m_dwPK_Device)
		return false; // it's not one of ours
	
	if (iResult != RIP_RESULT_SUCCESS && iResult != RIP_RESULT_FAILURE)
		return false; // we only treat those two

	g_pPlutoLogger->Write(LV_STATUS, "Ripping progress for drive number %d, result '%s'", iDrive_Number, iResult == RIP_RESULT_SUCCESS ? "success" : "fail");
	m_pJob->RippingProgress(iDrive_Number, iResult);
	
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
		g_pPlutoLogger->Write(LV_STATUS, "Getting fresh status info%s", bForce ? " (Forced)" : "");
		m_vectDriveStatus.clear();
		m_vectSlotStatus.clear();
#ifdef EMULATE_PF
		char * args[] = {"/bin/cat", "/tmp/samples/mtx-status", NULL};
#else
		char * args[] = {"/usr/sbin/mtx", "-f", (char *) m_sChanger.c_str(), "status", NULL};
#endif
		if (exec_output(args[0], args, sOutput))
		{
			vector<string> vect_sOutput_Rows;
			
			Tokenize(sOutput, "\n", vect_sOutput_Rows);
			for (size_t i = 0; i < vect_sOutput_Rows.size(); i++)
			{
				string sResult = "";
				vector<string> vsFF;
				string sWhoWhat;
				vector<string> vsC; // C = Components

				ExtractFields(vect_sOutput_Rows[i], vsFF);
				if (vsFF[0] == "Data")
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
					m_vectDriveStatus.push_back(iDiscFrom);
				}
				else if (vsFF.size() == 3 && vsFF[0] == "Storage" && vsFF[1] == "Element")
				{
					sWhoWhat = vsFF[2];
					Tokenize(sWhoWhat, ":", vsC);
					
					sResult = string("S") + vsC[0] + "=" +vsC[1];
					m_vectSlotStatus.push_back(vsC[1] == "Full");
				}
				
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
			sState = m_vectDriveStatus[i] == 0 ? "Empty" : ("Full-" + StringUtils::itos(m_vectDriveStatus[i]));
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

	string sOutput;
#ifdef EMULATE_PF
	char * args[] = {"/bin/cat", "/tmp/samples/lsscsi", NULL};
#else
	char * args[] = {"/usr/bin/lsscsi", "-g", NULL};
#endif
	if (! exec_output(args[0], args, sOutput))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to get device names");
		return false;
	}

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
		if (m_vectDriveStatus[i] != 0)
			CMD_Unload_from_Drive_into_Slot(m_vectDriveStatus[i], i);
	}
	
	PurgeInterceptors();
	// MessageInterceptorFn, int Device_From, int Device_To, int Device_Type, int Device_Category, int Message_Type, int Message_ID
	RegisterMsgInterceptor((MessageInterceptorFn)(&Powerfile_C200::MediaIdentified), 0, 0, 0, 0, MESSAGETYPE_EVENT, EVENT_Media_Identified_CONST);
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

/*  Since several parents can share the same child class, and each has it's own implementation, the base class in Gen_Devices
	cannot include the actual implementation.  Instead there's an extern function declared, and the actual new exists here.  You 
	can safely remove this block (put a ! after the dceag-createinst-b block) if this device is not embedded within other devices. */
//<-dceag-createinst-b->
Powerfile_C200_Command *Create_Powerfile_C200(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter)
{
	return new Powerfile_C200(pPrimaryDeviceCommand, pData, pEvent, pRouter);
}
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

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

**** IF YOU DON'T WANT DCEGENERATOR TO KEEP PUTTING THIS AUTO-GENERATED SECTION ****
**** ADD AN ! AFTER THE BEGINNING OF THE AUTO-GENERATE TAG, LIKE //<=dceag-sample-b->! ****
Without the !, everything between <=dceag-sometag-b-> and <=dceag-sometag-e->
will be replaced by DCEGenerator each time it is run with the normal merge selection.
The above blocks are actually <- not <=.  We don't want a substitution here

void Powerfile_C200::SomeFunction()
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
	g_pPlutoLogger->Write(LV_STATUS, "Loading disc from slot %d into drive %d", iSlot_Number, iDrive_Number);
#ifdef EMULATE_PF
	string sCmd = "/bin/true";
#else
	string sCmd = string("mtx -f ") + m_sChanger + " load " + itos(iSlot_Number) + " " + itos(iDrive_Number);
#endif

	sCMD_Result = "FAILED";
	if (m_vectDriveStatus[iDrive_Number] > 0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Disc unit full");
	}
	else if (! m_vectSlotStatus[iSlot_Number - 1])
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slot empty");
	}
	else if (m_vectDriveStatus[iDrive_Number] < 0 && m_vectDriveStatus[iDrive_Number] != -iSlot_Number)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Drive asked for by slot %d is reserved by slot %d", iSlot_Number, m_vectDriveStatus[iDrive_Number]);
	}
	else
	{
		int status = system(sCmd.c_str());
		if (WEXITSTATUS(status) == 0)
		{
#ifndef EMULATE_PF
			sCmd = "eject -s " + m_vectDrive[iDrive_Number].first; // this suddenly stopped working
			system(sCmd.c_str());
			sCmd = "mtx -f " + m_vectDrive[iDrive_Number].second + " eject"; // this is a patched version of mtx
			system(sCmd.c_str());
#endif
			g_pPlutoLogger->Write(LV_STATUS, "Loading disc succeeded");
			
			m_vectDriveStatus[iDrive_Number] = iSlot_Number;
			m_vectSlotStatus[iSlot_Number - 1] = false;

			Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
			pDDF->m_mediaInserted = true;
			pDDF->m_mediaDiskStatus = DISCTYPE_CD_AUDIO; // move to right place (Rip::Run), replace with real thing
			
			sCMD_Result = "OK";
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
	g_pPlutoLogger->Write(LV_STATUS, "Unloading disc from drive %d into slot %d", iDrive_Number, iSlot_Number);
#ifdef EMULATE_PF
	string sCmd = "/bin/true";
#else
	string sCmd = string("mtx -f ") + m_sChanger + " unload " + itos(iSlot_Number) + " " + itos(iDrive_Number);
#endif
	
	sCMD_Result = "FAILED";
	if (m_vectDriveStatus[iDrive_Number] == 0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Disc unit empty");
	}
	else if (m_vectSlotStatus[iSlot_Number - 1])
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slot full");
	}
	else
	{
		int status = system(sCmd.c_str());
		if (WEXITSTATUS(status) == 0)
		{
#ifndef EMULATE_PF
			sCmd = "eject -s " + m_vectDrive[iDrive_Number].first; // this suddenly stopped working
			system(sCmd.c_str());
			sCmd = "mtx -f " + m_vectDrive[iDrive_Number].second + " eject"; // this is a patched version of mtx
			system(sCmd.c_str());
#endif
			g_pPlutoLogger->Write(LV_STATUS, "Unloading disc succeeded");
			
			m_vectDriveStatus[iDrive_Number] = -iSlot_Number;
			m_vectSlotStatus[iSlot_Number - 1] = true;
			
			Disk_Drive_Functions * pDDF = GetDDF(iDrive_Number);
			pDDF->m_mediaInserted = false;
			
			ReleaseDrive(iDrive_Number, iSlot_Number);
			
			sCMD_Result = "OK";
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
	/** Starts ripping a DVD. */

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
		/** @param #157 Disks */
			/** Comma delimited list of slot number. For all, use "*". */

void Powerfile_C200::CMD_Bulk_Rip(string sDisks,string &sCMD_Result,Message *pMessage)
//<-dceag-c720-e->
{
	size_t i;
	g_pPlutoLogger->Write(LV_STATUS, "CMD_Bulk_Rip; sDisks='%s'", sDisks.c_str());

	// to replace this with "append if not empty but don't start anything; make new list if empty and start it"
	if (m_pJob->PendingTasks())
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not in IDLE state. Can't start mass rip");
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
		PowerfileRip_Task * pTask = new PowerfileRip_Task("Rip Slot " + StringUtils::itos(iSlot), 0, m_pJob);
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
	* sBulk_rip_status = "S1-F,S2-S,S3-R,S4-N";
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
	g_pPlutoLogger->Write(LV_STATUS, "CMD_Mass_identify_media: sDisks: %s", sDisks.c_str());

	// to replace this with "append if not empty but don't start anything; make new list if empty and start it"
	if (m_pJob->PendingTasks())
	{
		g_pPlutoLogger->Write(LV_WARNING, "Not in IDLE state. Can't start mass identify");
		sCMD_Result = "FAILED";
		return;
	}
	m_pJob->Reset();
	
	// if( handler.ContainsJob("Identify") )
	// log error and return
	// new Job("Identify")
	// for all disks
	// job.push_backnew IdentifyTask
	// job.service()
}

int Powerfile_C200::GetFreeDrive(int iSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	size_t i;

	for (i = 0; i < m_vectDriveStatus.size(); i++)
	{
		if (m_vectDriveStatus[i] == 0 || abs(m_vectDriveStatus[i]) == iSlot)
		{
			g_pPlutoLogger->Write(LV_STATUS, "Free drive %d to slot %d", i, iSlot);
			m_vectDriveStatus[i] = -iSlot;
			return i;
		}
	}
	g_pPlutoLogger->Write(LV_STATUS, "No free drives for slot %d", iSlot);

	return -1;
}

void Powerfile_C200::ReleaseDrive(int iDrive_Number, int iSlot)
{
	PLUTO_SAFETY_LOCK(dm, m_DriveMutex);
	if (abs(m_vectDriveStatus[iDrive_Number]) != iSlot)
	{
		g_pPlutoLogger->Write(LV_WARNING, "Slot %d tried to release drive %d which is in use by slot %d. Expect the application to mis-behave after this.",
			iSlot, iDrive_Number, abs(m_vectDriveStatus[iDrive_Number]));
		return;
	}
	m_vectDriveStatus[iDrive_Number] = 0;

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
			sResult += ",";
		else
			bComma = true;
		sResult += pTask->ToString();
	}
	return sResult;
}

string PowerfileRip_Task::ToString()
{
	static const char * textStatus[] = { "N", "R", "F", "S" };
	string sResult = "S" + StringUtils::itos(m_iSlot) + "-" + textStatus[m_eTaskStatus];
	return sResult;
}

string PowerfileIdentify_Task::ToString()
{
	static const char * textStatus[] = { "N", "I", "F", "S" };
	string sResult = "S" + StringUtils::itos(m_iSlot) + "-" + textStatus[m_eTaskStatus];
	return sResult;
}

void PowerfileRip_Task::Run()
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;

//#warning "TODO: UNINITIALIZED VARIABLES"
	// TODO: get these from Media_Plugin
	int iPK_Users = 0, iEK_Disc = 0;
	string sFormat = "ogg", sName = "/tmp/tempName", sTracks = "1,tempTrack|";
//#warning "TODO: UNINITIALIZED VARIABLES"
	string sCMD_Result;
	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;

	m_pDDF = pPowerfile_Job->m_pPowerfile_C200->GetDDF(m_iDrive_Number);
	m_pDDF->CMD_Rip_Disk(iPK_Users, sFormat, sName, sTracks, iEK_Disc, m_iDrive_Number, sCMD_Result, NULL);

	while (! m_bStop)
	{
		Sleep(100);
	}
	g_pPlutoLogger->Write(LV_STATUS, "Drive %d ripping task finished with status %d", m_iDrive_Number, m_eTaskStatus);
}

void PowerfileIdentify_Task::Run()
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;

	int iPK_Device = pPowerfile_Job->m_pPowerfile_C200->m_dwPK_Device;
	DCE::CMD_Identify_Media_Cat CMD_Identify_Media_Cat(iPK_Device, DEVICECATEGORY_Media_Identifiers_CONST,
		false, BL_SameComputer, iPK_Device, StringUtils::itos(m_iSlot), "P" + StringUtils::itos(iPK_Device) + "-S" + StringUtils::itos(m_iSlot));
	
	time_t TimeOut = time(NULL) + 60; // 60s timeout
	while (! m_bStop && time(NULL) < TimeOut)
	{
		Sleep(100);
	}
	if (m_bStop)
		m_eTaskStatus = TASK_COMPLETED;
	else // timeout
		m_eTaskStatus = TASK_FAILED;
	g_pPlutoLogger->Write(LV_STATUS, "Drive %d identifying task finished with status %d", m_iDrive_Number, m_eTaskStatus);
}

void Powerfile_Task::ThreadStarted()
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	m_iDrive_Number = pPowerfile_Job->m_pPowerfile_C200->GetFreeDrive(m_iSlot);
	pPowerfile_Job->m_pPowerfile_C200->CMD_Load_from_Slot_into_Drive(m_iSlot, m_iDrive_Number);
}

void Powerfile_Task::ThreadEnded()
{
	Powerfile_Job * pPowerfile_Job = (Powerfile_Job *) m_pJob;
	pPowerfile_Job->m_pPowerfile_C200->CMD_Unload_from_Drive_into_Slot(m_iSlot, m_iDrive_Number);
}

void Powerfile_Job::MediaIdentified(int iSlot)
{
	g_pPlutoLogger->Write(LV_STATUS, "Received MediaIdentified event for slot %d", iSlot);
	for(list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if (pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_iSlot == iSlot)
		{
			pTask->m_bStop = true;
			return;
		}
	}
	g_pPlutoLogger->Write(LV_CRITICAL, "Slot %d not found in task list!", iSlot);
}

void Powerfile_Job::RippingProgress(int iDrive_Number, int iResult)
{
	g_pPlutoLogger->Write(LV_STATUS, "Received RippingProgress message for drive %d, result %d", iDrive_Number, iResult);
	for(list<Task *>::iterator it = m_listTask.begin(); it != m_listTask.end(); ++it)
	{
		Powerfile_Task * pTask = (Powerfile_Task *) * it;
		if (pTask->m_eTaskStatus == TASK_IN_PROGRESS && pTask->m_iDrive_Number == iDrive_Number)
		{
			if (iResult == RIP_RESULT_SUCCESS)
				pTask->m_eTaskStatus = TASK_COMPLETED;
			else
				pTask->m_eTaskStatus = TASK_FAILED;
			pTask->m_bStop = true;
			return;
		}
	}
	g_pPlutoLogger->Write(LV_CRITICAL, "Drive %d not found in task list!", iDrive_Number);
}
