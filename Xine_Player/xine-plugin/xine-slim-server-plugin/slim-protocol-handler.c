#include "slim-protocol-handler.h"

// #include "input/input_plugin.h"

int slim_protocol_make_helo(char **outBuffer, int *len, unsigned char deviceID, unsigned char deviceRevision, unsigned char mac[6])
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
	buffer[7] = 8;
 
	buffer[8] = deviceID;
	buffer[9] = deviceRevision;
	buffer[10] = mac[0];
	buffer[11] = mac[1];
	buffer[12] = mac[2];
	buffer[13] = mac[3];
	buffer[14] = mac[4];
	buffer[15] = mac[5];
	buffer[16] = 0x80;
	buffer[17] = 0;

	return 1;
}



int slim_protocol_decode(unsigned char *buffer, int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand)
{
// 	char *pChar;  // usefull 

	if ( strncmp(buffer, "strm", 4) == 0 )
	{
		decodedCommand->commandType = COMMAND_STREAM;
		// decodedCommand->commandData.stream.
	}
	else if ( strncmp(buffer, "grfd", 4) == 0 )
	{
		decodedCommand->commandType = COMMAND_DISPLAY;
	}	
	else if ( strncmp(buffer, "vfdc", 4) == 0 )
	{
		decodedCommand->commandType = COMMAND_VFDISPLAY;
	}
	else if ( strncmp(buffer, "i2cc", 4) == 0 )
	{
		decodedCommand->commandType = COMMAND_I2C;
	}
	else if ( strncmp(buffer, "vers", 4) == 0 )
	{
		decodedCommand->commandType = COMMAND_VERSION;		
		decodedCommand->commandData.version.versionData = buffer + 4;				
		decodedCommand->commandData.version.versionDataLength = strlen(decodedCommand->commandData.version.versionData);
	}
	else
	{
		xine_log(xine_session, XINE_LOG_PLUGIN, _(LOG_MODULE "Protocol decode error: Could not interpret command: %4s"), buffer);
		return 0;
	}

	return 1;
}
