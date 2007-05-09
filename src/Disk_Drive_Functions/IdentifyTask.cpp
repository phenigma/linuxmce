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

#include "IdentifyTask.h"
#include "IdentifyJob.h"
#include "JukeBox.h"
#include "Disk_Drive_Functions.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "Media_Plugin/Media_Plugin.h"

using namespace nsJobHandler;

#define	ID_TIMEOUT	45 // Maximum seconds to wait for ID

IdentifyTask::IdentifyTask(IdentifyJob *pIdentifyJob,string sName)
	: Task(pIdentifyJob,sName)
{
	m_pIdentifyJob=pIdentifyJob; // A duplicate of m_pJob, but we don't need to keep recasting
	m_tSentIdCommand=0;
}

int IdentifyTask::Run()
{
	if( m_tSentIdCommand )
	{
		if( m_tSentIdCommand + ID_TIMEOUT < time(NULL) )
		{
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "IdentifyTask::Run didn't get a valid response");
			m_eTaskStatus_set(TASK_FAILED_CONTINUE);  // Keep going and put the disc back in its slot
			return 0;
		}
		return 1000; // Keep going
	}
	if( m_pIdentifyJob && m_pIdentifyJob->m_pDisk_Drive_Functions && m_pIdentifyJob->m_pDisk_Drive_Functions->m_pDevice_MediaIdentifier_get() )
	{
		Command_Impl *pCommand_Impl=NULL;
		if( m_pIdentifyJob->m_pSlot )
			pCommand_Impl = m_pIdentifyJob->m_pSlot->m_pJukeBox->m_pCommand_Impl;
		else
			pCommand_Impl = m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl_get();

		string sResponse;
		DCE::CMD_Identify_Media CMD_Identify_Media(pCommand_Impl->m_dwPK_Device,m_pIdentifyJob->m_pDisk_Drive_Functions->m_pDevice_MediaIdentifier_get()->m_dwPK_Device,
			m_pIdentifyJob->m_pDisk_Drive_Functions->m_dwPK_Device_get(),StringUtils::itos(m_pIdentifyJob->m_pDisk_Drive_Functions->m_discid),
			m_pIdentifyJob->m_pDisk_Drive_Functions->m_sDrive,
			pCommand_Impl->m_dwPK_Device);
		if( pCommand_Impl->SendCommand(CMD_Identify_Media,&sResponse) && sResponse=="OK" )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "IdentifyTask::Run waiting for identification");
			m_eTaskStatus_set(TASK_IN_PROGRESS);
			m_tSentIdCommand=time(NULL);
			return 1000;
		}
	}

	m_eTaskStatus_set(TASK_FAILED_CONTINUE);  // Keep going and put the disc back in its slot
	return 0;


	/*
	if( m_bSpawnedRip )
		return RunAlreadySpawned();  // We already spawned.  Another run which checks to see if the task is still active
	IdentifyJob *pIdentifyJob = (IdentifyJob *) m_pJob;

	if (!pIdentifyJob->m_pDisk_Drive_Functions->m_pDevice_AppServer)
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot rip -- no appserver");
		return 0;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Going to rip %s; drive number: %d", m_sName.c_str(), pIdentifyJob->m_iDrive_Number);

	if (!pIdentifyJob->m_pDisk_Drive_Functions->m_mediaInserted)
	{
		pIdentifyJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("",RIP_RESULT_NO_DISC, "", m_sName, pIdentifyJob->m_iEK_Disc);
		return 0;
	}

	if (pIdentifyJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_AUDIO && 
		pIdentifyJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_DVD_VIDEO && 
		pIdentifyJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_MIXED && 
		pIdentifyJob->m_pDisk_Drive_Functions->m_mediaDiskStatus != DISCTYPE_CD_VCD)
	{
		pIdentifyJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("", RIP_RESULT_INVALID_DISC_TYPE, "", m_sName, pIdentifyJob->m_iEK_Disc);
		return 0;
	}

	if (pIdentifyJob->m_pDisk_Drive_Functions->isRipping())
	{
		pIdentifyJob->m_pDisk_Drive_Functions->EVENT_Ripping_Progress("", RIP_RESULT_ALREADY_RIPPING, "", m_sName, pIdentifyJob->m_iEK_Disc);
		return 0;
	}

	string strParameters;
	strParameters = StringUtils::itos(pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + "\t" 
		+ StringUtils::itos(m_pJob->m_iID_get()) + "\t" 
		+ StringUtils::itos(m_iID_get()) + "\t"
		+ ((IdentifyJob *) m_pJob)->m_sFileName + "\t" + pIdentifyJob->m_pDisk_Drive_Functions->m_sDrive + "\t" 
		+ StringUtils::itos(pIdentifyJob->m_pDisk_Drive_Functions->m_mediaDiskStatus) + "\t" 
		+ StringUtils::itos(pIdentifyJob->m_iPK_Users) + "\t" 
		+ pIdentifyJob->m_sFormat + "\t" + pIdentifyJob->m_sTracks;

	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Launching ripping job2 with name \"%s\" for disk with type \"%d\" parms %s", 
		m_sName.c_str(), pIdentifyJob->m_pDisk_Drive_Functions->m_mediaDiskStatus, strParameters.c_str());

	string sResultMessage =
		StringUtils::itos(pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + " " + StringUtils::itos(pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) +
			" " + StringUtils::itos(MESSAGETYPE_COMMAND) +
			" " + StringUtils::itos(COMMAND_Update_Ripping_Status_CONST) + " " + StringUtils::itos(COMMANDPARAMETER_Task_CONST) + " " + StringUtils::itos(m_iID_get()) +
			" " + StringUtils::itos(COMMANDPARAMETER_Job_CONST) + " " + StringUtils::itos(m_pJob->m_iID_get()) + " " +
			StringUtils::itos(COMMANDPARAMETER_Status_CONST) + " ";

	m_sSpawnName="rip_job_" + StringUtils::itos(m_pJob->m_iID_get()) + "_task_" + StringUtils::itos(m_iID);

	DCE::CMD_Spawn_Application
		spawnApplication(pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device,
						pIdentifyJob->m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
						"/usr/pluto/bin/ripDiskWrapper.sh", 
						m_sSpawnName,
						strParameters,
						sResultMessage + "e",
						sResultMessage + "s",
						false, false, false, true);

	string sResponse;
    if (! pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(spawnApplication,&sResponse) || sResponse != "OK")
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Trying to rip - App server returned %s",sResponse.c_str());
		return 0;
	}

	m_bSpawnedRip=true;
	*/
	return 1000;  // Check again in 1 second
}

int IdentifyTask::RunAlreadySpawned()
{
	return 1000;
}

void IdentifyTask::UpdateProgress(string sStatus,int iPercent,int iTime,string sText,string sFilename)
{
	/*
	m_sText=sText;

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "IdentifyTask::UpdateProgress message %s", m_sText.c_str());

	if( sStatus=="p" )
	{
		m_iPercent=iPercent;
		m_iTime=iTime;
		ReportProgress();
	}
	else if( sStatus=="e" )
	{
		ReportFailure();
		m_eTaskStatus_set(TASK_FAILED);
	}
	else if( sStatus=="s" )
	{
		m_pIdentifyJob->m_pDisk_Drive_Functions->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(m_pIdentifyJob->m_iEK_Disc,
			m_pIdentifyJob->m_iPK_MediaType,m_pIdentifyJob->m_sFileName,m_pIdentifyJob->m_sTracks);
		ReportSuccess();
		m_eTaskStatus_set(TASK_COMPLETED);
	}
	*/
}

bool IdentifyTask::Abort()
{
	/*
	DCE::CMD_Kill_Application CMD_Kill_Application
		(m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device,
		m_pIdentifyJob->m_pDisk_Drive_Functions->m_pDevice_AppServer->m_dwPK_Device,
		m_sSpawnName,false);
    m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(CMD_Kill_Application);

	if( FileUtils::DirExists(m_pIdentifyJob->m_sName) )
	{
		string::size_type pos=0;
		while(pos<m_pIdentifyJob->m_sTracks.size())
		{
			string sTrack = StringUtils::Tokenize( m_pIdentifyJob->m_sTracks, "|", pos );
			string::size_type pos_name = sTrack.find(",");
			if( pos_name!=string::npos )
			{
				FileUtils::DelFile(m_pIdentifyJob->m_sName + "/" + sTrack.substr(pos_name+1) + "*");
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::RippingProgress deleting %s", (m_pIdentifyJob->m_sName + "/" + sTrack.substr(pos_name+1) + "*").c_str() );
			}
		}
		list<string> listFiles;
		FileUtils::FindFiles(listFiles,m_pIdentifyJob->m_sName,"*",true,false,1);
		FileUtils::FindDirectories(listFiles,m_pIdentifyJob->m_sName,false);
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"It's a directory %s with %d files",m_pIdentifyJob->m_sName.c_str(),(int) listFiles.size());
		if( listFiles.size()==0 )  // There's nothing else in that directory.  Delete it
		{
			FileUtils::DelDir(m_pIdentifyJob->m_sName);
			string sParent = FileUtils::BasePath(m_pIdentifyJob->m_sName);
			FileUtils::FindFiles(listFiles,sParent,"*",true,false,1);
			FileUtils::FindDirectories(listFiles,sParent,false);
		}
	}
	else
	{
		FileUtils::DelFile(m_pIdentifyJob->m_sName + ".*");  // Delete any temporary or in progress
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::RippingProgress deleting %s", (m_pIdentifyJob->m_sName + ".*").c_str() );
	}
*/
	return Task::Abort();
}

void IdentifyTask::ReportProgress()
{
	/*
	for( MapEntertainArea::iterator it=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.begin( );it!=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.end( );++it )
	{
		EntertainArea *pEntertainArea = ( *it ).second;
		for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
		{
			MediaDevice *pMediaDevice = *itVFD;
			DCE::CMD_Display_Message CMD_Display_Message_Name(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pRippingJob->m_sStatus,
				StringUtils::itos(VL_MSGTYPE_RIPPING_NAME),"tc","","");

			DCE::CMD_Display_Message CMD_Display_Message_SP(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				pRippingJob->m_sPercentage,
				StringUtils::itos(VL_MSGTYPE_RIPPING_PROGRESS),"tc","","");
			CMD_Display_Message_SP.m_pMessage->m_vectExtraMessages.push_back(CMD_Display_Message_Name.m_pMessage);
			SendCommand(CMD_Display_Message_SP);
		}
	}
	*/
}

void IdentifyTask::ReportFailure()
{
	/*
	SCREEN_PopupMessage SCREEN_PopupMessage(m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device, 
		m_pIdentifyJob->m_iPK_Orbiter,
		"Ripping failed", // Main message
		"", // Command Line
		"ripping_failed", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
	*/
}

void IdentifyTask::ReportSuccess()
{
	/*
	SCREEN_PopupMessage SCREEN_PopupMessage(m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device, 
		m_pIdentifyJob->m_iPK_Orbiter,
		"Ripping complete", // Main message
		"", // Command Line
		"ripping_complete", // Description
		"0", // sPromptToResetRouter
		"0", // sTimeout
		"1"); // sCannotGoBack
	m_pIdentifyJob->m_pDisk_Drive_Functions->m_pCommand_Impl->SendCommand(SCREEN_PopupMessage);
	*/
}

int IdentifyTask::SecondsRemaining()
{ 
	return 30;  // Typical ID time
}
