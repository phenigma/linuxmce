#include "stdafx.h"
#include "../DCE/CommonIncludes.h"
#include "../VIPShared/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "../VIPShared/PhoneDevice.h"
#include "Customer.h"

#include "Picture.h"
#include "ImageDefs.h"

BDCommandProcessor *Customer::GetCommandProcessor()
{
	if( m_pBDCommandProcessor )
		return m_pBDCommandProcessor;

	if( !m_pPhoneDevice )
		throw "Customer with no phone device";

#ifdef BT_SOCKET
	EstablishmentSocket *pSocket = new EstablishmentSocket(1,"localhost:3461","foo");
	if( !pSocket->Connect() )
	{
		MessageBox(NULL,"Could not connect to phone emulator","*ERROR*",MB_OK);
		return;
	}
	m_pBDCommandProcessor = new BDCommandProcessor_Windows_Socket("Mac",pSocket);
#else
	m_pBDCommandProcessor = new BDCommandProcessor_Windows_Bluetooth(m_pPhoneDevice->m_sMacAddress);
#endif

	return m_pBDCommandProcessor;
}
