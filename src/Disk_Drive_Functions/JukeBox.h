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

		Slot(int SlotNumber, Status status)
		{
			m_SlotNumber=SlotNumber;
			m_eStatus=status;
		}

		virtual ~Slot() {};
	};

	class Drive
	{
	public:
		int m_DriveNumber;
		enum Status
		{
			drive_empty,
			drive_full
		} m_eStatus;
		Disk_Drive_Functions *m_pDisk_Drive_Functions;

		Drive(int DriveNumber, Status status, Disk_Drive_Functions *pDisk_Drive_Functions)
		{
			m_DriveNumber=DriveNumber;
			m_eStatus=status;
			m_pDisk_Drive_Functions=pDisk_Drive_Functions;
		}

		virtual ~Drive() {};
	};

	typedef map<int,Slot *> map_int_Slotp;
	typedef map<int,Drive *> map_int_Drivep;

	class JukeBox
	{
	protected:
		map_int_Slotp m_mapSlot;
		map_int_Drivep m_mapDrive;

	public:
		enum JukeBoxReturnCode
		{
			jukebox_ok,
			jukebox_transport_failure,
			jukebox_communication_failure
		};

		virtual ~JukeBox()
		{
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
		}

		virtual bool Init()=0;

		virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)=0;
		virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)=0;
		virtual JukeBoxReturnCode Eject(Slot *pSlot)=0;  // Elect the disc in pSlot
		virtual JukeBoxReturnCode Load(Slot *pSlot=NULL)=0;  // If NULL, just allow bulk inserting into slots.
	};
}

#endif
