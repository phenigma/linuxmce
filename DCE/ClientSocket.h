/**
 *
 * @file ClientSocket.h
 * @brief header file for the ClientSocket class
 * @author
 * @todo notcommented
 *
 */


#ifndef DCECLIENTSOCKET_H
#define DCECLIENTSOCKET_H

#include "Socket.h"

#define MAX_RETRIES 3	// Maximum retries to establish TCP connection

namespace DCE
{
	class SocketListener; /** < to be able to use it in declarations; we include it's header in the cpp file */

	/**
	 * @brief the ClientSocket's responsibility is to make, and keep a reliable connection to the server.
	 */
	class ClientSocket : public Socket
	{
		
	public:
		
		string m_sIPAddress; /** < the server to connect to IP address */
		double m_dwPK_Device; /** < the device identifier */
		clock_t m_clockTimeout; /** < used to set timeouts */
		bool m_bWatchdogRunning; /** < specifies if the watchdog is running */
		bool m_bStopWatchdog; /** < specifies if the watchdog is stoped @todo ask is it used? */
		pthread_t m_pThread; /** < the wachdog thread */
		string m_sMyIPAddress; /** this client's machine IP address */

		/**
		 * @brief constructor, assignes values to the member data
		 */
		ClientSocket( int iDeviceID, string sIPAddress, string sName );

		/**
		 * @brief it establishes the connection to the server and eventually send the ExtraInfo using OnConnect; also writes to the logger
		 * @see OnConnect
		 */
		virtual bool Connect( string ExtraInfo = "" );

		/**
		 * @brief sends the extra info and expects for an OK signal from the server
		 */
		virtual bool OnConnect( string ExtraInfo = "" );
		
		/**
		 * @brief closes the socket, used after errors to make sure we have no hanging connections
		 */
		virtual void Disconnect();

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
