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

namespace MYTHTV
{
	/**
	@author igor
	*/
	class ProxyEventHandler
	{
	public:
		virtual void ChannelChanged(const char *host, int channelid) = 0;
// 		{
// 		};

		virtual void FrontendConnected(const char *host) = 0;
// 		{
// 		};

		virtual void FrontendDisconnected(const char *host) = 0;
/*		{
		};*/
	};
};

#endif
