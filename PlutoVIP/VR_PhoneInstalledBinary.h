#ifndef VR_PhoneInstalledBinary_H
#define VR_PhoneInstalledBinary_H

#include "RA/RA_Request.h"

class VR_PhoneInstalledBinary : public RA_Request
{
	// Request Variables
	u_int64_t m_iMacAddress;
	unsigned long m_iRevision;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_PhoneInstalledBinary(u_int64_t MacAddress,unsigned long Revision);

	// The server will call this constructor, then ProcessRequest
	VR_PhoneInstalledBinary(unsigned long size,const char *data);


	virtual unsigned long ID() { return VRS_PHONE_INSTALLED_BIN; }
	

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif

