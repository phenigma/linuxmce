#ifndef Command_Impl_H
#define Command_Impl_H

#include "HandleRequestSocket.h"

namespace DCE
{
	class ClientSocket;
	class Message;
	class DeviceData_Impl;
	class Event_Impl;

	typedef map<int, class Command_Impl *> MapCommand_Impl;
	typedef list<class Command_Impl *> ListCommand_Impl;

	// return false if DCE should stop processing the message
	typedef  bool (Command_Impl::*MessageInterceptorFn)(class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo);

	class Command_Impl : public HandleRequestSocket
	{
	private:
		list<Message *> m_MessageQueue;  // See QueueMessage notes
		vector<string> m_vectSpawnedDevices;  // Keep track of all the devices we spawned so we can kill them on create

	public:
		Command_Impl *m_pParent;
		MapCommand_Impl m_mapCommandImpl_Children;  
		pluto_pthread_mutex_t m_MessageQueueMutex;
		pthread_t m_pthread_queue_id;
		pthread_cond_t m_MessageQueueCond;
		bool m_bKillSpawnedDevicesOnExit;  // A derived class will set this to true if we should kill the devices when we die

		virtual bool Register() {return true; };   // returns true if successful.  This is only used by devices that are plug-ins

		class Router *m_pRouter; // This will be NULL if this is a plug-in being loaded in the same memory space

		// If your implementation can handle child devices, it MUST override and process
		// CreateCommand to instantiate your customized command processors.  
		// If DeviceManager passes a child device and this returns NULL, your 
		// app will terminate with an error.
		virtual Command_Impl *CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);

		virtual ~Command_Impl();

		void HandleChildren() { m_bHandleChildren = true; };
		virtual void CreateChildren();
		// This will try to spawn the children as separate sessions.  Derived classes should implement this to 
		// handle special cases, such as if the child requires the GUI interface
		virtual bool SpawnChildDevice(class DeviceData_Impl *pDeviceData_Impl_Child,string Display="");  // Another implementation may override this, passing in the display to export -- Linux only
		virtual void KillSpawnedDevices();

		Command_Impl *m_pPrimaryDeviceCommand;
		DeviceData_Impl *m_pData;
		Event_Impl *m_pEvent;
		Event_Impl *m_pcRequestSocket;  // Create a second socket for handling requests separately

		bool m_bQuit, m_bReload, m_bHandleChildren, m_bLocalMode;
		int m_TargetDeviceID;

		// Create a top-level command.  Will establish a connection to the server 
		// download the data, and attempt to run CreateCommand to instantiate child objects
		Command_Impl( int DeviceID, string ServerAddress, bool bLocalMode=false,class Router *pRouter=NULL);

		// Create a child command.  Assumes a pre-existing connection to the server
		// and requires that the data and event classes be already generated.
		Command_Impl(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent,class Router *pRouter);

		virtual void ReplaceParams(::std::string replacement);

		// Override OnDataChange to get notified when the server has changed a data parameter
		// Override RequestingParamter to handle specifying real-time parameter data (like running time)
		virtual bool Connect();
		virtual void OnDataChange(int ID, string ValueOld, string ValueNew) {};
		virtual void OnQuit() { m_bQuit = true; /* 1/4/2004 - AB removed this to try to stop socket failures Disconnect(); */};  // When DeviceManager wants the app to terminate, it calls this.
		virtual void OnReload() { m_bReload = true; m_bQuit = true; m_bTerminate=true; }// todo-test If it's a plug-in, just die and it will be reloaded /* same as above Disconnect(); */ }; // When DeviceManager wants the app to recheck its config, by default it exits.
		virtual void RequestingParameter(int PK_DeviceData) {};
		virtual bool ReceivedMessage(Message *pMessage);
		virtual void ReceivedString(string s);
		virtual void OnUnexpectedDisconnect() { OnQuit(); };
		virtual void OnDisconnect() {};

		// Most of the time when a device wants to send a message it doesn't want to block it's thread
		// until the message is sent by using the normal SendMessageWithConfirm.  Also, SendMessageWithConfirm introduces
		// the possibility that the receiving device may send something back in response, creating a 
		// potential race condition if the thread sending the message is blocking the same mutex
		// as the thread on which the new command comes in on.  To prevent this, call QueueMessage,
		// which causes the message to be put in a queue and sent on a separate thread.  This is like
		// the difference between SendMessage and PostMessage in Windows.
		void QueueMessage(Message *pMessage);
		virtual void ProcessMessageQueue();  // Do not call directly.  For internal use only

		// This will send the message and wait for the destination device to receive it and acknowledge with an "OK".  Only
		// then will the function return a true value.  Otherwise it returns false, and Response is whatever response
		// the destination device or server provided
		bool SendMessageWithConfirm(Message *pMessage,string &Response);

		// Send a regular command.  If confirmation is 0, it will go in a queue and there will be no response.  The return value is always true.
		// If you send a command that has 'out' parametes, they will not be filled in.  If confirmation is 1, the return value is true on 
		// success and m_pcResponse, if specified, will equal 'OK'.  On failure, the result will be false, and p_sResponse will be assigned
		// a comment, if it was specified.  If iConfirmation is left at -1, the default, the framework will wait for a response if there
		// are out parameters (as if iConfirmation==1), and will send without confirmation if there are no out parameters.
		bool SendCommand(class PreformedCommand &pPreformedCommand,int iConfirmation=-1,string *p_sResponse=NULL);

		// Re-route the primitives through the primary device command.
		virtual bool SendString(string s)
		{
			if (this!=m_pPrimaryDeviceCommand)
				return m_pPrimaryDeviceCommand->SendString(s);
			else
				return Socket::SendString(s);
		};

		virtual bool ReceiveString(string &s)
		{
			if (this!=m_pPrimaryDeviceCommand)
				return m_pPrimaryDeviceCommand->ReceiveString(s);
			else
				return Socket::ReceiveString(s);
		}

		virtual bool SendData(int size, const char *data)
		{
			if (this!=m_pPrimaryDeviceCommand)
				return m_pPrimaryDeviceCommand->SendData(size, data);
			else
				return Socket::SendData(size, data);
		}

		virtual bool ReceiveData(int size, char *data)
		{
			if (this!=m_pPrimaryDeviceCommand)
				return m_pPrimaryDeviceCommand->ReceiveData(size, data);
			else
				return Socket::ReceiveData(size, data);
		}
	};
}

#endif
