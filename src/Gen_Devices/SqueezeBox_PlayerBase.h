#ifndef SqueezeBox_PlayerBase_h
#define SqueezeBox_PlayerBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class SqueezeBox_Player_Event : public Event_Impl
{
public:
	SqueezeBox_Player_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID,58, ServerAddress, bConnectEventHandler) {};
	SqueezeBox_Player_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );
};


//   OUR DATA CLASS 

class SqueezeBox_Player_Data : public DeviceData_Impl
{
public:
	virtual ~SqueezeBox_Player_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 58; } ;
	virtual const char *GetDeviceDescription() { return "SqueezeBox_Player"; } ;
	int Get_PK_FloorplanObjectType() { return atoi(m_mapParameters[11].c_str());}
};



//   OUR COMMAND CLASS 

class SqueezeBox_Player_Command : public Command_Impl
{
public:
	SqueezeBox_Player_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new SqueezeBox_Player_Event(DeviceID, ServerAddress);
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new SqueezeBox_Player_Data();
		if( Size )
			m_pData->SerializeRead(Size,pConfig);
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, 58,ServerAddress);
	};
	SqueezeBox_Player_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~SqueezeBox_Player_Command() {};
	SqueezeBox_Player_Event *GetEvents() { return (SqueezeBox_Player_Event *) m_pEvent; };
	SqueezeBox_Player_Data *GetData() { return (SqueezeBox_Player_Data *) m_pData; };
	const char *GetClassName() { return "SqueezeBox_Player_Command"; };
	virtual int PK_DeviceTemplate_get() { return 58; };
	static int PK_DeviceTemplate_get_static() { return 58; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	int DATA_Get_PK_FloorplanObjectType() { return GetData()->Get_PK_FloorplanObjectType(); }
	//Event accessors
	//Commands - Override these to handle commands from the server
	virtual void CMD_Play_Media(string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Stop_Media(int iStreamID,int *iMediaPosition,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Pause_Media(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Restart_Media(int iStreamID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Change_Playback_Speed(int iStreamID,int iMediaPlaybackSpeed,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Skip_Fwd_ChannelTrack_Greater(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Skip_Back_ChannelTrack_Lower(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Enable_Broadcasting(int iStreamID,string *sMediaURL,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Report_Playback_Position(int iStreamID,string *sOptions,int *iMediaPosition,int *iMedia_Length,string &sCMD_Result,class Message *pMessage) {};

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
				case 37:
					{
						string sCMD_Result="OK";
					string sFilename=pMessage->m_mapParameters[13];
					int iPK_MediaType=atoi(pMessage->m_mapParameters[29].c_str());
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					int iMediaPosition=atoi(pMessage->m_mapParameters[42].c_str());
						CMD_Play_Media(sFilename.c_str(),iPK_MediaType,iStreamID,iMediaPosition,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
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
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					int iMediaPosition=atoi(pMessage->m_mapParameters[42].c_str());
						CMD_Stop_Media(iStreamID,&iMediaPosition,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[42]=StringUtils::itos(iMediaPosition);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
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
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
						CMD_Pause_Media(iStreamID,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
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
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
						CMD_Restart_Media(iStreamID,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
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
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					int iMediaPlaybackSpeed=atoi(pMessage->m_mapParameters[43].c_str());
						CMD_Change_Playback_Speed(iStreamID,iMediaPlaybackSpeed,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 63:
					{
						string sCMD_Result="OK";
						CMD_Skip_Fwd_ChannelTrack_Greater(sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 64:
					{
						string sCMD_Result="OK";
						CMD_Skip_Back_ChannelTrack_Lower(sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 243:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sMediaURL=pMessage->m_mapParameters[59];
						CMD_Enable_Broadcasting(iStreamID,&sMediaURL,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[59]=sMediaURL;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 259:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sOptions=pMessage->m_mapParameters[39];
					int iMediaPosition=atoi(pMessage->m_mapParameters[42].c_str());
					int iMedia_Length=atoi(pMessage->m_mapParameters[106].c_str());
						CMD_Report_Playback_Position(iStreamID,&sOptions,&iMediaPosition,&iMedia_Length,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[39]=sOptions;
						pMessageOut->m_mapParameters[42]=StringUtils::itos(iMediaPosition);
						pMessageOut->m_mapParameters[106]=StringUtils::itos(iMedia_Length);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
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
				if( pDeviceData_Base && pDeviceData_Base->IsChildOf(m_pData) )
					ReceivedCommandForChild(pDeviceData_Base,sCMD_Result,pMessage);
				else
					ReceivedUnknownCommand(sCMD_Result,pMessage);
					if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
					{
						Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[0]=sCMD_Result;
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
