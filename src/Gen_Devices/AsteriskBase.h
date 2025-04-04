#ifndef AsteriskBase_h
#define AsteriskBase_h
#include "DeviceData_Impl.h"
#include "Message.h"
#include "Command_Impl.h"
#include "Logger.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_EventParameter.h"
#include "../pluto_main/Define_DeviceData.h"


/**
* THESE CLASSES ARE AUTOGENERATED WITH PLUTO DCEGEN APPLICATION
* THIS FILE SHOULD NOT BE MODIFIED MANUALLY
*/

namespace DCE
{

/**
* @brief OUR EVENT CLASS
*/

class Asterisk_Event : public Event_Impl
{
public:

	/**
	* @brief Constructors
	*/
	Asterisk_Event(int DeviceID, string ServerAddress, bool bConnectEventHandler=true) :
		Event_Impl(DeviceID, DEVICETEMPLATE_Asterisk_CONST, ServerAddress, bConnectEventHandler, SOCKET_TIMEOUT) {};
	Asterisk_Event(class ClientSocket *pOCClientSocket, int DeviceID) : Event_Impl(pOCClientSocket, DeviceID) {};

	/**
	* @brief Events builder method
	*/
	class Event_Impl *CreateEvent(unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice);

	/**
	* @brief Events methods for our device
	*/

	virtual void PBX_CommandResult(int iCommandID,int iResult,string sMessage)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_PBX_CommandResult_CONST,
			3 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_CommandID_CONST, StringUtils::itos(iCommandID).c_str(),
			EVENTPARAMETER_Result_CONST, StringUtils::itos(iResult).c_str(),
			EVENTPARAMETER_Message_CONST, sMessage.c_str()));
	}

	virtual void PBX_Ring(string sSource_Channel,string sDestination_Channel,string sSource_Caller_ID,string sDestination_Caller_ID)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_PBX_Ring_CONST,
			4 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Source_Channel_CONST, sSource_Channel.c_str(),
			EVENTPARAMETER_Destination_Channel_CONST, sDestination_Channel.c_str(),
			EVENTPARAMETER_Source_Caller_ID_CONST, sSource_Caller_ID.c_str(),
			EVENTPARAMETER_Destination_Caller_ID_CONST, sDestination_Caller_ID.c_str()));
	}

	virtual void Incoming_Call(string sPhoneCallerID)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Incoming_Call_CONST,
			1 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_PhoneCallerID_CONST, sPhoneCallerID.c_str()));
	}

	virtual void Voice_Mail_Changed(int iValue,int iPK_Users)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Voice_Mail_Changed_CONST,
			2 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Value_CONST, StringUtils::itos(iValue).c_str(),
			EVENTPARAMETER_PK_Users_CONST, StringUtils::itos(iPK_Users).c_str()));
	}

	virtual void PBX_Hangup(string sChannel_ID,string sReason)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_PBX_Hangup_CONST,
			2 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Channel_ID_CONST, sChannel_ID.c_str(),
			EVENTPARAMETER_Reason_CONST, sReason.c_str()));
	}

	virtual void Extensions_Status(string sText)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Extensions_Status_CONST,
			1 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Text_CONST, sText.c_str()));
	}

	virtual void Calls_Status(string sText)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_Calls_Status_CONST,
			1 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Text_CONST, sText.c_str()));
	}

	virtual void PBX_Link(string sSource_Channel,string sDestination_Channel,string sSource_Caller_ID,string sDestination_Caller_ID)
	{
		SendMessage(new Message(m_dwPK_Device, DEVICEID_EVENTMANAGER, PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
			EVENT_PBX_Link_CONST,
			4 /* number of parameter's pairs (id, value) */,
			EVENTPARAMETER_Source_Channel_CONST, sSource_Channel.c_str(),
			EVENTPARAMETER_Destination_Channel_CONST, sDestination_Channel.c_str(),
			EVENTPARAMETER_Source_Caller_ID_CONST, sSource_Caller_ID.c_str(),
			EVENTPARAMETER_Destination_Caller_ID_CONST, sDestination_Caller_ID.c_str()));
	}

};


/**
* @brief OUR DATA CLASS
*/

class Asterisk_Data : public DeviceData_Impl
{
public:

	/**
	* @brief No-op destructor
	*/
	virtual ~Asterisk_Data() {};

	/**
	* @brief Builder data method
	*/
	class DeviceData_Impl *CreateData(DeviceData_Impl *Parent, char *pDataBlock, 
		unsigned long AllocatedSize, char *CurrentPosition);

	/**
	* @brief Returns the id of the device template
	*/
	virtual int GetPK_DeviceList() { return DEVICETEMPLATE_Asterisk_CONST; } ;

	/**
	* @brief Returns the description of the device
	*/
	virtual const char *GetDeviceDescription() { return "Asterisk"; } ;

	/**
	* @brief Device data access methods:
	*/

	bool Get_Manual_configuration()
	{
		if( m_bRunningWithoutDeviceData )
			return (m_pEvent_Impl->GetDeviceDataFromDatabase(m_dwPK_Device,DEVICEDATA_Manual_configuration_CONST)=="1" ? true : false);
		else
			return (m_mapParameters[DEVICEDATA_Manual_configuration_CONST]=="1" ? true : false);
	}

	string Get_Server_IP()
	{
		if( m_bRunningWithoutDeviceData )
			return m_pEvent_Impl->GetDeviceDataFromDatabase(m_dwPK_Device,DEVICEDATA_Server_IP_CONST);
		else
			return m_mapParameters[DEVICEDATA_Server_IP_CONST];
	}

};



//   OUR COMMAND CLASS 

class Asterisk_Command : public Command_Impl
{
public:
	Asterisk_Command(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL)
	: Command_Impl(DeviceID, ServerAddress, bLocalMode, pRouter)
	{
	}
	virtual bool GetConfig()
	{
		m_pData=NULL;
		m_pEvent = new Asterisk_Event(m_dwPK_Device, m_sHostName, !m_bLocalMode);
		if( m_pEvent->m_dwPK_Device )
			m_dwPK_Device = m_pEvent->m_dwPK_Device;
		if( m_sIPAddress!=m_pEvent->m_pClientSocket->m_sIPAddress )	
			m_sIPAddress=m_pEvent->m_pClientSocket->m_sIPAddress;
		m_sMacAddress=m_pEvent->m_pClientSocket->m_sMacAddress;
		if( m_pEvent->m_pClientSocket->m_eLastError!=cs_err_None )
		{
			if( m_pEvent->m_pClientSocket->m_eLastError==cs_err_BadDevice )
			{
				while( m_pEvent->m_pClientSocket->m_eLastError==cs_err_BadDevice && (m_dwPK_Device = DeviceIdInvalid())!=0 )
				{
					delete m_pEvent;
					m_pEvent = new Asterisk_Event(m_dwPK_Device, m_sHostName, !m_bLocalMode);
					if( m_pEvent->m_dwPK_Device )
						m_dwPK_Device = m_pEvent->m_dwPK_Device;
				}
			}
			if( m_pEvent->m_pClientSocket->m_eLastError==cs_err_NeedReload )
			{
				if( RouterNeedsReload() )
				{
					string sResponse;
					Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
					event_Impl.m_pClientSocket->SendString( "RELOAD" );
					if( !event_Impl.m_pClientSocket->ReceiveString( sResponse ) || sResponse!="OK" )
					{
						CannotReloadRouter();
						LoggerWrapper::GetInstance()->Write(LV_WARNING,"Reload request denied: %s",sResponse.c_str());
					}
				Sleep(10000);  // Give the router 10 seconds before we re-attempt, otherwise we'll get an error right away
				}	
			}
		}
		
		if( m_bLocalMode )
		{
			m_pData = new Asterisk_Data();
			return true;
		}
		if( (m_pEvent->m_pClientSocket->m_eLastError!=cs_err_None && m_pEvent->m_pClientSocket->m_eLastError!=cs_err_NeedReload) || m_pEvent->m_pClientSocket->m_Socket==INVALID_SOCKET )
			return false;

		int Size; char *pConfig = m_pEvent->GetConfig(Size);
		if( !pConfig )
			return false;
		m_pData = new Asterisk_Data();
		if( Size )
		{
			if( m_pData->SerializeRead(Size,pConfig)==false )
				return false;
		}
		else
		{
			m_pData->m_dwPK_Device=m_dwPK_Device;  // Assign this here since it didn't get it's own data
			string sResponse;
			Event_Impl event_Impl(DEVICEID_MESSAGESEND, 0, m_sHostName);
			event_Impl.m_pClientSocket->SendString( "PARENT " + StringUtils::itos(m_dwPK_Device) );
			if( event_Impl.m_pClientSocket->ReceiveString( sResponse ) && sResponse.size()>=8 )
				m_pData->m_dwPK_Device_ControlledVia = atoi( sResponse.substr(7).c_str() );
			m_pData->m_bRunningWithoutDeviceData=true;
		}
		delete[] pConfig;
		pConfig = m_pEvent->GetDeviceList(Size);
		if( m_pData->m_AllDevices.SerializeRead(Size,pConfig)==false )
			return false;
		delete[] pConfig;
		m_pData->m_pEvent_Impl = m_pEvent;
		m_pcRequestSocket = new Event_Impl(m_dwPK_Device, DEVICETEMPLATE_Asterisk_CONST,m_sHostName);
		if( m_iInstanceID )
		{
			m_pEvent->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
			m_pcRequestSocket->m_pClientSocket->SendString("INSTANCE " + StringUtils::itos(m_iInstanceID));
		}
		PostConfigCleanup();
		return true;
	};
	Asterisk_Command(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter) : Command_Impl(pPrimaryDeviceCommand, pData, pEvent, pRouter) {};
	virtual ~Asterisk_Command() {};
	Asterisk_Event *GetEvents() { return (Asterisk_Event *) m_pEvent; };
	Asterisk_Data *GetData() { return (Asterisk_Data *) m_pData; };
	const char *GetClassName() { return "Asterisk_Command"; };
	virtual int PK_DeviceTemplate_get() { return DEVICETEMPLATE_Asterisk_CONST; };
	static int PK_DeviceTemplate_get_static() { return DEVICETEMPLATE_Asterisk_CONST; };
	virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage) { };
	virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage) { };
	Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	//Data accessors
	bool DATA_Get_Manual_configuration() { return GetData()->Get_Manual_configuration(); }
	string DATA_Get_Server_IP() { return GetData()->Get_Server_IP(); }
	//Event accessors
	void EVENT_PBX_CommandResult(int iCommandID,int iResult,string sMessage) { GetEvents()->PBX_CommandResult(iCommandID,iResult,sMessage.c_str()); }
	void EVENT_PBX_Ring(string sSource_Channel,string sDestination_Channel,string sSource_Caller_ID,string sDestination_Caller_ID) { GetEvents()->PBX_Ring(sSource_Channel.c_str(),sDestination_Channel.c_str(),sSource_Caller_ID.c_str(),sDestination_Caller_ID.c_str()); }
	void EVENT_Incoming_Call(string sPhoneCallerID) { GetEvents()->Incoming_Call(sPhoneCallerID.c_str()); }
	void EVENT_Voice_Mail_Changed(int iValue,int iPK_Users) { GetEvents()->Voice_Mail_Changed(iValue,iPK_Users); }
	void EVENT_PBX_Hangup(string sChannel_ID,string sReason) { GetEvents()->PBX_Hangup(sChannel_ID.c_str(),sReason.c_str()); }
	void EVENT_Extensions_Status(string sText) { GetEvents()->Extensions_Status(sText.c_str()); }
	void EVENT_Calls_Status(string sText) { GetEvents()->Calls_Status(sText.c_str()); }
	void EVENT_PBX_Link(string sSource_Channel,string sDestination_Channel,string sSource_Caller_ID,string sDestination_Caller_ID) { GetEvents()->PBX_Link(sSource_Channel.c_str(),sDestination_Channel.c_str(),sSource_Caller_ID.c_str(),sDestination_Caller_ID.c_str()); }
	//Commands - Override these to handle commands from the server
	virtual void CMD_PBX_Originate(string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_PBX_Transfer(string sPhoneExtension,string sChannel_1,string sChannel_2,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_PBX_Hangup(string sChannel,string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_Send_Asterisk_Status(string &sCMD_Result,class Message *pMessage) {};
	virtual void CMD_PBX_Application(string sPhoneExtension,string sData_String,string sApplication,string &sCMD_Result,class Message *pMessage) {};

	//This distributes a received message to your handler.
	virtual ReceivedMessageResult ReceivedMessage(class Message *pMessageOriginal)
	{
		map<long, string>::iterator itRepeat;
		if( Command_Impl::ReceivedMessage(pMessageOriginal)==rmr_Processed )
		{
			if( pMessageOriginal->m_eExpectedResponse==ER_ReplyMessage && !pMessageOriginal->m_bRespondedToMessage )
			{
				pMessageOriginal->m_bRespondedToMessage=true;
				Message *pMessageOut=new Message(m_dwPK_Device,pMessageOriginal->m_dwPK_Device_From,PRIORITY_NORMAL,MESSAGETYPE_REPLY,0,0);
				pMessageOut->m_mapParameters[0]="OK";
				SendMessage(pMessageOut);
			}
			else if( (pMessageOriginal->m_eExpectedResponse==ER_DeliveryConfirmation || pMessageOriginal->m_eExpectedResponse==ER_ReplyString) && !pMessageOriginal->m_bRespondedToMessage )
			{
				pMessageOriginal->m_bRespondedToMessage=true;
				SendString("OK");
			}
			return rmr_Processed;
		}
		int iHandled=0;
		for(int s=-1;s<(int) pMessageOriginal->m_vectExtraMessages.size(); ++s)
		{
			Message *pMessage = s>=0 ? pMessageOriginal->m_vectExtraMessages[s] : pMessageOriginal;
			if (pMessage->m_dwPK_Device_To==m_dwPK_Device && pMessage->m_dwMessage_Type == MESSAGETYPE_COMMAND)
			{
				// Only buffer single messages, otherwise the caller won't know which messages were buffered and which weren't
				if( m_pMessageBuffer && pMessage->m_bCanBuffer && pMessageOriginal->m_vectExtraMessages.size()==1 && m_pMessageBuffer->BufferMessage(pMessage) )
					return rmr_Buffered;
				switch(pMessage->m_dwID)
				{
				case COMMAND_PBX_Originate_CONST:
					{
						string sCMD_Result="OK";
						string sPhoneNumber=pMessage->m_mapParameters[COMMANDPARAMETER_PhoneNumber_CONST];
						string sPhoneType=pMessage->m_mapParameters[COMMANDPARAMETER_PhoneType_CONST];
						string sPhoneExtension=pMessage->m_mapParameters[COMMANDPARAMETER_PhoneExtension_CONST];
						string sPhoneCallerID=pMessage->m_mapParameters[COMMANDPARAMETER_PhoneCallerID_CONST];
						CMD_PBX_Originate(sPhoneNumber.c_str(),sPhoneType.c_str(),sPhoneExtension.c_str(),sPhoneCallerID.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_PBX_Originate(sPhoneNumber.c_str(),sPhoneType.c_str(),sPhoneExtension.c_str(),sPhoneCallerID.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_PBX_Transfer_CONST:
					{
						string sCMD_Result="OK";
						string sPhoneExtension=pMessage->m_mapParameters[COMMANDPARAMETER_PhoneExtension_CONST];
						string sChannel_1=pMessage->m_mapParameters[COMMANDPARAMETER_Channel_1_CONST];
						string sChannel_2=pMessage->m_mapParameters[COMMANDPARAMETER_Channel_2_CONST];
						CMD_PBX_Transfer(sPhoneExtension.c_str(),sChannel_1.c_str(),sChannel_2.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_PBX_Transfer(sPhoneExtension.c_str(),sChannel_1.c_str(),sChannel_2.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_PBX_Hangup_CONST:
					{
						string sCMD_Result="OK";
						string sChannel=pMessage->m_mapParameters[COMMANDPARAMETER_Channel_CONST];
						CMD_PBX_Hangup(sChannel.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_PBX_Hangup(sChannel.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_Send_Asterisk_Status_CONST:
					{
						string sCMD_Result="OK";
						CMD_Send_Asterisk_Status(sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_Send_Asterisk_Status(sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				case COMMAND_PBX_Application_CONST:
					{
						string sCMD_Result="OK";
						string sPhoneExtension=pMessage->m_mapParameters[COMMANDPARAMETER_PhoneExtension_CONST];
						string sData_String=pMessage->m_mapParameters[COMMANDPARAMETER_Data_String_CONST];
						string sApplication=pMessage->m_mapParameters[COMMANDPARAMETER_Application_CONST];
						CMD_PBX_Application(sPhoneExtension.c_str(),sData_String.c_str(),sApplication.c_str(),sCMD_Result,pMessage);
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
						if( (itRepeat=pMessage->m_mapParameters.find(COMMANDPARAMETER_Repeat_Command_CONST))!=pMessage->m_mapParameters.end() )
						{
							int iRepeat=atoi(itRepeat->second.c_str());
							for(int i=2;i<=iRepeat;++i)
								CMD_PBX_Application(sPhoneExtension.c_str(),sData_String.c_str(),sApplication.c_str(),sCMD_Result,pMessage);
						}
					};
					iHandled++;
					continue;
				}
				iHandled += (Command_Impl::ReceivedMessage(pMessage)==rmr_NotProcessed ? 0 : 1);
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
				DeviceData_Impl *pDeviceData_Impl = m_pData->FindChild(pMessage->m_dwPK_Device_To);
				string sCMD_Result="UNHANDLED";
				if( pDeviceData_Impl )
				{
					// Only buffer single messages, otherwise the caller won't know which messages were buffered and which weren't
					if( m_pMessageBuffer && pMessage->m_bCanBuffer && pMessageOriginal->m_vectExtraMessages.size()==1 && m_pMessageBuffer->BufferMessage(pMessage) )
						return rmr_Buffered;
					ReceivedCommandForChild(pDeviceData_Impl,sCMD_Result,pMessage);
				}
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
			(pMessage->m_eExpectedResponse==ER_ReplyMessage || pMessage->m_eExpectedResponse==ER_ReplyString || pMessage->m_eExpectedResponse==ER_DeliveryConfirmation) )
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
		return iHandled!=0 ? rmr_Processed : rmr_NotProcessed;
	}
}; // end class


}
#endif
