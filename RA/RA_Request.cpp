/**
 *
 * @file RA_Request.cpp
 * @brief source file for the request class
 * @author
 * @todo notcommented
 *
 */


#include "VIPShared/VIPIncludes.h"
#include "RA_Request.h"
#include "RA_Action.h"

RA_Request::RA_Request()
{
	ResetValues();
}

RA_Request::~RA_Request()
{
	// free data allocated on the heap

	if( m_pcResponse )
		delete[] m_pcResponse;

	if( m_pcRequest )
		delete[] m_pcRequest;

	MYSTL_ITERATE_LIST(m_listActions,RA_Action,pAction,it)
	{
 		delete pAction;
	}
}

void RA_Request::ResetValues()
{
	// common sense initial values

	m_cProcessOutcome = NOT_PROCESSED;
	m_dwResponseSize = m_dwResponseChecksum = 0;
	m_pcResponse = m_pcRequest = NULL;
	m_dwRequestChecksum = m_dwRequestSize = 0;
}

RA_Request::RA_Request(unsigned long dwSize, const char *pcData)
{
	m_bSerializingRequest=true;
	ResetValues();

	SerializeRead(dwSize,(char *) pcData);
	m_bRequestWasEncrypted = false;
}

void RA_Request::ConvertRequestToBinary()
{
	m_bSerializingRequest=true;
	SerializeWrite();
	m_dwRequestSize = (unsigned long)(m_pcCurrentPosition - m_pcDataBlock);
	m_pcRequest = m_pcDataBlock;
}

void RA_Request::ConvertResponseToBinary()
{
	m_bSerializingRequest=false;
	SerializeWrite();

	m_dwResponseSize = (unsigned long) (m_pcCurrentPosition - m_pcDataBlock);
	m_pcResponse = m_pcDataBlock;
}

bool RA_Request::ParseResponse(unsigned long dwSize, const char *pcData)
{
	m_bSerializingRequest=false;
	SerializeRead(dwSize, (char *)pcData);
	m_dwResponseSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	return true;
}
