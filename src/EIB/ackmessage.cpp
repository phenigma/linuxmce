//
// C++ Implementation: ackmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ackmessage.h"
#include "busconnector.h"

#include "DCE/Logger.h"

#define ACK_TIMEOUT		500


using namespace DCE;

namespace EIBBUS {

int 
AckMessage::Send(BusConnector *pbusconn) {
	unsigned char acknowledge = 0xE5;
	pbusconn->Send(&acknowledge, 1);
	return 0;
}

int 
AckMessage::Recv(BusConnector *pbusconn) {
	unsigned char acknowledge = 0;
	if(RecvBuffer(pbusconn, &acknowledge, 1) <= 0) {
		return RECV_INVALID;
	}
	
	if(acknowledge != 0xE5) {
		//g_pPlutoLogger->Write(LV_WARNING, "Acknowledge not received (0xE5), received instead: %x", acknowledge);
		UndoRecvBuffer(pbusconn, &acknowledge, 1);
		return RECV_UNKNOWN;
	}
	
	return 0;
}

};
