#ifndef BD_PC_KeyWasPressed_h
#define BD_PC_KeyWasPressed_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_KeyWasPressed : public BDCommand 
{
	int m_Key;

public:
	BD_PC_KeyWasPressed()  {}
	BD_PC_KeyWasPressed(int Key);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_KEY_WAS_PRESSED; }
	virtual const char *Description() { return "Key pressed"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
