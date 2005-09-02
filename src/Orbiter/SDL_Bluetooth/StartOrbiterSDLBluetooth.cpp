/*
 StartOrbiterSDLBluetooth
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include <string>
using namespace std;

#include "StartOrbiterSDLBluetooth.h"
#include "OrbiterSDLBluetooth.h"

using namespace DCE;
//-----------------------------------------------------------------------------------------------------
OrbiterSDLBluetooth *StartOrbiterSDLBluetooth(
	class BDCommandProcessor *pBDCommandProcessor,
	int PK_Device, 
	int PK_DeviceTemplate,
	string sRouter_IP, 
	string sLocalDirectory, 
	bool bLocalMode, 
	int Width,
	int Height,
	int PK_Room,
	int PK_EntertainArea
)
{
    OrbiterSDLBluetooth * pOrbiterSDLBluetooth = NULL;

    try
    {
	    pOrbiterSDLBluetooth = new OrbiterSDLBluetooth(
				    pBDCommandProcessor,
				    PK_Device, PK_DeviceTemplate, sRouter_IP, 
				    sLocalDirectory, bLocalMode, Width, Height);

	    if (bLocalMode || (pOrbiterSDLBluetooth->GetConfig() && pOrbiterSDLBluetooth->Connect(0))) // Don't validate the device template
	    {
		    g_pPlutoLogger->Write(LV_STATUS, "Connect OK");
		    pOrbiterSDLBluetooth->Initialize(gtSDLGraphic,PK_Room,PK_EntertainArea);
		    g_pPlutoLogger->Write(LV_STATUS, "SDLGraphic initialized");
    		
		    if( !bLocalMode  )
			    pOrbiterSDLBluetooth->CreateChildren();

	    }
    }
    catch(string s)
    {
        g_pPlutoLogger->Write(LV_CRITICAL,s.c_str());
        pOrbiterSDLBluetooth = NULL;
    }
    catch(const char *s)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, s);
        pOrbiterSDLBluetooth = NULL;
    }
    catch(...)
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "Orbiter threw an unknown exception.");
        pOrbiterSDLBluetooth = NULL;
    }

	return pOrbiterSDLBluetooth;
}
//-----------------------------------------------------------------------------------------------------
