#include "VIPShared/VIPIncludes.h"
#include "VA_UpdateTransaction.h"
#include "VIPShared/VIPMenu.h"
#include "VIPShared/PlutoConfig.h"

// todo - is there a cross platform (linux/symbian/windows) way to dynamically allocate a growable memory block?
VA_UpdateTransaction::VA_UpdateTransaction(unsigned long TransNumber,unsigned long Status,string Message) 
	: RA_Action()
{ 
	m_iTransNumber=TransNumber; m_iStatus=Status;
	m_sMessage=Message;
}

void VA_UpdateTransaction::ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor)
{
	printf("Updated transaction: %d to %d %s\n",m_iTransNumber,m_iStatus,m_sMessage.c_str());
}

