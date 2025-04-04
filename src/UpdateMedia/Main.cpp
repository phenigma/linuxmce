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

#include "FileStatusObserver.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

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
#include "pluto_media/Define_AttributeType.h"

#include "DCE/Message.h"
#include "DCE/DeviceData_Impl.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_DeviceTemplate.h"

#include "version.h"

using namespace std;
using namespace DCE;

namespace UpdateMediaVars
{
	bool bError, bUpdateThumbnails, bUpdateSearchTokens, bRunAsDaemon, bSyncAttributes;
    string sDirectory;
    string sUPnPMountPoint;
    string sLocalUPnPServerName;
	bool bSyncFilesOnly;
	bool bDryRun;
	vector<string> vectModifiedFolders;

    Database_pluto_media *g_pDatabase_pluto_media = NULL;
    Database_pluto_main *g_pDatabase_pluto_main = NULL;

    pluto_pthread_mutex_t g_ConnectionMutex("connections");
	pluto_pthread_mutex_t g_FoldersListMutex("folders list");
	pthread_cond_t g_ActionCond;
}

using namespace UpdateMediaVars;


/*
 * This method syncs the pictures for attributes and files. 
 * It updates pictures for attributes of type performer, album and title where there is no currently assigned picture attribute.
 * It finds pictures to assign to this by looking at pictures assigned to the file that has these attributes (Album Artist/Performer/Album).
 *
 * - Attribute (PK_Attribute)
 *     |  FK_Attribute
 *     \- File_Attribute
 *     |   \ FK_File
 *     |    -  Picture_File (FK_Picture)
 *     \ FK_Attribute
 *      - Picture_Attribute (where FK_Picture is NULL)
 */
void SyncAttributes()
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Synchronizing pictures for attributes... "); 

	// Assign all file pictures to the corresponding title attribute
	string sqlTitle =
		"SELECT PK_Attribute,min(Picture_File.FK_Picture) as FK_Picture, Attribute.Name, AttributeType.Description, File.Filename, File.Path FROM Attribute "
		"JOIN File_Attribute ON File_Attribute.FK_Attribute=PK_Attribute "
		"JOIN Picture_File ON Picture_File.FK_File=File_Attribute.FK_File "
		"JOIN File ON File_Attribute.FK_File=PK_File "
		"JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "
		"LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
		"WHERE Picture_Attribute.FK_Picture is NULL AND FK_AttributeType IN (" 
		TOSTRING(ATTRIBUTETYPE_Title_CONST) ") "
		"GROUP BY PK_Attribute";

	PlutoSqlResult result;
	DB_ROW row;
	if((result.r = g_pDatabase_pluto_media->db_wrapper_query_result(sqlTitle)))
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Assigning Picture PK=%d from File '%s/%s' to Title Attribute '%s'(%s)",
							    row[1], row[5], row[4], row[2], row[3]);
		}
	}

	int nAffectedRecords = 0;
	if (!bDryRun) {
		int rows = g_pDatabase_pluto_media->threaded_db_wrapper_query(
			"INSERT INTO Picture_Attribute(FK_Attribute,FK_Picture) "
			"    SELECT "
			"        PK_Attribute, "
			"        MIN( Picture_File.FK_Picture) AS FK_Picture "
			"    FROM Attribute "
			"        JOIN File_Attribute ON File_Attribute.FK_Attribute=PK_Attribute "
			"        JOIN Picture_File ON Picture_File.FK_File=File_Attribute.FK_File "
			"        LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
			"            WHERE "
			"                Picture_Attribute.FK_Picture IS NULL "
			"            AND "
			"                FK_AttributeType= " TOSTRING(ATTRIBUTETYPE_Title_CONST)
			"    GROUP BY PK_Attribute"
		);

		if (rows == -1) {
			LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attributes sync failed!"); 
		} else {
			nAffectedRecords = rows;
		}
	}

        // Find all Album Artist, Performer and Album attributes that do not have a picture, but that have files with pictures
	string sqlPerformer =
		"SELECT PK_Attribute,min(Picture_File.FK_Picture) as FK_Picture, Attribute.Name, AttributeType.Description, f.Filename, f.Path, f.PK_File, AttributeType.PK_AttributeType FROM Attribute "
		"JOIN File_Attribute tfa ON tfa.FK_Attribute=PK_Attribute "
		"JOIN Picture_File ON Picture_File.FK_File=tfa.FK_File "
		"JOIN File f ON tfa.FK_File=f.PK_File "
		"JOIN AttributeType ON Attribute.FK_AttributeType=PK_AttributeType "
		"LEFT JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=PK_Attribute "
		"WHERE Picture_Attribute.FK_Picture is NULL AND FK_AttributeType IN (" 
		TOSTRING(ATTRIBUTETYPE_Album_Artist_CONST) ", "
		TOSTRING(ATTRIBUTETYPE_Performer_CONST) ", "
		TOSTRING(ATTRIBUTETYPE_Album_CONST) ") " 
		"AND f.EK_MediaType IN (4) "
		"GROUP BY PK_Attribute";

	if((result.r = g_pDatabase_pluto_media->db_wrapper_query_result(sqlPerformer)))
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Attribute '%s'(%s)(PK=%s) is missing picture...",
							    row[2], row[3], row[0]);
			LoggerWrapper::GetInstance()->Write(LV_MEDIA, "  - File '%s/%s' has one, checking..",
							    row[5], row[4]);
			string sqlCheck;
			if (atoi(row[7]) == ATTRIBUTETYPE_Album_Artist_CONST || atoi(row[7]) == ATTRIBUTETYPE_Performer_CONST)
			{
				// Album Artist
				// If assigning to Album Artist/Performer, make sure all songs on this album have the same
				// Album Artist/Performer before assigning the albume picture to the Album Artist/Performer
				// SQL will return ((# of songs on album) - (# of songs on album with same Album Artist/Performer))
				// If *all* tracks match the Album Artist/Performer the picture be added to the Album Artist/Performer
				sqlCheck =
					"SELECT "
					"( "
					"  SELECT "
					"      count(fa.FK_File) "
					"  FROM File_Attribute fa "
					"  JOIN Attribute albumAttr ON "
					"      albumAttr.PK_Attribute = fa.FK_Attribute AND "
					"      albumAttr.FK_AttributeType = " TOSTRING(ATTRIBUTETYPE_Album_CONST)
					"  JOIN File_Attribute f2 ON "
					"      f2.FK_Attribute = albumAttr.PK_Attribute "
					"  WHERE f2.FK_File = "+string(row[6])+ " "
					") - ( "
					"  SELECT COUNT(FK_File) "
					"    FROM File_Attribute "
					"    INNER JOIN Attribute ON FK_Attribute=PK_Attribute "
					"  WHERE FK_AttributeType= " + string(row[7]) + " "
					"    AND PK_Attribute= " + string(row[0]) + " "
					"    AND FK_File IN "
					"    ( "
					"      SELECT FK_File "
					"        FROM File_Attribute "
					"        INNER JOIN Attribute ON FK_Attribute=PK_Attribute "
					"      WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Album_CONST)
					"        AND PK_Attribute = "
					"        ( "
					"          SELECT PK_Attribute "
					"            FROM File_Attribute "
					"            INNER JOIN Attribute ON FK_Attribute=PK_Attribute "
					"          WHERE FK_AttributeType=" TOSTRING(ATTRIBUTETYPE_Album_CONST)
					"            AND FK_File = " + string(row[6]) + " "
					"        ) "
					"    ) "
					") AS fileCount ";
			}
			else if (atoi(row[7]) == ATTRIBUTETYPE_Album_CONST)
			{
				// Album
				// If assigning to album, make sure all files have the same picture
				sqlCheck = "SELECT 0 FROM DUAL"; //(SELECT count(distinct pf.FK_Picture) FROM Picture_File pf " 
			  //					"JOIN File_Attribute f2 ON f2.FK_File = pf.FK_File WHERE f2.FK_Attribute = "+string(row[0])+ " "
			  //		") - 1";
			}

			PlutoSqlResult result2;
			DB_ROW row2;
			if((result2.r = g_pDatabase_pluto_media->db_wrapper_query_result(sqlCheck)))
			{
				if ((row2 = db_wrapper_fetch_row(result2.r)))
				{

					LoggerWrapper::GetInstance()->Write(LV_MEDIA, "  - Result '%s'",
							    row2[0]);
					if (atoi(row2[0]) == 0) {
						LoggerWrapper::GetInstance()->Write(LV_MEDIA, "  - Assigning picture to attribute",
							    row2[0]);
						if (!bDryRun) {
							string sqlInsert = "INSERT INTO Picture_Attribute(FK_Attribute,FK_Picture) VALUES("+string(row[0])+","+string(row[1])+")";
							int rows = g_pDatabase_pluto_media->threaded_db_wrapper_query(sqlInsert);
							if(rows == -1)
								LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Assign picture to attribute - Update failed!");
							else
								++nAffectedRecords;
						}

					}
				}
			}
		}
	}

	if(nAffectedRecords == -1)
		LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Attributes sync failed!"); 
	else
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Attributes sync succeeded! Records affected %d", nAffectedRecords); 
}

void CombineAttributes(int PK_Attribute, int PK_AttributeType, string Name) {
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "DISABLED -- Found duplicated attribute %s, pk %d, type %d. Won't touch it.",
					    Name.c_str(), PK_Attribute, PK_AttributeType);

	//char *AffectedTables[] =
	//{
	//	"File_Attribute",
	//	"Attribute_Settings",
	//	"CoverArtScan",
	//	"Disc_Attribute",
	//	"Download_Attribute",
	//	"LongAttribute",
	//	"Picture_Attribute",
	//	"SearchToken_Attribute"
	//};

	//for(int i = 0; i < sizeof(AffectedTables) / sizeof(char *); ++i)
	//{
	//	string sSqlMoveRowsFromDuplicates =
	//		"UPDATE Attribute JOIN File_Attribute ON FK_Attribute = PK_Attribute\n"
	//		"SET FK_Attribute = " + string(row[dafAttribute]) + "\n"
	//		"WHERE FK_AttributeType = " + string(row[dafAttributerType]) + "\n"
	//		"AND Name = '" + StringUtils::SQLEscape(row[dafName]) + "'\n"
	//		"AND PK_Attribute <> " + row[dafAttribute];
	//	g_pDatabase_pluto_media->threaded_db_wrapper_query(sSqlMoveRowsFromDuplicates);
	//}

	//string sSqlDeleteDuplicates =
	//	"DELETE FROM Attribute\n"
	//	"WHERE FK_AttributeType = " + string(row[dafAttributerType]) + "\n"
	//	"AND Name = '" + StringUtils::SQLEscape(row[dafName]) + "'\n"
	//	"AND PK_Attribute <> " + row[dafAttribute];

	//            int nAffectedRecords = g_pDatabase_pluto_media->threaded_db_wrapper_query(sSqlDeleteDuplicates);
	//if(nAffectedRecords == -1)
	//{
	//	LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Main:CombineAttributes Failed to remove duplicated attributes! Query: %s",
	//		sSqlDeleteDuplicates.c_str());
	//}
	//else
	//{
	//	LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Main:CombineAttributes Removed %d duplicated attributes for '%s'", 
	//		nAffectedRecords, row[dafName]); 
	//}

}
void RemoveDuplicatedAttributes()
{
	string sSqlDuplicatedAttributes =
		"SELECT "
		"    FK_AttributeType, "
		"    Name, "
		"    PK_Attribute "
		"FROM "
		"    Attribute "
		"GROUP BY "
		"    FK_AttributeType, "
		"    Name "
		"HAVING "
		"    Count(PK_Attribute) > 1";

	enum DuplicatedAttributesFields
	{
		dafAttributerType,
		dafName,
		dafAttribute
	};

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "DISABLED -- Main:RemoveDuplicatedAttributes Removing duplicated attributes..."); 

	PlutoSqlResult result;
	DB_ROW row;
	if((result.r = g_pDatabase_pluto_media->db_wrapper_query_result(sSqlDuplicatedAttributes)))
	{
		while((row = db_wrapper_fetch_row(result.r)))
		{
			if(NULL != row[dafName] && NULL != row[dafAttributerType] && NULL != row[dafAttribute])
			{
				CombineAttributes(atoi(row[dafAttribute]), atoi(row[dafAttributerType]), row[dafName]);
			}
		}
	}

	LoggerWrapper::GetInstance()->Write(LV_WARNING, "DISABLED -- RemoveDuplicatedAttributes Finished removing duplicated attributes!"); 
}

void *UpdateMediaThread(void *)
{
	SyncAttributes();
	RemoveDuplicatedAttributes();

	PlutoMediaIdentifier::Activate(g_pDatabase_pluto_main);

	vector<string> vsUPnPDevices;

	while(true)
	{
		//load info about ModificationData, AttrCount, AttrDate, attributes, timestamp for all files

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:UpdateMediaThread Loading fresh data from db...");

		MediaState::Instance().LoadDbInfo(g_pDatabase_pluto_media, UpdateMediaVars::sDirectory);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:UpdateMediaThread Loaded fresh data from db");
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:UpdateMediaThread Worker thread: \"I'm wake!\"");

		//UPnP changes: as UPnP mount share doesn't work with inotify (??
		//at least I don't see it firing any expected events, we are manually
		//checking contents of 'devices' list for any changes
		//FIXME if UPnP server went down and up with new content within 2 mins sleep
		//changes in it's contents can go unnoticed
		if (!sUPnPMountPoint.empty())
		{
			vector<string> vsNewDevices;
			FileUtils::ReadFileIntoVector(sUPnPMountPoint+"/devices", vsNewDevices);
			sort(vsNewDevices.begin(), vsNewDevices.end());

			vector<string> vsChanges;
			set_symmetric_difference(vsNewDevices.begin(), vsNewDevices.end(), vsUPnPDevices.begin(), vsUPnPDevices.end(), back_inserter(vsChanges));

			if (!vsChanges.empty())
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Main:UpdateMediaThread UPnP mount point devices list changed, adding %s for processing", sUPnPMountPoint.c_str());
				vsUPnPDevices = vsNewDevices;
				vectModifiedFolders.push_back(sUPnPMountPoint);
			}
			else
			{
				//TODO process list if devices list appear to be same - see fixme note above
			}
		}

		PLUTO_SAFETY_LOCK(flm, g_FoldersListMutex);
		while(vectModifiedFolders.size())
		{
			string sItem = vectModifiedFolders.front();
			flm.Release();

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:UpdateMediaThread Folder to process: %s", sItem.c_str());
			PLUTO_SAFETY_LOCK(cm, g_ConnectionMutex );

			PlutoMediaFile::ResetNewFilesAddedStatus();

			UpdateMedia engine(g_pDatabase_pluto_media, g_pDatabase_pluto_main, sItem);
			engine.LoadExtensions();
			engine.DoIt();

			if( bUpdateSearchTokens )
				engine.UpdateSearchTokens();

			if( bUpdateThumbnails )
				engine.UpdateThumbnails();

			LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:UpdateMediaThread Synchronized '%s'.", sItem.c_str());

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

				RemoveDuplicatedAttributes();

				LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Main:UpdateMediaThread Command \"Check for new files\" sent!");

			}

			flm.Relock();
			vectModifiedFolders.erase(vectModifiedFolders.begin());
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:UpdateMediaThread Nothing to process, sleeping 2 minute...");
		timespec abstime;
		abstime.tv_sec = (long) (time(NULL) + 120);  //2 minutes
		abstime.tv_nsec = 0;
		flm.TimedCondWait(abstime);
	}
	return NULL;
}

void OnModify(list<string> &listFiles)
{
	for(list<string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
	{
		string sItem = *it;

		struct stat st;
		if(0 != stat(sItem.c_str(), &st))
		{

			LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Main:OnModify stat failed for %s. We'll try to parent!", sItem.c_str());

			size_t nPos = sItem.rfind("/");
			if(nPos != string::npos)
				sItem = sItem.substr(0, nPos);

			if(0 != stat(sItem.c_str(), &st))
			{

				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Main:OnModify stat failed for %s. We'll skip it!", sItem.c_str());

				continue;
			}
		}

		if(!(st.st_mode & S_IFDIR))
		{
			//this is a file, we need the folder from where is the file
			size_t nPos = sItem.rfind("/");
			if(nPos != string::npos)
				sItem = sItem.substr(0, nPos);
		}

		LoggerWrapper::GetInstance()->Write(LV_MEDIA, "Main:OnModify New folder %s to sync...", sItem.c_str());        

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

int connectToDataBase(string sDBHost, string sDBUser, string sDBPassword, string sPlutoMediaDbName, string sPlutoMainDbName, int iDBPort)
{

        //connect to the databases
        g_pDatabase_pluto_media = new Database_pluto_media(LoggerWrapper::GetInstance());
        if( !g_pDatabase_pluto_media->Connect(sDBHost,sDBUser,sDBPassword,sPlutoMediaDbName,iDBPort) )
        {
            LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Main:connectToDataBase Cannot connect to database!" );
            return 1;
        }

        g_pDatabase_pluto_main = new Database_pluto_main(LoggerWrapper::GetInstance());
        if( !g_pDatabase_pluto_main->Connect(sDBHost,sDBUser,sDBPassword, sPlutoMainDbName,iDBPort) )
        {
            LoggerWrapper::GetInstance()->Write( LV_CRITICAL, "Main:connectToDataBase Cannot connect to database!" );
            return 2;
        }
	return 0;
}

void closeDatabase() {
	delete g_pDatabase_pluto_media;
	g_pDatabase_pluto_media = NULL;
	delete g_pDatabase_pluto_main;
	g_pDatabase_pluto_main = NULL;
}

int main(int argc, char *argv[])
{
	//make sure we are logging everything
	LoggerWrapper::GetInstance()->LogAll();

	bError=false;
	bUpdateThumbnails=false;
	bUpdateSearchTokens=false;
	bRunAsDaemon=false;
	bSyncFilesOnly=false;
	bDryRun=false;
	bSyncAttributes=false;
	sDirectory="/home/";

        string sPlutoMediaDbName = "pluto_media";
        string sPlutoMainDbName = "pluto_main";


	string sDBHost;
	string sDBUser;
	string sDBPassword;
	int iDBPort;

	{
		DCEConfig dceConfig;
		dceConfig.m_sDBName="pluto_media";

		sDBHost = dceConfig.m_sDBHost;
		sDBUser = dceConfig.m_sDBUser;
		sDBPassword = dceConfig.m_sDBPassword;
		iDBPort = dceConfig.m_iDBPort;

		if(dceConfig.m_mapParameters_Exists("DVDKeysCache"))
		{
			string sDVDKeysCache = dceConfig.m_mapParameters_Find("DVDKeysCache");
			PlutoMediaFile::DVDKeysCacheSetup(sDVDKeysCache);
		}
	}

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
			sDBHost = argv[++optnum];
			break;
		case 'u':
			sDBUser = argv[++optnum];
			break;
		case 'p':
			sDBPassword = argv[++optnum];
			break;
		case 'D':
			sPlutoMainDbName = argv[++optnum];
			break;
		case 'M':
			sPlutoMediaDbName = argv[++optnum];
			break;
		case 'P':
			iDBPort = atoi(argv[++optnum]);
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
		case 'U':
			sUPnPMountPoint = argv[++optnum];
			break;
		case 'r':
			bDryRun = true;
			break;
		case 'a':
			bSyncAttributes = true;
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
			<< "Usage: UpdateMedia [-h hostname] [-u username] [-p password]" << endl
			<< "[-D main database] [-M media database] [-P mysql port]" << endl
			<< "[-d The list with directories, pipe delimited] [-U UPnP mount point to scan] [-s] [-t]" << endl
			<< "hostname       -- address or DNS of database host, default is `dce_router`" << endl
			<< "username       -- username for database connection" << endl
			<< "password       -- password for database connection, default is `` (empty)" << endl
			<< "main database  -- main database name.  default is pluto_main" << endl
			<< "media database -- media database name.  default is pluto_media" << endl
			<< "-s             -- Update all search tokens" << endl
			<< "-t             -- Update all thumbnails" << endl
			<< "-w             -- Synchronize files only" << endl
			<< "-B             -- Run as daemon" << endl
			<< "-r             -- dRy run, don't change anything(applies to sync attributes only)" << endl
			<< "-a             -- sync Attributes" << endl
			<< "Directory defaults to /home" << endl;

		exit(1);
	}

	if (bDryRun)
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Doing a DRY RUN!");
	}
	// detecting local UPnP server name
	if (sUPnPMountPoint!="")
	{
		//FIXME add real detection here
		sLocalUPnPServerName = "LinuxMCE";
	}

	if(!bRunAsDaemon)
	{
		vector<string> vectFolders;
		StringUtils::Tokenize(sDirectory, "|", vectFolders);
		if (!bDryRun)
		{
			// TODO: make updatemedia support dry run
			for(vector<string>::iterator it = vectFolders.begin(); it != vectFolders.end(); ++it)
			{
				string sFolder = *it;

				UpdateMedia UpdateMedia(sDBHost,sDBUser,sDBPassword,iDBPort,sFolder,bSyncFilesOnly);
				if(!sFolder.empty())
					UpdateMedia.DoIt();

				if( bUpdateSearchTokens )
					UpdateMedia.UpdateSearchTokens();

				if( bUpdateThumbnails )
					UpdateMedia.UpdateThumbnails();
			}
		}

		if (bSyncAttributes) {

			int res = connectToDataBase(sDBHost,sDBUser,sDBPassword,sPlutoMediaDbName, sPlutoMainDbName, iDBPort);
			if (res) {
				return res;
			}

			SyncAttributes();

			closeDatabase();
		}
		// extra code to process UPnP mount point
		if (!sUPnPMountPoint.empty())
		{
			UpdateMedia UpdateMedia(sDBHost,sDBUser,sDBPassword,iDBPort,sUPnPMountPoint,bSyncFilesOnly);
			UpdateMedia.DoIt();

			//TODO check and uncomment if necessary
/*
			if( bUpdateSearchTokens )
				UpdateMedia.UpdateSearchTokens();

			if( bUpdateThumbnails )
				UpdateMedia.UpdateThumbnails();
*/
		}
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:main Running as daemon... ");

		pthread_cond_init(&g_ActionCond, NULL);
		g_ConnectionMutex.Init(NULL);
		g_FoldersListMutex.Init(NULL, &g_ActionCond);


#ifdef USE_DEVEL_DATABASES
		sPlutoMediaDbName = "pluto_media_devel";
		sPlutoMainDbName = "pluto_main_devel";
#endif

		int res = connectToDataBase(sDBHost,sDBUser,sDBPassword,sPlutoMediaDbName, sPlutoMainDbName, iDBPort);
		if (res) {
			return res;
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:main Creating FileNotifier... ");

		FileNotifier fileNotifier(g_pDatabase_pluto_media);
		fileNotifier.RegisterCallbacks(OnModify, OnModify); //we'll use the same callback for OnCreate and OnDelete events

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:main Setting FileNotifier.Watch(es)... ");
		vector<string> vectFolders;
		StringUtils::Tokenize(sDirectory, "|", vectFolders);
		for(vector<string>::iterator it = vectFolders.begin(); it != vectFolders.end(); ++it)
		{
	  		fileNotifier.Watch(*it);
			vectModifiedFolders.push_back(*it);
		}

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:main Creating UpdateMediaThread... ");
		pthread_t UpdateMediaThreadId;
		pthread_create(&UpdateMediaThreadId, NULL, UpdateMediaThread, NULL);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:main Creating FileStatusObserver(fileNotifier)... ");
		FileStatusObserver::Instance().SetFileNotifier(&fileNotifier);

		LoggerWrapper::GetInstance()->Write(LV_STATUS, "Main:main Starting FileNotifier... ");
		fileNotifier.Run();//it waits for worker thread to exit; the user must press CTRL+C to finish it

		FileStatusObserver::Instance().UnsetFileNotifier();

		closeDatabase();
	}

	FileStatusObserver::Instance().Finalize();
	MutexTracking::Delete();

	return 0;
}

