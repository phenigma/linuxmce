/**
 *
 * @file Event_Impl.h
 * @brief header file for the Event_Impl class
 * @author
 * @todo notcommented
 *
 */
 

#ifndef OCDEVICEEVENT_H
#define OCDEVICEEVENT_H

#include "ClientSocket.h"

namespace DCE
{
	class Message;

	/**
	 * @brief implementation for an event
	 */
	class Event_Impl
	{
	
	public:
	
		unsigned long m_dwPK_Device; /** < the id for the device the event is asociated with */

		ClientSocket *m_pClientSocket; /** < pointer to the client socket the event came from @todo ask*/

		/**
		 * @brief If this is the master device's event, create a connection to the server
		 * @param bConnectEventHandler specifies if we should connect the client socket after creating it
		 */
		Event_Impl( unsigned long dwDevice, unsigned long dwDeviceTemplate, string sServerAddress, bool bConnectEventHandler=true );
		
		/**
		 * @brief If this is a sub-device's event, just use pointer to the connection
		 */
		Event_Impl( ClientSocket *pClientSocket, unsigned long dwDevice );
		
		/**
		 * @brief destructor, just frees memory pointed by m_pClientSocket (if this is a master's device event)
		 */
		virtual ~Event_Impl();

		/**
		 * @brief uses the client socket to send the specified message
		 * @return true if successfull
		 */
		bool SendMessage( Message *pMessage );
		
		/**
		 * @brief uses the client socket to send the specified message with an expected response of type ER_DeliveryConfirmation and puts the response in sResponse
		 * @return true if successfull
		 */
		bool SendMessage( Message *pMessage, string &sResponse );
		
		/**
		 * @brief uses the client socket to send the specified message with an expected response of ER_ReplyMessage
		 * @return the reply message
		 */
		Message *SendReceiveMessage( Message *pMessage );
		
		/**
		 * @brief uses the client socket to send the specified string
		 */
		string SendReceiveString( string s );

		/** @todo check comment */
		//string GetParameter(int ToDevice,int ParmType);

		/**
		 * @brief requests the confiuration from the router
		 * Event gets tasked with pulling the config, because Event has control
		 * over the socket on which the Device Manager is listening.
		 * @returns a pointer to the configuration data or NULL if errors encountered
		 */
		char *GetConfig( /*unsigned long*/ int &dwSize );
		
		/**
		 * @brief requests and tries to receive the device list from the server
		 * @returns NULL on errors, pointer to the data otherwise
		 */
		char *GetDeviceList( /*unsigned long*/ int &dwSize );
		
		/**
		 * @brief sets the specified parameter from the specified device to the specified value
		 * @warning not implemented here
		 */
		void SetParameter( unsigned long dwToDevice, unsigned long dwParmType, string sValue );
		
		/**
		 * @brief gets the specified parameter from the specified device and returns it's value in the sResult parameter
		 * @warning not implemented here
		 */
		bool GetParameter( unsigned long dwToDevice, unsigned long dwParmType, string &sResult );
		
		/**
		 * @brief gets the specified parameter default value from the specified device and returns it in the sResult parameter
		 * @warning not implemented here
		 */
		bool GetInitialParameter( unsigned long dwToDevice, unsigned long dwParmType, string &sResult );

		/**
		 * @brief should create a new Event_Impl object based on the parameters
		 * @see the second constructor
		 * @warning this must be implemented in a derived class
		 * @todo ask for more specific
		 */
		virtual class Event_Impl *CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice ) {return NULL;};
	};
}
#endif
