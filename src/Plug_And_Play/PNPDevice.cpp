#include "PNPDevice.h"

using namespace std;

PNPDevice::PNPDevice(std::string deviceID):
	state(PNP_DEVICE_UNKNOWN),
	ID(deviceID)
{

}

PNPDevice::~PNPDevice()
{
	
}

int PNPDevice::getState()
{
	return state;
}

void PNPDevice::setState(int newState)
{
	state = newState;	
}

string PNPDevice::getID()
{
	return ID;
}

void PNPDevice::setSerial(string newSerial)
{
	serial = newSerial;	
}

string PNPDevice::getSerial()
{
	return serial;
}

void PNPDevice::setPath(string newPath)
{
	path = newPath;	
}

string PNPDevice::getPath()
{
	return path;
}

void PNPDevice::setPnpProtocol(int newPnpProtocol)
{
	pnpProtocol = newPnpProtocol;	
}

int PNPDevice::getPnpProtocol()
{
	return pnpProtocol;
}
