//
// C++ Implementation: monitorrequestmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "monitorrequestmessage.h"
#include "busconnector.h"

namespace EIBBUS {

static const unsigned char s_monitorrequest[7] = 
		{0xA9, 0x90, 0x18, 0x34, 0x56, 0x78, 0x0A};

int 
MonitorRequestMessage::Send(BusConnector *pbusconn) {
	SendPeiBuffer(pbusconn, s_monitorrequest, sizeof(s_monitorrequest));
	return 0;
}

int 
MonitorRequestMessage::Recv(BusConnector *pbusconn) {
	return -1;
}

};
