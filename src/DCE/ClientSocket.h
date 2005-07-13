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

#ifdef WINCE
	#include <time.h>
	#ifndef _CLOCK_T_DEFINED
		typedef unsigned long clock_t;
		#define _CLOCK_T_DEFINED
	#endif 
#endif

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
		long m_dwPK_Device; /** < the device identifier */
		clock_t m_clockTimeout; /** < used to set timeouts */
		string m_sMyIPAddress; /** this client's machine IP address */

		/**
		 * @brief constructor, assignes values to the member data
		 */
		ClientSocket( int iDeviceID, string sIPAddress, string sName );

		virtual ~ClientSocket();

		/**
		 * @brief it establishes the connection to the server and eventually send the ExtraInfo using OnConnect; also writes to the logger
		 * @see OnConnect
		 */
		virtual bool Connect( int PK_DeviceTemplate=0,string sExtraInfo="" );

		/**
		 * @brief sends the extra info and expects for an OK signal from the server
		 */
		virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" );
		
		/**
		 * @brief closes the socket, used after errors to make sure we have no hanging connections
		 */
		virtual void Disconnect();

	};
}

#endif
