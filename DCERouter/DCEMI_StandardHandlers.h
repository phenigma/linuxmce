#ifndef DCEMI_StandardHandlers_h
#define DCEMI_StandardHandlers_h

class DCEMI_StandardHandlers : public PlugIn
{
public:
	DCEMI_StandardHandlers(class Router *pRouter);
	int Register();

	bool DeviceGroups(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool InitialParmValue(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool ClosestRelative(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool Family(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool AssociatedDevices(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool Children(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool MySqlQuery(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool SetBoot(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool Reboot(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool SetReadyStatus(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool RequestPublicIP(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool RequestCommandGroupDescription(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
	bool RequestConfigTimeStamp(class Socket *pSocket,class Message *pMessage,class Device_Router *pDeviceFrom,class Device_Router *pDeviceTo);
};

#endif
