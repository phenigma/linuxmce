//
// C++ Implementation: bauinitmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bauinitmessage.h"
#include "busconnector.h"

namespace EIBBUS {

static const unsigned char s_bauinit[4] = 
		{0x10, 0x40, 0x40, 0x16};

int 
BauInitMessage::Send(BusConnector *pbusconn) {
	return Message::SendBuffer(pbusconn, s_bauinit, sizeof(s_bauinit));
}

};
