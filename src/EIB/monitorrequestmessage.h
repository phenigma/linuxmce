//
// C++ Interface: monitorrequestmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSMONITORREQUESTMESSAGE_H
#define EIBBUSMONITORREQUESTMESSAGE_H

#include "message.h"

namespace EIBBUS {

/**
@author 
*/
class MonitorRequestMessage : public PeiMessage
{
public:
	virtual int Send(BusConnector *pbusconn);
	virtual int Recv(BusConnector *pbusconn);
};

};

#endif
