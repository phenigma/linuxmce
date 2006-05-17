/*
	ThreadedStateMachine::State

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "ThreadedStateMachine.h"

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
