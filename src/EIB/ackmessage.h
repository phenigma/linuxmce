//
// C++ Interface: ackmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSACKMESSAGE_H
#define EIBBUSACKMESSAGE_H

#include "message.h"

namespace EIBBUS {

/**
@author 
*/
class AckMessage : public Message {
public:
	virtual int Send(BusConnector *pbusconn);
	virtual int Recv(BusConnector *pbusconn);
};

};

#endif
