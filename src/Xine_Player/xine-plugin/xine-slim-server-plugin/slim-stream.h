#ifndef SLIM_STREAM_H
#define SLIM_STREAM_H

#include "local-defines.h"

#include <xine.h>

#define BUFFER_SIZE 262144
// #define BUFFER_SIZE 40960

typedef enum _ThreadControlCommandType
{
	THREAD_CONTROL_STOP,
	THREAD_CONTROL_START
} ThreadControlCommandType;

struct slim_stream
{
	char buffer[BUFFER_SIZE];

	int readPtr;
	int writePtr;

	unsigned char isEndOfStream;
	unsigned char isStreaming;

	unsigned int isConnecting;

	SOCKET connection;

	struct in_addr hostAddr;
	unsigned int  hostPort;
	unsigned char *uri;

	ThreadControlCommandType readerThreadControl;
	pthread_t		readerThread;
	pthread_attr_t	readerThreadAttr;

	pthread_mutex_t bufferMutex;
};

int stream_get_data_size(struct slim_stream *pstream);
int stream_get_free_space(struct slim_stream *pstream);

int stream_write(char *pBuffer, int off, int len, struct slim_stream *pStream);
int stream_read(char *pBuffer, int off, int len, struct slim_stream *pStream);
int stream_peek(char *pBuffer, int off, int len, struct slim_stream *pStream);

void stream_open(struct slim_stream *pStream, xine_t *xine);
void stream_close(struct slim_stream *pstream, xine_t *xine);
void stream_flush(struct slim_stream *pstream);

#endif // SLIM_STREAM_H

