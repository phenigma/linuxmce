#include "PowerfileJukebox.h"

namespace nsJukeBox
{
	/*virtual*/ void PowerfileJukebox::Init()
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::MoveFromSlotToDrive(Slot *pSlot,Drive *pDrive)
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::MoveFromDriveToSlot(Slot *pSlot,Drive *pDrive)
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Eject(Slot *pSlot)
	{
	}

	/*virtual*/ JukeBox::JukeBoxReturnCode PowerfileJukebox::Load(Slot *pSlot/*=NULL*/)
	{
	}
}
