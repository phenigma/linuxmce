#ifndef __Database_pluto_media_H_
#define __Database_pluto_media_H_
#include <mysql.h>
#include "PlutoUtils/MySQLHelper.h"
#ifdef WIN32
#ifdef EXPORT_DLL
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllexport)
	#endif
#else
	#ifndef DECLSPECIFIER
		#define DECLSPECIFIER __declspec(dllimport)
	#endif
#endif
#else
#define DECLSPECIFIER
#endif
class DECLSPECIFIER MySqlHelper;
class DECLSPECIFIER SerializeClass;

class DECLSPECIFIER Database_pluto_media: public MySqlHelper
{
public:
Database_pluto_media();
~Database_pluto_media();
void DeleteAllTables();
private:
class Table_Attribute* tblAttribute;
bool Commit_Attribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_AttributeType* tblAttributeType;
bool Commit_AttributeType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Bookmark* tblBookmark;
bool Commit_Bookmark(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Disc* tblDisc;
bool Commit_Disc(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Disc_Attribute* tblDisc_Attribute;
bool Commit_Disc_Attribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_File* tblFile;
bool Commit_File(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_FileFormat* tblFileFormat;
bool Commit_FileFormat(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_File_Attribute* tblFile_Attribute;
bool Commit_File_Attribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_File_Users* tblFile_Users;
bool Commit_File_Users(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaProvider* tblMediaProvider;
bool Commit_MediaProvider(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaSource* tblMediaSource;
bool Commit_MediaSource(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaSource_FileFormat* tblMediaSource_FileFormat;
bool Commit_MediaSource_FileFormat(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaSubType* tblMediaSubType;
bool Commit_MediaSubType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaType_AttributeType* tblMediaType_AttributeType;
bool Commit_MediaType_AttributeType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaType_FileFormat* tblMediaType_FileFormat;
bool Commit_MediaType_FileFormat(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_MediaType_MediaSubType* tblMediaType_MediaSubType;
bool Commit_MediaType_MediaSubType(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Picture* tblPicture;
bool Commit_Picture(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Picture_Attribute* tblPicture_Attribute;
bool Commit_Picture_Attribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Picture_Disc* tblPicture_Disc;
bool Commit_Picture_Disc(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Picture_File* tblPicture_File;
bool Commit_Picture_File(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_Playlist* tblPlaylist;
bool Commit_Playlist(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_PlaylistEntry* tblPlaylistEntry;
bool Commit_PlaylistEntry(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_SearchToken* tblSearchToken;
bool Commit_SearchToken(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_SearchToken_Attribute* tblSearchToken_Attribute;
bool Commit_SearchToken_Attribute(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_media_batdet* tblpsc_media_batdet;
bool Commit_psc_media_batdet(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_media_bathdr* tblpsc_media_bathdr;
bool Commit_psc_media_bathdr(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_media_batuser* tblpsc_media_batuser;
bool Commit_psc_media_batuser(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_media_repset* tblpsc_media_repset;
bool Commit_psc_media_repset(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_media_schema* tblpsc_media_schema;
bool Commit_psc_media_schema(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
class Table_psc_media_tables* tblpsc_media_tables;
bool Commit_psc_media_tables(bool bDeleteFailedModifiedRow,bool bDeleteFailedInsertRow);
public:
class Table_Attribute* Attribute_get() { if( !tblAttribute ) CreateTable_Attribute(); return tblAttribute; }
class Table_AttributeType* AttributeType_get() { if( !tblAttributeType ) CreateTable_AttributeType(); return tblAttributeType; }
class Table_Bookmark* Bookmark_get() { if( !tblBookmark ) CreateTable_Bookmark(); return tblBookmark; }
class Table_Disc* Disc_get() { if( !tblDisc ) CreateTable_Disc(); return tblDisc; }
class Table_Disc_Attribute* Disc_Attribute_get() { if( !tblDisc_Attribute ) CreateTable_Disc_Attribute(); return tblDisc_Attribute; }
class Table_File* File_get() { if( !tblFile ) CreateTable_File(); return tblFile; }
class Table_FileFormat* FileFormat_get() { if( !tblFileFormat ) CreateTable_FileFormat(); return tblFileFormat; }
class Table_File_Attribute* File_Attribute_get() { if( !tblFile_Attribute ) CreateTable_File_Attribute(); return tblFile_Attribute; }
class Table_File_Users* File_Users_get() { if( !tblFile_Users ) CreateTable_File_Users(); return tblFile_Users; }
class Table_MediaProvider* MediaProvider_get() { if( !tblMediaProvider ) CreateTable_MediaProvider(); return tblMediaProvider; }
class Table_MediaSource* MediaSource_get() { if( !tblMediaSource ) CreateTable_MediaSource(); return tblMediaSource; }
class Table_MediaSource_FileFormat* MediaSource_FileFormat_get() { if( !tblMediaSource_FileFormat ) CreateTable_MediaSource_FileFormat(); return tblMediaSource_FileFormat; }
class Table_MediaSubType* MediaSubType_get() { if( !tblMediaSubType ) CreateTable_MediaSubType(); return tblMediaSubType; }
class Table_MediaType_AttributeType* MediaType_AttributeType_get() { if( !tblMediaType_AttributeType ) CreateTable_MediaType_AttributeType(); return tblMediaType_AttributeType; }
class Table_MediaType_FileFormat* MediaType_FileFormat_get() { if( !tblMediaType_FileFormat ) CreateTable_MediaType_FileFormat(); return tblMediaType_FileFormat; }
class Table_MediaType_MediaSubType* MediaType_MediaSubType_get() { if( !tblMediaType_MediaSubType ) CreateTable_MediaType_MediaSubType(); return tblMediaType_MediaSubType; }
class Table_Picture* Picture_get() { if( !tblPicture ) CreateTable_Picture(); return tblPicture; }
class Table_Picture_Attribute* Picture_Attribute_get() { if( !tblPicture_Attribute ) CreateTable_Picture_Attribute(); return tblPicture_Attribute; }
class Table_Picture_Disc* Picture_Disc_get() { if( !tblPicture_Disc ) CreateTable_Picture_Disc(); return tblPicture_Disc; }
class Table_Picture_File* Picture_File_get() { if( !tblPicture_File ) CreateTable_Picture_File(); return tblPicture_File; }
class Table_Playlist* Playlist_get() { if( !tblPlaylist ) CreateTable_Playlist(); return tblPlaylist; }
class Table_PlaylistEntry* PlaylistEntry_get() { if( !tblPlaylistEntry ) CreateTable_PlaylistEntry(); return tblPlaylistEntry; }
class Table_SearchToken* SearchToken_get() { if( !tblSearchToken ) CreateTable_SearchToken(); return tblSearchToken; }
class Table_SearchToken_Attribute* SearchToken_Attribute_get() { if( !tblSearchToken_Attribute ) CreateTable_SearchToken_Attribute(); return tblSearchToken_Attribute; }
class Table_psc_media_batdet* psc_media_batdet_get() { if( !tblpsc_media_batdet ) CreateTable_psc_media_batdet(); return tblpsc_media_batdet; }
class Table_psc_media_bathdr* psc_media_bathdr_get() { if( !tblpsc_media_bathdr ) CreateTable_psc_media_bathdr(); return tblpsc_media_bathdr; }
class Table_psc_media_batuser* psc_media_batuser_get() { if( !tblpsc_media_batuser ) CreateTable_psc_media_batuser(); return tblpsc_media_batuser; }
class Table_psc_media_repset* psc_media_repset_get() { if( !tblpsc_media_repset ) CreateTable_psc_media_repset(); return tblpsc_media_repset; }
class Table_psc_media_schema* psc_media_schema_get() { if( !tblpsc_media_schema ) CreateTable_psc_media_schema(); return tblpsc_media_schema; }
class Table_psc_media_tables* psc_media_tables_get() { if( !tblpsc_media_tables ) CreateTable_psc_media_tables(); return tblpsc_media_tables; }
string m_sLastMySqlError;
bool Connect(string host, string user, string pass, string sDBName, int port=3306);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
bool Commit(bool bDeleteFailedModifiedRow=false,bool bDeleteFailedInsertRow=false);
private:
void CreateTable_Attribute();
void CreateTable_AttributeType();
void CreateTable_Bookmark();
void CreateTable_Disc();
void CreateTable_Disc_Attribute();
void CreateTable_File();
void CreateTable_FileFormat();
void CreateTable_File_Attribute();
void CreateTable_File_Users();
void CreateTable_MediaProvider();
void CreateTable_MediaSource();
void CreateTable_MediaSource_FileFormat();
void CreateTable_MediaSubType();
void CreateTable_MediaType_AttributeType();
void CreateTable_MediaType_FileFormat();
void CreateTable_MediaType_MediaSubType();
void CreateTable_Picture();
void CreateTable_Picture_Attribute();
void CreateTable_Picture_Disc();
void CreateTable_Picture_File();
void CreateTable_Playlist();
void CreateTable_PlaylistEntry();
void CreateTable_SearchToken();
void CreateTable_SearchToken_Attribute();
void CreateTable_psc_media_batdet();
void CreateTable_psc_media_bathdr();
void CreateTable_psc_media_batuser();
void CreateTable_psc_media_repset();
void CreateTable_psc_media_schema();
void CreateTable_psc_media_tables();
void DeleteTable_Attribute();
void DeleteTable_AttributeType();
void DeleteTable_Bookmark();
void DeleteTable_Disc();
void DeleteTable_Disc_Attribute();
void DeleteTable_File();
void DeleteTable_FileFormat();
void DeleteTable_File_Attribute();
void DeleteTable_File_Users();
void DeleteTable_MediaProvider();
void DeleteTable_MediaSource();
void DeleteTable_MediaSource_FileFormat();
void DeleteTable_MediaSubType();
void DeleteTable_MediaType_AttributeType();
void DeleteTable_MediaType_FileFormat();
void DeleteTable_MediaType_MediaSubType();
void DeleteTable_Picture();
void DeleteTable_Picture_Attribute();
void DeleteTable_Picture_Disc();
void DeleteTable_Picture_File();
void DeleteTable_Playlist();
void DeleteTable_PlaylistEntry();
void DeleteTable_SearchToken();
void DeleteTable_SearchToken_Attribute();
void DeleteTable_psc_media_batdet();
void DeleteTable_psc_media_bathdr();
void DeleteTable_psc_media_batuser();
void DeleteTable_psc_media_repset();
void DeleteTable_psc_media_schema();
void DeleteTable_psc_media_tables();
};
#endif
