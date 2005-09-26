#ifdef WIN32
	#include <winsock.h>
#endif
#include <iostream>
#include <mysql.h>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>

using namespace std;

#include "Database_pluto_media.h"


#include "DCEConfig.h"


namespace DCE
{
	Logger *g_pPlutoLogger; //dummy
}
using namespace DCE;
Database_pluto_media::Database_pluto_media()
{
tblAttribute=NULL;
tblAttributeType=NULL;
tblBookmark=NULL;
tblDisc=NULL;
tblDisc_Attribute=NULL;
tblFile=NULL;
tblFile_Attribute=NULL;
tblMediaProvider=NULL;
tblMediaType_AttributeType=NULL;
tblPicture=NULL;
tblPicture_Attribute=NULL;
tblPicture_Disc=NULL;
tblPicture_File=NULL;
tblPlaylist=NULL;
tblPlaylistEntry=NULL;
tblSearchToken=NULL;
tblSearchToken_Attribute=NULL;
tblpsc_media_batdet=NULL;
tblpsc_media_bathdr=NULL;
tblpsc_media_batuser=NULL;
tblpsc_media_repset=NULL;
tblpsc_media_schema=NULL;
tblpsc_media_tables=NULL;
}

Database_pluto_media::~Database_pluto_media()
{
	DeleteAllTables();
}

bool Database_pluto_media::Commit(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow)
{
bool bResult=true;
if( tblAttribute!=NULL )
	if( !Commit_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblAttributeType!=NULL )
	if( !Commit_AttributeType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblBookmark!=NULL )
	if( !Commit_Bookmark(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDisc!=NULL )
	if( !Commit_Disc(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblDisc_Attribute!=NULL )
	if( !Commit_Disc_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFile!=NULL )
	if( !Commit_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblFile_Attribute!=NULL )
	if( !Commit_File_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaProvider!=NULL )
	if( !Commit_MediaProvider(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblMediaType_AttributeType!=NULL )
	if( !Commit_MediaType_AttributeType(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture!=NULL )
	if( !Commit_Picture(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_Attribute!=NULL )
	if( !Commit_Picture_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_Disc!=NULL )
	if( !Commit_Picture_Disc(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPicture_File!=NULL )
	if( !Commit_Picture_File(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPlaylist!=NULL )
	if( !Commit_Playlist(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblPlaylistEntry!=NULL )
	if( !Commit_PlaylistEntry(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSearchToken!=NULL )
	if( !Commit_SearchToken(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblSearchToken_Attribute!=NULL )
	if( !Commit_SearchToken_Attribute(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_batdet!=NULL )
	if( !Commit_psc_media_batdet(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_bathdr!=NULL )
	if( !Commit_psc_media_bathdr(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_batuser!=NULL )
	if( !Commit_psc_media_batuser(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_repset!=NULL )
	if( !Commit_psc_media_repset(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_schema!=NULL )
	if( !Commit_psc_media_schema(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
if( tblpsc_media_tables!=NULL )
	if( !Commit_psc_media_tables(bDeleteFailedModifiedRow,bDeleteFailedInsertRow) )
		bResult=false;
return bResult;

}

void Database_pluto_media::DeleteAllTables()
{
DeleteTable_Attribute();
DeleteTable_AttributeType();
DeleteTable_Bookmark();
DeleteTable_Disc();
DeleteTable_Disc_Attribute();
DeleteTable_File();
DeleteTable_File_Attribute();
DeleteTable_MediaProvider();
DeleteTable_MediaType_AttributeType();
DeleteTable_Picture();
DeleteTable_Picture_Attribute();
DeleteTable_Picture_Disc();
DeleteTable_Picture_File();
DeleteTable_Playlist();
DeleteTable_PlaylistEntry();
DeleteTable_SearchToken();
DeleteTable_SearchToken_Attribute();
DeleteTable_psc_media_batdet();
DeleteTable_psc_media_bathdr();
DeleteTable_psc_media_batuser();
DeleteTable_psc_media_repset();
DeleteTable_psc_media_schema();
DeleteTable_psc_media_tables();
}

bool Database_pluto_media::Connect(string host, string user, string pass, string sDBName, int port)
{
return MySQLConnect(host, user, pass, sDBName, port);
}

bool Database_pluto_media::Connect(class DCEConfig *pDCEConfig)
{
	return Connect(pDCEConfig->m_sDBHost,pDCEConfig->m_sDBUser,pDCEConfig->m_sDBPassword,pDCEConfig->m_sDBName,pDCEConfig->m_iDBPort);
}
