#ifndef __MPLAYER_ENGINE_H__
#define __MPLAYER_ENGINE_H__

#include <string>
#include <map>

using namespace std;

void* EngineOutputReader(void *pInstance);

class MPlayerEngine {
private:
	bool m_bEngineIsRunning;
	bool m_bRunEngineOutputReader;
	pthread_t m_tEngineOutputReaderThread;

	int m_iInPipe[2], m_iOutPipe[2];
	bool m_bInPipe[2], m_bOutPipe[2];

	friend void* EngineOutputReader(void *pInstance);

	void ClosePipes();

	int m_iChildPID;
	map<string,string> m_mEngineAnswers;

public:
	MPlayerEngine();
	~MPlayerEngine();

	bool StartEngine();
	string ReadLine();
	void Command(string sCommand);
	string Ask(string sCommand, string sResponse);
};

#endif
