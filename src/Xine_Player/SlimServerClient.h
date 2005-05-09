#ifndef SLIMSERVERCLIENT_H
#define SLIMSERVERCLIENT_H

#include <string>
#include <pthread.h>

#include "SlimCommandHandler.h"
// #include "SlimDataBufferHandler.h"
#include "PlutoUtils/MultiThreadIncludes.h"

class SlimDataHandler;
class XineSlaveWrapper;

class SlimServerClient
{
public:
    typedef enum threadState
	{
		THREAD_STOPPED,
		THREAD_CONNECTING,
		THREAD_CONNECTED,
		THREAD_PLAYING,
		THREAD_PAUSED,
		THREAD_EXITING
	} ThreadStateType;

    typedef enum threadCommand
	{
		THREAD_UNKNOWN,
		THREAD_START,
		THREAD_STOP,
		THREAD_PLAY,
		THREAD_PAUSE,
		THREAD_EXIT
	} ThreadCommandType;

private:

	ThreadStateType 		dataThreadState, commandThreadState;
	ThreadCommandType		dataThreadCommand, commandThreadCommand;

	XineSlaveWrapper 		*m_pXineSlave;
	pluto_pthread_mutex_t   *m_pXineSlaveMutex;

	SlimDataHandler 	*m_pSlimDataHandler;
	SlimCommandHandler 	*m_pSlimCommandHandler;

	std::string 	m_strHostname;
	int 			m_iPort;

	pthread_t	commandThread;
	pthread_t	dataThread;

	int 		m_iStreamID;
	int 		m_iRequestingObjectID;
private:
	bool parse_server_address(std::string strConnectionString);

public:
    SlimServerClient();

	virtual bool commandDataThread(ThreadCommandType threadCommand);
	virtual bool connectToServer(std::string serverUrl, int controlledStreamID);
	virtual bool isConnecting(int iStreamID);
	virtual bool isConnected(int iStreamID);
	virtual void disconnectFromServer(int iStreamID);

	virtual void setXineSlaveObject(XineSlaveWrapper *pXineSlaveControl);
	virtual void setXineSlaveMutex(pluto_pthread_mutex_t *pXineSlaveMutex);
	virtual void setMacAddress(std::string strMacAddress);

    virtual ~SlimServerClient();

	SlimDataHandler		*getSlimDataHandler();
	SlimCommandHandler	*getSlimCommandHandler();

	int getPlaybackSeconds();

	std::string getHostName();
	std::string getFifoName();

	bool startDataReader(std::string fifoFileName, bool autostart);

	void setMediaStreamID(int iStreamID);
	void setRequestingObjectID(int iRequestingObjectID);

	static void *controlConnectionThread(void *arguments);
	static void *dataConnectionThread(void *arguments);
};

#endif
