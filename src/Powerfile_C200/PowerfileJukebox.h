#ifndef PowerfileJukebox_H
#define PowerfileJukebox_H

#include "../Disk_Drive_Functions/JukeBox.h"
#include "../JobHandler/JobHandler.h"

namespace DCE
{
	class Powerfile_C200;
}

namespace nsJukeBox
{
	class PowerfileJukebox : public JukeBox
	{
		public:
			bool m_bMtxAltres;
			DCE::Powerfile_C200 * m_pPowerfile;
			bool m_bStatusCached;
			bool Get_Jukebox_Status(string * sJukebox_Status, bool bForce);
			int m_TransferElement;

		public:
			PowerfileJukebox(class DCE::Powerfile_C200 * pPowerfile);
			virtual bool Init();

			virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode Eject(Slot *pSlot);  // Elect the disc in pSlot
			virtual JukeBoxReturnCode Eject(Drive *pDrive,int PK_Orbiter);  // Elect the disc in pDrive
			virtual JukeBoxReturnCode Eject(int iSlot_Number,int iPK_Device,int PK_Orbiter);
			virtual JukeBoxReturnCode Load(Slot *pSlot=NULL);  // If NULL, just allow bulk inserting into slots.
	};
};

#endif /*PowerfileJukebox_H*/
