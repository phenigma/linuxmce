#ifndef SLIM_STREAM_H
#define SLIM_STREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 262144

struct slim_stream
{
	char buffer[BUFFER_SIZE];

	int readPtr;
	int writePtr;

	unsigned char isEndOfStream;	
	unsigned char isStreaming;

	unsigned int isConnecting;

	unsigned SOCKET connection;

	unsigned char *uri;
	unsigned char hostAddr[4];
	unsigned int  hostPort;

	unsigned int	readerThreadControl;
	pthread_t		readerThread;
	pthread_mutex_t bufferMutex;;
};

int stream_get_data_size(struct slim_stream *pstream);
int stream_get_free_space(struct slim_stream *pstream);

int stream_write(char *pBuffer, int off, int len, struct slim_stream *pStream);
int stream_read(char *pBuffer, int off, int len, struct slim_stream *pStream);

void stream_open(struct slim_stream *pStream);
void stream_close(struct slim_stream *pstream);
void stream_flush(struct slim_stream *pstream);

#endif // SLIM_STREAM_H