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
#include "IOPool.h"

#define FAIL_SLEEP_TIME	2000

namespace DCE {

IOPool::IOPool(bool usemain)
	: LoopStateMachine(usemain) 
{}

IOPool::~IOPool() {
}

bool 
IOPool::handleIteration() {
	IOConnection* pconn = getConnection();
	if(!pconn) {
		return false;
	}
	
	IOPoolState* pstate = reinterpret_cast<IOPoolState*>(getState());
	if(pstate != NULL) {
		if(!pconn->isOpened()) {
			if(!pconn->Open()) {
				usleep(FAIL_SLEEP_TIME * 1000);
				return true;
			} else {
				if(pstate != NULL) {
					pstate->handleOpen(pconn);
					return true;
				}
			}
		}
		IOConnection* pconn = getConnection();
		if(pconn && pconn->isDataAvailable()) {
			if(pstate != NULL) {
				pstate->handleRead(pconn);
				return true;
			}
		}
	}
	return false;
}

};
