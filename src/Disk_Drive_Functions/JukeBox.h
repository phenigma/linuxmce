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
		Disk_Drive_Function *m_pDisk_Drive_Function;

		Drive( int DriveNumber, Status status, Disk_Drive_Function *pDisk_Drive_Function; )
		{
			m_DriveNumber=DriveNumber;
			m_eStatus=status;
			m_pDisk_Drive_Function=pDisk_Drive_Function;
		}
	};

	class JukeBox
	{
	protected:
		map<int,Slot *> m_mapSlot;
		map<int,Drive *> m_mapDrive;

	public:
		enum JukeBoxReturnCode
		{
			jukebox_ok,
			jukebox_transport_failure,
			jukebox_communication_failure
		};

		JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)=0;
		JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)=0;
		JukeBoxReturnCode Eject(Slot *pSlot)=0;  // Elect the disc in pSlot
		JukeBoxReturnCode Load(Slot *pSlot=NULL)=0;  // If NULL, just allow bulk inserting into slots.
	};
}

#endif
