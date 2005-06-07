#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD/BDCommandProcessor.h"
#include "BD/BDCommandProcessor_Windows_Socket.h"

#include "Socket.h"

using namespace DCE;

BDCommandProcessor_Windows_Socket::BDCommandProcessor_Windows_Socket(string sMacAddressPhone,Socket *pSocket)
: BDCommandProcessor(sMacAddressPhone)
	
{
	m_pSocket=pSocket;
}


bool BDCommandProcessor_Windows_Socket::SendData(int size,const char *data)
{
	return m_pSocket->SendData(size,data);
}

char *BDCommandProcessor_Windows_Socket::ReceiveData(int size)
{
	char *Data = new char[size];
	if( !m_pSocket->ReceiveData(size,Data) )
	{
		delete[] Data;
		return NULL;
	}
	return Data;
}

