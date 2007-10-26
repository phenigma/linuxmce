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

	int m_iChildPID;
	map<string,string> m_mEngineAnswers;

	bool StartEngine();
	void StopEngine();

	void ClosePipes();

	string ReadLine();

public:
	enum SeekType {SEEK_RELATIVE_TIME=0, SEEK_ABSOLUTE_PERCENTAGE, SEEK_ABSOLUTE_TIME};
	
	MPlayerEngine();
	~MPlayerEngine();

	// executes command with no response
	void ExecuteCommand(string sCommand);

	// executes command with response
	bool ExecuteCommand(string sCommand, string sResponseName, string &sResponseValue);
	
	bool StartPlayback(string sMediaFile);
	bool StartPlaylist(vector<string> &vFiles);
	void StopPlayback();
	
	float GetCurrentPosition();
	float GetFileLength();
	
	string GetAudioCodec();
	string GetVideoCodec();
	
	void Seek(float fValue, SeekType eType);
};

void Tokenize(const string& str,
	      vector<string>& tokens,
       const string& delimiters = " ");

#endif
