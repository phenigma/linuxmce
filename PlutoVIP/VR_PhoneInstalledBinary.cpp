#include "VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "VR_PhoneInstalledBinary.h"
#include "VA_UpdateTransaction.h"
#include "VR_ShowMenu.h"
#include "VIPMenu.h"
#include <iostream>
#include <sstream>

VR_PhoneInstalledBinary::VR_PhoneInstalledBinary(u_int64_t MacAddress,unsigned long Revision)
	: RA_Request()
{
	m_iMacAddress=MacAddress;
	m_iRevision=Revision;
}

VR_PhoneInstalledBinary::VR_PhoneInstalledBinary(unsigned long size,const char *data) 
	: RA_Request(size,data) 
{
	m_iMacAddress = Read_int64();
	m_iRevision = Read_unsigned_long();

	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
}

void VR_PhoneInstalledBinary::ConvertRequestToBinary()
{
	RA_Request::ConvertRequestToBinary();

	Write_int64(m_iMacAddress);
	Write_unsigned_long(m_iRevision);

	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcRequest = m_pcDataBlock;
}

void VR_PhoneInstalledBinary::ConvertResponseToBinary()
{
	RA_Request::ConvertResponseToBinary();
}


bool VR_PhoneInstalledBinary::ParseResponse(unsigned long size,const char *data)
{
	RA_Request::ParseResponse(size,data);
	return true;
}

bool VR_PhoneInstalledBinary::ProcessRequest()
{
#ifdef VIPSERVER
	std::ostringstream s;
	s << "UPDATE MacAddress SET Revision=" << m_iRevision << " WHERE PKID_MacAddress=" << m_iMacAddress;
//	xx

#endif
	return true;
}
