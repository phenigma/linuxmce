#ifndef BD_PC_OrderPlaced_h
#define BD_PC_OrderPlaced_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_OrderPlaced : public BDCommand 
{
	unsigned long m_iMenuCollectionID;
	MYSTL_CREATE_LIST(m_listBasket, BasketItem); 

public:
	BD_PC_OrderPlaced()  {}
	BD_PC_OrderPlaced(unsigned long MenuCollectionID);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_ORDER_PLACED; }
	virtual const char *Description() { return "Order Placed"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
