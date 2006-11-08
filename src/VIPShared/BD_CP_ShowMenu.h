#ifndef BD_CP_ShowMenu_h
#define BD_CP_ShowMenu_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_ShowMenu : public BDCommand
{
    long m_nCrtRoom;

public:
	BD_CP_ShowMenu()  {m_nCrtRoom = 0;}
	BD_CP_ShowMenu(long nCrtRoom);

	~BD_CP_ShowMenu();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SHOW_MENU; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Show Menu"; };
};
#endif
