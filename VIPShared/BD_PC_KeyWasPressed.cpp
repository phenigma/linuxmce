#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include <iostream>
#include <fstream>
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_KeyWasPressed.h"

#ifdef BLUETOOTH_DONGLE
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

#ifdef APPSERVER
#include "../level2/AppServer/MODCommandImpl.h"
#include "../level1/Controller/AppServer/ControllerAppServer.h"
#include "../pluto_dce/Table_Command.h"
#endif

BD_PC_KeyWasPressed::BD_PC_KeyWasPressed(int Key) 
	
{
	m_Key=Key;

#ifdef SYMBIAN
	LOG("#	Sending 'KeyWasPressed' command  #\n");
#endif 
}

void BD_PC_KeyWasPressed::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_Key);
}

void BD_PC_KeyWasPressed::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_Key = Read_long();
}


#ifdef BLUETOOTH_DONGLE
bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_OrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter)
	{
		//TODO: log this
		return false;
	}

	pOrbiter->m_pOrbiter->ButtonDown(m_Key);

	return true;
}

#else

bool BD_PC_KeyWasPressed::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
#endif

