#ifndef ESTABLISHMENTSOCKET_H
#define ESTABLISHMENTSOCKET_H

#include "ClientSocket.h"
using namespace DCE;

class EstablishmentSocket : public ClientSocket
{
public:
	EstablishmentSocket(long DeviceID, string IPAddress,string Name) :
		ClientSocket(DeviceID,IPAddress,Name) {};
	bool OnConnect(string ExtraInfo);
};

#endif
