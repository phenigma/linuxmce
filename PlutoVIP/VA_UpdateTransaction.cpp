#include "VIPIncludes.h"
#include "../VIPShared/VA_UpdateTransaction.h"
#include "../VIPShared/VIPMenu.h"
#include "../VIPShared/PlutoConfig.h"

// todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block?
VA_UpdateTransaction::VA_UpdateTransaction(unsigned long TransNumber,unsigned long Status,string Message) 
	: RA_Action()
{ 
	m_iTransNumber=TransNumber; m_iStatus=Status;
	m_sMessage=Message;
}

VA_UpdateTransaction::VA_UpdateTransaction(unsigned long size,const char *data)
	: RA_Action(size,data)
{
	m_iTransNumber = Read_unsigned_long();
	m_iStatus = Read_unsigned_long();
	Read_string(m_sMessage);

	m_dwActionSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
}

void VA_UpdateTransaction::ConvertActionToBinary()
{
	RA_Action::ConvertActionToBinary();
	Write_unsigned_long(m_iTransNumber);
	Write_unsigned_long(m_iStatus);
	Write_string(m_sMessage);

	m_dwActionSize = (unsigned long) (m_pcCurrentPosition-m_pcDataBlock);
	m_pcAction = m_pcDataBlock;

}

void VA_UpdateTransaction::ProcessAction()
{
	printf("Updated transaction: %d to %d %s\n",m_iTransNumber,m_iStatus,m_sMessage.c_str());
}

