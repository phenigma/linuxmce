#ifndef Bluetooth_DongleBase_h
#define Bluetooth_DongleBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Bluetooth_Dongle_Event : public Event_Impl
{
public:
	Bluetooth_Dongle_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID,13, ServerAddress, bConnectEventHandler) {};
	Bluetooth_Dongle_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice );
	virtual void Mobile_orbiter_detected(string sMac_Address,int iSignal_Strength,string sID)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 4,3,5,sMac_Address.c_str(),6,StringUtils::itos(iSignal_Strength).c_str(),7,sID.c_str()));
	}

	virtual void Mobile_orbiter_linked(string sMac_Address,string sVersion)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 5,2,5,sMac_Address.c_str(),8,sVersion.c_str()));
	}

	virtual void Mobile_orbiter_lost(string sMac_Address,bool bConnectionFailed)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 6,2,5,sMac_Address.c_str(),11,(bConnectionFailed ? "1" : "0")));
	}

};


//   OUR DATA CLASS 

class Bluetooth_Dongle_Data : public DeviceData_Impl
{
public:
	virtual ~Bluetooth_Dongle_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 13; } ;
	virtual const char *GetDeviceDescription() { return "Bluetooth_Dongle"; } ;
};



//   OUR COMMAND CLASS 

class Bluetooth_Dongle_Command : public Command_Impl
{
public:
	Bluetooth_Dongle_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Bluetooth_Dongle_Event(DeviceID, ServerAddress);
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Bluetooth_Dongle_Data();
		if( Size )
			m_pData->SerializeRead(Size,pConfig);
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, 13,ServerAddress);
	};
	Bluetooth_Dongle_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Bluetooth_Dongle_Command() {};
	Bluetooth_Dongle_Event *GetEvents() { return (Bluetooth_Dongle_Event *) m_pEvent; };
	Bluetooth_Dongle_Data *GetData() { return (Bluetooth_Dongle_Data *) m_pData; };
	const char *GetClassName() { return "Bluetooth_Dongle_Command"; };
	virtual int PK_DeviceTemplate_get() { return 13; };
	static int PK_DeviceTemplate_get_static() { return 13; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	//Event accessors
	void EVENT_Mobile_orbiter_detected(string sMac_Address,int iSignal_Strength,string sID) { GetEvents()->Mobile_orbiter_detected(sMac_Address.c_str(),iSignal_Strength,sID.c_str()); }
	void EVENT_Mobile_orbiter_linked(string sMac_Address,string sVersion) { GetEvents()->Mobile_orbiter_linked(sMac_Address.c_str(),sVersion.c_str()); }
	void EVENT_Mobile_orbiter_lost(string sMac_Address,bool bConnectionFailed) { GetEvents()->Mobile_orbiter_lost(sMac_Address.c_str(),bConnectionFailed); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_Link_with_mobile_orbiter(int iMediaPosition,string sMac_address,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Get_Signal_Strength(string sMac_address,int *iValue,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Create_Mobile_Orbiter(int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Send_File_To_Device(string sFilename,string sMac_address,string sIP_Address,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Ignore_MAC_Address(string sMac_address,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Disconnect_From_Mobile_Orbiter(string sMac_address,string &sCMD_Result,class Message *pMessage) {};

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
				case 60:
					{
						string sCMD_Result="OK";
					int iMediaPosition=atoi(pMessage->m_mapParameters[42].c_str());
					string sMac_address=pMessage->m_mapParameters[47];
						CMD_Link_with_mobile_orbiter(iMediaPosition,sMac_address.c_str(),sCMD_Result,pMessage);
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
					};
					iHandled++;
					continue;
				case 61:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
					int iValue=atoi(pMessage->m_mapParameters[48].c_str());
						CMD_Get_Signal_Strength(sMac_address.c_str(),&iValue,sCMD_Result,pMessage);
						if( pMessage->m_eExpectedResponse==ER_ReplyMessage && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							Message *pMessageOut=new Message(m_dwPK_Device,pMessage->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
						pMessageOut->m_mapParameters[48]=StringUtils::itos(iValue);
							pMessageOut->m_mapParameters[0]=sCMD_Result;
							SendMessage(pMessageOut);
						}
						else if( (pMessage->m_eExpectedResponse==ER_DeliveryConfirmation || pMessage->m_eExpectedResponse==ER_ReplyString) && !pMessage->m_bRespondedToMessage )
						{
							pMessage->m_bRespondedToMessage=true;
							SendString(sCMD_Result);
						}
					};
					iHandled++;
					continue;
				case 62:
					{
						string sCMD_Result="OK";
					int iPK_Device=atoi(pMessage->m_mapParameters[2].c_str());
					string sPK_EntertainArea=pMessage->m_mapParameters[45];
					string sMac_address=pMessage->m_mapParameters[47];
					int iPK_Room=atoi(pMessage->m_mapParameters[57].c_str());
						CMD_Create_Mobile_Orbiter(iPK_Device,sPK_EntertainArea.c_str(),sMac_address.c_str(),iPK_Room,sCMD_Result,pMessage);
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
					};
					iHandled++;
					continue;
				case 80:
					{
						string sCMD_Result="OK";
					string sFilename=pMessage->m_mapParameters[13];
					string sMac_address=pMessage->m_mapParameters[47];
					string sIP_Address=pMessage->m_mapParameters[58];
						CMD_Send_File_To_Device(sFilename.c_str(),sMac_address.c_str(),sIP_Address.c_str(),sCMD_Result,pMessage);
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
					};
					iHandled++;
					continue;
				case 332:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
						CMD_Ignore_MAC_Address(sMac_address.c_str(),sCMD_Result,pMessage);
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
					};
					iHandled++;
					continue;
				case 333:
					{
						string sCMD_Result="OK";
					string sMac_address=pMessage->m_mapParameters[47];
						CMD_Disconnect_From_Mobile_Orbiter(sMac_address.c_str(),sCMD_Result,pMessage);
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
