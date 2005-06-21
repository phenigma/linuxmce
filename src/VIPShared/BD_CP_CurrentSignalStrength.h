#ifndef BD_CP_CurrentSignalStrength_h
#define BD_CP_CurrentSignalStrength_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_CurrentSignalStrength : public BDCommand 
{
	int m_iSignalStrength;

public:
	BD_CP_CurrentSignalStrength()  {}
	BD_CP_CurrentSignalStrength(int iSignalStrength);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_CURRENT_SIGNAL_STRENGTH; }
	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
	virtual const char *Description() { return "Current signal strength"; };
};

#endif
