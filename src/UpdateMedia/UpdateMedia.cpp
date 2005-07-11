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
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else

#endif


#define  VERSION "<=version=>"

using namespace std;
using namespace DCE;

UpdateMedia::UpdateMedia(string host, string user, string pass, int port,string sDirectory)
{
	m_pDatabase_pluto_media = new Database_pluto_media( );
	if( !m_pDatabase_pluto_media->Connect( host, user, pass, "pluto_media", port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}

	m_pDatabase_pluto_main = new Database_pluto_main( );
	if( !m_pDatabase_pluto_main->Connect( host, user, pass, "pluto_main", port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}

	m_sDirectory=StringUtils::Replace(&sDirectory,"\\","/");  // Be sure no Windows \'s

	vector<Row_MediaType *> vectRow_MediaType;
	m_pDatabase_pluto_main->MediaType_get()->GetRows("1=1",&vectRow_MediaType);
	for(size_t s=0;s<vectRow_MediaType.size();++s)
	{
		Row_MediaType *pRow_MediaType = vectRow_MediaType[s];
		string::size_type pos=0;
		string sExtensions = pRow_MediaType->Extensions_get();
		while(pos < sExtensions.size() )
			m_mapExtensions[StringUtils::Tokenize(sExtensions,",",pos)] = pRow_MediaType->PK_MediaType_get();
	}

	vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
	m_pDatabase_pluto_main->DeviceTemplate_MediaType_get()->GetRows("1=1",&vectRow_DeviceTemplate_MediaType);
	for(size_t s=0;s<vectRow_DeviceTemplate_MediaType.size();++s)
	{
		Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[s];
		string::size_type pos=0;
		string sExtensions = pRow_DeviceTemplate_MediaType->Extensions_get();
		while(pos < sExtensions.size() )
			m_mapExtensions[StringUtils::Tokenize(sExtensions,",",pos)] = pRow_DeviceTemplate_MediaType->FK_MediaType_get();
	}
	
#ifdef WIN32
	chdir("Z:\\");
#endif

	// We don't want a trailing slash on the directory
	if( m_sDirectory.size() && m_sDirectory[ m_sDirectory.size()-1 ]=='/' )
		m_sDirectory = m_sDirectory.substr(0,m_sDirectory.size()-1);
}

void UpdateMedia::DoIt()
{
	if( !m_pDatabase_pluto_media->m_bConnected )
	{
		cerr << "Cannot connect to database" << endl;
		exit(1);
	}

	ReadDirectory(m_sDirectory);
}

int UpdateMedia::ReadDirectory(string sDirectory)
{
	// Build a list of the files on disk, and a map of those in the database
	int PK_Picture=0;
	list<string> listFilesOnDisk;
	FileUtils::FindFiles(listFilesOnDisk,sDirectory,m_sExtensions);

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
		// Is it in the database?
		int PK_File=0;
		map<string,pair<Row_File *,bool> >::iterator itMapFiles = mapFiles.find( *it );
		if( itMapFiles==mapFiles.end() )
		{
			// Nope.  It's either a new file, or it was moved here from some other directory.  If so,
			// then the the attribute should be set.
			PK_File = GetFileAttribute(sDirectory,*it);
cout << *it << " not in db-attr: " << PK_File << endl;
			if( !PK_File )
			{
				// Is it a media file?
				string sExtension = FileUtils::FindExtension(*it);
				int PK_MediaType = m_mapExtensions[sExtension];
				if( PK_MediaType )
					PK_File = AddFileToDatabase(PK_MediaType,sDirectory,*it);
				else
				{
cout << "not a media file" << endl;
					continue; // Nothing to do
}
			}
			else
			{
				Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
				if( !pRow_File )
				{
					SetFileAttribute(sDirectory,*it,0);  // The file doesn't exist.  Shouldn't really happen
cout << "Huh??  not in db now" << endl;
					continue; //  Nothing to do
				}
				// it was in the database, but it's been moved to a different directory
				pRow_File->Path_set( sDirectory );
				pRow_File->Table_File_get()->Commit();
			}
		}
		else
		{
			Row_File *pRow_File = itMapFiles->second.first;
			PK_File = pRow_File->PK_File_get();
cout << *it << " exists in db as: " << PK_File << endl;
			itMapFiles->second.second = true;  // We have the file
			// It's in the database already.  Be sure the attribute is set
			SetFileAttribute(sDirectory,*it,PK_File);  // The file doesn't exist.  Shouldn't really happen
		}

		int i = GetPicForFileOrDirectory(sDirectory + "/" + *it,PK_File);
		if( !PK_Picture )
			PK_Picture = i;
	}

	// See if there are any files in the database that aren't in the directory any more
	for(map<string,pair<Row_File *,bool> >::iterator it=mapFiles.begin();it!=mapFiles.end();++it)
	{
		Row_File *pRow_File = it->second.first;
		pRow_File->Missing_set(it->second.second==false);
cout << it->first << " exists: " << it->second.second << endl;
		pRow_File->Table_File_get()->Commit();
	}

	// Now recurse
	list<string> listSubDirectories;
	FileUtils::FindDirectories(listSubDirectories,sDirectory,false,true);

cout << (int) listSubDirectories.size() << " sub directories" << endl;
	for(list<string>::iterator it=listSubDirectories.begin();it!=listSubDirectories.end();++it)
	{
		string sSubDir = *it;
		if( StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="VIDEO_TS" || StringUtils::ToUpper(FileUtils::FilenameWithoutPath(sSubDir))=="AUDIO_TS" )
		{
cout << sDirectory << " is a ripped dvd" << endl;
#ifndef WIN32
			attr_set( sDirectory.c_str( ), "DIR_AS_FILE", "1", "1", 0 );
#endif
			break; // Don't recurse anymore
		}
		int i = ReadDirectory(sSubDir);
		if( !PK_Picture )
			PK_Picture = i;
	}

	// Whatever was the first picture we found will be the one for this directory
	int PK_Picture_Directory = GetPicForFileOrDirectory(sDirectory,0);
	if( PK_Picture_Directory )
		PK_Picture = PK_Picture_Directory;  // This takes priority

	SetPicAttribute("",sDirectory,PK_Picture);

	return PK_Picture;
}

void UpdateMedia::SetPicAttribute(string sDirectory,string sFile,int PK_Picture)
{
	cout << "SetPicAttribute " << sDirectory << " / " << sFile << " " << PK_Picture  << endl;
#ifndef WIN32
	string sPK_Picture = StringUtils::itos(PK_Picture);
	if( m_sDirectory.size() )
		attr_set( (sDirectory + "/" + sFile).c_str( ), "PIC", sPK_Picture.c_str( ), sPK_Picture.length( ), 0 );
	else
		attr_set( sFile.c_str( ), "PIC", sPK_Picture.c_str( ), sPK_Picture.length( ), 0 );
#endif
}

void UpdateMedia::SetFileAttribute(string sDirectory,string sFile,int PK_File)
{
	cout << "SetFileAttribute " << sDirectory << " / " << sFile << " " << PK_File  << endl;
#ifndef WIN32
	string sPK_File = StringUtils::itos(PK_File);
	attr_set( (sDirectory + "/" + sFile).c_str( ), "ID", sPK_File.c_str( ), sPK_File.length( ), 0 );
#endif
}

int UpdateMedia::GetFileAttribute(string sDirectory,string sFile)
{
#ifdef WIN32
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(sDirectory) + "' AND Filename='" + StringUtils::SQLEscape(sFile) + "'",&vectRow_File);
	if( vectRow_File.size() )
		return vectRow_File[0]->PK_File_get();
	else
		return 0;
#else
    int n = 79;
    char value[80];
    memset( value, 0, sizeof( value ) );

	if ( attr_get( (sDirectory + "/" + sFile).c_str( ), "ID", value, &n, 0 ) == 0 )
{
cout << "GetFileAttribute " << sDirectory << " / " << sFile << " " << value << endl;
		return atoi( value );
}
#endif
cout << "GetFileAttribute " << sDirectory << " / " << sFile << " not found " << endl;
	return 0;
}        


int UpdateMedia::AddFileToDatabase(int PK_MediaType,string sDirectory,string sFile)
{
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
	pRow_File->Path_set(sDirectory);
	pRow_File->Filename_set(sFile);
	pRow_File->EK_MediaType_set(PK_MediaType);
	// TODO: Add attributes from ID3 tags
	pRow_File->Table_File_get()->Commit();
cout << "Added " << sDirectory << " / " << sFile << " to db " << pRow_File->PK_File_get() << endl;
	return pRow_File->PK_File_get();
}

int UpdateMedia::GetPicForFileOrDirectory(string sFile,int PK_File)
{
	if( !PK_File )
	{
		vector<Row_File *> vectRow_File;
		m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(FileUtils::BasePath(sFile)) + 
			"' AND Filename='" + StringUtils::SQLEscape(FileUtils::FilenameWithoutPath(sFile)) + "'",
			&vectRow_File);
		if( vectRow_File.size() )
			PK_File = vectRow_File[0]->PK_File_get();
		else
			return 0;  // Can do nothing.  This isn't in the database
	}

	vector<Row_Picture_File *> vectPicture_File;
	m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File=" + StringUtils::itos(PK_File),&vectPicture_File);
cout << "Found " << (int) vectPicture_File.size() << " pics for file" << endl;
	if( vectPicture_File.size() )
	{
		SetPicAttribute("",sFile,vectPicture_File[0]->FK_Picture_get());
		return vectPicture_File[0]->FK_Picture_get();  // The first pic for this directory
	}

	// Does one of the attributes have a picture
	vector<Row_Picture_Attribute *> vectPicture_Attribute;
	m_pDatabase_pluto_media->Picture_Attribute_get()->GetRows(
		"JOIN File_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "
		" JOIN Attribute ON Picture_Attribute.FK_Attribute=Attribute.PK_Attribute "
		" JOIN AttributeType ON Attribute.FK_AttributeType=AttributeType.PK_AttributeType "
		" WHERE FK_File=" + 
		StringUtils::itos(PK_File) +
		" ORDER BY `PicPriority`",&vectPicture_Attribute);

cout << "Found " << (int) vectPicture_Attribute.size() << " pics for attribute" << endl;
	if( vectPicture_Attribute.size() )
	{
		SetPicAttribute("",sFile,vectPicture_Attribute[0]->FK_Picture_get());
		return vectPicture_Attribute[0]->FK_Picture_get();  // The first pic for this directory
	}
	return 0;
}
