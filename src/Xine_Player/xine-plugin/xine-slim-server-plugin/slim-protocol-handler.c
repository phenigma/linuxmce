#include "slim-protocol-handler.h"

// #include "input/input_plugin.h"

int slim_protocol_make_ack(unsigned char **outBuffer, int *len,
						   char code[4],  // the type of the ack to send
						   unsigned char crlf, // ?
						   unsigned char masInit, // ?
						   unsigned char masMode, // ?
						   int rptr, int wptr, // ?
						   long long bytesRX, int jiffies)
{
	char *buffer;
	unsigned int pos;
	*len =  4 /*STAT*/ + 4 /* payload len */ + 4 /* ack type */ +
			1 /* crlf */ + 1 /* masinit */ + 1 /* masmode */ +
			4 /* rptr */ + 4 /* wptr */ +
			8 /* bytesRX */ + 2 /* wireless signal == 255 */ +
			4 /* jiffies */;

	*len = 38;
	*outBuffer = (unsigned char *)xine_xmalloc(*len);
	buffer = *outBuffer;

	strncpy(buffer, "STAT", 4);

	// please excuse the lack of beauty of this code.
	pos = 4;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 30;

	buffer[pos++] = code[0];
	buffer[pos++] = code[1];
	buffer[pos++] = code[2];
	buffer[pos++] = code[3];

	buffer[pos++] = crlf;
	buffer[pos++] = masInit;
	buffer[pos++] = masMode;

	buffer[pos++] = (rptr >> 0x18) & 0xFF;
	buffer[pos++] = (rptr >> 0x10) & 0xFF;
	buffer[pos++] = (rptr >> 0x08) & 0xFF;
	buffer[pos++] = (rptr >> 0x00) & 0xFF;

	buffer[pos++] = (wptr >> 0x18) & 0xFF;
	buffer[pos++] = (wptr >> 0x10) & 0xFF;
	buffer[pos++] = (wptr >> 0x08) & 0xFF;
	buffer[pos++] = (wptr >> 0x00) & 0xFF;

	buffer[pos++] = (char)(bytesRX >> 0x38) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x30) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x28) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x20) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x18) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x10) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x08) & 0xFF;
	buffer[pos++] = (char)(bytesRX >> 0x00) & 0xFF;
	buffer[pos++] = 0;

	buffer[pos++] = 0xFF;
	buffer[pos++] = 0xFF;

	buffer[pos++] = (jiffies >> 0x18) & 0xFF;
	buffer[pos++] = (jiffies >> 0x10) & 0xFF;
	buffer[pos++] = (jiffies >> 0x08) & 0xFF;
	buffer[pos++] = (jiffies >> 0x00) & 0xFF;

	return 1;
}

int slim_protocol_make_helo(unsigned char **outBuffer, int *len, unsigned char deviceID, unsigned char deviceRevision, unsigned char mac[6])
{
	char *buffer;
	*len  = 4 /*HELO*/ + 4 /* payload len */ + 1 /* device ID */ +
			1 /* device ID */ + 6 + /* mac data */ + 2 /* flags */;
	*outBuffer = (char*)xine_xmalloc(*len);
	buffer = *outBuffer;

	strncpy(buffer, "HELO", 4);
	buffer[4] = 0;
	buffer[5] = 0;
	buffer[6] = 0;
	buffer[7] = *len - 8;

	buffer[8] = deviceID;
	buffer[9] = deviceRevision;
	buffer[10] = mac[0];
	buffer[11] = mac[1];
	buffer[12] = mac[2];
	buffer[13] = mac[3];
	buffer[14] = mac[4];
	buffer[15] = mac[5];
	buffer[16] = 0x00;
	buffer[17] = 0x00;

	return 1;
}

int slim_protocol_decode_strm_command(unsigned char *buffer, unsigned int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand)
{
	decodedCommand->type = COMMAND_STREAM;

	// get the primary command;
	if ( buffer[0] == 's' )
		decodedCommand->data.stream.command = STREAM_START;
	else if ( buffer[0] == 'u' )
		decodedCommand->data.stream.command = STREAM_UNPAUSE;
	else if ( buffer[0] == 'p' )
		decodedCommand->data.stream.command = STREAM_PAUSE;
	else if ( buffer[0] == 'q' )
		decodedCommand->data.stream.command = STREAM_QUIT;
	else
	{
		xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Invalid stream control command %c. Ignoring!\n"), buffer[0]);
		decodedCommand->data.stream.command = STREAM_NO_COMMAND;
		return 0;
	}

	if ( buffer[2] == 'm' )
		decodedCommand->data.stream.format = STREAM_FORMAT_MP3;
	else if ( buffer[2] == 'f' )
		decodedCommand->data.stream.format = STREAM_FORMAT_FLAC;
	else if ( buffer[2] == 'p' )
		decodedCommand->data.stream.format = STREAM_FORMAT_PCM;
	else
	{
		decodedCommand->data.stream.format = STREAM_FORMAT_UNKNOWN;
		xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Unknown streaming format: %c. Ignoring!\n"), buffer[2]);
		return 0;
	}

	xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Detected streaming format: %d!\n"), decodedCommand->data.stream.format);

	decodedCommand->data.stream.autoStart = buffer[1];
	decodedCommand->data.stream.hostPort = (buffer[18] << 0x08) | buffer[19];

	decodedCommand->data.stream.hostAddr.s_addr = (buffer[20] << 0x18) | (buffer[21] << 0x10) | (buffer[22] << 0x08) | buffer[23];

	decodedCommand->data.stream.urlAddress = buffer + 24;
	decodedCommand->data.stream.urlSize = 0;

	// the buffer + 24 is a classical GET format
	// GET /url HTTP/1.0\n\n
	// skip the GET part without crossing the buffer margins.
	while ( (decodedCommand->data.stream.urlAddress - buffer) < (int)bufferLength &&
			*decodedCommand->data.stream.urlAddress != ' ' )
		decodedCommand->data.stream.urlAddress++;

	// skip whitespace after GET without crossing buffer margins.
	while ( (decodedCommand->data.stream.urlAddress - buffer) < (int)bufferLength &&
			*decodedCommand->data.stream.urlAddress == ' ' )
		decodedCommand->data.stream.urlAddress++;

	// if we crossed the buffer this is an error.
	if ( (decodedCommand->data.stream.urlAddress - buffer) == bufferLength )
		return 0;

	// walk the complete URL until the next space (an url can't have spaces in it.
	while ( (decodedCommand->data.stream.urlAddress - buffer + decodedCommand->data.stream.urlSize) < bufferLength &&
		   *(decodedCommand->data.stream.urlAddress + decodedCommand->data.stream.urlSize) != ' ')
		   decodedCommand->data.stream.urlSize++;

	// if we crossed the buffer this is an error.
	if ( ((decodedCommand->data.stream.urlAddress - buffer) +  decodedCommand->data.stream.urlSize) == (int)bufferLength )
		return 0;

	return 1;
}

int slim_protocol_decode_vfdc_command(unsigned char *buffer, unsigned int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand)
{
	char *internalBuffer;
	unsigned int internalBufferPos, i;

	decodedCommand->type = COMMAND_VFDISPLAY;
	internalBuffer = (unsigned char *)xine_xmalloc(bufferLength);
	internalBufferPos = 0;

	for ( i = 0; i < bufferLength; i += 2 )
	{
		switch ( buffer[i] )
		{
		case 0x00:
			//	xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "VFD packet type 0x00 not implemented!\n"));
			break;
		case 0x02:
			if ( (buffer[i + 1] & 0x80) == 0x80 )
			{
				decodedCommand->data.vfDisplay.ddramAddr = buffer[i + 1] & 0x7F;
				decodedCommand->data.vfDisplay.setDdram = 1;
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "DDRAM addr set to %d\n"), buffer[i + 1] & 0x7F);
			}
			else if ( (buffer[i + 1] & 0x40) == 0x40 )
			{
				decodedCommand->data.vfDisplay.ddramAddr = 0;
				decodedCommand->data.vfDisplay.cgramAddr = buffer[i + 1] & 0x3F;
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "CGRAM addr set to %d\n"), buffer[i + 1] & 0x3F);
			}
			else if ( (buffer[i + 1] & 0x20) == 0x20 )
			{
				decodedCommand->data.vfDisplay.brightness = 32 - buffer[i + 7] * 4;
				i += 6;
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Brightness set to: %d\n"), 32 - buffer[i + 7] * 4);
			}
			else if ( (buffer[i + 1] & 0x10) == 0x10 )
			{
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "VFD packet type 0x02 \"Cursor or display shift not implemented\"!\n"));
			}
			else if ( (buffer[i + 1] & 0x08) == 0x08 )
			{
				decodedCommand->data.vfDisplay.displayOn = (( buffer[i + 1] & 0x04 ) == 0x04);
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Display set to: %d\n"), decodedCommand->data.vfDisplay.displayOn);
			}
			else if ( (buffer[i + 1] & 0x04) == 0x04 )
			{
				decodedCommand->data.vfDisplay.entryMode = (( buffer[i + 1] & 0x03 ) == 0x03);
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Entry mode set to: %d\n"), decodedCommand->data.vfDisplay.entryMode);
			}
			else if ((buffer[i + 1 ] & 0x02) == 0x02)
			{
				decodedCommand->data.vfDisplay.ddramAddr = 0;
				decodedCommand->data.vfDisplay.setDdram = 1;
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Resetting cursor\n"));
			}
			else if ((buffer[i + 1] & 0x01) == 0x01)
			{
				// TODO: See if that data is usable
				//for (int j = framebufLive; j < framebufMask; j++)
				// framebuf[j] = 0x00;
				decodedCommand->data.vfDisplay.ddramAddr = 0;
				decodedCommand->data.vfDisplay.entryMode = 0x02;
// 				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Clearing display\n"));
			}
			break;
		case 0x03:
			// if ( decodedCommand->data.vfDisplay.setDdram )
			internalBuffer[internalBufferPos++] = buffer[i+1];
			break;
		default:
			xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Unknown VFD packet code: 0x%x\n"), buffer[i]);
		}
	}

	return 1;
}

int slim_protocol_decode_i2cc_command(unsigned char *buffer, unsigned int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand)
{
	unsigned int i;

	decodedCommand->type = COMMAND_I2C;

	i = 0;
	while ( i < bufferLength )
	{
		switch(buffer[i++])
		{
		case 's':
			if ( buffer[i] != 0x3e )
				xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Unknown i2cc 's' command address 0x%x"), buffer[i++]);
			else
			{
				i++;
				if  ( buffer[i] == 'w' && buffer[i + 1] == 0x68 )
				{
					// TODO: Make the actual sound volume to work
					i += 22;
				}
				else if ( buffer[i] == 'w' && buffer[i + 1] == 0x6C )
				{
					i += 10;
				}
			}
			break;
		default:
			xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Unknown i2cc command %c"), buffer[i++]);
			while ( i < bufferLength && buffer[i++] != 's');
		}
	}

	return 1;
}

int slim_protocol_decode(unsigned char *buffer, unsigned int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand)
{
	if ( strncmp(buffer, "vers", 4) == 0 )
	{
		decodedCommand->type = COMMAND_VERSION;
		decodedCommand->data.version.versionData = buffer + 4;
		decodedCommand->data.version.versionDataLength = (unsigned int)strlen(decodedCommand->data.version.versionData);
		return 1;
	}

	if ( strncmp(buffer, "strm", 4) == 0 )
		return slim_protocol_decode_strm_command(buffer + 4, bufferLength - 4, xine_session, decodedCommand);

	if ( strncmp(buffer, "vfdc", 4) == 0 )
		return slim_protocol_decode_vfdc_command(buffer + 4, bufferLength - 4, xine_session, decodedCommand);

	if ( strncmp(buffer, "i2cc", 4) == 0 )
		return slim_protocol_decode_i2cc_command(buffer + 4, bufferLength - 4, xine_session, decodedCommand);

	if ( strncmp(buffer, "grfd", 4) == 0 )
	{
		decodedCommand->type = COMMAND_DISPLAY;
		xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Grfd command received!\n"));
	}
	//else if ( strncmp(buffer, "i2cc", 4) == 0 )
	//{
	//	xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "i2cc command received!\n"));
	//	decodedCommand->type = COMMAND_I2C;
	//}
	else
	{
		xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Protocol decode error: Could not interpret command: %4s"), buffer);
		return 0;
	}

	return 1;
}
