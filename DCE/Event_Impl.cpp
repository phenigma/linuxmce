#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "Event_Impl.h"
#include "DCE/Logger.h"
#include "Message.h"

using namespace DCE;

Event_Impl::Event_Impl(int DeviceID, string ServerAddress, bool bConnectEventHandler)
{
	m_DeviceID = DeviceID;
	m_pClientSocket = new ClientSocket(DeviceID, ServerAddress, string("Event Dev #") + StringUtils::itos(DeviceID));
	if( bConnectEventHandler )
		m_pClientSocket->Connect();

/*
#ifdef DEBUG
	g_pPlutoLogger->Write(LV_SOCKET,"DeviceEvent constructor %p device ID %d server IP %s socket %p %d",
		this,m_DeviceID,ServerAddress.c_str(),m_pClientSocket,m_pClientSocket->m_Socket);
#endif
*/
}

Event_Impl::Event_Impl(ClientSocket *pClientSocket, int DeviceID) 
{
	m_pClientSocket = pClientSocket;
	m_DeviceID = DeviceID;
}

Event_Impl::~Event_Impl()
{
	if (m_DeviceID == m_pClientSocket->m_DeviceID)
	{
		delete m_pClientSocket;
	}
}

char *Event_Impl::GetConfig(int &Size)
{
	// Create a loop so we can keep waiting
	while(true)
	{
		Size=0;
		m_pClientSocket->SendString("CONFIG");
		string Response;
		m_pClientSocket->ReceiveString(Response);
		if( Response=="WAIT" )
		{
			g_pPlutoLogger->Write(LV_STATUS,"DCE Router asked us to wait--it's busy at the moment");
			Sleep(5000);
			continue;
		}
		if ( Response.substr(0,6)=="CONFIG" )
		{
			Size = atoi(&Response.c_str()[6]);
			char *c = new char[Size+1];
			m_pClientSocket->ReceiveData(Size, c);
			return c;
		}
		
		g_pPlutoLogger->Write(LV_CRITICAL,"DCE Router cannot give us config: %s",Response.c_str());
		return NULL;
	}
	return NULL;
}

char *Event_Impl::GetDeviceList(int &Size)
{
	Size=0;
	m_pClientSocket->SendString("DEVICELIST");
	string Response;
	m_pClientSocket->ReceiveString(Response);
	if ( Response.substr(0,10)=="DEVICELIST" )
	{
		Size = atoi(&Response.c_str()[10]);
		char *c = new char[Size+1];
		m_pClientSocket->ReceiveData(Size, c);
		return c;
	}
	return NULL;
}

bool Event_Impl::SendMessage(Message *pMessage)
{
	return m_pClientSocket->SendMessage(pMessage);
}

bool Event_Impl::SendMessage(Message *pMessage,string &Response)
{
	pMessage->m_eExpectedResponse=ER_DeliveryConfirmation;
	if( !m_pClientSocket->SendMessage(pMessage) )
		return false;
	return m_pClientSocket->ReceiveString(Response);
}

Message *Event_Impl::SendReceiveMessage(Message *pMessage)
{
	pMessage->m_eExpectedResponse=ER_ReplyMessage;
	return m_pClientSocket->SendReceiveMessage(pMessage);
}

string Event_Impl::SendReceiveString(string s)
{
	return m_pClientSocket->SendReceiveString(s);
}
