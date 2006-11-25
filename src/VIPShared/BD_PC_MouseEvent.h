#ifndef BD_PC_MouseEvent_h
#define BD_PC_MouseEvent_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_PC_MouseEvent : public BDCommand 
{
public:
	typedef enum { meMove=0, meStylusDown, meStylusUp } MouseEventType;
private:
	int m_iX, m_iY;
    MouseEventType m_EventType; 

public:
	BD_PC_MouseEvent()  {}
	BD_PC_MouseEvent( int iX, int iY, MouseEventType EventType );

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_PC_MOUSE_EVENT; }
	virtual const char *Description() { return "Mouse event"; };

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);
};

#endif
