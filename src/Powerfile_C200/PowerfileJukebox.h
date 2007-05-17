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

			virtual void Media_Identified(int iPK_Device,string sValue_To_Assign,string sID,char *pData,int iData_Size,string sFormat,int iPK_MediaType,string sMediaURL,string sURL,int *iEK_Disc);

			virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive);
			virtual JukeBoxReturnCode Eject(Slot *pSlot);  // Elect the disc in pSlot
			virtual JukeBoxReturnCode Eject(Drive *pDrive);  // Elect the disc in pDrive
			virtual JukeBoxReturnCode Eject(int iSlot_Number,int iDrive_Number);
			virtual JukeBoxReturnCode Load(Slot *pSlot=NULL);  // If NULL, just allow bulk inserting into slots.
	};
};

#endif /*PowerfileJukebox_H*/
