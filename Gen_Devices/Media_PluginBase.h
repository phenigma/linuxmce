#ifndef Media_PluginBase_h
#define Media_PluginBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Media_Plugin_Event : public Event_Impl
{
public:
	Media_Plugin_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	Media_Plugin_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
};


//   OUR DATA CLASS 

class Media_Plugin_Data : public DeviceData_Impl
{
public:
	virtual ~Media_Plugin_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 2; } ;
	virtual const char *GetDeviceDescription() { return "Media_Plugin"; } ;
};



//   OUR COMMAND CLASS 

class Media_Plugin_Command : public Command_Impl
{
public:
	Media_Plugin_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Media_Plugin_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Media_Plugin_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	Media_Plugin_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Media_Plugin_Command() {};
	Media_Plugin_Event *GetEvents() { return (Media_Plugin_Event *) m_pEvent; };
	Media_Plugin_Data *GetData() { return (Media_Plugin_Data *) m_pData; };
	const char *GetClassName() { return "Media_Plugin_Command"; };
	int PK_DeviceTemplate_get() { return 2; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	//Commands - Override these to handle commands from the server
	virtual void CMD_MH_Play_Media(int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_MH_Send_Me_To_Remote(bool bNot_Full_Screen,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,string sPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual bool ReceivedMessage(class Message *pMessageOriginal)
	{
		if( pMessageOriginal->m_dwMessage_Type == MESSAGETYPE_MESSAGE_INTERCEPTED )
		{
			InterceptedMessage(pMessageOriginal);
			return true;
		}
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)
			{
				switch(pMessage->m_dwID)
				{
				case 43:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sFilename=pMessage->m_mapParameters[13];
					int iPK_MediaType=atoi(pMessage->m_mapParameters[29].c_str());
					int iPK_DeviceTemplate=atoi(pMessage->m_mapParameters[44].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_MH_Play_Media(iPK_Device,sPK_DesignObj.c_str(),sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
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
				case 44:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					int iPK_MediaType=atoi(pMessage->m_mapParameters[29].c_str());
					int iPK_DeviceTemplate=atoi(pMessage->m_mapParameters[44].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_MH_Stop_Media(iPK_Device,iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
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
				case 65:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
						CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,pMessage);
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
				case 73:
					{
						string sCMD_Result="OK";
					bool bNot_Full_Screen=(pMessage->m_mapParameters[93]=="1" ? true : false);
						CMD_MH_Send_Me_To_Remote(bNot_Full_Screen,sCMD_Result,pMessage);
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
				case 74:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_DesignObj=pMessage->m_mapParameters[3];
					string sOnOff=pMessage->m_mapParameters[8];
					string sPK_DesignObj_CurrentScreen=pMessage->m_mapParameters[16];
					int iPK_Text=atoi(pMessage->m_mapParameters[25].c_str());
					string sOptions=pMessage->m_mapParameters[39];
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					int iPK_Text_Timecode=atoi(pMessage->m_mapParameters[56].c_str());
					int iPK_Text_SectionDesc=atoi(pMessage->m_mapParameters[62].c_str());
					int iPK_Text_Synopsis=atoi(pMessage->m_mapParameters[63].c_str());
						CMD_Bind_to_Media_Remote(iPK_Device,sPK_DesignObj.c_str(),sOnOff.c_str(),sPK_DesignObj_CurrentScreen.c_str(),iPK_Text,sOptions.c_str(),sPK_EntertainArea.c_str(),iPK_Text_Timecode,iPK_Text_SectionDesc,iPK_Text_Synopsis,sCMD_Result,pMessage);
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
				case 214:
					{
						string sCMD_Result="OK";
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					string sName=pMessage->m_mapParameters[50];
					bool bSave_as_new=(pMessage->m_mapParameters[77]=="1" ? true : false);
						CMD_Save_playlist(iPK_Users,sPK_EntertainArea.c_str(),sName.c_str(),bSave_as_new,sCMD_Result,pMessage);
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
				case 231:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					int iEK_Playlist=atoi(pMessage->m_mapParameters[78].c_str());
						CMD_Load_Playlist(sPK_EntertainArea.c_str(),iEK_Playlist,sCMD_Result,pMessage);
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
				case 241:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_MH_Move_Media(iStreamID,sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
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
