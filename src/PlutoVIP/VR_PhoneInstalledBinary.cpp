#include "VIPShared/VIPIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "VR_PhoneInstalledBinary.h"
#include "VA_UpdateTransaction.h"
#include "VIPShared/VIPMenu.h"

#include "RA/RA_Processor.h"
#include "DCE/DCEMySqlConfig.h"

#include <assert.h>
#include <iostream>
#include <sstream>

VR_PhoneInstalledBinary::VR_PhoneInstalledBinary(u_int64_t MacAddress,unsigned long Revision)
	: RA_Request()
{
	m_iMacAddress=MacAddress;
	m_iRevision=Revision;
}


bool VR_PhoneInstalledBinary::ProcessRequest(RA_Processor *pRA_Processor)
{
#ifdef VIPSERVER

	DCEMySqlConfig *pDCEMySqlConfig = dynamic_cast<DCEMySqlConfig *>(pRA_Processor->m_pRA_Config);
	assert(NULL != pDCEMySqlConfig);

	std::ostringstream s;
	s << "UPDATE MacAddress SET Revision=" << m_iRevision << " WHERE PKID_MacAddress=" << m_iMacAddress;

	//TODO ... ?!

#endif
	return true;
}
