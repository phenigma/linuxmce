#ifndef VR_ManuallyMatchPhone_H
#define VR_ManuallyMatchPhone_H

#include "RA/RA_Request.h"
#include "PlutoVIPRequests.h"

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
	VR_ManuallyMatchPhone() {}

	virtual unsigned long ID() { return VRS_MANUALLY_MATCH_PHONE; }

	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_sBluetoothID + m_sIdentifiedPlutoIdPin
			+ m_iMacAddress + m_iIdentifiedPlutoId + m_iEstablishmentID;
	}
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif
