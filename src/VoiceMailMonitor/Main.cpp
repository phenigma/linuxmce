#define VERSION "<=version=>"

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "Logger.h"

#include "FileNotifier.h"
#include <sys/types.h>
#include <grp.h>

#include <list>
#include <string>

using namespace std;
using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger;
}

struct MonitoredDir
{
	int m_iWD;
	string m_sPath;
};

typedef list<MonitoredDir> listOfMonitoredDirs;
typedef list<string> listOfStrings;

bool DirIsWatched(listOfMonitoredDirs & listMonitoredDirs, string sDirectory)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_sPath == sDirectory)
		{
			listMonitoredDirs.erase(it_lOMD);
			return true;
		}
	}

	return false;
}

void UnwatchDir(listOfMonitoredDirs & listMonitoredDirs, const string & sDirectory)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_sPath == sDirectory)
		{
			g_pPlutoLogger->Write(LV_STATUS, "Unwatching directory '%s'", it_lOMD->m_sPath.c_str());
			listMonitoredDirs.erase(it_lOMD);
			break;
		}
	}
}

void UnwatchDir(listOfMonitoredDirs & listMonitoredDirs, int wd)
{
	for (listOfMonitoredDirs::iterator it_lOMD = listMonitoredDirs.begin(); it_lOMD != listMonitoredDirs.end(); it_lOMD++)
	{
		if (it_lOMD->m_iWD == wd)
		{
			g_pPlutoLogger->Write(LV_STATUS, "Unwatching directory '%s'", it_lOMD->m_sPath.c_str());
			listMonitoredDirs.erase(it_lOMD);
			break;
		}
	}
}

void WatchDir(listOfMonitoredDirs & listMonitoredDirs, const string & sDirectory, inotify & obj_Inotify)
{
	g_pPlutoLogger->Write(LV_STATUS, "Watching directory '%s'", sDirectory.c_str());

	MonitoredDir struct_MonitoredDir;
	struct_MonitoredDir.m_sPath = sDirectory;
	struct_MonitoredDir.m_iWD = obj_Inotify.watch(sDirectory, IN_CREATE | IN_IGNORED);
	
	listMonitoredDirs.push_back(struct_MonitoredDir);
}

void OperateFile(listOfMonitoredDirs & listMonitoredDirs, inotify & obj_Inotify, cpp_inotify_event & event)
{
	static group * grent = getgrnam("www-data");
	struct stat st;

	g_pPlutoLogger->Write(LV_STATUS, "Operating on '%s'", event.name.c_str());
	if (event.mask & IN_IGNORED)
	{
		UnwatchDir(listMonitoredDirs, event.wd);
		return;
	}

	if (grent != NULL)
	{
		chown(event.name.c_str(), -1, grent->gr_gid);
	}

	stat(event.name.c_str(), &st);
	if (S_ISDIR(st.st_mode))
	{
		// a directory was created
		chmod(event.name.c_str(), 0770);
		WatchDir(listMonitoredDirs, event.name, obj_Inotify);
	}
	else
	{
		// a file was created
		chmod(event.name.c_str(), 0660);
	}
}

int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger("/var/log/pluto/VoiceMailMonitor.log");
	string sDirectory = "/var/spool/asterisk/voicemail";
	inotify obj_Inotify;

	listOfMonitoredDirs listMonitoredDirs;
	listOfStrings listDirectories;

	g_pPlutoLogger->Write(LV_STATUS, "Running Voicemail Monitor");

	FileUtils::FindDirectories(listDirectories, sDirectory, true, true, 0, "", NULL);

	for (listOfStrings::iterator it_lOS = listDirectories.begin(); it_lOS != listDirectories.end(); it_lOS++)
	{
		WatchDir(listMonitoredDirs, *it_lOS, obj_Inotify);
	}

	while (true)
	{
		cpp_inotify_event event = obj_Inotify.get_event();

		OperateFile(listMonitoredDirs, obj_Inotify, event);
	}

	return 0;
}
