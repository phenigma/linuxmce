#ifndef VR_PhoneRespondedToRequest_H
#define VR_PhoneRespondedToRequest_H

#include "RA/RA_Request.h"

class VR_PhoneRespondedToRequest : public RA_Request
{
	// Request Variables
	unsigned long m_iRequestID;
	int m_iOriginalRequestSize,m_iOriginalResponseSize;
	char *m_pOriginalRequest,*m_pOriginalResponse;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_PhoneRespondedToRequest(unsigned long RequestID,unsigned long RequestSize,const char *RequestData,unsigned long ResponseSize,const char *ResponseData);

	// The server will call this constructor, then ProcessRequest
	VR_PhoneRespondedToRequest(unsigned long size,const char *data);


	virtual unsigned long ID() { return VRS_PHONE_RESPONDED; }
	

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif

