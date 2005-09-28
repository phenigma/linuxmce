#ifndef BD_PC_SetImageQuality_h
#define BD_PC_SetImageQuality_h

#include "BD/BDCommand.h"
#include "Proxy_Orbiter_Commands.h"

class BD_PC_SetImageQuality : public BDCommand 
{
public:

    unsigned long m_nImageQuality;

    BD_PC_SetImageQuality() {m_nImageQuality = 0;}
	BD_PC_SetImageQuality(unsigned long nImageQuality);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_SET_IMAGE_QUALITY; }
	virtual const char *Description() { return "Set Image Quality"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
