//
// C++ Interface: bauinitmessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSBAUINITMESSAGE_H
#define EIBBUSBAUINITMESSAGE_H

#include "message.h"

namespace EIBBUS {

/**
@author 
*/
class BauInitMessage : public Message {
public:
	virtual int Send(BusConnector *pbusconn);
};

};

#endif
