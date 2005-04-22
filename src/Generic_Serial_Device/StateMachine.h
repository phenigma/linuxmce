/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCESTATEMACHINE_H
#define DCESTATEMACHINE_H

namespace DCE {

/**
@author Igor Spac,,,
*/
class StateMachine {
public:
    StateMachine();
    virtual ~StateMachine();

	/*state access*/
protected:
	class State;
	
	void setState(State* pstate);
	State* getState();
	
	/*states*/
protected:
	class State {
		friend class StateMachine;
		friend class StateImpl;
	protected:
		State(StateMachine* psm) : psm_(psm) {};
		
		StateMachine* getSM() {
			return psm_;
		}

	protected:
		virtual void handleEnter() 
		{};
		virtual void handleExit()
		{};
	private:
		StateMachine* psm_;
	};
	
public:
	State* state_;
};

};

#endif
