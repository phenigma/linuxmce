#ifndef VR_PhoneInstalledBinary_H
#define VR_PhoneInstalledBinary_H

#include "RA/RA_Request.h"
#include "PlutoVIPrequests.h"

class VR_PhoneInstalledBinary : public RA_Request
{
	// Request Variables
	u_int64_t m_iMacAddress;
	unsigned long m_iRevision;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_PhoneInstalledBinary(u_int64_t MacAddress,unsigned long Revision);
	VR_PhoneInstalledBinary() {}

	virtual unsigned long ID() { return VRS_PHONE_INSTALLED_BIN; }
	
	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_iMacAddress + m_iRevision;
	}
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

