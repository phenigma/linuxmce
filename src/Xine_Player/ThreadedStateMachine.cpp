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
#include "ThreadedStateMachine.h"
#include "DCE/Logger.h"

using namespace DCE;

ThreadedStateMachine::ThreadedStateMachine()
	: m_machineWaitMutex("mutex-state-machine-wait"), m_desiredStatesMutex("mutex-desired-states-list")
		, m_bMachineSignalled(false), m_bNeedToQuit(false)
		, STATE_START("start"), STATE_WAITING("waiting"), STATE_QUIT("quit")
{
	m_pStateCurrent = &STATE_START;

	pthread_mutexattr_t      mutexAttr;
    pthread_mutexattr_init( &mutexAttr );
    pthread_mutexattr_settype( &mutexAttr,  PTHREAD_MUTEX_RECURSIVE_NP );

	pthread_cond_init( &m_machineWaitCondition, NULL );
	m_machineWaitMutex.Init( &mutexAttr, &m_machineWaitCondition );

	m_desiredStatesMutex.Init( &mutexAttr );

	setFinalState(STATE_QUIT);
}

ThreadedStateMachine::~ThreadedStateMachine()
{
	setNeedToQuit(true);
	pthread_join(runningThread, NULL);
}

bool ThreadedStateMachine::getNeedToQuit()
{
	return m_bNeedToQuit;
}

void ThreadedStateMachine::setNeedToQuit(bool bNeedToQuit)
{
	m_bNeedToQuit = bNeedToQuit;
}

void ThreadedStateMachine::setInitialState(ThreadedStateMachine::State &initialState)
{
	m_pStateCurrent = &initialState;
}

void ThreadedStateMachine::startMachine()
{
	pthread_create(&runningThread, NULL, threadMachineLoop, this);
}

void ThreadedStateMachine::signalConditionsChange()
{
	g_pPlutoLogger->Write(LV_STATUS, "ThreadedStateMachine::signalConditionsChange() signalling need to transition!");
	PLUTO_SAFETY_LOCK(waitTransitionLock, m_machineWaitMutex);
	m_bMachineSignalled = true;
	waitTransitionLock.Release();
	pthread_cond_signal(&m_machineWaitCondition);
}

void ThreadedStateMachine::waitConditionsChange()
{
	g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::waitConditionsChange() (\"%s\") waiting !", pthread_self(), m_pStateCurrent->getName().c_str());

	PLUTO_SAFETY_LOCK(waitTransitionLock, m_machineWaitMutex);
	m_bMachineSignalled = false;
	while ( ! m_bMachineSignalled )
	{
		g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::waitConditionsChange() calling conditional wait!", pthread_self());
		waitTransitionLock.CondWait();
// 		g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::waitConditionsChange() exit conditional wait!", pthread_self());
	}
	m_bMachineSignalled = false;
// 	g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::waitConditionsChange() waiting is over!", pthread_self());
}

void ThreadedStateMachine::emptyTransition(State &pSourceState, State &pTargetState)
{

}

ThreadedStateMachine::State& ThreadedStateMachine::getCurrentState()
{
	return *m_pStateCurrent;
}

void ThreadedStateMachine::machineLoop()
{
	while ( m_finalStates.find(m_pStateCurrent) == m_finalStates.end() )
	{
		State *pStateTarget = findNextState(m_pStateCurrent);

		if ( pStateTarget != &STATE_WAITING )
			g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::machineLoop() Current state: \"%s\" Next state: \"%s\"",
					pthread_self(),
					m_pStateCurrent->getName().c_str(),
					pStateTarget->getName().c_str());

// 		if ( pStateTarget != &STATE_WAITING )
// 			g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::machineLoop() Before transition %s -> %s!",
// 					pthread_self(), m_pStateCurrent->getName().c_str(), pStateTarget->getName().c_str());

		if ( m_stateMatrix.find(make_pair<State*, State*>(m_pStateCurrent, pStateTarget)) != m_stateMatrix.end() )
		{
			g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::machineLoop() Invoking transition function: %s -> %s !",
					pthread_self(), m_pStateCurrent->getName().c_str(), pStateTarget->getName().c_str());

			MachineTransitionFunction transitionFunction = m_stateMatrix[make_pair<State*, State*>(m_pStateCurrent, pStateTarget)];
			(this->*transitionFunction)(*m_pStateCurrent, *pStateTarget);
			m_pStateCurrent = pStateTarget;
		}
		else
		{
			waitConditionsChange();
		}

// 		if ( pStateTarget != &STATE_WAITING )
// 			g_pPlutoLogger->Write(LV_STATUS, "0x%x ThreadedStateMachine::machineLoop() After transition. %s!", pthread_self(), m_pStateCurrent->getName().c_str());
	}
}

void ThreadedStateMachine::setFinalState(ThreadedStateMachine::State &finalState)
{
	g_pPlutoLogger->Write(LV_STATUS, "0x%x Setting %s as final", pthread_self(), finalState.getName().c_str());
	m_finalStates[&finalState] = 1;
}

void ThreadedStateMachine::addTransition(ThreadedStateMachine::State &sourceState, ThreadedStateMachine::State &targetState, MachineTransitionFunction transition)
{
	g_pPlutoLogger->Write(LV_STATUS, "0x%x Adding transition: %s --> %s", pthread_self(), sourceState.getName().c_str(), targetState.getName().c_str());
	m_stateMatrix[make_pair<State*, State*>(&sourceState, &targetState)] = transition;
}

void *ThreadedStateMachine::threadMachineLoop(void *parameters)
{
	ThreadedStateMachine *pStateMachine = static_cast<ThreadedStateMachine*>(parameters);

	pStateMachine->machineLoop();

	return NULL;
}

ThreadedStateMachine::State::State(string name)
	: m_strName(name)
{
}

string ThreadedStateMachine::State::getName()
{
	return m_strName;
}

bool ThreadedStateMachine::State::operator==(const ThreadedStateMachine::State& other) const
{
	return this == &other;
}

bool ThreadedStateMachine::State::operator<(const ThreadedStateMachine::State& other) const
{
	return this < &other;
}
