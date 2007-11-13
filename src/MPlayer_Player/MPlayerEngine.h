#ifndef __MPLAYER_ENGINE_H__
#define __MPLAYER_ENGINE_H__

#include <string>
#include <map>

#define BLACK_MPEG_FILE "/usr/pluto/share/black.mpeg"

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
	
	string m_sCurrentFile;
	string m_sCurrentScreenshot;
	void SetCurrentFile(string sName);
	
	void SetCurrentScreenshot(string sName);
	string GetCurrentScreenshot();
	
	vector<string> m_vCurrentPlaylist;

public:
	enum SeekType {SEEK_RELATIVE_TIME=0, SEEK_ABSOLUTE_PERCENTAGE, SEEK_ABSOLUTE_TIME};
	enum EngineState {PLAYBACK_STARTED=1, PLAYBACK_FINISHED};
	
	MPlayerEngine();
	~MPlayerEngine();

	// executes command with no response
	void ExecuteCommand(string sCommand);

	// executes command with response
	bool ExecuteCommand(string sCommand, string sResponseName, string &sResponseValue);
	
	bool StartPlaylist(vector<string> &vFiles);
	void StopPlayback();
	
	float GetCurrentPosition();
	float GetFileLength();
	
	string GetAudioCodec();
	string GetVideoCodec();
	
	void Seek(float fValue, SeekType eType);
	EngineState GetEngineState();
	
	string GetCurrentFile();
	void GetScreenshot(int iWidth, int iHeight, char *&pData, int &iDataSize, string &sFormat, string &sCMD_Result);

private:
	bool StartPlayback(string sMediaFile);
	EngineState m_eEngineState;
	void SetEngineState(const EngineState &eValue);
};

void Tokenize(const string& str,
	      vector<string>& tokens,
       const string& delimiters = " ");

#endif
