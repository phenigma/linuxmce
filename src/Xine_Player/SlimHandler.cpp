/*
	SlimHandler

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#include "SlimHandler.h"

#include "DCE/Logger.h"

using namespace DCE;

SlimHandler::SlimHandler()
{
	m_iConnection = 0;
}


SlimHandler::~SlimHandler()
{

}

int SlimHandler::sendBuffer(unsigned char *buffer, unsigned int start, unsigned int len)
{
	int sentCharacters = 0;

	sentCharacters = send(m_iConnection, buffer + start, len, 0);

	g_pPlutoLogger->Write(LV_STATUS, "result from send was %d", sentCharacters);

	return sentCharacters;
}

bool SlimHandler::readBuffer(unsigned char *buffer, unsigned int start, unsigned int freeLen, int &readLen)
{
	if ( freeLen <= 2 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "The buffer is too small");
		readLen = 0;
		return false;
	}

	unsigned int frameLength;
	if ( recv (m_iConnection, buffer + start, 2, 0) != 2 )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not read the buffer size!");
		readLen = 0;
		return false;
	}

	frameLength = (unsigned int)(int)(buffer[start] << 8 | buffer[start + 1]);

	if ( frameLength >= freeLen)
	{
		g_pPlutoLogger->Write(LV_STATUS, "The buffer is too small: required %d", frameLength);
		readLen = 0;
		return false;
	}

	if ( recv (m_iConnection, buffer, frameLength, 0) != (int)frameLength)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not read all the bytes %d!", frameLength);
		readLen = 0;
		return false;
	}

	readLen = frameLength;
	return true;
}

bool SlimHandler::doHello()
{
	if ( m_iConnection == 0 )
		return false;

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
	m_commandBuffer[16] = 0x00;
	m_commandBuffer[17] = 0x00;

	return sendBuffer(m_commandBuffer, 0, 18) == 18;
}

void SlimHandler::setConnectionSocket(unsigned int iConnection)
{
	m_iConnection = iConnection;
}

void SlimHandler::setMacAddress(string strMacAddress)
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

void SlimHandler::doOneCommand()
{
	int readSize;
	if ( ! readBuffer(m_commandBuffer, 0, 1024 * 50 - 1, readSize) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Error reading command");
		return;
	}

	if ( readSize == 0 )
		return;

	if ( ! decodeCommand(m_commandBuffer, readSize) )
	{
		g_pPlutoLogger->Write(LV_STATUS, "Could not decode command: %s", m_commandBuffer);
		return;
	}

	dumpCommand();
}

bool SlimHandler::decodeCommand(unsigned char *buffer, int size)
{
	if ( strncmp((const char*)buffer, "vers", 4) == 0 )
	{
		protocolCommand.type = COMMAND_VERSION;
		protocolCommand.data.version.versionData = buffer + 4;
		protocolCommand.data.version.versionDataLength = (unsigned int)strlen((char*)protocolCommand.data.version.versionData);
		return 1;
	}

	if ( strncmp((const char*)buffer, "strm", 4) == 0 )
		return decodeStrmCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "vfdc", 4) == 0 )
		return decodeVfdcCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "i2cc", 4) == 0 )
		return decodeI2ccCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "visu", 4) == 0 )
		return decodeVisuCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "audg", 4) == 0 )
		return decodeAudgCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "grfb", 4) == 0 )
		return decodeGrfbCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "grfe", 4) == 0 )
		return decodeGrfeCommand(buffer + 4, size - 4);

	if ( strncmp((const char*)buffer, "grfd", 4) == 0 )
	{
		protocolCommand.type = COMMAND_DISPLAY;
		return true;
	}

	g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeCommand() Cound not interpret protocol command: %s", buffer);
	return false;
}

bool SlimHandler::decodeStrmCommand(unsigned char *buffer, int size)
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
		g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeStrmCommand() Invalid stream control command %c. Ignoring!", buffer[0]);
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
		g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeStrmCommand() Unknown streaming format: %c. Ignoring!", buffer[2]);
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

bool SlimHandler::decodeVfdcCommand(unsigned char *buffer, int size)
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

bool SlimHandler::decodeI2ccCommand(unsigned char *buffer, int size)
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

bool SlimHandler::decodeGrfbCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_GRFB;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeGrfbCommand() Unsupported command");
	return true;
}

bool SlimHandler::decodeGrfeCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_GRFE;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeGrfeCommand() Unsupported command");
	return true;
}

bool SlimHandler::decodeAudgCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_AUDG;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeAudgCommand() Unsupported command");
	return true;
}

bool SlimHandler::decodeVisuCommand(unsigned char *buffer, int size)
{
	protocolCommand.type = COMMAND_VISU;
//	g_pPlutoLogger->Write(LV_STATUS, "SlimHandler::decodeVisuCommand() Unsupported command");
	return true;
}

void SlimHandler::dumpCommand()
{
	switch(protocolCommand.type)
	{
	case COMMAND_VERSION:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command VERSION: ");
		g_pPlutoLogger->Write(LV_STATUS, "\tdata: %s: ", protocolCommand.data.version.versionData);
		g_pPlutoLogger->Write(LV_STATUS, "\tlength: %c: ", protocolCommand.data.version.versionDataLength);
		break;

	case COMMAND_STREAM:
		g_pPlutoLogger->Write(LV_STATUS, "Slim command STREAM: ");
		switch ( protocolCommand.data.stream.command )
		{
			case STREAM_NO_COMMAND: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: none"); break;
			case STREAM_START: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: start"); break;
			case STREAM_PAUSE: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: pause"); break;
			case STREAM_UNPAUSE: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: unpause"); break;
			case STREAM_QUIT: g_pPlutoLogger->Write(LV_STATUS, "\tsubcommand: quit"); break;
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
		g_pPlutoLogger->Write(LV_STATUS, "Slim command GRFE ");
		break;
	}
}
