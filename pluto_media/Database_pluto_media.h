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
class Table_SearchToken* tblSearchToken;
class Table_SearchToken_Attribute* tblSearchToken_Attribute;
class Table_Type* tblType;
class Table_Type_AttributeType* tblType_AttributeType;
class Table_Type_Extension* tblType_Extension;
public:
class Table_Attribute* Attribute_get() { return tblAttribute; }
class Table_AttributeType* AttributeType_get() { return tblAttributeType; }
class Table_File* File_get() { return tblFile; }
class Table_File_Attribute* File_Attribute_get() { return tblFile_Attribute; }
class Table_Picture* Picture_get() { return tblPicture; }
class Table_Picture_Attribute* Picture_Attribute_get() { return tblPicture_Attribute; }
class Table_Picture_File* Picture_File_get() { return tblPicture_File; }
class Table_SearchToken* SearchToken_get() { return tblSearchToken; }
class Table_SearchToken_Attribute* SearchToken_Attribute_get() { return tblSearchToken_Attribute; }
class Table_Type* Type_get() { return tblType; }
class Table_Type_AttributeType* Type_AttributeType_get() { return tblType_AttributeType; }
class Table_Type_Extension* Type_Extension_get() { return tblType_Extension; }
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
void CreateTable_SearchToken();
void CreateTable_SearchToken_Attribute();
void CreateTable_Type();
void CreateTable_Type_AttributeType();
void CreateTable_Type_Extension();
void DeleteTable_Attribute();
void DeleteTable_AttributeType();
void DeleteTable_File();
void DeleteTable_File_Attribute();
void DeleteTable_Picture();
void DeleteTable_Picture_Attribute();
void DeleteTable_Picture_File();
void DeleteTable_SearchToken();
void DeleteTable_SearchToken_Attribute();
void DeleteTable_Type();
void DeleteTable_Type_AttributeType();
void DeleteTable_Type_Extension();
};
#endif
