/**
 *
 * @file RA_Processor.h
 * @brief header file for the request processor class
 * @author
 * @todo notcommented
 *
 */

#ifndef RA_PROCESSOR_H
#define RA_PROCESSOR_H

#include <string>
#include <list>
#include <map>
#include "PlutoUtils/MyStl.h"
#include "DCE/Socket.h"

using namespace std;

/**
 * @brief request action processor class
 */
class RA_Processor
{
public:
	class RA_Config *m_pRA_Config;

private:

    unsigned long m_dwEstablishmentID; /** < ID of the establishment */
    unsigned long m_dwSoftwareVersion; /** < software version */

public:

	DCE::Socket *m_pSocket;
    /**
     * @brief basic constructor
     * @param EstablishmentID ID of the establishment
     * @param SoftwareVersion version
     */
    RA_Processor( unsigned long dwEstablishmentID, unsigned long dwSoftwareVersion, class RA_Config *pRA_Config = NULL )
    {
        m_dwEstablishmentID = dwEstablishmentID; 
		m_dwSoftwareVersion = dwSoftwareVersion;
		m_pRA_Config        = pRA_Config;
		m_pSocket			= NULL;
    };

	virtual ~RA_Processor() 
	{
	}

    /**
     * @brief adds a request
     * @param pRequest pointer to the request to be added
     */
    void AddRequest( class RA_Request *pRequest ) { MYSTL_ADDTO_LIST( m_listNewRequests, pRequest ); }

    /**
     * @brief sends a request to a server, and opens a socket
     * @param ServerAddress the adress of the server to send the request to
     * @param ppSocket, an optional pointer to a socket.  If specified, this function will set this to point to the socket, and will not close/destroy the socket after sending the requests
     */
	bool SendRequests( string sServerAddress, DCE::Socket **ppSocket=NULL );

    /**
     * @brief sends requests to a server over an existing socket
     * @param pSocket The socket to send the request on
     */
	bool SendRequests(DCE::Socket *pSocket);

    /**
     * @brief sends a single request to a server over an existing socket
     * @param pSocket The socket to send the request on
     */
	bool SendRequest(RA_Request *pRequest,DCE::Socket *pSocket);

	/**
     * @brief receive a request
     * @param pSocket the socket to receive the request on
     */
	bool ReceiveRequests(DCE::Socket *pSocket );

	void RemoveRequest(RA_Request *pRequest_Del);

private:

    /**
     * @brief for Symbian compatibility; see Symbian.txt for detailes
     */
    MYSTL_CREATE_LIST( m_listNewRequests, RA_Request );

    /**
     * @brief for Symbian compatibility; see Symbian.txt for detailes
     */
    MYSTL_CREATE_LIST( m_listRequests, RA_Request );

    /**
     * @brief for Symbian compatibility; see Symbian.txt for detailes
     */
    MYSTL_CREATE_LIST( m_listActions, RA_Action );

	// *** STATIC FUNCTIONS -- The derived application must implement these
public:
	/**
	 * @brief Builds a request from data.  The application that uses this must provide the implementatation for this method.
     * @return the new RA_Request object build
	 * @param dwSize size of data to build the request from
     * @param pcData the data to build the request from
	 * @param dwRequestID the request's ID
	 */
	static RA_Request *BuildRequestFromData( long dwSize, const char *pcData, unsigned long dwRequestID );

	/**
	 * @brief Builds an application from data.  The application that uses this must provide the implementatation for this method.
     * @return the new RA_Action object build
	 * @param dwSize size of data to build the action from
     * @param pcData the data to build the action from
	 * @param dwActionID the action's ID
	 */
	static RA_Action *BuildActionFromData( long dwSize, const char *pcData, unsigned long dwActionID );

	/**
	 * @brief Returns a pointer to the request processor.  The implementation can just return a pointer to an instance of this class unless it needs special data, like session tracking
     * @return the new RequestProcessor to handle this connection
	 */
	static RA_Processor *CreateRA_Processor(class RAServerSocket *pRAServerSocket,class RA_Config *pRA_Config = NULL);
};

#endif
