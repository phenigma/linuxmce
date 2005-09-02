#ifndef __StartOrbiterSDLBluetooth_H__
#define __StartOrbiterSDLBluetooth_H__

#include "PlutoUtils/CommonIncludes.h"	
#include "DCE/Logger.h"
namespace DCE
{
	class OrbiterSDLBluetooth;
};

//-----------------------------------------------------------------------------------------------------
DCE::OrbiterSDLBluetooth *StartOrbiterSDLBluetooth(
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
);
//-----------------------------------------------------------------------------------------------------
#endif
