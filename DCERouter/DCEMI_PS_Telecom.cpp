#include "DCEMI_PS_Telecom.h"
#include "Router.h"
#include "Message.h"
#include "../pluto_main/Define_DeviceTemplate.h"
#include "../pluto_main/Define_Command.h"
#include "../pluto_main/Define_Event.h"
#include "../pluto_main/Define_CommandParameter.h"
#include "../pluto_main/Define_DeviceCategory.h"


//temp
#define EVENT_NEW_NUMBER_OF_MESSAGES_CONST 1



bool DCEMI_PS_Telecom::Register()
{
	m_pRouter->RegisterInterceptor(
		new MessageInterceptorCallBack(this,(MessageInterceptorFn)(&DCEMI_PS_Telecom::NewNumMessages))
		,0,0,0,0,MESSAGETYPE_EVENT,EVENT_NEW_NUMBER_OF_MESSAGES_CONST);

	return true;
}

bool DCEMI_PS_Telecom::NewNumMessages(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo) 
{
	/*
	int User = atoi(pMessage->m_Parameters[C_EVENTPARAMETER_PK_USERS_CONST].c_str());
	
	string sNumMessages = pMessage->m_Parameters[C_EVENTPARAMETER_NUM_MESSAGES_CONST];
	int NumMessages;

	if( sNumMessages.length()>=3 && sNumMessages.substr(0,2)=="c=" )
		NumMessages = atoi(sNumMessages.substr(2).c_str());
	else
		NumMessages = atoi(sNumMessages.c_str());

	g_pPlutoLogger->Write(LV_STATUS, "New number of message for user: %d is %d",User,NumMessages);

	string query = "UPDATE Users SET NumVoiceMessage=" + StringUtils::itos(NumMessages) + " WHERE FK_S_Users=" + StringUtils::itos(User);
	int iresult;
	if( (iresult=m_pRouter->threaded_mysql_query(query.c_str()))!=0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Set User Messages, update mysql failed: %d",iresult);
	}

	// Now go through all the controllers and message indicators that have been bound
	
	map<int,MessageBindingList *>::iterator itMBList;
	itMBList=m_pPlutoEvents->m_mapMessageBindingList.find(User);
	if( itMBList!=m_pPlutoEvents->m_mapMessageBindingList.end() )
	{
		MessageBindingList *mbList=(*itMBList).second;
		map<int,MessageBinding *>::iterator itMB;
		for(itMB=mbList->m_mapMessageBinding.begin();itMB!=mbList->m_mapMessageBinding.end();++itMB)
		{
			MessageBinding *mb=(*itMB).second;
			for(int i=0;i<(int) mb->m_vectDesignObjs.size();++i)
			{
				m_pRouter->DispatchMessage(new Message(0,mb->m_PK_Controller,PRIORITY_NORMAL,MESSAGETYPE_COMMAND,COMMAND_SET_TEXT_CONST,
					3,COMMANDPARAMETER_PK_DesignObj_CONST,mb->m_vectDesignObjs[i].c_str(),COMMANDPARAMETER_ID_CONST,StringUtils::itos(TEXT_MESSAGE_INDICATOR_CONST).c_str(),
					COMMANDPARAMETER_Text_CONST,StringUtils::itos(NumMessages).c_str()));
			}
		}
	}
*/
	return true;
}
