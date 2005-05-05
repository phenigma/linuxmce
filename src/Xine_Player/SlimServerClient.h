#ifndef SLIMSERVERCLIENT_H
#define SLIMSERVERCLIENT_H

#include <string>
#include <pthread.h>

#include "SlimHandler.h"

class XineSlaveWrapper;

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
		THREAD_UNKNOWN,
		THREAD_START,
		THREAD_STOP,
		THREAD_EXIT
	} ThreadCommandType;

	ThreadStateType 		dataThreadState, commandThreadState;
	ThreadCommandType		dataThreadCommand, commandThreadCommand;

	XineSlaveWrapper *m_pXineSlave;
	SlimHandler *m_pSlimHandler;

	std::string 	m_strHostname;
	int 			m_iPort;

	pthread_t	commandThread;
	pthread_t	dataThread;

private:
	bool parse_server_address(std::string strConnectionString);

public:
    SlimServerClient();

	virtual bool connectToServer(std::string serverUrl, int controlledStreamID);
	virtual bool isConnecting(int iStreamID);
	virtual bool isConnected(int iStreamID);
	virtual void disconnectFromServer(int iStreamID);

	virtual void setXineSlaveObject(XineSlaveWrapper *pXineSlaveControl);
	virtual void setMacAddress(std::string strMacAddress);

    virtual ~SlimServerClient();

	SlimHandler *getSlimHandler();

	static void *controlConnectionThread(void *arguments);
	static void *dataConnectionThread(void *arguments);


};

#endif
