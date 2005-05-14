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

UpdateMedia::UpdateMedia(string host, string user, string pass, string db_name, int port,string sDirectory,string sExtensions)
{
	m_pDatabase_pluto_media = new Database_pluto_media( );
	if( !m_pDatabase_pluto_media->Connect( host, user, pass, db_name, port ) )
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
		return;
	}
	m_sDirectory=StringUtils::Replace(sDirectory,"\\","/");  // Be sure no Windows \'s
	m_sExtensions=sExtensions;

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
				if( sExtension.size() && m_sExtensions.find( "," + StringUtils::ToUpper(sExtension) + "," )!=string::npos )
					PK_File = AddFileToDatabase(sDirectory,*it);
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


		// If we're here, then this is a file and it's in the database.  Look for the picture
		vector<Row_Picture_File *> vectPicture_File;
		m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File=" + StringUtils::itos(PK_File),&vectPicture_File);
cout << "Found " << (int) vectPicture_File.size() << " pics for file" << endl;
		if( vectPicture_File.size() )
		{
			SetPicAttribute(sDirectory,*it,vectPicture_File[0]->FK_Picture_get());
			if( !PK_Picture )
				PK_Picture = vectPicture_File[0]->FK_Picture_get();  // The first pic for this directory
			continue;
		}

		// Does one of the attributes have a picture
		vector<Row_Picture_Attribute *> vectPicture_Attribute;
		m_pDatabase_pluto_media->Picture_Attribute_get()->GetRows(
			"JOIN File_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute WHERE FK_File=" + 
			StringUtils::itos(PK_File),&vectPicture_Attribute);

cout << "Found " << (int) vectPicture_Attribute.size() << " pics for attribute" << endl;
		if( vectPicture_Attribute.size() )
		{
			SetPicAttribute(sDirectory,*it,vectPicture_Attribute[0]->FK_Picture_get());
			if( !PK_Picture )
				PK_Picture = vectPicture_Attribute[0]->FK_Picture_get();  // The first pic for this directory
			continue;
		}
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
		int i = ReadDirectory(*it);
		if( !PK_Picture )
			PK_Picture = i;
	}

	// Whatever was the first picture we found will be the one for this directory
	SetPicAttribute(sDirectory,"",PK_Picture);

	return PK_Picture;
}

void UpdateMedia::SetPicAttribute(string sDirectory,string sFile,int PK_Picture)
{
	cout << "SetPicAttribute " << sDirectory << " / " << sFile << " " << PK_Picture  << endl;
#ifndef WIN32
	string sPK_Picture = StringUtils::itos(PK_Picture);
	attr_set( (sDirectory + "/" + sFile).c_str( ), "PIC", sPK_Picture.c_str( ), sPK_Picture.length( ), 0 );
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


int UpdateMedia::AddFileToDatabase(string sDirectory,string sFile)
{
	Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
	pRow_File->Path_set(sDirectory);
	pRow_File->Filename_set(sFile);
	// TODO: Add attributes from ID3 tags
	pRow_File->Table_File_get()->Commit();
cout << "Added " << sDirectory << " / " << sFile << " to db " << pRow_File->PK_File_get() << endl;
	return pRow_File->PK_File_get();
}

