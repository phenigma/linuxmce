/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "RipTask.h"
#include "RipJob.h"
#include "Disk_Drive_Functions.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"

using namespace nsJobHandler;

RipTask::RipTask(RipJob *pRipJob,string sName,int iTrack)
	: Task(pRipJob,sName)
{
	m_iTrack=iTrack;
	m_bSpawnedRip=false;
}

int RipTask::Run()
{
	if( m_bSpawnedRip )
		return RunAlreadySpawned();  // We already spawned.  Another run which checks to see if the task is still active

	RipJob *pRipJob = (RipJob *) m_pJob;

	if (!pRipJob->m_pDisk_Drive_Functions->m_pDevice_AppServer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot rip -- no appserver");
		return 0;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Going to rip %s; drive number: %d", m_sName.c_str(), pRipJob->m_iDrive_Number);

	if (!pRipJob->m_pDisk_Drive_Functions->m_mediaInserted)
	{
		pRipJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("",RIP_RESULT_NO_DISC, "", m_sName, pRipJob->m_iEK_Disc);
		return 0;
	}

	if (pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_AUDIO && 
		pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_DVD_VIDEO && 
		pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_MIXED && 
		pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_VCD)
	{
		pRipJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("", RIP_RESULT_INVALID_DISC_TYPE, "", m_sName, pRipJob->m_iEK_Disc);
		return 0;
	}

	if (pRipJob->m_pDisk_Drive_Functions->isRipping())
	{
		pRipJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("", RIP_RESULT_ALREADY_RIPPING, "", m_sName, pRipJob->m_iEK_Disc);
		return 0;
	}

	string strParameters, strCommOnFailure, strCommOnSuccess;
Message *pMessage=NULL; // ?? what's this again
	int iPK_Device_ReturnPath;
	if (pMessage)
	{
		iPK_Device_ReturnPath = pMessage->m_dwPK_Device_From;
	}
	else
	{
		iPK_Device_ReturnPath = pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device; // mirror to ourselves
		//iPK_Device_ReturnPath = 0;
	}

	strParameters = StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + "\t" 
		+ StringUtils::itos(iPK_Device_ReturnPath) + "\t" 
		+ m_sName + "\t" + pRipJob->m_pDisk_Drive_Functions->m_sDrive + "\t" 
		+ StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus) + "\t" 
		+ StringUtils::itos(pRipJob->m_iPK_Users) + "\t" 
		+ pRipJob->m_sFormat + "\t" + pRipJob->m_sTracks;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Launching ripping job2 with name \"%s\" for disk with type \"%d\" parms %s", 
		m_sName.c_str(), pRipJob->m_pDisk_Drive_Functions->m_mediaDiskStatus, strParameters.c_str());

	string sResultMessage =
		StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + " " + StringUtils::itos(DEVICEID_EVENTMANAGER) + " " + StringUtils::itos(MESSAGETYPE_EVENT) + 
			" " + StringUtils::itos(EVENT_Ripping_Progress_CONST) + " " + StringUtils::itos(EVENTPARAMETER_EK_Disc_CONST) + " " + StringUtils::itos(pRipJob->m_iEK_Disc) +
			" " + StringUtils::itos(EVENTPARAMETER_Name_CONST) + " \"" + m_sName + "\" " +
			StringUtils::itos(EVENTPARAMETER_Drive_Number_CONST) + " " + StringUtils::itos(pRipJob->m_iDrive_Number) + " " +
			StringUtils::itos(EVENTPARAMETER_PK_Device_CONST) + " " + StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + " " +
			StringUtils::itos(EVENTPARAMETER_Result_CONST) + " ";

	DCE::CMD_Spawn_Application
		spawnApplication(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device,
						pRipJob->m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
						"/usr/pluto/bin/ripDiskWrapper.sh", "rip_" + StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + "_" + StringUtils::itos(pRipJob->m_iDrive_Number),
						strParameters,
						sResultMessage + StringUtils::itos(RIP_RESULT_FAILURE),
						sResultMessage + StringUtils::itos(RIP_RESULT_SUCCESS),
						false, false, false, false);

	string sResponse;
    if (! pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(spawnApplication,&sResponse) || sResponse != "OK")
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Trying to rip - App server returned %s",sResponse.c_str());
		return 0;
	}

	m_bSpawnedRip=true;
	return 1000;  // Check again in 1 second
}

int RipTask::RunAlreadySpawned()
{
	return 1000;
}

