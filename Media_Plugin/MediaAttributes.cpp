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
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Playlist.h"
#include "pluto_media/Table_PlaylistEntry.h"

MediaAttributes::MediaAttributes(string host, string user, string pass, string db_name, int port)
    : MySqlHelper(host, user, pass, db_name, port)
{
    m_pDatabase_pluto_media = new Database_pluto_media( );
    if( !m_pDatabase_pluto_media->Connect( host, user, pass, db_name, port  ) )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot connect to database!" );
        throw "Cannot connect";
    }
}

int MediaAttributes::CreatedMedia( int PK_Type, string FilePath, listMediaAttribute *plistMediaAttribute, listMediaPicture *plistMediaPicture )
{
    PlutoSqlResult result;
    MYSQL_ROW row;
    int PK_File =0;

    string Path = FileUtils::BasePath(FilePath);
    string Name = FileUtils::FilenameWithoutPath(FilePath);

    string SQL = "SELECT PK_File FROM File WHERE Path='" + StringUtils::SQLEscape( Path ) + "' AND Filename='" + StringUtils::SQLEscape( Name ) + "'";
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        PK_File = atoi( row[0] );
    else
    {
        SQL = "INSERT INTO File( FK_Type, Path, Filename ) VALUES ( " + StringUtils::itos( PK_Type ) + ", '" + StringUtils::SQLEscape( Path ) + "', '" + StringUtils::SQLEscape( Name ) + "' )";
        PK_File = threaded_mysql_query_withID( SQL );
    }
    if( !PK_File )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "failed: %s", SQL.c_str( ) );
    }
    else
    {
        UpdatedMedia( PK_File, 0, FilePath, plistMediaAttribute, plistMediaPicture );
#ifndef WIN32
        string value = StringUtils::itos( PK_File );
        if ( attr_set( FilePath.c_str( ), "ID", value.c_str( ), value.length( ), 0 ) != 0 )
            g_pPlutoLogger->Write(LV_STATUS, "Error setting attribute for file: %s", FilePath.c_str());
#endif
    }

    printf( "Created Media %s returning %d\n", FilePath.c_str( ), PK_File );

    return PK_File;
}

void MediaAttributes::UpdatedMedia( int PK_File, int PK_Type, string FilePath, listMediaAttribute *plistMediaAttribute, listMediaPicture *plistMediaPicture )
{
    if( PK_Type )
    {
        string SQL = "UPDATE File SET FK_Type=" + StringUtils::itos( PK_Type ) +
            " WHERE PK_File=" + StringUtils::itos( PK_File );

        threaded_mysql_query_withID( SQL );
    }

    string Path = FileUtils::BasePath(FilePath);
    string Name = FileUtils::FilenameWithoutPath(FilePath);

    if( Path.length( )!=0 )
    {
        string SQL = "UPDATE File SET Path='" + StringUtils::SQLEscape( Path ) + "', Filename='" + StringUtils::SQLEscape( Name ) + "', Missing = 0 " +
            " WHERE PK_File=" + StringUtils::itos( PK_File );

        cout << "Query: " << SQL << endl;
        threaded_mysql_query_withID( SQL );
    }

    if( plistMediaAttribute )
    {
        for( listMediaAttribute::iterator it=plistMediaAttribute->begin( );it!=plistMediaAttribute->end( );++it )
        {
            MediaAttribute *ma = ( *it );
            AddAttribute( PK_File, ma->PK_Attribute, ma->PK_AttributeType, ma->Name, ma->FirstName );
        }
    }
    if( plistMediaPicture )
    {
        for( listMediaPicture::iterator it=plistMediaPicture->begin( );it!=plistMediaPicture->end( );++it )
        {
            MediaPicture *mp = ( *it );
            if( mp->PK_Picture )
                SetPicture( mp->PK_Picture, mp->Path );
            else
                AddPicture( mp->PK_File, mp->PK_Attribute, mp->Path );
        }
    }
}

int MediaAttributes::AddAttribute( int PK_File, int PK_Attribute, int PK_AttributeType, string Name, string FirstName )
{
    printf( "Add attribute file %d att %d Name %s \n", PK_File, PK_Attribute, Name.c_str( ) );

    if( ( Name.length( )==0 || Name=="( null )" ) && ( FirstName.length( )==0 || FirstName=="( null )" ) )
        return 0;
    PlutoSqlResult result;
    MYSQL_ROW row;
    if( !PK_Attribute )
    {
        bool bCombineAsOne=false;
        string SQL = "SELECT CombineAsOne FROM File "\
            "JOIN Type_AttributeType ON File.FK_Type=Type_AttributeType.FK_Type "\
            "WHERE FK_AttributeType=" + StringUtils::itos( PK_AttributeType ) + " AND PK_File=" + StringUtils::itos( PK_File );
        {
        }
        if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        {
            if( atoi( row[0] )==1 )
                bCombineAsOne=true;
        }


        if( bCombineAsOne )
        {
            SQL = "SELECT PK_Attribute FROM Attribute WHERE FK_AttributeType=" +
                StringUtils::itos( PK_AttributeType ) + " AND Name='" +
                StringUtils::SQLEscape( Name ) + "' AND FirstName='" + StringUtils::SQLEscape( FirstName ) + "'";

            if(     ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
            PK_Attribute = atoi( row[0] );
        }

        if( !PK_Attribute )
        {
            SQL = "INSERT INTO Attribute( FK_AttributeType, Name, FirstName ) VALUES( " +
                StringUtils::itos( PK_AttributeType ) + ", '" + StringUtils::SQLEscape( Name ) + "', '" +
                StringUtils::SQLEscape( FirstName ) + "' )";
            PK_Attribute = threaded_mysql_query_withID( SQL );
        }
    }
    if( PK_Attribute ) // For sure we should have it now
    {
        string SQL = "INSERT INTO File_Attribute VALUES( " + StringUtils::itos( PK_File ) + ", " +
            StringUtils::itos( PK_Attribute ) + " )";
        threaded_mysql_query( SQL, true ); // If it fails, it just means the association is already there
        UpdateSearchTokens( PK_Attribute );
    }
    return PK_Attribute;
}

void MediaAttributes::ChangeAttribute( int OldPK_AttributeType, int NewPK_AttributeType, string OldName, string OldFirstName, string NewName, string NewFirstName )
{
}

void MediaAttributes::ChangeAttribute( int PK_Attribute, string NewName, string NewFirstName )
{
}

void MediaAttributes::AddPicture( int PK_File, int PK_Attribute, string Path )
{
    // confirm the file exists
    FILE *file = fopen( Path.c_str( ), "rb" );
    if( !file )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Picture file %s doesn't exist", Path.c_str( ) );
        return;
    }
  fseek( file, 0L, SEEK_END );
  size_t file_size=ftell( file );
  fseek( file, 0L, SEEK_SET );
    char *data_block=( char * )malloc( file_size+1 );
  size_t bytesread=fread( data_block, 1, file_size, file );
    fclose( file );
    if( bytesread!=file_size )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Picture file %s can't be read", Path.c_str( ) );
        return;
    }

    PlutoSqlResult result;
    MYSQL_ROW row;

    int PK_Picture=0;

    string SQL;
    for( int loop=0;loop<=1;++loop )
    {
        if( loop==0 )
        {
            if( !PK_File )
                continue;
            else
                SQL = "SELECT FK_Picture, Extension FROM Picture_File JOIN Picture ON FK_Picture=PK_Picture WHERE FK_File=" + StringUtils::itos( PK_File );
        }
        else
        {
            if( !PK_Attribute )
                continue;
            else
                SQL = "SELECT FK_Picture, Extension FROM Picture_Attribute JOIN Picture ON FK_Picture=PK_Picture WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute );
        }
        if( ( result.r=mysql_query_result( SQL ) ) )
        {
            while( ( row=mysql_fetch_row( result.r ) ) )
            {
                int PKID = atoi( row[0] );
                char *Extension = row[1];
#ifdef WIN32
                file = fopen( ( "\\home\\mediapics\\" + StringUtils::itos( PKID ) + "." + Extension ).c_str( ), "rb" );
#else
                file = fopen( ( "/home/mediapics/" + StringUtils::itos( PKID ) + "." + Extension ).c_str( ), "rb" );
#endif
                if( !file ) // Can't find picture??
                {
                    SQL = "DELETE FROM Picture_File WHERE FK_Picture=" + StringUtils::itos( PKID );
                    threaded_mysql_query( SQL );
                    SQL = "DELETE FROM Picture_Attribute WHERE FK_Picture=" + StringUtils::itos( PKID );
                    threaded_mysql_query( SQL );
                    SQL = "DELETE FROM Picture WHERE PK_Picture=" + StringUtils::itos( PKID );
                    threaded_mysql_query( SQL );
                    continue;
                }
                fseek( file, 0L, SEEK_END );
                size_t file_size2=ftell( file );
                if( file_size2!=file_size )
                {
                    fclose( file );
                    continue;
                }

                // They're the same size. compare them
                fseek( file, 0L, SEEK_SET );
                char *data_block2=( char * )malloc( file_size+1 );
                fread( data_block2, 1, file_size, file );
                fclose( file );

                if( memcmp( data_block, data_block2, file_size )==0 )
                {
                    // This is the same file
                    PK_Picture = PKID;
                    delete[] data_block2;
                    break;
                }
                delete[] data_block2;
            }
        }
    }

    if( !PK_Picture )
    {
        string Extension = FileUtils::FindExtension( Path );
        string SQL = "INSERT INTO Picture( Extension ) VALUES( '" + Extension + "' )";
        PK_Picture = threaded_mysql_query_withID( SQL );
        FILE *file;
#ifdef WIN32
        file = fopen( ( "\\home\\mediapics\\" + StringUtils::itos( PK_Picture ) + "." + Extension ).c_str( ), "wb" );
#else
        file = fopen( ( "/home/mediapics/" + StringUtils::itos( PK_Picture ) + "." + Extension ).c_str( ), "wb" );
#endif
        if( !file )
            g_pPlutoLogger->Write( LV_CRITICAL, "Cannot write out pic: %d", PK_Picture );
        else
        {
            fwrite( data_block, 1, file_size, file );
            fclose( file );
        }
    }
    delete[] data_block;

    if( PK_Picture ) // For sure we should have it now
    {
#ifdef WIN32
        if( SetPicture( PK_Picture, Path ) || true ) // This will fail to do the compression
#else
        if( SetPicture( PK_Picture, Path )==0 )
#endif
        {
            if( PK_File )
            {
                SQL = "INSERT INTO Picture_File VALUES( " + StringUtils::itos( PK_Picture ) + ", " +
                    StringUtils::itos( PK_File ) + " )";
                threaded_mysql_query( SQL, true );
            }
            if( PK_Attribute )
            {
                SQL = "INSERT INTO Picture_Attribute VALUES( " + StringUtils::itos( PK_Picture ) + ", " +
                    StringUtils::itos( PK_Attribute ) + " )";
                threaded_mysql_query( SQL, true );
            }
        }
    }
}

int MediaAttributes::SetPicture( int PK_Picture, string Path )
{
    string Extension = FileUtils::FindExtension( Path );
    int result;

    // Make compressed copies
    string Cmd = "/usr/local/pluto/bin/convert -sample 100x100 /home/mediapics/" + StringUtils::itos( PK_Picture ) + "." + Extension +
        " /home/mediapics/" + StringUtils::itos( PK_Picture ) + "_tn." + Extension;
    if( ( result=system( Cmd.c_str( ) ) )!=0 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Aborting thumbnail picture %s returned %d", Cmd.c_str( ), result );
        return -1;
    }
/*
    Cmd = "/usr/local/pluto/bin/convert -sample 300x300 /home/mediapics/" + StringUtils::itos( PK_Picture ) + "." + Extension +
        " /home/mediapics/" + StringUtils::itos( PK_Picture ) + "_s." + Extension;
    if( ( result=system( Cmd.c_str( ) ) )!=0 )
    {
        g_pPlutoLogger->Write( LV_CRITICAL, "Aborting small picture %s returned %d", Cmd.c_str( ), result );
        return -1;
    }
*/
    return 0;
}

void MediaAttributes::UpdateSearchTokens( int PK_Attribute )
{
    PlutoSqlResult result;
    MYSQL_ROW row, row2;
    string SQL = "SELECT Name, FirstName FROM Attribute WHERE PK_Attribute=" + StringUtils::itos( PK_Attribute );
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        SQL = "DELETE FROM SearchToken_Attribute WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute );
        threaded_mysql_query( SQL );
        for( int i=0;i<=1;++i )
        {
            char *ptr = row[i];
            string::size_type pos=0;
            while( ptr && *ptr )
            {
                string s( ptr );
                string Token=StringUtils::ToUpper( StringUtils::Tokenize( s, " ", pos ) );
                if( Token.length( )==0 )
                    break;
                SQL = "SELECT PK_SearchToken FROM SearchToken WHERE Token='" +
                    StringUtils::SQLEscape( Token ) + "'";
                int PK_SearchToken=0;
                if( ( result.r=mysql_query_result( SQL ) ) && ( row2=mysql_fetch_row( result.r ) ) )
                {
                    PK_SearchToken = atoi( row2[0] );
                    printf( "Found token ( %d ): %s\n", PK_SearchToken, Token.c_str( ) );
                }
                else
                {
                    printf( "Didn't find token: %d %s ( %s )\n", PK_SearchToken, Token.c_str( ), SQL.c_str( ) );
                    SQL = "INSERT INTO SearchToken( Token ) VALUES( '" +
                        StringUtils::SQLEscape( Token ) + "' )";
                    PK_SearchToken = threaded_mysql_query_withID( SQL );
                }
                if( PK_SearchToken )
                {
                    SQL = "INSERT INTO SearchToken_Attribute VALUES( " +
                        StringUtils::itos( PK_SearchToken ) + ", " +
                        StringUtils::itos( PK_Attribute ) + " )";
                    threaded_mysql_query( SQL, true );
                }
            }
        }
    }
    else
        g_pPlutoLogger->Write( LV_CRITICAL, "Cannot update tokens for %d", PK_Attribute );
}
int macounter=0;
void MediaAttributes::ScanDirectory( string Path )
{
if( macounter>50 )
    return;
#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;
    ptrFileList = _findfirst( ( Path+"*.*" ).c_str( ), &finddata );
    while( ptrFileList!=-1 )
#else

    DIR *dirp = opendir( Path.c_str( ) );
    struct dirent entry;
    struct dirent *direntp = &entry;
    while ( dirp != NULL && ( readdir_r( dirp, direntp, &direntp ) == 0 ) && direntp )
#endif
    {
#ifdef WIN32
        if( ( finddata.attrib==_A_SUBDIR ) && finddata.name[0]!='.' )
            ScanDirectory( Path + finddata.name + "\\" );
        string FullPath = Path + finddata.name;
        char *Name = finddata.name;
#else
        if( entry.d_type == DT_DIR && entry.d_name[0] != '.' )
            ScanDirectory( Path + entry.d_name + "/" );
        string FullPath = Path + entry.d_name;
        char *Name = entry.d_name;
#endif

        // We've got a file
        if( string( "discinfo" )==Name )
        {
    printf( "Foiund discinfo %d in directory: %s\n", macounter++, Path.c_str( ) );
            char buffer[6000];
            string::size_type start=0, stop=0;
            FILE *file=fopen( FullPath.c_str( ), "rb" );
            int bytes = ( int ) fread( buffer, 1, 6000, file );
            fclose( file );
            if( bytes==0 || bytes>5999 )
            {
                g_pPlutoLogger->Write( LV_CRITICAL, "Error with file in %s bytes: %d", Path.c_str( ), bytes );
                exit( 1 );
            }
            buffer[bytes]=0;
            string sBuffer( buffer ), Token;

            // Asin
            start=sBuffer.find( "=" );
            stop=sBuffer.find( "DiscID=" );
            string Asin=sBuffer.substr( start+1, stop-start-1 );

            // DiscID
            start=sBuffer.find( "=", stop );
            stop=sBuffer.find( "Category=", start );
            string DiscID =sBuffer.substr( start+1, stop-start-1 );

            // Category
            start=sBuffer.find( "=", stop );
            stop=sBuffer.find( "Genre=", start );
            string Category=sBuffer.substr( start+1, stop-start-1 );

            // Genre
            start=sBuffer.find( "=", stop );
            stop=sBuffer.find( "Artist=", start );
            string Genre=sBuffer.substr( start+1, stop-start-1 );

            // Artist
            start=sBuffer.find( "=", stop );
            stop=sBuffer.find( "Title=", start );
            string Artist=sBuffer.substr( start+1, stop-start-1 );

            // Title
            start=sBuffer.find( "=", stop );
            stop=sBuffer.find( "Track1_Name=", start );
            string Title=sBuffer.substr( start+1, stop-start-1 );

            int iAlbum=0, iArtist=0, iAsin=0, iDisc=0, iCategory=0, iGenre=0;

            for( int i=1;i<999;++i )
            {
                start = sBuffer.find( "Track" + StringUtils::itos( i ) + "_Name=" );
                start = sBuffer.find( "=", start );
                stop = sBuffer.find( "Track" + StringUtils::itos( i ) + "_Artist=", start );
                string TrackName = sBuffer.substr( start+1, stop-start-1 );
                start = sBuffer.find( "=", stop );
                stop = sBuffer.find( "Track" + StringUtils::itos( i+1 ) + "_Name=", start );
                string TrackArtist = ( stop==string::npos ) ? sBuffer.substr( start+1 ) : sBuffer.substr( start+1, stop-start-1 );

                int PK_File = CreatedMedia( 1, Path + TrackName + ".flac", NULL, NULL );
                if( PK_File )
                {
                    AddAttribute( PK_File, 0, 2, TrackArtist, "" );
                    iArtist=AddAttribute( PK_File, iArtist, 2, Artist, "" );
                    iAlbum=AddAttribute( PK_File, iAlbum, 3, Title, "" );
                    AddAttribute( PK_File, 0, 4, TrackName, "" );
                    AddAttribute( PK_File, 0, 5, StringUtils::itos( i ), "" );
                    iAsin=AddAttribute( PK_File, iAsin, 6, Asin, "" );
                    iDisc=AddAttribute( PK_File, iDisc, 7, DiscID, "" );
                    iCategory=AddAttribute( PK_File, iCategory, 8, Category, "" );
                    iGenre=AddAttribute( PK_File, iGenre, 8, Genre, "" );
                    AddPicture( PK_File, iAlbum, Path + "cover.jpg" );
                }
                else
                    g_pPlutoLogger->Write( LV_CRITICAL, "xxxx" );


                if( stop==string::npos )
                    break;
            }
        }
#ifdef WIN32
        if( _findnext( ptrFileList, &finddata )<0 )
            break;
#endif

    }
#ifdef WIN32
    _findclose( ptrFileList );
#else
    closedir( dirp );
#endif

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
        mi->FirstName = StringUtils::Tokenize( Input, "|", pos );
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
        Result += mi->FirstName + "|";
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
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
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
        "JOIN Type_AttributeType ON "\
        "Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND FK_Attribute=" + StringUtils::itos( PK_Attribute );

    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
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
    if( ( result.r=mysql_query_result( SQL ) ) )
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
#ifdef WIN32
    PlutoSqlResult result;
    MYSQL_ROW row;
    string::size_type s;
    while( ( s=File.find( "Z:/" ) )!=string::npos )
        File.replace( s, 3, "/home/public/data/" );
    while( ( s=File.find( "Z:\\" ) )!=string::npos )
        File.replace( s, 3, "/home/public/data/" );
    while( ( s=File.find( '\\' ) )!=string::npos )
        File.replace( s, 1, "/" );
    string SQL = "SELECT PK_File FROM File WHERE Path='" + StringUtils::SQLEscape( File ) + "'";
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
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
                    cout << "Query: " << SQL << endl;
                    threaded_mysql_query( SQL );
                }
            }
        }
        return ID;
    }
    else
        return 0;
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
    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
    {
        *PK_Picture = atoi( row[0] );
        return row[1];
    }

    // Let's to see if any of the attributes associated with this file have pictures
    SQL = "SELECT FK_Picture, Extension from File_Attribute "\
        "JOIN Picture_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "\
        "JOIN Picture ON FK_Picture=PK_Picture "\
        "WHERE FK_File=" + StringUtils::itos( PK_File );
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
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
        "WHERE FK_Attribute=" + StringUtils::itos( PK_Attribute );
    PlutoSqlResult result;
    MYSQL_ROW row;
    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
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
        "WHERE Source.FK_Attribute=" + StringUtils::itos( PK_Attribute );

    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
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
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND PK_File=" + StringUtils::itos( PK_File );

    PlutoSqlResult result;
    MYSQL_ROW row;

    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}
int MediaAttributes::GetAttributeFromFilePath( string File )
{
    string SQL = "select PK_Attribute FROM File_Attribute "\
        "JOIN Attribute ON FK_Attribute=PK_Attribute "\
        "JOIN File ON FK_File=PK_File "\
        "JOIN Type_AttributeType ON Type_AttributeType.FK_Type=File.FK_Type "\
        "AND Type_AttributeType.FK_AttributeType=Attribute.FK_AttributeType "\
        "WHERE Identifier=1 AND Path='" + StringUtils::SQLEscape( File ) + "'";

    PlutoSqlResult result;
    MYSQL_ROW row;

    if( ( result.r=mysql_query_result( SQL ) ) && ( row=mysql_fetch_row( result.r ) ) )
        return atoi( row[0] );
    return 0;
}

bool MediaAttributes::isAttributeSpecification(string sFilename)
{
    // first 2 chars are # and a file (should A or a for Attributte). after that we need a number also
    return sFilename.length() >= 3 && sFilename[0] == '#' && (sFilename[1] == 'A' || sFilename[1] == 'a');
}

bool MediaAttributes::isFileSpecification(string sFilename)
{
    return sFilename.length() >= 3 && sFilename[0] == '#' && (sFilename[1] == 'F' || sFilename[1] =='f' );
}

bool MediaAttributes::GetFilesFromAttrBySpec(string sAttrSpecification, vector<string> &vectFilesListToUpdate)
{
    Row_Attribute *pRow_Attribute;
    int iAttrId;

    if ( (iAttrId = atoi(sAttrSpecification.substr(2).c_str())) == 0 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "MediaAttributtes::GetFilesFromAttrBySpec had invalid input ( the attr id was resolved to the 0 value ). Original input: %s", sAttrSpecification.c_str());
        return false;
    }

    pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->GetRow( iAttrId );

    if( !pRow_Attribute )
    {
        g_pPlutoLogger->Write(LV_CRITICAL,"No attributte with id (%d) available in database (original attributte spec: %s)", sAttrSpecification.c_str());
        return false;
    }

    vector<Row_File_Attribute *> vectRow_File_Attribute;
    pRow_Attribute->File_Attribute_FK_Attribute_getrows(&vectRow_File_Attribute);
    for(size_t s=0;s<vectRow_File_Attribute.size();++s)
    {
        Row_File_Attribute *pRow_File_Attribute = vectRow_File_Attribute[s];
        if ( pRow_File_Attribute )
        {
            Row_File *pFile = pRow_File_Attribute->FK_File_getrow();
            if ( pFile && pFile->Missing_get() == 0 )
                vectFilesListToUpdate.push_back(string("#F" + StringUtils::itos(pFile->PK_File_get())));
        }
    }

    return true;
}

int MediaAttributes::ConvertFileSpecToFileID(string sFileSpecification)
{
    int iFileID;

    if ( ! isFileSpecification(sFileSpecification) )
        return 0;

    if ( (iFileID = atoi(sFileSpecification.substr(2).c_str())) == 0 )
    {
        g_pPlutoLogger->Write(LV_WARNING, "MediaAttributtes::ConvertFileSpecToFilePath had invalid input ( the file id was resolved to the 0 value ). Original input: %s", sFileSpecification.c_str());
        return 0;
    }

    return iFileID;
}

string MediaAttributes::ConvertFileSpecToFilePath(string sFileSpecification)
{
    Row_File *pRow_File;
    int iFileID;


    if ( (iFileID = ConvertFileSpecToFileID(sFileSpecification)) == 0 )
        return sFileSpecification;

    pRow_File = m_pDatabase_pluto_media->File_get()->GetRow( iFileID );

    if ( !pRow_File )
    {
        g_pPlutoLogger->Write(LV_CRITICAL, "No file with id (%d) was found in the media database");
        return sFileSpecification;
    }

    return pRow_File->Path_get() + "/" + pRow_File->Filename_get();
}

MediaAttributesReturnCode MediaAttributes::SavePlaylist(deque<string> &playlistContents, int iPK_Users, int &iPlaylistID, string sPlaylistName )
{
    Table_Playlist *pPlaylistTable;
    Row_Playlist *pRowPlaylist;

    Table_PlaylistEntry *pEntriesTable;
    Row_PlaylistEntry *pEntry;
    string sFileEntry;
    int fileID;

    pPlaylistTable = m_pDatabase_pluto_media->Playlist_get();
    pEntriesTable = m_pDatabase_pluto_media->PlaylistEntry_get();

    if ( iPlaylistID != 0 )
        pRowPlaylist = pPlaylistTable->GetRow(iPlaylistID);
    else
        pRowPlaylist = pPlaylistTable->AddRow();

    pRowPlaylist->Name_set(sPlaylistName);
    pRowPlaylist->EK_User_set(iPK_Users);

    if ( pPlaylistTable->Commit() == false )
    {
        if ( iPlaylistID == 0 )
        {
            pRowPlaylist->Delete();
            pPlaylistTable->Commit();
        }
        else
        {
            pRowPlaylist->Reload();
        }
        return errPlaylistNameAlreadyExists;
    }

    // clear the previous playlist in order to be able to properly save it.
    vector<Row_PlaylistEntry*> previousList;
    pEntriesTable->GetRows( string("") + PLAYLISTENTRY_FK_PLAYLIST_FIELD + " = " + StringUtils::itos(iPlaylistID), &previousList);

    for ( vector<Row_PlaylistEntry*>::iterator  itOldList = previousList.begin(); itOldList != previousList.end(); itOldList++ )
        (*itOldList)->Delete();

    iPlaylistID = pRowPlaylist->PK_Playlist_get();

    deque<string>::iterator itEntries = playlistContents.begin();
    while ( itEntries != playlistContents.end() )
    {
        sFileEntry = *itEntries;

        pEntry = pEntriesTable->AddRow();
        pEntry->FK_Playlist_set(iPlaylistID);

        if ( (fileID = ConvertFileSpecToFileID(sFileEntry) ) == 0 )
        { // this is not a file spec
            pEntry->FK_File_setNull(true);
            pEntry->Path_set(FileUtils::BasePath(sFileEntry));
            pEntry->Filename_set(FileUtils::FilenameWithoutPath(sFileEntry, true));
        }
        else
        {
            pEntry->FK_File_set(fileID);
            pEntry->Path_setNull(true);
            pEntry->Filename_setNull(true);
        }

        itEntries++;
    }

    if ( pEntriesTable->Commit() == false )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Could not save the playlist");
        return errPlaylistCommitFailed;
    }

    return errNoError;
}

MediaAttributesReturnCode MediaAttributes::LoadPlaylist(int iPlaylistToLoadID, int &iPlaylistID, string &sPlaylistName, deque<string> &dequePlaylist)
{
    Table_Playlist *pPlaylistTable;
    Row_Playlist *pRowPlaylist;

    Table_PlaylistEntry *pEntriesTable;
    Row_PlaylistEntry *pEntry;

    string sEntry;

    pPlaylistTable  = m_pDatabase_pluto_media->Playlist_get();
    pEntriesTable   = m_pDatabase_pluto_media->PlaylistEntry_get();

    pRowPlaylist = pPlaylistTable->GetRow(iPlaylistToLoadID);

    if  ( pRowPlaylist == NULL )
        return errNoSuchPlaylist;

    vector<Row_PlaylistEntry*> listEntries;

    // use a "WHERE FK_Playlist = <playlist_ID> ORDER BY PK_PlaylistEntry" clause on the select
    if ( pEntriesTable->GetRows(string(PLAYLISTENTRY_FK_PLAYLIST_FIELD) + " = " + StringUtils::itos(iPlaylistToLoadID) + " ORDER BY " + PLAYLISTENTRY_PK_PLAYLISTENTRY_FIELD, &listEntries) == false )
        return errCantLoadEntries;

    sPlaylistName = pRowPlaylist->Name_get();
    iPlaylistID   = pRowPlaylist->PK_Playlist_get();
    dequePlaylist.clear();

    for ( vector<Row_PlaylistEntry*>::iterator itEntries = listEntries.begin(); itEntries != listEntries.end(); itEntries++ )
    {
        pEntry = *itEntries;

        if ( pEntry->FK_File_isNull() )
            sEntry = pEntry->Path_get() + "/" + pEntry->Filename_get();
        else
            sEntry = string("#F ") + StringUtils::itos(pEntry->FK_File_get());

        dequePlaylist.push_back(sEntry);
    }

    return errNoError;
}

void MediaAttributes::MarkAsMissing(int iKey, string fileName)
{
    g_pPlutoLogger->Write(LV_STATUS, "Marking %s as missing", fileName.c_str());

    string SQL = "UPDATE File SET Missing=1 WHERE PK_File=" + StringUtils::itos( iKey );

    threaded_mysql_query_withID( SQL );
}

