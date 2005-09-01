/**
 *
 * @file SocketListener.h
 * @brief header file for the SocketListener class
 * @author
 * @todo notcommented
 *
 */


#ifndef SOCKETLISTENER_H
#define SocketLISTENER_H

#include <map>
#include <list>
#include <string>
#include "ServerSocket.h"
#include "DCE/Logger.h"

namespace DCE
{
	class ServerSocket;  /** < to be able to use it in declarations; we include it's header in the cpp file */
	class Message;  /** < to be able to use it in declarations; we include it's header in the cpp file */

	typedef ::std::map<int, ServerSocket *> DeviceClientMap; /** < integer map of  */

	/**
	 * @brief opens a socket on a server waiting for incomming connections
	 */
	class SocketListener
	{
		
	protected:
		
		pthread_t m_ListenerThreadID; /** < the thread on witch the socket runs */
		int m_iListenPort; /** < the port on witch to listen */
		SOCKET m_Socket; /** < the socket that listens for incomming connections */
		
	public:
		
		string m_sName; /** < the socket listener name */
		pluto_pthread_mutex_t m_ListenerMutex; /** < to control access to the shared memory */

		bool m_bTerminate; /** < set to true when the listener terminates (from the destructor)  */
		bool m_bRunning; /** < specifies if the listener is running - set by StartListening */
		bool m_bClosed; /** < specifies if the socket is closed @todo ask how it's used */
		
		::std::list<Socket *> m_listClients; /** < a list of sockets created for incoming connections */	
		DeviceClientMap m_mapCommandHandlers; /** < map of server sockets associated with clients (command handlers) */

		/**
		 * @brief constructor, creates a SocketListener and gives it the name specified by the parameter; the other member data receive default values
		 * @see the implementation for the mb data default values
		 */
		SocketListener( string sName );

		/**
		 * @brief destructor, cleanins up and waits for the thread to die (writes an entry to the log also)
		 */
		virtual ~SocketListener();

		/**
		 * @brief creates the thread that will listen at the specified port
		 * @todo ask
		 */
		void StartListening( int iPortNumber );
		
		/** @todo check comment */
		// void StopListening();

		/**
		 * @brief runs the listener by creating the master socket, setting it up and telling it to start listening 
		 */
		void Run();

		/**
		 * @brief creates a socket listener based on the parameter data
		 * @see the class member data
		 */
		virtual Socket *CreateSocket( SOCKET newsock, string sName, string sIPAddress="", string sMacAddress="" );
			
		/**
		 * @brief removes the socket from the listClients mb data and clears any dependencies
		 */
		virtual void RemoveSocket( Socket *Socket );
		

		/**
		 * @brief Called when an event handler is registered
		 */
		void RegisterEventHandler( ServerSocket *Socket, int iDeviceID );

		/**
		 * @brief binds the specified socket to the specified device in the mapCommandHandlers mb data (after setting it up a little)
		 */
		void RegisterCommandHandler( ServerSocket *Socket, int iDeviceID );


		/**
		 * @brief waiting to be overriden
		 */
		virtual void RegisteredEventHandler( ServerSocket *pSocket, int iDeviceID ) {};

		/**
		 * @brief waiting to be overriden
		 */
		virtual void RegisteredCommandHandler( ServerSocket *pSocket, int iDeviceID ) {};
		
		/**
		 * @brief gets the host IP address
		 */
		::std::string GetIPAddress();

		/**
		 * @brief gets the host IP address for the specified device
		 */
		::std::string GetIPAddress( int iDeviceID );
	
		/**
		 * @brief sends the string to the specified device using the associated socket
		 */
		bool SendString( int iDeviceID, string sLine );
		
		/**
		 * @brief sends the message to the specified device using the associated socket
		 */
		bool SendMessage( int iDeviceID, Message *pMessage );

		/**
		 * @brief sends the data pointed by pcData to the specified device using the associated socket
		 */
		bool SendData( int iDeviceID, int iLength, const char *pcData );

		/**
		 * @brief override to get the string from the specified socket
		 */
		virtual bool ReceivedString( Socket *pSocket, string sLine )=0;

		/**
		 * @brief override to get the message from the specified socket
		 */
		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage )=0;

		/**
		 * @brief override it to set a action for the disconection of the specified device
		 * @todo ask
		 */
		virtual void OnDisconnected( int iDeviceID ) {}

		/**
		 * @brief If a device connects but doesn't know it's ID, this function can be implemented and it will
		 * look up the device id based on the device template and the IP Address
		 */
		virtual int GetDeviceID( int iPK_DeviceTemplate, string sMacAddress, string sIPAddress) { return 0; } // Something else needs to implement this }

		/**
		 * @brief Returns 0 if the device doesn't exist, 1 if it does but isn't of the device template, 2 if it's a good match, 3 if it's a new device and the router needs to be reloaded
		 */
		virtual int ConfirmDeviceTemplate( int iPK_Device, int iPK_DeviceTemplate ) { return true; }  // Something else needs to implement this }

		/**
		 * @brief Close any open sockets
		 */
		void DropAllSockets();

		// Called when a ping test fails
		virtual void PingFailed( ServerSocket *pServerSocket, int dwPK_Device ) {};
	};
}

#endif
