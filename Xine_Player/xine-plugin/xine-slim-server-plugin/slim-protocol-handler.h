#ifndef SLIM_PROTOCOL_HANDLER_H
#define SLIM_PROTOCOL_HANDLER_H

#define LOG_MODULE "SlimServer 5.4.0+ Input Plugin (protocol decoder): "
#define LOG_VERBOSE 1

#include "xine_internal.h"
// #include "xineutils.h"

// sending helo

int slim_protocol_make_helo(char **outBuffer, int *len, unsigned char deviceID, unsigned char deviceRevision, unsigned char mac[6]);
int slim_protocol_make_ack(char **outBuffer, int *len, 
								char code[4],  // the type of the ack to send
								unsigned char crlf, // ?
								unsigned char masInit, // ? 
								unsigned char masMode, // ? 
								int rptr, int wptr, // ?
								long bytesRx, int jiffies);

typedef enum _CommandType 
{
	COMMAND_VERSION		= 0,
	COMMAND_STREAM		= 1,
	COMMAND_I2C			= 2,
	COMMAND_VFDISPLAY	= 3,	
	COMMAND_DISPLAY		= 4
} CommandType;

struct slimCommand 
{
	CommandType commandType;

	union {
		struct _versionCommand {
			char *versionData;			
			unsigned int versionDataLength;
		} version;

		struct _vfDisplayCommand {
			unsigned char	ddramAddr;
			unsigned char	cgramAddr;
			char			setDdram;
			char			brightness;
			char			displayOn;
			unsigned char	entryMode;
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

int slim_protocol_decode(unsigned char *buffer, unsigned int bufferLength, xine_t *xine_session, struct slimCommand *decodedCommand);

#endif // SLIM_PROTOCOL_HANDLER_H