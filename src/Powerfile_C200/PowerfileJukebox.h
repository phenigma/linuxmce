#ifndef PowerfileJukebox_H
#define PowerfileJukebox_H

#include "../Disk_Drive_Functions/JukeBox.h"

namespace nsJukeBox
{
	class PowerfileJukebox : public JukeBox
	{
		virtual void Init();

		virtual JukeBoxReturnCode MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive);
		virtual JukeBoxReturnCode MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive);
		virtual JukeBoxReturnCode Eject(Slot *pSlot);  // Elect the disc in pSlot
		virtual JukeBoxReturnCode Load(Slot *pSlot=NULL);  // If NULL, just allow bulk inserting into slots.
	};
};

#endif /*PowerfileJukebox_H*/
