#ifndef Orbiter_PluginBase_h
#define Orbiter_PluginBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Orbiter_Plugin_Event : public Event_Impl
{
public:
	Orbiter_Plugin_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID,12, ServerAddress, bConnectEventHandler) {};
	Orbiter_Plugin_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );
	virtual void Follow_Me_Lighting(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 30,4,14,StringUtils::itos(iPK_Orbiter).c_str(),27,StringUtils::itos(iPK_Room).c_str(),31,StringUtils::itos(iPK_Users).c_str(),32,StringUtils::itos(iPK_Room_Left).c_str()));
	}

	virtual void Follow_Me_Climate(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 31,4,14,StringUtils::itos(iPK_Orbiter).c_str(),27,StringUtils::itos(iPK_Room).c_str(),31,StringUtils::itos(iPK_Users).c_str(),32,StringUtils::itos(iPK_Room_Left).c_str()));
	}

	virtual void Follow_Me_Media(int iPK_Orbiter,int iPK_Users,int iPK_EntArea,int iPK_EntArea_Left)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 32,4,14,StringUtils::itos(iPK_Orbiter).c_str(),31,StringUtils::itos(iPK_Users).c_str(),33,StringUtils::itos(iPK_EntArea).c_str(),34,StringUtils::itos(iPK_EntArea_Left).c_str()));
	}

	virtual void Follow_Me_Telecom(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 33,4,14,StringUtils::itos(iPK_Orbiter).c_str(),27,StringUtils::itos(iPK_Room).c_str(),31,StringUtils::itos(iPK_Users).c_str(),32,StringUtils::itos(iPK_Room_Left).c_str()));
	}

	virtual void Follow_Me_Security(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 34,4,14,StringUtils::itos(iPK_Orbiter).c_str(),27,StringUtils::itos(iPK_Room).c_str(),31,StringUtils::itos(iPK_Users).c_str(),32,StringUtils::itos(iPK_Room_Left).c_str()));
	}

};


//   OUR DATA CLASS 

class Orbiter_Plugin_Data : public DeviceData_Impl
{
public:
	virtual ~Orbiter_Plugin_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 12; } ;
	virtual const char *GetDeviceDescription() { return "Orbiter_Plugin"; } ;
	int Get_ThreshHold() { return atoi(m_mapParameters[61].c_str());}
};



//   OUR COMMAND CLASS 

class Orbiter_Plugin_Command : public Command_Impl
{
public:
	Orbiter_Plugin_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Orbiter_Plugin_Event(DeviceID, ServerAddress);
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Orbiter_Plugin_Data();
		if( Size )
			m_pData->SerializeRead(Size,pConfig);
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, 12,ServerAddress);
	};
	Orbiter_Plugin_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Orbiter_Plugin_Command() {};
	Orbiter_Plugin_Event *GetEvents() { return (Orbiter_Plugin_Event *) m_pEvent; };
	Orbiter_Plugin_Data *GetData() { return (Orbiter_Plugin_Data *) m_pData; };
	const char *GetClassName() { return "Orbiter_Plugin_Command"; };
	virtual int PK_DeviceTemplate_get() { return 12; };
	static int PK_DeviceTemplate_get_static() { return 12; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	int DATA_Get_ThreshHold() { return GetData()->Get_ThreshHold(); }
	//Event accessors
	void EVENT_Follow_Me_Lighting(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left) { GetEvents()->Follow_Me_Lighting(iPK_Orbiter,iPK_Room,iPK_Users,iPK_Room_Left); }
	void EVENT_Follow_Me_Climate(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left) { GetEvents()->Follow_Me_Climate(iPK_Orbiter,iPK_Room,iPK_Users,iPK_Room_Left); }
	void EVENT_Follow_Me_Media(int iPK_Orbiter,int iPK_Users,int iPK_EntArea,int iPK_EntArea_Left) { GetEvents()->Follow_Me_Media(iPK_Orbiter,iPK_Users,iPK_EntArea,iPK_EntArea_Left); }
	void EVENT_Follow_Me_Telecom(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left) { GetEvents()->Follow_Me_Telecom(iPK_Orbiter,iPK_Room,iPK_Users,iPK_Room_Left); }
	void EVENT_Follow_Me_Security(int iPK_Orbiter,int iPK_Room,int iPK_Users,int iPK_Room_Left) { GetEvents()->Follow_Me_Security(iPK_Orbiter,iPK_Room,iPK_Users,iPK_Room_Left); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_Set_Current_User(int iPK_Users,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Entertainment_Area(string sPK_EntertainArea,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Current_Room(int iPK_Room,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_New_Mobile_Orbiter(int iPK_Users,int iPK_DeviceTemplate,string sMac_address,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Add_Unknown_Device(string sText,string sID,string sMac_address,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Get_Floorplan_Layout(string *sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Get_Current_Floorplan(string sID,int iPK_FloorplanType,string *sValue_To_Assign,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Orbiter_Registered(string sOnOff,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_FollowMe(int iPK_Device,string sText,int iPK_Users,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Regen_Orbiter(int iPK_Device,string sForce,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Regen_Orbiter_Finished(int iPK_Device,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Set_Room_For_Device(int iPK_Device,int iPK_Room,string &sCMD_Result,class Message *pMessage) {};

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
				case 58:
					{
						string sCMD_Result="OK";
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
						CMD_Set_Current_User(iPK_Users,sCMD_Result,pMessage);
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
				case 59:
					{
						string sCMD_Result="OK";
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
						CMD_Set_Entertainment_Area(sPK_EntertainArea.c_str(),sCMD_Result,pMessage);
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
				case 77:
					{
						string sCMD_Result="OK";
					int iPK_Room=atoi(pMessage->m_mapParameters[57].c_str());
						CMD_Set_Current_Room(iPK_Room,sCMD_Result,pMessage);
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
				case 78:
					{
						string sCMD_Result="OK";
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
					int iPK_DeviceTemplate=atoi(pMessage->m_mapParameters[44].c_str());
					string sMac_address=pMessage->m_mapParameters[47];
						CMD_New_Mobile_Orbiter(iPK_Users,iPK_DeviceTemplate,sMac_address.c_str(),sCMD_Result,pMessage);
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
				case 79:
					{
						string sCMD_Result="OK";
					string sText=pMessage->m_mapParameters[9];
					string sID=pMessage->m_mapParameters[10];
					string sMac_address=pMessage->m_mapParameters[47];
						CMD_Add_Unknown_Device(sText.c_str(),sID.c_str(),sMac_address.c_str(),sCMD_Result,pMessage);
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
				case 183:
					{
						string sCMD_Result="OK";
					string sValue_To_Assign=pMessage->m_mapParameters[5];
						CMD_Get_Floorplan_Layout(&sValue_To_Assign,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[5]=sValue_To_Assign;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 186:
					{
						string sCMD_Result="OK";
					string sID=pMessage->m_mapParameters[10];
					int iPK_FloorplanType=atoi(pMessage->m_mapParameters[46].c_str());
					string sValue_To_Assign=pMessage->m_mapParameters[5];
						CMD_Get_Current_Floorplan(sID.c_str(),iPK_FloorplanType,&sValue_To_Assign,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage )
						{
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[5]=sValue_To_Assign;
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString )
							SendString(sCMD_Result);
					};
					iHandled++;
					continue;
				case 255:
					{
						string sCMD_Result="OK";
					string sOnOff=pMessage->m_mapParameters[8];
						CMD_Orbiter_Registered(sOnOff.c_str(),sCMD_Result,pMessage);
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
				case 261:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sText=pMessage->m_mapParameters[9];
					int iPK_Users=atoi(pMessage->m_mapParameters[17].c_str());
						CMD_Set_FollowMe(iPK_Device,sText.c_str(),iPK_Users,sCMD_Result,pMessage);
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
				case 266:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sForce=pMessage->m_mapParameters[21];
						CMD_Regen_Orbiter(iPK_Device,sForce.c_str(),sCMD_Result,pMessage);
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
				case 267:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
						CMD_Regen_Orbiter_Finished(iPK_Device,sCMD_Result,pMessage);
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
				case 274:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					int iPK_Room=atoi(pMessage->m_mapParameters[57].c_str());
						CMD_Set_Room_For_Device(iPK_Device,iPK_Room,sCMD_Result,pMessage);
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
