#ifndef VA_UpdateTransaction_H
#define VA_UpdateTransaction_H

#include "RA/RA_Action.h"
#include "PlutoVIPRequests.h"

class VA_UpdateTransaction : public RA_Action
{
public:
	unsigned long m_iTransNumber,m_iStatus;
	string m_sMessage;

	VA_UpdateTransaction(unsigned long TransNumber,unsigned long Status,string Message);
	VA_UpdateTransaction() {}

	unsigned long ID() { return ACTION_UPDATETRANSACTION; }
	virtual void SetupSerialization()
	{
		RA_Action::SetupSerialization();
		StartSerializeList() + m_iTransNumber + m_iStatus
			+ m_sMessage;
	}

    virtual void ProcessAction(class RA_Request *pRequest,class RA_Processor *pRA_Processor);
};


#endif

