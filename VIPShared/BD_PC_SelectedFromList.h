#ifndef BD_PC_SelectedFromList_h
#define BD_PC_SelectedFromList_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_SelectedFromList : public BDCommand 
{
	unsigned long m_uItemIndex;

public:
	BD_PC_SelectedFromList()  {}
	BD_PC_SelectedFromList(unsigned long uItemIndex);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_SELECTED_FROM_LIST; }
	virtual const char *Description() { return "Selected from list"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
