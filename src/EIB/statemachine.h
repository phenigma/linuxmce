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

namespace EIBBUS {

#define STATE_UNKNOWNSTATE	-1

/**
@author 
*/
class StateMachine{
public:
    StateMachine() 
	: state_(STATE_UNKNOWNSTATE) {}

public:
	inline void setState(int state) {
		state_=  state;
	}
	inline int getState() {
		return state_;
	}

private:
	int state_;
};

};

#endif
