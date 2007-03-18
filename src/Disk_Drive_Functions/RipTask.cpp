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

	string sResultMessage;
	/*=
		StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + " " + StringUtils::itos(DEVICEID_EVENTMANAGER) + " " + StringUtils::itos(MESSAGETYPE_EVENT) + 
			" " + StringUtils::itos(EVENT_Ripping_Progress_CONST) + " " + StringUtils::itos(EVENTPARAMETER_EK_Disc_CONST) + " " + StringUtils::itos(pRipJob->m_iEK_Disc) +
			" " + StringUtils::itos(EVENTPARAMETER_Name_CONST) + " \"" + m_sName + "\" " +
			StringUtils::itos(EVENTPARAMETER_Drive_Number_CONST) + " " + StringUtils::itos(pRipJob->m_iDrive_Number) + " " +
			StringUtils::itos(EVENTPARAMETER_PK_Device_CONST) + " " + StringUtils::itos(pRipJob->m_pDisk_Drive_Functions->m_pCommand_Impl->m_dwPK_Device) + " " +
			StringUtils::itos(EVENTPARAMETER_Result_CONST) + " ";
*/
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


/*
	class RippingJob
	{
	public:


		MediaDevice *m_pMediaDevice_Disk_Drive;
		int m_iPK_Orbiter,m_iPK_User,m_iPK_Disc,m_iPK_MediaType;
		string m_sName,m_sTracks,m_sStatus,m_sPercentage;
		bool m_bAborted;

		RippingJob(MediaDevice *pMediaDevice_Disk_Drive,int iPK_Orbiter,int iPK_Disc,int iPK_MediaType,int iPK_User,string sName,string sTracks)
		{ m_pMediaDevice_Disk_Drive=pMediaDevice_Disk_Drive; m_iPK_Orbiter=iPK_Orbiter; m_iPK_Disc=iPK_Disc; m_iPK_MediaType=iPK_MediaType; m_iPK_User=iPK_User; m_sName=sName; m_sTracks=sTracks; m_bAborted=false;}
	};
	*/

/*

// This should be in sync with the typedef in the Disk_Drive.h
typedef enum {
	RIP_RESULT_BEGIN_ENUM		 = 0x01,
	RIP_RESULT_ALREADY_RIPPING,
	RIP_RESULT_NO_DISC,
	RIP_RESULT_INVALID_DISC_TYPE,
	RIP_RESULT_SUCCESS,
	RIP_RESULT_FAILURE,
	RIP_RESULT_STILLGOING,
	RIP_RESULT_END_ENUM
} rippingResult;

bool Media_Plugin::RippingAborted( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	RippingJob *pRippingJob = m_mapRippingJobs_Find(pMessage->m_dwPK_Device_To);
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"Aborted ripping of job %p %s",pRippingJob,pRippingJob ? pRippingJob->m_sName.c_str() : "");
	if( pRippingJob )
		pRippingJob->m_bAborted=true;
	return false;
}

bool Media_Plugin::RippingProgress( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo )
{
	string      sJobName = pMessage->m_mapParameters[EVENTPARAMETER_Name_CONST];
	int		iResult  = atoi( pMessage->m_mapParameters[EVENTPARAMETER_Result_CONST].c_str( ) );
	int		iPK_Disc = atoi( pMessage->m_mapParameters[EVENTPARAMETER_EK_Disc_CONST].c_str( ) );

	// See Disk_Drive.h for the defines
	if ( iResult <= RIP_RESULT_BEGIN_ENUM || iResult >= RIP_RESULT_END_ENUM )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Invalid result parameters. Ignoring event.");
		return false;
	}

	PLUTO_SAFETY_LOCK( mm, m_MediaMutex );

	RippingJob *pRippingJob = m_mapRippingJobs_Find(pMessage->m_dwPK_Device_From);
	if ( !pRippingJob )
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Unrecognized ripping job: %s. Ignoring event.", sJobName.c_str());
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got a ripping completed event for job named \"%s\" from device: \"%d\" job %p aborted %d iResult: %d", 
		sJobName.c_str(), pMessage->m_dwPK_Device_From,pRippingJob,(int) pRippingJob->m_bAborted,iResult);

	string sMessage;
	switch ( iResult )
	{
		case RIP_RESULT_ALREADY_RIPPING: 	sMessage = "There is already a ripping job in this entertainment area!"; 	break;
		case RIP_RESULT_NO_DISC:			sMessage = "There is no disk in the Media Director which is controlling this entertainment area!";	break;
		case RIP_RESULT_INVALID_DISC_TYPE:	sMessage = "Can't rip the disk that is in the unit at this moment (unknown format)!";	break;
		case RIP_RESULT_FAILURE:			
			if( pRippingJob->m_bAborted )	sMessage = "Ripping canceled";
			else							sMessage = "While ripping the disk, pluto encountered a disk read problem. Please check if the disc is not scratched."; break;
		case RIP_RESULT_SUCCESS:			sMessage = "The disk was ripped succesfully.";	break;
		case RIP_RESULT_BEGIN_ENUM:
		case RIP_RESULT_END_ENUM:
			break;
	}

	if( pRippingJob->m_bAborted )
	{
		if( FileUtils::DirExists(pRippingJob->m_sName) )
		{
			string::size_type pos=0;
			while(pos<pRippingJob->m_sTracks.size())
			{
				string sTrack = StringUtils::Tokenize( pRippingJob->m_sTracks, "|", pos );
				string::size_type pos_name = sTrack.find(",");
				if( pos_name!=string::npos )
				{
					FileUtils::DelFile(pRippingJob->m_sName + "/" + sTrack.substr(pos_name+1) + "*");
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::RippingProgress deleting %s", (pRippingJob->m_sName + "/" + sTrack.substr(pos_name+1) + "*").c_str() );
				}
			}
			list<string> listFiles;
			FileUtils::FindFiles(listFiles,pRippingJob->m_sName,"*",true,false,1);
			FileUtils::FindDirectories(listFiles,pRippingJob->m_sName,false);
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"It's a directory %s with %d files",pRippingJob->m_sName.c_str(),(int) listFiles.size());
			if( listFiles.size()==0 )  // There's nothing else in that directory.  Delete it
			{
				FileUtils::DelDir(pRippingJob->m_sName);
				string sParent = FileUtils::BasePath(pRippingJob->m_sName);
				FileUtils::FindFiles(listFiles,sParent,"*",true,false,1);
				FileUtils::FindDirectories(listFiles,sParent,false);
			}
		}
		else
		{
			FileUtils::DelFile(pRippingJob->m_sName + ".*");  // Delete any temporary or in progress
LoggerWrapper::GetInstance()->Write(LV_STATUS,"Media_Plugin::RippingProgress deleting %s", (pRippingJob->m_sName + ".*").c_str() );
		}
	}

	if( iResult==RIP_RESULT_STILLGOING )
	{
		pRippingJob->m_sStatus = FileUtils::FilenameWithoutPath(sJobName);
		pRippingJob->m_sPercentage = pMessage->m_mapParameters[EVENTPARAMETER_Value_CONST] + "%";

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

		return false;
	}
	else if( iResult==RIP_RESULT_SUCCESS )
		m_pMediaAttributes->m_pMediaAttributes_LowLevel->AddRippedDiscToDatabase(pRippingJob->m_iPK_Disc,
			pRippingJob->m_iPK_MediaType,pRippingJob->m_sName,pRippingJob->m_sTracks);
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"Ripping wasn't successful--not adding it to the database");

	if( pRippingJob->m_iPK_Orbiter )
	{
		SCREEN_PopupMessage SCREEN_PopupMessage(m_dwPK_Device, pRippingJob->m_iPK_Orbiter,
			sMessage, // Main message
			"", // Command Line
			"ripping_failed", // Description
			"0", // sPromptToResetRouter
			"0", // sTimeout
			"1"); // sCannotGoBack
		SendCommand(SCREEN_PopupMessage);
	}

	for( MapEntertainArea::iterator it=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.begin( );it!=pRippingJob->m_pMediaDevice_Disk_Drive->m_mapEntertainArea.end( );++it )
	{
		EntertainArea *pEntertainArea = ( *it ).second;
		for(ListMediaDevice::iterator itVFD=pEntertainArea->m_listVFD_LCD_Displays.begin();itVFD!=pEntertainArea->m_listVFD_LCD_Displays.end();++itVFD)
		{
			MediaDevice *pMediaDevice = *itVFD;
			DCE::CMD_Display_Message CMD_Display_Message(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				sMessage,
				StringUtils::itos(VL_MSGTYPE_RUNTIME_NOTICES),"tc","","");

			DCE::CMD_Display_Message CMD_Display_Message_Name(m_dwPK_Device,pMediaDevice->m_pDeviceData_Router->m_dwPK_Device,
				"",
				StringUtils::itos(VL_MSGTYPE_RIPPING_NAME),"tc","","");
			CMD_Display_Message_Name.m_pMessage->m_vectExtraMessages.push_back(CMD_Display_Message.m_pMessage);
			SendCommand(CMD_Display_Message);
		}
	}

	delete pRippingJob;
	m_mapRippingJobs.erase(pMessage->m_dwPK_Device_From);
	return false;
}
*/