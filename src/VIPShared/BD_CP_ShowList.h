#ifndef BD_CP_ShowList_h
#define BD_CP_ShowList_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_ShowList : public BDCommand
{
	unsigned long m_x, m_y, m_Width, m_Height;
	bool m_bSendSelectedOnMove, m_bTurnOn;

#ifndef SYMBIAN
	list<string> m_DataGridList;
#else
	RPointerArray<string> m_DataGridList;
#endif //SYMBIAN

public:
	BD_CP_ShowList()  {}

#ifndef SYMBIAN
	BD_CP_ShowList(
		unsigned long x, 
		unsigned long y, 
		unsigned long Width, 
		unsigned long Height,
		list<string>  &DataGridList,
		bool		  bSendSelectedOnMove,
		bool		  bTurnOn
	);
#endif //SYMBIAN

	virtual ~BD_CP_ShowList();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size, const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SHOW_LIST; }
	virtual const char *Description() { return "Show List"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};
#endif
