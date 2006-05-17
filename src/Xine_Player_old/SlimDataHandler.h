/*
	SlimDataHandler

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef SLIMDATAHANDLER_H
#define SLIMDATAHANDLER_H

#include <string>

#include "SocketCommunicator.h"
#include "SlimServerClientUser.h"
#include "SocketOperationListener.h"
#include "ThreadedStateMachine.h"

using std::string;

class SlimServerClient;

//#define BUFFER_SIZE (4*262144)
#define BUFFER_SIZE (262144)

#define BUFFER_FULLNESS_THRESHOLD_SIZE (262144 / 2)

#define NETWORK_BUFFER_SIZE (1024*25)

class SlimDataHandler: public SocketCommunicator, public SlimServerClientUser, public SocketOperationListener, public ThreadedStateMachine
{
public:
	class PlayerState {
	public:
		typedef enum _state {
			PLAYER_DISCONNECTED,
			PLAYER_CONNECTED,
			PLAYER_PLAYING,
			PLAYER_BUFFERING,
			PLAYER_PAUSED
		} STATE;

		static string describeState(PlayerState::STATE state);
	};

private:
	unsigned char 	*m_urlAddress;
	unsigned int 	m_urlAddressSize;
	unsigned char 	m_streamFormat;

	unsigned char networkBuffer[NETWORK_BUFFER_SIZE];

	unsigned char pipeBuffer[PIPE_BUF];

	unsigned char buffer[BUFFER_SIZE];
	unsigned int  readPtr;
	unsigned int  writePtr;

	long long bytesRx;

	pluto_pthread_mutex_t 	bufferAccessMutex;
	pthread_cond_t			bufferAccessCondition;

	string 	fifoPipeName;
	int		fifoFileDescriptor;

	struct timeval epochTime;

	bool m_isLogBufferAccess;
	bool m_isPlayerCreated;
	bool m_isAutoStart;

	PlayerState::STATE m_playerState;

protected:
	void resetBuffer();
	bool fillBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen);
	bool peekBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen);
	bool readBuffer(char *pBuffer, unsigned int pBufferOffset, unsigned int pBufferLen);
	bool readBuffer(unsigned int readSize);

	State STATE_CLOSED_PIPE;
	State STATE_WAIT_TO_SEND;
	State STATE_READY_TO_SEND;

	virtual void openPipe(State &fromState, State &toState);
	virtual void closePipe(State &fromState, State &toState);
	virtual void waitToSendDataThruPipe(State &fromState, State &toState);
	virtual void sendDataThruPipe(State &fromState, State &toState);
	virtual void quitMachine(State &fromState, State &toState);

	virtual State *findNextState(State *pCurrentState);
	virtual void resetMachine();
public:

    SlimDataHandler();

    ~SlimDataHandler();

	virtual bool openConnection();
	virtual bool closeConnection();

	bool initDataProcessing();

	bool createPlayer();
// 	bool startPlayer();
	bool pausePlayer();
	bool unpausePlayer();

	bool readStreamData();

	unsigned char getStreamFormat();
	unsigned int getReadPtr();
	unsigned int getWritePtr();
	unsigned int getBufferFreeSpace();
	unsigned int getBufferFilledSpace();

	unsigned int getPipeBufferSize();
	unsigned int getBufferThresoldLevel();

	long long getBytesRx();

	unsigned int getBufferSize();
	long long getJiffies();

	void setStreamFormat(unsigned char streamFormat);
	bool setConnectionData(string strHostName, int port, unsigned char *urlAddress, unsigned int urlAddressSize);

	PlayerState::STATE getPlayerState();
	void setPlayerState(PlayerState::STATE playerState);

	bool isAutoStart();
	void setAutoStart(bool isAutoStart);

	bool isPlayerCreated();
	void setPlayerCreated(bool playerCreated);

	// base interface implementations
	virtual bool dataIsAvailable(int socket);
};

#endif
