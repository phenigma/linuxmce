/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "IOPool.h"

#define FAIL_SLEEP_TIME				2000
#define DATA_AVAILABLE_WAIT_TIME	5

namespace DCE {

IOPool::IOPool(bool usemain)
	: LoopStateMachine(usemain) 
{}

IOPool::~IOPool() {
}

bool 
IOPool::handleStartup() {
	IOConnection* pconn = getConnection();
	if(pconn) {
		pconn->Open();
	}
	return LoopStateMachine::handleStartup();
}

bool 
IOPool::handleIteration() {
printf("In handleIteration\n");
	IOConnection* pconn = getConnection();
	if(!pconn) {
printf("!pcon\n");
		return LoopStateMachine::handleIteration();
	}
	
	IOPoolState* pstate = reinterpret_cast<IOPoolState*>(getState());
printf("pstate: %d\n",pstate);
	if(pstate != NULL) {
		if(!pconn->isOpened()) {
printf("!pconn->isOpened()\n");
			if(!pconn->Open()) {	
printf("!pconn->Open()\n");
				usleep(FAIL_SLEEP_TIME * 1000);
				return true;
			} else {
printf("else pstate: %p\n",pstate);
				if(pstate != NULL) {
					pstate->handleOpen(pconn);
					return true;
				}
			}
		}
		IOConnection* pconn = getConnection();
printf("pconn : %p\n",pconn);
		if(pconn && pconn->isDataAvailable(DATA_AVAILABLE_WAIT_TIME)) {
printf("data available - pstate: %p\n",pstate);
			if(pstate != NULL) {
				pstate->handleRead(pconn);
				return true;
			}
		}
else
printf("no data available\n");

	}
	return LoopStateMachine::handleIteration();
}

void 
IOPool::handleTerminate() {
	LoopStateMachine::handleTerminate();
	IOConnection* pconn = getConnection();
	if(pconn) {
		pconn->Close();
	}
}

};
