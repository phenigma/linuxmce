#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_SelectedFromList.h"

#ifdef BLUETOOTH_DONGLE
#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "../Orbiter/Orbiter.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

BD_PC_SelectedFromList::BD_PC_SelectedFromList(unsigned long uItemIndex) 
	
{
	m_uItemIndex = uItemIndex;

#ifdef SYMBIAN
	LOG("#	Sending 'SelectedFromList' command  #\n");
#endif

}

void BD_PC_SelectedFromList::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_long(m_uItemIndex);
}

void BD_PC_SelectedFromList::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
	m_uItemIndex = Read_unsigned_long();
}

bool BD_PC_SelectedFromList::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef BLUETOOTH_DONGLE
	BDCommandProcessor_BluetoothDongle *m_pProcessor = 
		(BDCommandProcessor_BluetoothDongle *)pProcessor;

	BD_Orbiter *pOrbiter = 
		m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

	if(NULL == pOrbiter)
	{
		//TODO: log this
		return false;
	}

	pOrbiter->m_pOrbiter->SelectedGrid(m_uItemIndex);
#endif

	return true;
}

