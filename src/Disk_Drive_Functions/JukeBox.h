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

#ifndef JukeBox_h
#define JukeBox_h

#include "Disk_Drive_Functions.h"

namespace nsJukeBox
{
	// A slot is one of the slots in the Jukebox to hold discs.  A Drive is a physical drive
	// for playing media.  Init() should populate m_mapSlot with all the slots where the key
	// is the slot number.  In other words for a 200 disc jukebox, there will be 200 entries.
	// Init() should also populate m_mapDrive for all the drives, where the key is the drive number.

	class Slot
	{
	public:
		int m_SlotNumber;
		enum Status
		{
			slot_empty,
			slot_identified_disc,
			slot_unknown_medium,
			slot_defective
		} m_eStatus;
		Row_Disc *m_pRow_Disc;
		class JukeBox *m_pJukeBox;

		Slot(int SlotNumber, Status status,JukeBox *pJukeBox)
		{
			m_SlotNumber=SlotNumber;
			m_eStatus=status;
			m_pRow_Disc=NULL;
			m_pJukeBox=pJukeBox;
		}

		virtual ~Slot() {};
	};

	class Drive
	{
	public:
		enum Locked
		{
			locked_available,
			locked_identify_job,
			locked_rip_job,
			locked_playback
		} m_eLocked; // Indicates if the drive is in use, if so for what, or available
		Job *m_pJob;  // If it's locked by a job, this is the job

		int m_iSourceSlot;  // Slot where the disk came from originally, or -1 if there is no disc or it's unknown
		int m_DriveNumber;
		bool m_bFunctional;
		string m_sSRdev; // The /dev/sgX entry
		enum Status
		{
			drive_empty,
			drive_full
		} m_eStatus;
		Disk_Drive_Functions *m_pDisk_Drive_Functions;
		Row_Disc *m_pRow_Disc;
		JukeBox *m_pJukeBox;

		Drive(int DriveNumber, Status status, Disk_Drive_Functions *pDisk_Drive_Functions, string sSRdev, JukeBox *pJukeBox, bool bFunctional)
		{
			m_DriveNumber=DriveNumber;
			m_eStatus=status;
			m_pDisk_Drive_Functions=pDisk_Drive_Functions;
			m_sSRdev=sSRdev;
			m_bFunctional=bFunctional;
			m_iSourceSlot=0;
			m_pRow_Disc=NULL;
			m_eLocked=locked_available;
			m_pJob=NULL;
			m_pJukeBox=pJukeBox;
		}

		virtual ~Drive() {};
	};

	typedef map<int,Slot *> map_int_Slotp;
	typedef map<int,Drive *> map_int_Drivep;

	class JukeBox
	{
	protected:
		pluto_pthread_mutex_t m_DriveMutex;
		pthread_mutexattr_t m_MutexAttr;

	public:
		map_int_Slotp m_mapSlot;
		map_int_Drivep m_mapDrive;
		Drive *m_mapDrive_Find( int Drive ) { map_int_Drivep::iterator it = m_mapDrive.find( Drive ); return it == m_mapDrive.end() ? NULL : (*it).second; }
		Slot *m_mapSlot_Find( int Slot ) { map_int_Slotp::iterator it = m_mapSlot.find( Slot ); return it == m_mapSlot.end() ? NULL : (*it).second; }

		string m_sChangerDev;
		JobHandler * m_pJobHandler;
		Database_pluto_media * m_pDatabase_pluto_media;
		MediaAttributes_LowLevel * m_pMediaAttributes_LowLevel;
		Command_Impl *m_pCommand_Impl;

	public:
		enum JukeBoxReturnCode
		{
			jukebox_ok,
			jukebox_transport_failure,
			jukebox_communication_failure
		};

		JukeBox(Command_Impl *pCommand_Impl);
		virtual ~JukeBox();

		virtual bool Init();
		void UpdateDrivesSlotsFromDatabase(); // Udpate the m_pRow_Disc's in slot's and drives

		// If a drive is available, it locks it and returns the drive.  Otherwise returns NULL
		Drive *LockAvailableDrive(Drive::Locked eLocked,Job *pJob);

		virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)=0;
		virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)=0;
		virtual JukeBoxReturnCode Eject(Slot *pSlot)=0;  // Elect the disc in pSlot
		virtual JukeBoxReturnCode Load(Slot *pSlot=NULL)=0;  // If NULL, just allow bulk inserting into slots.

		// High level functions
		void MassIdentify(string sSlots);
	};
}

#endif
