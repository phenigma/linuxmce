//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SLIMSERVERCLIENT_H
#define SLIMSERVERCLIENT_H

#include <string>
#include <pthread.h>

// #include "XineSlaveWrapper.h"

/**
*/
class SlimServerClient
{
    typedef enum threadState
	{
		THREAD_STOPPED,
		THREAD_CONNECTING,
		THREAD_CONNECTED,
		THREAD_EXITING
	} ThreadStateType;

    typedef enum threadCommand
	{
		THREAD_START,
		THREAD_STOP,
		THREAD_EXIT
	} ThreadCommandType;

	ThreadStateType 		dataThreadState, commandThreadState;
	ThreadCommandType		dataThreadCommand, commandThreadCommand;

	class XineSlaveWrapper *m_pXine;

	unsigned char 	m_macAddress[6];
	std::string 	m_strHostname;
	int 			m_iPort;

	pthread_t	commandThread;
	pthread_t	dataThread;

private:
	bool parse_server_address(std::string strConnectionString);

// slim_protocol_functions
	bool slim_do_hello(int connection);

public:
    SlimServerClient();

	virtual bool connectToServer(std::string serverUrl, int controlledStreamID);
	virtual bool isConnecting(int iStreamID);
	virtual bool isConnected(int iStreamID);
	virtual void disconnectFromServer(int iStreamID);

	virtual void setXineSlaveObject(class XineSlaveWrapper *pXineSlaveControl);
	virtual void setMacAddress(std::string strMacAddress);

    virtual ~SlimServerClient();

	static void *controlConnectionThread(void *arguments);
	static void *dataConnectionThread(void *arguments);
};

#endif
