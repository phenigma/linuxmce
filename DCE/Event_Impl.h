#ifndef OCDEVICEEVENT_H
#define OCDEVICEEVENT_H

#include "ClientSocket.h"

namespace DCE
{
	class Message;

	class Event_Impl
	{
	public:
		int m_DeviceID;

		ClientSocket *m_pClientSocket;

		// If this is the master device's event, create a connection to the server
		Event_Impl(int DeviceID, string ServerAddress, bool bConnectEventHandler=true);
		// If this is a sub-device's event, just use pointer to the connection
		Event_Impl(ClientSocket *pClientSocket,int DeviceID);
		virtual ~Event_Impl();

		bool SendMessage(Message *pMessage);
		bool SendMessage(Message *pMessage,string &Response);
		Message *SendReceiveMessage(Message *pMessage);
		string SendReceiveString(string s);

		// Event gets tasked with pulling the config, because Event has control
		// over the socket on which the Device Manager is listening.
		char *GetConfig(int &Size);
		char *GetDeviceList(int &Size);
		//	string GetParameter(int ToDevice,int ParmType);
		void SetParameter(int ToDevice, int ParmType, string Value);
		bool GetParameter(int ToDevice,int ParmType, string &Result);
		bool GetInitialParameter(int ToDevice,int ParmType, string &Result);

		// This must be implemented in a derived class
		virtual class Event_Impl *CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID) {return NULL;};
	};
}
#endif
