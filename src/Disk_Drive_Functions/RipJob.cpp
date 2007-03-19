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

#include "RipJob.h"
#include "RipTask.h"
#include "DCE/Logger.h"

using namespace nsJobHandler;
using namespace DCE;

RipJob::RipJob(class JobHandler *pJobHandler,
			Disk_Drive_Functions *pDisk_Drive_Functions,
			JukeBox *pJukeBox,
			int iPK_Users, int iEK_Disc, int iDrive_Number,
			int iPK_Orbiter,
			string sFormat, string sFileName, string sTracks)
	: Job(pJobHandler,"RipJob")
{
	m_pDisk_Drive_Functions=pDisk_Drive_Functions;
	m_pJukeBox=pJukeBox;
	m_iPK_Users=iPK_Users;
	m_iEK_Disc=iEK_Disc;
	m_iDrive_Number=iDrive_Number;
	m_iPK_Orbiter=iPK_Orbiter;
	m_sFormat=sFormat;
	m_sFileName=sFileName;
	m_sTracks=sTracks;
}

bool RipJob::ReadyToRun()
{
	if( m_pDisk_Drive_Functions )  // We're ready to go if we have a drive to do the ripping
	{
		AddRippingTasks();
		return true;
	}

	if( !m_pJukeBox ) // If we don't have a jukebox, something is wrong, we have nothing and can't proceed
	{
		m_eJobStatus = job_Error;
		return false;
	}

	// Try to get an available drive from the jukebox to do the ripping
//	m_pDisk_Drive_Functions = m_pJukeBox->LockAvailableDrive(this);
	if( m_pDisk_Drive_Functions==NULL )  // We don't have a drive
		return false;

// MoveSlotToDrive();
//		AddRippingTasks();
// MoveDriveToSlot(any_available);
	return true;
}

void RipJob::AddRippingTasks()
{
	AddTask(new RipTask(this,"Rip",0));
}

