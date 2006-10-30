#ifndef BD_PC_Configure_h
#define BD_PC_Configure_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"
#include "../SerializeClass/SerializeClass.h"
#include "PhoneConfig.h"

/*
 *
 *	BD_PC_Configure command - sends current phone configuration
 *
 */
class BD_PC_Configure : public BDCommand 
{
protected:	
	PhoneConfig m_Cfg;
public:
	BD_PC_Configure()  {}
	BD_PC_Configure( PhoneConfig& cfg );

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_CONFIGURE; }
	virtual const char *Description() { return "Configure"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
