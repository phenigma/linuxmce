#ifndef RAClientSocket_H
#define RAClientSocket_H

#include "ClientSocket.h"

class RAClientSocket : public DCE::ClientSocket
{
public:
	RAClientSocket(long DeviceID, string IPAddress,string Name) :
		ClientSocket(DeviceID,IPAddress,Name) {};
	bool OnConnect(string ExtraInfo);
};

#endif
