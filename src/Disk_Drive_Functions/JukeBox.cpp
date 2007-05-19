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
	m_eLocked=Disk_Drive_Functions::locked_available;
	m_pLockedPtr=NULL;
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
		if( pRow_DiscLocation->Type_get()=="U" )
			continue;  // It's got something in it, but we don't know what it is

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

Drive *JukeBox::LockAvailableDrive(Disk_Drive_Functions::Locked eLocked,Job *pJob,void *p_void,bool bEmptyOnly)
{
	PLUTO_SAFETY_LOCK(dl,m_DriveMutex);

	map_int_Drivep::iterator itDrive;
	for (itDrive = m_mapDrive.begin(); itDrive != m_mapDrive.end(); itDrive++)
	{
		Drive *pDrive = itDrive->second;
		if( pDrive->m_eLocked_get(NULL)==Disk_Drive_Functions::locked_available )
		{
			if( pDrive->m_mediaInserted && bEmptyOnly )
				continue;

			if( pDrive->LockDrive(eLocked,p_void) )
			{
				pDrive->m_pJob = pJob;
				return pDrive;
			}
		}
	}

	return NULL;
}

void JukeBox::MassIdentify(string sSlots,int PK_Orbiter)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "JukeBox::MassIdentify: sSlots: %s", sSlots.c_str());

	if( sSlots=="*" || sSlots.empty() )
	{
		for(map_int_Drivep::iterator it=m_mapDrive.begin();it!=m_mapDrive.end();++it)
		{
			Drive *pDrive = it->second;
			if( pDrive->m_mediaInserted )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,pDrive,NULL,PK_Orbiter,m_pCommand_Impl);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
		for(map_int_Slotp::iterator it=m_mapSlot.begin();it!=m_mapSlot.end();++it)
		{
			Slot *pSlot = it->second;
			if( pSlot->m_eStatus!=Slot::slot_defective && pSlot->m_eStatus!=Slot::slot_empty )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,NULL,pSlot,PK_Orbiter,m_pCommand_Impl);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
	}
	else
	{
		string::size_type pos=0;
		while(pos<sSlots.size())
		{
			int iSlot = atoi( StringUtils::Tokenize(sSlots,",",pos).c_str() );
			Slot *pSlot = m_mapSlot_Find(iSlot);
			if( pSlot->m_eStatus!=Slot::slot_defective && pSlot->m_eStatus!=Slot::slot_empty )
			{
				IdentifyJob *pIdentifyJob = new IdentifyJob(m_pJobHandler,NULL,pSlot,PK_Orbiter,m_pCommand_Impl);
				m_pJobHandler->AddJob(pIdentifyJob);
			}
		}
	}
}

bool JukeBox::LockJukebox(Disk_Drive_Functions::Locked locked,void *p_void)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::LockDrive m_eLocked %d locked %d %p", (int) m_eLocked, (int) locked,m_pLockedPtr);

	if( m_eLocked!=Disk_Drive_Functions::locked_available )
	{
		if( m_pLockedPtr==p_void )
		{
			LoggerWrapper::GetInstance()->Write(LV_STATUS,"LockJukebox::LockJukebox already locked");
			return true;
		}
		LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::LockJukebox cannot lock %p/%p", m_pLockedPtr, p_void);
		return false;
	}

	m_pLockedPtr=p_void;
	m_eLocked=locked;
	return true;
}

void JukeBox::UnlockJukebox()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS,"JukeBox::UnlockJukebox %p",m_pLockedPtr);
	m_eLocked=Disk_Drive_Functions::locked_available;
}

void JukeBox::AssertJukeboxIsLocked()
{
	if( m_eLocked==Disk_Drive_Functions::locked_available )
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"JukeBox::AssertJukeboxIsLocked");  // Report that something is wrong since we're using this without locking it

}

Disk_Drive_Functions::Locked JukeBox::m_eLocked_get(void **p_void)
{
	if( p_void )
		*p_void = m_pLockedPtr;

	return m_eLocked;
}

void JukeBox::UpdateDiscLocation(int PK_Device_Original,int Slot_Original,int PK_Device_New,int Slot_New)
{
	string sSQL = "UPDATE DiscLocation SET EK_Device=" + StringUtils::itos(PK_Device_New) + ",Slot=" + (Slot_New==-1 ? "NULL" : StringUtils::itos(Slot_New)) +
		" WHERE EK_Device=" + StringUtils::itos(PK_Device_Original) + (Slot_Original==-1 ? "" : " AND Slot=" + StringUtils::itos(Slot_Original));
	m_pDatabase_pluto_media->threaded_mysql_query(sSQL);
}

void JukeBox::RemoveDiscFromDb(int PK_Device,int Slot)
{
	string sSQL = "DELETE FROM DiscLocation WHERE EK_Device=" + StringUtils::itos(PK_Device);
	if( Slot!=-1 )
		sSQL += " AND Slot=" + StringUtils::itos(Slot);

	m_pDatabase_pluto_media->threaded_mysql_query(sSQL);
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"tmp:%s",sSQL.c_str());
}

void JukeBox::AddDiscToDb(int PK_Device,int Slot,char Type)
{
	string sSQL = "INSERT INTO DiscLocation(EK_Device,Slot,Type) VALUES(" + StringUtils::itos(PK_Device) + "," + (Slot ? StringUtils::itos(Slot) : "NULL") + ",'" + Type + "')";
	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"tmp:%s",sSQL.c_str());
	m_pDatabase_pluto_media->threaded_mysql_query(sSQL);
}
