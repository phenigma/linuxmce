#ifndef SLIM_PROTOCOL_HANDLER_H
#define SLIM_PROTOCOL_HANDLER_H

#include "xine_internal.h"
// #include "xineutils.h"

// sending helo
int slim_protocol_make_helo(char **outBuffer, int *len, unsigned char deviceID, unsigned char deviceRevision, unsigned char mac[6]);

typedef enum _CommandType 
{
	COMMAND_VERSION,
	COMMAND_STREAM,
	COMMAND_I2C,
	COMMAND_VFDISPLAY,	
	COMMAND_DISPLAY
} CommandType;

struct slimCommand {
	CommandType commandType;

	union {
		struct _versionCommand {
			char *versionData;
			unsigned int versionDataLength;
		} version;

		struct _vfDisplayCommand {
			char code;
		} vfDisplay;

		struct _streamCommand {
		 	char code;
		} stream;

		struct _i2cCommand {
			char code;
		} i2c;

		struct _graphicCommand {
			char code;
		} grafic;

	} commandData;
};

int slim_protocol_decode(unsigned char *buffer, int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand);

#endif // SLIM_PROTOCOL_HANDLER_H