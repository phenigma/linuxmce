//
// C++ Implementation: statemachine
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "statemachine.h"

namespace EIBBUS {

/* State */
StateMachine::State::State(StateMachine* psm) {
	psm_ = psm;
}

StateMachine::State::~State() {
	psm_->unregState(this);
	psm_ = NULL;
}
		
StateMachine* 
StateMachine::State::getStateMachine() {
	return psm_;
}

void 
StateMachine::State::setState(State* pstate) {
	if(psm_ != NULL) {
		psm_->setState(pstate);
	}
}
		
StateMachine::State* 
StateMachine::State::getState() {
	if(psm_ != NULL) {
		return psm_->getState();
	} else {
		return NULL;
	}
}

/* State Machine*/

StateMachine::StateMachine(int depth)
	: depth_(depth), unknstate_(this)
{
	if(depth_ < MIN_DEPTH) {
		depth_ = MIN_DEPTH;
	}
	setState(&unknstate_);
}

StateMachine::~StateMachine() {
}

void 
StateMachine::regState(State *pstate) {
}

void 
StateMachine::unregState(State *pstate) {
	if(getState() == pstate) {
		setState(&unknstate_);
	}
}

void 
StateMachine::setState(State *pstate) {
	if(getState() == pstate) {
		return;
	}
	
	while(statequeue_.size() >= depth_) {
		statequeue_.pop_front();
	}
	statequeue_.push_back(pstate);
	handleNewState();
}

StateMachine::State*
StateMachine::getState() {
	return statequeue_.back();
}

void 
StateMachine::rollState() {
	if(statequeue_.size() > MIN_DEPTH) {
		statequeue_.pop_back();
	}
}

void 
StateMachine::Handle(void* p) {
	getState()->Handle(p);
}

};
