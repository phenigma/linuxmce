/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef VR_PhoneInstalledBinary_H
#define VR_PhoneInstalledBinary_H

#include "RA/RA_Request.h"
#include "PlutoVIPRequests.h"

class VR_PhoneInstalledBinary : public RA_Request
{
	// Request Variables
	unsigned long long m_iMacAddress;
	unsigned long m_iRevision;

	// Response Variables

public:
	// The establishment will call this constructor, then ConvertRequestToBinary
	VR_PhoneInstalledBinary(unsigned long long MacAddress,unsigned long Revision);
	VR_PhoneInstalledBinary() {}

	virtual uint32_t ID() { return VRS_PHONE_INSTALLED_BIN; }
	
	virtual void SetupSerialization_Request()
	{
		RA_Request::SetupSerialization_Request();
		StartSerializeList() + m_iMacAddress + m_iRevision;
	}
	virtual bool ProcessRequest(class RA_Processor *pRA_Processor);
};


#endif

