/**
 *
 * @file RA_Request.h
 * @brief header file for the RA_Request class
 * @author @todo addauthor
 * @todo notcommented
 *
 */


#ifndef RA_REQUEST_H
#define RA_REQUEST_H


#include "SerializeClass/SerializeClass.h"
//#include "VIPShared/VIPIncludes.h"

// Process outcomes
#define USER_CANNOT_PAY_ONLINE	-100

// General Messages
#define LOGIN_FAILED			-4
#define INTERNAL_ERROR			-3
#define RECORD_NOT_FOUND		-2
#define INVALID_REQUEST_DATA	-1
#define NOT_PROCESSED			 0
#define SUCCESSFULLY_PROCESSED	 1


/**
 * @brief request class
 * containes the info for a request
 */
class RA_Request : public SerializeClass
{
private:
	bool m_bSerializingRequest; /** < Keep track of whether we're serializing the request or the response */

public:
	unsigned long m_dwResponseSize; /** < response dwSize */
	unsigned long m_dwResponseChecksum; /** < checksum */
	char *m_pcResponse; /** < actual pcData for the response */

	unsigned long m_dwRequestSize; /** < request dwSize */
	unsigned long m_dwRequestChecksum; /** < request checksum */
	char *m_pcRequest; /** < actual request pcData */

	char m_cProcessOutcome; /** < outcome of the process */
	bool m_bRequestWasEncrypted; /** < true if message was encrypted, false otherwise */

	/**
     * @brief resets all values for the request invoking it
     */	
	void ResetValues();
	
	/**
     * @brief adds a action to the response
     */
	void AddActionsToResponse(class RA_Action *pRA_Action) { MYSTL_ADDTO_LIST(m_listActions, pRA_Action); }

public:

	/**
     * @brief basic constructor
     * the requestor/client calls this constructor, and adds the request to the RA_Processor
	 * the request processor will call ConvertRequestToBinary and send
	 * it will call ProcessResponse when it receives a response
     */
	RA_Request();

	/**
	 * @brief more elaborate constructor
	 * the server receiving the Request calls this constructor, and then calls ProcessRequest
	 * the server will then call ConvertResponseToBinary and send back the response
	 * if received a binary request, call ProcessRequest
	 * @param dwSize size of data to build request from
	 * @param pcData pointer to the actual data
	 */
	void CreateRequest(unsigned long dwSize,const char *pcData);

	/**
	 * virtual destructor
	 */
	virtual ~RA_Request();

	// Accessors
	long ResponseSize() { return m_dwResponseSize; } /** < returns the private member */
	long ResponseChecksum() { return m_dwResponseChecksum; } /** < returns the private member */
	long RequestSize() { return m_dwRequestSize; } /** < returns the private member */
	long RequestChecksum() { return m_dwRequestChecksum; } /** < returns the private member */
	const char *Response() { return m_pcResponse; } /** < returns the private member */
	const char *Request() { return m_pcRequest; } /** < returns the private member */

	/**
	 * @brief 
	 * @todo notcommented
	 * @param dwSize data size
	 * @param pcResponse pointer to the actual data
	 */
	void StoreResponse(long dwSize, const char *pcResponse) { m_dwResponseSize=dwSize; m_pcResponse= (char *)pcResponse; }

	/**
    * @brief for Symbian compatibility; see Symbian.txt for detailes
	* the will respond to a request with some response, and also a list of actions for the establishment
	* @todo write Symbian.txt
    */
	MYSTL_CREATE_LIST( m_listActions, RA_Action );

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID()=0; /** < should return the ID of the request */
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor)=0; /* < process the request */

	/**
    * @brief These classes have 2 sets of members, 1 for the request, and 1 for the response.  So we
	* track whether we are doing a request or a response with m_bSerializingRequest, and then call
	* SetupSerialization_Request or SetupSerialization_Response, which the derived class must implement
    */
	virtual void SetupSerialization(int iSC_Version);

	virtual void SetupSerialization_Request() {};
	virtual void SetupSerialization_Response() { StartSerializeList() + m_cProcessOutcome; };

	// *** FUNCTION THAT NEED TO BE OVERRIDDEN
	virtual void ConvertRequestToBinary(); /** < converts request to binary */
	virtual void ConvertResponseToBinary(); /** <  converts response to binary */
	virtual bool ParseResponse( unsigned long dwSize, const char *pcData ); /** < pharses the response */
};

#endif
