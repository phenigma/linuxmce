#ifndef BD_PC_ReportingVariables_h
#define BD_PC_ReportingVariables_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_PC_ReportingVariables : public BDCommand 
{
	unsigned long m_iMenuCollectionID;
	MYSTL_CREATE_LONG_MAP(m_mapVariables,VIPVariable);

public:
	BD_PC_ReportingVariables()  {}
	BD_PC_ReportingVariables(unsigned long MenuCollectionID);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_REPORTING_VARIABLES; }
	virtual const char *Description() { return "Reporting Variables"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
