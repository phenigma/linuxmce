#if !defined(__BD_CP_SIMULATEEVENT_H__)
#define __BD_CP_SIMULATEEVENT_H__

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_SimulateEvent : public BDCommand
{
	long m_EventType; // reserved
	long m_key;

public:
	BD_CP_SimulateEvent()  
	{
		m_EventType = 0;
	}

	BD_CP_SimulateEvent(
		long EventType,
		long key
	);

	~BD_CP_SimulateEvent();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SIMULATE_EVENT; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Simulate event"; };
};

#endif //__BD_CP_SIMULATEEVENT_H__