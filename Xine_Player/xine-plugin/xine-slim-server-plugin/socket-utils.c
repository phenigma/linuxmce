#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>

#include "socket-utils.h"

SOCKET attempt_connect_to_inaddr(struct in_addr ia, int port, xine_t *xine)
{
	SOCKET s;
	struct sockaddr_in sin;

	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( s == -1 )
	{
		if ( xine )
			xine_log(xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Error while trying to create socket: %s\n"), strerror(errno));
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr   = ia;
	sin.sin_port   = htons(port);

#ifndef WIN32
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1 && errno != EINPROGRESS)
#else
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1 && WSAGetLastError() != WSAEINPROGRESS)
#endif
	{
		if ( xine )
			xine_log(xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Error while trying to connect the socket: %s\n"), strerror(errno));

		close((int)s);
		return -1;
	}

	return s;
}

SOCKET connect_to_host_with_addr(struct in_addr host, int port, xine_t *xine)
{
	SOCKET s;

	if ( (s = attempt_connect_to_inaddr(host, port, xine)) != -1 )
		return s;

	return -1;
}

SOCKET connect_to_host_with_name(const char *host, int port, xine_t *xine)
{
	struct hostent *h;
	int             i;
	SOCKET			s;

	h = gethostbyname(host);
	if ( h == NULL )
	{
		if ( xine != NULL )
			xine_log(xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Unable to resolve '%s'.\n"), host);
		return 0;
	}

	for ( i=0; h->h_addr_list[i]; i++)
	{
		struct in_addr ia;
		memcpy (&ia, h->h_addr_list[i], 4);
		s = attempt_connect_to_inaddr(ia, port, xine);
		if (s != -1)
			return s;
	}

	if ( xine != NULL )
		xine_log(xine, XINE_LOG_PLUGIN, _(LOG_MODULE "Unable to connect to '%s'.\n"), host);

	return -1;
}
