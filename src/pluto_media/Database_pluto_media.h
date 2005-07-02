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
class Table_AttributeType* tblAttributeType;
class Table_Bookmark* tblBookmark;
class Table_Disc* tblDisc;
class Table_Disc_Attribute* tblDisc_Attribute;
class Table_File* tblFile;
class Table_File_Attribute* tblFile_Attribute;
class Table_Picture* tblPicture;
class Table_Picture_Attribute* tblPicture_Attribute;
class Table_Picture_Disc* tblPicture_Disc;
class Table_Picture_File* tblPicture_File;
class Table_Playlist* tblPlaylist;
class Table_PlaylistEntry* tblPlaylistEntry;
class Table_SearchToken* tblSearchToken;
class Table_SearchToken_Attribute* tblSearchToken_Attribute;
class Table_Type* tblType;
class Table_Type_AttributeType* tblType_AttributeType;
class Table_Type_Extension* tblType_Extension;
class Table_psc_media_batdet* tblpsc_media_batdet;
class Table_psc_media_bathdr* tblpsc_media_bathdr;
class Table_psc_media_batuser* tblpsc_media_batuser;
class Table_psc_media_repset* tblpsc_media_repset;
class Table_psc_media_schema* tblpsc_media_schema;
class Table_psc_media_tables* tblpsc_media_tables;
public:
class Table_Attribute* Attribute_get() { if( !tblAttribute ) CreateTable_Attribute(); return tblAttribute; }
class Table_AttributeType* AttributeType_get() { if( !tblAttributeType ) CreateTable_AttributeType(); return tblAttributeType; }
class Table_Bookmark* Bookmark_get() { if( !tblBookmark ) CreateTable_Bookmark(); return tblBookmark; }
class Table_Disc* Disc_get() { if( !tblDisc ) CreateTable_Disc(); return tblDisc; }
class Table_Disc_Attribute* Disc_Attribute_get() { if( !tblDisc_Attribute ) CreateTable_Disc_Attribute(); return tblDisc_Attribute; }
class Table_File* File_get() { if( !tblFile ) CreateTable_File(); return tblFile; }
class Table_File_Attribute* File_Attribute_get() { if( !tblFile_Attribute ) CreateTable_File_Attribute(); return tblFile_Attribute; }
class Table_Picture* Picture_get() { if( !tblPicture ) CreateTable_Picture(); return tblPicture; }
class Table_Picture_Attribute* Picture_Attribute_get() { if( !tblPicture_Attribute ) CreateTable_Picture_Attribute(); return tblPicture_Attribute; }
class Table_Picture_Disc* Picture_Disc_get() { if( !tblPicture_Disc ) CreateTable_Picture_Disc(); return tblPicture_Disc; }
class Table_Picture_File* Picture_File_get() { if( !tblPicture_File ) CreateTable_Picture_File(); return tblPicture_File; }
class Table_Playlist* Playlist_get() { if( !tblPlaylist ) CreateTable_Playlist(); return tblPlaylist; }
class Table_PlaylistEntry* PlaylistEntry_get() { if( !tblPlaylistEntry ) CreateTable_PlaylistEntry(); return tblPlaylistEntry; }
class Table_SearchToken* SearchToken_get() { if( !tblSearchToken ) CreateTable_SearchToken(); return tblSearchToken; }
class Table_SearchToken_Attribute* SearchToken_Attribute_get() { if( !tblSearchToken_Attribute ) CreateTable_SearchToken_Attribute(); return tblSearchToken_Attribute; }
class Table_Type* Type_get() { if( !tblType ) CreateTable_Type(); return tblType; }
class Table_Type_AttributeType* Type_AttributeType_get() { if( !tblType_AttributeType ) CreateTable_Type_AttributeType(); return tblType_AttributeType; }
class Table_Type_Extension* Type_Extension_get() { if( !tblType_Extension ) CreateTable_Type_Extension(); return tblType_Extension; }
class Table_psc_media_batdet* psc_media_batdet_get() { if( !tblpsc_media_batdet ) CreateTable_psc_media_batdet(); return tblpsc_media_batdet; }
class Table_psc_media_bathdr* psc_media_bathdr_get() { if( !tblpsc_media_bathdr ) CreateTable_psc_media_bathdr(); return tblpsc_media_bathdr; }
class Table_psc_media_batuser* psc_media_batuser_get() { if( !tblpsc_media_batuser ) CreateTable_psc_media_batuser(); return tblpsc_media_batuser; }
class Table_psc_media_repset* psc_media_repset_get() { if( !tblpsc_media_repset ) CreateTable_psc_media_repset(); return tblpsc_media_repset; }
class Table_psc_media_schema* psc_media_schema_get() { if( !tblpsc_media_schema ) CreateTable_psc_media_schema(); return tblpsc_media_schema; }
class Table_psc_media_tables* psc_media_tables_get() { if( !tblpsc_media_tables ) CreateTable_psc_media_tables(); return tblpsc_media_tables; }
string m_sLastMySqlError;
bool Connect(string host, string user, string pass, string sDBName, int port);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
private:
void CreateTable_Attribute();
void CreateTable_AttributeType();
void CreateTable_Bookmark();
void CreateTable_Disc();
void CreateTable_Disc_Attribute();
void CreateTable_File();
void CreateTable_File_Attribute();
void CreateTable_Picture();
void CreateTable_Picture_Attribute();
void CreateTable_Picture_Disc();
void CreateTable_Picture_File();
void CreateTable_Playlist();
void CreateTable_PlaylistEntry();
void CreateTable_SearchToken();
void CreateTable_SearchToken_Attribute();
void CreateTable_Type();
void CreateTable_Type_AttributeType();
void CreateTable_Type_Extension();
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
void DeleteTable_File_Attribute();
void DeleteTable_Picture();
void DeleteTable_Picture_Attribute();
void DeleteTable_Picture_Disc();
void DeleteTable_Picture_File();
void DeleteTable_Playlist();
void DeleteTable_PlaylistEntry();
void DeleteTable_SearchToken();
void DeleteTable_SearchToken_Attribute();
void DeleteTable_Type();
void DeleteTable_Type_AttributeType();
void DeleteTable_Type_Extension();
void DeleteTable_psc_media_batdet();
void DeleteTable_psc_media_bathdr();
void DeleteTable_psc_media_batuser();
void DeleteTable_psc_media_repset();
void DeleteTable_psc_media_schema();
void DeleteTable_psc_media_tables();
};
#endif
