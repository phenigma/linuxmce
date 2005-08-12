/*
 MediaAttributes

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "PlutoUtils/CommonIncludes.h"
#include "MediaAttributes.h"
#include "Logger.h"

#ifndef WIN32
#include <dirent.h>
#include <attr/attributes.h>
#endif

// pluto_media related database access
#include "pluto_main/Table_MediaType.h"

#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_MediaType_AttributeType.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Bookmark.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_PlaylistEntry.h"
#include "pluto_media/Table_Disc.h"
#include "pluto_media/Table_Picture_Disc.h"
#include "pluto_media/Table_Disc_Attribute.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Picture_Attribute.h"

#include "MediaFile.h"
#include "MediaStream.h"

using namespace DCE;

MediaAttributes::MediaAttributes(string host, string user, string pass, string db_name, int port)
{
    m_pDatabase_pluto_media = new Database_pluto_media( );
    if( !m_pDatabase_pluto_media->Connect( host, user, pass, db_name, port  ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        throw "Cannot connect";
    }
}

MediaAttributes::~MediaAttributes()
{
	delete m_pDatabase_pluto_media;
}

void MediaAttributes::TransformFilenameToDeque(string sFilename,deque<MediaFile *> &dequeFilenames)
{
	if( sFilename.length()==0 )
		return;

	if( sFilename[0] != '!' || sFilename.length()<3 )
	{
		dequeFilenames.push_back(new MediaFile(this,sFilename));  // Just a normal file
		return;
	}

	if( sFilename[1] == 'A' || sFilename[1] == 'a' )
	{
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow( atoi(sFilename.substr(2).c_str()) );
		if( !pRow_Attribute )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Attribute lookup on %s is invalid",sFilename.c_str());
			return;
		}

		vector<Row_File_Attribute *> vectRow_File_Attribute;
		pRow_Attribute->File_Attribute_FK_Attribute_getrows(&vectRow_File_Attribute);
		for(size_t s=0;s<vectRow_File_Attribute.size();++s)
		{
			Row_File_Attribute *pRow_File_Attribute = vectRow_File_Attribute[s];
			dequeFilenames.push_back(new MediaFile(pRow_File_Attribute->FK_File_get(),GetFilePathFromFileID(pRow_File_Attribute->FK_File_get())));
		}
	}
	else if( sFilename[1] == 'F' || sFilename[1] == 'f' )
		dequeFilenames.push_back(new MediaFile(atoi(sFilename.substr(2).c_str()),GetFilePathFromFileID(atoi(sFilename.substr(2).c_str()))));
	else if( sFilename[1] == 'B' || sFilename[1] == 'b' )
	{
		Row_Bookmark *pRow_Bookmark = m_pDatabase_pluto_media->Bookmark_get()->GetRow(atoi(sFilename.substr(2).c_str()));
		if( pRow_Bookmark )
		{
			Row_File *pRow_File = pRow_Bookmark->FK_File_getrow();
			if( pRow_File )
			{
				MediaFile *pMediaFile = new MediaFile(pRow_File->PK_File_get(),pRow_File->Path_get() + "/" + pRow_File->Filename_get());
				pMediaFile->m_sStartPosition = pRow_Bookmark->Position_get();
				dequeFilenames.push_back(pMediaFile);
			}
		}
	}
	else
	{
		dequeFilenames.push_back(new MediaFile(this,sFilename));  // Just a normal file
	}
}

void MediaAttributes::ChangeAttribute( int OldPK_AttributeType, int NewPK_AttributeType, string OldName, string NewName )
{
}

void MediaAttributes::ChangeAttribute( int PK_Attribute, string NewName )
{
}

listMediaAttribute *MediaAttributes::AttributesFromString( string Input )
{
    listMediaAttribute *plistMediaAttribute = new listMediaAttribute( );

    string::size_type pos=0;
    while( true )
    {
        MediaAttribute *mi = new MediaAttribute( );
        mi->PK_Attribute = atoi( StringUtils::Tokenize( Input, "|", pos ).c_str( ) );
        mi->PK_AttributeType = atoi( StringUtils::Tokenize( Input, "|", pos ).c_str( ) );
        mi->Name = StringUtils::Tokenize( Input, "|", pos );
        if( !mi->PK_Attribute && !mi->PK_AttributeType )
        {
            delete mi;
            break;
        }
        plistMediaAttribute->push_back( mi );
    }

    return plistMediaAttribute;
}

string MediaAttributes::AttributesToString( listMediaAttribute *plistMediaAttribute )
{
    string Result;
    for( listMediaAttribute::iterator it = plistMediaAttribute->begin( );it != plistMediaAttribute->end( );++it )
    {
        MediaAttribute *mi = ( *it );
        Result += StringUtils::itos( mi->PK_Attribute ) + "|";
        Result += StringUtils::itos( mi->PK_AttributeType ) + "|";
        Result += mi->Name + "|";
    }

    return Result;
}

listMediaPicture *MediaAttributes::PicturesFromString( string Input )
{
    listMediaPicture *plistMediaPicture = new listMediaPicture( );

    string::size_type pos=0;
    while( true )
    {
        MediaPicture *mp = new MediaPicture( );
        mp->Path = StringUtils::Tokenize( Input, "|", pos );
        if( mp->Path.length( )==0 )
        {
            delete mp;
            break;
        }
        mp->Extension = FileUtils::FindExtension( mp->Path );
        plistMediaPicture->push_back( mp );
    }

    return plistMediaPicture;
}

string MediaAttributes::PicturesToString( listMediaPicture *plistMediaPicture )
{
    string Result;
    for( listMediaPicture::iterator it = plistMediaPicture->begin( );it != plistMediaPicture->end( );++it )
    {
        MediaPicture *mp = ( *it );
        Result += mp->Path + "|";
    }

    return Result;
}

string MediaAttributes::GetFilePathFromFileID( int PK_File )
{
    string SQL = "SELECT Path,Filename FROM File WHERE PK_File=" + StringUtils::itos( PK_File );
    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        string strPath = row[0];
        string strFile = row[1];
        return strPath + "/" + strFile;
    }

    return "";
}

int MediaAttributes::GetFileIDFromAttributeID( int PK_Attribute )
{
    string Result;
    string SQL = "SELECT PK_File FROM File "\
        "JOIN File_Attribute ON FK_File=PK_File "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND FK_Attribute=" + StringUtils::itos( PK_Attribute );

    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}

string MediaAttributes::GetFilePathsFromAttributeID( int PK_Atribute )
{
    string Result;
    string SQL = "select DISTINCT Path "\
        "FROM File_Attribute As Source "\
        "JOIN File_Attribute As Dest "\
        "ON Source.FK_File=Dest.FK_File AND Source.FK_Attribute=" + StringUtils::itos( PK_Atribute ) + " "\
        "JOIN File ON Dest.FK_File=PK_File "\
        "ORDER BY Path";

    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) )
    {
        while( ( row=mysql_fetch_row( result.r ) ) )
        {
            Result += row[0];
            Result += "|";
        }
    }
    return Result;
}

int MediaAttributes::GetFileIDFromFilePath( string File )
{
    // moved here since we want to fallback on the windows like functionality when we don't have an attributte for the file.
	PlutoSqlResult result;
    MYSQL_ROW row;

#ifdef WIN32
    string::size_type s;
    while( ( s=File.find( "Z:/" ) )!=string::npos )
        File.replace( s, 3, "/home/public/data/" );
    while( ( s=File.find( "Z:\\" ) )!=string::npos )
        File.replace( s, 3, "/home/public/data/" );
    while( ( s=File.find( '\\' ) )!=string::npos )
        File.replace( s, 1, "/" );

	string Path = FileUtils::BasePath(File);
	if( Path.length() && Path[ Path.length()-1 ]=='/' )
		Path = Path.substr(0,Path.length()-1);
	string SQL = "SELECT PK_File FROM File WHERE Path='" + StringUtils::SQLEscape( Path ) +
		"' AND Filename='" + StringUtils::SQLEscape( FileUtils::FilenameWithoutPath(File) ) + "'";
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        return atoi( row[0] );

    return 0; // Can't do this in Windows
#else
    int n = 79;
    char value[80];
    memset( value, 0, sizeof( value ) );

    if ( attr_get( File.c_str( ), "ID", value, &n, 0 ) == 0 )
    {
        int ID = atoi( value );
        string DatabaseFile = GetFilePathFromFileID( ID );
        if( File!=DatabaseFile )
        {
            if( DatabaseFile.length( )==0 )
            {
                g_pPlutoLogger->Write( LV_CRITICAL, "There appears to be a foreign file in the system %d %s", ID, File.c_str( ) );
                return 0;
            }
            else
            {
                // Confirm that the file doesn't exist in the old location. If it does, there are 2 with the same ID
                FILE *file = fopen( DatabaseFile.c_str( ), "rb" );
                if( file )
                {
                    g_pPlutoLogger->Write( LV_CRITICAL, "There are 2 files with id %d %s and: %s", ID, File.c_str( ), DatabaseFile.c_str( ) );
                    return 0;
                }
                else
                {
                    // They must have moved it
                    g_pPlutoLogger->Write( LV_MEDIA, "File %d moved from %s to %s", ID, DatabaseFile.c_str( ), File.c_str( ) );

                    string path = FileUtils::BasePath( File );
                    string name = FileUtils::FilenameWithoutPath( File, true );

                    string SQL;
                    SQL += "UPDATE File SET Path='" + StringUtils::SQLEscape( path ) + "', Filename='" + StringUtils::SQLEscape( name ) + "' WHERE PK_File=" + StringUtils::itos(ID);
                    // cout << "Query: " << SQL << endl;
                    m_pDatabase_pluto_media->threaded_mysql_query( SQL );
                }
            }
        }
        return ID;
    }
    else
	{	// if no attributte is set then fall back to the windows functionality.
		string Path = FileUtils::BasePath(File);
		if( Path.length() && Path[ Path.length()-1 ]=='/' )
			Path = Path.substr(0,Path.length()-1);

		string SQL = "SELECT PK_File FROM File WHERE Path='" + StringUtils::SQLEscape( Path ) +
			"' AND Filename='" + StringUtils::SQLEscape( FileUtils::FilenameWithoutPath(File) ) + "'";
		if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
			return atoi( row[0] );
		else
			return 0;
	}
#endif
}

string MediaAttributes::GetAnyPictureUnderDirectory( string File, int *PK_Picture, int MaxDepthToSearch )
{
    int FilesToScan=5;
#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;
    string::size_type s;
    while( ( s=File.find( '/' ) )!=string::npos )
        File.replace( s, 1, "\\" );

    ptrFileList = _findfirst( ( File+"\\*.*" ).c_str( ), &finddata );
    while( ptrFileList!=-1 )
    {
        if( MaxDepthToSearch && ( finddata.attrib==_A_SUBDIR ) && finddata.name[0]!='.' )
        {
            string Extension = GetAnyPictureUnderDirectory( File + "\\" + finddata.name, PK_Picture, MaxDepthToSearch-1 );
            if( *PK_Picture )
                return Extension;
        }
        else if ( finddata.name[0] != '.' )
        {
            FilesToScan--;
            string Extension = GetPictureFromFilePath( File + "\\" + finddata.name, PK_Picture );
            if( *PK_Picture )
                return Extension;
        }
        if(     !FilesToScan )
        {
            *PK_Picture=0;
            return "";
        }
        if( _findnext( ptrFileList, &finddata )<0 )
            break;
    }

    _findclose( ptrFileList );

#else
    DIR *dirp = opendir( File.c_str( ) );
    struct dirent entry;
    struct dirent *direntp = &entry;
    if ( dirp == NULL )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "opendir2 %s failed: %s", File.c_str( ), strerror( errno ) );
        return "";
    }
    while ( dirp != NULL && ( readdir_r( dirp, direntp, &direntp ) == 0 ) && direntp )
    {
#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "GetPicture: Reading %s %s depth %d",File.c_str(),entry.d_name,MaxDepthToSearch);
#endif
        if( MaxDepthToSearch && ( entry.d_type==DT_DIR ) && entry.d_name[0]!='.' )
        {
            string Extension = GetAnyPictureUnderDirectory( File + "/" + entry.d_name, PK_Picture, MaxDepthToSearch-1 );
            if( *PK_Picture )
            {
                closedir( dirp );
                return Extension;
            }
        }
        else if ( entry.d_name[0] != '.' )
        {
            FilesToScan--;
            string Extension = GetPictureFromFilePath( File + "/" + entry.d_name, PK_Picture );

#ifdef DEBUG
g_pPlutoLogger->Write( LV_STATUS, "GetPicture: Reading %s %s got pic %d %s",File.c_str(),entry.d_name,*PK_Picture,Extension.c_str());
#endif
            if( *PK_Picture )
            {
                closedir( dirp );
                return Extension;
			}
        }
        if ( ! FilesToScan )
        {
            closedir( dirp );
            *PK_Picture=0;
            return "";
        }
    }
    closedir( dirp );
#endif
return "";
}

string MediaAttributes::GetPictureFromFilePath( string File, int *PK_Picture )
{
    int PK_File = GetFileIDFromFilePath( File );

// g_pPlutoLogger->Write(LV_STATUS, "Got file id: %d", PK_File);

    if( PK_File )
        return GetPictureFromFileID( PK_File, PK_Picture );

	*PK_Picture=0;
    return "";
}

string MediaAttributes::GetPictureFromFileID( int PK_File, int *PK_Picture )
{
    string SQL = "select PK_Picture, Extension FROM Picture_File JOIN Picture ON "\
        "FK_Picture=PK_Picture "\
        "WHERE FK_File=" + StringUtils::itos( PK_File );

//	g_pPlutoLogger->Write(LV_STATUS, "MediaAttributes::GetPictureFromFileID() Running query: %s", SQL.c_str());
    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    // Let's to see if any of the attributes associated with this file have pictures
    SQL = "SELECT FK_Picture, Extension from File_Attribute "\
        "JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "\
        "JOIN Picture ON FK_Picture=PK_Picture "\
        "WHERE FK_File=" + StringUtils::itos( PK_File );

//	g_pPlutoLogger->Write(LV_STATUS, "MediaAttributes::GetPictureFromFileID() Running another query: %s", SQL.c_str());
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    *PK_Picture=0;
    return "";
}

string MediaAttributes::GetPictureFromAttributeID( int PK_Attribute, int *PK_Picture )
{
    string SQL = "select PK_Picture, Extension FROM Picture_Attribute JOIN Picture ON "\
        "FK_Picture=PK_Picture "\
        "WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute ) + " limit 1";
    PlutoSqlResult result,result2;
    MYSQL_ROW row;
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

	// Let's to see if any of the attributes associated with this file have pictures
    SQL = "select DISTINCT FK_Picture, Extension "\
        "FROM File_Attribute AS Source "\
        "JOIN File_Attribute As Dest ON Source.FK_File=Dest.FK_File "\
        "JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=Dest.FK_Attribute "\
        "JOIN Picture ON FK_Picture=PK_Picture "\
        "WHERE Source.FK_Attribute=" + StringUtils::itos( PK_Attribute ) + " limit 1";

    if( ( result2.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result2.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    *PK_Picture=0;
    return "";
}
int MediaAttributes::GetAttributeFromFileID( int PK_File )
{
    string SQL = "select PK_Attribute FROM File_Attribute "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN File ON FK_File=PK_File "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND PK_File=" + StringUtils::itos( PK_File );

    PlutoSqlResult result;
    MYSQL_ROW row;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}
int MediaAttributes::GetAttributeFromFilePath( string File )
{
    string SQL = "select PK_Attribute FROM File_Attribute "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN File ON FK_File=PK_File "\
        "JOIN MediaType_AttributeType ON File.EK_MediaType=MediaType_AttributeType.EK_MediaType "\
        "AND MediaType_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND Path='" + StringUtils::SQLEscape( File ) + "'";

    PlutoSqlResult result;
    MYSQL_ROW row;

    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}

bool MediaAttributes::SavePlaylist(deque<MediaFile *> &dequeMediaFile, int iPK_Users, int &iPK_Playlist, string sPlaylistName )
{
	Row_Playlist *pRow_Playlist=NULL;
	if( iPK_Playlist )
		pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(iPK_Playlist);
	else
		pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->AddRow();

	if( !pRow_Playlist )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Tried to save to an invalid playlist");
		return false;
	}

    pRow_Playlist->Name_set(sPlaylistName);
    pRow_Playlist->EK_User_set(iPK_Users);

g_pPlutoLogger->Write(LV_WARNING, "pl3 = %s %s",sPlaylistName.c_str(),pRow_Playlist->Name_get().c_str());

    if ( !pRow_Playlist->Table_Playlist_get()->Commit(true,true) )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"Cannot save/create playlist with ID %d error: %s",iPK_Playlist,
	        m_pDatabase_pluto_media->m_sLastMySqlError.c_str());

		return false;
    }

    if( iPK_Playlist )
	{
		// We'll just re-write the whole thing out again anyway
	    vector<Row_PlaylistEntry*> vectRow_PlaylistEntry;
		pRow_Playlist->PlaylistEntry_FK_Playlist_getrows(&vectRow_PlaylistEntry);
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Deleting %d rows from old playlist",(int) vectRow_PlaylistEntry.size());
#endif
		for(size_t s=0;s<vectRow_PlaylistEntry.size();++s)
			vectRow_PlaylistEntry[s]->Delete();
	}

    iPK_Playlist = pRow_Playlist->PK_Playlist_get();
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS,"Save playlist id %d with %d rows",iPK_Playlist,(int) dequeMediaFile.size());
#endif

	for(size_t s=0;s<dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = dequeMediaFile[s];
		Row_PlaylistEntry *pRow_PlaylistEntry = m_pDatabase_pluto_media->PlaylistEntry_get()->AddRow();
		pRow_PlaylistEntry->FK_Playlist_set( iPK_Playlist );
		if( pMediaFile->m_dwPK_File )
			pRow_PlaylistEntry->FK_File_set(pMediaFile->m_dwPK_File);
		pRow_PlaylistEntry->Path_set(pMediaFile->m_sPath);
		pRow_PlaylistEntry->Filename_set(pMediaFile->m_sFilename);
	}

    if( !m_pDatabase_pluto_media->PlaylistEntry_get()->Commit(true,true) )
    {
		g_pPlutoLogger->Write(LV_CRITICAL, "Could not save the playlist error: %s",m_pDatabase_pluto_media->m_sLastMySqlError.c_str());
        return false;
    }

    return true;
}

int MediaAttributes::LoadPlaylist(int iPK_Playlist, deque<MediaFile *> &dequeMediaFile, string &sPlaylistName)
{
    Row_Playlist *pRow_Playlist = m_pDatabase_pluto_media->Playlist_get()->GetRow(iPK_Playlist);
    if ( !pRow_Playlist )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find playlist: %d",iPK_Playlist);
		return false;
	}

    PurgeDequeMediaFile(dequeMediaFile);
    vector<Row_PlaylistEntry *> vectRow_PlaylistEntry;
	pRow_Playlist->PlaylistEntry_FK_Playlist_getrows(&vectRow_PlaylistEntry);
    for( size_t s=0;s<vectRow_PlaylistEntry.size();++s )
		dequeMediaFile.push_back(new MediaFile(vectRow_PlaylistEntry[s]));

    return (int) dequeMediaFile.size();
}

void MediaAttributes::PurgeDequeMediaFile(deque<MediaFile *> &dequeMediaFile)
{
	for(size_t s=0;s<dequeMediaFile.size();++s)
		delete dequeMediaFile[s];
	dequeMediaFile.clear();
}

void MediaAttributes::MarkAsMissing(int iKey, string fileName)
{
    g_pPlutoLogger->Write(LV_STATUS, "Marking %s as missing", fileName.c_str());

    string SQL = "UPDATE File SET Missing=1 WHERE PK_File=" + StringUtils::itos( iKey );

    m_pDatabase_pluto_media->threaded_mysql_query_withID( SQL );
}

void operator+= (deque<MediaFile *> &dTarget, deque<MediaFile *> &dAdditional)
{
    for(size_t s=0;s<dAdditional.size();++s)
        dTarget.push_back(dAdditional[s]);
}

Row_Attribute *MediaAttributes::GetAttributeFromDescription(int PK_MediaType,int PK_AttributeType,string sName)
{
	vector<Row_Attribute *> vectRow_Attribute;
	Row_MediaType_AttributeType *pMediaType_AttributeType = NULL;
	if( PK_MediaType==MEDIATYPE_pluto_CD_CONST )
		pMediaType_AttributeType = m_pDatabase_pluto_media->MediaType_AttributeType_get()->GetRow(MEDIATYPE_pluto_StoredAudio_CONST,PK_AttributeType);
	else
		pMediaType_AttributeType = m_pDatabase_pluto_media->MediaType_AttributeType_get()->GetRow(PK_MediaType,PK_AttributeType);
	if( pMediaType_AttributeType && pMediaType_AttributeType->CombineAsOne_get()==1 )
	{
		string::size_type posTab;
		string sWhere = "FK_AttributeType=" + StringUtils::itos(PK_AttributeType) + " AND Name='" + StringUtils::SQLEscape(sName) + "'";
		m_pDatabase_pluto_media->Attribute_get()->GetRows(sWhere,&vectRow_Attribute);
	}

	if( vectRow_Attribute.size() )
		return vectRow_Attribute[0];
	else
	{
		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->AddRow();
		pRow_Attribute->FK_AttributeType_set(PK_AttributeType);
		pRow_Attribute->Name_set(sName);
		pRow_Attribute->Table_Attribute_get()->Commit();
		UpdateSearchTokens(pRow_Attribute);
		return pRow_Attribute;
	}
}

int MediaAttributes::AddIdentifiedDiscToDB(string sIdentifiedDisc,MediaStream *pMediaStream)
{
	Row_Disc *pRow_Disc;
	
	vector<Row_Disc *> vectRow_Disc;
	m_pDatabase_pluto_media->Disc_get()->GetRows("ID='" + StringUtils::SQLEscape(sIdentifiedDisc) + "'",&vectRow_Disc);
	if( vectRow_Disc.size() )
		pRow_Disc = vectRow_Disc[0];
	else
	{
		pRow_Disc = m_pDatabase_pluto_media->Disc_get()->AddRow();
		pRow_Disc->ID_set(sIdentifiedDisc);
		pRow_Disc->EK_MediaType_set(pMediaStream->m_iPK_MediaType);
		pRow_Disc->Table_Disc_get()->Commit();
	}
	pMediaStream->m_dwPK_Disc = pRow_Disc->PK_Disc_get();

	AddAttributesToDisc(pRow_Disc,0,0,&pMediaStream->m_mapPK_Attribute);

	if( pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST )
	{
		for(size_t sFile=0;sFile<pMediaStream->m_dequeMediaFile.size();sFile++)
		{
			MediaFile *pMediaFile=pMediaStream->m_dequeMediaFile[sFile];
			AddAttributesToDisc(pRow_Disc,sFile+1,0,&pMediaFile->m_mapPK_Attribute);
			for(size_t sSection=0;sSection<pMediaFile->m_dequeMediaSection.size();sSection++)
				AddAttributesToDisc(pRow_Disc,sFile+1,sSection+1,&pMediaFile->m_dequeMediaSection[sSection]->m_mapPK_Attribute);
		}
	}

	for(size_t sSection=0;sSection<pMediaStream->m_dequeMediaSection.size();sSection++)
		AddAttributesToDisc(pRow_Disc,0,sSection,&pMediaStream->m_dequeMediaSection[sSection]->m_mapPK_Attribute);

	for(size_t sTitle=0;sTitle<pMediaStream->m_dequeMediaTitle.size();sTitle++)
	{
		MediaTitle *pMediaTitle=pMediaStream->m_dequeMediaTitle[sTitle];
		AddAttributesToDisc(pRow_Disc,sTitle+1,0,&pMediaTitle->m_mapPK_Attribute);
		for(size_t sSection=0;sSection<pMediaTitle->m_dequeMediaSection.size();sSection++)
			AddAttributesToDisc(pRow_Disc,sTitle+1,sSection+1,&pMediaTitle->m_dequeMediaSection[sSection]->m_mapPK_Attribute);
	}

	if( pMediaStream->m_pPictureData && pMediaStream->m_iPictureSize )
	{
		Row_Picture *pRow_Picture = AddPicture(pMediaStream->m_pPictureData, pMediaStream->m_iPictureSize, "jpg");

		Row_Picture_Disc *pRow_Picture_Disc = m_pDatabase_pluto_media->Picture_Disc_get()->AddRow();
		pRow_Picture_Disc->FK_Disc_set( pRow_Disc->PK_Disc_get() );
		pRow_Picture_Disc->FK_Picture_set( pRow_Picture->PK_Picture_get() );
		m_pDatabase_pluto_media->Picture_Disc_get()->Commit();

		// Find all attributes that we added that identify this disc and need the same picture
		string sWhere="SELECT Attribute.* FROM Attribute"
			" JOIN Disc_Attribute ON FK_Attribute=PK_Attribute"
			" JOIN Disc ON FK_Disc=PK_Disc"
			" JOIN MediaType_AttributeType ON Attribute.FK_AttributeType=MediaType_AttributeType.FK_AttributeType"
			" AND MediaType_AttributeType.EK_MediaType=" + StringUtils::itos(pMediaStream->m_iPK_MediaType==MEDIATYPE_pluto_CD_CONST ? MEDIATYPE_pluto_StoredAudio_CONST : pMediaStream->m_iPK_MediaType) +
			" WHERE PK_Disc=" + StringUtils::itos(pRow_Disc->PK_Disc_get()) + " AND Identifier=1";
		vector<Row_Attribute *> vectRow_Attribute;
		m_pDatabase_pluto_media->Attribute_get()->GetRows(sWhere,&vectRow_Attribute);
		for(size_t s=0;s<vectRow_Attribute.size();++s)
		{
			Row_Attribute *pRow_Attribute = vectRow_Attribute[s];
			Row_Picture_Attribute *pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->GetRow(pRow_Picture->PK_Picture_get(),pRow_Attribute->PK_Attribute_get());
			if( !pRow_Picture_Attribute )
			{
				pRow_Picture_Attribute = m_pDatabase_pluto_media->Picture_Attribute_get()->AddRow();
				pRow_Picture_Attribute->FK_Picture_set(pRow_Picture->PK_Picture_get());
				pRow_Picture_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
				m_pDatabase_pluto_media->Picture_Attribute_get()->Commit();
			}
		}
	}
	return pRow_Disc->PK_Disc_get();
}

void MediaAttributes::AddAttributesToDisc(Row_Disc *pRow_Disc,int iFileOrTrack,int iSection,map<int,int> *p_mapPK_Attribute)
{
	for( map<int,int>::iterator it=p_mapPK_Attribute->begin();it!=p_mapPK_Attribute->end();++it)
	{
		if( it->first==0 )
			continue;  // Don't store the recognition type

		Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow(it->second);
		if( !pRow_Attribute )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"MediaAttributes::AddAttributesToDisc now can't find attribute %d",it->second);
			continue;
		}
		Row_Disc_Attribute *pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->GetRow(pRow_Disc->PK_Disc_get(),pRow_Attribute->PK_Attribute_get(),iFileOrTrack,iSection);
		if( !pRow_Disc_Attribute )
		{
			pRow_Disc_Attribute = m_pDatabase_pluto_media->Disc_Attribute_get()->AddRow();
			pRow_Disc_Attribute->FK_Disc_set(pRow_Disc->PK_Disc_get());
			pRow_Disc_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get()); 
			pRow_Disc_Attribute->Track_set(iFileOrTrack);
			pRow_Disc_Attribute->Section_set(iSection);
			pRow_Disc_Attribute->Table_Disc_Attribute_get()->Commit();
		}
	}
}

bool MediaAttributes::IsDiscAlreadyIdentified(string sIdentifiedDisc,MediaStream *pMediaStream)
{
	vector<Row_Disc *> vectRow_Disc;
	m_pDatabase_pluto_media->Disc_get()->GetRows("ID='" + StringUtils::SQLEscape(sIdentifiedDisc) + "'",&vectRow_Disc);
	if( vectRow_Disc.size()==0 )
		return false;

	Row_Disc *pRow_Disc = vectRow_Disc[0];
	pMediaStream->m_dwPK_Disc = pRow_Disc->PK_Disc_get();

	vector<Row_Disc_Attribute *> vectRow_Disc_Attribute;
	pRow_Disc->Disc_Attribute_FK_Disc_getrows(&vectRow_Disc_Attribute);

	if( vectRow_Disc_Attribute.size()<2 )
		return false; // If it was identified successfully, we'd have at least 2 entries.  It could have been a failed identify before, so we'll retry
	for(size_t s=0;s<vectRow_Disc_Attribute.size();++s)
	{
		Row_Disc_Attribute *pRow_Disc_Attribute = vectRow_Disc_Attribute[s];
		Row_Attribute *pRow_Attribute = pRow_Disc_Attribute->FK_Attribute_getrow();
g_pPlutoLogger->Write(LV_STATUS,"Already in DB %p %p %d",pRow_Disc_Attribute,pRow_Attribute,(pRow_Attribute ? pRow_Attribute->PK_Attribute_get() : 0));
		if( !pRow_Attribute )
			continue; // Should never happen
		if( pRow_Disc->EK_MediaType_get()==MEDIATYPE_pluto_CD_CONST ) // Tracks are treated as files
			AddAttributeToStream(pMediaStream,pRow_Attribute,pRow_Disc_Attribute->Track_get(),0,pRow_Disc_Attribute->Section_get());
		else
			AddAttributeToStream(pMediaStream,pRow_Attribute,0,pRow_Disc_Attribute->Track_get(),pRow_Disc_Attribute->Section_get());
	}
	return true;
}

void MediaAttributes::AddAttributeToStream(MediaStream *pMediaStream,Row_Attribute *pRow_Attribute,int File,int Title_Track,int Section)
{
	int PK_AttributeType=pRow_Attribute->FK_AttributeType_get();
	// For CD's, the tracks are represented as files
g_pPlutoLogger->Write(LV_STATUS,"MediaAttributes::AddAttributeToStream %p %d",pRow_Attribute,(pRow_Attribute ? pRow_Attribute->PK_Attribute_get() : 0));
	if( File )
	{
		if( File>0 && File<=pMediaStream->m_dequeMediaFile.size() )
		{
			MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[File-1];
			if( Title_Track )
			{
				while( Title_Track>pMediaFile->m_dequeMediaTitle.size() )
					pMediaFile->m_dequeMediaTitle.push_back(new MediaTitle());
				MediaTitle *pMediaTitle=pMediaFile->m_dequeMediaTitle[Title_Track-1];
				if( Section==0 )
					pMediaTitle->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
				else
				{
					MediaSection *pMediaSection=GetMediaSection(&pMediaTitle->m_dequeMediaSection,Section);
					pMediaSection->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
				}
			}
			else if( Section==0 )
				pMediaFile->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
			else
				GetMediaSection(&pMediaFile->m_dequeMediaSection,Section)->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
		}
	}
	else if( Title_Track ) // For DVD's Track is the title
	{
		while( Title_Track>pMediaStream->m_dequeMediaTitle.size() )
			pMediaStream->m_dequeMediaTitle.push_back(new MediaTitle());
		MediaTitle *pMediaTitle=pMediaStream->m_dequeMediaTitle[Title_Track-1];
		if( Section==0 )
			pMediaTitle->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
		else
		{
			MediaSection *pMediaSection=GetMediaSection(&pMediaTitle->m_dequeMediaSection,Section);
			pMediaSection->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
		}
	}
	else if( Section==0 )
		pMediaStream->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
	else
		GetMediaSection(&pMediaStream->m_dequeMediaSection,Section)->m_mapPK_Attribute[PK_AttributeType] = pRow_Attribute->PK_Attribute_get();
}

void MediaAttributes::LoadStreamAttributes(MediaStream *pMediaStream)
{
	for(size_t s=0;s<pMediaStream->m_dequeMediaFile.size();++s)
	{
		MediaFile *pMediaFile = pMediaStream->m_dequeMediaFile[s];
		if( !pMediaFile->m_dwPK_File )
			continue;

		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(pMediaFile->m_dwPK_File);
		if( !pRow_File )
			continue;

		vector<Row_File_Attribute *> vectRow_File_Attribute;
		pRow_File->File_Attribute_FK_File_getrows(&vectRow_File_Attribute);
		for(size_t sA=0;sA<vectRow_File_Attribute.size();++sA)
		{
			Row_File_Attribute *pRow_File_Attribute=vectRow_File_Attribute[sA];
			AddAttributeToStream(pMediaStream,pRow_File_Attribute->FK_Attribute_getrow(),s+1,pRow_File_Attribute->Track_get(),pRow_File_Attribute->Section_get());
		}
	}
}

Row_Picture * MediaAttributes::AddPicture(char *pData,int iData_Size,string sFormat)
{
	sFormat = StringUtils::ToLower(sFormat);
	Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->AddRow();
	pRow_Picture->Extension_set(sFormat);
	m_pDatabase_pluto_media->Picture_get()->Commit();

	FILE *file = fopen( ("/home/mediapics/" + StringUtils::itos(pRow_Picture->PK_Picture_get()) + "." + sFormat).c_str(),"wb");
	if( !file )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot create bookmark pic file");
		pRow_Picture->Delete();
		m_pDatabase_pluto_media->Picture_get()->Commit();
		pRow_Picture=NULL;
	}
	else
	{
		fwrite((void *) pData,iData_Size,1,file);
		fclose(file);
		string Cmd = "convert -sample 75x75 /home/mediapics/" + StringUtils::itos( pRow_Picture->PK_Picture_get() ) + "." + sFormat +
			" /home/mediapics/" + StringUtils::itos( pRow_Picture->PK_Picture_get() ) + "_tn." + sFormat;
		int result;
		if( ( result=system( Cmd.c_str( ) ) )!=0 )
			g_pPlutoLogger->Write( LV_CRITICAL, "Thumbnail picture %s returned %d", Cmd.c_str( ), result );
	}

	return pRow_Picture;
}

void MediaAttributes::UpdateSearchTokens(Row_Attribute *pRow_Attribute)
{
	int PK_Attribute = pRow_Attribute->PK_Attribute_get();
    PlutoSqlResult result;
    MYSQL_ROW row, row2;
    string SQL = "SELECT Name FROM Attribute WHERE PK_Attribute=" + StringUtils::itos( PK_Attribute );
    if( ( result.r=m_pDatabase_pluto_media->mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        SQL = "DELETE FROM SearchToken_Attribute WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute );
        m_pDatabase_pluto_media->threaded_mysql_query( SQL );
        char *ptr = row[0];
        string::size_type pos=0;
        while( ptr && *ptr )
        {
            string s( ptr );
            string Token=StringUtils::UpperAZ09Only( StringUtils::Tokenize( s, " ", pos ) );
            if( Token.length( )==0 )
                break;
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
                    StringUtils::itos( PK_Attribute ) + " )";
                m_pDatabase_pluto_media->threaded_mysql_query( SQL, true );
            }
        }
    }
    else
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot update tokens for %d", PK_Attribute );
}
