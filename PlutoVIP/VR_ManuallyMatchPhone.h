#ifndef VR_ManuallyMatchPhone_H
#define VR_ManuallyMatchPhone_H

#include "RA/RA_Request.h"

class VR_ManuallyMatchPhone : public RA_Request
{
	// Request Variables
	string m_sBluetoothID,m_sIdentifiedPlutoIdPin;
	u_int64_t m_iMacAddress;
	unsigned long m_iIdentifiedPlutoId;
	unsigned long m_iEstablishmentID;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_ManuallyMatchPhone(unsigned long EstablishmentID,
		string sBluetooth,u_int64_t MacAddress,unsigned long IdentifiedPlutoId,string IdentiedPlutoIdPin);

	// The server will call this constructor, then ProcessRequest
	VR_ManuallyMatchPhone(unsigned long size,const char *data);

	virtual unsigned long ID() { return VRS_MANUALLY_MATCH_PHONE; }
	

	virtual bool ProcessRequest();
	virtual bool ParseResponse(unsigned long size,const char *data);

	// These call the base class and then add their output
	virtual void ConvertRequestToBinary();
	virtual void ConvertResponseToBinary();
};


#endif
