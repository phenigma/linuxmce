/**
 *
 * @file Socket.h
 * @brief header file for the Socket class
 * @author
 * @todo notcommented
 *
 */


#ifndef SOCKET_H
#define SOCKET_H

#include "PlutoUtils/MultiThreadIncludes.h"

#define INSOCKBUFFER_SIZE 8192

namespace DCE
{
	
	class Message; /** < to be able to use it in declarations; we include it's header in the cpp file */

	/**
	 * @brief defines a socket that works under different architectures
	 * @todo ask !!!
	 */
	class Socket
	{
		
	public:
	
		SOCKET m_Socket; /** < the actual socket @todo ask */
		
		string m_sName; /** < a name for the socket */
		
		int m_iSocketCounter; /** < counts the sockets actually created */

		char *m_pcSockLogFile; /** < name of log file for the socket */
		char *m_pcSockLogErrorFile; /** < name of error log file for the socket */	
		pluto_pthread_mutex_t m_SocketMutex; /** < for synchronisation between threads @todo ask */
		pthread_mutexattr_t m_SocketMutexAttr;  /** < for synchronisation between threads @todo ask */

		char *m_pcInSockBuffer; /** < input buffer for the socket @todo ask */
		char *m_pcCurInsockBuffer; /** < input buffer current position @todo ask */
		
		int m_iSockBufBytesLeft; /** < bytes left in the socket buffer @todo ask */
		int m_iReceiveTimeout; /** < the interval after witch the sockets stops expecting for an answer in seconds */
		
		/**
		 * @brief creates a new socket objest with the specified name, and it also writes a log
		 */
		Socket( string sName );
		
		/**
		 * @brief frees the allocated memory and closes the base socket, and it also writes a log
		 */
		virtual ~Socket();
		
		/** @todo check message */
		//	void DCESocketLock();
		//	void DCESocketUnlock();

		/**
		 * @brief use to set the member
		 */
		void SetReceiveTimeout( int TimeoutSeconds ) { m_iReceiveTimeout = TimeoutSeconds; };
		
		/**
		 * @brief sends the message data imediatlly and deletes the message object if required
		 * @param bDeleteMessage specifies if the message should be deleted after use
		 * @return the return of SendData (witch it uses)
		 * @see SendData
		 */
		bool SendMessage( Message *pMessage, bool bDeleteMessage = true );


		/**
		 * @brief it reads the data from the socket then parses it into a new Message
		 * this gets called in response to a MESSAGE <Length> string
		 */
		virtual Message *ReceiveMessage( int iLength );

		/**
		 * @brief just sends raw data to the socket.
		 */
		virtual bool SendData( int iSize, const char *pcData );
		
		/**
		 * @brief just reads raw data from the socket
		 */
		virtual bool ReceiveData( int iSize, char *pcData );

		/**
		 * @brief sends a string to the socket.
		 * @return false on errors
		 */
		virtual bool SendString( string sToSend );
		
		/**
		 * @brief reads a string from the socket.
		 * @return false on errors
		 */
		virtual bool ReceiveString( string &sResult );

		/**
		 * @brief sends a string to the socket and retrives the answer
		 * @return false on errors
		 */
		virtual string SendReceiveString( string sToSend );

		/**
		 * @brief sends a message to the socket and retrives the answer
		 * @return false on errors
		 */
		virtual Message *SendReceiveMessage( Message *pMessage );
	};
}

#endif

