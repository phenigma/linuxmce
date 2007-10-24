#include <iostream>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>

#include "MPlayerEngine.h"
#include "Log.h"

#define BLACK_MPEG_FILE "/usr/pluto/share/black.mpeg"
#define MPLAYER_BINARY "/opt/pluto-mplayer/bin/mplayer"
#define MPLAYER_BINARY_SHORT "mplayer"

using namespace std;

MPlayerEngine::MPlayerEngine() {
	m_iChildPID = 0;
	m_bEngineIsRunning = false;
	m_bInPipe[0] = m_bInPipe[1] = false;
	m_bOutPipe[0] = m_bOutPipe[1] = false;
	m_bRunEngineOutputReader = false;
	
	StartEngine();
}

MPlayerEngine::~MPlayerEngine() {
	StopEngine();
	ClosePipes();
}

void MPlayerEngine::ClosePipes() {
	Log("Closing pipes");
	for (int i=0; i<2; i++) {
		if (m_bInPipe[i])
			close(m_iInPipe[i]);
		if (m_bOutPipe[i])
			close(m_iOutPipe[i]);
		m_bInPipe[i] = m_bOutPipe[i] = false;
	}
	Log("Pipes are closed");
}

bool MPlayerEngine::StartEngine() {
	// TODO add error reporting

	// initializing in pipe
	if ( pipe(m_iInPipe) == -1 ) {
		Log("pipe() failed for in pipe");
		return false;
	}
	else {
		m_bInPipe[0] = m_bInPipe[1] = true;
	}

	// initializing out pipe
	if ( pipe(m_iOutPipe) == -1 ) {
		Log("pipe() failed for out pipe");
		return false;
	}
	else {
		m_bOutPipe[0] = m_bOutPipe[1] = true;
	}

	int pid = fork();
	if (pid == -1) {
		Log("Fork failed!");
		return false;
	}

	if (pid == 0) {
		Log("I'm a child");

		// connecting child stdin to InPipe, stdout & stderr to OutPipe
		if ( dup2(m_iInPipe[0], STDIN_FILENO)<0 || dup2(m_iOutPipe[1], STDOUT_FILENO)<0 ||
				dup2(m_iOutPipe[1], STDERR_FILENO)<0 ) {
			Log("dup2() failed in child");
			printf("%d : %s\n", errno, strerror(errno));
			exit(127);
		}

		Log("Starting mplayer");
		
		execle(MPLAYER_BINARY, MPLAYER_BINARY_SHORT, "-slave", "-idle", "-msglevel", "all=4", "-noborder", 
		       "-fixed-vo", "-fs", "-vo", "xv", "-lavdopts", "fast:threads=2", BLACK_MPEG_FILE, (char *) 0, environ);

		Log("execle() failed");
		exit(127);
	}
	else {
		Log("I'm a parent");
		m_iChildPID = pid;
		
		// closing child's end of pipe 
		close(m_iInPipe[0]);
		m_bInPipe[0] = false;
		close(m_iOutPipe[1]);
		m_bOutPipe[1] = false;
		fcntl(m_iOutPipe[0], F_SETFL, O_NONBLOCK);
		
		m_bEngineIsRunning = true;

		m_bRunEngineOutputReader = true;
		// TODO error reporting here
		pthread_create(&m_tEngineOutputReaderThread, NULL, &EngineOutputReader, this);
	}

	return true;
}

void MPlayerEngine::StopEngine() {
	if (m_bEngineIsRunning) {
		// quitting engine and giving it time to actually quit
		ExecuteCommand("quit");
		sleep(1);
	
		// if it is still running, let's kill it
		kill(m_iChildPID, SIGKILL);
		m_iChildPID = 0;
	
		// exiting output reader thread
		if (m_bRunEngineOutputReader)
		{
			m_bRunEngineOutputReader = false;
			pthread_join(m_tEngineOutputReaderThread, NULL);
		}
		
		m_bEngineIsRunning = false;
	}
}

string MPlayerEngine::ReadLine() {
	string sResult;
	char buf[1024];
	int iSize = 0;
	do {
		iSize = read(m_iOutPipe[0], buf, 1023);
		if (iSize>0)
		{
			buf[iSize] = 0;
			sResult += buf;
		}
	} while (iSize > 0);
	
	return sResult;
}

void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters/* = " "*/)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void* EngineOutputReader(void *pInstance) {
	Log("EngineOutputReader - started");

	MPlayerEngine* pThis = (MPlayerEngine*) pInstance;

	pollfd pfd[1];
	pfd[0].fd = pThis->m_iOutPipe[0];
	pfd[0].events = POLLIN;
	
	while (pThis->m_bRunEngineOutputReader) {
		poll(pfd, 1, 300);
		if (pfd[0].revents & POLLIN)
		{
			string sLine = pThis->ReadLine();
			Log("Read line: " + sLine);
			vector<string> vLines;
			Tokenize(sLine, vLines, "\n");
			for (vector<string>::iterator i=vLines.begin(); i!=vLines.end(); ++i) {
				if ( strncmp(i->c_str(), "ANS_", 4) == 0 ) {
					Log("Found answer: " + *i);
					vector<string> vAnswer;
					Tokenize(*i, vAnswer, "=");
					string sValue;
					if ( vAnswer.size()>=2 ) {
						sValue = vAnswer[1];
					}

				// TODO add sync
					pThis->m_mEngineAnswers[vAnswer[0]] = sValue;
				}
			}
		}
	}

	Log("EngineOutputReader - stopped");

	return NULL;
}

void MPlayerEngine::ExecuteCommand(string sCommand) {
	//TODO check that engine runs
	char buf[1024];
	// TODO check that command is \n-terminated
	int iLen = snprintf(buf, 1024, "%s\n", sCommand.c_str());
	int iSize = write(m_iInPipe[1], buf, iLen);
	cout << "Wrote " << iSize << " bytes" << endl;
}

bool MPlayerEngine::ExecuteCommand(string sCommand, string sResponseName, string &sResponseValue) {
	//TODO check that engine runs
	
	sResponseName = "ANS_" + sResponseName;
	// TODO protect access
	m_mEngineAnswers[sResponseName] = sResponseName;
	ExecuteCommand(sCommand);
	// giving MPlayer time to reply
	sleep(1);
	
	// TODO protect access
	if ( m_mEngineAnswers[sResponseName] == sResponseName ) {
		sResponseValue = "";
		return false;
	}
	else {
		sResponseValue = m_mEngineAnswers[sResponseName];
		return true;
	}
}

void MPlayerEngine::StopPlayback() {
	string sCommand = "loadfile ";
	sCommand += BLACK_MPEG_FILE;
	ExecuteCommand(sCommand);
}

bool MPlayerEngine::StartPlayback(string sMediaFile) {
	// TODO add error handling
	string sCommand = "loadfile ";
	sCommand += sMediaFile;
	ExecuteCommand(sCommand);
	return true;
}

float MPlayerEngine::GetCurrentPosition() {
	string sCommand = "get_time_pos";
	string sTimePos;
	if ( ExecuteCommand(sCommand, "TIME_POSITION", sTimePos) ) {
		Log("Current stream time position: " + sTimePos);
		return atof(sTimePos.c_str());
	}
	else {
		Log("Failed to get stream time position");
		return 0.0;
	}
}

float MPlayerEngine::GetFileLength() {
	string sCommand = "get_time_length";
	string sTimeLen;
	if ( ExecuteCommand(sCommand, "LENGTH", sTimeLen) ) {
		Log("Current stream time length: " + sTimeLen);
		return atof(sTimeLen.c_str());
	}
	else {
		Log("Failed to get stream time length");
		return 0.0;
	}
}

string MPlayerEngine::GetAudioCodec() {
	string sCommand = "get_audio_codec";
	string sAudioCodec;
	if ( ExecuteCommand(sCommand, "AUDIO_CODEC", sAudioCodec) ) {
		Log("Current stream audio codec: " + sAudioCodec);
		return sAudioCodec;
	}
	else {
		Log("Failed to get stream audio codec");
		return "";
	}
}

string MPlayerEngine::GetVideoCodec() {
	string sCommand = "get_video_codec";
	string sVideoCodec;
	if ( ExecuteCommand(sCommand, "VIDEO_CODEC", sVideoCodec) ) {
		Log("Current stream video codec: " + sVideoCodec);
		return sVideoCodec;
	}
	else {
		Log("Failed to get stream video codec");
		return "";
	}
}

void MPlayerEngine::Seek(float fValue, SeekType eType) {
	string sCommand = "seek ";
	char buf[32];
	snprintf(buf, 32, "%.1f %i", fValue, eType);
	sCommand += buf;
	ExecuteCommand(sCommand);
}
