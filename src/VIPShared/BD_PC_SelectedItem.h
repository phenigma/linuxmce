#ifndef BD_PC_SelectedItem_h
#define BD_PC_SelectedItem_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"


/*
 *
 *	BD_PC_SelectedItem command - sends selected item in local rendered menus
 *
 */
class BD_PC_SelectedItem : public BDCommand 
{
	string m_sItem;

public:
	BD_PC_SelectedItem()  {}
	BD_PC_SelectedItem(string& sItem);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_ITEM_SELECTED; }
	virtual const char *Description() { return "Menu item selected"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
