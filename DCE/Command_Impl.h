/**
 *
 * @file Command_Impl.h
 * @brief header file for the Command_Impl class
 * @author
 * @todo notcommented
 *
 */


#ifndef COMMAND_IMPL_H
#define COMMAND_IMPL_H

#include "HandleRequestSocket.h"
#include "DeviceData_Impl.h"

namespace DCE
{
	class ClientSocket;
	class Message;
	class DeviceData_Impl;
	class Event_Impl;

	typedef map<int, class Command_Impl *> MapCommand_Impl;
	typedef list<class Command_Impl *> ListCommand_Impl;

	/**
	 * @brief return false if DCE should stop processing the message
	 */
	typedef  bool ( Command_Impl::*MessageInterceptorFn )( class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo );

	
	/**
	 * @brief the implementation for the command from the DCE namespace (Data Commands and Events); classes derived from this one handle commmands for specific devices
	 */
	class Command_Impl : public HandleRequestSocket
	{
	
	private:
	
		list<Message *> m_listMessageQueue;  /** < there are two ways of sending a message: realtime and queued (in a sepparted thread); this is the queue of messages */
		vector<string> m_vectSpawnedDevices;  /** < Keep track of all the devices we spawned so we can kill them on create */

	public:
	
		Command_Impl *m_pParent; /** < if the command was created as an embedded command, keep a pointer to it's parent */
		MapCommand_Impl m_mapCommandImpl_Children; /** < map containing the commands that this command has created */
		pluto_pthread_mutex_t m_listMessageQueueMutex; /** < for protecin the access to the MessageQueue */
		pthread_t m_pthread_queue_id; /** < the thread id for the tread that's treating the messages from the message queue */
		pthread_cond_t m_listMessageQueueCond; /** < condition for the messages in the queue */
		bool m_bKillSpawnedDevicesOnExit;  /** < a derived class will set this to true if we should kill the devices when we die */
		class Router *m_pRouter; /** < this will be NULL if this is not a plug-in being loaded in the same memory space, otherwise it will point to the router that can see the shared mamory space */
		
		Command_Impl *m_pPrimaryDeviceCommand; /** < pointer to the command for the primary device (if this one was spawned by it) */
		DeviceData_Impl *m_pData; /** < a pointer to a device data implementation class (one derived from it) */
		Event_Impl *m_pEvent; /** < a pointer to the event implementation class (one derived from it) */
		Event_Impl *m_pcRequestSocket;  /** < Create a second socket for handling requests separately */
		
		int m_iTargetDeviceID; /** < the device targeted by the command */

		bool m_bWatchdogRunning; /** < specifies if the watchdog is running */
		bool m_bStopWatchdog; /** < specifies if the watchdog is stoped @todo ask is it used? */
		pthread_t m_pThread; /** < the wachdog thread */

		/** < If this is a plug-in, the message interceptors will be registered directly.  Otherwise there will be 'callbacks' stored in this map */
		int m_dwMessageInterceptorCounter;
		map<int, MessageInterceptorFn> m_mapMessageInterceptorFn;
        MessageInterceptorFn m_mapMessageInterceptorFn_Find(int dwMessageInterceptorCounter) { map<int,MessageInterceptorFn>::iterator it = m_mapMessageInterceptorFn.find(dwMessageInterceptorCounter); return it==m_mapMessageInterceptorFn.end() ? NULL : (*it).second; }

		/** flags */
		
		bool m_bQuit; /** < set when DeviceManager wants the app to terminate */
		bool m_bReload;
		bool m_bHandleChildren; /** < used by ReceivedMessage() */
		bool m_bLocalMode; /** < no router */
		bool m_bGeneric; /** < This is not a custom generated command handler.  Just a generic child with no handlers. */
		
		
		/**
		 * @brief create a top-level command
		 * will establish a connection to the server download the data, and attempt to run CreateCommand to instantiate child objects
		 */
		Command_Impl( int DeviceID, string ServerAddress, bool bLocalMode=false, class Router *pRouter=NULL );

		/**
		 * @brief create a child command
		 * assumes a pre-existing connection to the server and requires that the data and event classes be already generated
		 */
		Command_Impl( Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, class Router *pRouter );
		
		/**
		 * @brief frees memory allocated on the heap and calls KillSpawnedDevices if the flag is set
		 * @see KillSpawnedDevices
		 */
		virtual ~Command_Impl();

		/**
		 * @brief  now all this used to do is done automatically; keeping it for now *just in case*
		 * If your implementation can handle child devices, it MUST override and process
		 * CreateCommand to instantiate your customized command processors.  
		 * If DeviceManager passes a child device and this returns NULL, your 
		 * app will terminate with an error.
		 */
		virtual Command_Impl *CreateCommand( int iPK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent );
		
		/**
		 * @brief used by devices witch are plug-ins to register
		 * @return true if successfull
		 */
		virtual bool Register() { return true; };

		/**
		 * @brief accessor for the m_bHandleChildren data member
		 */
		void HandleChildren() { m_bHandleChildren = true; };
		
		/**
		 * @brief creates a new command embedded into this one
		 */
		virtual void CreateChildren();
		
		/**
		 * @brief this will try to spawn the children as separate sessions (for example different displays under Linux or different comman prompts for Windows)
		 * derived classes should implement this to handle special cases, such as if the child requires the GUI interface
		 * 
		 */
		virtual bool SpawnChildDevice( class DeviceData_Impl *pDeviceData_Impl_Child, string sDisplay="" );  // Another implementation may override this, passing in the display to export -- Linux only
		
		virtual void KillSpawnedDevices();

		/**
		 * @brief replaces the values for all the parameters in m_pData with new values specified in sReplacement
		 * @param sReplacement looks like this: param_index,param_new_value,param_index,param_new_value ...
		 */
		virtual void ReplaceParams( ::std::string sReplacement );

		/**
		 * @brief tries to connect to a client socket
		 * @return true on success, false otherwise
		 */		 
		virtual bool Connect();
		
		/** events */
		
		/**
		 * @brief override to get notified when the server has changed a data parameter
		 */
		virtual void OnDataChange( int ID, string ValueOld, string ValueNew ) {};
		
		/**
		 * @brief sets the m_bQuit mb data; used when DeviceManager wants the app to terminate
		 * @todo check: 1/4/2004 - AB removed this to try to stop socket failures Disconnect()
		 */
		virtual void OnQuit() { m_bQuit = true; };
		
		/**
		 * @brief when DeviceManager wants the app to recheck its config, by default it exits.
		 * sets the m_bReload, m_bQuit, m_bTerminate flags to true
		 * @todo test if it's a plug-in, just die and it will be reloaded (same as above Disconnect());
		 */
		virtual void OnReload() { m_bReload = true; m_bQuit = true; m_bTerminate=true; }
		
		/**
		 * @brief just calls OnQuit()
		 * @see m_bQuit
		 */
		virtual void OnUnexpectedDisconnect() { OnQuit(); };
		
		/**
		 * @brief called when the socket disconnects, should be overriden
		 */
		virtual void OnDisconnect() {};

		
		
		/**
		 * @brief override to handle specifying real-time parameter data (like running time)
		 */
		virtual void RequestingParameter( int iPK_DeviceData ) {};
		
		/**
		 * @brief when it receives a strin it sends it to all the command impl children
		 */
		virtual void ReceivedString( string sLine );
		
		/**
		 * @brief performes a specific action bases on the message type
		 */
		virtual bool ReceivedMessage( Message *pMessage );
		
		/**
		 * @brief adds the message to the message queue
		 * Most of the time when a device wants to send a message it doesn't want to block it's thread
		 * until the message is sent by using the normal SendMessageWithConfirm.  Also, SendMessageWithConfirm introduces
		 * the possibility that the receiving device may send something back in response, creating a 
		 * potential race condition if the thread sending the message is blocking the same mutex
		 * as the thread on which the new command comes in on.  To prevent this, call QueueMessageToRouter,
		 * which causes the message to be put in a queue and sent on a separate thread.  This is like
		 * the difference between SendMessage and PostMessage in Windows.
		 */
		void QueueMessageToRouter( Message *pMessage );
		
		/**
		 * @brief sends a message.  This will send the message on the event socket since that is the correct outgoing socket
		 */
		void SendMessageToRouter( Message *pMessage ) { m_pEvent->SendMessage(pMessage); }

		/**
		 * @brief sends all the messges in the message queue
		 * @warning Do not call directly.  For internal use only (called by the queue handler only)
		 */
		virtual void ProcessMessageQueue();

		/**
		 * @brief
		 * Send a command.  If the command includes out parameters, the framework
		 * will wait for a response so it can fill those parameters.  Otherwise, it will
		 * send without waiting for a reply
		 */

		bool SendCommand( class PreformedCommand &pPreformedCommand ) { return InternalSendCommand( pPreformedCommand,-1,NULL ); }

		/**
		 * @brief
		 * Send a command and wait for the reply, which will be put into p_sReponse.
		 * If the command is successful, it will be "OK".
		 */
		bool SendCommand(class  PreformedCommand &pPreformedCommand, string *p_sResponse ) { return InternalSendCommand( pPreformedCommand,1,p_sResponse ); }

		/**
		 * @brief
		 * Send a command and don't wait for a reply even if the command has out 
		 * parameters.  The out parameters will be unmodified
		 */
		bool SendCommandNoResponse( class PreformedCommand &pPreformedCommand ) { return InternalSendCommand( pPreformedCommand,0,NULL ); }
		bool InternalSendCommand( class PreformedCommand &pPreformedCommand, int iConfirmation, string *p_sResponse );

		/**
		 * @brief Register a function that we want to get called back when a message matching a given criteria is received.
		 * Any of the criteria may be 0, indicating all messages will be a match.
		 */
		void RegisterMsgInterceptor(MessageInterceptorFn pMessageInterceptorFn,int PK_Device_From,int PK_Device_To,int PK_DeviceTemplate,int PK_DeviceCategory,int MessageType,int MessageID);

		/**
		 * @brief If this is not a plugin loaded into the Router's memory space, the callback will be in the form of a message to this function
		 */
		void InterceptedMessage(Message *pMessage);

		/** Re-route the primitives through the primary device command. */
		
		/** @brief sending a string directly or through the parent device */
		virtual bool SendString(string s)
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->SendString( s );
			else
				return Socket::SendString( s );
		};

		/** @brief recieving a string directly or through the parent device */
		virtual bool ReceiveString( string &s )
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->ReceiveString( s );
			else
				return Socket::ReceiveString( s );
		}

		/** @brief sending a data block directly or through the parent device */
		virtual bool SendData( int iSize, const char *pcData )
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->SendData( iSize, pcData );
			else
				return Socket::SendData( iSize, pcData );
		}

		/** @brief recieving a data block directly or through the parent device */
		virtual bool ReceiveData( int iSize, char *pcData)
		{
			if ( this != m_pPrimaryDeviceCommand )
				return m_pPrimaryDeviceCommand->ReceiveData( iSize, pcData );
			else
				return Socket::ReceiveData( iSize, pcData );
		}

		/**
		 * 
		 * Watchdog will kill the connection if it exceeds Timeout.
		 * Eventually SendMessage will need to be overridden to create
		 * a watchdog and retransmit if they don't go through.
		 * 
		 */

		/**
		 * @brief starts the watchdog thread with the specified timeout
		 */
		void StartWatchDog( clock_t Timeout );


		/**
		 * @brief stops the watchdog tread
		 */
		void StopWatchDog();

	};
}

#endif
