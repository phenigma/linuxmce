#ifndef BD_PC_ReportMyVersion_h
#define BD_PC_ReportMyVersion_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_ReportMyVersion : public BDCommand
{
	string m_sVersion;

public:
	BD_PC_ReportMyVersion()  {}
	BD_PC_ReportMyVersion(string sVersion);

	~BD_PC_ReportMyVersion();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_REPORT_MY_VERSION; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Report My Version"; };
};
#endif
