#ifndef VA_ShowMenu_H
#define VA_ShowMenu_H

#include "RA/RA_Action.h"

class VA_ForwardRequestToPhone : public RA_Action
{
public:
	char *m_pOriginalRequest;
	int m_iOriginalRequestSize;
	unsigned char m_iReturnRequest;
	unsigned long m_iRequestID;
	u_int64_t m_iMacAddress;

	// For an incoming menu
	VA_ForwardRequestToPhone(unsigned long size,const char *data);

	VA_ForwardRequestToPhone(unsigned char iReturnRequest, class RA_Request *pRequest,u_int64_t MacAddress);

	unsigned long ID() { return ACTION_FORWARDREQUEST_TOPHONE; }
	void ConvertActionToBinary();
	virtual void ProcessAction();
};


#endif

