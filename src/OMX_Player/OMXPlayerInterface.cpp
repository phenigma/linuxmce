#include <mutex> // mutex
#include <string> // to_string
#include <iostream> // cout
#include <fstream> // ifstream
#include <unistd.h> // execl/usleep
#include <sys/wait.h> // waitpid()
#include <fcntl.h> // fcntl (pipe redirection)

//#define USE_PIPES

const int RETRIES = 10;
const int DBUS_RETRIES = 200;

using namespace std;

#include "OMXPlayerInterface.h"
#include "KeyConfig.h"


// D-Bus interfaces proxies
OMXPlayerClient::OMXPlayerClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}

OMXPropsClient::OMXPropsClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}

OMXRootClient::OMXRootClient(DBus::Connection &connection, const char *path, const char *name)
  : DBus::ObjectProxy(connection, path, name)
{

}


// omx client start
static const char *OMXPLAYER_SERVER_NAME = "org.mpris.MediaPlayer2.omxplayer";
static const char *OMXPLAYER_SERVER_PATH = "/org/mpris/MediaPlayer2";

// FIXME: get current user
static const string OMXPLAYER_DBUS_ADDR = "/tmp/omxplayerdbus.root";
#define OMXPLAYER_DBUS_PID /tmp/omxplayerdbus.root.pid

// constructor
OMXPlayerInterface::OMXPlayerInterface(string sAudioDevice, bool bPassthrough, string sGpuDeInt) {
  // callback fn's set NULL so we know it's not registered
  m_fpStateNotifier = NULL;

  m_bOMXIsRunning = false;

  g_player_client = NULL;
  g_props_client = NULL;
  g_root_client = NULL;
  g_player_conn = NULL;
  g_props_conn = NULL;
  g_root_conn = NULL;

  m_bFinished = false;
  m_bStopped = true;
  m_sAudioDevice = sAudioDevice;
  m_bPassthrough = bPassthrough;

  m_bRunPlayerOutputReader = false;
  m_bRunPlayerMonitor = false;
  m_tPlayerOutputReaderThread = 0;
  m_tPlayerMonitorThread = 0;

  m_iChildPID = 0;

  m_bStoppedByReader = false;
  m_bStoppedByMonitor = false;
  m_iErrCount = 0;

  m_bInPipe[0] = m_bInPipe[1] = false;
  m_bOutPipe[0] = m_bOutPipe[1] = false;

  Set_Stopped(true);

  DBus::_init_threading();
  DBus::default_dispatcher = &dispatcher;
}

// destructor
OMXPlayerInterface::~OMXPlayerInterface() {
    Log("~OMXPlayerInterface() - DESTRUCTOR! - Calling DeInit()");
    SetState(STATE::EXITING);

    DeInit();
}

void OMXPlayerInterface::DeInit() {
  if (!IsFinished()) {
    Log("DeInit() - Calling Stop()"); 
    Stop();
    while (!IsFinished()) usleep(1000);
  }
}

void OMXPlayerInterface::Init() {
	if ( ePlayerState != STATE::UNKNOWN ) {
		Log("OMXPlayerInterface::Init - ERROR - Init() has already been run.");
		return;
	}
	SetState(STATE::STOPPED);
}


/* */
bool OMXPlayerInterface::Connect_Player() {
    const char *dbus_addr = m_s_dbus_addr.c_str();

    // setup the dbus 'Player' connection and client
    if (g_player_conn == NULL) {
      Log("Connect_Player() - Creating connection to 'Player' bus");
      bool bPrivate(true);
      g_player_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_player_conn->connected()) {
        Log("Connect_Player() - g_player_conn not connected(), exiting.");
        return false;
      }
      if (!g_player_conn->register_bus()) {
        Log("Connect_Player() - g_player_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_player_client == NULL) {
      Log("Connect_Player() - Creating Player_proxy client interface");
      g_player_client = new OMXPlayerClient(*g_player_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
      return true;
    }
    return false;
}

void OMXPlayerInterface::Disconnect_Player() {
    Log("Disconnect_Player() - cleaning up D-Bus interface proxies");
    // Clean up the player interfaces and connections;
    if (g_player_client != NULL)
      delete g_player_client;
    g_player_client = NULL;
    if (g_player_conn != NULL)
      delete g_player_conn;
    g_player_conn = NULL;
}

bool OMXPlayerInterface::Reconnect_Player() {
    Log("Reconnect_Player() - reconnecting dbus player interface");
    Disconnect_Player();
    return Connect_Player();
}
/* */

/* */
bool OMXPlayerInterface::Connect_Properties() {
    const char *dbus_addr = m_s_dbus_addr.c_str();

    // setup the dbus 'Player' connection and client
    if (g_props_conn == NULL) {
      Log("Connect_Properties() - Creating connection to 'Player' bus");
      bool bPrivate(true);
      g_props_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_props_conn->connected()) {
        Log("Connect_Properties() - g_props_conn not connected(), exiting.");
        return false;
      }
      if (!g_props_conn->register_bus()) {
        Log("Connect_Properties() - g_props_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_props_client == NULL) {
      Log("Connect_Properties() - Creating Player_proxy client interface");
      g_props_client = new OMXPropsClient(*g_props_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
      return true;
    }
    return false;
}

void OMXPlayerInterface::Disconnect_Properties() {
    Log("Disconnect_Properties() - cleaning up D-Bus interface proxies");
    // Clean up the properties interfaces and connections;
    if (g_props_client != NULL)
      delete g_props_client;
    g_props_client = NULL;
    if (g_props_conn != NULL)
      delete g_props_conn;
    g_props_conn = NULL;
}

bool OMXPlayerInterface::Reconnect_Properties() {
    Log("Reconnect_Properties() - reconnecting dbus player interface");
    Disconnect_Properties();
    return Connect_Properties();
}
/* */

/* */
bool OMXPlayerInterface::Connect_Root() {
    const char *dbus_addr = m_s_dbus_addr.c_str();

    // setup the dbus 'Player' connection and client
    if (g_root_conn == NULL) {
      Log("Connect_Root() - Creating connection to 'Player' bus");
      bool bPrivate(true);
      g_root_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_root_conn->connected()) {
        Log("Connect_Root() - g_root_conn not connected(), exiting.");
        return false;
      }
      if (!g_root_conn->register_bus()) {
        Log("Connect_Root() - g_root_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_root_client == NULL) {
      Log("Connect_Root() - Creating Player_proxy client interface");
      g_root_client = new OMXRootClient(*g_root_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
      return true;
    }
    return false;
}

void OMXPlayerInterface::Disconnect_Root() {
    Log("Disconnect_Root() - cleaning up D-Bus interface proxies");
    // Clean up the properties interfaces and connections;
    if (g_root_client != NULL)
      delete g_root_client;
    g_root_client = NULL;
    if (g_root_conn != NULL)
      delete g_root_conn;
    g_root_conn = NULL;
}

bool OMXPlayerInterface::Reconnect_Root() {
    Log("Reconnect_Root() - reconnecting dbus player interface");
    Disconnect_Root();
    return Connect_Root();
}
/* */

typedef std::vector< std::string > STRINGARRAY;
inline const int iterator_to_index(STRINGARRAY &a, STRINGARRAY::iterator it)
{
	return it - a.begin();
}

bool OMXPlayerInterface::getSubtitlesShowing() {
	return m_bSubtitlesShowing;
}

void OMXPlayerInterface::HideSubtitles() {
	Log("OMXPlayerInterface::HideSubtitles");
	Do_HideSubtitles();
	m_bSubtitlesShowing = false;
}

void OMXPlayerInterface::Do_HideSubtitles() {
  try {
    g_player_client->HideSubtitles();
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_HideSubtitles() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
}

void OMXPlayerInterface::ShowSubtitles() {
	Log("OMXPlayerInterface::ShowSubtitles");
	Do_ShowSubtitles();
	m_bSubtitlesShowing = true;
}

void OMXPlayerInterface::Do_ShowSubtitles() {
  try {
    g_player_client->ShowSubtitles();
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_ShowSubtitles() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
}

bool OMXPlayerInterface::setSubtitle(int track) {
	Log("OMXPlayerInterface::setSubtitle - " + to_string(track) );
	return Do_SelectSubtitle(track);
}

bool OMXPlayerInterface::Do_SelectSubtitle(int track) {
  bool ret(false);
  try {
    ret = g_player_client->SelectSubtitle(track);
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_SelectSubtitle() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
  // TODO: cache this?
  return ret;
}

int OMXPlayerInterface::getCurrentSubtitle() {
	// TODO: cache this?
	std::vector< std::string > vsSubtitleTracks = Do_ListSubtitles();
	for(std::vector<std::string>::iterator it = vsSubtitleTracks.begin(); it < vsSubtitleTracks.end(); it++)
	{
		// <index>:<language>:<name>:<codec>:<active>
		string::size_type tokenPos = 0;
	        string index = Tokenize(*it, string(":"), tokenPos);
		tokenPos = it->find( "active", tokenPos );
		if ( tokenPos != string::npos )
			return atoi( index.c_str() );

/*	        string language = Tokenize(*it, string(":"), tokenPos);
	        string name = Tokenize(*it, string(":"), tokenPos);
	        string codec = Tokenize(*it, string(":"), tokenPos);
	        string active = Tokenize(*it, string(":"), tokenPos);

		if (active == "active")
			return atoi( index.c_str() );
*/
	}
	return -1;
}

int OMXPlayerInterface::getMaxSubtitle() {
	int ret;
	std::vector< std::string > vsSubtitleTracks = Do_ListSubtitles();
	ret = iterator_to_index( vsSubtitleTracks,vsSubtitleTracks.end() );
	return ret;
}

std::vector< std::string > OMXPlayerInterface::Do_ListSubtitles() {
  std::vector< std::string > ret;
  int err_count=0;
  bool done=false;
  while ( !done && (err_count < DBUS_RETRIES)) {
    try {
      ret = g_player_client->ListSubtitles();
      done = true;
    }
    catch (DBus::Error &dbus_err) {
      Log("Send_Action() - D-Bus error - omxplayer has gone away?");
      Reconnect_Player();
    }
  }
  return ret;
}

std::vector< std::string > OMXPlayerInterface::Get_ListSubtitles() {
	Log("OMXPlayerInterface::Get_ListSubtitles - called");
	std::vector< std::string > ret;
	ret = Do_ListSubtitles();
	return ret;
}

bool OMXPlayerInterface::setVideo(int track) {
	return Do_SelectVideo(track);
}

bool OMXPlayerInterface::Do_SelectVideo(int track) {
  bool ret(false);
  try {
    Log("Do_SelectVideo() - SelectVideo(track) does not exist in dbus interface.");
    //ret = g_player_client->SelectVideo(track);
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_SelectVideo() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
  return ret;
}


int OMXPlayerInterface::getCurrentVideo() {
	std::vector< std::string > vsVideoTracks = Do_ListVideo();
	for(std::vector<std::string>::iterator it = vsVideoTracks.begin(); it < vsVideoTracks.end(); it++)
	{
		// <index>:<language>:<name>:<codec>:<active>
		string::size_type tokenPos = 0;
	        string index = Tokenize(*it, string(":"), tokenPos);
		tokenPos = it->find( "active", tokenPos );
		if ( tokenPos != string::npos )
			return atoi( index.c_str() );

/*	        string language = Tokenize(*it, string(":"), tokenPos);
	        string name = Tokenize(*it, string(":"), tokenPos);
	        string codec = Tokenize(*it, string(":"), tokenPos);
	        string active = Tokenize(*it, string(":"), tokenPos);

		if (active == "active")
			return atoi( index.c_str() );
*/
	}
	return -1;
}

int OMXPlayerInterface::getMaxVideo() {
	int ret;
	std::vector< std::string > vsVideoTracks = Do_ListVideo();
	ret = iterator_to_index( vsVideoTracks,vsVideoTracks.end() );
	return ret;
}

std::vector< std::string > OMXPlayerInterface::Do_ListVideo() {
  std::vector< std::string > ret;
  int err_count=0;
  bool done=false;
  while ( !done && (err_count < DBUS_RETRIES)) {
    try {
      ret = g_player_client->ListVideo();
      done = true;
    }
    catch (DBus::Error &dbus_err) {
      Log("Send_Action() - D-Bus error - omxplayer has gone away?");
      Reconnect_Player();
    }
  }
  return ret;
}

std::vector< std::string > OMXPlayerInterface::Get_ListVideo() {
	Log("OMXPlayerInterface::Get_ListVideo - called");
	std::vector< std::string > ret;
	ret = Do_ListVideo();
	return ret;
}


bool OMXPlayerInterface::setAudio(int track) {
	Log("OMXPlayerInterface::setAudio - " + to_string(track) );
	return Do_SelectAudio(track);
}

bool OMXPlayerInterface::Do_SelectAudio(int track) {
  bool ret;
  try {
    ret = g_player_client->SelectAudio(track);
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_SelectAudio() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
  return ret;
}

int OMXPlayerInterface::getCurrentAudio() {
	std::vector< std::string > vsAudioTracks = Do_ListAudio();
	for(std::vector<std::string>::iterator it = vsAudioTracks.begin(); it < vsAudioTracks.end(); it++)
	{
		// <index>:<language>:<name>:<codec>:<active>
		string::size_type tokenPos = 0;
	        string index = Tokenize(*it, string(":"), tokenPos);

		tokenPos = it->find( "active", tokenPos );
		if ( tokenPos != string::npos )
			return atoi( index.c_str() );

/*	        string language = Tokenize(*it, string(":"), tokenPos);
	        string name = Tokenize(*it, string(":"), tokenPos);
	        string codec = Tokenize(*it, string(":"), tokenPos);
	        string active = Tokenize(*it, string(":"), tokenPos);

		if (active == "active")
			return atoi( index.c_str() );
*/
	}
	return -1;
}

int OMXPlayerInterface::getMaxAudio() {
	int ret;
	std::vector< std::string > vsAudioTracks = Do_ListAudio();
	ret = iterator_to_index( vsAudioTracks,vsAudioTracks.end() );
	return ret;
}

std::vector< std::string > OMXPlayerInterface::Do_ListAudio() {
  std::vector< std::string > ret;
  int err_count=0;
  bool done=false;
  while ( !done && (err_count < DBUS_RETRIES)) {
    try {
      ret = g_player_client->ListAudio();
      done = true;
    }
    catch (DBus::Error &dbus_err) {
      Log("Send_Action() - D-Bus error - omxplayer has gone away?");
      Reconnect_Player();
    }
  }
  return ret;
}

std::vector< std::string > OMXPlayerInterface::Get_ListAudio() {
	Log("OMXPlayerInterface::Get_ListAudio - called");
	std::vector< std::string > ret;
	ret = Do_ListAudio();
	return ret;
}

int OMXPlayerInterface::Get_Subtitle() {
	return getCurrentSubtitle();
}

int OMXPlayerInterface::Get_Audio() {
	return getCurrentAudio();
}

int OMXPlayerInterface::Get_Speed() {
	int ret = 0;
	// FIXME: GET THIS DATA
	// we cannot access this data currently in omxplayer
	ret = 1000;
	return ret;
}

int OMXPlayerInterface::Get_Title() {
	int ret = 0;
	// FIXME: GET THIS DATA
	// we cannot access this data currently in omxplayer
	return ret;
}

int OMXPlayerInterface::Get_Chapter() {
	int ret = 0;
	// FIXME: GET THIS DATA
	// we cannot access this data currently in omxplayer
	return ret;
}

string OMXPlayerInterface::Get_CurrentFile() {
	return m_sMediaURL;
}


void OMXPlayerInterface::Do_Seek(int64_t usecs) {
    Log("Do_Seek() - calling Seek(" + to_string(usecs) + ")");
    Send_Seek(usecs);
}

int64_t OMXPlayerInterface::Send_Seek(int64_t usecs) {
    int64_t sought;
    Log("Send_Seek(" + to_string(usecs) + ")");
    try {
      sought = g_player_client->Seek(usecs);
    }
    catch (DBus::Error &dbus_err) {
      Log("Send_Seek() - D-Bus error - seek failed.");
      Reconnect_Player();
    }
    if (!sought)
        Log("Send_Seek() - FAILED!");
    else if (sought == usecs )
	Log("Send_Seek() - SUCCESS!");
    else
        Log("Send_Seek() - This should not happen!");
    return sought;
}

void OMXPlayerInterface::Do_SetPosition(string sMediaURL, int64_t xPos) {
    Log("Do_SetPosition() - calling SetPosition('" + sMediaURL + "', " + to_string(xPos) + ") [" + TimeFromUSec(xPos) + "]");
    if (Send_SetPosition(sMediaURL, xPos) != xPos)
	Log("Do_SetPosition() - error setting position!");
}

int64_t OMXPlayerInterface::Send_SetPosition(string sMediaURL, int64_t xPos) {
  int64_t ret;
  Log("Send_SetPosition() - sending SetPosition('" + sMediaURL + "', " + to_string(xPos) + ") [" + TimeFromUSec(xPos) + "]");
  try {
    ret = g_player_client->SetPosition(sMediaURL, xPos);
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_SetPosition() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
  m_sMediaURL = sMediaURL;
  return ret;
}

void OMXPlayerInterface::Do_DecreaseSpeed() {
    Log("Do_DecreaseSpeed() - calling Send_Action(KeyConfig::ACTION_DECREASE_SPEED)");
    Send_Action(KeyConfig::ACTION_DECREASE_SPEED);
}

void OMXPlayerInterface::Do_IncreaseSpeed() {
    Log("Do_IncreaseSpeed() - calling Send_Action(KeyConfig::ACTION_INCREASE_SPEED)");
    Send_Action(KeyConfig::ACTION_INCREASE_SPEED);
}

void OMXPlayerInterface::Do_DecreaseVolume() {
    Log("Do_DecreaseVolume() - calling Send_Action(KeyConfig::ACTION_DECREASE_VOLUME)");
    Send_Action(KeyConfig::ACTION_DECREASE_VOLUME);
}

void OMXPlayerInterface::Do_IncreaseVolume() {
    Log("Do_IncreaseVolume() - calling Send_Action(KeyConfig::ACTION_INCREASE_VOLUME)");
    Send_Action(KeyConfig::ACTION_INCREASE_VOLUME);
}

void OMXPlayerInterface::Do_Mute() {
  try {
    g_props_client->Mute();
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_Mute() - D-Bus error - omxplayer has gone away?");
    Reconnect_Properties();
  }
}

void OMXPlayerInterface::Do_UnMute() {
  try {
    g_props_client->Unmute();
  }
  catch (DBus::Error &dbus_err) {
    Log("Do_Mute() - D-Bus error - omxplayer has gone away?");
    Reconnect_Properties();
  }
}

void OMXPlayerInterface::Do_SeekBackSmall() {
    Log("Do_SeekBack() - calling Send_Action(KeyConfig::ACTION_SEEK_BACK_SMALL)");
    Send_Action(KeyConfig::ACTION_SEEK_BACK_SMALL);
}

void OMXPlayerInterface::Do_SeekBackLarge() {
    Log("Do_SeekBack() - calling Send_Action(KeyConfig::ACTION_SEEK_BACK_LARGE)");
    Send_Action(KeyConfig::ACTION_SEEK_BACK_LARGE);
}

void OMXPlayerInterface::Do_SeekForwardSmall() {
    Log("Do_SeekForward() - calling Send_Action(KeyConfig::ACTION_SEEK_FORWARD_SMALL)");
    Send_Action(KeyConfig::ACTION_SEEK_FORWARD_SMALL);
}

void OMXPlayerInterface::Do_SeekForwardLarge() {
    Log("Do_SeekForward() - calling Send_Action(KeyConfig::ACTION_SEEK_FORWARD_LARGE)");
    Send_Action(KeyConfig::ACTION_SEEK_FORWARD_LARGE);
}

void OMXPlayerInterface::Do_Rewind() {
    Log("Do_Rewind() - calling Send_Action(KeyConfig::ACTION_REWIND)");
    Send_Action(KeyConfig::ACTION_REWIND);
}

void OMXPlayerInterface::Do_FastForward() {
    Log("Do_Rewind() - calling Send_Action(KeyConfig::ACTION_FAST_FORWARD");
    Send_Action(KeyConfig::ACTION_FAST_FORWARD);
}


void OMXPlayerInterface::Send_Action(int Action) {
  Log("Send_Action() - sending Action(" + to_string(Action) + ")");
  try {
    g_player_client->Action(Action);
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Action() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
}

void OMXPlayerInterface::Do_Pause() {
    Log("Do_Pause() - calling Send_Pause()");
    Send_Pause();
}

void OMXPlayerInterface::Send_Pause() {
  Log("Send_Pause() - sending Pause()");
  try {
    g_player_client->Pause();
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Pause() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
}

void OMXPlayerInterface::Send_Stop() {
  Log("Send_Stop() - sending Stop()");
  try {
    g_player_client->Stop();
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Stop() - D-Bus error - omxplayer has gone away?");
    Reconnect_Player();
  }
}


void OMXPlayerInterface::Stop() {
    m_mtxLocal.lock();
    if (!m_bOMXIsRunning && ePlayerState >= STATE::STOPPING ) {
      m_mtxLocal.unlock();

      Log("Stop() - m_bOMXIsRunning = false, exiting Stop()");
      return;
    }
    m_bOMXIsRunning = false;
    m_mtxLocal.unlock();

    SetState(STATE::STOPPING);

    Log("Stop() - called");

    if ( !IsStopped() ) Send_Stop();
//    Send_Quit();

    // exiting player monitor thread
    if (m_bRunPlayerMonitor && m_tPlayerMonitorThread) {
      Log("Stop() - joining m_tPlayerMonitorThread");
      m_bRunPlayerMonitor = false;
      pthread_join(m_tPlayerMonitorThread, NULL);
      m_tPlayerMonitorThread = 0;
      Log("Stop() - m_tPlayerMonitorThread joined");
    }

    // exiting output reader thread
    if (m_bRunPlayerOutputReader && m_tPlayerOutputReaderThread)
    {
      Log("Stop() - joining m_tPlayerOutputReaderThread");
      m_bRunPlayerOutputReader = false;
      pthread_join(m_tPlayerOutputReaderThread, NULL);
      m_tPlayerOutputReaderThread = 0;
      Log("Stop() - m_tPlayerOutputReaderThread joined");
    }

#ifdef USE_PIPES
    ClosePipes();
#endif

    Log("Stop() - cleaning up D-Bus interface proxies");
    // Clean up the client interfaces and connections;
    if (g_player_client != NULL)
      delete g_player_client;
    g_player_client = NULL;
    if (g_props_client != NULL)
      delete g_props_client;
    g_props_client = NULL;
    if (g_root_client != NULL)
      delete g_root_client;
    g_root_client = NULL;

    Log("Stop() - cleaning up D-Bus connection proxies");
    if (g_player_conn != NULL)
      delete g_player_conn;
    g_player_conn = NULL;
    if (g_props_conn != NULL)
      delete g_props_conn;
    g_props_conn = NULL;
    if (g_root_conn != NULL)
      delete g_root_conn;
    g_root_conn = NULL;

    Log("Stop() - finished");
    Set_Stopped(true);
    Set_Finished();
    SetState(STATE::STOPPED);
}

void OMXPlayerInterface::Send_Quit() {
  Log("Send_Quit() - sending Quit()");
  try {

    if (!g_props_client) {
      Log("Send_Quit() - FUBAR! (props) - Stop() pressed when not PLAYING?");
      return;
    }

    if (!g_props_client->CanQuit() ) {
      Log("Send_Quit() - player unable to quit");
      return; // false;
    }

    if (!g_root_client) {
      Log("Send_Quit() - FUBAR! (root) - Stop() pressed when not PLAYING?");
      return;
    }
    g_root_client->Quit();
  }
  catch (DBus::Error &dbus_err) {
    Log("Send_Quit() - can't send, has omxplayer gone away?");
    Reconnect_Root();
  }
}

void OMXPlayerInterface::ClosePipes() {
  Log("ClosePipes() - Closing pipes");
  for (int i=0; i<2; i++) {
    if (m_bInPipe[i])
      close(m_iInPipe[i]);
    if (m_bOutPipe[i])
      close(m_iOutPipe[i]);
    m_bInPipe[i] = m_bOutPipe[i] = false;
  }
  Log("ClosePipes() - Pipes are closed");
}

void OMXPlayerInterface::OpenPipes() {
  Log("OpenPipes() - called");
  // setup pipes for the PlayerOutputReaderThread

  // initializing in pipe
  if ( pipe(m_iInPipe) == -1 ) {
    Log("pipe() failed for in pipe");
    return;
  }
  else {
    m_bInPipe[0] = m_bInPipe[1] = true;
  }

  // initializing out pipe
  if ( pipe(m_iOutPipe) == -1 ) {
    Log("pipe() failed for out pipe");
    return;
  }
  else {
    m_bOutPipe[0] = m_bOutPipe[1] = true;
  }
  Log("OpenPipes() - finished");
}


bool OMXPlayerInterface::Play(string sMediaURL, string sMediaPosition) {
  if ( ePlayerState != STATE::STOPPED  ) {
	Log("OMXPlayerInterface::Play - Must be STOPPED to Play()");
	return false;
  }

  SetState(STATE::INITIALIZING);

  m_sMediaURL = sMediaURL;
  // TODO add error reporting

#ifdef USE_PIPES
  OpenPipes();
#endif

  // fork a child process to run omxplayer in
  pid_t pid = fork();
  if (pid < 0)                      // failed to fork
  {
    cerr << "Failed to fork" << endl;
    exit(127);
    // Throw exception
  }
  else if (pid == 0)                // child
  {
    // Code only executed by child process
    Log("(Child's) Play() - begins");

#ifdef USING_PIPES    // connecting child stdin to InPipe, stdout & stderr to OutPipe
    Log("(Child's) Play() - connecting to InPipe and OutPipe");
    if ( dup2(m_iInPipe[0], STDIN_FILENO)<0 || dup2(m_iOutPipe[1], STDOUT_FILENO)<0 ||
         dup2(m_iOutPipe[1], STDERR_FILENO)<0 ) {
      Log("dup2() failed in child");
//      printf("%d : %s\n", errno, strerror(errno));
      exit(127);
    }
#endif

    // build command line for launching omxplayer
    vector<char*> args;
    char *fullname = (char *)"/usr/bin/omxplayer";
    char *name = (char *)"omxplayer";
    args.push_back(name);

    // set the passthrough option (if enabled)
    if (m_bPassthrough) {
      char *passthru = (char *)"-p";
      args.push_back(passthru);
    }

    // check the audio output device
    string sopt;
    if (m_sAudioDevice == "S") {
        sopt = "-olocal";
    }
    else {
       sopt = "-ohdmi";
    }
    args.push_back((char *)sopt.c_str());

    if (true) {
      // set the video mode to match the source media, reset when done
      char *refresh = (char *)"-r";
      args.push_back(refresh);
    }

    if (true) {
      // disable omxplayer's stdin keys
      char *nokeys = (char *)"--no-keys";
      args.push_back(nokeys);
    }

    if (true) {
      // add the starting position
      sMediaPosition = "-l" + sMediaPosition;
      Log("(Child's) Play() - sMediaPosition: " + sMediaPosition);
      args.push_back((char *)sMediaPosition.c_str());
    }

    // add the media url
    Log("(Child's) Play() - MediaURL: " + sMediaURL);
    args.push_back((char *)sMediaURL.c_str());

    string sTxt = "(Child's) Play() - cmdline: ";
    for(std::vector<char *>::iterator it = args.begin(); it != args.end(); ++it) {
      string sArg(*it);
      sTxt = sTxt + sArg + " ";
    }
    Log(sTxt);

    // need a \0 as the last item in args[]
    args.push_back((char *)NULL);

    // replace this proces with omxplayer
    execv(fullname, &args[0]);

    Log("child - omxplayer - THIS SHOULD NEVER BE SEEN!");
    exit(127);
  }
  else                         // parent
  {
    // Code only executed by parent process
    m_iChildPID = pid;
    m_bOMXIsRunning = true;

#ifdef USE_PIPES
    Log("Play() - closing child's end of pipes");
    // closing child's end of pipes
    close(m_iInPipe[0]);
    m_bInPipe[0] = false;
    close(m_iOutPipe[1]);
    m_bOutPipe[1] = false;
    fcntl(m_iOutPipe[0], F_SETFL, O_NONBLOCK);


    // setup the output reader thread to watch for 'Stopped at XX:XX:XX' and 'have a nice day ;)' signals from omxplayer?
    m_bRunPlayerOutputReader = true;
    // TODO error reporting here
    Log("Play() - creating PlayerOutputReaderThread");
    pthread_create(&m_tPlayerOutputReaderThread, NULL, &PlayerOutputReader, this);
#endif

    int i=0;
    const char *dbus_addr;
    string line("");
    ifstream infile;
/*
    bool bPrivate(true);
*/

    // Don't get dbus_addr if we already know it, changes on rpi reboot only
    if ( m_s_dbus_addr.empty() ) {
      Log("Play() - Opening file '" + OMXPLAYER_DBUS_ADDR + "' to get d-bus address");
      infile.open(OMXPLAYER_DBUS_ADDR.c_str());

      while (++i<=RETRIES && line == "") {
        while (!infile.is_open() && ++i<=RETRIES ) {
          usleep(100000);
          infile.open(OMXPLAYER_DBUS_ADDR.c_str());
        }
        if (!infile.is_open())  {
          Log("Play() - Could not open file " + OMXPLAYER_DBUS_ADDR + ", exiting... ");
          Do_QuitOnError();
          return false;
        }
        else {
          getline(infile, line);
          infile.close();
        }
      }

      if ( line != "" ) {
        Log("Play() - Read line: " + line);
      }
      else {
        Log("Play() - Failed to read D-Bus address line from " + OMXPLAYER_DBUS_ADDR);
//        Stop();
        return false;
      }
      m_s_dbus_addr = line;
    }

    dbus_addr = m_s_dbus_addr.c_str();

    Log("Play() - Calling Connect_Player()");
    if ( !Connect_Player() )
      return false;
/*
    // setup the dbus 'Player' connection and client
    if (g_player_conn == NULL) {
      Log("Play() - Creating connection to 'Player' bus");
      g_player_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_player_conn->connected()) {
        Log("Play() - g_player_conn not connected(), exiting.");
      }
      if (!g_player_conn->register_bus()) {
        Log("Play() - g_player_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_player_client == NULL) {
      Log("Play() - Creating Player_proxy client interface");
      g_player_client = new OMXPlayerClient(*g_player_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
    }
*/

    Log("Play() - Calling Connect_Properties()");
    if ( !Connect_Properties() )
      return false;
/*
    // setup the dbus 'Properties' connection and client
    if (g_props_conn == NULL) {
      Log("Play() - Creating connection to 'Propertiess' bus");
      g_props_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_props_conn->connected()) {
        Log("Play() - g_props_conn not connected(), exiting.");
      }
      if (!g_props_conn->register_bus()) {
        Log("Play() - g_props_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_props_client == NULL) {
      Log("Play() - Creating Properties_proxy client interface");
      g_props_client = new OMXPropsClient(*g_props_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
    }
*/

    Log("Play() - Calling Connect_Root()");
    if ( !Connect_Root() )
      return false;
/*
    // setup the dbus 'Root' connection and client
    if (g_root_conn == NULL) {
      Log("Play() - Creating connection to 'Root' bus");
      g_root_conn = new DBus::Connection(dbus_addr, bPrivate);
      if (!g_root_conn->connected()) {
        Log("Play() - g_root_conn not connected(), exiting.");
      }
      if (!g_root_conn->register_bus()) {
        Log("Play() - g_root_conn unable to register_bus(), exiting.");
        return false;
      }
    }
    if (g_root_client == NULL) {
      Log("Play() - Creating Root_proxy client interface");
      g_root_client = new OMXRootClient(*g_root_conn, OMXPLAYER_SERVER_PATH, OMXPLAYER_SERVER_NAME);
    }
*/

    // setup the monitoring thread to waitpid()
    m_bRunPlayerMonitor = true;
    // TODO error reporting here
    Log("Play() - Creating PlayerMonitorThread");
    pthread_create(&m_tPlayerMonitorThread, NULL, &PlayerMonitor, this);

    if (g_player_client != NULL && g_props_client != NULL) {
      int i = 0;
      string sIdentity;
      bool bIdentity = false;
      while (!bIdentity && ++i<=DBUS_RETRIES) {
        try {
          sIdentity = g_props_client->Identity();
          bIdentity = true;
        }
        catch (DBus::Error &dbus_err) {
if (i >=DBUS_RETRIES)
{
  Log("OMXPlayerInterface::Play - FAILED to Play(), exiting player if running...");
  Do_QuitOnError();
  return false;
}
          //Log("OMXPlayerInterface::Play() - Waiting for Identity()...");
          usleep(100000);
        }
      }
      Log("Play() - Identity() [" + to_string(i) + "]: " + sIdentity);
    }
    else {
      Log("Play() - FUBAR!");
    }

    SetState(STATE::PLAYING);
    Set_Stopped(false);
    // at this point the player is accessible through dbus
    return true;
  }
  // Code executed by both parent and child.
  Log("OMXPlayerInterface::Play - End - this should never happen.");
  return false;
}


void OMXPlayerInterface::SetState(OMXPlayerInterface::STATE playerState) {
  Log("OMXPlayerInterface::SetState - State: " + to_string((int)playerState));

  ePlayerState = playerState;

  // if a c callback has been registered notify it
  if (m_fpStateNotifier)
	m_fpStateNotifier(playerState);

  // if a c++ class has inherited this notify it
  PlayerStateNotifier(playerState);
}

OMXPlayerInterface::STATE OMXPlayerInterface::Get_PlayerState(void) {
//  Log("OMXPlayerInterface::Get_PlayerState");
  return ePlayerState;
}

void OMXPlayerInterface::Inc_Error(void) {
  ++m_iErrCount;
  Log("[" + to_string(m_iErrCount) + "] - ");

  if ( m_iErrCount >= DBUS_RETRIES ) {
    Stop();
  }

}

void OMXPlayerInterface::Set_Stopped(bool val) {

  int i = 0;
  while (!m_mtxLocal.try_lock() && ++i<=RETRIES) {
    usleep(1000);
  }
  if (i >= RETRIES) {
    Log("Set_Stopped() - can't get lock");
    return;
  }

  //Log("Set_Stopped() - setting m_bStopped=" + string(val ? "true" : "false"));
  m_bStopped = val;
  m_mtxLocal.unlock();
}

bool OMXPlayerInterface::IsStopped(void) {
  bool ret;

  int i = 0;
  while (!m_mtxLocal.try_lock() && ++i<=RETRIES) {
    usleep(1000);
  }
  if (i >= RETRIES) {
    Log("IsStopped() - can't get lock");
    return false;
  }

  ret = m_bStopped;
  m_mtxLocal.unlock();
  return ret;
}

int OMXPlayerInterface::Get_ErrCount(void) {
  int ret;
  ret = m_iErrCount;
  return ret;
}

void OMXPlayerInterface::Set_ErrCount(int count) {
  m_iErrCount = count;
  if (m_iErrCount >= DBUS_RETRIES) {
    Log("Set_ErrCount() - count exceeds retries, calling Stop()");
    Stop();
  }
}

string OMXPlayerInterface::Get_PlaybackStatus(void) {
  string ret;

  if (!g_props_client)
      Log("Get_PlaybackStatus() - FUBAR! Please report this to the author!");

  try {
    ret = g_props_client->PlaybackStatus();
    Set_ErrCount(0);
  }
  catch (DBus::Error &dbus_err) {
    Reconnect_Properties();
    Inc_Error();
    ret = "";
  }
  return ret;
}

uint64_t OMXPlayerInterface::Get_Position(void) {
  uint64_t ret = -1;
  try {
    ret = g_props_client->Position();
    Set_ErrCount(0);
  }
  catch (DBus::Error &dbus_err) {
    Reconnect_Properties();
    Inc_Error();
  }
  return ret;
}

uint64_t OMXPlayerInterface::Get_Duration(void) {
  uint64_t ret = -1;
  try {
    ret = g_props_client->Duration();
    Set_ErrCount(0);
  }
  catch (DBus::Error &dbus_err) {
    Reconnect_Properties();
    Inc_Error();
  }
  return ret;
}

bool OMXPlayerInterface::Get_CanQuit(void) {
  bool ret;
  ret = g_props_client->CanQuit();
  return ret;
}

bool OMXPlayerInterface::IsFinished(void) {
  bool ret = false;

  int i = 0;
  while (!m_mtxLocal.try_lock() && ++i<=RETRIES) {
    usleep(1000);
  }
  if (i >= RETRIES) {
    Log("IsFinished() - can't get lock");
    return false;
  }

  ret = m_bFinished;
  m_mtxLocal.unlock();
  return ret;
}

void OMXPlayerInterface::Set_Finished() {
  m_mtxLocal.lock();
  m_bFinished = true;
  m_mtxLocal.unlock();
}

void OMXPlayerInterface::Log(string txt) {
  m_mtxLog.lock();
  cout << txt << endl;
  m_mtxLog.unlock();
}

void OMXPlayerInterface::Do_QuitOnError() {
        Stop();
        system("killall omxplayer.bin");
//        system("killall omxplayer");
}

void *PlayerOutputReader(void *pInstance) {
  OMXPlayerInterface* pThis = (OMXPlayerInterface*) pInstance;

  pThis->Log("[PlayerOutputReader] started");

  while (!pThis->IsStopped()) {
    usleep(10000);
  }

  pThis->Log("[PlayerOutputReader] exiting");

  return NULL;
}


void *PlayerMonitor(void *pInstance) {
  OMXPlayerInterface* pThis = (OMXPlayerInterface*) pInstance;

  pThis->Log("[PlayerMonitor] started");

  pid_t watch_id = pThis->m_iChildPID;
  waitpid(watch_id, NULL, 0);

  pThis->Log("[PlayerMonitor] calling Stop()");
  pThis->Stop();
  pThis->Log("[PlayerMonitor] exiting");

  return NULL;
}




string OMXPlayerInterface::ZeroPad(int num, int width)
{
    std::ostringstream ss;
    ss << std::setw( width ) << std::setfill( '0' ) << num;
    return ss.str();
}

string OMXPlayerInterface::TimeFromUSec(uint64_t usecs) {
        string ret;
        ret = ZeroPad(usecs/1000/1000/60/60,2) + ":" + ZeroPad(usecs/1000/1000/60 % 60,2) + ":" + ZeroPad(usecs/1000/1000 % 60,2);// + "." + ZeroPad(usecs/1000 % 1000,3);// + ":" + to_string(usecs % 1000);
        return ret;
}

int64_t OMXPlayerInterface::USecFromTime(string sTime) {

	const int64_t usec = 1;
	const int64_t msec = 1000;
	const int64_t sec = 1000 * msec;
	const int64_t min = sec * 60;
	const int64_t hour = min * 60;

	int64_t usecs = 0;
	int64_t iPlaceholderValue = hour;
	iPlaceholderValue = hour;
	string::size_type tokenPos = 0;
	string curValue = Tokenize(sTime, string(":"), tokenPos);
	while (curValue!="")
	{
		int iValue = atoi(curValue.c_str());
		usecs = usecs + (iPlaceholderValue * iValue);

		if (iPlaceholderValue == hour) {
			iPlaceholderValue = min;
		}
		else if (iPlaceholderValue == min) {
			iPlaceholderValue = sec;
		}
		else if (iPlaceholderValue == sec) {
			iPlaceholderValue = msec;
		}
		else if (iPlaceholderValue == msec) {
			iPlaceholderValue = usec;
		}

		curValue = Tokenize(sTime, string(":"), tokenPos);
	}
	return usecs;
}



string OMXPlayerInterface::Tokenize( string &sInput, string sToken, string::size_type &CurPos, bool bMultiCharacterTokens )
{
    if ( CurPos >= sInput.length() ) // allready extracted all the pices
	{
        return "";
	}

    string::size_type TokenPos;

    if ( bMultiCharacterTokens==false && sToken.length() > 1 )
    {
        TokenPos = sInput.length() + 1;

        for( string::size_type i=0; i < sToken.length(); i++ )
        {
            string::size_type NewPos = CurPos;
            Tokenize( sInput, sToken.substr(i, 1), NewPos );
            if ( NewPos < TokenPos )
                TokenPos = NewPos;
        }
        TokenPos--;
    }
    else
    {
        TokenPos = sInput.find( sToken, CurPos );
        if ( TokenPos == string::npos )
        {
            TokenPos = sInput.length();
        }
    }

	string sReturnValue = sInput.substr( CurPos, (TokenPos - CurPos) );
	CurPos = TokenPos + (bMultiCharacterTokens ? sToken.length() : 1);
    return sReturnValue;
}

/*
void OMXPlayerInterface::Tokenize(string &Input, string Tokens, vector<string> &vect_strings)
{
	const char *pTokens = Tokens.c_str();
	int Size = int(Tokens.size());
	if( Size<1 || Size>3 )
		return; // Not supported no tokens or more than 3

	const char *pcPtr = Input.c_str();
	char *pPtr_dup = strdup(pcPtr);
	char *pPtr = pPtr_dup;
	const char *pPtr_Start = pPtr;

	while( *pPtr )
    {
		while( *pPtr && *pPtr!=pTokens[0]
			&& (Size<2 || *pPtr!=pTokens[1])
			&& (Size<3 || *pPtr!=pTokens[2]) )
				pPtr++;

		// We're stopped on a token, terminate this string and skip over any more tokens
		while( *pPtr &&
			( *pPtr==pTokens[0] ||
			  (Size>1 && *pPtr==pTokens[1]) ||
			  (Size>2 && *pPtr==pTokens[2]) ) )
		{
			(*pPtr)=0;
			pPtr++;
		}

		if( *pPtr_Start && pPtr_Start!=pPtr )
            vect_strings.push_back(pPtr_Start);
		pPtr_Start=pPtr;
    }
	free(pPtr_dup);
}

void OMXPlayerInterface::Tokenize(string &Input, string Tokens, deque<string> &deque_strings, bool bPushToFront)
{
    string::size_type pos=0;
    string sToken;
    while( (sToken=Tokenize(Input,Tokens,pos)).length() )
    {
        if( bPushToFront )
            deque_strings.push_front(sToken);
        else
            deque_strings.push_back(sToken);
    }
}


*/
