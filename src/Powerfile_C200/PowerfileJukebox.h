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
		private:
			pluto_pthread_mutex_t m_DriveMutex;

		public:
			string m_sChangerDev;
			bool m_bMtxAltres;
			DCE::Powerfile_C200 * m_pPowerfile;
			JobHandler * m_pJobHandler;
			Database_pluto_media * m_pDatabase_pluto_media;
			MediaAttributes_LowLevel * m_pMediaAttributes_LowLevel;

			bool m_bStatusCached;

			bool Get_Jukebox_Status(string * sJukebox_Status, bool bForce);

		public:
			PowerfileJukebox(class DCE::Powerfile_C200 * pPowerfile);
			virtual bool Init();

			virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode Eject(Slot *pSlot);  // Elect the disc in pSlot
			virtual JukeBoxReturnCode Load(Slot *pSlot=NULL);  // If NULL, just allow bulk inserting into slots.
	};
};

#endif /*PowerfileJukebox_H*/
