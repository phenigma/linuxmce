#include <pthread.h>
#include "slim-stream.h"

#include <Windows.h>
int stream_get_data_size(struct slim_stream *pStream)
{
	int availableData;

	if (pStream->writePtr == pStream->readPtr)
		return 0;

	availableData = pStream->writePtr - pStream->readPtr - 1;

	if (availableData < 0)
		availableData += BUFFER_SIZE;

	return availableData;
}

int stream_get_free_space(struct slim_stream *pStream)
{
	int freeSpace;
	
	freeSpace = pStream->readPtr - pStream->writePtr - 1;		
	if ( freeSpace < 0 )
		freeSpace += BUFFER_SIZE;
	
	return (unsigned int)freeSpace;
}

void stream_open(struct slim_stream *pStream)
{
	pStream->writePtr = 0;
	pStream->readPtr = 0;
	pStream->isEndOfStream = 0;
	pStream->isStreaming = 1;
}

void stream_close(struct slim_stream *stream)
{
	stream->isEndOfStream = 1;
	
	stream->readerThreadControl = THREAD_CONTROL_STOP;
	pthread_join(stream->readerThread);

	close(stream->connection);
	stream->connection = 0;
}

int stream_write(char *pBuffer, int off, int len, struct slim_stream *pStream)
{
	pthread_mutex_lock(&pStream->stateMutex);
	pthread_mutex_unlock(&pStream->stateMutex);
	return 0;
}

int stream_read(char *pBuffer, int off, int len, struct slim_stream *pStream)
{
	int copiedCount;

	pthread_mutex_lock(&pStream->stateMutex);
	while ( stream_get_data_size(pStream) <= len )
	{
		if ( pStream->isEndOfStream )
			return -1; 

		pthread_mutex_unlock(&pStream->stateMutex);
		Sleep(50); // sleep for 0.05 seconds and release the lock
		pthread_mutex_lock(&pStream->stateMutex);
	}

		//if ( pStream->isStreaming && pStream->writePtr == pStream->readPtr )
		//{
			 // sleep for 0.1 seconds
			// int bufferUnderrunHere = 1;
			// return READ_BUFFER_UNDERRUN;
		//}
		//		if (streaming && readPtr == writePtr) {
		//			logger.debug("buffer underrun");
		//			for (Iterator i = listeners.iterator(); i.hasNext();)
		//				((AudioBufferInputStreamListener) i.next())
		//				.bufferUnderrun(this);
		//		}

		//		wait();
//	}

	
	pStream->isStreaming = 1;

	copiedCount = 0;

	if ( pStream->writePtr < pStream->readPtr )
	{
		int count = BUFFER_SIZE - pStream->readPtr;
		if ( count > len )
			count = len;

		memcpy(pBuffer + off, pStream->buffer + pStream->readPtr, count);

		pStream->readPtr += len;
		if ( pStream->readPtr >= BUFFER_SIZE )
			pStream->readPtr -= BUFFER_SIZE;

		len -= count;
		copiedCount += count;
	}

	if ( len > 0 )
	{
		int count = pStream->writePtr - pStream->readPtr;
		if (count > len)
			count = len;

		memcpy(pBuffer + off + copiedCount, pStream->buffer + pStream->readPtr, count);

		pStream->readPtr += count;
		if ( pStream->readPtr >= BUFFER_SIZE )
			pStream->readPtr -= BUFFER_SIZE;

		len -= count;
		copiedCount += count;
	}

	pthread_mutex_unlock(&pStream->stateMutex);
	return copiedCount;
}

void stream_flush(struct slim_stream *pStream)
{
	pStream->readPtr = pStream->writePtr;
}
