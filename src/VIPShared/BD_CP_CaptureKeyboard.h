#ifndef BD_CP_CaptureKeyboard_h
#define BD_CP_CaptureKeyboard_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_CaptureKeyboard : public BDCommand 
{
	bool m_bOnOff;
	bool m_bReset;
	int m_iEditType;
	bool m_bDataGrid;
	int  m_iVariable;
	string m_sText;

public:
	BD_CP_CaptureKeyboard()  {}
	BD_CP_CaptureKeyboard(
		bool bOnOff, bool bDataGrid, bool bReset, int iEditType, 
		int iVariable, string sText
	);

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_CAPTURE_KEYBOARD; }
	virtual const char *Description() { return "Capture Keyboard"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
