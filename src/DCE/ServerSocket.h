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
	 * @todo ask !!!
	 */
	class ServerSocket : public Socket
	{
		
	public:
		
		unsigned long m_dwPK_Device; /** < the device ID */

		pluto_pthread_mutex_t m_ConnectionMutex; /** < for controlling access to the shared memory */
		pthread_t m_ClientThreadID; /** < the thread running the main loop */

		SocketListener *m_pListener; /** the listener that created this command handler */
		class Command_Impl *m_pCommand; /** pointer to the command for this server socket (the one for the device) */

		bool m_bThreadRunning;
		/**
		 * @brief constructor, assignes values to the member data and starts the looping thread
		 */
		ServerSocket( SocketListener *pListener, SOCKET Sock, string sName, string sIPAddress="" );

		/**
		 * @brief destructor, kills the tread
		 */
		virtual ~ServerSocket();
			
		void Run();

	private:
		/**
		 * @brief runs a loop that handles the client
		 */
		void _Run();
		
		static void *BeginWapClientThread(void*);
		
		
	};
}

#endif
