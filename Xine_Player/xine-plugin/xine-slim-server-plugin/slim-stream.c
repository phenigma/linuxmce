#include "slim-stream.h"

#include "socket-utils.h"

#define TMP_BUFFER_SIZE 20480

char buffer[TMP_BUFFER_SIZE];

int stream_get_data_size(struct slim_stream *pStream)
{
	int availableData;

	pthread_mutex_lock(&pStream->bufferMutex);
	if (pStream->writePtr != pStream->readPtr)
	{
		availableData = pStream->writePtr - pStream->readPtr - 1;

		if (availableData < 0)
			availableData += BUFFER_SIZE;
	}
	else
		availableData = 0;

	pthread_mutex_unlock(&pStream->bufferMutex);

	return availableData;
}

int stream_get_free_space(struct slim_stream *pStream)
{
	int freeSpace;

	pthread_mutex_lock(&pStream->bufferMutex);
	freeSpace = pStream->readPtr - pStream->writePtr - 1;

		if ( freeSpace < 0 )
		freeSpace += BUFFER_SIZE;

	pthread_mutex_unlock(&pStream->bufferMutex);
	return (unsigned int)freeSpace;
}

void *stream_reader_function(void*pData)
{
	struct slim_stream *pStream = (struct slim_stream*)pData;
	xine_stream_t *xine_stream = NULL;

	unsigned int done, len, linenum, contentlength, writeBufferSize;
	int pos;

	writeBufferSize = 0;
	_snprintf(buffer, BUFFER_SIZE - writeBufferSize,
			"GET %s HTTP/1.0\015\012"
			"User-Agent: xine/xine\015\012"
			"Accept: */*\015\012"
			"\015\012", pStream->uri);

	printf("Sending: \n%s\n", buffer);
	writeBufferSize = (unsigned int)strlen(buffer);
	if ( _x_io_tcp_write(xine_stream, (int)pStream->connection, buffer, writeBufferSize) != writeBufferSize )
		return NULL;

	contentlength = 0;
	linenum = 0;
	done = 0;
	len = 0;

	while (!done)
	{
		if ( read((int)pStream->connection, buffer + len, 1) != 1 )
		{
			printf("Error reading data from server.\n");
			return NULL;
		}

		if (buffer[len] == '\012')
		{
			buffer[len] = '\0';
			len--;

			if (len >= 0 && buffer[len] == '\015')
			{
				buffer[len] = '\0';
				len--;
			}

			printf("-->Read: %s\n", buffer);

			linenum++;
			if (linenum == 1)
			{
				int httpver, httpsub, httpcode;
				char httpstatus[51];

				if (sscanf(buffer, "HTTP/%d.%d %d %50[^\015\012]", &httpver, &httpsub, &httpcode, httpstatus) != 4)
				{
					if (sscanf(buffer, "ICY %d %50[^\015\012]", &httpcode, httpstatus) != 2)
					{
						_x_message(xine_stream, XINE_MSG_CONNECTION_REFUSED, "invalid http answer", NULL);
						xine_log (NULL, XINE_LOG_MSG, _("input_http: invalid http answer\n"));
						return 0;
					}
					else
					{
						// shoutcast = 1;
						done = 1;
					}
				}

				//if (httpcode >= 300 && httpcode < 400)
				//{
				//	xine_log (this->stream->xine, XINE_LOG_MSG, _("input_http: 3xx redirection: >%d %s<\n"), httpcode, httpstatus);
				//}
				//else if (httpcode == 404)
				//{
				//	_x_message(this->stream, XINE_MSG_FILE_NOT_FOUND, this->mrl, NULL);
				//	xine_log (this->stream->xine, XINE_LOG_MSG, _("input_http: http status not 2cxx: >%d %s<\n"), httpcode, httpstatus);
				//	return 0;
					//} else if (httpcode == 403) {
					//	_x_message(this->stream, XINE_MSG_PERMISSION_ERROR, this->mrl, NULL);
					//	xine_log (this->stream->xine, XINE_LOG_MSG,
					//		_("input_http: http status not 2cxx: >%d %s<\n"),
					//		httpcode, httpstatus);
					//	return 0;
					//} else if (httpcode < 200 || httpcode >= 300) {
					//	_x_message(this->stream, XINE_MSG_CONNECTION_REFUSED, "http status not 2cxx: ",
					//		httpstatus, NULL);
					//	xine_log (this->stream->xine, XINE_LOG_MSG,
					//		_("input_http: http status not 2cxx: >%d %s<\n"),
					//		httpcode, httpstatus);
					//	return 0;
					//}
			}
			else
			{
				if (contentlength == 0)
				{
					intmax_t parsedLength;

					if (sscanf(buffer, "Content-Length: %" SCNdMAX , &parsedLength) == 1) {
						xine_log (NULL, XINE_LOG_MSG,  _("input_http: content length = %" PRIiMAX " bytes\n"), parsedLength);
						contentlength = (off_t)parsedLength;
					}
				}

				if (!strncmp(buffer, "Location: ", 10))
				{
					//char *href = (buffer + 10);

					//lprintf ("trying to open target of redirection: >%s<\n", href);

					//href = _x_canonicalise_url (this->mrl, href);
					//free(this->mrl);
					//this->mrl = href;
					//return http_plugin_open(this_gen);
				}
			}

			if (len == -1) done = 1;
			else		   len = 0;
		} else
			len ++;
	}

	while ( pStream->readerThreadControl != THREAD_CONTROL_STOP )
	{
		len = read((int)pStream->connection, buffer, TMP_BUFFER_SIZE);

		while ( pStream->readerThreadControl != THREAD_CONTROL_STOP && stream_get_free_space(pStream) < len )
			Sleep(50);

		if ( THREAD_CONTROL_STOP != pStream->readerThreadControl )
			stream_write(buffer, 0, len, pStream);
	}

	return NULL;
}

void stream_close(struct slim_stream *stream, xine_t *xine_handler)
{
	stream->isEndOfStream = 1;

	stream->readerThreadControl = THREAD_CONTROL_STOP;

	xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE_DATA_STREAM "Closing data stream connection.\n"));
	if ( stream->readerThread )
	{
		pthread_join(stream->readerThread, NULL);
		pthread_attr_destroy(&stream->readerThreadAttr);
	}

	xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE_DATA_STREAM "Closed data stream connection.\n"));
	close((int)stream->connection);
	stream->connection = -1;
}

void stream_open(struct slim_stream *pStream, xine_t *xine_handler)
{
	pStream->writePtr = 0;
	pStream->readPtr = 0;
	pStream->isEndOfStream = 0;
	pStream->isStreaming = 1;

	xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE_DATA_STREAM "Opening data stream connection.\n"));
	pStream->connection = connect_to_host_with_addr(pStream->hostAddr, pStream->hostPort, NULL);
	xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE_DATA_STREAM "Connected. Creating reader thread.\n"));

	if ( pthread_attr_init(&pStream->readerThreadAttr) != 0 )
	{
		stream_close(pStream, xine_handler);
		return;
	}

	pStream->readerThreadControl = THREAD_CONTROL_START;
	if ( pthread_create(&pStream->readerThread, &pStream->readerThreadAttr, stream_reader_function, pStream) != 0 )
	{
		pthread_attr_destroy(&pStream->readerThreadAttr);
		stream_close(pStream, xine_handler);
		return;
	}
	xine_log(xine_handler, XINE_LOG_PLUGIN, _(LOG_MODULE_DATA_STREAM "Thread started.\n"));
}

int stream_write(char *pBuffer, int off, int len, struct slim_stream *pStream)
{
	int oldWritePtr;
	oldWritePtr = pStream->writePtr;

	if ( stream_get_free_space(pStream) < len )
	{
/**
		printf("--->Buffer (%d) writing %d: [ %d->%d, %d->%d ].\n",
					BUFFER_SIZE, len,
					pStream->readPtr, pStream->readPtr,
					oldWritePtr, pStream->writePtr);
*/
		return 0;
	}

	pthread_mutex_lock(&pStream->bufferMutex);
	if ( pStream->writePtr < pStream->readPtr)
	{
		memcpy(pStream->buffer + pStream->writePtr, pBuffer + off, len);
		pStream->writePtr += len;
	}
	else
	{
		if ( pStream->writePtr + len < BUFFER_SIZE )
		{
			memcpy(pStream->buffer + pStream->writePtr, pBuffer + off, len);
			pStream->writePtr += len;
		}
		else
		{
			int currentWriteSize = pStream->writePtr;

			memcpy(pStream->buffer + pStream->writePtr, pBuffer + off, BUFFER_SIZE - currentWriteSize );
			memcpy(pStream->buffer, pBuffer + off + BUFFER_SIZE - currentWriteSize, len - (BUFFER_SIZE - currentWriteSize) );

			pStream->writePtr = len - (BUFFER_SIZE - currentWriteSize);
		}
	}

// 	printf("--->Buffer (%d) writing %d: [ %d->%d, %d->%d ].\n",
// 			BUFFER_SIZE, len,
// 			pStream->readPtr, pStream->readPtr,
// 			oldWritePtr, pStream->writePtr);

	pthread_mutex_unlock(&pStream->bufferMutex);
	return 0;
}

int stream_read(char *pBuffer, int off, int len, struct slim_stream *pStream)
{
	int oldReadPtr;
	int requestedReadSize;
	int copiedCount;

	oldReadPtr = pStream->readPtr;
	requestedReadSize = len;

	while ( stream_get_data_size(pStream) <= len )
	{
		if ( pStream->isEndOfStream )
			return -1;

		printf("Reader sleeping!\n");
		Sleep(50); // sleep for 0.05 seconds and release the lock
		printf("Reader sleep completed!\n");
	}

	pthread_mutex_lock(&pStream->bufferMutex);

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

/*
	printf("<---Buffer (%d) reading %d: [ %d->%d, %d->%d ].\n",
				BUFFER_SIZE, requestedReadSize,
				oldReadPtr, pStream->readPtr,
				pStream->writePtr, pStream->writePtr);
*/
	pthread_mutex_unlock(&pStream->bufferMutex);
	return copiedCount;
}

void stream_flush(struct slim_stream *pStream)
{
	pStream->readPtr = pStream->writePtr;
}
