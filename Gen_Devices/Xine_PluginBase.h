#ifndef Xine_PluginBase_h
#define Xine_PluginBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Xine_Plugin_Event : public Event_Impl
{
public:
	Xine_Plugin_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	Xine_Plugin_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
	virtual void Playback_Events()
	{
		SendMessage(new Message(m_DeviceID, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 2,0));
	}

};


//   OUR DATA CLASS 

class Xine_Plugin_Data : public DeviceData_Impl
{
public:
	virtual ~Xine_Plugin_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 6; } ;
	virtual const char *GetDeviceDescription() { return "Xine_Plugin"; } ;
};



//   OUR COMMAND CLASS 

class Xine_Plugin_Command : public Command_Impl
{
public:
	Xine_Plugin_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Xine_Plugin_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Xine_Plugin_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	Xine_Plugin_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Xine_Plugin_Command() {};
	Xine_Plugin_Event *GetEvents() { return (Xine_Plugin_Event *) m_pEvent; };
	Xine_Plugin_Data *GetData() { return (Xine_Plugin_Data *) m_pData; };
	const char *GetClassName() { return "Xine_Plugin_Command"; };
	int PK_DeviceTemplate_get() { return 6; };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	void EVENT_Playback_Events() { GetEvents()->Playback_Events(); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_Create_Media(string sFilename,int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Stop_Media(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};

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
				case 36:
					{
						string sCMD_Result="OK";
					string sFilename=pMessage->m_Parameters[13];
					int iStreamID=atoi(pMessage->m_Parameters[41].c_str());
						CMD_Create_Media(sFilename.c_str(),iStreamID,sCMD_Result,pMessage);
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
				case 37:
					{
						string sCMD_Result="OK";
					string sFilename=pMessage->m_Parameters[13];
					int iPK_MediaType=atoi(pMessage->m_Parameters[29].c_str());
					int iStreamID=atoi(pMessage->m_Parameters[41].c_str());
					int iMediaPosition=atoi(pMessage->m_Parameters[42].c_str());
						CMD_Play_Media(sFilename.c_str(),iPK_MediaType,iStreamID,iMediaPosition,sCMD_Result,pMessage);
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
				case 38:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_Parameters[41].c_str());
						CMD_Stop_Media(iStreamID,sCMD_Result,pMessage);
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
				case 39:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_Parameters[41].c_str());
						CMD_Pause_Media(iStreamID,sCMD_Result,pMessage);
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
				case 40:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_Parameters[41].c_str());
						CMD_Restart_Media(iStreamID,sCMD_Result,pMessage);
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
				case 41:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_Parameters[41].c_str());
					int iMediaPlaybackSpeed=atoi(pMessage->m_Parameters[43].c_str());
						CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,sCMD_Result,pMessage);
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
				case 65:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_Parameters[5];
						CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,pMessage);
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
