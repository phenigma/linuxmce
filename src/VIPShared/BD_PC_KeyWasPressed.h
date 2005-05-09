#ifndef BD_PC_KeyWasPressed_h
#define BD_PC_KeyWasPressed_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

//enum EventType { eButtonDown, eButtonUp };

class BD_PC_KeyWasPressed : public BDCommand 
{
	int m_Key;
    int m_EventType; // 0 - ButtonDown, 1 - ButtonUp

public:
	BD_PC_KeyWasPressed()  {}
	BD_PC_KeyWasPressed(int Key, int EventType);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_KEY_WAS_PRESSED; }
	virtual const char *Description() { return "Key pressed"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
