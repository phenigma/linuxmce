#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include "xine/xine_internal.h"
#include "xine/xineutils.h"

#include "local-defines.h"

SOCKET connect_to_host_with_addr(struct in_addr host, int port, xine_t *xine);

SOCKET connect_to_host_with_name(const char *host, int port, xine_t *xine);

#endif // SOCKET_UTILS_H
