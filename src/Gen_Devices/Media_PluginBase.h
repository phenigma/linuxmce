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
	Media_Plugin_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID,2, ServerAddress, bConnectEventHandler) {};
	Media_Plugin_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );
	virtual void Watching_Media(int iPK_Room)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 19,1,27,StringUtils::itos(iPK_Room).c_str()));
	}

	virtual void Stopped_Watching_Media(int iPK_Room)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 20,1,27,StringUtils::itos(iPK_Room).c_str()));
	}

	virtual void Listening_to_Media(int iPK_Room)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 21,1,27,StringUtils::itos(iPK_Room).c_str()));
	}

	virtual void Stopped_Listening_To_Medi(int iPK_Room)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 22,1,27,StringUtils::itos(iPK_Room).c_str()));
	}

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
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Media_Plugin_Data();
		if( Size )
			m_pData->SerializeRead(Size,pConfig);
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, 2,ServerAddress);
	};
	Media_Plugin_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Media_Plugin_Command() {};
	Media_Plugin_Event *GetEvents() { return (Media_Plugin_Event *) m_pEvent; };
	Media_Plugin_Data *GetData() { return (Media_Plugin_Data *) m_pData; };
	const char *GetClassName() { return "Media_Plugin_Command"; };
	virtual int PK_DeviceTemplate_get() { return 2; };
	static int PK_DeviceTemplate_get_static() { return 2; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	void EVENT_Watching_Media(int iPK_Room) { GetEvents()->Watching_Media(iPK_Room); }
	void EVENT_Stopped_Watching_Media(int iPK_Room) { GetEvents()->Stopped_Watching_Media(iPK_Room); }
	void EVENT_Listening_to_Media(int iPK_Room) { GetEvents()->Listening_to_Media(iPK_Room); }
	void EVENT_Stopped_Listening_To_Medi(int iPK_Room) { GetEvents()->Stopped_Listening_To_Medi(iPK_Room); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_MH_Play_Media(int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_MH_Stop_Media(int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Bind_to_Media_Remote(int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Save_playlist(int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Load_Playlist(string sPK_EntertainArea,int iEK_Playlist,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_MH_Move_Media(int iStreamID,string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Move_Playlist_entry_Up(int iValue,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Move_Playlist_entry_Down(int iValue,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Remove_playlist_entry(int iValue,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Get_EntAreas_For_Device(int iPK_Device,string *sText,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Rip_Disk(int iPK_Users,string sName,string sTracks,int iEK_Disc,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_MH_Set_Volume(string sPK_EntertainArea,string sLevel,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Media_Private(string sPK_EntertainArea,bool bTrueFalse,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Add_Media_Attribute(string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Media_Attribute_Text(string sValue_To_Assign,int iEK_Attribute,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Get_Attribute(int iEK_Attribute,string *sText,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Save_Bookmark(string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Delete_Bookmark(int iEK_Bookmark,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Rename_Bookmark(string sValue_To_Assign,int iPK_Users,int iEK_Bookmark,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Media_Position(int iStreamID,string sMediaPosition,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Auto_Resume_Options(string sValue_To_Assign,int iPK_Users,int iPK_MediaType,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Update_Time_Code(int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual bool ReceivedMessage(class Message *pMessageOriginal)
	{
		map<long, string>::iterator itRepeat;
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
				case 43:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sFilename=pMessage->m_mapParameters[13];
					int iPK_MediaType=atoi(pMessage->m_mapParameters[29].c_str());
					int iPK_DeviceTemplate=atoi(pMessage->m_mapParameters[44].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					bool bResume=(pMessage->m_mapParameters[116]=="1" ? true : false);
					int iRepeat=atoi(pMessage->m_mapParameters[117].c_str());
						CMD_MH_Play_Media(iPK_Device,sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),bResume,iRepeat,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_MH_Play_Media(iPK_Device,sFilename.c_str(),iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),bResume,iRepeat,sCMD_Result,pMessage);
						}
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
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_MH_Stop_Media(iPK_Device,iPK_MediaType,iPK_DeviceTemplate,sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 65:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
						CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,pMessage);
						}
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
					string sOptions=pMessage->m_mapParameters[39];
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					int iPK_Text_Synopsis=atoi(pMessage->m_mapParameters[63].c_str());
						CMD_Bind_to_Media_Remote(iPK_Device,sPK_DesignObj.c_str(),sOnOff.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sOptions.c_str(),sPK_EntertainArea.c_str(),iPK_Text_Synopsis,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Bind_to_Media_Remote(iPK_Device,sPK_DesignObj.c_str(),sOnOff.c_str(),sPK_DesignObj_CurrentScreen.c_str(),sOptions.c_str(),sPK_EntertainArea.c_str(),iPK_Text_Synopsis,sCMD_Result,pMessage);
						}
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
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Save_playlist(iPK_Users,sPK_EntertainArea.c_str(),sName.c_str(),bSave_as_new,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 231:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					int iEK_Playlist=atoi(pMessage->m_mapParameters[78].c_str());
						CMD_Load_Playlist(sPK_EntertainArea.c_str(),iEK_Playlist,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Load_Playlist(sPK_EntertainArea.c_str(),iEK_Playlist,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 241:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_MH_Move_Media(iStreamID,sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_MH_Move_Media(iStreamID,sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 269:
					{
						string sCMD_Result="OK";
					int iValue=atoi(pMessage->m_mapParameters[48].c_str());
						CMD_Move_Playlist_entry_Up(iValue,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Move_Playlist_entry_Up(iValue,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 270:
					{
						string sCMD_Result="OK";
					int iValue=atoi(pMessage->m_mapParameters[48].c_str());
						CMD_Move_Playlist_entry_Down(iValue,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Move_Playlist_entry_Down(iValue,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 271:
					{
						string sCMD_Result="OK";
					int iValue=atoi(pMessage->m_mapParameters[48].c_str());
						CMD_Remove_playlist_entry(iValue,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Remove_playlist_entry(iValue,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 331:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sText=pMessage->m_mapParameters[9];
						CMD_Get_EntAreas_For_Device(iPK_Device,&sText,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[9]=sText;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Get_EntAreas_For_Device(iPK_Device,&sText,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 337:
					{
						string sCMD_Result="OK";
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
					string sName=pMessage->m_mapParameters[50];
					string sTracks=pMessage->m_mapParameters[121];
					int iEK_Disc=atoi(pMessage->m_mapParameters[131].c_str());
						CMD_Rip_Disk(iPK_Users,sName.c_str(),sTracks.c_str(),iEK_Disc,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Rip_Disk(iPK_Users,sName.c_str(),sTracks.c_str(),iEK_Disc,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 372:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					string sLevel=pMessage->m_mapParameters[76];
						CMD_MH_Set_Volume(sPK_EntertainArea.c_str(),sLevel.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_MH_Set_Volume(sPK_EntertainArea.c_str(),sLevel.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 388:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					bool bTrueFalse=(pMessage->m_mapParameters[119]=="1" ? true : false);
						CMD_Set_Media_Private(sPK_EntertainArea.c_str(),bTrueFalse,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Set_Media_Private(sPK_EntertainArea.c_str(),bTrueFalse,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 391:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sTracks=pMessage->m_mapParameters[121];
					int iEK_AttributeType=atoi(pMessage->m_mapParameters[122].c_str());
						CMD_Add_Media_Attribute(sValue_To_Assign.c_str(),iStreamID,sTracks.c_str(),iEK_AttributeType,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Add_Media_Attribute(sValue_To_Assign.c_str(),iStreamID,sTracks.c_str(),iEK_AttributeType,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 392:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
					int iEK_Attribute=atoi(pMessage->m_mapParameters[123].c_str());
						CMD_Set_Media_Attribute_Text(sValue_To_Assign.c_str(),iEK_Attribute,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Set_Media_Attribute_Text(sValue_To_Assign.c_str(),iEK_Attribute,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 393:
					{
						string sCMD_Result="OK";
					int iEK_Attribute=atoi(pMessage->m_mapParameters[123].c_str());
					string sText=pMessage->m_mapParameters[9];
						CMD_Get_Attribute(iEK_Attribute,&sText,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[9]=sText;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Get_Attribute(iEK_Attribute,&sText,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 409:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_Save_Bookmark(sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Save_Bookmark(sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 410:
					{
						string sCMD_Result="OK";
					int iEK_Bookmark=atoi(pMessage->m_mapParameters[129].c_str());
						CMD_Delete_Bookmark(iEK_Bookmark,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Delete_Bookmark(iEK_Bookmark,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 411:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
					int iEK_Bookmark=atoi(pMessage->m_mapParameters[129].c_str());
						CMD_Rename_Bookmark(sValue_To_Assign.c_str(),iPK_Users,iEK_Bookmark,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Rename_Bookmark(sValue_To_Assign.c_str(),iPK_Users,iEK_Bookmark,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 412:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sMediaPosition=pMessage->m_mapParameters[42];
						CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Set_Media_Position(iStreamID,sMediaPosition.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 417:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
					int iPK_MediaType=atoi(pMessage->m_mapParameters[29].c_str());
						CMD_Set_Auto_Resume_Options(sValue_To_Assign.c_str(),iPK_Users,iPK_MediaType,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Set_Auto_Resume_Options(sValue_To_Assign.c_str(),iPK_Users,iPK_MediaType,sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case 689:
					{
						string sCMD_Result="OK";
					int iStreamID=atoi(pMessage->m_mapParameters[41].c_str());
					string sTime=pMessage->m_mapParameters[102];
					string sTotal=pMessage->m_mapParameters[132];
					string sSpeed=pMessage->m_mapParameters[133];
					string sTitle=pMessage->m_mapParameters[134];
					string sSection=pMessage->m_mapParameters[135];
						CMD_Update_Time_Code(iStreamID,sTime.c_str(),sTotal.c_str(),sSpeed.c_str(),sTitle.c_str(),sSection.c_str(),sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
						if( (itRepeat=pMessage->m_mapParameters.find(72))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(pMessage->m_mapParameters[72].c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Update_Time_Code(iStreamID,sTime.c_str(),sTotal.c_str(),sSpeed.c_str(),sTitle.c_str(),sSection.c_str(),sCMD_Result,pMessage);
						}
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
					if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
					{
							pMessage->m_bRespondedToMessage=true;
						Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[0]=sCMD_Result;
						SendMessage(pMessageOut);
					}
					else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
						SendString(sCMD_Result);
						}
					if( sCMD_Result!="UNHANDLED" && sCMD_Result!="UNKNOWN DEVICE" )
						iHandled++;
				}
			}
			if( iHandled==0 && !pMessage->m_bRespondedToMessage &&
			(pMessage->m_eExpectedResponse==ER_ReplyMessage || pMessage->m_eExpectedResponse==ER_ReplyString) )
			{
				pMessage->m_bRespondedToMessage=true;
				if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
				{
					Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
					pMessageOut->m_mapParameters[0]="UNHANDLED";
					SendMessage(pMessageOut);
				}
				else
					SendString("UNHANDLED");
			}
		}
		return iHandled!=0;
	}
}; // end class


}
#endif
