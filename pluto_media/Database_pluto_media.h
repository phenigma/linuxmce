#ifndef __Database_pluto_media_H_
#define __Database_pluto_media_H_
#include <mysql.h>
#ifdef SQL2CPP_DLLEXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif
class DLL_EXPORT Database_pluto_media
{
public:
MYSQL *db_handle;
Database_pluto_media();
~Database_pluto_media();
void DeleteAllTables();
private:
class Table_Attribute* tblAttribute;
class Table_AttributeType* tblAttributeType;
class Table_File* tblFile;
class Table_File_Attribute* tblFile_Attribute;
class Table_Picture* tblPicture;
class Table_Picture_Attribute* tblPicture_Attribute;
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
class Table_psc_media_repset* tblpsc_media_repset;
class Table_psc_media_schema* tblpsc_media_schema;
class Table_psc_media_tables* tblpsc_media_tables;
public:
class Table_Attribute* Attribute_get() { return tblAttribute; }
class Table_AttributeType* AttributeType_get() { return tblAttributeType; }
class Table_File* File_get() { return tblFile; }
class Table_File_Attribute* File_Attribute_get() { return tblFile_Attribute; }
class Table_Picture* Picture_get() { return tblPicture; }
class Table_Picture_Attribute* Picture_Attribute_get() { return tblPicture_Attribute; }
class Table_Picture_File* Picture_File_get() { return tblPicture_File; }
class Table_Playlist* Playlist_get() { return tblPlaylist; }
class Table_PlaylistEntry* PlaylistEntry_get() { return tblPlaylistEntry; }
class Table_SearchToken* SearchToken_get() { return tblSearchToken; }
class Table_SearchToken_Attribute* SearchToken_Attribute_get() { return tblSearchToken_Attribute; }
class Table_Type* Type_get() { return tblType; }
class Table_Type_AttributeType* Type_AttributeType_get() { return tblType_AttributeType; }
class Table_Type_Extension* Type_Extension_get() { return tblType_Extension; }
class Table_psc_media_batdet* psc_media_batdet_get() { return tblpsc_media_batdet; }
class Table_psc_media_bathdr* psc_media_bathdr_get() { return tblpsc_media_bathdr; }
class Table_psc_media_repset* psc_media_repset_get() { return tblpsc_media_repset; }
class Table_psc_media_schema* psc_media_schema_get() { return tblpsc_media_schema; }
class Table_psc_media_tables* psc_media_tables_get() { return tblpsc_media_tables; }
bool Connect(string host, string user, string pass, string sDBName, int port);
bool Connect(class DCEConfig *pDCEConfig);
void Disconnect();
private:
void CreateTable_Attribute();
void CreateTable_AttributeType();
void CreateTable_File();
void CreateTable_File_Attribute();
void CreateTable_Picture();
void CreateTable_Picture_Attribute();
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
void CreateTable_psc_media_repset();
void CreateTable_psc_media_schema();
void CreateTable_psc_media_tables();
void DeleteTable_Attribute();
void DeleteTable_AttributeType();
void DeleteTable_File();
void DeleteTable_File_Attribute();
void DeleteTable_Picture();
void DeleteTable_Picture_Attribute();
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
void DeleteTable_psc_media_repset();
void DeleteTable_psc_media_schema();
void DeleteTable_psc_media_tables();
};
#endif
