#include "BD/BDCommandProcessor.h"
#include <in_sock.h>

const TInt KPort = 3450;

void BDCommandProcessor_Symbian_Socket::Start() 
{
	iSocketServ.Connect();

	User::LeaveIfError(iTransferSocket.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp));

	TInetAddr iAddress;
	
        // Set up address information
	iAddress.SetPort(KPort);
	iAddress.SetAddress(iServerAddress);
	
	TRequestStatus ilStatus;
	iTransferSocket.Connect(iAddress, ilStatus);
	User::WaitForRequest(ilStatus);
}

