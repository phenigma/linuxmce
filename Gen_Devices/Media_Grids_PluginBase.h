#ifndef Media_Grids_PluginBase_h
#define Media_Grids_PluginBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Media_Grids_Plugin_Event : public Event_Impl
{
public:
	Media_Grids_Plugin_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	Media_Grids_Plugin_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
};


//   OUR DATA CLASS 

class Media_Grids_Plugin_Data : public DeviceData_Impl
{
public:
	virtual ~Media_Grids_Plugin_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 25; } ;
	virtual const char *GetDeviceDescription() { return "Media_Grids_Plugin"; } ;
};



//   OUR COMMAND CLASS 

class Media_Grids_Plugin_Command : public Command_Impl
{
public:
	Media_Grids_Plugin_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Media_Grids_Plugin_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Media_Grids_Plugin_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	Media_Grids_Plugin_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Media_Grids_Plugin_Command() {};
	Media_Grids_Plugin_Event *GetEvents() { return (Media_Grids_Plugin_Event *) m_pEvent; };
	Media_Grids_Plugin_Data *GetData() { return (Media_Grids_Plugin_Data *) m_pData; };
	const char *GetClassName() { return "Media_Grids_Plugin_Command"; };
	int PK_DeviceTemplate_get() { return 25; };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	//Commands - Override these to handle commands from the server

	//This distributes a received message to your handler.
	virtual bool ReceivedMessage(class Message *pMessageOriginal)
	{
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
		}
		return iHandled!=0;
	}
}; // end class


}
#endif
