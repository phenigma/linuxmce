#include "VIPShared/VIPIncludes.h"
#include "VIPShared/PlutoConfig.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "VR_PhoneInstalledBinary.h"
#include "VA_UpdateTransaction.h"
#include "VR_ShowMenu.h"
#include "VIPShared/VIPMenu.h"
#include <iostream>
#include <sstream>

VR_PhoneInstalledBinary::VR_PhoneInstalledBinary(u_int64_t MacAddress,unsigned long Revision)
	: RA_Request()
{
	m_iMacAddress=MacAddress;
	m_iRevision=Revision;
}


bool VR_PhoneInstalledBinary::ProcessRequest(class RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER
	std::ostringstream s;
	s << "UPDATE MacAddress SET Revision=" << m_iRevision << " WHERE PKID_MacAddress=" << m_iMacAddress;
//	xx

#endif
	return true;
}
