#ifndef BD_PC_VMCHidden_h
#define BD_PC_VMCHidden_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_VMCHidden : public BDCommand 
{
	unsigned long m_iMenuCollectionID;

public:
	BD_PC_VMCHidden()  {}
	BD_PC_VMCHidden(unsigned long MenuCollectionID);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_VMC_HIDDEN; }
	virtual const char *Description() { return "VMC Hidden"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
