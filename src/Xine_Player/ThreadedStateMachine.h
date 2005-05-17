/*
	ThreadedStateMachine

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef THREADEDSTATEMACHINE_H
#define THREADEDSTATEMACHINE_H

#include "PlutoUtils/MultiThreadIncludes.h"

class ThreadedStateMachine
{
protected:
	class State
	{
		string m_strName;
	public:
		State(string name);

		string getName();
		bool operator==(const State& other) const;
		bool operator<(const State& other) const;
	};

	typedef void (ThreadedStateMachine::*MachineTransitionFunction)(State &fromState, State &toState);

private:
	pthread_t runningThread;

	pthread_cond_t			m_machineWaitCondition;
	pluto_pthread_mutex_t 	m_machineWaitMutex;
	pluto_pthread_mutex_t 	m_desiredStatesMutex;

	static void *threadMachineLoop(void *parameters);

	map<pair<State*, State*>, MachineTransitionFunction> m_stateMatrix;
	map<State*, int> m_finalStates;

	State *m_pStateCurrent;

	list<State*> m_desiredStatesList;

	bool m_bMachineSignalled;
	bool m_bMachineIsWaiting;
	bool m_bNeedToQuit;

protected:
	State STATE_START;
	State STATE_WAITING;
	State STATE_QUIT;

	virtual void machineLoop();

	virtual void waitConditionsChange();
	virtual void signalConditionsChange();

	virtual bool machineIsWaiting();

	virtual void emptyTransition(State &pSourceState, State &pTargetState);
	virtual State *findNextState(State *pCurrentState) = 0;

	bool getNeedToQuit();
	void setNeedToQuit(bool bNeedToQuit);

	State& getCurrentState();
public:

	ThreadedStateMachine();

    virtual ~ThreadedStateMachine();

	virtual void addTransition(ThreadedStateMachine::State &sourceState, ThreadedStateMachine::State &targetState, MachineTransitionFunction transition);
	virtual void setFinalState(ThreadedStateMachine::State &finalState);
	virtual void setInitialState(ThreadedStateMachine::State &initialState);

	virtual void startMachine();
};

#endif
