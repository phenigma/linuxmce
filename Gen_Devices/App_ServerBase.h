#ifndef App_ServerBase_h
#define App_ServerBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class App_Server_Event : public Event_Impl
{
public:
	App_Server_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	App_Server_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
};


//   OUR DATA CLASS 

class App_Server_Data : public DeviceData_Impl
{
public:
	virtual ~App_Server_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 26; } ;
	virtual const char *GetDeviceDescription() { return "App_Server"; } ;
};



//   OUR COMMAND CLASS 

class App_Server_Command : public Command_Impl
{
public:
	App_Server_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new App_Server_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new App_Server_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	App_Server_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~App_Server_Command() {};
	App_Server_Event *GetEvents() { return (App_Server_Event *) m_pEvent; };
	App_Server_Data *GetData() { return (App_Server_Data *) m_pData; };
	const char *GetClassName() { return "App_Server_Command"; };
	int PK_DeviceTemplate_get() { return 26; };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	//Commands - Override these to handle commands from the server
	virtual void CMD_Simulate_Keypress(int iPK_Button,string sName,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Spawn_Application(string sFilename,string sName,string sArguments,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Kill_Application(string sName,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Hide_Application(string sName,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual bool ReceivedMessage(class Message *pMessageOriginal)
	{
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_DeviceIDTo==m_DeviceID && pMessage->m_MessageType == MESSAGETYPE_COMMAND)
			{
				switch(pMessage->m_ID)
				{
				case 28:
					{
						string sCMD_Result="OK";
					int iPK_Button=atoi(pMessage->m_Parameters[26].c_str());
					string sName=pMessage->m_Parameters[50];
						CMD_Simulate_Keypress(iPK_Button,sName.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_DeviceID,pMessage->m_DeviceIDFrom,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 67:
					{
						string sCMD_Result="OK";
					string sFilename=pMessage->m_Parameters[13];
					string sName=pMessage->m_Parameters[50];
					string sArguments=pMessage->m_Parameters[51];
						CMD_Spawn_Application(sFilename.c_str(),sName.c_str(),sArguments.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_DeviceID,pMessage->m_DeviceIDFrom,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 69:
					{
						string sCMD_Result="OK";
					string sName=pMessage->m_Parameters[50];
						CMD_Kill_Application(sName.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_DeviceID,pMessage->m_DeviceIDFrom,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 70:
					{
						string sCMD_Result="OK";
					string sName=pMessage->m_Parameters[50];
						CMD_Hide_Application(sName.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_DeviceID,pMessage->m_DeviceIDFrom,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				}
				iHandled += Command_Impl::ReceivedMessage(pMessage);			}
		}
		return iHandled!=0;
	}
}; // end class


}
#endif
