#ifndef VR_RequestSecureTransaction_H
#define VR_RequestSecureTransaction_H

#include "RA/RA_Request.h"

class VR_RequestSecureTransaction : public RA_Request
{
	// Request Variables
	unsigned long m_FKID_PlutoId_Establishment,m_FKID_PlutoId_User;

	// Response Variables
	u_int64_t m_lMacAddress;
	unsigned long m_iConfirmEstablishmentId;
	unsigned long m_iSizeKey;
	char *m_pKey;

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_RequestSecureTransaction(unsigned long FKID_PlutoId_Establishment,unsigned long FKID_PlutoId_User);

	// The server will call this constructor, then ProcessRequest
	VR_RequestSecureTransaction(unsigned long size,const char *data);


	virtual unsigned long ID() { return VRP_REQUEST_SEC_TRANS; }
	

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif

