#ifndef CRITERIAINFO_H
#define CRITERIAINFO_H

class CriteriaInfo
{
public:
	class Message *pMessage;
	
	class EventHandler *pEventHandler;
	class DeviceData_Router *pDevice;
	class Event_Impl *pEvent_Impl;
	int PK_C_Mode_Room,PK_C_Mode_House;

	CriteriaInfo(class Event_Impl *ocDeviceEvent,class Message *message,class DeviceData_Router *device,int C_Mode_Room,int C_Mode_House) :
	pEvent_Impl(ocDeviceEvent), pMessage(message), pLogger(logger), pEventHandler(NULL), pDevice(device),PK_C_Mode_Room(C_Mode_Room),PK_C_Mode_House(C_Mode_House) {};
};

#endif
