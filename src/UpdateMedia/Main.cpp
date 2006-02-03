#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "UpdateMedia.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#endif

#include "inotify/FileNotifier.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;

namespace UpdateMediaVars
{
    bool bError, bUpdateThumbnails, bUpdateSearchTokens, bRunAsDaemon;
    string sDirectory;
	vector<string> vectModifiedFolders;

    Database_pluto_media *g_pDatabase_pluto_media = NULL;
    Database_pluto_main *g_pDatabase_pluto_main = NULL;

    pluto_pthread_mutex_t g_ConnectionMutex("connections");
	pluto_pthread_mutex_t g_FoldersListMutex("folders list");
	pthread_cond_t g_ActionCond;
}

using namespace UpdateMediaVars;

namespace DCE
{
	Logger *g_pPlutoLogger;
}

void *UpdateMediaThread(void *)
{
	while(true)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Worker thread: \"I'm wake!\"");        
		PLUTO_SAFETY_LOCK(flm, g_FoldersListMutex);

		while(vectModifiedFolders.size())
		{
			string sItem = vectModifiedFolders.front();
			flm.Release();

			g_pPlutoLogger->Write(LV_WARNING, "Folder to sync: %s", sItem.c_str());	
			PLUTO_SAFETY_LOCK(cm, g_ConnectionMutex );
			g_pPlutoLogger->Write(LV_STATUS, "Synchronizing '%s'...", sItem.c_str());	

			UpdateMedia UpdateMedia(g_pDatabase_pluto_media, g_pDatabase_pluto_main, sItem);
			UpdateMedia.DoIt();

			if( bUpdateSearchTokens )
				UpdateMedia.UpdateSearchTokens();

			if( bUpdateThumbnails )
				UpdateMedia.UpdateThumbnails();

			g_pPlutoLogger->Write(LV_STATUS, "Synchronized '%s'.", sItem.c_str());

			flm.Relock();
			vectModifiedFolders.erase(vectModifiedFolders.begin());
		}

		g_pPlutoLogger->Write(LV_WARNING, "Nothing to process, sleeping 1 minute...");        
		timespec abstime;
		abstime.tv_sec = (long) (time(NULL) + 60); 
		abstime.tv_nsec = 0;
		flm.TimedCondWait(abstime);		
	}
}

void OnModify(list<string> &listFiles) 
{
	g_pPlutoLogger->Write(LV_STATUS, "Notified with new event...");        

	for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
	{
		string sItem = *it;

		struct stat st;
		stat(sItem.c_str(), &st);

		if(!(st.st_mode & S_IFDIR))
		{
			//this is a file, we need the folder from where is the file
			size_t nPos = sItem.rfind("/");
			if(nPos != string::npos)
				sItem = sItem.substr(0, nPos);
		}

		g_pPlutoLogger->Write(LV_STATUS, "New folder %s to sync...", sItem.c_str());        
		PLUTO_SAFETY_LOCK(flm, g_FoldersListMutex);

		bool bFound = false;
		for(vector<string>::iterator it = vectModifiedFolders.begin(); it != vectModifiedFolders.end(); it++)
		{
			if(*it == sItem)
			{
				bFound = true;
				break;
			}
		}

		if(!bFound)
		{
			vectModifiedFolders.push_back(sItem);
			g_pPlutoLogger->Write(LV_STATUS, "Waking up worker thread...");        
			pthread_cond_broadcast(&g_ActionCond);
		}
	}
}

int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger("/var/log/pluto/UpdateMedia.newlog");
	dceConfig.m_sDBName="pluto_media";
	bError=false;
	bUpdateThumbnails=false;
	bUpdateSearchTokens=false;
	bRunAsDaemon=false;
	sDirectory="/home/";

	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		if( argv[optnum][0]!='-' )
		{
			cerr << "Unknown option " << argv[optnum] << endl;
			bError=true;
		}

		c=argv[optnum][1];
		switch (c)
		{
		case 'h':
			dceConfig.m_sDBHost = argv[++optnum];
			break;
		case 'u':
			dceConfig.m_sDBUser = argv[++optnum];
			break;
		case 'p':
			dceConfig.m_sDBPassword = argv[++optnum];
			break;
		case 'P':
			dceConfig.m_iDBPort = atoi(argv[++optnum]);
			break;
		case 'd':
			sDirectory = argv[++optnum];
			break;
		case 's':
			bUpdateSearchTokens=true;
			break;
		case 't':
			bUpdateThumbnails=true;
			break;
		case 'B':
			bRunAsDaemon=true;
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError )
	{
		cout << "UpdateMedia, v." << VERSION << endl
			<< "Usage: UpdateMedia [-h hostname] [-u username] [-p password] [-D database] [-P mysql port]" << endl
			<< "[-d Directory] [-s] [-t]" << endl
			<< "hostname    -- address or DNS of database host, default is `dce_router`" << endl
			<< "username    -- username for database connection" << endl
			<< "password    -- password for database connection, default is `` (empty)" << endl
			<< "database    -- database name.  default is pluto_main" << endl
			<< "-s          -- Update all search tokens" << endl
			<< "-t          -- Update all thumbnails" << endl
#ifndef WIN32
			<< "-B          -- Run as daemon" << endl
#endif
			<< "Directory defaults to /home" << endl;

		exit(0);
	}

	if(!bRunAsDaemon)
	{
		UpdateMedia UpdateMedia(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,dceConfig.m_iDBPort,sDirectory);
		if( sDirectory.size() )
			UpdateMedia.DoIt();

		if( bUpdateSearchTokens )
			UpdateMedia.UpdateSearchTokens();

		if( bUpdateThumbnails )
			UpdateMedia.UpdateThumbnails();
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "Running as daemon... ");

		pthread_cond_init(&g_ActionCond, NULL);
        g_ConnectionMutex.Init(NULL);
		g_FoldersListMutex.Init(NULL, &g_ActionCond);

        string sPlutoMediaDbName = "pluto_media";
        string sPlutoMainDbName = "pluto_main";

#ifdef USE_DEVEL_DATABASES
        sPlutoMediaDbName = "pluto_media_devel";
        sPlutoMainDbName = "pluto_main_devel";
#endif

        //connect to the databases
        g_pDatabase_pluto_media = new Database_pluto_media( );
        if( !g_pDatabase_pluto_media->Connect(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword, sPlutoMediaDbName,dceConfig.m_iDBPort) )
        {
            g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
            return 1;
        }

        g_pDatabase_pluto_main = new Database_pluto_main( );
        if( !g_pDatabase_pluto_main->Connect(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword, sPlutoMainDbName,dceConfig.m_iDBPort) )
        {
            g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
            return 2;
        }

		FileNotifier fileNotifier(g_pDatabase_pluto_media);
		fileNotifier.RegisterCallbacks(OnModify, OnModify); //we'll use the same callback for OnCreate and OnDelete events
  		fileNotifier.Watch(sDirectory);

		pthread_t UpdateMediaThreadId;
		pthread_create(&UpdateMediaThreadId, NULL, UpdateMediaThread, NULL);

		fileNotifier.Run();//it waits for worker thread to exit; the user must press CTRL+C to finish it

        pthread_mutex_destroy(&g_ConnectionMutex.mutex);
		pthread_mutex_destroy(&g_FoldersListMutex.mutex);
	}

	return 0;
}

