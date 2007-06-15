/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"
#include "UpdateMedia.h"
#include "PlutoUtils/MultiThreadIncludes.h"

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
#include "PlutoMediaFile.h"
#include "PlutoMediaFile.h"
#include "pluto_media/Define_AttributeType.h"

#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceTemplate.h"

#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;
DCEConfig dceConfig;

namespace UpdateMediaVars
{
    bool bError, bUpdateThumbnails, bUpdateSearchTokens, bRunAsDaemon;
    string sDirectory;
	bool bSyncFilesOnly;
	vector<string> vectModifiedFolders;

    Database_pluto_media *g_pDatabase_pluto_media = NULL;
    Database_pluto_main *g_pDatabase_pluto_main = NULL;

    pluto_pthread_mutex_t g_ConnectionMutex("connections");
	pluto_pthread_mutex_t g_FoldersListMutex("folders list");
	pthread_cond_t g_ActionCond;
}

using namespace UpdateMediaVars;



void SyncAttributes()
{
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Synchronizing attributes... "); 

    int nAffectedRecords = g_pDatabase_pluto_media->threaded_db_wrapper_query(
		"INSERT INTO Picture_Attribute(FK_Attribute,FK_Picture) "
		"SELECT PK_Attribute,min(Picture_File.FK_Picture) as FK_Picture FROM Attribute "
		"JOIN File_Attribute ON File_Attribute.FK_Attribute=PK_Attribute "
		"JOIN Picture_File ON Picture_File.FK_File=File_Attribute.FK_File "
		"LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
		"WHERE Picture_Attribute.FK_Picture is NULL AND FK_AttributeType IN (" 
		TOSTRING(ATTRIBUTETYPE_Performer_CONST) ", "
		TOSTRING(ATTRIBUTETYPE_Album_CONST) ", " 
		TOSTRING(ATTRIBUTETYPE_Title_CONST) ") "
		"GROUP BY PK_Attribute"
	);

	if(nAffectedRecords == -1)
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attributes sync failed!"); 
	else
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Attributes sync succeeded! Records affected %d", nAffectedRecords); 
}

void *UpdateMediaThread(void *)
{
	SyncAttributes();

	PlutoMediaIdentifier::Activate(g_pDatabase_pluto_main);

	time_t tStart = time(NULL);

	while(true)
	{
		//load info about ModificationData, AttrCount, AttrDate, attributes, timestamp for all files
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loading fresh data from db...");
		MediaState::Instance().LoadDbInfo(g_pDatabase_pluto_media, FileUtils::ExcludeTrailingSlash(UpdateMediaVars::sDirectory));
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Loaded fresh data from db");

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Worker thread: \"I'm wake!\"");        

		PLUTO_SAFETY_LOCK(flm, g_FoldersListMutex);
		while(vectModifiedFolders.size())
		{
			string sItem = vectModifiedFolders.front();
			flm.Release();

			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Folder to process: %s", sItem.c_str());	
			PLUTO_SAFETY_LOCK(cm, g_ConnectionMutex );
			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Synchronizing '%s'...", sItem.c_str());	

			PlutoMediaFile::ResetNewFilesAddedStatus();
				
			UpdateMedia engine(g_pDatabase_pluto_media, g_pDatabase_pluto_main, sItem);
			engine.LoadExtensions();
			engine.DoIt();

			if( bUpdateSearchTokens )
				engine.UpdateSearchTokens();

			if( bUpdateThumbnails )
				engine.UpdateThumbnails();

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Synchronized '%s'.", sItem.c_str());

			if(PlutoMediaFile::NewFilesAdded())
			{
				LoggerWrapper::GetInstance()->Write(LV_STATUS, "New files were added to db for '%s'.", sItem.c_str());

				SyncAttributes();

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Sending \"Check for new files\" command to Media_Plugin...");
				Event_Impl *pEvent = new Event_Impl(DEVICEID_MESSAGESEND, 0, "dcerouter");
				Message* pMessage = new Message(0, DEVICETEMPLATE_Media_Plugin_CONST, BL_SameHouse, 
					MESSAGETYPE_COMMAND, PRIORITY_NORMAL, 
					COMMAND_Check_For_New_Files_CONST, 0);
				pEvent->SendMessage(pMessage);
				delete pEvent;
				pEvent = NULL;

				LoggerWrapper::GetInstance()->Write(LV_STATUS, "Command \"Check for new files\" sent!");
			}

			flm.Relock();
			vectModifiedFolders.erase(vectModifiedFolders.begin());
		}

		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Nothing to process, sleeping 2 minute...");        
		timespec abstime;
		abstime.tv_sec = (long) (time(NULL) + 120);  //2 minutes
		abstime.tv_nsec = 0;
		flm.TimedCondWait(abstime);		
	}
}

void OnModify(list<string> &listFiles) 
{
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

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "New folder %s to sync...", sItem.c_str());        
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
			pthread_cond_broadcast(&g_ActionCond);
		}
	}
}

int main(int argc, char *argv[])
{
	dceConfig.m_sDBName="pluto_media";
	bError=false;
	bUpdateThumbnails=false;
	bUpdateSearchTokens=false;
	bRunAsDaemon=false;
	bSyncFilesOnly=false;
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
		case 'w':
			bSyncFilesOnly = true;
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
			<< "-w			-- Synchronize files only" << endl
			<< "-B          -- Run as daemon" << endl
			<< "Directory defaults to /home" << endl;

		exit(1);
	}

	if(!bRunAsDaemon)
	{
		UpdateMedia UpdateMedia(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword,
			dceConfig.m_iDBPort,sDirectory,bSyncFilesOnly);
		if( sDirectory.size() )
			UpdateMedia.DoIt();

		if( bUpdateSearchTokens )
			UpdateMedia.UpdateSearchTokens();

		if( bUpdateThumbnails )
			UpdateMedia.UpdateThumbnails();
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Running as daemon... ");

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
        g_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
        if( !g_pDatabase_pluto_media->Connect(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword, sPlutoMediaDbName,dceConfig.m_iDBPort) )
        {
            LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
            return 1;
        }

        g_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
        if( !g_pDatabase_pluto_main->Connect(dceConfig.m_sDBHost,dceConfig.m_sDBUser,dceConfig.m_sDBPassword, sPlutoMainDbName,dceConfig.m_iDBPort) )
        {
            LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Cannot connect to database!" );
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

		delete g_pDatabase_pluto_media;
		g_pDatabase_pluto_media = NULL;
		delete g_pDatabase_pluto_main;
		g_pDatabase_pluto_main = NULL;
	}

	MutexTracking::Delete();
	
	return 0;
}

