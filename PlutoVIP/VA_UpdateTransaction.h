#ifndef VA_UpdateTransaction_H
#define VA_UpdateTransaction_H

#include "RA/RA_Action.h"

class VA_UpdateTransaction : public RA_Action
{
public:
	unsigned long m_iTransNumber,m_iStatus;
	string m_sMessage;

	// For an incoming menu
	VA_UpdateTransaction(unsigned long size,const char *data);

	VA_UpdateTransaction(unsigned long TransNumber,unsigned long Status,string Message);

	unsigned long ID() { return ACTION_UPDATETRANSACTION; }
	void ConvertActionToBinary();
	virtual void ProcessAction();
};


#endif

