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
#include "SerialIOPool.h"

namespace DCE {

SerialIOPool::SerialIOPool(bool usemain) 
	: IOPool(usemain)
{
	setState(&defstate_);
}

SerialIOPool::~SerialIOPool() {
}

IOConnection* 
SerialIOPool::getConnection() {
	return &serconn;
}


bool 
SerialIOPool::handleIteration() {
	return IOPool::handleIteration();
}


void 
SerialIOPool::SerialIOState::handleOpen(IOConnection* pconn) {
}

void 
SerialIOPool::SerialIOState::handleRead(IOConnection* pconn) {
}

void 
SerialIOPool::SerialIOState::handleClose(IOConnection* pconn) {
}

};
