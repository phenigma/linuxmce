#ifndef BD_CP_ShowVMC_h
#define BD_CP_ShowVMC_h

#include "BD/BDCommand.h"
#include "PlutoPhoneCommands.h"

class BD_CP_ShowVMC : public BDCommand
{
	char *m_pVMC;
	unsigned char m_iStore;
	unsigned long m_iVMCSize;

public:
	BD_CP_ShowVMC()  {}
	BD_CP_ShowVMC(unsigned char Store,unsigned long VMCSize,const char *pVMC);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SHOW_VMC; }
	virtual const char *Description() { return "Show VMC"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};
#endif
