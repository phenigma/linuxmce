#ifndef LOCAL_DEFINES_H
#define LOCAL_DEFINES_H

#ifndef LOG_MODULE
#define LOG_MODULE "SlimServer 5.4.0+ Input Plugin: "
#endif

#ifndef LOG_MODULE_DATA_STREAM
#define LOG_MODULE_DATA_STREAM "SlimServer 5.4.0+ Input Plugin (data stream): "
#endif

#define LOG_VERBOSE 1


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h>
#include <pthread.h>

#ifndef WIN32
	#define SOCKET int
	#define Sleep(time) usleep(time * 1000)
	#define _snprintf snprintf
#else
	#include <Windows.h>
#endif

#endif // LOCAL_DEFINES_H
