#ifndef BD_PC_GetSignalStrength_h
#define BD_PC_GetSignalStrength_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_GetSignalStrength : public BDCommand 
{
public:
	BD_PC_GetSignalStrength();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_GET_SIGNAL_STRENGTH; }
	virtual const char *Description() { return "Set Signal Strength"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
