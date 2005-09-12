#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_PC_SetImageQuality.h"

#ifdef BLUETOOTH_DONGLE
#include "../Orbiter/Orbiter.h"
#include "../Orbiter/SDL_Bluetooth/OrbiterSDLBluetooth.h"
#include "BD/BDCommandProcessor.h"
#include "../Bluetooth_Dongle/Bluetooth_Dongle.h"
#include "BDCommandProcessor_BluetoothDongle.h"
#endif

BD_PC_SetImageQuality::BD_PC_SetImageQuality(unsigned long nImageQuality) 
{
    m_nImageQuality = nImageQuality;

#ifdef SYMBIAN
	LOG("#	Sending 'Set Image Quality' command  #\n");
#endif 
}

void BD_PC_SetImageQuality::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
    Write_long(m_nImageQuality);
}

void BD_PC_SetImageQuality::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
    m_nImageQuality = Read_long();
}

bool BD_PC_SetImageQuality::ProcessCommand(BDCommandProcessor *pProcessor)
{
#ifdef BLUETOOTH_DONGLE
    BDCommandProcessor_BluetoothDongle *m_pProcessor = 
        (BDCommandProcessor_BluetoothDongle *)pProcessor;

    BD_Orbiter *pOrbiter = 
        m_pProcessor->m_pBluetooth_Dongle->m_mapOrbiterSockets_Find(pProcessor->m_sMacAddressPhone);

    if(NULL == pOrbiter || NULL == pOrbiter->m_pOrbiter || pOrbiter->m_pOrbiter->m_bQuit)
    {
        g_pPlutoLogger->Write(LV_WARNING, "No keypress command will be dispatch: Orbiter was killed or is exiting.");
        return false;
    }

    ((OrbiterSDLBluetooth *)pOrbiter->m_pOrbiter)->SetImageQuality(m_nImageQuality);
#endif

    return true;
}

