//
// C++ Implementation: statefullproxypeerthread
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "statefullproxypeerthread.h"

namespace MYTHTV {

StatefullProxyPeerThread::StatefullProxyPeerThread(ProxyServer *pserver, int srcsockfd, int destsockfd)
	: ProxyPeerThread(pserver, srcsockfd, destsockfd), state_(PEER_STATE_NONE)
{}

StatefullProxyPeerThread::~StatefullProxyPeerThread()
{}




};
