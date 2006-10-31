#ifndef BD_CP_MenuData_h
#define BD_CP_MenuData_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

#if defined(SYMBIAN) || defined(VIPESTABLISHMENT)
	class MenuData { /*not implemented!*/ };
#else
	#include "MenuData.h"
#endif


/*
 *
 *	BD_CP_MenuData command - sends menu items and drawing info for local rendered menus
 *
 */
class BD_CP_MenuData : public BDCommand
{
protected:
	MenuData m_Data;
public:
	BD_CP_MenuData()  {}

	BD_CP_MenuData( MenuData& data );

	virtual ~BD_CP_MenuData();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size, const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_MENU_DATA; }
	virtual const char *Description() { return "Menu Data"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};
#endif
