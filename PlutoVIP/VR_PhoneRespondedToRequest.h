#ifndef VR_PhoneRespondedToRequest_H
#define VR_PhoneRespondedToRequest_H

#include "RA/RA_Request.h"
#include "PlutoVIPrequests.h"

class VR_PhoneRespondedToRequest : public RA_Request
{
	// Request Variables
	unsigned long m_iRequestID;
	PlutoDataBlock m_pdbOriginalRequest,m_pdbOriginalResponse;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_PhoneRespondedToRequest(unsigned long RequestID,unsigned long RequestSize,const char *RequestData,unsigned long ResponseSize,const char *ResponseData);
	VR_PhoneRespondedToRequest() {}

	virtual unsigned long ID() { return VRS_PHONE_RESPONDED; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_iRequestID + m_pdbOriginalRequest + m_pdbOriginalResponse;
	}
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

