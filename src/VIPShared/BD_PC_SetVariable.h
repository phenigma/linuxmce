#ifndef BD_PC_SetVariable_h
#define BD_PC_SetVariable_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_SetVariable : public BDCommand 
{
	string m_sVariableValue;
	int	   m_iVariable;

public:
	BD_PC_SetVariable()  {}
	BD_PC_SetVariable(int iVariable, string sVariableValue);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_SET_VARIABLE; }
	virtual const char *Description() { return "Set Variable"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
