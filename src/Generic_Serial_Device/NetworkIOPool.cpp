//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "NetworkIOPool.h"

namespace DCE {

NetworkIOPool::NetworkIOPool(bool usemain) 
	: IOPool(usemain), defstate_(this)
{
	setState(&defstate_);
}


NetworkIOPool::~NetworkIOPool()
{
}

IOConnection* 
NetworkIOPool::getConnection() {
	return &netconn;
}


bool 
NetworkIOPool::handleIteration() {
	return IOPool::handleIteration();
}


void 
NetworkIOPool::NetworkIOState::handleOpen(IOConnection* pconn) {
}

void 
NetworkIOPool::NetworkIOState::handleRead(IOConnection* pconn) {
}

void 
NetworkIOPool::NetworkIOState::handleClose(IOConnection* pconn) {
}

};
