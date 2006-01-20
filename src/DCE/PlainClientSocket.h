#ifndef PlainClientSocket_H
#define PlainClientSocket_H

#include "ClientSocket.h"

class PlainClientSocket : public DCE::ClientSocket
{
public:
	PlainClientSocket(string IPAddress) :
		ClientSocket(1,IPAddress,"Plain Socket") {};
	bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );
};

#endif
