#ifndef Generic_Input_OuputBase_h
#define Generic_Input_OuputBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"


namespace DCE
{
//   OUR EVENT CLASS 

class Generic_Input_Ouput_Event : public Event_Impl
{
public:
	Generic_Input_Ouput_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) : Event_Impl(DeviceID, ServerAddress, bConnectEventHandler) {};
	Generic_Input_Ouput_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};
	//Events
	class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID);
	virtual void Pin_Changed(bool bOnOff)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 9,1,10,(bOnOff ? "1" : "0")));
	}

};


//   OUR DATA CLASS 

class Generic_Input_Ouput_Data : public DeviceData_Impl
{
public:
	virtual ~Generic_Input_Ouput_Data() {};
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition);
	virtual int GetPK_DeviceList() { return 43; } ;
	virtual const char *GetDeviceDescription() { return "Generic_Input_Ouput"; } ;
	int Get_Channel() { return atoi(m_mapParameters[12].c_str());}
	int Get_InputOrOutput() { return atoi(m_mapParameters[18].c_str());}
	int Get_Default_State() { return atoi(m_mapParameters[19].c_str());}
};



//   OUR COMMAND CLASS 

class Generic_Input_Ouput_Command : public Command_Impl
{
public:
	Generic_Input_Ouput_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
		if( m_bLocalMode )
			return;
		m_pData=NULL;
		m_pEvent = new Generic_Input_Ouput_Event(DeviceID, ServerAddress);
		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			throw "Cannot get configuration data";
		m_pData = new Generic_Input_Ouput_Data();
		m_pData->SerializeRead(Size,pConfig);
		delete pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		m_pData->m_AllDevices.SerializeRead(Size,pConfig);
		delete pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(DeviceID, ServerAddress);
	};
	Generic_Input_Ouput_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Generic_Input_Ouput_Command() {};
	Generic_Input_Ouput_Event *GetEvents() { return (Generic_Input_Ouput_Event *) m_pEvent; };
	Generic_Input_Ouput_Data *GetData() { return (Generic_Input_Ouput_Data *) m_pData; };
	const char *GetClassName() { return "Generic_Input_Ouput_Command"; };
	int PK_DeviceTemplate_get() { return 43; };
	virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	int DATA_Get_Channel() { return GetData()->Get_Channel(); }
	int DATA_Get_InputOrOutput() { return GetData()->Get_InputOrOutput(); }
	int DATA_Get_Default_State() { return GetData()->Get_Default_State(); }
	//Event accessors
	void EVENT_Pin_Changed(bool bOnOff) { GetEvents()->Pin_Changed(bOnOff); }
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
