#ifndef VA_ShowMenu_H
#define VA_ShowMenu_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

class VA_ForwardRequestToPhone : public RA_Action
{
public:
	PlutoDataBlock m_pdbOriginalRequest;
	char m_iReturnRequest;
	unsigned long m_iRequestID;
	u_int64_t m_iMacAddress;

	VA_ForwardRequestToPhone(unsigned char iReturnRequest, class RA_Request *pRequest,u_int64_t MacAddress);
	VA_ForwardRequestToPhone() {}

	unsigned long ID() { return ACTION_FORWARDREQUEST_TOPHONE; }
	virtual void SetupSerialization()
	{
		RA_Action::SetupSerialization();
		StartSerializeList() + m_pdbOriginalRequest + m_iReturnRequest
			+ m_iRequestID + m_iMacAddress;

	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

