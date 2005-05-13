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
#include "SlimCommandHandler.h"
#include "SlimDataHandler.h"
#include "SlimServerClient.h"

#include "DCE/Logger.h"

using namespace DCE;

SlimCommandHandler::SlimCommandHandler()
{
// #ifdef DEBUG
	bLogControlProtocol = true;
//#else
//	bLogControlProtocol = false;
//#endif
}


SlimCommandHandler::~SlimCommandHandler()
{

}

bool SlimCommandHandler::readCommand(int &commandBufferSize)
{
	unsigned int frameLength;

	if ( ! readData(m_commandBuffer, 0, 2, commandBufferSize) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not read the buffer size!");
	 	commandBufferSize = 0;
		return false;
	}

	frameLength = (unsigned int)(int)(m_commandBuffer[0] << 8 | m_commandBuffer[1]);

	if ( frameLength >= sizeof(m_commandBuffer) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "The buffer is too small: required %d available %d", frameLength, sizeof(m_commandBuffer));
		commandBufferSize = 0;
		return false;
	}

	if ( ! readData(m_commandBuffer, 0, frameLength, commandBufferSize))
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not read all the bytes %d!", frameLength);
		return false;
	}

	return true;
}

bool SlimCommandHandler::doIR(char format, char noBits, int irCode)
{
	if ( ! isCommunicationOpen() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Socket is not open yet. Can't do hello !");
		return false;
	}

	long jiffies = getSlimServerClient()->getSlimDataHandler()->getJiffies();

	strncpy((char*)m_commandBuffer, "IR  ", 4);
	int pos = 4;
	m_commandBuffer[pos++] = 0;
	m_commandBuffer[pos++] = 0;
	m_commandBuffer[pos++] = 0;
	m_commandBuffer[pos++] = 10;

	m_commandBuffer[pos++] = (jiffies >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (jiffies >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (jiffies >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (jiffies >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = format;
	m_commandBuffer[pos++] = noBits;

	m_commandBuffer[pos++] = (irCode >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (irCode >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (irCode >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (irCode >> 0x00) & 0xFF;

	int sentChars;
	return writeData(m_commandBuffer, 0, 18, sentChars);
}

bool SlimCommandHandler::doHello()
{
	if ( ! isCommunicationOpen() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Socket is not open yet. Can't do hello !");
		return false;
	}

	strncpy((char*)m_commandBuffer, "HELO", 4);
	m_commandBuffer[4] = 0;
	m_commandBuffer[5] = 0;
	m_commandBuffer[6] = 0;
	m_commandBuffer[7] = 10;

	m_commandBuffer[8] = 3;
	m_commandBuffer[9] = 0;

	m_commandBuffer[10] = m_macAddress[0];
	m_commandBuffer[11] = m_macAddress[1];
	m_commandBuffer[12] = m_macAddress[2];
	m_commandBuffer[13] = m_macAddress[3];
	m_commandBuffer[14] = m_macAddress[4];
	m_commandBuffer[15] = m_macAddress[5];
	m_commandBuffer[16] = 0x80;
	m_commandBuffer[17] = 0x00;

	int sentChars;
	return writeData(m_commandBuffer, 0, 18, sentChars);
}


bool SlimCommandHandler::doStatus(char *statusType)
{
	if ( ! isCommunicationOpen() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Socket is not open yet. Can't do status !");
		return false;
	}

	char crlf 	 = 0;
	char masInit = getSlimServerClient()->getSlimDataHandler()->getStreamFormat();
	char masMode = 1;

	long long bytesRX = getSlimServerClient()->getSlimDataHandler()->getBytesRx();
	unsigned int jiffies = getSlimServerClient()->getSlimDataHandler()->getJiffies();

	int decoderBufferSize = getSlimServerClient()->getSlimDataHandler()->getBufferSize();
	int decoderBufferFullness = getSlimServerClient()->getSlimDataHandler()->getBufferFilledSpace();
	int outputBufferSize = 0;
	int outputBufferFullness = 0;

	int elapsedSeconds = getSlimServerClient()->getPlaybackSeconds();

	if ( bLogControlProtocol )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Sending status command: %s", statusType);
		g_pPlutoLogger->Write(LV_STATUS, "\tcrlf: 0x%x", crlf);
		g_pPlutoLogger->Write(LV_STATUS, "\tmasInit: 0x%x", masInit);
		g_pPlutoLogger->Write(LV_STATUS, "\tmasMode: 0x%x", masMode);
		g_pPlutoLogger->Write(LV_STATUS, "\tdecoderBufferSize: %d", decoderBufferSize);
		g_pPlutoLogger->Write(LV_STATUS, "\tdecoderBufferFullness: %d", decoderBufferFullness);
		g_pPlutoLogger->Write(LV_STATUS, "\tbytesRX: %d", bytesRX);
		g_pPlutoLogger->Write(LV_STATUS, "\tjiffies: %d", jiffies);
		g_pPlutoLogger->Write(LV_STATUS, "\toutputBufferSize: %d", outputBufferSize);
		g_pPlutoLogger->Write(LV_STATUS, "\toutputBufferFullnes: %d", outputBufferFullness);
		g_pPlutoLogger->Write(LV_STATUS, "\telapsedSeconds: %d", elapsedSeconds);
	}

	int writeCount;
	int pos;

	strncpy((char*)m_commandBuffer, "STAT", 4);

	pos = 4;
	m_commandBuffer[pos++] = 0;
	m_commandBuffer[pos++] = 0;
	m_commandBuffer[pos++] = 0;
	m_commandBuffer[pos++] = 4 + 3 + 4 + 4 + 8 + 2 + 4 + 4 + 4 + 4;

	m_commandBuffer[pos++] = statusType[0];
	m_commandBuffer[pos++] = statusType[1];
	m_commandBuffer[pos++] = statusType[2];
	m_commandBuffer[pos++] = statusType[3];

	m_commandBuffer[pos++] = crlf;
	m_commandBuffer[pos++] = masInit;
	m_commandBuffer[pos++] = masMode;

	m_commandBuffer[pos++] = (decoderBufferSize >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (decoderBufferSize >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (decoderBufferSize >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (decoderBufferSize >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = (decoderBufferFullness >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (decoderBufferFullness >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (decoderBufferFullness >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (decoderBufferFullness >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = (char)(bytesRX >> 0x38) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x30) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x28) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x20) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (char)(bytesRX >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = 0xFF;
	m_commandBuffer[pos++] = 0xFF;

	m_commandBuffer[pos++] = (jiffies >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (jiffies >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (jiffies >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (jiffies >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = (outputBufferSize >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (outputBufferSize >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (outputBufferSize >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (outputBufferSize >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = (outputBufferFullness >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (outputBufferFullness >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (outputBufferFullness >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (outputBufferFullness >> 0x00) & 0xFF;

	m_commandBuffer[pos++] = (elapsedSeconds >> 0x18) & 0xFF;
	m_commandBuffer[pos++] = (elapsedSeconds >> 0x10) & 0xFF;
	m_commandBuffer[pos++] = (elapsedSeconds >> 0x08) & 0xFF;
	m_commandBuffer[pos++] = (elapsedSeconds >> 0x00) & 0xFF;

	if ( gettimeofday(&lastStatusTimeVal, NULL) == -1 )
		g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::doStatus() gettimeofday failed %s", strerror(errno));

	return writeData(m_commandBuffer, 0, pos, writeCount);
}
void SlimCommandHandler::setMacAddress(string strMacAddress)
{
	const char *startChar;
	char *endChar;

	unsigned int index, value;

	index = 0;
	startChar = strMacAddress.c_str();
	while ( *endChar != 0 )
	{
		value = strtol(startChar, &endChar, 16);
		m_macAddress[index++] = value & 0xFF;
		if ( *endChar )
			startChar = ++endChar;
	}
}

void SlimCommandHandler::doOneCommand()
{
	int readSize;
	if ( ! readCommand(readSize) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Error reading command");
		return;
	}

	if ( readSize == 0 )
		return;

	if ( ! decodeCommand(readSize) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not decode command: %s", m_commandBuffer);
		return;
	}

	dumpCommand();

	processCommand();
}

bool SlimCommandHandler::processCommand()
{
	switch ( protocolCommand.type )
	{
		case COMMAND_STREAM: return processStrmCommand();
		default: return true;
	}
}

bool SlimCommandHandler::processStrmCommand()
{
	switch(protocolCommand.data.stream.command)
	{
		case STREAM_START:   return startStreamingClient();
		case STREAM_UNPAUSE: return unpauseStreamingClient();
		default:
			g_pPlutoLogger->Write(LV_STATUS, "Command not implemented yet!");
			return false;
	}
}

bool SlimCommandHandler::decodeCommand(int size)
{
	if ( strncmp((const char*)m_commandBuffer, "vers", 4) == 0 )
	{
		protocolCommand.type = COMMAND_VERSION;
		protocolCommand.data.version.versionData = m_commandBuffer + 4;
		protocolCommand.data.version.versionDataLength = (unsigned int)strlen((char*)protocolCommand.data.version.versionData);
		return 1;
	}

	if ( strncmp((const char*)m_commandBuffer, "strm", 4) == 0 )
		return decodeStrmCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "vfdc", 4) == 0 )
		return decodeVfdcCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "i2cc", 4) == 0 )
		return decodeI2ccCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "visu", 4) == 0 )
		return decodeVisuCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "audg", 4) == 0 )
		return decodeAudgCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "grfb", 4) == 0 )
		return decodeGrfbCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "grfe", 4) == 0 )
		return decodeGrfeCommand(m_commandBuffer + 4, size - 4);

	if ( strncmp((const char*)m_commandBuffer, "grfd", 4) == 0 )
	{
		protocolCommand.type = COMMAND_DISPLAY;
		return true;
	}

	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeCommand() Cound not interpret protocol command: %s", m_commandBuffer);
	return false;
}

bool SlimCommandHandler::decodeStrmCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_STREAM;

	// get the primary command;
	if ( buffer[0] == 's' )
		protocolCommand.data.stream.command = STREAM_START;
	else if ( buffer[0] == 'u' )
		protocolCommand.data.stream.command = STREAM_UNPAUSE;
	else if ( buffer[0] == 'p' )
		protocolCommand.data.stream.command = STREAM_PAUSE;
	else if ( buffer[0] == 'q' )
		protocolCommand.data.stream.command = STREAM_QUIT;
	else if ( buffer[0] == 'f' )
		protocolCommand.data.stream.command = STREAM_FLUSH;
	else if ( buffer[0] == 't' )
		protocolCommand.data.stream.command = STREAM_STATUS;
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeStrmCommand() Invalid stream control command %c. Ignoring!", buffer[0]);
		protocolCommand.data.stream.command = STREAM_NO_COMMAND;
		return false;
	}

	if ( buffer[2] == 'm' )
		protocolCommand.data.stream.format = STREAM_FORMAT_MP3;
	else if ( buffer[2] == 'f' )
		protocolCommand.data.stream.format = STREAM_FORMAT_FLAC;
	else if ( buffer[2] == 'p' )
		protocolCommand.data.stream.format = STREAM_FORMAT_PCM;
	else
	{
		protocolCommand.data.stream.format = STREAM_FORMAT_UNKNOWN;
		g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeStrmCommand() Unknown streaming format: %c. Ignoring!", buffer[2]);
		return false;
	}

	protocolCommand.data.stream.autoStart = buffer[1];
	protocolCommand.data.stream.hostPort = (buffer[18] << 0x08) | buffer[19];

	protocolCommand.data.stream.hostAddr.s_addr = (buffer[20] << 0x18) | (buffer[21] << 0x10) | (buffer[22] << 0x08) | buffer[23];

	protocolCommand.data.stream.urlAddress = buffer + 24;
	protocolCommand.data.stream.urlSize = 0;

	// the buffer + 24 is a classical GET format
	// GET /url HTTP/1.0\n\n
	// skip the GET part without crossing the buffer margins.
	while ( (protocolCommand.data.stream.urlAddress - buffer) < size &&
			*protocolCommand.data.stream.urlAddress != ' ' )
		protocolCommand.data.stream.urlAddress++;

	// skip whitespace after GET without crossing buffer margins.
	while ( (protocolCommand.data.stream.urlAddress - buffer) < size &&
			*protocolCommand.data.stream.urlAddress == ' ' )
		protocolCommand.data.stream.urlAddress++;

	// if we crossed the buffer this is an error.
	if ( (protocolCommand.data.stream.urlAddress - buffer) == size )
		return false;

	// walk the complete URL until the next space (an url can't have spaces in it.
	while ( (protocolCommand.data.stream.urlAddress - buffer + protocolCommand.data.stream.urlSize) < (unsigned int)size &&
		   *(protocolCommand.data.stream.urlAddress + protocolCommand.data.stream.urlSize) != ' ')
		   protocolCommand.data.stream.urlSize++;

	// if we crossed the buffer this is an error.
	if ( ((protocolCommand.data.stream.urlAddress - buffer) +  protocolCommand.data.stream.urlSize) == (unsigned int)size )
		return false;

	return true;
}

bool SlimCommandHandler::decodeVfdcCommand(unsigned char *buffer, int size)
{
	unsigned char *internalBuffer;
	unsigned int internalBufferPos;

	protocolCommand.type = COMMAND_VFDISPLAY;
	internalBuffer = new unsigned char[size];
	internalBufferPos = 0;

	for ( int i = 0; i < size; i += 2 )
	{
		switch ( buffer[i] )
		{
		case 0x00:
			g_pPlutoLogger->Write(LV_STATUS, "VFD packet type 0x00 not implemented!", buffer[i + 1] & 0x3F);
			break;
		case 0x02:
			if ( (buffer[i + 1] & 0x80) == 0x80 )
			{
				protocolCommand.data.vfDisplay.ddramAddr = buffer[i + 1] & 0x7F;
				protocolCommand.data.vfDisplay.setDdram = 1;
				g_pPlutoLogger->Write(LV_STATUS, "DDRAM addr set to %d", buffer[i + 1] & 0x3F);
			}
			else if ( (buffer[i + 1] & 0x40) == 0x40 )
			{
				protocolCommand.data.vfDisplay.ddramAddr = 0;
				protocolCommand.data.vfDisplay.cgramAddr = buffer[i + 1] & 0x3F;
				g_pPlutoLogger->Write(LV_STATUS, "CGRAM addr set to %d", buffer[i + 1] & 0x3F);
			}
			else if ( (buffer[i + 1] & 0x20) == 0x20 )
			{
				protocolCommand.data.vfDisplay.brightness = 32 - buffer[i + 7] * 4;
				i += 6;
				g_pPlutoLogger->Write(LV_STATUS, "Brightness set to: %d", 32 - buffer[i + 7] * 4);
			}
			else if ( (buffer[i + 1] & 0x10) == 0x10 )
			{
				g_pPlutoLogger->Write(LV_STATUS, "VFD packet type 0x02 \"Cursor or display shift not implemented\"!");
			}
			else if ( (buffer[i + 1] & 0x08) == 0x08 )
			{
				protocolCommand.data.vfDisplay.displayOn = (( buffer[i + 1] & 0x04 ) == 0x04);
				g_pPlutoLogger->Write(LV_STATUS, "Display set to: %d", protocolCommand.data.vfDisplay.displayOn);
			}
			else if ( (buffer[i + 1] & 0x04) == 0x04 )
			{
				protocolCommand.data.vfDisplay.entryMode = (( buffer[i + 1] & 0x03 ) == 0x03);
				g_pPlutoLogger->Write(LV_STATUS, "Entry mode set to: %d", protocolCommand.data.vfDisplay.entryMode);
			}
			else if ((buffer[i + 1 ] & 0x02) == 0x02)
			{
				protocolCommand.data.vfDisplay.ddramAddr = 0;
				protocolCommand.data.vfDisplay.setDdram = 1;
				g_pPlutoLogger->Write(LV_STATUS, "Resetting cursor");
			}
			else if ((buffer[i + 1] & 0x01) == 0x01)
			{
				// TODO: See if that data is usable
				//for (int j = framebufLive; j < framebufMask; j++)
				// framebuf[j] = 0x00;
				protocolCommand.data.vfDisplay.ddramAddr = 0;
				protocolCommand.data.vfDisplay.entryMode = 0x02;
				g_pPlutoLogger->Write(LV_STATUS, "Clearing display");
			}
			break;
		case 0x03:
			// if ( protocolCommand.data.vfDisplay.setDdram )
			internalBuffer[internalBufferPos++] = buffer[i+1];
			break;
		default:
			g_pPlutoLogger->Write(LV_STATUS, "Unknown VFD packet code: 0x%x", buffer[i]);
		}
	}

	return true;
}

bool SlimCommandHandler::decodeI2ccCommand(unsigned char *buffer, int size)
{
	int i;

	protocolCommand.type = COMMAND_I2C;

	i = 0;
	while ( i < size )
	{
		switch(buffer[i++])
		{
		case 's':
			if ( buffer[i] != 0x3e )
				g_pPlutoLogger->Write(LV_STATUS, "Unknown i2cc 's' command address 0x%x", buffer[i++]);
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
			g_pPlutoLogger->Write(LV_STATUS, "Unknown i2cc command %c", buffer[i++]);
			while ( i < size && buffer[i++] != 's');
		}
	}

	return true;
}

bool SlimCommandHandler::decodeGrfbCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_GRFB;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeGrfbCommand() Unsupported command");
	return true;
}

bool SlimCommandHandler::decodeGrfeCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_GRFE;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeGrfeCommand() Unsupported command");
	return true;
}

bool SlimCommandHandler::decodeAudgCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_AUDG;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeAudgCommand() Unsupported command");
	return true;
}

bool SlimCommandHandler::decodeVisuCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_VISU;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::decodeVisuCommand() Unsupported command");
	return true;
}

void SlimCommandHandler::dumpCommand()
{
	if ( ! bLogControlProtocol )
		return;

	switch(protocolCommand.type)
	{
	case COMMAND_VERSION:
		g_pPlutoLogger->Write(LV_STATUS, "0x%x Slim command VERSION: ", pthread_self());
		g_pPlutoLogger->Write(LV_STATUS, "0x%x\tdata: %s: ", pthread_self(), protocolCommand.data.version.versionData);
		g_pPlutoLogger->Write(LV_STATUS, "0x%x\tlength: %c: ", pthread_self(), protocolCommand.data.version.versionDataLength);
		break;

	case COMMAND_STREAM:
		g_pPlutoLogger->Write(LV_STATUS, "0x%x Slim command STREAM: ", pthread_self());
		switch ( protocolCommand.data.stream.command )
		{
			case STREAM_NO_COMMAND: g_pPlutoLogger->Write(LV_STATUS, "0x%x \tsubcommand: none", pthread_self()); break;
			case STREAM_START: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: start"); break;
			case STREAM_PAUSE: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: pause"); break;
			case STREAM_UNPAUSE: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: unpause"); break;
			case STREAM_QUIT: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: quit"); break;
			case STREAM_FLUSH: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: flush"); break;
			case STREAM_STATUS: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: status"); break;
		}
		switch ( protocolCommand.data.stream.format )
		{
			case STREAM_FORMAT_UNKNOWN: g_pPlutoLogger->Write(LV_STATUS, "\tformat: unknown"); break;
			case STREAM_FORMAT_MP3: g_pPlutoLogger->Write(LV_STATUS, "\tformat: mp3"); break;
			case STREAM_FORMAT_FLAC: g_pPlutoLogger->Write(LV_STATUS, "\tformat: flac"); break;
			case STREAM_FORMAT_PCM: g_pPlutoLogger->Write(LV_STATUS, "\tformat: pcm"); break;
		}

		g_pPlutoLogger->Write(LV_STATUS, "\turl addr size: %d", protocolCommand.data.stream.urlSize);
		g_pPlutoLogger->Write(LV_STATUS, "\turl addr: %s", protocolCommand.data.stream.urlAddress);

		g_pPlutoLogger->Write(LV_STATUS, "\tautostart: %c", protocolCommand.data.stream.autoStart);

		g_pPlutoLogger->Write(LV_STATUS, "\thost addr: %d.%d.%d.%d",
					protocolCommand.data.stream.hostAddr.s_addr >> 0x18 & 0xFF,
					protocolCommand.data.stream.hostAddr.s_addr >> 0x10 & 0xFF,
					protocolCommand.data.stream.hostAddr.s_addr >> 0x08 & 0xFF,
					protocolCommand.data.stream.hostAddr.s_addr >> 0x00 & 0xFF);
		g_pPlutoLogger->Write(LV_STATUS, "\thost port: %d", protocolCommand.data.stream.hostPort);
		break;

	case COMMAND_I2C:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command I2C: ");
		g_pPlutoLogger->Write(LV_STATUS, "\tcode: %c: ", protocolCommand.data.i2c.code);
		break;

	case COMMAND_VFDISPLAY:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command VFDISPLAY: ");
		g_pPlutoLogger->Write(LV_STATUS, "\tddramAddr: %c: ", protocolCommand.data.vfDisplay.ddramAddr);
		g_pPlutoLogger->Write(LV_STATUS, "\tcgramAddr: %c: ", protocolCommand.data.vfDisplay.cgramAddr);
		g_pPlutoLogger->Write(LV_STATUS, "\tsetDdram: %c: ", protocolCommand.data.vfDisplay.setDdram);
		g_pPlutoLogger->Write(LV_STATUS, "\tbrightness: %c: ", protocolCommand.data.vfDisplay.brightness);
		g_pPlutoLogger->Write(LV_STATUS, "\tdisplayOn: %c: ", protocolCommand.data.vfDisplay.displayOn);
		g_pPlutoLogger->Write(LV_STATUS, "\tentryMode: %c: ", protocolCommand.data.vfDisplay.entryMode);
		break;

	case COMMAND_DISPLAY:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command DISPLAY ");
		break;

	case COMMAND_VISU:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command VISU ");
		break;
	case COMMAND_AUDG:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command AUDG ");
		break;
	case COMMAND_GRFB:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command GRFB ");
		break;
	case COMMAND_GRFE:
		g_pPlutoLogger->Write(LV_STATUS, "0x%x Slim command GRFE ", pthread_self());
		break;
	}
}

bool SlimCommandHandler::unpauseStreamingClient()
{
	SlimDataHandler *pSlimDataHandler = getSlimServerClient()->getSlimDataHandler();

	string strHostName = getSlimServerClient()->getHostName();

	if ( protocolCommand.data.stream.hostAddr.s_addr != 0 )
		strHostName = StringUtils::Format("%d.%d.%d.%d",
				protocolCommand.data.stream.hostAddr.s_addr >> 0x18 & 0xFF,
				protocolCommand.data.stream.hostAddr.s_addr >> 0x10 & 0xFF,
				protocolCommand.data.stream.hostAddr.s_addr >> 0x08 & 0xFF,
				protocolCommand.data.stream.hostAddr.s_addr >> 0x00 & 0xFF);

	pSlimDataHandler->setConnectionData(strHostName, protocolCommand.data.stream.hostPort, protocolCommand.data.stream.urlAddress, protocolCommand.data.stream.urlSize);

	if ( ! pSlimDataHandler->openConnection() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "bool SlimCommandHandler::startStreamingClient() Opening data connection failed!");
		return false;
	}

	if ( ! doStatus("STMr") )
	{
		pSlimDataHandler->closeCommunication();
		return false;
	}

	getSlimServerClient()->getDataReaderThread()->addSocketOperationListenerForSocket(pSlimDataHandler->getCommunicationSocket(), pSlimDataHandler);

	return true;
}

bool SlimCommandHandler::startStreamingClient()
{
	SlimDataHandler *pSlimDataHandler = getSlimServerClient()->getSlimDataHandler();

	string strHostName = getSlimServerClient()->getHostName();

	if ( protocolCommand.data.stream.hostAddr.s_addr != 0 )
	{
		strHostName = StringUtils::Format("%d.%d.%d.%d",
				protocolCommand.data.stream.hostAddr.s_addr >> 0x18 & 0xFF,
				protocolCommand.data.stream.hostAddr.s_addr >> 0x10 & 0xFF,
				protocolCommand.data.stream.hostAddr.s_addr >> 0x08 & 0xFF,
				protocolCommand.data.stream.hostAddr.s_addr >> 0x00 & 0xFF);
	}

	pSlimDataHandler->setConnectionData(strHostName, protocolCommand.data.stream.hostPort, protocolCommand.data.stream.urlAddress, protocolCommand.data.stream.urlSize);

	if ( protocolCommand.data.stream.format == STREAM_FORMAT_MP3 )
	{
		pSlimDataHandler->setStreamFormat('m');
	}
	else if ( protocolCommand.data.stream.format == STREAM_FORMAT_FLAC )
	{
		pSlimDataHandler->setStreamFormat('f');
	}
	else if ( protocolCommand.data.stream.format == STREAM_FORMAT_PCM )
	{
		pSlimDataHandler->setStreamFormat('p');
	}
	else
	{
		pSlimDataHandler->setStreamFormat(' ');
	}

	if ( ! pSlimDataHandler->openConnection() )
	{
		g_pPlutoLogger->Write(LV_STATUS, "bool SlimCommandHandler::startStreamingClient() Opening data connection failed!");
		return false;
	}

	if ( ! doStatus("STMc") )
	{
		pSlimDataHandler->closeCommunication();
		return false;
	}

	if ( ! doStatus("STMh") )
	{
		pSlimDataHandler->closeCommunication();
		return false;
	}

	getSlimServerClient()->getDataReaderThread()->addSocketOperationListenerForSocket(pSlimDataHandler->getCommunicationSocket(), pSlimDataHandler);

	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::startStreamingClient() Before pSlimDataHandler->initDataProcessing!");
	pSlimDataHandler->initDataProcessing(protocolCommand.data.stream.autoStart - '0');
	g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::startStreamingClient() After pSlimDataHandler->initDataProcessing!");

	return true;
}

bool SlimCommandHandler::needToSendStatus()
{
	struct timeval currentTimeVal;
	if ( gettimeofday(&currentTimeVal, NULL) == -1 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "SlimCommandHandler::needToSendStatus() gettimeofday failed: %s", strerror(errno));
		return false;
	}

	if ( ((currentTimeVal.tv_sec - lastStatusTimeVal.tv_sec) * 1000 + (currentTimeVal.tv_usec - lastStatusTimeVal.tv_usec) / 1000 ) > 1000 )
		return true;

	return false;
}

bool SlimCommandHandler::dataIsAvailable(int socket)
{
	if ( socket == getCommunicationSocket() )
		doOneCommand();

	if ( needToSendStatus() )
	{
		if ( getSlimServerClient()->getSlimDataHandler()->getBufferFilledSpace() == 0 )
			doStatus("STMd");
		else
			doStatus("STMt");
	}

	return true;
}
