//
// C++ Interface: proxyeventhandler
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYTHTVPROXYEVENTHANDLER_H
#define MYTHTVPROXYEVENTHANDLER_H

namespace MYTHTV {

/**
@author igor
*/
class ProxyEventHandler{
public:
	virtual void ChannelChanged(const char *host, const char* channelid) 
	{};
};

};

#endif
