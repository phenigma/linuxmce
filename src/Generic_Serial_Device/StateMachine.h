/***************************************************************************
 *   Copyright (C) 2005 by Igor Spac,,,                                    *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
		State() : psm_(0) {};
		State(StateMachine* psm) : psm_(psm) {};

	protected:
		virtual void handleEnter() 
		{};
		virtual void handleExit()
		{};
	private:
		StateMachine* psm_;
	};

	template <class T = StateMachine>
	class StateImpl : public State {
	public:
		StateImpl(T* psm)
		: State(psm) {};
		
	public:
		T* getSM() {
			return (T)psm_;
		}
	};
		
public:
	State* state_;
};

};

#endif
