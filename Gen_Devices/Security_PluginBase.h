#ifndef Security_PluginBase_h
#define Security_PluginBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Security_Plugin_Event : public Event_Impl
{
public:
	Security_Plugin_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	Security_Plugin_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
};


//   OUR DATA CLASS 

class Security_Plugin_Data : public DeviceData_Impl
{
public:
	virtual ~Security_Plugin_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 33; } ;
	virtual const char *GetDeviceDescription() { return "Security_Plugin"; } ;
};



//   OUR COMMAND CLASS 

class Security_Plugin_Command : public Command_Impl
{
public:
	Security_Plugin_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Security_Plugin_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Security_Plugin_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	Security_Plugin_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Security_Plugin_Command() {};
	Security_Plugin_Event *GetEvents() { return (Security_Plugin_Event *) m_pEvent; };
	Security_Plugin_Data *GetData() { return (Security_Plugin_Data *) m_pData; };
	const char *GetClassName() { return "Security_Plugin_Command"; };
	int PK_DeviceTemplate_get() { return 33; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
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
			 if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND )
			{
				MapCommand_Impl::iterator it = m_mapCommandImpl_Children.find(pMessage->m_dwPK_Device_To);
				if( it!=m_mapCommandImpl_Children.end() )
				{
					Command_Impl *pCommand_Impl = (*it).second;
					iHandled += pCommand_Impl->ReceivedMessage(pMessage);
			}
			else
			{
				DeviceData_Base *pDeviceData_Base = m_pData->m_AllDevices.m_mapDeviceData_Base_Find(pMessage->m_dwPK_Device_To);
				string sCMD_Result="UNHANDLED";
				if( pDeviceData_Base->IsChildOf(m_pData) )
					ReceivedCommandForChild(pDeviceData_Base,sCMD_Result,pMessage);
				else
					ReceivedUnknownCommand(sCMD_Result,pMessage);
					if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
					{
						Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						SendMessage(pMessageOut);
					}
					else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
						SendString(sCMD_Result);
				}
			}
		}
		return iHandled!=0;
	}
}; // end class


}
#endif
