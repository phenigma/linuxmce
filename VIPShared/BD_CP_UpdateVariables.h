#ifndef BD_CP_UpdateVariables_h
#define BD_CP_UpdateVariables_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_CP_UpdateVariables : public BDCommand 
{
	unsigned long m_iMenuCollectionID;
	MYSTL_CREATE_LONG_MAP(m_mapVariables,VIPVariable);

public:
	BD_CP_UpdateVariables()  {}
	BD_CP_UpdateVariables(unsigned long MenuCollectionID);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_UPDATE_VARIABLES; }
	virtual const char *Description() { return "Update Variables"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
