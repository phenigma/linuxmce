//
// C++ Interface: statemachine
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSSTATEMACHINE_H
#define EIBBUSSTATEMACHINE_H

#include <list>

namespace EIBBUS {

#define MIN_DEPTH		1
#define DEF_DEPTH		10

/**
@author 
*/
class StateMachine { 
protected:
	class State;

public:
	StateMachine(int depth = DEF_DEPTH);
	virtual ~StateMachine();
		
public:
	void setState(State *pstate);
	State *getState();
	void rollState();
	
	void Handle(void* p = NULL); /*has an optional param*/
	
protected:
	virtual void handleNewState() {};

private:
	void regState(State *pstate);
	void unregState(State *pstate);
	
private:
	unsigned int depth_;
	std::list<State*> statequeue_;
	
protected:
	/*State class*/
	class State {
		friend class StateMachine;
	public:
		State(StateMachine* psm);
		virtual ~State();
			
	public:
		StateMachine* getStateMachine();
		
		void setState(State* pstate);
		State* getState();
	
	protected:
		virtual void Handle(void* p) 
		{};
	
	private:
		StateMachine* psm_;
	};
	/**/

private:
	State unknstate_;
};

};

#endif
