#ifndef AsteriskBase_h
#define AsteriskBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Asterisk_Event : public Event_Impl
{
public:
	Asterisk_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	Asterisk_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
	virtual void PBX_CommandResult(int iCommandID,int iResult,string sMessage)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 14,3,19,StringUtils::itos(iCommandID).c_str(),20,StringUtils::itos(iResult).c_str(),21,sMessage.c_str()));
	}

	virtual void PBX_Ring(string sPhoneExtension,string sPhoneCallID,string sPhoneCallerID)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 15,3,22,sPhoneExtension.c_str(),23,sPhoneCallID.c_str(),24,sPhoneCallerID.c_str()));
	}

};


//   OUR DATA CLASS 

class Asterisk_Data : public DeviceData_Impl
{
public:
	virtual ~Asterisk_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 45; } ;
	virtual const char *GetDeviceDescription() { return "Asterisk"; } ;
};



//   OUR COMMAND CLASS 

class Asterisk_Command : public Command_Impl
{
public:
	Asterisk_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Asterisk_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Asterisk_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	Asterisk_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Asterisk_Command() {};
	Asterisk_Event *GetEvents() { return (Asterisk_Event *) m_pEvent; };
	Asterisk_Data *GetData() { return (Asterisk_Data *) m_pData; };
	const char *GetClassName() { return "Asterisk_Command"; };
	int PK_DeviceTemplate_get() { return 45; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	void EVENT_PBX_CommandResult(int iCommandID,int iResult,string sMessage) { GetEvents()->PBX_CommandResult(iCommandID,iResult,sMessage.c_str()); }
	void EVENT_PBX_Ring(string sPhoneExtension,string sPhoneCallID,string sPhoneCallerID) { GetEvents()->PBX_Ring(sPhoneExtension.c_str(),sPhoneCallID.c_str(),sPhoneCallerID.c_str()); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_PBX_Transfer(string sPhoneExtension,int iCommandID,string sPhoneCallID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_PBX_Hangup(int iCommandID,string sPhoneCallID,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual bool ReceivedMessage(class Message *pMessageOriginal)
	{
		if( Command_Impl::ReceivedMessage(pMessageOriginal) )
			return true;
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)
			{
				switch(pMessage->m_dwID)
				{
				case 233:
					{
						string sCMD_Result="OK";
					string sPhoneNumber=pMessage->m_mapParameters[75];
					string sPhoneType=pMessage->m_mapParameters[82];
					string sPhoneExtension=pMessage->m_mapParameters[83];
					string sPhoneCallerID=pMessage->m_mapParameters[84];
					int iCommandID=atoi(pMessage->m_mapParameters[85].c_str());
						CMD_PBX_Originate(sPhoneNumber.c_str(),sPhoneType.c_str(),sPhoneExtension.c_str(),sPhoneCallerID.c_str(),iCommandID,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 235:
					{
						string sCMD_Result="OK";
					string sPhoneExtension=pMessage->m_mapParameters[83];
					int iCommandID=atoi(pMessage->m_mapParameters[85].c_str());
					string sPhoneCallID=pMessage->m_mapParameters[87];
						CMD_PBX_Transfer(sPhoneExtension.c_str(),iCommandID,sPhoneCallID.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 237:
					{
						string sCMD_Result="OK";
					int iCommandID=atoi(pMessage->m_mapParameters[85].c_str());
					string sPhoneCallID=pMessage->m_mapParameters[87];
						CMD_PBX_Hangup(iCommandID,sPhoneCallID.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				}
				iHandled += Command_Impl::ReceivedMessage(pMessage);
			}
			else if( pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND )
			{
				MapCommand_Impl::iterator it = m_mapCommandImpl_Children.find(pMessage->m_dwPK_Device_To);
				if( it!=m_mapCommandImpl_Children.end() && !(*it).second->m_bGeneric )
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
					if( sCMD_Result!="UNHANDLED" )
						iHandled++;
				}
			}
		}
		return iHandled!=0;
	}
}; // end class


}
#endif
