/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCELOOPSTATEMACHINE_H
#define DCELOOPSTATEMACHINE_H

#include "StateMachine.h"
#include "IOThread.h"
#include "IOMutex.h"
#include "IOEvent.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class LoopStateMachine : public StateMachine {
public:
    LoopStateMachine(bool usemain = false);
    virtual ~LoopStateMachine();

public:
	void setIdleDelay(unsigned idledelay) {
		idledelay_ = idledelay;
	}
	unsigned getIdleDelay() {
		return idledelay_;
	}
	
		
public:
	virtual bool handleStartup();
	virtual bool handleIteration();
	virtual void handleTerminate();
	
private:
	unsigned idledelay_;
	struct timespec lastidle_;
	
/*
	//thread callback
protected:
	virtual bool handleStartup();
	virtual void handleTerminate();
	virtual void* _Run();
*/
	/*states*/	
protected:
	class LoopState : public State {
		friend class LoopStateMachine;
	protected:
		LoopState(LoopStateMachine* psm) 
			: State(psm) {};
		virtual void handleStart()
		{};
		virtual void handleIdle()
		{};
		virtual void handleStop()
		{};
	};
};

};

#endif
