#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
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

#ifndef WIN32
#include <dirent.h>
#include <attr/attributes.h>
#endif

#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"

#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"

#include "id3info/id3info.h"
#include "PlutoMediaFile.h"

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else
#endif


#define  VERSION "<=version=>"
//#define  USE_DEVEL_DATABASES 

using namespace std;
using namespace DCE;

UpdateMedia::UpdateMedia(string host, string user, string pass, int port,string sDirectory)
{
    string sPlutoMediaDbName = "pluto_media";
    string sPlutoMainDbName = "pluto_main";

#ifdef USE_DEVEL_DATABASES
    sPlutoMediaDbName = "pluto_media_devel";
    sPlutoMainDbName = "pluto_main_devel";
#endif

    //connect to the databases
	m_pDatabase_pluto_media = new Database_pluto_media( );
	if( !m_pDatabase_pluto_media->Connect( host, user, pass, sPlutoMediaDbName, port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}

	m_pDatabase_pluto_main = new Database_pluto_main( );
	if( !m_pDatabase_pluto_main->Connect( host, user, pass, sPlutoMainDbName, port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}

	m_bAsDaemon = false;
    PlutoMediaIdentifier::Activate(m_pDatabase_pluto_main);

	m_sDirectory = StringUtils::Replace(&sDirectory,"\\","/");  // Be sure no Windows \'s
    FileUtils::ExcludeTrailingSlash(m_sDirectory);

#ifdef WIN32 //for debugging
	chdir("Z:\\");
#endif
}

UpdateMedia::UpdateMedia(Database_pluto_media *pDatabase_pluto_media, 
    Database_pluto_main *pDatabase_pluto_main, string sDirectory)
{
    //reusing connections
    m_pDatabase_pluto_main = pDatabase_pluto_main;
    m_pDatabase_pluto_media = pDatabase_pluto_media;

	m_bAsDaemon = true;
	PlutoMediaIdentifier::Activate(m_pDatabase_pluto_main);

    m_sDirectory = StringUtils::Replace(&sDirectory,"\\","/");  // Be sure no Windows \'s
    FileUtils::ExcludeTrailingSlash(m_sDirectory);
}

void UpdateMedia::DoIt()
{
	if( !m_pDatabase_pluto_media->m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		exit(1);
	}

	ReadDirectory(m_sDirectory, !m_bAsDaemon);
    SyncDbWithDirectory(m_sDirectory); //mark missing/not-missing files, recursively
}

int UpdateMedia::ReadDirectory(string sDirectory, bool bRecursive)
{
	if( sDirectory.size()==0 )
		return 0;
	// Strip any trailing /
	if( sDirectory[ sDirectory.size()-1 ] == '/' )
		sDirectory = sDirectory.substr(0,sDirectory.size()-1);

	// Build a list of the files on disk, and a map of those in the database
	int PK_Picture=0;
	list<string> listFilesOnDisk;
	FileUtils::FindFiles(listFilesOnDisk,sDirectory,m_sExtensions,false,false,0,"");

	map<string,pair<Row_File *,bool> > mapFiles;

	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(sDirectory) + "'",
		&vectRow_File);
	for(size_t s=0;s<vectRow_File.size();++s)
	{
		Row_File *pRow_File = vectRow_File[s];
		mapFiles[pRow_File->Filename_get()] = 
			make_pair<Row_File *, bool>(pRow_File,false);
	}

	cout << "===============" << sDirectory << " disk: " << (int) listFilesOnDisk.size() << " db: " << (int) vectRow_File.size() << endl;

	// Now start matching them up
	for(list<string>::iterator it=listFilesOnDisk.begin();it!=listFilesOnDisk.end();++it)
	{
        string sFile = *it;

		//ignore id3 files
		if(StringUtils::ToLower(FileUtils::FindExtension(sFile)) == "id3")
			continue;

        PlutoMediaFile PlutoMediaFile_(m_pDatabase_pluto_media, m_pDatabase_pluto_main,
            sDirectory, sFile);
            
		// Is it in the database?
		int PK_File=0;
		map<string,pair<Row_File *,bool> >::iterator itMapFiles = mapFiles.find( *it );
		if( itMapFiles==mapFiles.end() )
		{
			PK_File = PlutoMediaFile_.HandleFileNotInDatabase();
			if(!PK_File)
				continue; // Nothing to do
		}	
		else
		{
			Row_File *pRow_File = itMapFiles->second.first;
			PK_File = pRow_File->PK_File_get();
cout << sFile << " exists in db as: " << PK_File << endl;
			itMapFiles->second.second = true;  // We have the file
			// It's in the database already.  Be sure the attribute is set
			PlutoMediaFile_.SetFileAttribute(PK_File);  
		}

        int i = PlutoMediaFile_.GetPicAttribute(PK_File);
		if(!PK_Picture)
			PK_Picture = i;

		if( PK_Picture )
		{
			string sSql="SELECT Attribute.* FROM Attribute"
				" JOIN File_Attribute ON FK_Attribute=PK_Attribute"
				" JOIN File ON FK_File=PK_File"
				" JOIN MediaType_AttributeType ON Attribute.FK_AttributeType=MediaType_AttributeType.FK_AttributeType"
				" AND MediaType_AttributeType.EK_MediaType=File.EK_MediaType"
				" WHERE PK_File=" + StringUtils::itos(PK_File) + " AND Identifier=1";
			vector<Row_Attribute *> vectRow_Attribute;
			m_pDatabase_pluto_media->Attribute_get()->GetRows(sSql,&vectRow_Attribute);
			for(size_t s=0;s<vectRow_Attribute.size();++s)
			{
				Row_Attribute *pRow_Attribute = vectRow_Attribute[s];
				Row_Picture_Attribute *pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->GetRow(PK_Picture,pRow_Attribute->PK_Attribute_get());
				if( !pRow_Picture_Attribute )
				{
					pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->AddRow();
					pRow_Picture_Attribute->FK_Picture_set(PK_Picture);
					pRow_Picture_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
					m_pDatabase_pluto_media->Picture_Attribute_get()->Commit();
				}
			}
		}
		Sleep(50);
	}

	// Now recurse
	list<string> listSubDirectories;
	FileUtils::FindDirectories(listSubDirectories,sDirectory,false,true,0,""
#ifndef WIN32
		,&m_MapInodes
#endif
	);

    PlutoMediaFile PlutoMediaParentFolder(m_pDatabase_pluto_media, m_pDatabase_pluto_main,
        FileUtils::BasePath(sDirectory),FileUtils::FilenameWithoutPath(sDirectory));

    cout << (int) listSubDirectories.size() << " sub directories" << endl;
	for(list<string>::iterator it=listSubDirectories.begin();it!=listSubDirectories.end();++it)
	{
		string sSubDir = *it;

		string SQL = "select count(*) from File Where Path LIKE '" + 
			StringUtils::SQLEscape(FileUtils::ExcludeTrailingSlash(sSubDir)) + "%' AND Missing = 0";

		PlutoSqlResult allresult;
		MYSQL_ROW row;
		if((allresult.r = m_pDatabase_pluto_media->mysql_query_result(SQL)))
		{
			row = mysql_fetch_row(allresult.r);

			if(row && atoi(row[0]) > 0 && !bRecursive)
			{
				g_pPlutoLogger->Write(LV_WARNING, "Skipping subdir %s cause it's already scanned", sSubDir.c_str());
				continue;
			}
		}

        //is sDirectory a ripped dvd ?
		if( 
            StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="VIDEO_TS" || 
            StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="AUDIO_TS" 
        )
		{
             cout << sDirectory << " is a ripped dvd" << endl;

            #ifndef WIN32
			    attr_set( sDirectory.c_str( ), "DIR_AS_FILE", "1", 1, 0 );
            #endif

			// Add this directory like it were a file
			int PK_File = PlutoMediaParentFolder.HandleFileNotInDatabase(MEDIATYPE_pluto_DVD_CONST);
			Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
			pRow_File->IsDirectory_set(false);
			m_pDatabase_pluto_media->File_get()->Commit();

			PlutoMediaParentFolder.SetFileAttribute(PK_File);
			break; // Don't recurse anymore
		}


		map<string,pair<Row_File *,bool> >::iterator itMapFiles = mapFiles.find(FileUtils::FilenameWithoutPath(sSubDir));
		if( itMapFiles!=mapFiles.end() && !itMapFiles->second.first->IsDirectory_get())
		{
            cout << sDirectory << " is a sub-dir already categoriezed ripped dvd" << endl;

            #ifndef WIN32
			    attr_set( (sSubDir).c_str( ), "DIR_AS_FILE", "1", 1, 0 );
            #endif

            PlutoMediaFile PlutoMediaSubDir(m_pDatabase_pluto_media, m_pDatabase_pluto_main,
                FileUtils::BasePath(sSubDir), "");
            PlutoMediaSubDir.SetFileAttribute(itMapFiles->second.first->PK_File_get());
			continue; // This directory is already in the database 
		}

		int i = ReadDirectory(sSubDir, bRecursive);
		if( !PK_Picture )
			PK_Picture = i;

		Sleep(50);
	}

	// Whatever was the first picture we found will be the one for this directory
	int PK_Picture_Directory = PlutoMediaParentFolder.GetPicAttribute(0);
	if( PK_Picture_Directory )
		PK_Picture = PK_Picture_Directory;  // This takes priority

	PlutoMediaParentFolder.SetPicAttribute(PK_Picture, "");

	return PK_Picture;
}

void UpdateMedia::UpdateSearchTokens()
{
	string SQL = "DELETE FROM SearchToken_Attribute";
	m_pDatabase_pluto_media->threaded_mysql_query(SQL);

	SQL = "SELECT PK_Attribute,Name FROM Attribute";

	PlutoSqlResult allresult,result;
    MYSQL_ROW row,row2;
    if( ( allresult.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
	{
		while( ( row=mysql_fetch_row( allresult.r ) ) )
		{
			Sleep(50);

			string sName = row[1];
			string::size_type pos=0;
			while( pos<sName.size() )
			{
				string Token=StringUtils::UpperAZ09Only( StringUtils::Tokenize( sName, " ", pos ) );
				if( Token.length( )==0 )
					continue;
				SQL = "SELECT PK_SearchToken FROM SearchToken WHERE Token='" +
					StringUtils::SQLEscape( Token ) + "'";
				int PK_SearchToken=0;
				if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row2=mysql_fetch_row( result.r ) ) )
				{
					PK_SearchToken = atoi( row2[0] );
				}
				else
				{
					SQL = "INSERT INTO SearchToken( Token ) VALUES( '" +
						StringUtils::SQLEscape( Token ) + "' )";
					PK_SearchToken = m_pDatabase_pluto_media->threaded_mysql_query_withID( SQL );
				}
				if( PK_SearchToken )
				{
					SQL = "INSERT INTO SearchToken_Attribute(FK_SearchToken,FK_Attribute) VALUES( " +
						StringUtils::itos( PK_SearchToken ) + ", " +
						row[0] + " )";
					m_pDatabase_pluto_media->threaded_mysql_query( SQL, true );
				}
			}
		}
	}
}

void UpdateMedia::UpdateThumbnails()
{
	string SQL = "SELECT PK_Picture,Extension FROM Picture";
	PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
	{
		while( ( row=mysql_fetch_row( result.r ) ) )
		{
			Sleep(50);

			time_t tModTime=0,tTnModTime=0;
#ifndef WIN32
			struct stat64 dirEntryStat;
            if ( stat64((string("/home/mediapics/") + row[0] + "." + row[1]).c_str(), &dirEntryStat) == 0 )
				tModTime = dirEntryStat.st_mtime;

            if ( stat64((string("/home/mediapics/") + row[0] + "_tn." + row[1]).c_str(), &dirEntryStat) == 0 )
				tTnModTime = dirEntryStat.st_mtime;
#endif
			if( tModTime>tTnModTime )
			{
				string Cmd = "convert -sample 75x75 /home/mediapics/" + string(row[0])  + "." + row[1] +
					" /home/mediapics/" + row[0] + "_tn." + row[1];
				int result;
				if( ( result=system( Cmd.c_str( ) ) )!=0 )
					g_pPlutoLogger->Write( LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str( ), result );
			}
			else if( tModTime==0 && tTnModTime==0 )
			{
				cout << "Picture " << row[0] << " missing.  Deleting all references" << endl;
				m_pDatabase_pluto_media->threaded_mysql_query("DELETE FROM Picture WHERE PK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_mysql_query("DELETE FROM Picture_Attribute WHERE FK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_mysql_query("DELETE FROM Picture_Disc WHERE FK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_mysql_query("DELETE FROM Picture_File WHERE FK_Picture=" + string(row[0]));
				m_pDatabase_pluto_media->threaded_mysql_query("UPDATE Bookmark SET FK_Picture=NULL WHERE FK_Picture=" + string(row[0]));
			}
		}
	}
}

void UpdateMedia::SyncDbWithDirectory(string sDirectory)
{
    bool bRecordsModified = false;

	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path LIKE '" + StringUtils::SQLEscape(sDirectory) + "/%' OR Path = '" + StringUtils::SQLEscape(sDirectory) + "'",
		&vectRow_File);

	for(size_t s=0;s<vectRow_File.size();++s)
	{
		Row_File *pRow_File = vectRow_File[s];
        string sFilePath = pRow_File->Path_get() + "/" + pRow_File->Filename_get();
        bool bFileIsMissing = !FileUtils::FileExists(sFilePath);
        if(bFileIsMissing && !pRow_File->Missing_get()) 
        {
            pRow_File->Missing_set(1);
            cout << "Marking record as missing in database: " << sFilePath << endl;
            bRecordsModified = true;
        }
        else if(!bFileIsMissing && pRow_File->Missing_get())
        {
            pRow_File->Missing_set(0);
            cout << "Marking record as NOT missing in database: " << sFilePath << endl;
            bRecordsModified = true;
        }
	}

    if(bRecordsModified)
        m_pDatabase_pluto_media->File_get()->Commit();
}
