#include "VIPIncludes.h"
#include "VR_RequestSecureTransaction.h"
#include "VIPShared/PlutoConfig.h"
#include "VIPMenu.h"

VR_RequestSecureTransaction::VR_RequestSecureTransaction(unsigned long FKID_PlutoId_Establishment,unsigned long FKID_PlutoId_User) 
	: RA_Request()
{
	m_FKID_PlutoId_Establishment=FKID_PlutoId_Establishment;
	m_FKID_PlutoId_User=FKID_PlutoId_User;

	m_pKey=NULL;
	m_iSizeKey=0;
}

VR_RequestSecureTransaction::VR_RequestSecureTransaction(unsigned long size,const char *data) 
	: RA_Request(size,data) 
{
	m_pKey=NULL;
	m_iSizeKey=0;
	m_FKID_PlutoId_Establishment = Read_unsigned_long();
	m_FKID_PlutoId_User = Read_unsigned_long();
	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
}

void VR_RequestSecureTransaction::ConvertRequestToBinary()
{
	RA_Request::ConvertRequestToBinary();

	Write_unsigned_long(m_FKID_PlutoId_Establishment);
	Write_unsigned_long(m_FKID_PlutoId_User);

	m_dwRequestSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcRequest = m_pcDataBlock;
}

void VR_RequestSecureTransaction::ConvertResponseToBinary()
{
	RA_Request::ConvertResponseToBinary();
	Write_unsigned_int64(m_lMacAddress);
	Write_unsigned_long(m_iConfirmEstablishmentId);
	Write_unsigned_long(m_iSizeKey);
	Write_block(m_pKey,m_iSizeKey);

	m_dwResponseSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcResponse = m_pcDataBlock;
}


bool VR_RequestSecureTransaction::ParseResponse(unsigned long size,const char *data)
{
	// Todo - decrypt with private key
	RA_Request::ParseResponse(size,data);

	m_lMacAddress = Read_unsigned_int64();
	m_iConfirmEstablishmentId = Read_unsigned_long();
	m_iSizeKey = Read_unsigned_long();
	m_pKey = Read_block(m_iSizeKey);

	m_dwResponseSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);

	return true;
}

bool VR_RequestSecureTransaction::ProcessRequest()
{
	m_lMacAddress = 1234567890;
	m_iConfirmEstablishmentId = m_FKID_PlutoId_Establishment;
	m_iSizeKey = 100;
	m_pKey = new char[101];
	strcpy(m_pKey,"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

	return true;
}
