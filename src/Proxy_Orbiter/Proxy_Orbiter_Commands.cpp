/*
 Proxy_Orbiter_Commands
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "Proxy_Orbiter_Commands.h"

#include "BD/BD_WhatDoYouHave.h"
#include "BD_PC_Disconnect.h"
#include "BD_PC_KeyWasPressed.h"
#include "BD_PC_SetImageQuality.h"

#include "BD/BD_HaveNothing.h"

#include "BD_CP_ShowImage.h"
#include "BD_CP_Disconnect.h"
#include "BD_CP_SimulateEvent.h"

class BDCommand *BuildCommandFromData( unsigned long dwType )
{
	switch( dwType )
	{
	case BD_PC_WHAT_DO_YOU_HAVE:
		return new BD_WhatDoYouHave();
	case BD_PC_DISCONNECT:
		return new BD_PC_Disconnect(); 
	case BD_PC_KEY_WAS_PRESSED:
		return new BD_PC_KeyWasPressed();
    case BD_PC_SET_IMAGE_QUALITY:
        return new BD_PC_SetImageQuality();


	case BD_CP_HAVE_NOTHING:
		return new BD_HaveNothing();
	case BD_CP_SHOW_IMAGE:
		return new BD_CP_ShowImage();
	case BD_CP_DISCONNECT:
		return new BD_CP_Disconnect();
	case BD_CP_SIMULATE_EVENT:
		return new BD_CP_SimulateEvent();

	default:
		// Log Error
		return NULL;
	};
}
