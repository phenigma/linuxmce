#ifndef BD_CP_SendMeKeystrokes_h
#define BD_CP_SendMeKeystrokes_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_SendMeKeystrokes : public BDCommand 
{
	char m_YesNo;

public:
	BD_CP_SendMeKeystrokes()  {}
	BD_CP_SendMeKeystrokes(char YesNo);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SEND_ME_KEYSTROKES; }
	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
	virtual const char *Description() { return "Send me keystrokes"; };
};

#endif
