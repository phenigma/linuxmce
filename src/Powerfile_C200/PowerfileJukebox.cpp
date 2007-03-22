#include "PowerfileJukebox.h"
#include "Utils.h"

#include "../PlutoUtils/FileUtils.h"
#include "../PlutoUtils/ProcessUtils.h"
#include "../PlutoUtils/StringUtils.h"
using namespace StringUtils;

#include "../DCE/DCEConfig.h"
extern DCEConfig g_DCEConfig;

#include "../pluto_media/Database_pluto_media.h"
#include "../Media_Plugin/MediaAttributes_LowLevel.h"
#include "Powerfile_C200.h"

#define MTX_CMD "/opt/mtx-pluto/sbin/mtx"

namespace nsJukeBox
{
	/*
	 * PowerfileDrive class implementation
	 */

	PowerfileDrive::PowerfileDrive(int DriveNumber, Status status, Disk_Drive_Functions *pDisk_Drive_Functions,
			const string & sSRdev, const string & sSGdev, bool bFunctional)
		: Drive(DriveNumber, status, pDisk_Drive_Functions)
	{
		m_sSRdev = sSRdev;
		m_sSGdev = sSGdev;
		m_bFunctional = bFunctional;
	}

	/*
	 * PowerfileJukebox class implementation
	 */

	PowerfileJukebox::PowerfileJukebox(Powerfile_C200 * pPowerfile)
	{
		m_pPowerfile = pPowerfile;
		m_bStatusCached = false;
	}

	bool PowerfileJukebox::Get_Jukebox_Status(string * sJukebox_Status, bool bForce)
	{
		bool bComma = false, bResult = false;
		string sOutput;
		if (sJukebox_Status)
			* sJukebox_Status = "";

		int nDrive = 0; // Drives are counted from 0
		int nSlot = 1;  // Slots are counted from 1

		if (! m_bStatusCached || bForce)
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Getting fresh status info (changer:%s) %s", m_sChangerDev.c_str(), bForce ? " (Forced)" : "");
#ifdef EMULATE_PF
			char * args[] = {"/bin/cat", "/tmp/samples/mtx-status", NULL};
#else
			char * args1[] = {MTX_CMD, "-f", (char *) m_sChangerDev.c_str(), "nobarcode", "status", NULL};
			char * args2[] = {MTX_CMD, "-f", (char *) m_sChangerDev.c_str(), "altres", "nobarcode", "status", NULL};
			char ** args = m_bMtxAltres ? args2 : args1;
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
					if (vsFF.size() == 0)
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Extract fields failed to get anything from %d: %s", i, vect_sOutput_Rows[i].c_str());
					}
					// Data Transfer Element
					else if (vsFF[0] == "Data" && vsFF.size() >= 4)
					{
						int iDiscFrom = 0;
						sWhoWhat = vsFF[3];
						Tokenize(sWhoWhat, ":", vsC); // Unit_number:State
						
						sResult = string("D") + vsC[0] + "=" + vsC[1];
						if (vsC[1] == "Full")
						{
							sResult += string("-") + vsFF[6];
							sscanf(vsFF[6].c_str(), "%d", &iDiscFrom);

							m_mapDrive[nDrive]->m_eStatus = Drive::drive_full;
							(dynamic_cast<PowerfileDrive *> (m_mapDrive[nDrive]))->m_iSlotOfProvenience = iDiscFrom;
						}
						else
						{
							m_mapDrive[nDrive]->m_eStatus = Drive::drive_empty;
							(dynamic_cast<PowerfileDrive *> (m_mapDrive[nDrive]))->m_iSlotOfProvenience = 0;
						}
						(dynamic_cast<PowerfileDrive *> (m_mapDrive[nDrive]))->m_bFunctional = true;
						m_mapDrive[nDrive]->m_DriveNumber = nDrive;
						nDrive ++;
					}
					// Storage Element
					else if (vsFF.size() == 3 && vsFF[0] == "Storage" && vsFF[1] == "Element")
					{
						sWhoWhat = vsFF[2];
						Tokenize(sWhoWhat, ":", vsC);
						
						sResult = string("S") + vsC[0] + "=" +vsC[1];
						if (vsC[1] == "Empty")
						{
							m_mapSlot[nSlot]->m_eStatus = Slot::slot_empty;
						}
						else
						{
							m_mapSlot[nSlot]->m_eStatus = Slot::slot_unknown_medium;
							// TODO: distinguish between slot_unknown_medium and slot_identified_disc
						}
						m_mapSlot[nSlot]->m_SlotNumber = nSlot;
						nSlot ++;
					}
					else
					{
						LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Don't know what to do with vsFF[0]=%s and size=%d",
							vsFF[0].c_str(),(int) vsFF.size());
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
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished getting device status");
				m_bStatusCached = true;
				bResult = true;
			}
			else
			{
				LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get device status");
			}
		}
		else // Use cached info
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Using cached info");
			string sResult, sState;
			bool bComma = false;

			map_int_Slotp::iterator itSlot;
			map_int_Drivep::iterator itDrive;
			
			for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
			{
				PowerfileDrive * pPowerfileDrive = dynamic_cast<PowerfileDrive *> (itDrive->second);
				sState = pPowerfileDrive->m_iSlotOfProvenience == 0 ? "Empty" : ("Full-" + StringUtils::itos(pPowerfileDrive->m_iSlotOfProvenience));
				sResult = "D" + StringUtils::itos(pPowerfileDrive->m_DriveNumber) + "=" + sState;
				if (sJukebox_Status && sResult != "")
				{
					if (bComma)
						* sJukebox_Status += ",";
					else
						bComma = true;
					* sJukebox_Status += sResult;
				}
			}

			for (itSlot = m_mapSlot.begin(); itSlot != m_mapSlot.end(); itSlot++)
			{
				Slot * pSlot = itSlot->second;
				sState = pSlot->m_eStatus == Slot::slot_empty ? "Empty" : "Full"; // XXX: distinguish between varieties of full?
				sResult = "S" + StringUtils::itos(pSlot->m_SlotNumber) + "=" + sState;
				if (sJukebox_Status && sResult != "")
				{
					if (bComma)
						* sJukebox_Status += ",";
					else
						bComma = true;
					* sJukebox_Status += sResult;
				}
			}

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished getting device status");
			bResult = true;
		}
		return bResult;
	}

	/*virtual*/ bool PowerfileJukebox::Init()
	{
		m_pJobHandler = new JobHandler();
		m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
		// TODO: the connection data is stored in pluto.conf; use it
		if (!m_pDatabase_pluto_media->Connect("dcerouter", "root", "", "pluto_media", 3306))
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
			m_pPowerfile->m_bQuit_set(true);
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
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed to get device names");
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
			if (vsFF[1] == "cd/dvd" &&
					(
					 0 == 1
					 || /* Powerfile C200 */      (vsFF[2] == "TOSHIBA"  && vsFF[3] == "DVD-ROM" && vsFF[4] == "SD-M1212")
					 || /* Powerfile R200 DLC */  (vsFF[2] == "MATSHITA" && vsFF[3] == "DVD-RAM" && vsFF[4] == "SW-9585S")
					 || /* Sony VAIO XL1B2 */     (vsFF[2] == "MATSHITA" && vsFF[3] == "DVD-RAM" && vsFF[4] == "SW-9584" )
					)
				)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found DVD unit %d: %s", nDrive, vsFF[6].c_str());

				Disk_Drive_Functions * pDDF = new Disk_Drive_Functions(m_pPowerfile, vsFF[6],
						m_pJobHandler, m_pDatabase_pluto_media, m_pMediaAttributes_LowLevel);
				PowerfileDrive * pPowerfileDrive= new PowerfileDrive(nDrive, Drive::drive_empty, pDDF, vsFF[6], vsFF[7]);
				m_mapDrive[nDrive] = pPowerfileDrive;

				nDrive++;
			}
			else if (vsFF[1] == "mediumx")
			{
				int iField = 0;
				if (1 == 0
						|| /* Powerfile C200 */     (vsFF[2] == "Escient"  && vsFF[3] == "Powerfile"    && vsFF[4] == "C200" && (iField = 7))
						|| /* Powerfile R200 DLC */ (vsFF[2] == "PowrFile" && vsFF[3] == "C200"                              && (iField = 6))
						|| /* Sony VAIO XL1B2 */    (vsFF[2] == "Sony"     && vsFF[3] == "VAIOChanger1"                      && (iField = 6, m_bMtxAltres = true))
					)
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Found changer unit: %s", vsFF[iField].c_str());
					m_sChangerDev = vsFF[iField];
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Found an unknown changer unit. Things won't work.");
				}
			}
		}

		if (!Get_Jukebox_Status(NULL, true))
			return false;
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Finished config");

		map_int_Drivep::iterator itDrive;
		for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
		{
			PowerfileDrive * pPowerfileDrive = dynamic_cast<PowerfileDrive *> (itDrive->second);
			if (pPowerfileDrive->m_iSlotOfProvenience != 0)
			{
				MoveFromDriveToSlot(m_mapSlot[pPowerfileDrive->m_iSlotOfProvenience], pPowerfileDrive);
			}
		}
		
		string sDefective = m_pPowerfile->DATA_Get_Defective_Units();
		vector<string> vect_sDefective;
		Tokenize(sDefective, ",", vect_sDefective);
		const int iDrive_Number_Max = m_mapDrive.size() - 1;
		for (size_t i = 0; i < vect_sDefective.size(); i++)
		{
			int iDrive_Number = atoi(vect_sDefective[i].c_str());
			if (iDrive_Number < 0 || iDrive_Number > iDrive_Number_Max)
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Defective unit %d out of range (0-%d)", iDrive_Number, iDrive_Number_Max);
				continue;
			}
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Marking user-marked-as-defective drive %d as OFFLINE", iDrive_Number);
			(dynamic_cast<PowerfileDrive *> (m_mapDrive[iDrive_Number]))->m_bFunctional = false;
		}

		return true;
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Eject(Slot *pSlot)
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Load(Slot *pSlot/*=NULL*/)
	{
	}
}
