//
// C++ Implementation: XinePlayer Network Utils
//
// Description:
//
//
// Author: Toader Mihai Claudiu, mtoader@gmail.com
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "DCE/Logger.h"

using namespace DCE;


// forward declarations for internal used functions
int connect_to_in_addr(struct in_addr ia, int port);

int connect_to_host(const char *pHostname, int port)
{
	struct hostent *h;
	int             i;
	int 			s;

	h = gethostbyname(pHostname);
	if ( h == NULL )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Unable to resolve '%s'", pHostname);
		return 0;
	}

	for ( i=0; h->h_addr_list[i]; i++)
	{
		struct in_addr ia;
		memcpy (&ia, h->h_addr_list[i], 4);
		s = connect_to_in_addr(ia, port);
		if (s != -1)
			return s;
	}

	g_pPlutoLogger->Write(LV_CRITICAL, "Unable to connect to '%s:%d'", pHostname, port);

	return -1;
}

int connect_to_in_addr(struct in_addr ia, int port)
{
	int 				s;
	struct sockaddr_in 	sin;

	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( s == -1 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Error trying to create socket: '%s'", strerror(errno));
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr   = ia;
	sin.sin_port   = htons(port);

	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1 && errno != EINPROGRESS)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Error trying to connect the socket: '%s'", strerror(errno));
		close(s);
		return -1;
	}

	return s;
}
