/*
 SlimCommandHandler

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef SLIMCOMMANDHANDLER_H
#define SLIMCOMMANDHANDLER_H

#include "SocketCommunicator.h"
#include "SlimServerClientUser.h"

#include <string>

#include <netinet/in.h>

#include "SocketOperationListener.h"

using std::string;

typedef enum _CommandType
{
	COMMAND_VERSION		= 0,
	COMMAND_STREAM		= 1,
	COMMAND_I2C			= 2,
	COMMAND_VFDISPLAY	= 3,
	COMMAND_DISPLAY		= 4,
	COMMAND_VISU		= 5,
	COMMAND_AUDG		= 6,
	COMMAND_GRFB		= 7,
	COMMAND_GRFE		= 8
} CommandType;

typedef enum _StreamManagementCommandType
{
	STREAM_NO_COMMAND,
	STREAM_UNPAUSE,
	STREAM_PAUSE,
	STREAM_START,
	STREAM_QUIT,
	STREAM_FLUSH,
	STREAM_STATUS
} StreamManagementCommandType;

typedef enum _StreamFormatType
{
	STREAM_FORMAT_UNKNOWN = 0,
	STREAM_FORMAT_MP3 = 1,
	STREAM_FORMAT_FLAC = 2,
	STREAM_FORMAT_PCM = 3
} StreamFormatType;


class SlimServerClient;

struct slimProtocolCommand
{
	CommandType type;

	union _commands {
		struct _versionCommand {
			unsigned char *versionData;
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
		 	StreamManagementCommandType command;
			StreamFormatType			format;
			char						autoStart;

			struct in_addr				hostAddr;
			unsigned int				hostPort;

			unsigned int				urlSize;
			unsigned char				*urlAddress;
		} stream;

		struct _i2cCommand {
			char code;
		} i2c;

		struct _graphicCommand {
			char code;
		} grafic;

		struct _visuCommand {

		} visu;
	} data;
};


class SlimCommandHandler: public SocketCommunicator, public SlimServerClientUser, public SocketOperationListener
{
private:
	unsigned char 	m_commandBuffer[1024 * 50];
	unsigned char 	m_macAddress[6];

	struct slimProtocolCommand protocolCommand;

	bool bLogControlProtocol;

	struct timeval lastStatusTimeVal;

private:
//	int sendBuffer(unsigned char *buffer, unsigned int start, unsigned int len);

//	bool readBuffer(unsigned char *buffer, unsigned int start, unsigned int freeLen, int &readLen);

	void dumpCommand();

	bool readCommand(int &readSize);
	bool decodeCommand(int readSize);
	bool processCommand();

	bool decodeStrmCommand(unsigned char *buffer, int size);
	bool decodeVfdcCommand(unsigned char *buffer, int size);
	bool decodeI2ccCommand(unsigned char *buffer, int size);
	bool decodeVisuCommand(unsigned char *buffer, int size);
	bool decodeAudgCommand(unsigned char *buffer, int size);
	bool decodeGrfbCommand(unsigned char *buffer, int size);
	bool decodeGrfeCommand(unsigned char *buffer, int size);

	bool processStrmCommand();

	bool quitStreamingClient();
	bool startStreamingClient();
	bool unpauseStreamingClient();

public:
    SlimCommandHandler();

    ~SlimCommandHandler();

	void setMacAddress(string strMacAddress);

	bool closeConnection();

	// protocol related functions
	bool doHello();
	bool doStatus(char *statusType);
	bool doIR(char format, char noBits, int irCode);

	// helper functions
	void doOneCommand();

	bool needToSendStatus();

	// base interface implementations
	virtual bool dataIsAvailable(int socket);
};

#endif
