#ifndef BD_PC_Disconnect_h
#define BD_PC_Disconnect_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_Disconnect : public BDCommand 
{
public:
	BD_PC_Disconnect()  {}

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_DISCONNECT; }
	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
	virtual const char *Description() { return "Disconnect"; };
};


#endif
