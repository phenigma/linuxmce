/**
 *
 * @file ServerSocket.h
 * @brief header file for the ServerSocket class
 * @author
 * @todo notcommented
 *
 */


#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Socket.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using namespace std;

namespace DCE
{
	class SocketListener; /** < to be able to use it in declarations, we include it's header in the cpp file */

	/**
	 * @brief implements a server socket (to handle a specific client command)
	 * See notes about mutex protection in SocketListener.h
	 */
	class ServerSocket : public Socket
	{
	private:
		int m_iReferencesOutstanding; /** < How many references to this are outstanding, we can only delete it when this is 0 */

	public:
		unsigned long m_dwPK_Device; /** < the device ID */

		pluto_pthread_mutex_t m_ConnectionMutex; /** < for controlling access to the shared memory */
		pthread_t m_ClientThreadID; /** < the thread running the main loop */

		SocketListener *m_pListener; /** the listener that created this command handler */

		int m_iInstanceID; // The ID from Command_Impl.  See notes for same variable in Command_Impl.h

		bool m_bThreadRunning,m_bAlreadyRemoved;
		/**
		 * @brief constructor, assignes values to the member data and starts the looping thread
		 */
		ServerSocket( SocketListener *pListener, SOCKET Sock, string sName, string sIPAddress="", string sMacAddress="" );

		/**
		 * @brief destructor, kills the tread
		 */
		virtual ~ServerSocket();


		void Run();

		// Special behavior for the server
		virtual void PingFailed();

		void IncrementReferences() { m_iReferencesOutstanding++; }
		void DecrementReferences() { m_iReferencesOutstanding--; }
		int m_iReferencesOutstanding_get() { return m_iReferencesOutstanding; }

	private:
		/**
		 * @brief runs a loop that handles the client
		 * Returns true if the socket is to be delete after this and false in the other case.
		 */
		bool _Run();

		static void *BeginWapClientThread(void*);


	};
}

#endif
