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
#include "JukeBox.h"
#include "IdentifyJob.h"

#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "pluto_media/Table_DiscLocation.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "Gen_Devices/AllScreens.h"
#include "PlutoUtils/FileUtils.h"
#include "Media_Plugin/Media_Plugin.h"

using namespace nsJobHandler;
using namespace nsJukeBox;

JukeBox::JukeBox(Command_Impl *pCommand_Impl)
	: m_DriveMutex("Jukebox drive mutex", true)
{
    pthread_mutexattr_init( &m_MutexAttr );
    pthread_mutexattr_settype( &m_MutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );
    m_DriveMutex.Init( &m_MutexAttr );

	m_pCommand_Impl = pCommand_Impl;
	m_pJobHandler = NULL;
	m_pDatabase_pluto_media = NULL;
	m_pMediaAttributes_LowLevel = NULL;
}

JukeBox::~JukeBox()
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);
	map_int_Slotp::iterator itSlot;
	map_int_Drivep::iterator itDrive;

	for (itSlot = m_mapSlot.begin(); itSlot != m_mapSlot.end(); itSlot++)
	{
		delete itSlot->second;
	}

	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		delete itDrive->second;
	}

	delete m_pJobHandler;
	delete m_pDatabase_pluto_media;
	delete m_pMediaAttributes_LowLevel;
}

bool JukeBox::Init()
{
	// The derived class should first populate the mapSlot and mapDrive

	m_pJobHandler = new JobHandler();
	m_pJobHandler->StartThread();
	m_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
	// TODO: the connection data is stored in pluto.conf; use it
	if (!m_pDatabase_pluto_media->Connect(m_pCommand_Impl->m_sIPAddress, "root", "", "pluto_media", 3306))
	{
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Cannot connect to database!");
		m_pCommand_Impl->m_bQuit_set(true);
		return false;
	}
	m_pMediaAttributes_LowLevel = new MediaAttributes_LowLevel(m_pDatabase_pluto_media, 0);
	return true;
}

void JukeBox::UpdateDrivesSlotsFromDatabase()
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);

	vector<Row_DiscLocation *> vectRow_DiscLocation;
	m_pDatabase_pluto_media->DiscLocation_get()->GetRows("EK_Device=" + StringUtils::itos(m_pCommand_Impl->m_dwPK_Device),&vectRow_DiscLocation);
	for(vector<Row_DiscLocation *>::iterator it=vectRow_DiscLocation.begin();it!=vectRow_DiscLocation.end();++it)
	{
		Row_DiscLocation *pRow_DiscLocation = *it;
		Row_Disc *pRow_Disc = pRow_DiscLocation->FK_Disc_getrow();
		Slot *pSlot = m_mapSlot_Find( pRow_DiscLocation->Slot_get() );
		if( !pSlot || !pRow_Disc )
		{
			pRow_DiscLocation->Delete(); // Something is wrong with this
			m_pDatabase_pluto_media->DiscLocation_get()->Commit();
		}
		pSlot->m_pRow_Disc = pRow_Disc;
	}
}

Drive *JukeBox::LockAvailableDrive(Drive::Locked eLocked,Job *pJob)
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);

	map_int_Drivep::iterator itDrive;
	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		Drive *pDrive = itDrive->second;
		if( pDrive->m_eLocked==Drive::locked_available )
		{
			pDrive->m_eLocked = eLocked;
			pDrive->m_pJob = pJob;
			return pDrive;
		}
	}

	return NULL;
}

void JukeBox::MassIdentify(string sSlots)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeBox::MassIdentify: sSlots: %s", sSlots.c_str());

	if( sSlots=="*" || sSlots.empty() )
	{
		for(map_int_Drivep::iterator it=m_mapDrive.begin();it!=m_mapDrive.end();++it)
		{
			Drive *pDrive = it->second;
			if( pDrive->m_eStatus==Drive::drive_full )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,pDrive->m_pDisk_Drive_Functions,NULL);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
		for(map_int_Slotp::iterator it=m_mapSlot.begin();it!=m_mapSlot.end();++it)
		{
			IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,NULL,it->second);
			m_pJobHandler->AddJob(pIdentifyJob);
		}
	}
}
