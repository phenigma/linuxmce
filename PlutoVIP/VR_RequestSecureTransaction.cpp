#include "VIPShared/VIPIncludes.h"
#include "VR_RequestSecureTransaction.h"
#include "VIPShared/PlutoConfig.h"
#include "VIPShared/VIPMenu.h"

VR_RequestSecureTransaction::VR_RequestSecureTransaction(unsigned long FKID_PlutoId_Establishment,unsigned long FKID_PlutoId_User) 
	: RA_Request()
{
	m_FKID_PlutoId_Establishment=FKID_PlutoId_Establishment;
	m_FKID_PlutoId_User=FKID_PlutoId_User;
}

bool VR_RequestSecureTransaction::ProcessRequest(class RA_Processor *pRA_Processor)
{
	m_lMacAddress = 1234567890;
	m_iConfirmEstablishmentId = m_FKID_PlutoId_Establishment;
	m_pdbKey.m_dwSize = 100;
	m_pdbKey.m_pBlock = new char[101];
	strcpy(m_pdbKey.m_pBlock,"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

	return true;
}
