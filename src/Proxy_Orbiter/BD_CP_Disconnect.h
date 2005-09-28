#ifndef BD_CP_Disconnect_h
#define BD_CP_Disconnect_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_CP_Disconnect : public BDCommand 
{
public:
	BD_CP_Disconnect()  {}
	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_DISCONNECT; }
	virtual const char *Description() { return "Disconnect"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor) { return true; }
};


#endif
